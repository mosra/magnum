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

#include "Plane.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

constexpr struct VertexSolid {
    Vector3 position;
    Vector3 normal;
} VerticesSolid[] {
    {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
};
constexpr Trade::MeshAttributeData AttributesSolid[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesSolid, &VerticesSolid[0].position,
            Containers::arraySize(VerticesSolid), sizeof(VertexSolid))},
    Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
        Containers::stridedArrayView(VerticesSolid, &VerticesSolid[0].normal,
            Containers::arraySize(VerticesSolid), sizeof(VertexSolid))}
};

}

Trade::MeshData planeSolid() {
    return Trade::MeshData{MeshPrimitive::TriangleStrip,
        {}, VerticesSolid,
        Trade::meshAttributeDataNonOwningArray(AttributesSolid)};
}

Trade::MeshData planeSolid(const PlaneFlags flags) {
    /* Return the compile-time data if nothing extra is requested */
    if(!flags) return planeSolid();

    /* Calculate attribute count and vertex size */
    std::size_t stride = sizeof(Vector3) + sizeof(Vector3);
    std::size_t attributeCount = 2;
    if(flags & PlaneFlag::Tangents) {
        stride += sizeof(Vector4);
        ++attributeCount;
    }
    if(flags & PlaneFlag::TextureCoordinates) {
        stride += sizeof(Vector2);
        ++attributeCount;
    }

    /* Set up the layout */
    Containers::Array<char> vertexData{Containers::NoInit, 4*stride};
    Containers::Array<Trade::MeshAttributeData> attributeData{attributeCount};
    std::size_t attributeIndex = 0;
    std::size_t attributeOffset = 0;

    Containers::StridedArrayView1D<Vector3> positions{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + attributeOffset),
        4, std::ptrdiff_t(stride)};
    attributeData[attributeIndex++] = Trade::MeshAttributeData{
        Trade::MeshAttribute::Position, positions};
    attributeOffset += sizeof(Vector3);

    Containers::StridedArrayView1D<Vector3> normals{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + sizeof(Vector3)),
        4, std::ptrdiff_t(stride)};
    attributeData[attributeIndex++] = Trade::MeshAttributeData{
        Trade::MeshAttribute::Normal, normals};
    attributeOffset += sizeof(Vector3);

    Containers::StridedArrayView1D<Vector4> tangents;
    if(flags & PlaneFlag::Tangents) {
        tangents = Containers::StridedArrayView1D<Vector4>{vertexData,
            reinterpret_cast<Vector4*>(vertexData.data() + attributeOffset),
            4, std::ptrdiff_t(stride)};
        attributeData[attributeIndex++] = Trade::MeshAttributeData{
            Trade::MeshAttribute::Tangent, tangents};
        attributeOffset += sizeof(Vector4);
    }

    Containers::StridedArrayView1D<Vector2> textureCoordinates;
    if(flags & PlaneFlag::TextureCoordinates) {
        textureCoordinates = Containers::StridedArrayView1D<Vector2>{vertexData,
            reinterpret_cast<Vector2*>(vertexData.data() + attributeOffset),
            4, std::ptrdiff_t(stride)};
        attributeData[attributeIndex++] = Trade::MeshAttributeData{
            Trade::MeshAttribute::TextureCoordinates, textureCoordinates};
        attributeOffset += sizeof(Vector2);
    }

    CORRADE_INTERNAL_ASSERT(attributeIndex == attributeCount);
    CORRADE_INTERNAL_ASSERT(attributeOffset == stride);

    /* Fill the data */
    for(std::size_t i = 0; i != 4; ++i) {
        positions[i] = VerticesSolid[i].position;
        normals[i] = VerticesSolid[i].normal;
        if(flags & PlaneFlag::Tangents)
            tangents[i] = {1.0f, 0.0f, 0.0f, 1.0f};
    }
    if(flags & PlaneFlag::TextureCoordinates) {
        textureCoordinates[0] = {1.0f, 0.0f};
        textureCoordinates[1] = {1.0f, 1.0f};
        textureCoordinates[2] = {0.0f, 0.0f};
        textureCoordinates[3] = {0.0f, 1.0f};
    }

    return Trade::MeshData{MeshPrimitive::TriangleStrip,
        std::move(vertexData), std::move(attributeData)};
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Trade::MeshData planeSolid(const PlaneTextureCoords textureCoords) {
    return planeSolid(textureCoords == PlaneTextureCoords::Generate ?
        PlaneFlag::TextureCoordinates : PlaneFlags{});
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

namespace {

/* Can't be just an array of Vector3 because MSVC 2015 is special. See
   Crosshair.cpp for details. */
constexpr struct VertexWireframe {
    Vector3 position;
} VerticesWireframe[]{
    {{-1.0f, -1.0f, 0.0f}},
    {{ 1.0f, -1.0f, 0.0f}},
    {{ 1.0f,  1.0f, 0.0f}},
    {{-1.0f,  1.0f, 0.0f}}
};
constexpr Trade::MeshAttributeData AttributesWireframe[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesWireframe, &VerticesWireframe[0].position,
            Containers::arraySize(VerticesWireframe), sizeof(VertexWireframe))}
};

}

Trade::MeshData planeWireframe() {
    return Trade::MeshData{MeshPrimitive::LineLoop,
        {}, VerticesWireframe,
        Trade::meshAttributeDataNonOwningArray(AttributesWireframe)};
}

}}
