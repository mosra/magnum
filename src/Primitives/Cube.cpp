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

#include "Cube.h"

#include "Math/Point3D.h"

using namespace std;

namespace Magnum { namespace Primitives {

Cube::Cube(): MeshData3D("", Mesh::Primitive::Triangles, new vector<uint32_t>{
    0, 2, 1,
    2, 3, 1,
    1, 3, 5,
    3, 7, 5,
    5, 7, 4,
    7, 6, 4,
    4, 6, 0,
    6, 2, 0,
    2, 7, 3,
    2, 6, 7,
    4, 1, 5,
    4, 0, 1
}, {new vector<Point3D>}, {new vector<Vector3>{
    {-1.0f, -1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    {-1.0f, -1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f}
}}, {}) {
    positions(0)->assign(normals(0)->begin(), normals(0)->end());
}

}}
