#ifndef Magnum_Magnum_h
#define Magnum_Magnum_h
/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <GL/glew.h>

#include "Math/Matrix4.h"
#include "Math/Vector4.h"

namespace Magnum {

typedef Math::Vector3<GLfloat> Vector3;
typedef Math::Vector4<GLfloat> Vector4;
typedef Math::Matrix4<GLfloat> Matrix4;

/**
 * @brief Disable copying of given class
 * @param class             Class name
 *
 * Makes copy constructor and assignment operator private, so the class cannot
 * be copied. Should be placed at the beginning of class definition.
 */
#define DISABLE_COPY(class) \
    class(const class&); \
    class& operator=(const class&);

}

#endif