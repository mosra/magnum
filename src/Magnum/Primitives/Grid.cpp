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

#include "Grid.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

Trade::MeshData3D grid3DSolid(const Vector2i& subdivisions, const GridFlags flags) {
    const Vector2i vertexCount = subdivisions + Vector2i{2};
    const Vector2i faceCount = subdivisions + Vector2i{1};

    std::vector<Vector3> positions;
    positions.reserve(vertexCount.product());
    for(Int y = 0; y != vertexCount.y(); ++y)
        for(Int x = 0; x != vertexCount.x(); ++x)
            positions.emplace_back((Vector2(x, y)/Vector2(faceCount))*2.0f - Vector2{1.0f}, 0.0f);

    std::vector<UnsignedInt> indices;
    indices.reserve(faceCount.product()*6);
    for(Int y = 0; y != faceCount.y(); ++y) {
        for(Int x = 0; x != faceCount.x(); ++x) {
            /* 2--1 5
               | / /|
               |/ / |
               0 3--4 */
            indices.insert(indices.end(), {
                UnsignedInt(y*vertexCount.x() + x),
                UnsignedInt((y + 1)*vertexCount.x() + x + 1),
                UnsignedInt((y + 1)*vertexCount.x() + x + 0),
                UnsignedInt(y*vertexCount.x() + x),
                UnsignedInt(y*vertexCount.x() + x + 1),
                UnsignedInt((y + 1)*vertexCount.x() + x + 1)});
        }
    }

    std::vector<std::vector<Vector3>> normals;
    if(flags & GridFlag::GenerateNormals)
        normals.emplace_back(positions.size(), Vector3::zAxis(1.0f));

    std::vector<std::vector<Vector2>> textureCoordinates;
    if(flags & GridFlag::GenerateTextureCoords) {
        textureCoordinates.emplace_back();
        textureCoordinates[0].reserve(positions.size());
        for(std::size_t i = 0; i != positions.size(); ++i)
            textureCoordinates[0].emplace_back(positions[i].xy()*0.5f + Vector2{0.5f});
    }

    return Trade::MeshData3D{MeshPrimitive::Triangles, std::move(indices), {std::move(positions)}, std::move(normals), std::move(textureCoordinates), {}, nullptr};
}

Trade::MeshData3D grid3DWireframe(const Vector2i& subdivisions) {
    const Vector2i vertexCount = subdivisions + Vector2i{2};
    const Vector2i faceCount = subdivisions + Vector2i{1};

    std::vector<Vector3> positions;
    positions.reserve(vertexCount.product());
    for(Int y = 0; y != vertexCount.y(); ++y)
        for(Int x = 0; x != vertexCount.x(); ++x)
            positions.emplace_back((Vector2(x, y)/Vector2(faceCount))*2.0f - Vector2{1.0f}, 0.0f);

    std::vector<UnsignedInt> indices;
    indices.reserve(vertexCount.y()*(vertexCount.x() - 1)*2 +
                    vertexCount.x()*(vertexCount.y() - 1)*2);
    for(Int y = 0; y != vertexCount.y(); ++y) {
        for(Int x = 0; x != vertexCount.x(); ++x) {
            /* 3    7
               |    | ...
               2    6
               0--1 4--5 ... */
            if(x != vertexCount.x() - 1) indices.insert(indices.end(), {
                UnsignedInt(y*vertexCount.x() + x),
                UnsignedInt(y*vertexCount.x() + x + 1)});
            if(y != vertexCount.y() - 1) indices.insert(indices.end(), {
                UnsignedInt(y*vertexCount.x() + x),
                UnsignedInt((y + 1)*vertexCount.x() + x)});
        }
    }

    return Trade::MeshData3D{MeshPrimitive::Lines, std::move(indices), {std::move(positions)}, {}, {}, {}, nullptr};
}

}}
