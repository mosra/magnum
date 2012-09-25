#ifndef Magnum_MeshTools_Transform_h
#define Magnum_MeshTools_Transform_h
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

/** @file
 * @brief Function Magnum::MeshTools::transform()
 */

#include "Math/Matrix.h"

namespace Magnum { namespace MeshTools {

/**
@brief Transform vertices using given matrix

Usable for mesh transformations that would otherwise negatively affect
dependent objects, such as (uneven) scaling. Example usage:

@code
std::vector<Point3D> vertices;
MeshTools::transform(Matrix4::scaling({2.0f, 0.5f, 0.0f}), vertices);
@endcode
*/
template<size_t size, class T, class U> inline void transform(const Math::Matrix<size, T>& matrix, U& vertices) {
    for(Math::Vector<size, T>& vertex: vertices)
        vertex = matrix*vertex;
}

}}

#endif
