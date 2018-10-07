#ifndef Magnum_Primitives_Capsule_h
#define Magnum_Primitives_Capsule_h
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
 * @brief Function @ref Magnum::Primitives::capsule2DWireframe(), @ref Magnum::Primitives::capsule3DSolid(), @ref Magnum::Primitives::capsule3DWireframe()
 */

#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Primitives {

/**
@brief Wireframe 2D capsule
@param hemisphereRings Number of (line) rings for each hemisphere. Must be
    larger or equal to @cpp 1 @ce.
@param cylinderRings Number of (line) rings for cylinder. Must be larger or
    equal to @cpp 1 @ce.
@param halfLength    Half the length of cylinder part

Cylinder of radius @cpp 1.0f @ce along Y axis with hemispheres instead of caps.
Indexed @ref MeshPrimitive::Lines.

@image html primitives-capsule2dwireframe.png width=256px

@see @ref capsule3DSolid(), @ref capsule3DWireframe(), @ref circle2DWireframe(),
    @ref squareWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData2D capsule2DWireframe(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, Float halfLength);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief 2D capsule
@deprecated Use @ref capsule2DWireframe() instead.
*/
struct MAGNUM_PRIMITIVES_EXPORT Capsule2D {
    /** @copybrief capsule2DWireframe()
     * @deprecated Use @ref capsule2DWireframe() instead.
     */
    CORRADE_DEPRECATED("use capsule2DWireframe() instead") static Trade::MeshData2D wireframe(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, Float halfLength);
};
#endif

/**
@brief Whether to generate capsule texture coordinates

@see @ref capsule3DSolid()
*/
enum class CapsuleTextureCoords: UnsignedByte {
    DontGenerate,   /**< Don't generate texture coordinates */
    Generate        /**< Generate texture coordinates */
};

/**
@brief Solid 3D capsule
@param hemisphereRings  Number of (face) rings for each hemisphere. Must be
    larger or equal to @cpp 1 @ce.
@param cylinderRings    Number of (face) rings for cylinder. Must be larger or
    equal to @cpp 1 @ce.
@param segments         Number of (face) segments. Must be larger or equal to
    @cpp 3 @ce.
@param halfLength       Half the length of cylinder part
@param textureCoords    Whether to generate texture coordinates

Cylinder of radius @cpp 1.0f @ce along Y axis with hemispheres instead of caps.
Indexed @ref MeshPrimitive::Triangles with normals and optional 2D texture
coordinates. If texture coordinates are generated, vertices of one segment are
duplicated for texture wrapping.

@image html primitives-capsule3dsolid.png width=256px

The capsule is by default created with radius set to @f$ 1.0 @f$. In order to
get radius @f$ r @f$, length @f$ l @f$ and preserve correct normals, set
@p halfLength to @f$ 0.5 \frac{l}{r} @f$ and then scale all
@ref Trade::MeshData3D::positions() by @f$ r @f$, for example using
@ref MeshTools::transformPointsInPlace().
@see @ref capsule3DWireframe(), @ref capsule2DWireframe(), @ref cylinderSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData3D capsule3DSolid(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, UnsignedInt segments, Float halfLength, CapsuleTextureCoords textureCoords = CapsuleTextureCoords::DontGenerate);

/**
@brief Wireframe 3D capsule
@param hemisphereRings  Number of (line) rings for each hemisphere. Must be
    larger or equal to @cpp 1 @ce.
@param cylinderRings    Number of (line) rings for cylinder. Must be larger or
    equal to @cpp 1 @ce.
@param segments         Number of line segments. Must be larger or equal to
    @cpp 4 @ce and multiple of @cpp 4 @ce.
@param halfLength       Half the length of cylinder part

Cylinder of radius @cpp 1.0f @ce along Y axis with hemispheres instead of caps.
Indexed @ref MeshPrimitive::Lines.

@image html primitives-capsule3dwireframe.png width=256px

@see @ref capsule2DWireframe(), @ref capsule3DSolid(), @ref cylinderSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData3D capsule3DWireframe(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, UnsignedInt segments, Float halfLength);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief 3D capsule
@deprecated Use @ref capsule3DSolid() or @ref capsule3DWireframe() instead.
*/
struct MAGNUM_PRIMITIVES_EXPORT Capsule3D {
    /** @brief @copybrief CapsuleTextureCoords
     * @deprecated Use @ref CapsuleTextureCoords instead.
     */
    typedef CORRADE_DEPRECATED("use CapsuleTextureCoords instead") CapsuleTextureCoords TextureCoords;

    /** @brief @copybrief capsule3DSolid()
     * @deprecated Use @ref capsule3DSolid() instead.
     */
    CORRADE_DEPRECATED("use capsule3DSolid() instead") static Trade::MeshData3D solid(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, UnsignedInt segments, Float halfLength, CapsuleTextureCoords textureCoords = CapsuleTextureCoords::DontGenerate);

    /** @brief @copybrief capsule3DWireframe()
     * @deprecated Use @ref capsule3DWireframe() instead.
     */
    CORRADE_DEPRECATED("use capsule3DWireframe() instead") static Trade::MeshData3D wireframe(UnsignedInt hemisphereRings, UnsignedInt cylinderRings, UnsignedInt segments, Float halfLength);
};
#endif

}}

#endif
