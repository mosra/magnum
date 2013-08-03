/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Math/Vector3.h"
#include "Primitives/Implementation/Spheroid.h"
#include "Primitives/Implementation/WireframeSpheroid.h"
#include "Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

Trade::MeshData3D Capsule::solid(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, UnsignedInt segments, Float length, TextureCoords textureCoords) {
    CORRADE_ASSERT(hemisphereRings >= 1 && cylinderRings >= 1 && segments >= 3, "Capsule must have at least one hemisphere ring, one cylinder ring and three segments", Trade::MeshData3D(Mesh::Primitive::Triangles, {}, {}, {}, {}));

    Implementation::Spheroid capsule(segments, textureCoords == TextureCoords::Generate ?
        Implementation::Spheroid::TextureCoords::Generate :
        Implementation::Spheroid::TextureCoords::DontGenerate);

    Float height = 2.0f+length;
    Float hemisphereTextureCoordsVIncrement = 1.0f/(hemisphereRings*height);
    Rad hemisphereRingAngleIncrement(Constants::pi()/(2*hemisphereRings));

    /* Bottom cap vertex */
    capsule.capVertex(-height/2, -1.0f, 0.0f);

    /* Rings of bottom hemisphere */
    capsule.hemisphereVertexRings(hemisphereRings-1, -length/2, -Rad(Constants::pi())/2+hemisphereRingAngleIncrement, hemisphereRingAngleIncrement, hemisphereTextureCoordsVIncrement, hemisphereTextureCoordsVIncrement);

    /* Rings of cylinder */
    capsule.cylinderVertexRings(cylinderRings+1, -length/2, length/cylinderRings, 1.0f/height, length/(cylinderRings*height));

    /* Rings of top hemisphere */
    capsule.hemisphereVertexRings(hemisphereRings-1, length/2, hemisphereRingAngleIncrement, hemisphereRingAngleIncrement, (1.0f + length)/height+hemisphereTextureCoordsVIncrement, hemisphereTextureCoordsVIncrement);

    /* Top cap vertex */
    capsule.capVertex(height/2, 1.0f, 1.0f);

    /* Faces */
    capsule.bottomFaceRing();
    capsule.faceRings(hemisphereRings*2-2+cylinderRings);
    capsule.topFaceRing();

    return capsule.finalize();
}

Trade::MeshData3D Capsule::wireframe(const UnsignedInt hemisphereRings, const UnsignedInt cylinderRings, const UnsignedInt segments, const Float length) {
    CORRADE_ASSERT(hemisphereRings >= 1 && cylinderRings >= 1 && segments >= 4 && segments%4 == 0, "Primitives::Capsule::wireframe(): improper parameters", Trade::MeshData3D(Mesh::Primitive::Lines, {}, {}, {}, {}));

    Implementation::WireframeSpheroid capsule(segments/4);

    /* Bottom hemisphere */
    capsule.bottomHemisphere(-length/2, hemisphereRings);

    /* Cylinder */
    capsule.ring(-length/2);
    for(UnsignedInt i = 0; i != cylinderRings; ++i) {
        capsule.cylinder();
        capsule.ring(-length/2 + (i+1)*(length/cylinderRings));
    }

    /* Top hemisphere */
    capsule.topHemisphere(length/2, hemisphereRings);

    return capsule.finalize();
}

}}
