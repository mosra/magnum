#ifndef Magnum_Primitives_Square_h
#define Magnum_Primitives_Square_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

/** @file
 * @brief Function @ref Magnum::Primitives::squareSolid(), @ref Magnum::Primitives::squareWireframe()
 */

#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Primitives {

/**
@brief Whether to generate square texture coordinates

@see @ref squareSolid()
*/
enum class SquareTextureCoords: UnsignedByte {
    DontGenerate,       /**< Don't generate texture coordinates */

    /** Generate texture coordinates with origin in bottom left corner. */
    Generate
};

/**
@brief Solid 2D square

2x2 square. Non-indexed @ref MeshPrimitive::TriangleStrip.

@image html primitives-squaresolid.png width=256px

@see @ref squareWireframe(), @ref planeSolid(), @ref gradient2D()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData2D squareSolid(SquareTextureCoords textureCoords = SquareTextureCoords::DontGenerate);

/**
@brief Wireframe 2D square

2x2 square. Non-indexed @ref MeshPrimitive::LineLoop.

@image html primitives-squarewireframe.png width=256px

@see @ref squareSolid(), @ref planeWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData2D squareWireframe();

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief 2D square
@deprecated Use @ref squareSolid() or @ref squareWireframe() instead.
*/
struct MAGNUM_PRIMITIVES_EXPORT Square {
    /** @brief @copybrief SquareTextureCoords
     * @deprecated Use @ref SquareTextureCoords instead.
     */
    typedef CORRADE_DEPRECATED("use SquareTextureCoords instead") SquareTextureCoords TextureCoords;

    /** @brief @copybrief squareSolid()
     * @deprecated Use @ref squareWireframe() instead.
     */
    CORRADE_DEPRECATED("use squareSolid() instead") static Trade::MeshData2D solid(SquareTextureCoords textureCoords = SquareTextureCoords::DontGenerate);

    /** @brief @copybrief squareWireframe()
     * @deprecated Use @ref squareWireframe() instead.
     */
    CORRADE_DEPRECATED("use squareWireframe() instead") static Trade::MeshData2D wireframe();
};
#endif

}}

#endif
