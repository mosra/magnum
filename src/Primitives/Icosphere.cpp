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

#include "Icosphere.h"

#include "Math/Vector4.h"

using namespace std;

namespace Magnum { namespace Primitives {

Icosphere<0>::Icosphere(): MeshData("", Mesh::Primitive::Triangles, new vector<unsigned int>{
    1, 2, 6,
    1, 7, 2,
    3, 4, 5,
    4, 3, 8,
    6, 5, 11,
    5, 6, 10,
    9, 10, 2,
    10, 9, 3,
    7, 8, 9,
    8, 7, 0,
    11, 0, 1,
    0, 11, 4,
    6, 2, 10,
    1, 6, 11,
    3, 5, 10,
    5, 4, 11,
    2, 7, 9,
    7, 1, 0,
    3, 9, 8,
    4, 8, 0
}, {new vector<Vector4>}, {new vector<Vector3>{
    Vector3(0, -0.525731f, 0.850651f),
    Vector3(0.850651f, 0, 0.525731f),
    Vector3(0.850651f, 0, -0.525731f),
    Vector3(-0.850651f, 0, -0.525731f),
    Vector3(-0.850651f, 0, 0.525731f),
    Vector3(-0.525731f, 0.850651f, 0),
    Vector3(0.525731f, 0.850651f, 0),
    Vector3(0.525731f, -0.850651f, 0),
    Vector3(-0.525731f, -0.850651f, 0),
    Vector3(0, -0.525731f, -0.850651f),
    Vector3(0, 0.525731f, -0.850651f),
    Vector3(0, 0.525731f, 0.850651f)
}}, {}) {
    positions(0)->assign(normals(0)->begin(), normals(0)->end());
}

}}
