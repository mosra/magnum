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

#include "Crosshair.h"

#include "Math/Vector3.h"
#include "Trade/MeshData2D.h"
#include "Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

Trade::MeshData2D Crosshair2D::wireframe() {
    return Trade::MeshData2D(Mesh::Primitive::Lines, nullptr, {new std::vector<Vector2>{
        {-1.0f,  0.0f}, {1.0f, 0.0f},
        { 0.0f, -1.0f}, {0.0f, 1.0f}
    }}, {});
}

Trade::MeshData3D Crosshair3D::wireframe() {
    return Trade::MeshData3D(Mesh::Primitive::Lines, nullptr, {new std::vector<Vector3>{
        {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f},
        { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f, 0.0f},
        { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}
    }}, {}, {});
}

}}
