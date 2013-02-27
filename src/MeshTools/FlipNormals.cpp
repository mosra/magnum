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

#include "FlipNormals.h"

#include "Math/Vector3.h"

namespace Magnum { namespace MeshTools {

void flipFaceWinding(std::vector<UnsignedInt>& indices) {
    CORRADE_ASSERT(!(indices.size()%3), "MeshTools::flipNormals(): index count is not divisible by 3!", );

    for(std::size_t i = 0; i != indices.size(); i += 3)
        std::swap(indices[i+1], indices[i+2]);
}

void flipNormals(std::vector<Vector3>& normals) {
    for(Vector3& normal: normals)
        normal = -normal;
}

}}
