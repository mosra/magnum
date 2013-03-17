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

#include "Cylinder.h"

#include "Math/Functions.h"
#include "Math/Vector3.h"

namespace Magnum { namespace Primitives {

Cylinder::Cylinder(UnsignedInt rings, UnsignedInt segments, Float length, Flags flags): Capsule(segments, flags & Flag::GenerateTextureCoords ? TextureCoords::Generate : TextureCoords::DontGenerate) {
    CORRADE_ASSERT(rings >= 1 && segments >= 3, "Cylinder must have at least one ring and three segments", );

    Float y = length*0.5f;
    Float textureCoordsV = flags & Flag::CapEnds ? 1.0f/(length+2.0f) : 0.0f;

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

void Cylinder::capVertexRing(Float y, Float textureCoordsV, const Vector3& normal) {
    Rad segmentAngleIncrement = 2*Rad(Constants::pi())/segments;

    for(UnsignedInt i = 0; i != segments; ++i) {
        Rad segmentAngle = i*segmentAngleIncrement;
        positions(0)->push_back({Math::sin(segmentAngle), y, Math::cos(segmentAngle)});
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
