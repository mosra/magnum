#ifndef Magnum_Primitives_Plane_h
#define Magnum_Primitives_Plane_h
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
 * @brief Function @ref Magnum::Primitives::planeSolid(), @ref Magnum::Primitives::planeWireframe()
 */

#include "Magnum/Trade/Trade.h"
#include "Magnum/Primitives/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Primitives {

/**
@brief Whether to generate plane texture coordinates

@see @ref planeSolid()
*/
enum class PlaneTextureCoords: UnsignedByte {
    DontGenerate,       /**< Don't generate texture coordinates */

    /** Generate texture coordinates with origin in bottom left corner. */
    Generate
};

/**
@brief Solid 3D plane

2x2 plane. Non-indexed @ref MeshPrimitive::TriangleStrip on the XY plane with
normals in positive Z direction.

@image html primitives-planesolid.png width=256px

@see @ref planeWireframe(), @ref squareSolid(), @ref gradient3D()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData3D planeSolid(PlaneTextureCoords textureCoords = PlaneTextureCoords::DontGenerate);

/**
@brief Wireframe 3D plane

2x2 plane. Non-indexed @ref MeshPrimitive::LineLoop on the XY plane.

@image html primitives-planewireframe.png width=256px

@see @ref planeSolid(), @ref squareWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData3D planeWireframe();

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief 3D plane
@deprecated Use @ref planeSolid() or @ref planeWireframe() instead.
*/
struct MAGNUM_PRIMITIVES_EXPORT Plane {
    /** @brief @copybrief PlaneTextureCoords
     * @deprecated Use @ref PlaneTextureCoords instead.
     */
    typedef CORRADE_DEPRECATED("use PlaneTextureCoords instead") PlaneTextureCoords TextureCoords;

    /** @brief @copybrief planeSolid()
     * @deprecated Use @ref planeSolid() instead.
     */
    CORRADE_DEPRECATED("use planeSolid() instead") static Trade::MeshData3D solid(PlaneTextureCoords textureCoords = PlaneTextureCoords::DontGenerate);

    /** @brief @copybrief planeWireframe()
     * @deprecated Use @ref planeWireframe() instead.
     */
    CORRADE_DEPRECATED("use planeWireframe() instead") static Trade::MeshData3D wireframe();
};
#endif

}}

#endif
