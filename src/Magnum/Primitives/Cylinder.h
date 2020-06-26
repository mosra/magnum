#ifndef Magnum_Primitives_Cylinder_h
#define Magnum_Primitives_Cylinder_h
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
 * @brief Function @ref Magnum::Primitives::cylinderSolid(), @ref Magnum::Primitives::cylinderWireframe()
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief Cylinder flag

@see @ref CylinderFlags, @ref cylinderSolid()
*/
enum class CylinderFlag: UnsignedByte {
    /**
     * Generate texture coordinates
     * @m_since{2020,06}
     */
    TextureCoordinates = 1 << 0,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * Generate texture coordinates
     * @m_deprecated_since{2020,06} Use @ref CylinderFlag::TextureCoordinates
     *      instead.
     */
    GenerateTextureCoords CORRADE_DEPRECATED_ENUM("use TextureCoordinates instead") = TextureCoordinates,
    #endif

    /**
     * Generate four-component tangents. The last component can be used to
     * reconstruct a bitangent as described in the documentation of
     * @ref Trade::MeshAttribute::Tangent.
     * @m_since{2020,06}
     */
    Tangents = 1 << 1,

    CapEnds = 1 << 2                 /**< Cap ends */
};

/**
@brief Cylinder flags

@see @ref cylinderSolid()
*/
typedef Containers::EnumSet<CylinderFlag> CylinderFlags;

CORRADE_ENUMSET_OPERATORS(CylinderFlags)

/**
@brief Solid 3D cylinder
@param rings        Number of (face) rings. Must be larger or equal to
    @cpp 1 @ce.
@param segments     Number of (face) segments. Must be larger or equal to
    @cpp 3 @ce.
@param halfLength   Half the cylinder length
@param flags        Flags

Cylinder of radius @cpp 1.0f @ce along the Y axis, centered at origin.
@ref MeshPrimitive::Triangles with @ref MeshIndexType::UnsignedInt indices,
interleaved @ref VertexFormat::Vector3 positions, @ref VertexFormat::Vector3
normals, optional @ref VertexFormat::Vector4 tangents, optional
@ref VertexFormat::Vector2 texture coordinates and optional capped ends. If
texture coordinates are generated, vertices of one segment are duplicated for
texture wrapping.

@image html primitives-cylindersolid.png width=256px

The cylinder is by default created with radius set to @f$ 1.0 @f$. In order to
get radius @f$ r @f$, length @f$ l @f$ and preserve correct normals, set
@p halfLength to @f$ 0.5 \frac{l}{r} @f$ and then scale all positions by
@f$ r @f$, for example using @ref MeshTools::transformPointsInPlace().
@see @ref cylinderWireframe(), @ref coneSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData cylinderSolid(UnsignedInt rings, UnsignedInt segments, Float halfLength, CylinderFlags flags = {});

/**
@brief Wireframe 3D cylinder
@param rings        Number of (line) rings. Must be larger or equal to
    @cpp 1 @ce.
@param segments     Number of (line) segments. Must be larger or equal to
    @cpp 4 @ce and multiple of @cpp 4 @ce.
@param halfLength   Half the cylinder length

Cylinder of radius @cpp 1.0f @ce along the Y axis, centerd at origin.
@ref MeshPrimitive::Lines with @ref MeshIndexType::UnsignedInt indices and
@ref VertexFormat::Vector3 positions.

@image html primitives-cylinderwireframe.png width=256px

@see @ref cylinderSolid(), @ref coneWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData cylinderWireframe(UnsignedInt rings, UnsignedInt segments, Float halfLength);

}}

#endif
