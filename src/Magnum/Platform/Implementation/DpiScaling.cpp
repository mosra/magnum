#ifndef Magnum_Platform_Implementation_dpiScaling_hpp
#define Magnum_Platform_Implementation_dpiScaling_hpp
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "DpiScaling.h"

#include <Corrade/Utility/Arguments.h>

#include "Magnum/Magnum.h"

#ifdef _MAGNUM_PLATFORM_USE_X11
#include <cstring>
#include <dlfcn.h>
#include <X11/Xresource.h>
#include <Corrade/Containers/ScopeGuard.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>
#undef None
#endif

#ifdef CORRADE_TARGET_EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

#if defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN
#include <windows.h>
#ifdef __has_include
#if __has_include(<shellscalingapi.h>)
#include <shellscalingapi.h>
#endif
#endif
#include <Corrade/Utility/Assert.h>
#endif

namespace Magnum { namespace Platform { namespace Implementation {

Utility::Arguments windowScalingArguments() {
    Utility::Arguments args{"magnum"};
    args.addOption("dpi-scaling", "default")
        .setFromEnvironment("dpi-scaling")
        #ifdef CORRADE_TARGET_APPLE
        .setHelp("dpi-scaling", "\n      window DPI scaling", "default|framebuffer|<d>|\"<h> <v>\"")
        #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
        .setHelp("dpi-scaling", "\n      window DPI scaling", "default|virtual|physical|<d>|\"<h> <v>\"")
        #else
        .setHelp("dpi-scaling", "\n      window DPI scaling", "default|physical|<d>|\"<h> <v>\"")
        #endif
        ;
    return args;
}

#ifdef _MAGNUM_PLATFORM_USE_X11
Float x11DpiScaling() {
    /* If the end app links to X11, these symbols will be available in a global
       scope and we can use that to query the DPI. If not, then those symbols
       won't be and that's okay -- it may be using Wayland or something else. */
    void* xlib = dlopen(nullptr, RTLD_NOW|RTLD_GLOBAL);
    Containers::ScopeGuard closeXlib{xlib, dlclose};
    #ifdef __GNUC__ /* http://www.mr-edd.co.uk/blog/supressing_gcc_warnings */
    __extension__
    #endif
    auto xOpenDisplay = reinterpret_cast<Display*(*)(char*)>(dlsym(xlib, "XOpenDisplay"));
    #ifdef __GNUC__ /* http://www.mr-edd.co.uk/blog/supressing_gcc_warnings */
    __extension__
    #endif
    auto xCloseDisplay = reinterpret_cast<int(*)(Display*)>(dlsym(xlib, "XCloseDisplay"));
    #ifdef __GNUC__ /* http://www.mr-edd.co.uk/blog/supressing_gcc_warnings */
    __extension__
    #endif
    auto xResourceManagerString = reinterpret_cast<char*(*)(Display*)>(dlsym(xlib, "XResourceManagerString"));
    #ifdef __GNUC__ /* http://www.mr-edd.co.uk/blog/supressing_gcc_warnings */
    __extension__
    #endif
    auto xrmGetStringDatabase = reinterpret_cast<XrmDatabase(*)(const char*)>(dlsym(xlib, "XrmGetStringDatabase"));
    #ifdef __GNUC__ /* http://www.mr-edd.co.uk/blog/supressing_gcc_warnings */
    __extension__
    #endif
    auto xrmGetResource = reinterpret_cast<int(*)(XrmDatabase, const char*, const char*, char**, XrmValue*)>(dlsym(xlib, "XrmGetResource"));
    #ifdef __GNUC__ /* http://www.mr-edd.co.uk/blog/supressing_gcc_warnings */
    __extension__
    #endif
    auto xrmDestroyDatabase = reinterpret_cast<void(*)(XrmDatabase)>(dlsym(xlib, "XrmDestroyDatabase"));
    if(!xOpenDisplay || !xCloseDisplay || !xResourceManagerString || !xrmGetStringDatabase || !xrmGetResource || !xrmDestroyDatabase) {
        Warning{} << "Platform: can't load X11 symbols for getting virtual DPI scaling, falling back to physical DPI";
        return {};
    }

    Display* display = xOpenDisplay(nullptr);
    Containers::ScopeGuard closeDisplay{display, xCloseDisplay};

    const char* rms = xResourceManagerString(display);
    if(rms) {
        XrmDatabase db = xrmGetStringDatabase(rms);
        CORRADE_INTERNAL_ASSERT(db);
        Containers::ScopeGuard closeDb{db, xrmDestroyDatabase};

        XrmValue value;
        char* type{};
        if(xrmGetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value)) {
            if(type && strcmp(type, "String") == 0) {
                const float scaling = std::stof(value.addr)/96.0f;
                CORRADE_INTERNAL_ASSERT(scaling);
                return scaling;
            }
        }
    }

