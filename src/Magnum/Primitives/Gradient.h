#ifndef Magnum_Primitives_Gradient_h
#define Magnum_Primitives_Gradient_h
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
 * @brief Function @ref Magnum::Primitives::gradient2D(), @ref Magnum::Primitives::gradient2DHorizontal(), @ref Magnum::Primitives::gradient2DVertical(), @ref Magnum::Primitives::gradient3D(), @ref Magnum::Primitives::gradient3DHorizontal(), @ref Magnum::Primitives::gradient3DVertical()
 */

#include "Magnum/Magnum.h"
#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief 2D square with a gradient

2x2 square with vertex colors, centered at origin. Non-indexed
@ref MeshPrimitive::TriangleStrip with interleaved @ref VertexFormat::Vector2
positions and @ref VertexFormat::Vector4 colors. Vertex colors correspond to
the gradient defined by the endpoints @p a and @p b, linearly interpolated from
@p colorA to @p colorB.

@image html primitives-gradient2d.png width=256px

@see @ref gradient2DHorizontal(), @ref gradient2DVertical(), @ref gradient3D(),
    @ref squareSolid(), @ref MeshTools::generateTriangleStripIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData gradient2D(const Vector2& a, const Color4& colorA, const Vector2& b, const Color4& colorB);

/**
@brief 2D square with a horizontal gradient

Equivalent to calling @ref gradient2D() like this:

@snippet MagnumPrimitives.cpp gradient2DHorizontal

@image html primitives-gradient2dhorizontal.png width=256px

@see @ref gradient2DVertical(), @ref gradient3DHorizontal(), @ref squareSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData gradient2DHorizontal(const Color4& colorLeft, const Color4& colorRight);

/**
@brief 2D square with a vertical gradient

Equivalent to calling @ref gradient2D() like this:

@snippet MagnumPrimitives.cpp gradient2DVertical

@image html primitives-gradient2dvertical.png width=256px

@see @ref gradient2DHorizontal(), @ref gradient3DVertical(), @ref squareSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData gradient2DVertical(const Color4& colorBottom, const Color4& colorTop);

/**
@brief 3D plane with a gradient

2x2 square on the XY plane with vertex colors, centered at origin. Non-indexed
@ref MeshPrimitive::TriangleStrip with interleaved @ref VertexFormat::Vector3
positions, @ref VertexFormat::Vector3 normals in positive Z direction and
@ref VertexFormat::Vector4 colors. Vertex colors correspond to the gradient
defined by the endpoints @p a and @p b, linearly interpolated from @p colorA to
@p colorB.

@image html primitives-gradient3d.png width=256px

@see @ref gradient3DHorizontal(), @ref gradient3DVertical(), @ref gradient2D(),
    @ref planeSolid(), @ref MeshTools::generateTriangleStripIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData gradient3D(const Vector3& a, const Color4& colorA, const Vector3& b, const Color4& colorB);

/**
@brief 3D plane with a horizontal gradient

Equivalent to calling @ref gradient3D() like this:

@snippet MagnumPrimitives.cpp gradient3DHorizontal

@image html primitives-gradient3dhorizontal.png width=256px

@see @ref gradient3DVertical(), @ref gradient2DHorizontal(), @ref planeSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData gradient3DHorizontal(const Color4& colorLeft, const Color4& colorRight);

/**
@brief 3D plane with a vertical gradient

Equivalent to calling @ref gradient3D() like this:

@snippet MagnumPrimitives.cpp gradient3DVertical

@image html primitives-gradient3dvertical.png width=256px

@see @ref gradient3DHorizontal(), @ref gradient2DVertical(), @ref planeSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData gradient3DVertical(const Color4& colorBottom, const Color4& colorTop);

}}

#endif
