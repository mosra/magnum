#ifndef Magnum_Primitives_Line_h
#define Magnum_Primitives_Line_h
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
 * @brief Function @ref Magnum::Primitives::line2D(), @ref Magnum::Primitives::line3D()
 */

#include "Magnum/Magnum.h"
#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief 2D line

Non-indexed @ref MeshPrimitive::Lines with @ref VertexFormat::Vector2 positions
going from @p a to @p b.

@image html primitives-line2d.png width=256px

@see @ref line3D(), @ref line3D(const Vector3&, const Vector3&), @ref axis2D(),
    @ref crosshair2D()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData line2D(const Vector2& a, const Vector2& b);

/**
@brief 2D line in an identity transformation

Equivalent to calling @ref line2D(const Vector2&, const Vector2&) as

@snippet MagnumPrimitives.cpp line2D-identity
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData line2D();

/**
@brief 3D line

Non-indexed @ref MeshPrimitive::Lines with @ref VertexFormat::Vector3 positions
going from @p a to @p b.

@image html primitives-line3d.png width=256px

@see @ref line3D(), @ref line2D(const Vector2&, const Vector2&), @ref axis3D(),
    @ref crosshair3D()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData line3D(const Vector3& a, const Vector3& b);

/**
@brief 3D line in an identity transformation

Unit-size line in direction of positive X axis. Equivalent to calling
@ref line3D(const Vector3&, const Vector3&) as

@snippet MagnumPrimitives.cpp line3D-identity
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData line3D();

}}

#endif
