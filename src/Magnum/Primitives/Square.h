#ifndef Magnum_Primitives_Square_h
#define Magnum_Primitives_Square_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Utility/Macros.h>

#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief Square flag
@m_since{2020,06}

@see @ref SquareFlags, @ref squareSolid()
*/
enum class SquareFlag: UnsignedByte {
    /** Generate texture coordinates with origin in bottom left corner */
    TextureCoordinates = 1 << 0
};

/**
@brief Square flags
@m_since{2020,06}

@see @ref squareSolid()
*/
typedef Containers::EnumSet<SquareFlag> SquareFlags;

CORRADE_ENUMSET_OPERATORS(SquareFlags)

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Whether to generate square texture coordinates
@m_deprecated_since{2020,06} Use @ref SquareFlags instead.
*/
enum class CORRADE_DEPRECATED_ENUM("use SquareFlags instead") SquareTextureCoords: UnsignedByte {
    DontGenerate,       /**< Don't generate texture coordinates */

    /** Generate texture coordinates with origin in bottom left corner. */
    Generate
};
#endif

/**
@brief Solid 2D square
@param flags        Flags
@m_since{2020,06}

2x2 square, centered at origin. Non-indexed @ref MeshPrimitive::TriangleStrip
with interleaved @ref VertexFormat::Vector2 positions and optional
@ref VertexFormat::Vector2 texture coordinates. The returned instance
references data stored in constant memory --- pass the data through
@ref MeshTools::owned() to get a mutable copy, if needed.

@image html primitives-squaresolid.png width=256px

@see @ref squareWireframe(), @ref planeSolid(), @ref gradient2D(),
    @ref MeshTools::generateTriangleStripIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData squareSolid(SquareFlags flags = {});

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief planeSolid(PlaneFlags)
@m_deprecated_since{2020,06} Use @ref planeSolid(PlaneFlags) instead.
*/
CORRADE_IGNORE_DEPRECATED_PUSH
MAGNUM_PRIMITIVES_EXPORT CORRADE_DEPRECATED("use squareSolid(SquareFlags) instead") Trade::MeshData squareSolid(SquareTextureCoords textureCoords);
CORRADE_IGNORE_DEPRECATED_POP
#endif

/**
@brief Wireframe 2D square

2x2 square, centered at origin. Non-indexed @ref MeshPrimitive::LineLoop with
@ref VertexFormat::Vector2 positions. The returned instance references data
stored in constant memory --- pass the data through @ref MeshTools::owned() to
get a mutable copy, if needed.

@image html primitives-squarewireframe.png width=256px

@see @ref squareSolid(), @ref planeWireframe(),
    @ref MeshTools::generateLineLoopIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData squareWireframe();

}}

#endif
