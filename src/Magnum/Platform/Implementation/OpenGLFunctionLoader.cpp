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

/* GLX-specific includes */
#elif defined(CORRADE_TARGET_UNIX) && defined(MAGNUM_PLATFORM_USE_GLX)
#include <GL/glx.h>

/* Otherwise unsupported */
#else
#error unsupported platform
#endif

#ifdef CORRADE_TARGET_WINDOWS
#include "Magnum/Platform/Implementation/configure.h"

#ifdef _MAGNUM_BUILD_GPU_PREFERENCE_SYMBOLS
static_assert(sizeof(int) == 4, "int is not 32bit?!");

extern "C" {

/* Exported symbols. Default to the iGPU. These symbols are fetched from
   GL::Context and set to 1 if `--magnum-gpu-preference` is set to `dedicated`.
   In case the application already exports these and the symbols are
   conflicting, compile Magnum with the BUILD_GPU_PREFERENCE_SYMBOLS option
   disabled. */

/* https://community.amd.com/thread/169965 */
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0;

/* http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf */
__declspec(dllexport) int NvOptimusEnablement = 0;

}
#endif
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
    library = GetModuleHandleA("OpenGL32.dll");

    /* Make sure the linker doesn't discard these symbols */
    #ifdef _MAGNUM_BUILD_GPU_PREFERENCE_SYMBOLS
    NvOptimusEnablement &= 0xffff;
    AmdPowerXpressRequestHighPerformance &= 0xffff;
    #endif
}

/** @todo closing the library is not needed? */
OpenGLFunctionLoader::~OpenGLFunctionLoader() = default;

auto OpenGLFunctionLoader::load(const char* const name) -> FunctionPointer {
     /** @todo Is this shit real?! OH MY GOD. */
     const PROC address = wglGetProcAddress(reinterpret_cast<LPCSTR>(name));
     const auto integerAddress = reinterpret_cast<std::ptrdiff_t>(address);
     if(address && integerAddress != 1 && integerAddress != 2 &&
        integerAddress != 3 && integerAddress != -1) return address;

     return GetProcAddress(library, reinterpret_cast<LPCSTR>(name));
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
