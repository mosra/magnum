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

#include "Math/Functions.h"
#include "Math/Vector3.h"

namespace Magnum { namespace Primitives {

Capsule::Capsule(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, UnsignedInt segments, Float length, TextureCoords textureCoords): MeshData3D(Mesh::Primitive::Triangles, new std::vector<UnsignedInt>, {new std::vector<Vector3>()}, {new std::vector<Vector3>()}, textureCoords == TextureCoords::Generate ? std::vector<std::vector<Vector2>*>{new std::vector<Vector2>()} : std::vector<std::vector<Vector2>*>()), segments(segments), textureCoords(textureCoords) {
    CORRADE_ASSERT(hemisphereRings >= 1 && cylinderRings >= 1 && segments >= 3, "Capsule must have at least one hemisphere ring, one cylinder ring and three segments", );

    Float height = 2.0f+length;
    Float hemisphereTextureCoordsVIncrement = 1.0f/(hemisphereRings*height);
    Rad hemisphereRingAngleIncrement(Constants::pi()/(2*hemisphereRings));

    /* Bottom cap vertex */
    capVertex(-height/2, -1.0f, 0.0f);

    /* Rings of bottom hemisphere */
    hemisphereVertexRings(hemisphereRings-1, -length/2, -Rad(Constants::pi()/2)+hemisphereRingAngleIncrement, hemisphereRingAngleIncrement, hemisphereTextureCoordsVIncrement, hemisphereTextureCoordsVIncrement);

    /* Rings of cylinder */
    cylinderVertexRings(cylinderRings+1, -length/2, length/cylinderRings, 1.0f/height, length/(cylinderRings*height));

    /* Rings of top hemisphere */
    hemisphereVertexRings(hemisphereRings-1, length/2, hemisphereRingAngleIncrement, hemisphereRingAngleIncrement, (1.0f + length)/height+hemisphereTextureCoordsVIncrement, hemisphereTextureCoordsVIncrement);

    /* Top cap vertex */
    capVertex(height/2, 1.0f, 1.0f);

    /* Faces */
    bottomFaceRing();
    faceRings(hemisphereRings*2-2+cylinderRings);
    topFaceRing();
}

Capsule::Capsule(UnsignedInt segments, TextureCoords textureCoords): MeshData3D(Mesh::Primitive::Triangles, new std::vector<UnsignedInt>, {new std::vector<Vector3>()}, {new std::vector<Vector3>()}, textureCoords == TextureCoords::Generate ? std::vector<std::vector<Vector2>*>{new std::vector<Vector2>()} : std::vector<std::vector<Vector2>*>()), segments(segments), textureCoords(textureCoords) {}

void Capsule::capVertex(Float y, Float normalY, Float textureCoordsV) {
    positions(0)->push_back({0.0f, y, 0.0f});
    normals(0)->push_back({0.0f, normalY, 0.0f});

    if(textureCoords == TextureCoords::Generate)
        textureCoords2D(0)->push_back({0.5, textureCoordsV});
}

void Capsule::hemisphereVertexRings(UnsignedInt count, Float centerY, Rad startRingAngle, Rad ringAngleIncrement, Float startTextureCoordsV, Float textureCoordsVIncrement) {
    Rad segmentAngleIncrement(2*Constants::pi()/segments);
    Float x, y, z;
    for(UnsignedInt i = 0; i != count; ++i) {
        Rad ringAngle = startRingAngle + Rad(i*ringAngleIncrement);
        x = z = Math::cos(ringAngle);
        y = Math::sin(ringAngle);

        for(UnsignedInt j = 0; j != segments; ++j) {
            Rad segmentAngle(j*segmentAngleIncrement);
            positions(0)->push_back({x*Math::sin(segmentAngle), centerY+y, z*Math::cos(segmentAngle)});
            normals(0)->push_back({x*Math::sin(segmentAngle), y, z*Math::cos(segmentAngle)});

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

void Capsule::cylinderVertexRings(UnsignedInt count, Float startY, Float yIncrement, Float startTextureCoordsV, Float textureCoordsVIncrement) {
    Rad segmentAngleIncrement(2*Constants::pi()/segments);
    for(UnsignedInt i = 0; i != count; ++i) {
        for(UnsignedInt j = 0; j != segments; ++j) {
            Rad segmentAngle(j*segmentAngleIncrement);
            positions(0)->push_back({Math::sin(segmentAngle), startY, Math::cos(segmentAngle)});
            normals(0)->push_back({Math::sin(segmentAngle), 0.0f, Math::cos(segmentAngle)});

            if(textureCoords == TextureCoords::Generate)
                textureCoords2D(0)->push_back({j*1.0f/segments, startTextureCoordsV + i*textureCoordsVIncrement});
        }

        /* Duplicate first segment in the ring for additional vertex for texture coordinate */
        if(textureCoords == TextureCoords::Generate) {
            positions(0)->push_back((*positions(0))[positions(0)->size()-segments]);
            normals(0)->push_back((*normals(0))[normals(0)->size()-segments]);
            textureCoords2D(0)->push_back({1.0f, startTextureCoordsV + i*textureCoordsVIncrement});
        }

        startY += yIncrement;
    }
}

void Capsule::bottomFaceRing() {
    for(UnsignedInt j = 0; j != segments; ++j) {
        /* Bottom vertex */
        indices()->push_back(0);

        /* Top right vertex */
        indices()->push_back((j != segments-1 || textureCoords == TextureCoords::Generate) ?
            j+2 : 1);

        /* Top left vertex */
        indices()->push_back(j+1);
    }
}

void Capsule::faceRings(UnsignedInt count, UnsignedInt offset) {
    UnsignedInt vertexSegments = segments + (textureCoords == TextureCoords::Generate ? 1 : 0);

    for(UnsignedInt i = 0; i != count; ++i) {
        for(UnsignedInt j = 0; j != segments; ++j) {
            UnsignedInt bottomLeft = i*vertexSegments+j+offset;
            UnsignedInt bottomRight = ((j != segments-1 || textureCoords == TextureCoords::Generate) ?
                i*vertexSegments+j+1+offset : i*segments+offset);
            UnsignedInt topLeft = bottomLeft+vertexSegments;
            UnsignedInt topRight = bottomRight+vertexSegments;

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
    UnsignedInt vertexSegments = segments + (textureCoords == TextureCoords::Generate ? 1 : 0);

    for(UnsignedInt j = 0; j != segments; ++j) {
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
