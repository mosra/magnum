#ifndef Primitives_Axis_h
#define Primitives_Axis_h
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
 * @brief Function @ref Magnum::Primitives::axis2D(), @ref Magnum::Primitives::axis3D()
 */

#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief 2D axis

Two color-coded arrows for visualizing orientation (XY is RG), going from
@cpp 0.0f @ce to @cpp 1.0f @ce on the X and Y axis. @ref MeshPrimitive::Lines
with @ref MeshIndexType::UnsignedShort indices, interleaved
@ref VertexFormat::Vector2 positions and @ref VertexFormat::Vector3 colors. The
returned instance references data stored in constant memory --- pass the data
through @ref MeshTools::owned() to get a mutable copy, if needed.

@image html primitives-axis2d.png width=256px

@see @ref axis3D(), @ref crosshair2D(), @ref line2D()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData axis2D();

/**
@brief 3D axis

Three color-coded arrows for visualizing orientation (XYZ is RGB), going from
@cpp 0.0f @ce to @cpp 1.0f @ce on the X, Y and Z axis.
@ref MeshPrimitive::Lines with @ref MeshIndexType::UnsignedShort indices,
interleaved @ref VertexFormat::Vector3 positions and @ref VertexFormat::Vector3
colors. The returned instance references data stored in constant memory ---
pass the data through @ref MeshTools::owned() to get a mutable copy, if needed.

@image html primitives-axis3d.png width=256px

@see @ref axis2D(), @ref crosshair3D(), @ref line3D()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData axis3D();

}}

#endif
