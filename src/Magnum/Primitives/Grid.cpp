/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

Trade::MeshData grid3DSolid(const Vector2i& subdivisions, const GridFlags flags) {
    const Vector2i vertexCount = subdivisions + Vector2i{2};
    const Vector2i faceCount = subdivisions + Vector2i{1};

    /* Indices */
    Containers::Array<char> indexData{std::size_t(faceCount.product()*6)*sizeof(UnsignedInt)};
    auto indices = Containers::arrayCast<UnsignedInt>(indexData);
    {
        std::size_t i = 0;
        for(Int y = 0; y != faceCount.y(); ++y) {
            for(Int x = 0; x != faceCount.x(); ++x) {
                /* 2--1 5
                   | / /|
                   |/ / |
                   0 3--4 */
                indices[i++] = UnsignedInt(y*vertexCount.x() + x);
                indices[i++] = UnsignedInt((y + 1)*vertexCount.x() + x + 1);
                indices[i++] = UnsignedInt((y + 1)*vertexCount.x() + x + 0);
                indices[i++] = UnsignedInt(y*vertexCount.x() + x);
                indices[i++] = UnsignedInt(y*vertexCount.x() + x + 1);
                indices[i++] = UnsignedInt((y + 1)*vertexCount.x() + x + 1);
            }
        }
    }

    /* Calculate attribute count and vertex size */
    std::size_t stride = sizeof(Vector3);
    std::size_t attributeCount = 1;
    if(flags & GridFlag::Normals) {
        stride += sizeof(Vector3);
        ++attributeCount;
    }
    if(flags & GridFlag::Tangents) {
        stride += sizeof(Vector4);
        ++attributeCount;
    }
    if(flags & GridFlag::TextureCoordinates) {
        stride += sizeof(Vector2);
        ++attributeCount;
    }
    Containers::Array<char> vertexData{stride*vertexCount.product()};
    Containers::Array<Trade::MeshAttributeData> attributes{attributeCount};
    std::size_t attributeIndex = 0;
    std::size_t attributeOffset = 0;

    /* Fill positions */
    Containers::StridedArrayView1D<Vector3> positions{vertexData,
        reinterpret_cast<Vector3*>(vertexData.begin()),
        std::size_t(vertexCount.product()), std::ptrdiff_t(stride)};
    attributes[attributeIndex++] =
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions};
    attributeOffset += sizeof(Vector3);
    {
        std::size_t i = 0;
        for(Int y = 0; y != vertexCount.y(); ++y)
            for(Int x = 0; x != vertexCount.x(); ++x)
                positions[i++] = {(Vector2(x, y)/Vector2(faceCount))*2.0f - Vector2{1.0f}, 0.0f};
    }

    /* Fill normals and tangents, if any. Those are the same for all. */
    if(flags & GridFlag::Normals) {
        Containers::StridedArrayView1D<Vector3> normals{vertexData,
            reinterpret_cast<Vector3*>(vertexData.begin() + attributeOffset),
            std::size_t(vertexCount.product()), std::ptrdiff_t(stride)};
        attributes[attributeIndex++] =
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, normals};
        attributeOffset += sizeof(Vector3);
        for(auto&& i: normals) i = Vector3::zAxis(1.0f);
    }
    if(flags & GridFlag::Tangents) {
        Containers::StridedArrayView1D<Vector4> tangents{vertexData,
            reinterpret_cast<Vector4*>(vertexData.begin() + attributeOffset),
            std::size_t(vertexCount.product()), std::ptrdiff_t(stride)};
        attributes[attributeIndex++] =
            Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, tangents};
        attributeOffset += sizeof(Vector4);
        for(auto&& i: tangents) i = {1.0f, 0.0f, 0.0f, 1.0f};
    }

    if(flags & GridFlag::TextureCoordinates) {
        Containers::StridedArrayView1D<Vector2> textureCoords{vertexData,
            reinterpret_cast<Vector2*>(vertexData.begin() + attributeOffset),
            std::size_t(vertexCount.product()), std::ptrdiff_t(stride)};
        attributes[attributeIndex++] =
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, textureCoords};
        attributeOffset += sizeof(Vector2);
        for(std::size_t i = 0; i != positions.size(); ++i)
            textureCoords[i] = positions[i].xy()*0.5f + Vector2{0.5f};
    }

    CORRADE_INTERNAL_ASSERT(attributeIndex == attributeCount);
    CORRADE_INTERNAL_ASSERT(attributeOffset == stride);

    return Trade::MeshData{MeshPrimitive::Triangles,
        std::move(indexData), Trade::MeshIndexData{indices},
        std::move(vertexData), std::move(attributes)};
}

namespace {

constexpr Trade::MeshAttributeData AttributeData3DWireframe[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3,
        0, 0, sizeof(Vector3)}
};

}

Trade::MeshData grid3DWireframe(const Vector2i& subdivisions) {
    const Vector2i vertexCount = subdivisions + Vector2i{2};
    const Vector2i faceCount = subdivisions + Vector2i{1};

    Containers::Array<char> indexData{sizeof(UnsignedInt)*
        (vertexCount.y()*(vertexCount.x() - 1)*2 +
        vertexCount.x()*(vertexCount.y() - 1)*2)};
    auto indices = Containers::arrayCast<UnsignedInt>(indexData);
    {
        std::size_t i = 0;
        for(Int y = 0; y != vertexCount.y(); ++y) {
            for(Int x = 0; x != vertexCount.x(); ++x) {
                /* 3    7
                   |    | ...
                   2    6
                   0--1 4--5 ... */
                if(x != vertexCount.x() - 1) {
                    indices[i++] = UnsignedInt(y*vertexCount.x() + x);
                    indices[i++] = UnsignedInt(y*vertexCount.x() + x + 1);
                }
                if(y != vertexCount.y() - 1) {
                    indices[i++] = UnsignedInt(y*vertexCount.x() + x);
                    indices[i++] = UnsignedInt((y + 1)*vertexCount.x() + x);
                }
            }
        }
    }

    Containers::Array<char> vertexData{sizeof(Vector3)*vertexCount.product()};
    auto positions = Containers::arrayCast<Vector3>(vertexData);
    {
        std::size_t i = 0;
        for(Int y = 0; y != vertexCount.y(); ++y)
            for(Int x = 0; x != vertexCount.x(); ++x)
                positions[i++] = {(Vector2(x, y)/Vector2(faceCount))*2.0f - Vector2{1.0f}, 0.0f};
    }

    return Trade::MeshData{MeshPrimitive::Lines,
        std::move(indexData), Trade::MeshIndexData{indices},
        std::move(vertexData),
        Trade::meshAttributeDataNonOwningArray(AttributeData3DWireframe),
        UnsignedInt(vertexCount.product())};
}

}}
