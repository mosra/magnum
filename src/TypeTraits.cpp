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

#include "TypeTraits.h"

#include <type_traits>

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
static_assert(std::is_same<GLubyte, std::uint8_t>::value, "GLubyte is not the same as std::uint8_t");
static_assert(std::is_same<GLbyte, std::int8_t>::value, "GLbyte is not the same as std::int8_t");
static_assert(std::is_same<GLushort, std::uint16_t>::value, "GLushort is not the same as std::uint16_t");
static_assert(std::is_same<GLshort, std::int16_t>::value, "GLshort is not the same as std::int16_t");
static_assert(std::is_same<GLuint, std::uint32_t>::value, "GLuint is not the same as std::uint32_t");
static_assert(std::is_same<GLint, std::int32_t>::value, "GLint is not the same as std::int32_t");
static_assert(std::is_same<GLsizei, std::int32_t>::value, "GLsizei is not the same as std::int32_t");
static_assert(std::is_same<GLfloat, float>::value, "GLfloat is not the same as float");
#ifndef MAGNUM_TARGET_GLES
static_assert(std::is_same<GLdouble, double>::value, "GLdouble is not the same as double");
#endif
#endif

}
