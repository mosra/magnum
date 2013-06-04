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
#include "Magnum.h"
#include "Shapes/LineSegment.h"
#include "Shapes/Point.h"

using namespace Magnum::Math::Geometry;

namespace Magnum { namespace Shapes {

namespace {
    template<UnsignedInt dimensions> static typename DimensionTraits<dimensions>::VectorType unitVector();

    template<> inline Vector2 unitVector<2>() {
        return Vector2(1/Constants::sqrt2());
    }

    template<> inline Vector3 unitVector<3>() {
        return Vector3(1/Constants::sqrt3());
    }
}

template<UnsignedInt dimensions> Sphere<dimensions> Sphere<dimensions>::transformed(const typename DimensionTraits<dimensions>::MatrixType& matrix) const {
    return Sphere<dimensions>(matrix.transformPoint(_position),
        (matrix.rotationScaling()*unitVector<dimensions>()).length()*_radius);
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const Point<dimensions>& other) const {
    return (other.position()-_position).dot() < Math::pow<2>(_radius);
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const Line<dimensions>& other) const {
    return Distance::linePointSquared(other.a(), other.b(), _position) < Math::pow<2>(_radius);
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const LineSegment<dimensions>& other) const {
    return Distance::lineSegmentPointSquared(other.a(), other.b(), _position) < Math::pow<2>(_radius);
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const Sphere<dimensions>& other) const {
    return (other._position-_position).dot() < Math::pow<2>(_radius+other._radius);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_SHAPES_EXPORT Sphere<2>;
template class MAGNUM_SHAPES_EXPORT Sphere<3>;
#endif

}}
