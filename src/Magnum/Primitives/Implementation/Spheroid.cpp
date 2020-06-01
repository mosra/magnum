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

#include "Spheroid.h"

#include <Corrade/Containers/GrowableArray.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Mesh.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/ArrayAllocator.h"

namespace Magnum { namespace Primitives { namespace Implementation {

Spheroid::Spheroid(UnsignedInt segments, Flags flags): _segments(segments), _flags{flags}, _stride{sizeof(Vector3) + sizeof(Vector3)}, _attributeCount{2} {
    if(_flags & Flag::Tangents) {
        _tangentOffset = _stride;
        _stride += sizeof(Vector4);
        ++_attributeCount;
    } else _tangentOffset = ~std::size_t{};

    if(_flags & Flag::TextureCoordinates) {
        _textureCoordinateOffset = _stride;
        _stride += sizeof(Vector2);
        ++_attributeCount;
    } else _textureCoordinateOffset = ~std::size_t{};
}

void Spheroid::append(const Vector3& position, const Vector3& normal) {
    Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData,
        Containers::arrayCast<const char>(Containers::arrayView(&position, 1)));
    Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData,
        Containers::arrayCast<const char>(Containers::arrayView(&normal, 1)));
    if(_flags & Flag::Tangents) {
        /** @todo make arrayGrow() a public API instead of this */
        constexpr const char empty[sizeof(Vector4)]{};
        Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData,
            Containers::arrayView(empty));
    }
    if(_flags & Flag::TextureCoordinates) {
        /** @todo make arrayGrow() a public API instead */
        constexpr const char empty[sizeof(Vector2)]{};
        Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData,
            Containers::arrayView(empty));
    }
}

Vector3 Spheroid::lastVertexPosition(const std::size_t offsetFromEnd) {
    return Containers::arrayCast<Vector3>(_vertexData.slice<sizeof(Vector3)>(_vertexData.size() - _stride*offsetFromEnd))[0];
}

Vector3 Spheroid::lastVertexNormal(const std::size_t offsetFromEnd) {
    return Containers::arrayCast<Vector3>(_vertexData.slice<sizeof(Vector3)>(_vertexData.size() - _stride*offsetFromEnd + sizeof(Vector3)))[0];
}

Vector4& Spheroid::lastVertexTangent(const std::size_t offsetFromEnd) {
    return Containers::arrayCast<Vector4>(_vertexData.slice<sizeof(Vector4)>(_vertexData.size() - _stride*offsetFromEnd + _tangentOffset))[0];
}

Vector2& Spheroid::lastVertexTextureCoords(const std::size_t offsetFromEnd) {
    return Containers::arrayCast<Vector2>(_vertexData.slice<sizeof(Vector2)>(_vertexData.size() - _stride*offsetFromEnd + _textureCoordinateOffset))[0];
}

void Spheroid::capVertex(Float y, Float normalY, Float textureCoordsV) {
    append({0.0f, y, 0.0f}, {0.0f, normalY, 0.0f});
    if(_flags & Flag::Tangents)
        lastVertexTangent(1) = {normalY > 0.0f ? 1.0f : -1.0f, 0.0f, 0.0f, 1.0f};
    if(_flags & Flag::TextureCoordinates)
        lastVertexTextureCoords(1) = {0.5f, textureCoordsV};
}

void Spheroid::hemisphereVertexRings(UnsignedInt count, Float centerY, Rad startRingAngle, Rad ringAngleIncrement, Float startTextureCoordsV, Float textureCoordsVIncrement) {
    const Rad segmentAngleIncrement(Constants::tau()/_segments);
    for(UnsignedInt i = 0; i != count; ++i) {
        const Rad ringAngle = startRingAngle + Float(i)*ringAngleIncrement;
        const std::pair<Float, Float> ringSinCos = Math::sincos(ringAngle);
        const Float x = ringSinCos.second;
        const Float z = ringSinCos.second;
        const Float y = ringSinCos.first;

        for(UnsignedInt j = 0; j != _segments; ++j) {
            const Rad segmentAngle = Float(j)*segmentAngleIncrement;
            const std::pair<Float, Float> segmentSinCos = Math::sincos(segmentAngle);
            append({x*segmentSinCos.first, centerY+y, z*segmentSinCos.second},
                   {x*segmentSinCos.first, y, z*segmentSinCos.second});

            if(_flags & Flag::Tangents)
                lastVertexTangent(1) = {segmentSinCos.second, 0.0f, -segmentSinCos.first, 1.0f};
            if(_flags & Flag::TextureCoordinates)
                lastVertexTextureCoords(1) = {j*1.0f/_segments, startTextureCoordsV + i*textureCoordsVIncrement};
        }

        /* Duplicate first segment in the ring for additional vertex for
           texture coordinate */
        if(_flags & (Flag::TextureCoordinates|Flag::Tangents)) {
            append(lastVertexPosition(_segments), lastVertexNormal(_segments));
            if(_flags & Flag::Tangents)
                lastVertexTangent(1) = lastVertexTangent(_segments + 1);
            if(_flags & Flag::TextureCoordinates)
                lastVertexTextureCoords(1) = {1.0f, startTextureCoordsV + i*textureCoordsVIncrement};
        }
    }
}

