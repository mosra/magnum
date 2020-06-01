#ifndef Magnum_GL_OpenGL_h
#define Magnum_GL_OpenGL_h
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

/** @file
 * @brief OpenGL headers
 */

#include <Corrade/configure.h>

#include "Magnum/configure.h"

/* Desktop OpenGL */
#ifndef MAGNUM_TARGET_GLES
    #include "MagnumExternal/OpenGL/GL/flextGL.h"

/* Special case for Emscripten (no extension loading) */
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
    #ifdef MAGNUM_TARGET_GLES2
        #include "MagnumExternal/OpenGL/GLES2/flextGLEmscripten.h"
    #else
        #include "MagnumExternal/OpenGL/GLES3/flextGLEmscripten.h"
    #endif

/* Special case for desktop GLES on Windows (still links to the old opengl32.dll) */
#elif defined(CORRADE_TARGET_WINDOWS) && defined(MAGNUM_TARGET_DESKTOP_GLES)
    #ifdef MAGNUM_TARGET_GLES2
        #include "MagnumExternal/OpenGL/GLES2/flextGLWindowsDesktop.h"
    #else
        #include "MagnumExternal/OpenGL/GLES3/flextGLWindowsDesktop.h"
    #endif

/* Generic, sane OpenGL ES */
#elif defined(MAGNUM_TARGET_GLES2)
    #include "MagnumExternal/OpenGL/GLES2/flextGL.h"
#else
    #include "MagnumExternal/OpenGL/GLES3/flextGL.h"
#endif

#endif
