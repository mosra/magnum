#ifndef Magnum_Primitives_Cube_h
#define Magnum_Primitives_Cube_h
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
 * @brief Function @ref Magnum::Primitives::cubeSolid(), @ref Magnum::Primitives::cubeSolidStrip(), @ref Magnum::Primitives::cubeWireframe()
 */

#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Primitives {

/**
@brief Solid 3D cube

Indexed @ref MeshPrimitive::Triangles with flat normals.

@image html primitives-cubesolid.png width=256px

@see @ref cubeSolidStrip(), @ref cubeWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData3D cubeSolid();

/**
@brief Solid 3D cube as a single strip

Non-indexed @ref MeshPrimitive::TriangleStrip. Just positions, no
normals or anything else.

@image html primitives-cubesolid.png width=256px

@see @ref cubeSolid(), @ref cubeWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData3D cubeSolidStrip();

/**
@brief Wireframe 3D cube

Indexed @ref MeshPrimitive::Lines.

@image html primitives-cubewireframe.png width=256px

@see @ref cubeSolid(), @ref cubeSolidStrip()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData3D cubeWireframe();

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief 3D cube
@deprecated Use @ref cubeSolid(), @ref cubeSolidStrip() or @ref cubeWireframe()
    instead.
*/
struct MAGNUM_PRIMITIVES_EXPORT Cube {
    /** @brief @copybrief cubeSolid()
     * @deprecated Use @ref cubeSolid() instead.
     */
    CORRADE_DEPRECATED("use cubeSolid() instead") static Trade::MeshData3D solid();

    /** @brief @copybrief cubeSolidStrip()
     * @deprecated Use @ref cubeSolidStrip() instead.
     */
    CORRADE_DEPRECATED("use cubeSolidStrip() instead") static Trade::MeshData3D solidStrip();

    /** @brief @copybrief cubeWireframe()
     * @deprecated Use @ref cubeWireframe() instead.
     */
    CORRADE_DEPRECATED("use cubeWireframe() instead") static Trade::MeshData3D wireframe();
};
#endif

}}

#endif