void Spheroid::cylinderVertexRings(const UnsignedInt count, const Float startY, const Vector2& increment, const Float startTextureCoordsV, const Float textureCoordsVIncrement) {
    const Vector2 baseNormal = -increment.perpendicular().normalized();
    Vector2 base = {1.0f, startY};

    const Rad segmentAngleIncrement(Constants::tau()/_segments);
    for(UnsignedInt i = 0; i != count; ++i) {
        for(UnsignedInt j = 0; j != _segments; ++j) {
            const Rad segmentAngle = Float(j)*segmentAngleIncrement;
            const std::pair<Float, Float> segmentSinCos = Math::sincos(segmentAngle);
            append({base.x()*segmentSinCos.first, base.y(), base.x()*segmentSinCos.second},
                   {baseNormal.x()*segmentSinCos.first, baseNormal.y(), baseNormal.x()*segmentSinCos.second});

            if(_flags & Flag::Tangents)
                lastVertexTangent(1) = {segmentSinCos.second, 0.0f, -segmentSinCos.first, 1.0f};
            if(_flags & Flag::TextureCoordinates)
                lastVertexTextureCoords(1) = {j*1.0f/_segments, startTextureCoordsV + i*textureCoordsVIncrement};
        }

        /* Duplicate first segment in the ring for additional vertex for texture coordinate */
        if(_flags & (Flag::TextureCoordinates|Flag::Tangents)) {
            append(lastVertexPosition(_segments), lastVertexNormal(_segments));
            if(_flags & Flag::Tangents)
                lastVertexTangent(1) = lastVertexTangent(_segments + 1);
            if(_flags & Flag::TextureCoordinates)
                lastVertexTextureCoords(1) = {1.0f, startTextureCoordsV + i*textureCoordsVIncrement};
        }

        base += increment;
    }
}

void Spheroid::bottomFaceRing() {
    for(UnsignedInt j = 0; j != _segments; ++j) {
        Containers::arrayAppend<Trade::ArrayAllocator>(_indexData, {
            /* Bottom vertex */
            0u,

            /* Top right vertex */
            (j != _segments-1 || _flags & (Flag::TextureCoordinates|Flag::Tangents)) ?
            j+2 : 1,

            /* Top left vertex */
            j+1
        });
    }
}

void Spheroid::faceRings(UnsignedInt count, UnsignedInt offset) {
    const UnsignedInt vertexSegments = _segments + (_flags & (Flag::TextureCoordinates|Flag::Tangents) ? 1 : 0);

    for(UnsignedInt i = 0; i != count; ++i) {
        for(UnsignedInt j = 0; j != _segments; ++j) {
            const UnsignedInt bottomLeft = i*vertexSegments+j+offset;
            const UnsignedInt bottomRight = ((j != _segments-1 || _flags & (Flag::TextureCoordinates|Flag::Tangents)) ?
                i*vertexSegments+j+1+offset : i*_segments+offset);
            const UnsignedInt topLeft = bottomLeft+vertexSegments;
            const UnsignedInt topRight = bottomRight+vertexSegments;

            Containers::arrayAppend<Trade::ArrayAllocator>(_indexData, {
                bottomLeft,
                bottomRight,
                topRight,
                bottomLeft,
                topRight,
                topLeft
            });
        }
    }
}

