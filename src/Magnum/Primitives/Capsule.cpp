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

#include "Capsule.h"

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Mesh.h"
#include "Magnum/Primitives/Implementation/Spheroid.h"
#include "Magnum/Primitives/Implementation/WireframeSpheroid.h"
#include "Magnum/Trade/ArrayAllocator.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

Trade::MeshData capsule2DWireframe(const UnsignedInt hemisphereRings, const UnsignedInt cylinderRings, const Float halfLength) {
    CORRADE_ASSERT(hemisphereRings >= 1 && cylinderRings >= 1,
        "Primitives::capsule2DWireframe(): at least one hemisphere ring and one cylinder ring expected",
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    Containers::Array<Vector2> vertexData;
    Containers::arrayReserve<Trade::ArrayAllocator>(vertexData,
        hemisphereRings*4+2+(cylinderRings-1)*2);
    const Rad angleIncrement(Constants::piHalf()/hemisphereRings);
    const Float cylinderIncrement = 2.0f*halfLength/cylinderRings;

    /* Bottom cap vertex */
    Containers::arrayAppend<Trade::ArrayAllocator>(vertexData,
        {0.0f, -halfLength-1.0f});

    /* Bottom hemisphere */
    for(UnsignedInt i = 0; i != hemisphereRings; ++i) {
        const Rad angle(Float(i+1)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);
        const Float x = sincos.first;
        const Float y = -sincos.second-halfLength;
        Containers::arrayAppend<Trade::ArrayAllocator>(vertexData,
            {{-x, y}, {x, y}});
    }

    /* Cylinder (bottom and top vertices are done within caps) */
    for(UnsignedInt i = 0; i != cylinderRings-1; ++i) {
        const Float y = (i+1)*cylinderIncrement-halfLength;
        Containers::arrayAppend<Trade::ArrayAllocator>(vertexData,
            {{-1.0f, y}, {1.0f, y}});
    }

    /* Top hemisphere */
    for(UnsignedInt i = 0; i != hemisphereRings; ++i) {
        const Rad angle(Float(i)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);
        const Float x = sincos.second;
        const Float y = sincos.first+halfLength;
        Containers::arrayAppend<Trade::ArrayAllocator>(vertexData,
            {{-x, y}, {x, y}});
    }

    /* Top cap vertex */
    Containers::arrayAppend<Trade::ArrayAllocator>(vertexData,
        {0.0f, halfLength+1.0f});

    Containers::Array<UnsignedInt> indexData;
    Containers::arrayReserve<Trade::ArrayAllocator>(indexData,
        hemisphereRings*8+cylinderRings*4);

    /* Bottom cap indices */
    Containers::arrayAppend<Trade::ArrayAllocator>(indexData,
        {0u, 1u, 0u, 2u});

    /* Side indices */
    for(UnsignedInt i = 0; i != cylinderRings+hemisphereRings*2-2; ++i)
        Containers::arrayAppend<Trade::ArrayAllocator>(indexData,
            {i*2+1, i*2+3, i*2+2, i*2+4});

    /* Top cap indices */
    Containers::arrayAppend<Trade::ArrayAllocator>(indexData,
        {UnsignedInt(vertexData.size())-3, UnsignedInt(vertexData.size())-1,
         UnsignedInt(vertexData.size())-2, UnsignedInt(vertexData.size())-1});

    Trade::MeshIndexData indices{indexData};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position, Containers::arrayView(vertexData)};
    return Trade::MeshData{MeshPrimitive::Lines,
        Containers::arrayAllocatorCast<char, Trade::ArrayAllocator>(std::move(indexData)), indices,
        Containers::arrayAllocatorCast<char, Trade::ArrayAllocator>(std::move(vertexData)), {positions}};
}

Trade::MeshData capsule3DSolid(const UnsignedInt hemisphereRings, const UnsignedInt cylinderRings, const UnsignedInt segments, const Float halfLength, const CapsuleFlags flags) {
    CORRADE_ASSERT(hemisphereRings >= 1 && cylinderRings >= 1 && segments >= 3,
        "Primitives::capsule3DSolid(): at least one hemisphere ring, one cylinder ring and three segments expected",
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    Implementation::Spheroid capsule{segments, Implementation::Spheroid::Flag(UnsignedByte(flags))};

    Float height = 2.0f+2.0f*halfLength;
    Float hemisphereTextureCoordsVIncrement = 1.0f/(hemisphereRings*height);
    Rad hemisphereRingAngleIncrement(Constants::piHalf()/hemisphereRings);

    /* Bottom cap vertex */
    capsule.capVertex(-height/2, -1.0f, 0.0f);

    /* Rings of bottom hemisphere */
    capsule.hemisphereVertexRings(hemisphereRings-1, -halfLength, -Rad(Constants::piHalf())+hemisphereRingAngleIncrement, hemisphereRingAngleIncrement, hemisphereTextureCoordsVIncrement, hemisphereTextureCoordsVIncrement);

    /* Rings of cylinder */
    capsule.cylinderVertexRings(cylinderRings+1, -halfLength, {0.0f, 2.0f*halfLength/cylinderRings}, 1.0f/height, 2.0f*halfLength/(cylinderRings*height));

    /* Rings of top hemisphere */
    capsule.hemisphereVertexRings(hemisphereRings-1, halfLength, hemisphereRingAngleIncrement, hemisphereRingAngleIncrement, (1.0f + 2.0f*halfLength)/height+hemisphereTextureCoordsVIncrement, hemisphereTextureCoordsVIncrement);

    /* Top cap vertex */
    capsule.capVertex(height/2, 1.0f, 1.0f);

    /* Faces */
    capsule.bottomFaceRing();
    capsule.faceRings(hemisphereRings*2-2+cylinderRings);
    capsule.topFaceRing();

    return capsule.finalize();
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Trade::MeshData capsule3DSolid(const UnsignedInt hemisphereRings, const UnsignedInt cylinderRings, const UnsignedInt segments, const Float halfLength, const CapsuleTextureCoords textureCoords) {
    return capsule3DSolid(hemisphereRings, cylinderRings, segments, halfLength,
        textureCoords == CapsuleTextureCoords::Generate ?
        CapsuleFlag::TextureCoordinates : CapsuleFlags{});
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

Trade::MeshData capsule3DWireframe(const UnsignedInt hemisphereRings, const UnsignedInt cylinderRings, const UnsignedInt segments, const Float halfLength) {
    CORRADE_ASSERT(hemisphereRings >= 1 && cylinderRings >= 1 && segments >= 4 && segments%4 == 0,
        "Primitives::capsule3DWireframe(): at least one hemisphere and cylinder ring and multiples of 4 segments expected",
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    Implementation::WireframeSpheroid capsule(segments/4);

    /* Bottom hemisphere */
    capsule.bottomHemisphere(-halfLength, hemisphereRings);

    /* Cylinder */
    capsule.ring(-halfLength);
    for(UnsignedInt i = 0; i != cylinderRings; ++i) {
        capsule.cylinder();
        capsule.ring(-halfLength + (i+1)*(2.0f*halfLength/cylinderRings));
    }

    /* Top hemisphere */
    capsule.topHemisphere(halfLength, hemisphereRings);

    return capsule.finalize();
}

}}
