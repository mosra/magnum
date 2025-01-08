#ifndef Magnum_configure_h
#define Magnum_configure_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#cmakedefine MAGNUM_BUILD_DEPRECATED
#cmakedefine MAGNUM_BUILD_STATIC
#cmakedefine MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS
#cmakedefine MAGNUM_TARGET_GL
#cmakedefine MAGNUM_TARGET_GLES
#cmakedefine MAGNUM_TARGET_GLES2
#cmakedefine MAGNUM_TARGET_WEBGL
#cmakedefine MAGNUM_TARGET_EGL
#cmakedefine MAGNUM_TARGET_VK

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Corrade/configure.h"
/* The following applies only to desktop platforms */
#if !defined(CORRADE_TARGET_IOS) && !defined(CORRADE_TARGET_ANDROID) && !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_WINDOWS_RT)
/* MAGNUM_TARGET_HEADLESS used to be an option defined on desktop GL only, not
   on ES; MAGNUM_TARGET_EGL is defined implicitly also on all platforms that
   use EGL exclusively. Deliberate double space after the #define to avoid
   being unconditionally matched by older FindMagnum modules. */
#if !defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_EGL)
#define  MAGNUM_TARGET_HEADLESS
#endif
/* MAGNUM_TARGET_DESKTOP_GLES used to do the opposite of
   MAGNUM_TARGET_HEADLESS, i.e. force GLX/WGL instead of EGL on GLES builds.
   Deliberate double space after the #define to avoid being unconditionally
   matched by older FindMagnum modules. */
#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_EGL)
#define  MAGNUM_TARGET_DESKTOP_GLES
#endif
#endif
/* MAGNUM_TARGET_GLES3 used to be an inverse of MAGNUM_TARGET_GLES2 in an
   anticipation of there eventually being GLES 4. Used very rarely in the
   internals and only led to confusion. It's however still a cmakedefine so old
   FindMagnum modules, which use it for linking OpenGLES3 libraries, can detect
   and use it. */
#cmakedefine MAGNUM_TARGET_GLES3
#endif

#endif // kate: hl c++