void Spheroid::topFaceRing() {
    const UnsignedInt vertexSegments = _segments + (_flags & (Flag::TextureCoordinates|Flag::Tangents) ? 1 : 0);

    const UnsignedInt vertexCount = _vertexData.size()/_stride;

    for(UnsignedInt j = 0; j != _segments; ++j) {
        Containers::arrayAppend<Trade::ArrayAllocator>(_indexData, {
            /* Bottom left vertex */
            vertexCount - vertexSegments + j - 1,

            /* Bottom right vertex */
            (j != _segments-1 || _flags & (Flag::TextureCoordinates|Flag::Tangents)) ?
            vertexCount - vertexSegments + j : vertexCount - _segments - 1,

            /* Top vertex */
            vertexCount - 1
        });
    }
}

void Spheroid::capVertexRing(Float y, Float textureCoordsV, const Vector3& normal) {
    const Rad segmentAngleIncrement(Constants::tau()/_segments);

    for(UnsignedInt i = 0; i != _segments; ++i) {
        const Rad segmentAngle = Float(i)*segmentAngleIncrement;
        const std::pair<Float, Float> segmentSinCos = Math::sincos(segmentAngle);
        append({segmentSinCos.first, y, segmentSinCos.second}, normal);

        if(_flags & Flag::Tangents)
            lastVertexTangent(1) = {segmentSinCos.second, 0.0f, -segmentSinCos.first, 1.0f};
        if(_flags & Flag::TextureCoordinates)
            lastVertexTextureCoords(1) = {i*1.0f/_segments, textureCoordsV};
    }

    /* Duplicate first segment in the ring for additional vertex for texture
       coordinate */
    if(_flags & (Flag::TextureCoordinates|Flag::Tangents)) {
        append(lastVertexPosition(_segments), normal);
        if(_flags & Flag::Tangents)
            lastVertexTangent(1) = lastVertexTangent(_segments + 1);
        if(_flags & Flag::TextureCoordinates)
            lastVertexTextureCoords(1) = {1.0f, textureCoordsV};
    }
}

Trade::MeshData Spheroid::finalize() {
    Trade::MeshIndexData indices{_indexData};

    std::size_t attributeOffset = 0;
    Containers::Array<Trade::MeshAttributeData> attributes{_attributeCount};
    attributes[attributeOffset++] = Trade::MeshAttributeData{
        Trade::MeshAttribute::Position, VertexFormat::Vector3,
        /* GCC 4.8 can't handle the stridedArrayView() convenience thing */
        Containers::StridedArrayView1D<const char>{_vertexData,
            _vertexData.data(),
            _vertexData.size()/_stride, std::ptrdiff_t(_stride)}};
    attributes[attributeOffset++] = Trade::MeshAttributeData{
        Trade::MeshAttribute::Normal, VertexFormat::Vector3,
        /* GCC 4.8 can't handle the stridedArrayView() convenience thing */
        Containers::StridedArrayView1D<const char>{_vertexData,
            _vertexData.data() + sizeof(Vector3),
            _vertexData.size()/_stride, std::ptrdiff_t(_stride)}};

    if(_flags & Flag::Tangents)
        attributes[attributeOffset++] = Trade::MeshAttributeData{
            Trade::MeshAttribute::Tangent, VertexFormat::Vector4,
            /* GCC 4.8 can't handle the stridedArrayView() convenience thing */
            Containers::StridedArrayView1D<const char>{_vertexData,
                _vertexData.data() + _tangentOffset,
                _vertexData.size()/_stride, std::ptrdiff_t(_stride)}};
    if(_flags & Flag::TextureCoordinates)
        attributes[attributeOffset++] = Trade::MeshAttributeData{
            Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2,
            /* GCC 4.8 can't handle the stridedArrayView() convenience thing */
            Containers::StridedArrayView1D<const char>{_vertexData,
                _vertexData.data() + _textureCoordinateOffset,
                _vertexData.size()/_stride, std::ptrdiff_t(_stride)}};

    CORRADE_INTERNAL_ASSERT(attributeOffset == _attributeCount);

    return Trade::MeshData{MeshPrimitive::Triangles,
        Containers::arrayAllocatorCast<char, Trade::ArrayAllocator>(std::move(_indexData)),
        indices, std::move(_vertexData), std::move(attributes)};
}

}}}
