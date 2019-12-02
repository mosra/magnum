#ifndef Magnum_Primitives_UVSphere_h
#define Magnum_Primitives_UVSphere_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Class @ref Magnum::Primitives::uvSphereSolid(), @ref Magnum::Primitives::uvSphereWireframe()
 */

#include "Magnum/Trade/Trade.h"
#include "Magnum/Primitives/visibility.h"

namespace Magnum { namespace Primitives {

/**
@brief Whether to generate UV sphere texture coordinates

@see @ref uvSphereSolid()
*/
enum class UVSphereTextureCoords: UnsignedByte {
    DontGenerate,       /**< Don't generate texture coordinates */
    Generate            /**< Generate texture coordinates */
};

/**
@brief Solid 3D UV sphere
@param rings            Number of (face) rings. Must be larger or equal to
    @cpp 2 @ce.
@param segments         Number of (face) segments. Must be larger or
    equal to @cpp 3 @ce.
@param textureCoords    Whether to generate texture coordinates

Sphere with radius @cpp 1.0f @ce. @ref MeshPrimitive::Triangles with
@ref MeshIndexType::UnsignedInt indices, interleaved @ref VertexFormat::Vector3
positions, @ref VertexFormat::Vector3 normals and optional
@ref VertexFormat::Vector2 texture coordinates. If texture coordinates are
generated, vertices of one segment are duplicated for texture wrapping.

@image html primitives-uvspheresolid.png width=256px

@see @ref icosphereSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData uvSphereSolid(UnsignedInt rings, UnsignedInt segments, UVSphereTextureCoords textureCoords = UVSphereTextureCoords::DontGenerate);

/**
@brief Wireframe 3D UV sphere
@param rings            Number of (line) rings. Must be larger or equal to
    @cpp 2 @ce and multiple of @cpp 2 @ce.
@param segments         Number of (line) segments. Must be larger or equal to
    @cpp 4 @ce and multiple of @cpp 4 @ce.

Sphere with radius @cpp 1.0f @ce. @ref MeshPrimitive::Lines with
@ref MeshIndexType::UnsignedInt indices and @ref VertexFormat::Vector3
positions.

@image html primitives-uvspherewireframe.png width=256px

@see @ref icosphereSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData uvSphereWireframe(UnsignedInt rings, UnsignedInt segments);

}}

#endif
