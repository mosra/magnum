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

#include "Circle.h"

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Mesh.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

Trade::MeshData circle2DSolid(const UnsignedInt segments, const CircleTextureCoords textureCoords) {
    CORRADE_ASSERT(segments >= 3, "Primitives::circle2DSolid(): segments must be >= 3",
        (Trade::MeshData{MeshPrimitive::TriangleFan, 0}));

    /* Allocate interleaved array for all vertex data */
    std::size_t stride = sizeof(Vector2);
    std::size_t attributeCount = 1;
    if(textureCoords == CircleTextureCoords::Generate) {
        ++attributeCount;
        stride += sizeof(Vector2);
    }
    Containers::Array<char> vertexData{stride*(segments + 2)};
    Containers::Array<Trade::MeshAttributeData> attributes{attributeCount};

    /* Fill positions */
    Containers::StridedArrayView1D<Vector2> positions{vertexData,
        reinterpret_cast<Vector2*>(vertexData.begin()),
        segments + 2, std::ptrdiff_t(stride)};
    attributes[0] =
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions};
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
    if(textureCoords == CircleTextureCoords::Generate) {
        Containers::StridedArrayView1D<Vector2> textureCoords{vertexData,
            reinterpret_cast<Vector2*>(vertexData.begin() + sizeof(Vector2)),
            positions.size(), std::ptrdiff_t(stride)};
        attributes[1] =
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, textureCoords};
        for(std::size_t i = 0; i != positions.size(); ++i)
            textureCoords[i] = positions[i]*0.5f + Vector2{0.5f};
    }

    return Trade::MeshData{MeshPrimitive::TriangleFan, std::move(vertexData), std::move(attributes)};
}

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

    return Trade::MeshData{MeshPrimitive::LineLoop, std::move(vertexData), {Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions}}};
}

Trade::MeshData circle3DSolid(const UnsignedInt segments, CircleTextureCoords textureCoords) {
    CORRADE_ASSERT(segments >= 3, "Primitives::circle3DSolid(): segments must be >= 3",
        (Trade::MeshData{MeshPrimitive::TriangleFan, 0}));

    /* Allocate interleaved array for all vertex data */
    std::size_t stride = 2*sizeof(Vector3);
    std::size_t attributeCount = 2;
    if(textureCoords == CircleTextureCoords::Generate) {
        ++attributeCount;
        stride += sizeof(Vector2);
    }
    Containers::Array<char> vertexData{stride*(segments + 2)};
    Containers::Array<Trade::MeshAttributeData> attributes{attributeCount};

    /* Fill positions */
    Containers::StridedArrayView1D<Vector3> positions{vertexData,
        reinterpret_cast<Vector3*>(vertexData.begin()),
        segments + 2, std::ptrdiff_t(stride)};
    attributes[0] =
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions};
    positions[0] = {};
    /* Points on the circle. The first/last point is here twice to close the
       circle properly. */
    const Rad angleIncrement(Constants::tau()/segments);
    for(UnsignedInt i = 0; i != segments + 1; ++i) {
        const Rad angle(Float(i)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);
        positions[i + 1] = {sincos.second, sincos.first, 0.0f};
    }

    /* Fill normals */
    Containers::StridedArrayView1D<Vector3> normals{vertexData,
        reinterpret_cast<Vector3*>(vertexData.begin() + sizeof(Vector3)),
        segments + 2, std::ptrdiff_t(stride)};
    attributes[1] =
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, normals};
    for(Vector3& normal: normals) normal = Vector3::zAxis(1.0f);

    /* Fill texture coords, if any */
    if(textureCoords == CircleTextureCoords::Generate) {
        Containers::StridedArrayView1D<Vector2> textureCoords{vertexData,
            reinterpret_cast<Vector2*>(vertexData.begin() + 2*sizeof(Vector3)),
            positions.size(), std::ptrdiff_t(stride)};
        attributes[2] =
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, textureCoords};
        for(std::size_t i = 0; i != positions.size(); ++i)
            textureCoords[i] = positions[i].xy()*0.5f + Vector2{0.5f};
    }

    return Trade::MeshData{MeshPrimitive::TriangleFan, std::move(vertexData), std::move(attributes)};
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

    return Trade::MeshData{MeshPrimitive::LineLoop, std::move(vertexData), {Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions}}};
}

}}
