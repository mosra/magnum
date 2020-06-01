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

#include "Cone.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Primitives/Implementation/Spheroid.h"
#include "Magnum/Primitives/Implementation/WireframeSpheroid.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

Trade::MeshData coneSolid(const UnsignedInt rings, const UnsignedInt segments, const Float halfLength, const ConeFlags flags) {
    CORRADE_ASSERT(rings >= 1 && segments >= 3,
        "Primitives::coneSolid(): at least one ring and three segments expected",
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    Implementation::Spheroid cone{segments, Implementation::Spheroid::Flag(UnsignedByte(flags))};

    const Float length = 2.0f*halfLength;
    const Float textureCoordsV = flags & ConeFlag::CapEnd ? 1.0f/(length + 1.0f) : 0.0f;

    /* Bottom cap */
    if(flags & ConeFlag::CapEnd) {
        cone.capVertex(-halfLength, -1.0f, 0.0f);
        cone.capVertexRing(-halfLength, textureCoordsV, Vector3::yAxis(-1.0f));
    }

    /* Vertex rings */
    cone.cylinderVertexRings(rings+1, -halfLength, {-1.0f/rings, length/rings}, textureCoordsV, length/(rings*(flags & ConeFlag::CapEnd ? length + 1.0f : length)));

    /* Faces. Account for the extra vertices for caps and texture coords. */
    if(flags & ConeFlag::CapEnd) cone.bottomFaceRing();
    if(flags & ConeFlag::CapEnd && flags & (ConeFlag::Tangents|ConeFlag::TextureCoordinates))
        cone.faceRings(rings, 2 + segments);
    else if(flags & ConeFlag::CapEnd)
        cone.faceRings(rings, 1 + segments);
    else
        cone.faceRings(rings, 0);

    return cone.finalize();
}

Trade::MeshData coneWireframe(const UnsignedInt segments, const Float halfLength) {
    CORRADE_ASSERT(segments >= 4 && segments%4 == 0,
        "Primitives::coneWireframe(): multiples of 4 segments expected",
        (Trade::MeshData{MeshPrimitive::Lines, 0}));

    Implementation::WireframeSpheroid cone{segments/4};
    cone.ring(-halfLength);
    cone.topHemisphere(halfLength - 1.0f, 1);
    return cone.finalize();
}

}}
