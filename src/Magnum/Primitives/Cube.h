#ifndef Magnum_Primitives_Cube_h
#define Magnum_Primitives_Cube_h
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
 * @brief Function @ref Magnum::Primitives::cubeSolid(), @ref Magnum::Primitives::cubeSolidStrip(), @ref Magnum::Primitives::cubeWireframe()
 */

#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief Solid 3D cube

2x2x2 cube, centered at origin. @ref MeshPrimitive::Triangles with
@ref MeshIndexType::UnsignedShort indices, interleaved
@ref VertexFormat::Vector3 positions and flat @ref VertexFormat::Vector3
normals. The returned instance references data stored in constant memory ---
pass the data through @ref MeshTools::owned() to get a mutable copy, if needed.

@image html primitives-cubesolid.png width=256px

@see @ref cubeSolidStrip(), @ref cubeWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData cubeSolid();

/**
@brief Solid 3D cube as a single strip

2x2x2 cube, centered at origin. Non-indexed @ref MeshPrimitive::TriangleStrip
with @ref VertexFormat::Vector3 positions. No normals or anything else, use
@ref cubeSolid() instead if you need these. The returned instance references
data stored in constant memory --- pass the data through @ref MeshTools::owned()
to get a mutable copy, if needed.

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
positions. The returned instance references data stored in constant memory ---
pass the data through @ref MeshTools::owned() to get a mutable copy, if needed.

@image html primitives-cubewireframe.png width=256px

@see @ref cubeSolid(), @ref cubeSolidStrip()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData cubeWireframe();

}}

#endif
