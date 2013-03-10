#ifndef Magnum_Primitives_Capsule_h
#define Magnum_Primitives_Capsule_h
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

/** @file
 * @brief Class Magnum::Primitives::Capsule
 */

#include "Trade/MeshData3D.h"

#include "Primitives/magnumPrimitivesVisibility.h"

namespace Magnum { namespace Primitives {

/**
@brief 3D capsule primitive

%Cylinder along Y axis with hemispheres instead of caps. Indexed @ref Mesh::Primitive "Triangles"
with normals and optional 2D texture coordinates.
*/
class Capsule: public Trade::MeshData3D {
    friend class UVSphere;
    friend class Cylinder;

    public:
        /** @brief Whether to generate texture coordinates */
        enum class TextureCoords {
            Generate,       /**< Generate texture coordinates */
            DontGenerate    /**< Don't generate texture coordinates */
        };

        /**
         * @brief Constructor
         * @param hemisphereRings Number of (face) rings for each hemisphere.
         *      Must be larger or equal to 1.
         * @param cylinderRings Number of (face) rings for cylinder. Must be
         *      larger or equal to 1.
         * @param segments      Number of (face) segments. Must be larger or equal to 3.
         * @param length        Length of the capsule, excluding hemispheres.
         * @param textureCoords Whether to generate texture coordinates.
         *
         * If texture coordinates are generated, vertices of one segment are
         * duplicated for texture wrapping.
         */
        explicit MAGNUM_PRIMITIVES_EXPORT Capsule(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, UnsignedInt segments, Float length, TextureCoords textureCoords = TextureCoords::DontGenerate);

    private:
        Capsule(UnsignedInt segments, TextureCoords textureCoords);

        void capVertex(Float y, Float normalY, Float textureCoordsV);
        void hemisphereVertexRings(UnsignedInt count, Float centerY, Rad startRingAngle, Rad ringAngleIncrement, Float startTextureCoordsV, Float textureCoordsVIncrement);
        void cylinderVertexRings(UnsignedInt count, Float startY, Float yIncrement, Float startTextureCoordsV, Float textureCoordsVIncrement);
        void bottomFaceRing();
        void faceRings(UnsignedInt count, UnsignedInt offset = 1);
        void topFaceRing();

        UnsignedInt segments;
        TextureCoords textureCoords;
};

}}

#endif
