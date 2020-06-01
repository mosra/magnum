#ifndef Magnum_Platform_Implementation_OpenGLFunctionLoader_h
#define Magnum_Platform_Implementation_OpenGLFunctionLoader_h
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

#include "Magnum/Magnum.h"

#if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_PLATFORM_USE_EGL)
/* I didn't find a better way to circumvent the need for including windows.h */
struct HINSTANCE__;
typedef struct HINSTANCE__* HMODULE;
#endif

namespace Magnum { namespace Platform { namespace Implementation {

class OpenGLFunctionLoader {
    public:
        using FunctionPointer = void(*)();

        explicit OpenGLFunctionLoader();
        ~OpenGLFunctionLoader();

        FunctionPointer load(const char* name);

    private:
        /* EGL-specific handles (nothing needed) */
        #ifdef MAGNUM_PLATFORM_USE_EGL

        /* CGL-specific handles */
        #elif defined(CORRADE_TARGET_APPLE)
        void* library;

        /* WGL-specific handles */
        #elif defined(CORRADE_TARGET_WINDOWS)
        HMODULE library;
        FunctionPointer getProcAddress;

        /* GLX-specific handles (nothing needed) */
        #elif defined(CORRADE_TARGET_UNIX) && defined(MAGNUM_PLATFORM_USE_GLX)

        /* Otherwise unsupported */
        #else
        #error unsupported platform
        #endif
};

}}}

#endif
