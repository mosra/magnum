#ifndef Magnum_Shapes_Shapes_h
#define Magnum_Shapes_Shapes_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
@brief Forward declarations for the @ref Magnum::Shapes namespace

@deprecated The @ref Magnum::Shapes library is a failed design experiment and
    is scheduled for removal in a future release. Related geometry algorithms
    were moved to @ref Magnum::Math::Distance and @ref Magnum::Math::Intersection;
    if you need a full-fledged physics library, please have look at
    [Bullet](https://bulletphysics.org), which has Magnum integration in
    @ref Magnum::BulletIntegration, or at [Box2D](https://box2d.org/), which
    has a @ref examples-box2d "Magnum example" as well.
*/

#include <Corrade/Utility/Macros.h>

#include "Magnum/Types.h"

#ifndef MAGNUM_BUILD_DEPRECATED
#error the Shapes library is scheduled for removal, see the docs for alternatives
#endif

/* I still have a test for this class and it shouldn't pollute the log there */
#ifndef _MAGNUM_DO_NOT_WARN_DEPRECATED_SHAPES
CORRADE_DEPRECATED_FILE("the Shapes library is scheduled for removal, see the docs for alternatives")
#endif

namespace Magnum { namespace CORRADE_DEPRECATED_NAMESPACE("scheduled for removal, see the docs for alternatives") Shapes {

CORRADE_IGNORE_DEPRECATED_PUSH
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") AbstractShape;
typedef AbstractShape<2> CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") AbstractShape2D;
typedef AbstractShape<3> CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") AbstractShape3D;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") AxisAlignedBox;
typedef AxisAlignedBox<2> CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") AxisAlignedBox2D;
typedef AxisAlignedBox<3> CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") AxisAlignedBox3D;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Box;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Box<2> Box2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Box<3> Box3D;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Capsule;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Capsule<2> Capsule2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Capsule<3> Capsule3D;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Collision;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Collision<2> Collision2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Collision<3> Collision3D;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Composition;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Composition<2> Composition2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Composition<3> Composition3D;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Cylinder;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Cylinder<2> Cylinder2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Cylinder<3> Cylinder3D;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Line;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Line<2> Line2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Line<3> Line3D;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") LineSegment;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") LineSegment<2> LineSegment2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") LineSegment<3> LineSegment3D;

template<class> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Shape;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") ShapeGroup;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") ShapeGroup<2> ShapeGroup2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") ShapeGroup<3> ShapeGroup3D;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Sphere;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Sphere<2> Sphere2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Sphere<3> Sphere3D;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") InvertedSphere;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") InvertedSphere<2> InvertedSphere2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") InvertedSphere<3> InvertedSphere3D;

class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Plane;

template<UnsignedInt> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Point;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Point<2> Point2D;
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Point<3> Point3D;
#endif
CORRADE_IGNORE_DEPRECATED_POP

}}

#endif
