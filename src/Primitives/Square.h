#ifndef Magnum_Primitives_Square_h
#define Magnum_Primitives_Square_h
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
 * @brief Class Magnum::Primitives::Square
 */

#include "Trade/MeshData2D.h"

namespace Magnum { namespace Primitives {

/**
@brief 2D square primitive

2x2 square, non-indexed.
*/
class Square: public Trade::MeshData2D {
    public:
        /** @brief Constructor */
        Square();
};

}}

#endif
