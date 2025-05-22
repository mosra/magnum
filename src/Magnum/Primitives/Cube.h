#ifndef Magnum_Primitives_Cube_h
#define Magnum_Primitives_Cube_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Enum @ref Magnum::Primitives::CubeFlag, enum set @ref Magnum::Primitives::CubeFlags, function @ref Magnum::Primitives::cubeSolid(), @ref Magnum::Primitives::cubeSolidStrip(), @ref Magnum::Primitives::cubeWireframe()
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief Cube flag
@m_since_latest

@see @ref CubeFlags, @ref cubeSolid()
*/
enum class CubeFlag: UnsignedByte {
    /**
     * Texture coordinates with a single image used for all faces, oriented in
     * a way that makes the image upright and not mirrored if looking from the
     * default +Z direction. Useful if all faces are meant to look the same.
     * Mutually exclusive with other `TextureCoordinates*` flags.
     *
     * @htmlinclude primitives-cube-all-same.svg
     */
    TextureCoordinatesAllSame = 1 << 1,

    /**
     * Texture coordinates with +X, +Y, +Z faces in the top row and -X, -Y and
     * -Z in the bottom row, oriented in a way that makes the image upright and
     * not mirrored if looking from the default +Z direction. Useful to have a
     * different texture for each face but still make use of the whole texture
     * area with no wasted space. Mutually exclusive with other
     * `TextureCoordinates*` flags.
     *
     * @htmlinclude primitives-cube-positive-up-negative-down.svg
     */
    TextureCoordinatesPositiveUpNegativeDown = 2 << 1,

    /**
     * Texture coordinates with both upper and lower face going from -X.
     * Mutually exclusive with other `TextureCoordinates*` flags.
     *
     * @htmlinclude primitives-cube-negative-x-up-negative-x-down.svg
     */
    TextureCoordinatesNegativeXUpNegativeXDown = 3 << 1,

    /**
     * Texture coordinates with upper face going from -X and lower face from
     * +Z. Mutually exclusive with other `TextureCoordinates*` flags.
     *
     * @htmlinclude primitives-cube-negative-x-up-positive-z-down.svg
     */
    TextureCoordinatesNegativeXUpPositiveZDown = 4 << 1,

    /**
     * Texture coordinates with upper face going from -X and lower face from
     * +X. Mutually exclusive with other `TextureCoordinates*` flags.
     *
     * @htmlinclude primitives-cube-negative-x-up-positive-x-down.svg
     */
    TextureCoordinatesNegativeXUpPositiveXDown = 5 << 1,

    /**
     * Texture coordinates with upper face going from -X and lower face from
     * -Z. Mutually exclusive with other `TextureCoordinates*` flags.
     *
     * @htmlinclude primitives-cube-negative-x-up-negative-z-down.svg
     */
    TextureCoordinatesNegativeXUpNegativeZDown = 6 << 1,

    /**
     * Texture coordinates with both upper face lower face going from +Z.
     * Mutually exclusive with other `TextureCoordinates*` flags.
     *
     * @htmlinclude primitives-cube-positive-z-up-positive-z-down.svg
     */
    TextureCoordinatesPositiveZUpPositiveZDown = 7 << 1,

    /**
     * Texture coordinates with upper face going from +Z and lower face from
     * +X. Mutually exclusive with other `TextureCoordinates*` flags.
     *
     * @htmlinclude primitives-cube-positive-z-up-positive-x-down.svg
     */
    TextureCoordinatesPositiveZUpPositiveXDown = 8 << 1,

    /**
     * Generate four-component tangents. The last component can be used to
     * reconstruct a bitangent as described in the documentation of
     * @ref Trade::MeshAttribute::Tangent. Requires one of the
     * `TextureCoordinates*` to be set in order to know the tangent direction.
     */
    Tangents = 1 << 0,
};

/**
@brief Cube flags
@m_since_latest

@see @ref cubeSolid()
*/
typedef Containers::EnumSet<CubeFlag> CubeFlags;

CORRADE_ENUMSET_OPERATORS(CubeFlags)

/**
@brief Solid 3D cube

2x2x2 cube, centered at origin. @ref MeshPrimitive::Triangles with
@ref MeshIndexType::UnsignedShort indices, interleaved
@ref VertexFormat::Vector3 positions, flat @ref VertexFormat::Vector3 normals, optional @ref VertexFormat::Vector4 tangents and optional
@ref VertexFormat::Vector2 texture coordinates. If no @p flags are passed, the
returned instance references @ref Trade::DataFlag::Global data --- pass the
mesh through @ref MeshTools::copy() to get a mutable copy, if needed.

@image html primitives-cubesolid.png width=256px

Expects that at most one `TextureCoordinates*` @ref CubeFlag is set. If
@ref CubeFlag::Tangents is set, expects that exactly one `TextureCoordinates*`
@ref CubeFlag is set as well. Rotate or mirror the resulting mesh texture
coordinates, positions or both to create remaining texture mapping variants.

@see @ref cubeSolidStrip(), @ref cubeWireframe(), @ref MeshTools::transform3D(),
    @ref MeshTools::transformTextureCoordinates2D()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
Trade::MeshData cubeSolid(CubeFlags flags = {});
#else
/* Make it possible to DCE the texture / tangent logic if no flags are used */
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData cubeSolid();
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData cubeSolid(CubeFlags flags);
#endif

/**
@brief Solid 3D cube as a single strip

2x2x2 cube, centered at origin. Non-indexed @ref MeshPrimitive::TriangleStrip
with @ref VertexFormat::Vector3 positions. No normals or anything else, use
@ref cubeSolid() instead if you need these. The returned instance references
@ref Trade::DataFlag::Global data --- pass the mesh through
@ref MeshTools::copy() to get a mutable copy, if needed.

Vertex positions of this mesh can be also generated directly in the vertex
shader using @glsl gl_VertexID @ce ([source](https://twitter.com/turanszkij/status/1141638406956617730),
adapted to exactly match the output of this function):

@snippet Magnum/Primitives/Test/data.glsl cubeSolidStrip

@see @ref cubeWireframe(), @ref MeshTools::generateTriangleStripIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData cubeSolidStrip();

/**
@brief Wireframe 3D cube

2x2x2 cube, centered at origin. @ref MeshPrimitive::Lines with
@ref MeshIndexType::UnsignedShort indices and @ref VertexFormat::Vector3
positions. The returned instance references @ref Trade::DataFlag::Global data
--- pass the mesh through @ref MeshTools::copy() to get a mutable copy, if
needed.

@image html primitives-cubewireframe.png width=256px

@see @ref cubeSolid(), @ref cubeSolidStrip()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData cubeWireframe();

}}

#endif
