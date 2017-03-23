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

#include "Cylinder.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Primitives/Implementation/Spheroid.h"
#include "Magnum/Primitives/Implementation/WireframeSpheroid.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

Trade::MeshData3D Cylinder::solid(const UnsignedInt rings, const UnsignedInt segments, const Float halfLength, const Flags flags) {
    CORRADE_ASSERT(rings >= 1 && segments >= 3, "Primitives::Cylinder::solid(): cylinder must have at least one ring and three segments",
        (Trade::MeshData3D{MeshPrimitive::Triangles, {}, {}, {}, {}, {}, nullptr}));

    Implementation::Spheroid cylinder(segments, flags & Flag::GenerateTextureCoords ? Implementation::Spheroid::TextureCoords::Generate : Implementation::Spheroid::TextureCoords::DontGenerate);

    const Float length = 2.0f*halfLength;
    const Float textureCoordsV = flags & Flag::CapEnds ? 1.0f/(length+2.0f) : 0.0f;

    /* Bottom cap */
    if(flags & Flag::CapEnds) {
        cylinder.capVertex(-halfLength, -1.0f, 0.0f);
        cylinder.capVertexRing(-halfLength, textureCoordsV, Vector3::yAxis(-1.0f));
    }

    /* Vertex rings */
    cylinder.cylinderVertexRings(rings+1, -halfLength, length/rings, textureCoordsV, length/(rings*(flags & Flag::CapEnds ? length + 2.0f : length)));

    /* Top cap */
    if(flags & Flag::CapEnds) {
        cylinder.capVertexRing(halfLength, 1.0f - textureCoordsV, Vector3::yAxis(1.0f));
        cylinder.capVertex(halfLength, 1.0f, 1.0f);
    }

    /* Faces */
    if(flags & Flag::CapEnds) cylinder.bottomFaceRing();
    cylinder.faceRings(rings, flags & Flag::CapEnds ? (1 + segments) : 0);
    if(flags & Flag::CapEnds) cylinder.topFaceRing();

    return cylinder.finalize();
}

Trade::MeshData3D Cylinder::wireframe(const UnsignedInt rings, const UnsignedInt segments, const Float halfLength) {
    CORRADE_ASSERT(rings >= 1 && segments >= 4 && segments%4 == 0, "Primitives::Cylinder::wireframe(): improper parameters",
        (Trade::MeshData3D{MeshPrimitive::Lines, {}, {}, {}, {}, {}, nullptr}));

    Implementation::WireframeSpheroid cylinder(segments/4);

    const Float increment = 2*halfLength/rings;

    /* Rings */
    cylinder.ring(-halfLength);
    for(UnsignedInt i = 0; i != rings; ++i) {
        cylinder.cylinder();
        cylinder.ring(-halfLength + (i+1)*increment);
    }

    return cylinder.finalize();
}

}}
