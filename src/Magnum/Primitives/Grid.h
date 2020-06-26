#ifndef Magnum_Primitives_Grid_h
#define Magnum_Primitives_Grid_h
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
 * @brief Function @ref Magnum::Primitives::grid3DSolid(), @ref Magnum::Primitives::grid3DWireframe()
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Math.h"
#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief Grid flag

@see @ref GridFlags, @ref grid3DSolid()
*/
enum class GridFlag: UnsignedByte {
    /**
     * Generate texture coordinates with origin in the bottom left corner
     * @m_since{2020,06}
     */
    TextureCoordinates = 1 << 0,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * Generate texture coordinates with origin in the bottom left corner
     * @m_deprecated_since{2020,06} Use @ref GridFlag::TextureCoordinates
     *      instead.
     */
    GenerateTextureCoords CORRADE_DEPRECATED_ENUM("use TextureCoordinates instead") = TextureCoordinates,
    #endif

    /**
     * Generate normals in positive Z direction. Disable if you'd be
     * generating your own normals anyway (for example based on a heightmap).
     */
    Normals = 1 << 1,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * Generate normals in positive Z direction.
     * @m_deprecated_since{2020,06} Use @ref GridFlag::Normals instead.
     */
    GenerateNormals CORRADE_DEPRECATED_ENUM("use Normals instead") = Normals,
    #endif

    /**
     * Generate four-component tangents. The last component can be used to
     * reconstruct a bitangent as described in the documentation of
     * @ref Trade::MeshAttribute::Tangent.
     * @m_since{2020,06}
     */
    Tangents = 1 << 2
};

/**
@brief Grid flags

@see @ref grid3DSolid()
*/
typedef Containers::EnumSet<GridFlag> GridFlags;

CORRADE_ENUMSET_OPERATORS(GridFlags)

/**
@brief Solid 3D grid

2x2 grid on the XY plane with normals in positive Z direction, centered at
origin. @ref MeshPrimitive::Triangles with @ref MeshIndexType::UnsignedInt
indices, interleaved @ref VertexFormat::Vector3 positions, optional
@ref VertexFormat::Vector3 normals, optional @ref VertexFormat::Vector4
tangents and optional @ref VertexFormat::Vector2 texture coordinates.

@image html primitives-grid3dsolid.png width=256px

The @p subdivisions parameter describes how many times the plane gets cut in
each direction. Specifying @cpp {0, 0} @ce will make the result an (indexed)
equivalent to @ref planeSolid(); @cpp {5, 3} @ce will make the grid have 6
cells horizontally and 4 vertically. In particular, this is different from the
`subdivisions` parameter in @ref icosphereSolid().
@see @ref grid3DWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData grid3DSolid(const Vector2i& subdivisions, GridFlags flags = GridFlag::Normals);

/**
@brief Wireframe 3D grid

2x2 grid on the XY plane, centered at origin. @ref MeshPrimitive::Lines with
@ref MeshIndexType::UnsignedInt indices and @ref VertexFormat::Vector3
positions.

@image html primitives-grid3dwireframe.png width=256px

The @p subdivisions parameter describes how many times the plane gets cut in
each direction. Specifying @cpp {0, 0} @ce will make the result an (indexed)
equivalent to @ref planeWireframe(); @cpp {5, 3} @ce will make the grid have 6
cells horizontally and 4 vertically. In particular, this is different from the
`subdivisions` parameter in @ref icosphereSolid(). Also please note the grid
has vertices in each intersection to be suitable for deformation along the Z
axis --- not just long lines crossing each other.
@see @ref grid3DSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData grid3DWireframe(const Vector2i& subdivisions);

}}

#endif
