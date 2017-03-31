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

#include "Spheroid.h"

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Mesh.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives { namespace Implementation {

Spheroid::Spheroid(UnsignedInt segments, TextureCoords textureCoords): segments(segments), textureCoords(textureCoords) {}

void Spheroid::capVertex(Float y, Float normalY, Float textureCoordsV) {
    positions.emplace_back(0.0f, y, 0.0f);
    normals.emplace_back(0.0f, normalY, 0.0f);

    if(textureCoords == TextureCoords::Generate)
        textureCoords2D.emplace_back(0.5, textureCoordsV);
}

void Spheroid::hemisphereVertexRings(UnsignedInt count, Float centerY, Rad startRingAngle, Rad ringAngleIncrement, Float startTextureCoordsV, Float textureCoordsVIncrement) {
    Rad segmentAngleIncrement(Constants::tau()/segments);
    Float x, y, z;
    for(UnsignedInt i = 0; i != count; ++i) {
        Rad ringAngle = startRingAngle + Float(i)*ringAngleIncrement;
        x = z = Math::cos(ringAngle);
        y = Math::sin(ringAngle);

        for(UnsignedInt j = 0; j != segments; ++j) {
            Rad segmentAngle = Float(j)*segmentAngleIncrement;
            positions.emplace_back(x*Math::sin(segmentAngle), centerY+y, z*Math::cos(segmentAngle));
            normals.emplace_back(x*Math::sin(segmentAngle), y, z*Math::cos(segmentAngle));

            if(textureCoords == TextureCoords::Generate)
                textureCoords2D.emplace_back(j*1.0f/segments, startTextureCoordsV + i*textureCoordsVIncrement);
        }

        /* Duplicate first segment in the ring for additional vertex for texture coordinate */
        if(textureCoords == TextureCoords::Generate) {
            positions.push_back(positions[positions.size()-segments]);
            normals.push_back(normals[normals.size()-segments]);
            textureCoords2D.emplace_back(1.0f, startTextureCoordsV + i*textureCoordsVIncrement);
        }
    }
}

void Spheroid::cylinderVertexRings(UnsignedInt count, Float startY, Float yIncrement, Float startTextureCoordsV, Float textureCoordsVIncrement) {
    Rad segmentAngleIncrement(Constants::tau()/segments);
    for(UnsignedInt i = 0; i != count; ++i) {
        for(UnsignedInt j = 0; j != segments; ++j) {
            Rad segmentAngle = Float(j)*segmentAngleIncrement;
            positions.emplace_back(Math::sin(segmentAngle), startY, Math::cos(segmentAngle));
            normals.emplace_back(Math::sin(segmentAngle), 0.0f, Math::cos(segmentAngle));

            if(textureCoords == TextureCoords::Generate)
                textureCoords2D.emplace_back(j*1.0f/segments, startTextureCoordsV + i*textureCoordsVIncrement);
        }

        /* Duplicate first segment in the ring for additional vertex for texture coordinate */
        if(textureCoords == TextureCoords::Generate) {
            positions.push_back(positions[positions.size()-segments]);
            normals.push_back(normals[normals.size()-segments]);
            textureCoords2D.emplace_back(1.0f, startTextureCoordsV + i*textureCoordsVIncrement);
        }

        startY += yIncrement;
    }
}

void Spheroid::bottomFaceRing() {
    for(UnsignedInt j = 0; j != segments; ++j) {
        /* Bottom vertex */
        indices.push_back(0);

        /* Top right vertex */
        indices.push_back((j != segments-1 || textureCoords == TextureCoords::Generate) ?
            j+2 : 1);

        /* Top left vertex */
        indices.push_back(j+1);
    }
}

void Spheroid::faceRings(UnsignedInt count, UnsignedInt offset) {
    UnsignedInt vertexSegments = segments + (textureCoords == TextureCoords::Generate ? 1 : 0);

    for(UnsignedInt i = 0; i != count; ++i) {
        for(UnsignedInt j = 0; j != segments; ++j) {
            UnsignedInt bottomLeft = i*vertexSegments+j+offset;
            UnsignedInt bottomRight = ((j != segments-1 || textureCoords == TextureCoords::Generate) ?
                i*vertexSegments+j+1+offset : i*segments+offset);
            UnsignedInt topLeft = bottomLeft+vertexSegments;
            UnsignedInt topRight = bottomRight+vertexSegments;

            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topLeft);
        }
    }
}

void Spheroid::topFaceRing() {
    UnsignedInt vertexSegments = segments + (textureCoords == TextureCoords::Generate ? 1 : 0);

    for(UnsignedInt j = 0; j != segments; ++j) {
        /* Bottom left vertex */
        indices.push_back(normals.size()-vertexSegments+j-1);

        /* Bottom right vertex */
        indices.push_back((j != segments-1 || textureCoords == TextureCoords::Generate) ?
            normals.size()-vertexSegments+j : normals.size()-segments-1);

        /* Top vertex */
        indices.push_back(normals.size()-1);
    }
}

void Spheroid::capVertexRing(Float y, Float textureCoordsV, const Vector3& normal) {
    Rad segmentAngleIncrement(Constants::tau()/segments);

    for(UnsignedInt i = 0; i != segments; ++i) {
        Rad segmentAngle = Float(i)*segmentAngleIncrement;
        positions.emplace_back(Math::sin(segmentAngle), y, Math::cos(segmentAngle));
        normals.push_back(normal);

        if(textureCoords == TextureCoords::Generate)
            textureCoords2D.emplace_back(i*1.0f/segments, textureCoordsV);
    }

    /* Duplicate first segment in the ring for additional vertex for texture coordinate */
    if(textureCoords == TextureCoords::Generate) {
        positions.push_back(positions[positions.size()-segments]);
        normals.push_back(normal);
        textureCoords2D.emplace_back(1.0f, textureCoordsV);
    }
}

Trade::MeshData3D Spheroid::finalize() {
    return Trade::MeshData3D{MeshPrimitive::Triangles, std::move(indices), {std::move(positions)}, {std::move(normals)},
        textureCoords == TextureCoords::Generate ? std::vector<std::vector<Vector2>>{std::move(textureCoords2D)} : std::vector<std::vector<Vector2>>(), {}, nullptr};
}

}}}
