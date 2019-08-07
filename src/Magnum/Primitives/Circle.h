#ifndef Magnum_Primitives_Circle_h
#define Magnum_Primitives_Circle_h
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
 * @brief Function @ref Magnum::Primitives::circle2DSolid(), @ref Magnum::Primitives::circle2DWireframe(), @ref Magnum::Primitives::circle3DSolid(), @ref Magnum::Primitives::circle3DWireframe()
 */

#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief Solid 2D circle
@param segments  Number of segments. Must be greater or equal to @cpp 3 @ce.

Circle with radius @cpp 1.0f @ce. Non-indexed @ref MeshPrimitive::TriangleFan.

@image html primitives-circle2dsolid.png width=256px

@see @ref circle2DWireframe(), @ref circle3DSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData2D circle2DSolid(UnsignedInt segments);

/**
@brief Wireframe 2D circle
@param segments  Number of segments. Must be greater or equal to @cpp 3 @ce.

Circle with radius @cpp 1.0f @ce. Non-indexed @ref MeshPrimitive::LineLoop.

@image html primitives-circle2dwireframe.png width=256px

@see @ref circle2DSolid(), @ref circle3DWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData2D circle2DWireframe(UnsignedInt segments);

/**
@brief Solid 3D circle
@param segments  Number of segments. Must be greater or equal to @cpp 3 @ce.

Circle on the XY plane with radius @cpp 1.0f @ce. Non-indexed
@ref MeshPrimitive::TriangleFan with normals in positive Z direction.

@image html primitives-circle3dsolid.png width=256px

@see @ref circle3DWireframe(), @ref circle2DSolid()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData3D circle3DSolid(UnsignedInt segments);

/**
@brief Wireframe 3D circle
@param segments  Number of segments. Must be greater or equal to @cpp 3 @ce.

Circle on the XY plane with radius @cpp 1.0f @ce. Non-indexed
@ref MeshPrimitive::LineLoop.

@image html primitives-circle3dwireframe.png width=256px

@see @ref circle2DSolid(), @ref circle3DWireframe()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData3D circle3DWireframe(UnsignedInt segments);

}}

#endif
