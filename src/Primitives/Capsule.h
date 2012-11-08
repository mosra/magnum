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
@brief 3D capsule primitive

Cylinder along Y axis with hemispheres instead of caps. Indexed triangle mesh
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
        Capsule(std::uint32_t hemisphereRings, std::uint32_t cylinderRings, std::uint32_t segments, GLfloat length, TextureCoords textureCoords = TextureCoords::DontGenerate);

    private:
        Capsule(std::uint32_t segments, TextureCoords textureCoords);

        void capVertex(GLfloat y, GLfloat normalY, GLfloat textureCoordsV);
        void hemisphereVertexRings(std::uint32_t count, GLfloat centerY, GLfloat startRingAngle, GLfloat ringAngleIncrement, GLfloat startTextureCoordsV, GLfloat textureCoordsVIncrement);
        void cylinderVertexRings(std::uint32_t count, GLfloat startY, GLfloat yIncrement, GLfloat startTextureCoordsV, GLfloat textureCoordsVIncrement);
        void bottomFaceRing();
        void faceRings(std::uint32_t count, std::uint32_t offset = 1);
        void topFaceRing();

        std::uint32_t segments;
        TextureCoords textureCoords;
};

}}

#endif
