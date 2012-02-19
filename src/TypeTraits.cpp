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

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
static_assert(sizeof(GLubyte) == sizeof(unsigned char), "GLubyte is not the same as unsigned char");
static_assert(sizeof(GLbyte) == sizeof(char), "GLbyte is not the same as char");
static_assert(sizeof(GLushort) == sizeof(unsigned short), "GLushort is not the same as unsigned short");
static_assert(sizeof(GLshort) == sizeof(short), "GLshort is not the same as short");
static_assert(sizeof(GLuint) == sizeof(unsigned int), "GLuint is not the same as unsigned int");
static_assert(sizeof(GLint) == sizeof(int), "GLint is not the same as int");
static_assert(sizeof(GLfloat) == sizeof(float), "GLfloat is not the same as float");
static_assert(sizeof(GLdouble) == sizeof(double), "GLdouble is not the same as double");
#endif

}
