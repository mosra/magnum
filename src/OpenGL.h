#ifndef Magnum_OpenGL_h
#define Magnum_OpenGL_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <corradeConfigure.h>
#include "magnumConfigure.h"

/* Desktop OpenGL */
#ifndef MAGNUM_TARGET_GLES
#include <GL/glew.h>
#include <OpenGL/GL/glcorearb.h>

/* NaCl has its own gl2.h, the official one causes linker issues. Additionaly
   to NaCl's gl2ext.h we are including our own to prevent undeclared symbol
   errors with some recent extensions. */
#elif defined(CORRADE_TARGET_NACL)

/* Enable function prototypes (the supported ones shouldn't fail at link time) */
#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#undef __gl2ext_h_
#include <OpenGL/GLES2/gl2ext.h>

/* Generic OpenGL ES */
#else
#include <OpenGL/KHR/khrplatform.h>
#ifndef MAGNUM_TARGET_GLES2
#include <OpenGL/GLES3/gl3platform.h>
#include <OpenGL/GLES3/gl3.h>
#else
#include <OpenGL/GLES2/gl2platform.h>
#include <OpenGL/GLES2/gl2.h>
#include <OpenGL/GLES2/gl2ext.h>
#endif
#endif

#endif
