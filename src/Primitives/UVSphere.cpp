/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "UVSphere.h"

using namespace std;

namespace Magnum { namespace Primitives {

UVSphere::UVSphere(unsigned int rings, unsigned int segments, TextureCoords textureCoords): Capsule(segments, textureCoords) {
    CORRADE_ASSERT(rings >= 2 && segments >= 3, "UVSphere must have at least two rings and three segments", );

    GLfloat textureCoordsVIncrement = 1.0f/rings;
    GLfloat ringAngleIncrement = Math::Constants<GLfloat>::pi()/rings;

    /* Bottom cap vertex */
    capVertex(-1.0f, -1.0f, 0.0f);

    /* Vertex rings */
    hemisphereVertexRings(rings-1, 0.0f, -Math::Constants<GLfloat>::pi()/2+ringAngleIncrement, ringAngleIncrement, textureCoordsVIncrement, textureCoordsVIncrement);

    /* Top cap vertex */
    capVertex(1.0f, 1.0f, 1.0f);

    /* Faces */
    bottomFaceRing();
    faceRings(rings-2);
    topFaceRing();
}

}}
