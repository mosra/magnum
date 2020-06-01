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

#include "Circle.h"

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Mesh.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

constexpr Trade::MeshAttributeData AttributeData2D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2,
        0, 0, sizeof(Vector2)}
};

constexpr Trade::MeshAttributeData AttributeData2DTextureCoords[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2,
        0, 0, 2*sizeof(Vector2)},
    Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2,
        sizeof(Vector2), 0, 2*sizeof(Vector2)}
};

}

Trade::MeshData circle2DSolid(const UnsignedInt segments, const Circle2DFlags flags) {
    CORRADE_ASSERT(segments >= 3, "Primitives::circle2DSolid(): segments must be >= 3",
        (Trade::MeshData{MeshPrimitive::TriangleFan, 0}));

    /* Allocate interleaved array for all vertex data */
    Containers::Array<Trade::MeshAttributeData> attributes;
    if(flags & Circle2DFlag::TextureCoordinates)
        attributes = Trade::meshAttributeDataNonOwningArray(AttributeData2DTextureCoords);
    else
        attributes = Trade::meshAttributeDataNonOwningArray(AttributeData2D);
    const std::size_t stride = attributes[0].stride();
    Containers::Array<char> vertexData{stride*(segments + 2)};

    /* Fill positions */
    Containers::StridedArrayView1D<Vector2> positions{vertexData,
        reinterpret_cast<Vector2*>(vertexData.begin()),
        segments + 2, std::ptrdiff_t(stride)};
    positions[0] = {};
    /* Points on the circle. The first/last point is here twice to close the
       circle properly. */
    const Rad angleIncrement(Constants::tau()/segments);
    for(UnsignedInt i = 0; i != segments + 1; ++i) {
        const Rad angle(Float(i)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);
        positions[i + 1] = {sincos.second, sincos.first};
    }

    /* Fill texture coords, if any */
    if(flags & Circle2DFlag::TextureCoordinates) {
        Containers::StridedArrayView1D<Vector2> textureCoords{vertexData,
            reinterpret_cast<Vector2*>(vertexData.begin() + sizeof(Vector2)),
            positions.size(), std::ptrdiff_t(stride)};
        for(std::size_t i = 0; i != positions.size(); ++i)
            textureCoords[i] = positions[i]*0.5f + Vector2{0.5f};
    }

    return Trade::MeshData{MeshPrimitive::TriangleFan, std::move(vertexData), std::move(attributes), UnsignedInt(positions.size())};
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Trade::MeshData circle2DSolid(const UnsignedInt segments, const CircleTextureCoords textureCoords) {
    return circle2DSolid(segments, textureCoords == CircleTextureCoords::Generate ?
        Circle2DFlag::TextureCoordinates : Circle2DFlags{});
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

Trade::MeshData circle2DWireframe(const UnsignedInt segments) {
    CORRADE_ASSERT(segments >= 3, "Primitives::circle2DWireframe(): segments must be >= 3",
        (Trade::MeshData{MeshPrimitive::LineLoop, 0}));

    Containers::Array<char> vertexData{segments*sizeof(Vector2)};
    auto positions = Containers::arrayCast<Vector2>(vertexData);

    /* Points on circle */
    const Rad angleIncrement(Constants::tau()/segments);
    for(UnsignedInt i = 0; i != segments; ++i) {
        const Rad angle(Float(i)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);
        positions[i] = {sincos.second, sincos.first};
    }

    return Trade::MeshData{MeshPrimitive::LineLoop, std::move(vertexData),
        Trade::meshAttributeDataNonOwningArray(AttributeData2D), UnsignedInt(positions.size())};
}

Trade::MeshData circle3DSolid(const UnsignedInt segments, const Circle3DFlags flags) {
    CORRADE_ASSERT(segments >= 3, "Primitives::circle3DSolid(): segments must be >= 3",
        (Trade::MeshData{MeshPrimitive::TriangleFan, 0}));

    /* Calculate attribute count and vertex size */
    std::size_t stride = sizeof(Vector3) + sizeof(Vector3);
    std::size_t attributeCount = 2;
    if(flags & Circle3DFlag::Tangents) {
        stride += sizeof(Vector4);
        ++attributeCount;
    }
    if(flags & Circle3DFlag::TextureCoordinates) {
        stride += sizeof(Vector2);
        ++attributeCount;
    }

    /* Set up the layout */
    Containers::Array<char> vertexData{Containers::NoInit, (segments + 2)*stride};
    Containers::Array<Trade::MeshAttributeData> attributeData{attributeCount};
    std::size_t attributeIndex = 0;
    std::size_t attributeOffset = 0;

    Containers::StridedArrayView1D<Vector3> positions{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + attributeOffset),
        segments + 2, std::ptrdiff_t(stride)};
    attributeData[attributeIndex++] = Trade::MeshAttributeData{
        Trade::MeshAttribute::Position, positions};
    attributeOffset += sizeof(Vector3);

    Containers::StridedArrayView1D<Vector3> normals{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + attributeOffset),
        segments + 2, std::ptrdiff_t(stride)};
    attributeData[attributeIndex++] = Trade::MeshAttributeData{
        Trade::MeshAttribute::Normal, normals};
    attributeOffset += sizeof(Vector3);

    Containers::StridedArrayView1D<Vector4> tangents;
    if(flags & Circle3DFlag::Tangents) {
        tangents = Containers::StridedArrayView1D<Vector4>{vertexData,
            reinterpret_cast<Vector4*>(vertexData.data() + attributeOffset),
            segments + 2, std::ptrdiff_t(stride)};
        attributeData[attributeIndex++] = Trade::MeshAttributeData{
            Trade::MeshAttribute::Tangent, tangents};
        attributeOffset += sizeof(Vector4);
    }

    Containers::StridedArrayView1D<Vector2> textureCoordinates;
    if(flags & Circle3DFlag::TextureCoordinates) {
        textureCoordinates = Containers::StridedArrayView1D<Vector2>{vertexData,
            reinterpret_cast<Vector2*>(vertexData.data() + attributeOffset),
            segments + 2, std::ptrdiff_t(stride)};
        attributeData[attributeIndex++] = Trade::MeshAttributeData{
            Trade::MeshAttribute::TextureCoordinates, textureCoordinates};
        attributeOffset += sizeof(Vector2);
    }

    CORRADE_INTERNAL_ASSERT(attributeIndex == attributeCount);
    CORRADE_INTERNAL_ASSERT(attributeOffset == stride);

    /* Fill the data. First is center, the first/last point on the edge is
       twice to close the circle properly. */
    positions[0] = {};
    normals[0] = {0.0f, 0.0f, 1.0f};
    if(flags & Circle3DFlag::Tangents)
        tangents[0] = {1.0f, 0.0f, 0.0f, 1.0f};
    if(flags & Circle3DFlag::TextureCoordinates)
        textureCoordinates[0] = {0.5f, 0.5f};
    const Rad angleIncrement(Constants::tau()/segments);
    for(UnsignedInt i = 1; i != segments + 2; ++i) {
        const Rad angle(Float(i - 1)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);

        positions[i] = {sincos.second, sincos.first, 0.0f};
        normals[i] = {0.0f, 0.0f, 1.0f};
        if(flags & Circle3DFlag::Tangents)
            tangents[i] = {1.0f, 0.0f, 0.0f, 1.0f};
        if(flags & Circle3DFlag::TextureCoordinates)
            textureCoordinates[i] = positions[i].xy()*0.5f + Vector2{0.5f};
    }

    return Trade::MeshData{MeshPrimitive::TriangleFan,
        std::move(vertexData), std::move(attributeData)};
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Trade::MeshData circle3DSolid(const UnsignedInt segments, const CircleTextureCoords textureCoords) {
    return circle3DSolid(segments, textureCoords == CircleTextureCoords::Generate ?
        Circle3DFlag::TextureCoordinates : Circle3DFlags{});
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

namespace {

constexpr Trade::MeshAttributeData AttributeData3DWireframe[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3,
        0, 0, sizeof(Vector3)}
};

}

Trade::MeshData circle3DWireframe(const UnsignedInt segments) {
    CORRADE_ASSERT(segments >= 3, "Primitives::circle3DWireframe(): segments must be >= 3",
        (Trade::MeshData{MeshPrimitive::LineLoop, 0}));

    Containers::Array<char> vertexData{segments*sizeof(Vector3)};
    auto positions = Containers::arrayCast<Vector3>(vertexData);

    /* Points on circle */
    const Rad angleIncrement(Constants::tau()/segments);
    for(UnsignedInt i = 0; i != segments; ++i) {
        const Rad angle(Float(i)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);
        positions[i] = {sincos.second, sincos.first, 0.0f};
    }

    return Trade::MeshData{MeshPrimitive::LineLoop, std::move(vertexData),
        Trade::meshAttributeDataNonOwningArray(AttributeData3DWireframe), UnsignedInt(positions.size())};
}

}}
