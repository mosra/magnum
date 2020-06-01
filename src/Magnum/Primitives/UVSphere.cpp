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

#include "UVSphere.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Primitives/Implementation/Spheroid.h"
#include "Magnum/Primitives/Implementation/WireframeSpheroid.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

Trade::MeshData uvSphereSolid(const UnsignedInt rings, const UnsignedInt segments, const UVSphereFlags flags) {
    CORRADE_ASSERT(rings >= 2 && segments >= 3,
        "Primitives::uvSphereSolid(): at least two rings and three segments expected",
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    Implementation::Spheroid sphere(segments, Implementation::Spheroid::Flag(UnsignedByte(flags)));

    Float textureCoordsVIncrement = 1.0f/rings;
    Rad ringAngleIncrement(Constants::pi()/rings);

    /* Bottom cap vertex */
    sphere.capVertex(-1.0f, -1.0f, 0.0f);

    /* Vertex rings */
    sphere.hemisphereVertexRings(rings-1, 0.0f, -Rad(Constants::piHalf())+ringAngleIncrement, ringAngleIncrement, textureCoordsVIncrement, textureCoordsVIncrement);

    /* Top cap vertex */
    sphere.capVertex(1.0f, 1.0f, 1.0f);

    /* Faces */
    sphere.bottomFaceRing();
    sphere.faceRings(rings-2);
    sphere.topFaceRing();

    return sphere.finalize();
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Trade::MeshData uvSphereSolid(const UnsignedInt rings, const UnsignedInt segments, const UVSphereTextureCoords textureCoords) {
    return uvSphereSolid(rings, segments, textureCoords == UVSphereTextureCoords::Generate ? UVSphereFlag::TextureCoordinates : UVSphereFlags{});
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

Trade::MeshData uvSphereWireframe(const UnsignedInt rings, const UnsignedInt segments) {
    CORRADE_ASSERT(rings >= 2 && rings%2 == 0 && segments >= 4 && segments%4 == 0,
        "Primitives::uvSphereWireframe(): multiples of 2 rings and multiples of 4 segments expected",
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    Implementation::WireframeSpheroid sphere(segments/4);

    /* Make sphere */
    sphere.bottomHemisphere(0.0f, rings/2);
    sphere.ring(0.0f);
    sphere.topHemisphere(0.0f, rings/2);

    return sphere.finalize();
}

}}
