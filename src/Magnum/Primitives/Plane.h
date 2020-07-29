#ifndef Magnum_Primitives_Plane_h
#define Magnum_Primitives_Plane_h
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
 * @brief Function @ref Magnum::Primitives::planeSolid(), @ref Magnum::Primitives::planeWireframe()
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Utility/Macros.h>

#include "Magnum/Trade/Trade.h"
#include "Magnum/Primitives/visibility.h"

namespace Magnum { namespace Primitives {

/**
@brief Plane flag
@m_since{2020,06}

@see @ref PlaneFlags, @ref planeSolid()
*/
enum class PlaneFlag: UnsignedByte {
    /** Generate texture coordinates with origin in bottom left corner */
    TextureCoordinates = 1 << 0,

    /**
     * Generate four-component tangents. The last component can be used to
     * reconstruct a bitangent as described in the documentation of
     * @ref Trade::MeshAttribute::Tangent.
     * @m_since{2020,06}
     */
    Tangents = 1 << 1
};

/**
@brief Plane flags
@m_since{2020,06}

@see @ref planeSolid()
*/
typedef Containers::EnumSet<PlaneFlag> PlaneFlags;

CORRADE_ENUMSET_OPERATORS(PlaneFlags)

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Whether to generate plane texture coordinates
@m_deprecated_since{2020,06} Use @ref PlaneFlags instead.
*/
enum class CORRADE_DEPRECATED_ENUM("use PlaneFlags instead") PlaneTextureCoords: UnsignedByte {
    DontGenerate,       /**< Don't generate texture coordinates */

    /** Generate texture coordinates with origin in bottom left corner. */
    Generate
};
#endif

/**
@brief Solid 3D plane
@param flags        Flags
@m_since{2020,06}

2x2 square on the XY plane, centered at origin. Non-indexed
@ref MeshPrimitive::TriangleStrip with @ref VertexFormat::Vector3 positions,
@ref VertexFormat::Vector3 normals in positive Z direction, optional
@ref VertexFormat::Vector4 tangents and optional @ref VertexFormat::Vector2
texture coordinates. The returned instance may reference data stored in
constant memory --- pass the data through @ref MeshTools::owned() to get a
mutable copy, if needed.

@image html primitives-planesolid.png width=256px

@see @ref planeWireframe(), @ref squareSolid(), @ref gradient3D(),
    @ref MeshTools::generateTriangleStripIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData planeSolid(PlaneFlags flags);

/** @overload */
/* Separate API so apps that don't need texture coordinate / tangents don't
   need to drag in the extra code needed to allocate & calculate them */
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData planeSolid();

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief planeSolid(PlaneFlags)
@m_deprecated_since{2020,06} Use @ref planeSolid(PlaneFlags) instead.
*/
CORRADE_IGNORE_DEPRECATED_PUSH
MAGNUM_PRIMITIVES_EXPORT CORRADE_DEPRECATED("use planeSolid(PlaneFlags) instead") Trade::MeshData planeSolid(PlaneTextureCoords textureCoords);
CORRADE_IGNORE_DEPRECATED_POP
#endif

/**
@brief Wireframe 3D plane

2x2 square on the XY plane, centered at origin. Non-indexed
@ref MeshPrimitive::LineLoop on the XY plane with @ref VertexFormat::Vector3
positions. The returned instance references data stored in constant memory ---
pass the data through @ref MeshTools::owned() to get a mutable copy, if needed.

@image html primitives-planewireframe.png width=256px

@see @ref planeSolid(), @ref squareWireframe(),
    @ref MeshTools::generateLineLoopIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData planeWireframe();

}}

#endif
