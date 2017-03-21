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

#include "Capsule.h"

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Mesh.h"
#include "Magnum/Primitives/Implementation/Spheroid.h"
#include "Magnum/Primitives/Implementation/WireframeSpheroid.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

Trade::MeshData2D Capsule2D::wireframe(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, Float halfLength) {
    CORRADE_ASSERT(hemisphereRings >= 1 && cylinderRings >= 1, "Capsule must have at least one hemisphere ring, one cylinder ring and three segments",
        (Trade::MeshData2D{MeshPrimitive::Triangles, {}, {}, {}, {}, nullptr}));

    std::vector<Vector2> positions;
    positions.reserve(hemisphereRings*4+2+(cylinderRings-1)*2);
    const Rad angleIncrement(Constants::piHalf()/hemisphereRings);
    const Float cylinderIncrement = 2.0f*halfLength/cylinderRings;

    /* Bottom cap vertex */
    positions.emplace_back(0.0f, -halfLength-1.0f);

    /* Bottom hemisphere */
    for(UnsignedInt i = 0; i != hemisphereRings; ++i) {
        const Rad angle(Float(i+1)*angleIncrement);
        const Float x = Math::sin(angle);
        const Float y = -Math::cos(angle)-halfLength;
        positions.insert(positions.end(), {{-x, y}, {x, y}});
    }

    /* Cylinder (bottom and top vertices are done within caps */
    for(UnsignedInt i = 0; i != cylinderRings-1; ++i) {
        const Float y = (i+1)*cylinderIncrement-halfLength;
        positions.insert(positions.end(), {{-1.0f, y}, {1.0f, y}});
    }

    /* Top hemisphere */
    for(UnsignedInt i = 0; i != hemisphereRings; ++i) {
        const Rad angle(Float(i)*angleIncrement);
        const Float x = Math::cos(angle);
        const Float y = Math::sin(angle)+halfLength;
        positions.insert(positions.end(), {{-x, y}, {x, y}});
    }

    /* Top cap vertex */
    positions.emplace_back(0.0f, halfLength+1.0f);

    std::vector<UnsignedInt> indices;
    indices.reserve(hemisphereRings*8+cylinderRings*4);

    /* Bottom cap indices */
    indices.insert(indices.end(), {0, 1, 0, 2});

    /* Side indices */
    for(UnsignedInt i = 0; i != cylinderRings+hemisphereRings*2-2; ++i)
        indices.insert(indices.end(), {i*2+1, i*2+3,
                                       i*2+2, i*2+4});

    /* Top cap indices */
    indices.insert(indices.end(),
        {UnsignedInt(positions.size())-3, UnsignedInt(positions.size())-1,
         UnsignedInt(positions.size())-2, UnsignedInt(positions.size())-1});

    return Trade::MeshData2D{MeshPrimitive::Lines, std::move(indices), {std::move(positions)}, {}, {}, nullptr};
}

Trade::MeshData3D Capsule3D::solid(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, UnsignedInt segments, Float halfLength, TextureCoords textureCoords) {
    CORRADE_ASSERT(hemisphereRings >= 1 && cylinderRings >= 1 && segments >= 3, "Capsule must have at least one hemisphere ring, one cylinder ring and three segments",
        (Trade::MeshData3D{MeshPrimitive::Triangles, {}, {}, {}, {}, {}, nullptr}));

    Implementation::Spheroid capsule(segments, textureCoords == TextureCoords::Generate ?
        Implementation::Spheroid::TextureCoords::Generate :
        Implementation::Spheroid::TextureCoords::DontGenerate);

    Float height = 2.0f+2.0f*halfLength;
    Float hemisphereTextureCoordsVIncrement = 1.0f/(hemisphereRings*height);
    Rad hemisphereRingAngleIncrement(Constants::piHalf()/hemisphereRings);

    /* Bottom cap vertex */
    capsule.capVertex(-height/2, -1.0f, 0.0f);

    /* Rings of bottom hemisphere */
    capsule.hemisphereVertexRings(hemisphereRings-1, -halfLength, -Rad(Constants::piHalf())+hemisphereRingAngleIncrement, hemisphereRingAngleIncrement, hemisphereTextureCoordsVIncrement, hemisphereTextureCoordsVIncrement);

    /* Rings of cylinder */
    capsule.cylinderVertexRings(cylinderRings+1, -halfLength, 2.0f*halfLength/cylinderRings, 1.0f/height, 2.0f*halfLength/(cylinderRings*height));

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

Trade::MeshData3D Capsule3D::wireframe(const UnsignedInt hemisphereRings, const UnsignedInt cylinderRings, const UnsignedInt segments, const Float halfLength) {
    CORRADE_ASSERT(hemisphereRings >= 1 && cylinderRings >= 1 && segments >= 4 && segments%4 == 0, "Primitives::Capsule::wireframe(): improper parameters",
        (Trade::MeshData3D{MeshPrimitive::Triangles, {}, {}, {}, {}, {}, nullptr}));

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
