#ifndef Magnum_Primitives_UVSphere_h
#define Magnum_Primitives_UVSphere_h
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
 * @brief Class @ref Magnum::Primitives::uvSphereSolid(), @ref Magnum::Primitives::uvSphereWireframe()
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Utility/Macros.h>

#include "Magnum/Trade/Trade.h"
#include "Magnum/Primitives/visibility.h"

namespace Magnum { namespace Primitives {

/**
@brief UV sphere flag
@m_since{2020,06}

@see @ref UVSphereFlags, @ref uvSphereSolid()
*/
enum class UVSphereFlag: UnsignedByte {
    TextureCoordinates = 1 << 0, /**< Generate texture coordinates */

    /**
     * Generate four-component tangents. The last component can be used to
     * reconstruct a bitangent as described in the documentation of
     * @ref Trade::MeshAttribute::Tangent.
     * @m_since{2020,06}
     */
    Tangents = 1 << 1
};

/**
@brief UV sphere flags
@m_since{2020,06}

@see @ref uvSphereSolid()
*/
typedef Containers::EnumSet<UVSphereFlag> UVSphereFlags;

CORRADE_ENUMSET_OPERATORS(UVSphereFlags)

/**
@brief Solid 3D UV sphere
@param rings            Number of (face) rings. Must be larger or equal to
    @cpp 2 @ce.
@param segments         Number of (face) segments. Must be larger or
    equal to @cpp 3 @ce.
@param flags            Flags
@m_since{2020,06}

Sphere of radius @cpp 1.0f @ce, centered at origin.
@ref MeshPrimitive::Triangles with @ref MeshIndexType::UnsignedInt indices,
interleaved @ref VertexFormat::Vector3 positions, @ref VertexFormat::Vector3
normals, optional @ref VertexFormat::Vector4 tangents and
@ref VertexFormat::Vector2 texture coordinates. If texture coordinates are
generated, vertices of one segment are duplicated for texture wrapping.

@image html primitives-uvspheresolid.png width=256px

@see @ref icosphereSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData uvSphereSolid(UnsignedInt rings, UnsignedInt segments, UVSphereFlags flags = {});

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Whether to generate UV sphere texture coordinates
@m_deprecated_since{2020,06} Use @ref UVSphereFlags instead.
*/
enum class CORRADE_DEPRECATED("use UVSphereFlags instead") UVSphereTextureCoords: UnsignedByte {
    DontGenerate,       /**< Don't generate texture coordinates */
    Generate            /**< Generate texture coordinates */
};

/**
@brief @copybrief uvSphereSolid(UnsignedInt, UnsignedInt, UVSphereFlags)
@m_deprecated_since{2020,06} Use @ref uvSphereSolid(UnsignedInt, UnsignedInt, UVSphereFlags)
    instead.
*/
CORRADE_IGNORE_DEPRECATED_PUSH
MAGNUM_PRIMITIVES_EXPORT CORRADE_DEPRECATED("use uvSphereSolid() with UVSphereFlags instead") Trade::MeshData uvSphereSolid(UnsignedInt rings, UnsignedInt segments, UVSphereTextureCoords textureCoords);
CORRADE_IGNORE_DEPRECATED_POP
#endif

/**
@brief Wireframe 3D UV sphere
@param rings            Number of (line) rings. Must be larger or equal to
    @cpp 2 @ce and multiple of @cpp 2 @ce.
@param segments         Number of (line) segments. Must be larger or equal to
    @cpp 4 @ce and multiple of @cpp 4 @ce.

Sphere of radius @cpp 1.0f @ce, centered at origin. @ref MeshPrimitive::Lines
with @ref MeshIndexType::UnsignedInt indices and @ref VertexFormat::Vector3
positions.

@image html primitives-uvspherewireframe.png width=256px

@see @ref icosphereSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData uvSphereWireframe(UnsignedInt rings, UnsignedInt segments);

}}

#endif
