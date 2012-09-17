#ifndef Magnum_Primitives_Capsule_h
#define Magnum_Primitives_Capsule_h
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

/** @file
 * @brief Class Magnum::Primitives::Capsule
 */

#include "Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

/**
@brief %Capsule primitive

Cylinder along Y axis with hemispheres instead of caps.
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
        Capsule(unsigned int hemisphereRings, unsigned int cylinderRings, unsigned int segments, GLfloat length, TextureCoords textureCoords = TextureCoords::DontGenerate);

    private:
        Capsule(unsigned int segments, TextureCoords textureCoords);

        void capVertex(GLfloat y, GLfloat normalY, GLfloat textureCoordsV);
        void hemisphereVertexRings(unsigned int count, GLfloat centerY, GLfloat startRingAngle, GLfloat ringAngleIncrement, GLfloat startTextureCoordsV, GLfloat textureCoordsVIncrement);
        void cylinderVertexRings(unsigned int count, GLfloat startY, GLfloat yIncrement, GLfloat startTextureCoordsV, GLfloat textureCoordsVIncrement);
        void bottomFaceRing();
        void faceRings(unsigned int count, unsigned int offset = 1);
        void topFaceRing();

        unsigned int segments;
        TextureCoords textureCoords;
};

}}

#endif
