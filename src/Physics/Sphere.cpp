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

#include "Sphere.h"

#include "Math/Functions.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Geometry/Distance.h"
#include "LineSegment.h"
#include "Point.h"

using namespace Magnum::Math::Geometry;

namespace Magnum { namespace Physics {

namespace {
    template<UnsignedInt dimensions> static typename DimensionTraits<dimensions>::VectorType unitVector();

    template<> inline Vector2 unitVector<2>() {
        return Vector2(1/Constants::sqrt2());
    }

    template<> inline Vector3 unitVector<3>() {
        return Vector3(1/Constants::sqrt3());
    }
}

template<UnsignedInt dimensions> void Sphere<dimensions>::applyTransformationMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) {
    _transformedPosition = matrix.transformPoint(_position);
    Float scaling = (matrix.rotationScaling()*unitVector<dimensions>()).length();
    _transformedRadius = scaling*_radius;
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::collides(const AbstractShape<dimensions>* other) const {
    if(other->type() == AbstractShape<dimensions>::Type::Point)
        return *this % *static_cast<const Point<dimensions>*>(other);
    if(other->type() == AbstractShape<dimensions>::Type::Line)
        return *this % *static_cast<const Line<dimensions>*>(other);
    if(other->type() == AbstractShape<dimensions>::Type::LineSegment)
        return *this % *static_cast<const LineSegment<dimensions>*>(other);
    if(other->type() == AbstractShape<dimensions>::Type::Sphere)
        return *this % *static_cast<const Sphere<dimensions>*>(other);

    return AbstractShape<dimensions>::collides(other);
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const Point<dimensions>& other) const {
    return (other.transformedPosition()-transformedPosition()).dot() <
        Math::pow<2>(transformedRadius());
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const Line<dimensions>& other) const {
    return Distance::linePointSquared(other.transformedA(), other.transformedB(), transformedPosition()) <
        Math::pow<2>(transformedRadius());
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const LineSegment<dimensions>& other) const {
    return Distance::lineSegmentPointSquared(other.transformedA(), other.transformedB(), transformedPosition()) <
        Math::pow<2>(transformedRadius());
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const Sphere<dimensions>& other) const {
    return (other.transformedPosition()-transformedPosition()).dot() <
        Math::pow<2>(transformedRadius()+other.transformedRadius());
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_PHYSICS_EXPORT Sphere<2>;
template class MAGNUM_PHYSICS_EXPORT Sphere<3>;
#endif

}}
