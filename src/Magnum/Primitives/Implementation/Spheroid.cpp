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

#include "Spheroid.h"

#include <Corrade/Containers/GrowableArray.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Mesh.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/ArrayAllocator.h"

namespace Magnum { namespace Primitives { namespace Implementation {

Spheroid::Spheroid(UnsignedInt segments, Flags flags): _segments(segments), _flags{flags} {}

namespace {

struct Vertex {
    Vector3 position;
    Vector3 normal;
};

struct VertexTextureCoords {
    Vector3 position;
    Vector3 normal;
    Vector2 textureCoords;
};

}

/** @todo gah this is fugly, any idea how to do this less awful? expose
    arrayGrow? also, with current growth strategy this might realloc too much
    at the beginning since the growth is optimized for adding a single
    element */
void Spheroid::append(const Vector3& position, const Vector3& normal, const Vector2& textureCoords) {
    if(_flags & Flag::TextureCoordinates) {
        const VertexTextureCoords v[]{{position, normal, textureCoords}};
        Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData,
            Containers::arrayCast<const char>(Containers::arrayView(v)));
    } else {
        const Vertex v[]{{position, normal}};
        Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData,
            Containers::arrayCast<const char>(Containers::arrayView(v)));
    }
}

void Spheroid::setLastVertexTextureCoords(const Vector2& textureCoords) {
    /* Assuming append() was called before */
    Containers::arrayCast<VertexTextureCoords>(_vertexData).back().textureCoords = textureCoords;
}

void Spheroid::capVertex(Float y, Float normalY, Float textureCoordsV) {
    append({0.0f, y, 0.0f}, {0.0f, normalY, 0.0f});
    if(_flags & Flag::TextureCoordinates)
        setLastVertexTextureCoords({0.5f, textureCoordsV});
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

            if(_flags & Flag::TextureCoordinates)
                setLastVertexTextureCoords({j*1.0f/_segments, startTextureCoordsV + i*textureCoordsVIncrement});
        }

        /* Duplicate first segment in the ring for additional vertex for texture coordinate */
        if(_flags & Flag::TextureCoordinates) {
            /* This view will become dangling right after append() */
            auto typedVertices = Containers::arrayCast<VertexTextureCoords>(_vertexData);
            append(typedVertices[typedVertices.size()-_segments].position,
                   typedVertices[typedVertices.size()-_segments].normal,
                   {1.0f, startTextureCoordsV + i*textureCoordsVIncrement});
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

            if(_flags & Flag::TextureCoordinates)
                setLastVertexTextureCoords({j*1.0f/_segments, startTextureCoordsV + i*textureCoordsVIncrement});
        }

        /* Duplicate first segment in the ring for additional vertex for texture coordinate */
        if(_flags & Flag::TextureCoordinates) {
            /* This view will become dangling right after append() */
            auto typedVertices = Containers::arrayCast<VertexTextureCoords>(_vertexData);
            append(typedVertices[typedVertices.size()-_segments].position,
                   typedVertices[typedVertices.size()-_segments].normal,
                   {1.0f, startTextureCoordsV + i*textureCoordsVIncrement});
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
            (j != _segments-1 || _flags & Flag::TextureCoordinates) ?
            j+2 : 1,

            /* Top left vertex */
            j+1
        });
    }
}

void Spheroid::faceRings(UnsignedInt count, UnsignedInt offset) {
    const UnsignedInt vertexSegments = _segments + (_flags & Flag::TextureCoordinates ? 1 : 0);

    for(UnsignedInt i = 0; i != count; ++i) {
        for(UnsignedInt j = 0; j != _segments; ++j) {
            const UnsignedInt bottomLeft = i*vertexSegments+j+offset;
            const UnsignedInt bottomRight = ((j != _segments-1 || _flags & Flag::TextureCoordinates) ?
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
    const UnsignedInt vertexSegments = _segments + (_flags & Flag::TextureCoordinates ? 1 : 0);

    UnsignedInt vertexCount;
    if(_flags & Flag::TextureCoordinates)
        vertexCount = _vertexData.size()/sizeof(VertexTextureCoords);
    else
        vertexCount = _vertexData.size()/sizeof(Vertex);

    for(UnsignedInt j = 0; j != _segments; ++j) {
        Containers::arrayAppend<Trade::ArrayAllocator>(_indexData, {
            /* Bottom left vertex */
            vertexCount - vertexSegments + j - 1,

            /* Bottom right vertex */
            (j != _segments-1 || _flags & Flag::TextureCoordinates) ?
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

        if(_flags & Flag::TextureCoordinates)
            setLastVertexTextureCoords({i*1.0f/_segments, textureCoordsV});
    }

    /* Duplicate first segment in the ring for additional vertex for texture coordinate */
    if(_flags & Flag::TextureCoordinates) {
        /* This view will become dangling right after append() */
        auto typedVertices = Containers::arrayCast<VertexTextureCoords>(_vertexData);
        append(typedVertices[typedVertices.size()-_segments].position,
               normal,
               {1.0f, textureCoordsV});
    }
}

namespace {

constexpr Trade::MeshAttributeData AttributeData[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3,
        offsetof(Vertex, position), 0, sizeof(Vertex)},
    Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3,
        offsetof(Vertex, normal), 0, sizeof(Vertex)}
};

constexpr Trade::MeshAttributeData AttributeDataTextureCoords[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3,
        offsetof(VertexTextureCoords, position), 0, sizeof(VertexTextureCoords)},
    Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3,
        offsetof(VertexTextureCoords, normal), 0, sizeof(VertexTextureCoords)},
    Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2,
        offsetof(VertexTextureCoords, textureCoords), 0, sizeof(VertexTextureCoords)}
};

}

Trade::MeshData Spheroid::finalize() {
    Trade::MeshIndexData indices{_indexData};

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(_flags & Flag::TextureCoordinates)
        attributes = Trade::meshAttributeDataNonOwningArray(AttributeDataTextureCoords);
    else
        attributes = Trade::meshAttributeDataNonOwningArray(AttributeData);
    const UnsignedInt vertexCount = _vertexData.size()/attributes[0].stride();

    return Trade::MeshData{MeshPrimitive::Triangles,
        Containers::arrayAllocatorCast<char, Trade::ArrayAllocator>(std::move(_indexData)),
        indices, std::move(_vertexData), std::move(attributes), vertexCount};
}

}}}
