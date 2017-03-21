/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "Icosphere.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Subdivide.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

Trade::MeshData3D Icosphere::solid(const UnsignedInt subdivisions) {
    std::vector<UnsignedInt> indices{
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
    };

    std::vector<Vector3> positions{
        {0.0f, -0.525731f, 0.850651f},
        {0.850651f, 0.0f, 0.525731f},
        {0.850651f, 0.0f, -0.525731f},
        {-0.850651f, 0.0f, -0.525731f},
        {-0.850651f, 0.0f, 0.525731f},
        {-0.525731f, 0.850651f, 0.0f},
        {0.525731f, 0.850651f, 0.0f},
        {0.525731f, -0.850651f, 0.0f},
        {-0.525731f, -0.850651f, 0.0f},
        {0.0f, -0.525731f, -0.850651f},
        {0.0f, 0.525731f, -0.850651f},
        {0.0f, 0.525731f, 0.850651f}
    };

    for(std::size_t i = 0; i != subdivisions; ++i)
        MeshTools::subdivide(indices, positions, [](const Vector3& a, const Vector3& b) {
            return (a+b).normalized();
        });

    indices = MeshTools::duplicate(indices, MeshTools::removeDuplicates(positions));

    std::vector<Vector3> normals(positions);
    return Trade::MeshData3D{MeshPrimitive::Triangles, std::move(indices), {std::move(positions)}, {std::move(normals)}, {}, {}, nullptr};
}

}}
