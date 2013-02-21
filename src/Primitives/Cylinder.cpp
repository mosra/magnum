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

#include "Cylinder.h"

#include "Math/Point3D.h"

namespace Magnum { namespace Primitives {

Cylinder::Cylinder(std::uint32_t rings, std::uint32_t segments, GLfloat length, Flags flags): Capsule(segments, flags & Flag::GenerateTextureCoords ? TextureCoords::Generate : TextureCoords::DontGenerate) {
    CORRADE_ASSERT(rings >= 1 && segments >= 3, "Cylinder must have at least one ring and three segments", );

    GLfloat y = length*0.5f;
    GLfloat textureCoordsV = flags & Flag::CapEnds ? 1.0f/(length+2.0f) : 0.0f;

    /* Bottom cap */
    if(flags & Flag::CapEnds) {
        capVertex(-y, -1.0f, 0.0f);
        capVertexRing(-y, textureCoordsV, Vector3::yAxis(-1.0f));
    }

    /* Vertex rings */
    cylinderVertexRings(rings+1, -y, length/rings, textureCoordsV, length/(rings*(flags & Flag::CapEnds ? length + 2.0f : length)));

    /* Top cap */
    if(flags & Flag::CapEnds) {
        capVertexRing(y, 1.0f - textureCoordsV, Vector3::yAxis(1.0f));
        capVertex(y, 1.0f, 1.0f);
    }

    /* Faces */
    if(flags & Flag::CapEnds) bottomFaceRing();
    faceRings(rings, flags & Flag::CapEnds ? 1 : 0);
    if(flags & Flag::CapEnds) topFaceRing();
}

void Cylinder::capVertexRing(GLfloat y, GLfloat textureCoordsV, const Vector3& normal) {
    GLfloat segmentAngleIncrement = 2*Constants::pi()/segments;

    for(std::uint32_t i = 0; i != segments; ++i) {
        GLfloat segmentAngle = i*segmentAngleIncrement;
        positions(0)->push_back({std::sin(segmentAngle), y, std::cos(segmentAngle)});
        normals(0)->push_back(normal);

        if(textureCoords == TextureCoords::Generate)
            textureCoords2D(0)->push_back({i*1.0f/segments, textureCoordsV});
    }

    /* Duplicate first segment in the ring for additional vertex for texture coordinate */
    if(textureCoords == TextureCoords::Generate) {
        positions(0)->push_back((*positions(0))[positions(0)->size()-segments]);
        normals(0)->push_back(normal);
        textureCoords2D(0)->push_back({1.0f, textureCoordsV});
    }
}

}}
