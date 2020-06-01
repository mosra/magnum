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

#include <type_traits>

#include "Magnum/Types.h"
#include "Magnum/GL/OpenGL.h"

namespace Magnum { namespace GL {

/* Verify types. GLbyte, GLushort, and GLshort are used only by desktop GL for
   single-value vertex attributes and nowhere else and those functions are no
   on ES. */
static_assert(std::is_same<GLubyte, UnsignedByte>::value, "GLubyte is not the same as UnsignedByte");
#ifndef MAGNUM_TARGET_GLES
static_assert(std::is_same<GLbyte, Byte>::value, "GLbyte is not the same as Byte");
static_assert(std::is_same<GLushort, UnsignedShort>::value, "GLushort is not the same as UnsignedShort");
static_assert(std::is_same<GLshort, Short>::value, "GLshort is not the same as Short");
#endif
static_assert(std::is_same<GLuint, UnsignedInt>::value, "GLuint is not the same as UnsignedInt");
static_assert(std::is_same<GLint, Int>::value, "GLint is not the same as Int");
static_assert(std::is_same<GLsizei, Int>::value, "GLsizei is not the same as Int");
static_assert(std::is_same<GLfloat, Float>::value, "GLfloat is not the same as Float");
#ifndef MAGNUM_TARGET_GLES
static_assert(std::is_same<GLdouble, Double>::value, "GLdouble is not the same as Double");
#endif

/* Verify boolean values */
static_assert(GL_FALSE == false, "GL_FALSE is not the same as false");
static_assert(GL_TRUE == true, "GL_TRUE is not the same as true");

}}
