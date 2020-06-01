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

#include "OpenGLFunctionLoader.h"

#include <Corrade/Utility/Assert.h>

/* EGL-specific includes */
#ifdef MAGNUM_PLATFORM_USE_EGL
#include <EGL/egl.h>

/* CGL-specific includes */
#elif defined(CORRADE_TARGET_APPLE)
#include <dlfcn.h>

/* WGL-specific stuff */
#elif defined(CORRADE_TARGET_WINDOWS)
#ifdef _MSC_VER
#pragma warning(disable: 4055)
#pragma warning(disable: 4054)
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/* GLX-specific includes */
#elif defined(CORRADE_TARGET_UNIX) && defined(MAGNUM_PLATFORM_USE_GLX)
#include <GL/glx.h>

/* Otherwise unsupported */
#else
#error unsupported platform
#endif

namespace Magnum { namespace Platform { namespace Implementation {

/* EGL-specific implementation */
#ifdef MAGNUM_PLATFORM_USE_EGL
OpenGLFunctionLoader::OpenGLFunctionLoader() = default;

OpenGLFunctionLoader::~OpenGLFunctionLoader() = default;

auto OpenGLFunctionLoader::load(const char* const name) -> FunctionPointer {
    return eglGetProcAddress(name);
}

/* CGL-specific implementation. Apple doesn't have any possibility of a
   "desktop GLES" implementation (and never will have), so explicitly check
   that we are *not* on GLES. */
#elif defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
OpenGLFunctionLoader::OpenGLFunctionLoader() {
    CORRADE_INTERNAL_ASSERT_OUTPUT(library = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY));
}

OpenGLFunctionLoader::~OpenGLFunctionLoader() {
    dlclose(library);
}

auto OpenGLFunctionLoader::load(const char* const name) -> FunctionPointer {
    return reinterpret_cast<FunctionPointer>(dlsym(library, name));
}

/* WGL-specific implementation */
#elif defined(CORRADE_TARGET_WINDOWS)
OpenGLFunctionLoader::OpenGLFunctionLoader() {
    library = GetModuleHandleA("opengl32.dll");
    getProcAddress = reinterpret_cast<FunctionPointer>(GetProcAddress(library, "wglGetProcAddress"));
}

/* FreeLibrary() should not be called:
   https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlea */
OpenGLFunctionLoader::~OpenGLFunctionLoader() = default;

auto OpenGLFunctionLoader::load(const char* const name) -> FunctionPointer {
    /* First try wglGetProcAddress that we extracted above, then a
       normal GetProcAddress(). Not using wglGetProcAddress() directly because
       that would mean we need to explicitly link to opengl32.dll (and since
       this is inside a static library, also forcing our users to link to it
       too). And why do that when `library` has that already anyway. */
    const PROC address = reinterpret_cast<PROC(WINAPI*)(LPCSTR)>(getProcAddress)(reinterpret_cast<LPCSTR>(name));
    /* This actually is real, it seems -- OH MY GOD
       https://community.khronos.org/t/wglgetprocaddress/77122 */
    const auto integerAddress = reinterpret_cast<std::ptrdiff_t>(address);
    if(address && integerAddress != 1 && integerAddress != 2 &&
        integerAddress != 3 && integerAddress != -1) return reinterpret_cast<FunctionPointer>(address);

    return reinterpret_cast<FunctionPointer>(GetProcAddress(library, reinterpret_cast<LPCSTR>(name)));
}

/* GLX-specific implementation */
#elif defined(CORRADE_TARGET_UNIX) && defined(MAGNUM_PLATFORM_USE_GLX)
OpenGLFunctionLoader::OpenGLFunctionLoader() = default;

OpenGLFunctionLoader::~OpenGLFunctionLoader() = default;

auto OpenGLFunctionLoader::load(const char* const name) -> FunctionPointer {
    return glXGetProcAddressARB(reinterpret_cast<const GLubyte*>(name));
}

/* Otherwise unsupported */
#else
#error unsupported platform
#endif

}}}
