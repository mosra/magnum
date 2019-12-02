#ifndef Magnum_Primitives_Plane_h
#define Magnum_Primitives_Plane_h
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
 * @brief Function @ref Magnum::Primitives::planeSolid(), @ref Magnum::Primitives::planeWireframe()
 */

#include "Magnum/Trade/Trade.h"
#include "Magnum/Primitives/visibility.h"

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
@ref VertexFormat::Vector3 positions and @ref VertexFormat::Vector3 normals in
positive Z direction. The returned instance references data stored in constant
memory.

@image html primitives-planesolid.png width=256px

@see @ref planeWireframe(), @ref squareSolid(), @ref gradient3D()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData planeSolid(PlaneTextureCoords textureCoords = PlaneTextureCoords::DontGenerate);

/**
@brief Wireframe 3D plane

2x2 plane. Non-indexed @ref MeshPrimitive::LineLoop on the XY plane with
@ref VertexFormat::Vector3 positions. The returned instance references data
stored in constant memory.

@image html primitives-planewireframe.png width=256px

@see @ref planeSolid(), @ref squareWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData planeWireframe();

}}

#endif
