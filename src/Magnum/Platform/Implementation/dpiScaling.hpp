#ifndef Magnum_Platform_Implementation_dpiScaling_hpp
#define Magnum_Platform_Implementation_dpiScaling_hpp
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Utility/Arguments.h>

#include "Magnum/Magnum.h"

#ifdef _MAGNUM_PLATFORM_USE_X11
#include <dlfcn.h>
#include <X11/Xresource.h>
#include <Corrade/Containers/ScopedExit.h>
#undef None
#endif

#ifdef CORRADE_TARGET_EMSCRIPTEN
#include <emscripten/html5.h>
#endif

namespace Magnum { namespace Platform { namespace Implementation {

namespace {

inline Utility::Arguments windowScalingArguments() {
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
/* Returns DPI scaling for current X11 instance. Because X11 (as opposed to
   Wayland) doesn't have per-monitor scaling, it's fetched from the default
   display. */
inline Float x11DpiScaling() {
    /* If the end app links to X11, these symbols will be available in a global
       scope and we can use that to query the DPI. If not, then those symbols
       won't be and that's okay -- it may be using Wayland or something else. */
    void* xlib = dlopen(nullptr, RTLD_NOW|RTLD_GLOBAL);
    Containers::ScopedExit closeXlib{xlib, dlclose};
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
    Containers::ScopedExit closeDisplay{display, xCloseDisplay};

    const char* rms = xResourceManagerString(display);
    if(rms) {
        XrmDatabase db = xrmGetStringDatabase(rms);
        CORRADE_INTERNAL_ASSERT(db);
        Containers::ScopedExit closeDb{db, xrmDestroyDatabase};

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
inline Float emscriptenDpiScaling() {
    return Float(emscripten_get_device_pixel_ratio());
}
#endif

}

#ifdef CORRADE_TARGET_APPLE
bool isAppleBundleHiDpiEnabled();
#endif

}}}

#endif