    Warning{} << "Platform: can't get Xft.dpi property for virtual DPI scaling, falling back to physical DPI";
    return {};
}
#endif

#ifdef CORRADE_TARGET_EMSCRIPTEN
Float emscriptenDpiScaling() {
    return Float(emscripten_get_device_pixel_ratio());
}
#endif

#if defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)
bool isWindowsAppDpiAware() {
    /** @todo use GetWindowDpiAwarenessContext() (since Windows 10)? I think
        it's not needed for a simple boolean return value. */

    #ifdef DPI_ENUMS_DECLARED
    /* The GetProcessDpiAwareness() function is available only since Windows
       8.1, so load it manually to avoid a link-time error when building for
       Windows 7. Also, the shellscalingapi.h include might not be available
       on older MinGW, so it's guarded by __has_include(). Here, if the
       DPI_ENUMS_DECLARED define is present, the header exists and has what we
       need. */
    HMODULE const shcore = GetModuleHandleA("Shcore.dll");
    if(shcore) {
        /* GCC 8 adds -Wcast-function-type, enabled by default with -Wextra,
           which causes this line to emit a warning on MinGW. We know what
           we're doing, so suppress that. */
        #if defined(CORRADE_TARGET_GCC) && __GNUC__ >= 8
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wcast-function-type"
        #endif
        auto* const getProcessDpiAwareness = reinterpret_cast<HRESULT(WINAPI *)(HANDLE, PROCESS_DPI_AWARENESS*)>(GetProcAddress(shcore, "GetProcessDpiAwareness"));
        #if defined(CORRADE_TARGET_GCC) && __GNUC__ >= 8
        #pragma GCC diagnostic pop
        #endif
        PROCESS_DPI_AWARENESS result{};
        return getProcessDpiAwareness && getProcessDpiAwareness(nullptr, &result) == S_OK && result != PROCESS_DPI_UNAWARE;
    }
    #endif

    /* IsProcessDPIAware() is available since Windows Vista. At this point we
       can require it (XP support? haha no), so assert that everything works
       correctly. */
    HMODULE const user32 = GetModuleHandleA("User32.dll");
    CORRADE_INTERNAL_ASSERT(user32);
    /* GCC 8 adds -Wcast-function-type, enabled by default with -Wextra, which
       causes this line to emit a warning on MinGW. We know what we're doing,
       so suppress that. */
    #if defined(CORRADE_TARGET_GCC) && __GNUC__ >= 8
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
    #endif
    auto const isProcessDPIAware = reinterpret_cast<BOOL(WINAPI *)()>(GetProcAddress(user32, "IsProcessDPIAware"));
    #if defined(CORRADE_TARGET_GCC) && __GNUC__ >= 8
    #pragma GCC diagnostic pop
    #endif
    CORRADE_INTERNAL_ASSERT(isProcessDPIAware);
    return isProcessDPIAware();
}
#endif

}}}

#endif
