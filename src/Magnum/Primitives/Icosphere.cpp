/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <Corrade/Containers/GrowableArray.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Subdivide.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

constexpr UnsignedInt Indices[]{
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

constexpr Vector3 Positions[]{
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

}

Trade::MeshData icosphereSolid(const UnsignedInt subdivisions) {
    const std::size_t indexCount = Containers::arraySize(Indices)*(1 << subdivisions*2);
    const std::size_t vertexCount = Containers::arraySize(Positions) + ((indexCount - Containers::arraySize(Indices))/3);

    Containers::Array<char> indexData{indexCount*sizeof(UnsignedInt)};
    auto indices = Containers::arrayCast<UnsignedInt>(indexData);
    std::memcpy(indices.begin(), Indices, sizeof(Indices));

    struct Vertex {
        Vector3 position;
        Vector3 normal;
    };
    Containers::Array<char> vertexData;
    arrayResize(vertexData, Containers::NoInit, sizeof(Vertex)*vertexCount);

    /* Build up the subdivided positions */
    {
        auto vertices = Containers::arrayCast<Vertex>(vertexData);
        Containers::StridedArrayView1D<Vector3> positions{vertices, &vertices[0].position, vertices.size(), sizeof(Vertex)};
        for(std::size_t i = 0; i != Containers::arraySize(Positions); ++i)
            positions[i] = Positions[i];

        for(std::size_t i = 0; i != subdivisions; ++i) {
            const std::size_t iterationIndexCount = Containers::arraySize(Indices)*(1 << (i + 1)*2);
            const std::size_t iterationVertexCount = Containers::arraySize(Positions) + ((iterationIndexCount - Containers::arraySize(Indices))/3);
            MeshTools::subdivideInPlace(indices.prefix(iterationIndexCount), positions.prefix(iterationVertexCount), [](const Vector3& a, const Vector3& b) {
                return (a+b).normalized();
            });
        }

        /** @todo i need arrayShrinkAndGiveUpMemoryIfItDoesntCauseRealloc() */
        arrayResize(vertexData, MeshTools::removeDuplicatesIndexedInPlace(Containers::stridedArrayView(indices), Containers::stridedArrayView(positions))*sizeof(Vertex));
    }

    /* Build up the views again with correct size, fill the normals */
    auto vertices = Containers::arrayCast<Vertex>(vertexData);
    Containers::StridedArrayView1D<Vector3> positions{vertices, &vertices[0].position, vertices.size(), sizeof(Vertex)};
    Containers::StridedArrayView1D<Vector3> normals{vertices, &vertices[0].normal, vertices.size(), sizeof(Vertex)};
    for(std::size_t i = 0; i != positions.size(); ++i)
        normals[i] = positions[i];

    return Trade::MeshData{MeshPrimitive::Triangles, std::move(indexData),
        Trade::MeshIndexData{indices}, std::move(vertexData),
        {Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions},
         Trade::MeshAttributeData{Trade::MeshAttribute::Normal, normals}}};
}

}}
