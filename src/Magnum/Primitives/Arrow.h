#ifndef Magnum_Primitives_Arrow_h
#define Magnum_Primitives_Arrow_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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
 * @brief Function @ref Magnum::Primitives::arrow2D(), @ref Magnum::Primitives::arrow3D()
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief 2D arrow
@m_since_latest

Indexed @ref MeshPrimitive::Lines with @ref VertexFormat::Vector2 positions
going from @p a to @p b and @ref MeshIndexType::UnsignedShort indices. The
arrowhead has a constant size independent of the length. The returned instance
references @ref Trade::DataFlag::Global index data --- pass the mesh through
@ref MeshTools::copy() to get a mutable copy, if needed.

@image html primitives-arrow2d.png width=256px

@see @ref arrow2D(), @ref arrow3D(const Vector3&, const Vector3&, const Vector3&),
    @ref line2D(const Vector2&, const Vector2&), @ref axis2D(),
    @ref crosshair2D()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData arrow2D(const Vector2& a, const Vector2& b);

/**
@brief 2D arrow in an identity transformation
@m_since_latest

Unit-size arrow in direction of positive X axis. Equivalent to calling
@ref arrow2D(const Vector2&, const Vector2&) as

@snippet Primitives.cpp arrow2D-identity
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData arrow2D();

/**
@brief 3D arrow
@m_since_latest

Indexed @ref MeshPrimitive::Lines with @ref VertexFormat::Vector3 positions
going from @p a to @p b and @ref MeshIndexType::UnsignedShort indices. The
arrowhead has a constant size independent of the length. The @p tangent
argument can be used to adjust orientation of the arrowhead, which is by
default aligned with the XY plane. The returned instance references
@ref Trade::DataFlag::Global index data --- pass the mesh through
@ref MeshTools::copy() to get a mutable copy, if needed.

@image html primitives-arrow3d.png width=256px

@see @ref arrow3D(), @ref arrow2D(const Vector2&, const Vector2&),
    @ref line3D(const Vector3&, const Vector3&), @ref axis3D(),
    @ref crosshair3D()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData arrow3D(const Vector3& a, const Vector3& b, const Vector3& tangent = Vector3::zAxis());

/**
@brief 3D arrow in an identity transformation
@m_since_latest

Unit-size arrow in direction of positive X axis. Equivalent to calling
@ref arrow3D(const Vector3&, const Vector3&, const Vector3&) as

@snippet Primitives.cpp arrow3D-identity
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData arrow3D();

}}

#endif
