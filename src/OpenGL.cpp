/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <type_traits>

#include "OpenGL.h"
#include "Types.h"

namespace Magnum {

static_assert(std::is_same<GLubyte, UnsignedByte>::value, "GLubyte is not the same as UnsignedByte");
static_assert(std::is_same<GLbyte, Byte>::value, "GLbyte is not the same as Byte");
static_assert(std::is_same<GLushort, UnsignedShort>::value, "GLushort is not the same as UnsignedShort");
static_assert(std::is_same<GLshort, Short>::value, "GLshort is not the same as Short");
static_assert(std::is_same<GLuint, UnsignedInt>::value, "GLuint is not the same as UnsignedInt");
static_assert(std::is_same<GLint, Int>::value, "GLint is not the same as Int");
static_assert(std::is_same<GLsizei, Int>::value, "GLsizei is not the same as Int");
static_assert(std::is_same<GLfloat, Float>::value, "GLfloat is not the same as Float");
#ifndef MAGNUM_TARGET_GLES
static_assert(std::is_same<GLdouble, Double>::value, "GLdouble is not the same as Double");
#endif

}
