#ifndef Magnum_Primitives_Cube_h
#define Magnum_Primitives_Cube_h
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
 * @brief Class Magnum::Primitives::Cube
 */

#include "Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

/**
@brief 3D cube primitive

Indexed with smooth normals.
@todo Does anyone EVER want smooth normals on a cube?!
*/
class Cube: public Trade::MeshData3D {
    public:
        /** @brief Constructor */
        Cube();
};

}}

#endif
