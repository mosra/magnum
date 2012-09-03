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

#include "Capsule.h"

using namespace std;

namespace Magnum { namespace Primitives {

Capsule::Capsule(unsigned int rings, unsigned int segments, GLfloat length, TextureCoords textureCoords): MeshData("", Mesh::Primitive::Triangles, new vector<unsigned int>, {new vector<Vector4>()}, {new vector<Vector3>()}, textureCoords == TextureCoords::Generate ? vector<vector<Vector2>*>{new vector<Vector2>()} : vector<vector<Vector2>*>()), segments(segments), textureCoords(textureCoords) {
    CORRADE_ASSERT(rings >= 1 && segments >= 3, "Capsule must have at least one ring and three segments", );

    GLfloat height = 2.0f+length;
    GLfloat textureCoordsVIncrement = 1.0f/(rings*height);
    GLfloat ringAngleIncrement = Math::Constants<GLfloat>::pi()/(2*rings);

    /* Bottom cap vertex */
    capVertex(-height/2, -1.0f, 0.0f);

    /* Rings of bottom hemisphere */
    vertexRings(rings, -length/2, -Math::Constants<GLfloat>::pi()/2+ringAngleIncrement, ringAngleIncrement, textureCoordsVIncrement, textureCoordsVIncrement);

    /* Rings of top hemisphere */
    vertexRings(rings, length/2, 0.0f, ringAngleIncrement, (1.0f + length)/height, textureCoordsVIncrement);

    /* Top cap vertex */
    capVertex(height/2, 1.0f, 1.0f);

    /* Faces */
    bottomFaceRing();
    faceRings(rings*2-1);
    topFaceRing();
}

void Capsule::capVertex(GLfloat y, GLfloat normalY, GLfloat textureCoordsV) {
    positions(0)->push_back({0.0f, y, 0.0f});
    normals(0)->push_back({0.0f, normalY, 0.0f});

    if(textureCoords == TextureCoords::Generate)
        textureCoords2D(0)->push_back({0.5, textureCoordsV});
}

void Capsule::vertexRings(unsigned int count, GLfloat centerY, GLfloat startRingAngle, GLfloat ringAngleIncrement, GLfloat startTextureCoordsV, GLfloat textureCoordsVIncrement) {
    GLfloat segmentAngleIncrement = 2*Math::Constants<GLfloat>::pi()/segments;
    GLfloat x, y, z;
    for(unsigned int i = 0; i != count; ++i) {
        GLfloat ringAngle = startRingAngle + i*ringAngleIncrement;
        x = z = cos(ringAngle);
        y = sin(ringAngle);

        for(unsigned int j = 0; j != segments; ++j) {
            GLfloat segmentAngle = j*segmentAngleIncrement;
            positions(0)->push_back({x*sin(segmentAngle), centerY+y, z*cos(segmentAngle)});
            normals(0)->push_back({x*sin(segmentAngle), y, z*cos(segmentAngle)});

            if(textureCoords == TextureCoords::Generate)
                textureCoords2D(0)->push_back({j*1.0f/segments, startTextureCoordsV + i*textureCoordsVIncrement});
        }

        /* Duplicate first segment in the ring for additional vertex for texture coordinate */
        if(textureCoords == TextureCoords::Generate) {
            positions(0)->push_back((*positions(0))[positions(0)->size()-segments]);
            normals(0)->push_back((*normals(0))[normals(0)->size()-segments]);
            textureCoords2D(0)->push_back({1.0f, startTextureCoordsV + i*textureCoordsVIncrement});
        }
    }
}

void Capsule::bottomFaceRing() {
    for(unsigned int j = 0; j != segments; ++j) {
        /* Bottom vertex */
        indices()->push_back(0);

        /* Top right vertex */
        indices()->push_back((j != segments-1 || textureCoords == TextureCoords::Generate) ?
            j+2 : 1);

        /* Top left vertex */
        indices()->push_back(j+1);
    }
}

void Capsule::faceRings(unsigned int count) {
    unsigned int vertexSegments = segments + (textureCoords == TextureCoords::Generate ? 1 : 0);

    for(unsigned int i = 0; i != count; ++i) {
        for(unsigned int j = 0; j != segments; ++j) {
            unsigned int bottomLeft = i*vertexSegments+j+1;
            unsigned int bottomRight = ((j != segments-1 || textureCoords == TextureCoords::Generate) ?
                i*vertexSegments+j+2 : i*segments+1);
            unsigned int topLeft = bottomLeft+vertexSegments;
            unsigned int topRight = bottomRight+vertexSegments;

            indices()->push_back(bottomLeft);
            indices()->push_back(bottomRight);
            indices()->push_back(topRight);
            indices()->push_back(bottomLeft);
            indices()->push_back(topRight);
            indices()->push_back(topLeft);
        }
    }
}

void Capsule::topFaceRing() {
    unsigned int vertexSegments = segments + (textureCoords == TextureCoords::Generate ? 1 : 0);

    for(unsigned int j = 0; j != segments; ++j) {
        /* Bottom left vertex */
        indices()->push_back(normals(0)->size()-vertexSegments+j-1);

        /* Bottom right vertex */
        indices()->push_back((j != segments-1 || textureCoords == TextureCoords::Generate) ?
            normals(0)->size()-vertexSegments+j : normals(0)->size()-segments-1);

        /* Top vertex */
        indices()->push_back(normals(0)->size()-1);
    }
}

}}
