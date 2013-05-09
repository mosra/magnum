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

#include "CollisionDispatch.h"

#include "Shapes/AxisAlignedBox.h"
#include "Shapes/Box.h"
#include "Shapes/Capsule.h"
#include "Shapes/LineSegment.h"
#include "Shapes/Plane.h"
#include "Shapes/Point.h"
#include "Shapes/Sphere.h"
#include "Shapes/shapeImplementation.h"

namespace Magnum { namespace Shapes { namespace Implementation {

namespace {
    inline constexpr UnsignedInt operator*(ShapeDimensionTraits<2>::Type a, ShapeDimensionTraits<2>::Type b) {
        return UnsignedInt(a)*UnsignedInt(b);
    }
    inline constexpr UnsignedInt operator*(ShapeDimensionTraits<3>::Type a, ShapeDimensionTraits<3>::Type b) {
        return UnsignedInt(a)*UnsignedInt(b);
    }
}

template<> bool collides(const AbstractShape<2>* const a, const AbstractShape<2>* const b) {
    if(a->type() < b->type()) return collides(b, a);

    switch(a->type()*b->type()) {
        case ShapeDimensionTraits<2>::Type::Sphere*ShapeDimensionTraits<2>::Type::Point:
            return static_cast<const Shape<Sphere2D>*>(a)->shape % static_cast<const Shape<Point2D>*>(b)->shape;
        case ShapeDimensionTraits<2>::Type::Sphere*ShapeDimensionTraits<2>::Type::Line:
            return static_cast<const Shape<Sphere2D>*>(a)->shape % static_cast<const Shape<Line2D>*>(b)->shape;
        case ShapeDimensionTraits<2>::Type::Sphere*ShapeDimensionTraits<2>::Type::LineSegment:
            return static_cast<const Shape<Sphere2D>*>(a)->shape % static_cast<const Shape<LineSegment2D>*>(b)->shape;
        case ShapeDimensionTraits<2>::Type::Sphere*ShapeDimensionTraits<2>::Type::Sphere:
            return static_cast<const Shape<Sphere2D>*>(a)->shape % static_cast<const Shape<Sphere2D>*>(b)->shape;

        case ShapeDimensionTraits<2>::Type::Capsule*ShapeDimensionTraits<2>::Type::Point:
            return static_cast<const Shape<Capsule2D>*>(a)->shape % static_cast<const Shape<Point2D>*>(b)->shape;
        case ShapeDimensionTraits<2>::Type::Capsule*ShapeDimensionTraits<2>::Type::Sphere:
            return static_cast<const Shape<Capsule2D>*>(a)->shape % static_cast<const Shape<Sphere2D>*>(b)->shape;

        case ShapeDimensionTraits<2>::Type::AxisAlignedBox*ShapeDimensionTraits<2>::Type::Point:
            return static_cast<const Shape<AxisAlignedBox2D>*>(a)->shape % static_cast<const Shape<Point2D>*>(b)->shape;
    }

    return false;
}

template<> bool collides(const AbstractShape<3>* const a, const AbstractShape<3>* const b) {
    if(a->type() < b->type()) return collides(b, a);

    switch(a->type()*b->type()) {
        case ShapeDimensionTraits<3>::Type::Sphere*ShapeDimensionTraits<3>::Type::Point:
            return static_cast<const Shape<Sphere3D>*>(a)->shape % static_cast<const Shape<Point3D>*>(b)->shape;
        case ShapeDimensionTraits<3>::Type::Sphere*ShapeDimensionTraits<3>::Type::Line:
            return static_cast<const Shape<Sphere3D>*>(a)->shape % static_cast<const Shape<Line3D>*>(b)->shape;
        case ShapeDimensionTraits<3>::Type::Sphere*ShapeDimensionTraits<3>::Type::LineSegment:
            return static_cast<const Shape<Sphere3D>*>(a)->shape % static_cast<const Shape<LineSegment3D>*>(b)->shape;
        case ShapeDimensionTraits<3>::Type::Sphere*ShapeDimensionTraits<3>::Type::Sphere:
            return static_cast<const Shape<Sphere3D>*>(a)->shape % static_cast<const Shape<Sphere3D>*>(b)->shape;

        case ShapeDimensionTraits<3>::Type::Capsule*ShapeDimensionTraits<3>::Type::Point:
            return static_cast<const Shape<Capsule3D>*>(a)->shape % static_cast<const Shape<Point3D>*>(b)->shape;
        case ShapeDimensionTraits<3>::Type::Capsule*ShapeDimensionTraits<3>::Type::Sphere:
            return static_cast<const Shape<Capsule3D>*>(a)->shape % static_cast<const Shape<Sphere3D>*>(b)->shape;

        case ShapeDimensionTraits<3>::Type::AxisAlignedBox*ShapeDimensionTraits<3>::Type::Point:
            return static_cast<const Shape<AxisAlignedBox3D>*>(a)->shape % static_cast<const Shape<Point3D>*>(b)->shape;

        case ShapeDimensionTraits<3>::Type::Plane*ShapeDimensionTraits<3>::Type::Line:
            return static_cast<const Shape<Plane>*>(a)->shape % static_cast<const Shape<Line3D>*>(b)->shape;
        case ShapeDimensionTraits<3>::Type::Plane*ShapeDimensionTraits<3>::Type::LineSegment:
            return static_cast<const Shape<Plane>*>(a)->shape % static_cast<const Shape<LineSegment3D>*>(b)->shape;
    }

    return false;
}

}}}
