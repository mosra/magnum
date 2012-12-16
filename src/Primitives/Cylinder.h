#ifndef Magnum_Primitives_Cylinder_h
#define Magnum_Primitives_Cylinder_h
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
 * @brief Class Magnum::Primitives::UVSphere
 */

#include <Containers/EnumSet.h>

#include "Primitives/Capsule.h"

namespace Magnum { namespace Primitives {

/**
@brief 3D cylinder primitive

Indexed triangle mesh with normals, optional 2D texture coordinates and
optional capped ends.
*/
class Cylinder: public Capsule {
    public:
        /**
         * @brief %Flags
         *
         * @see Flags, Cylinder()
         */
        enum class Flag {
            GenerateTextureCoords = 1,  /**< @brief Generate texture coordinates */
            CapEnds                     /**< @brief Cap ends */
        };

        /** @brief %Flags */
        typedef Corrade::Containers::EnumSet<Flag, int> Flags;

        /**
         * @brief Constructor
         * @param rings         Number of (face) rings. Must be larger or
         *      equal to 1.
         * @param segments      Number of (face) segments. Must be larger or
         *      equal to 3.
         * @param length        Cylinder length
         * @param flags         Flags
         *
         * If texture coordinates are generated, vertices of one segment are
         * duplicated for texture wrapping.
         */
        explicit Cylinder(std::uint32_t rings, std::uint32_t segments, GLfloat length, Flags flags = Flags());

    private:
        void capVertexRing(GLfloat y, GLfloat textureCoordsV, const Vector3& normal);
};

CORRADE_ENUMSET_OPERATORS(Cylinder::Flags)

}}

#endif
