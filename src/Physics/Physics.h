#ifndef Magnum_Physics_Physics_h
#define Magnum_Physics_Physics_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Forward declarations for Magnum::Physics namespace
 */

#include "Types.h"

namespace Magnum { namespace Physics {

/** @todoc remove when doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt> class AbstractObjectShape;
typedef AbstractObjectShape<2> AbstractObjectShape2D;
typedef AbstractObjectShape<3> AbstractObjectShape3D;

template<UnsignedInt> class AxisAlignedBox;
typedef AxisAlignedBox<2> AxisAlignedBox2D;
typedef AxisAlignedBox<3> AxisAlignedBox3D;

template<UnsignedInt> class Box;
typedef Box<2> Box2D;
typedef Box<3> Box3D;

template<UnsignedInt> class Capsule;
typedef Capsule<2> Capsule2D;
typedef Capsule<3> Capsule3D;

template<UnsignedInt> class Line;
typedef Line<2> Line2D;
typedef Line<3> Line3D;

template<UnsignedInt> class LineSegment;
typedef LineSegment<2> LineSegment2D;
typedef LineSegment<3> LineSegment3D;

template<class> class ObjectShape;

template<UnsignedInt> class ObjectShapeGroup;
typedef ObjectShapeGroup<2> ObjectShapeGroup2D;
typedef ObjectShapeGroup<3> ObjectShapeGroup3D;

class Plane;

template<UnsignedInt> class Point;
typedef Point<2> Point2D;
typedef Point<3> Point3D;

template<UnsignedInt> class ShapeGroup;
typedef ShapeGroup<2> ShapeGroup2D;
typedef ShapeGroup<3> ShapeGroup3D;

template<UnsignedInt> class Sphere;
typedef Sphere<2> Sphere2D;
typedef Sphere<3> Sphere3D;
#endif

}}

#endif
