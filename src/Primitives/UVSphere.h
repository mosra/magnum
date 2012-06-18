#ifndef Magnum_Primitives_UVSphere_h
#define Magnum_Primitives_UVSphere_h
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

#include "Primitives/Capsule.h"

namespace Magnum { namespace Primitives {

/** @ingroup mesh
@brief UV Sphere primitive
*/
class UVSphere: public Capsule {
    public:
        /**
         * @brief Constructor
         * @param rings         Number of (face) rings. Must be larger or equal to 2.
         * @param segments      Number of (face) segments. Must be larger or equal to 3.
         * @param textureCoords Whether to generate texture coordinates.
         *
         * If texture coordinates are generated, vertices of one segment are
         * duplicated for texture wrapping.
         */
        UVSphere(unsigned int rings, unsigned int segments, TextureCoords textureCoords = TextureCoords::DontGenerate);
};

}}

#endif
