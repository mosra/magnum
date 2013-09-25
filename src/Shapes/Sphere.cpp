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

template<UnsignedInt dimensions> Sphere<dimensions> Sphere<dimensions>::transformed(const typename DimensionTraits<dimensions, Float>::MatrixType& matrix) const {
    return Sphere<dimensions>(matrix.transformPoint(_position), matrix.uniformScaling()*_radius);
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const Point<dimensions>& other) const {
    return (_position - other.position()).dot() < Math::pow<2>(_radius);
}

template<UnsignedInt dimensions> Collision<dimensions> Sphere<dimensions>::operator/(const Point<dimensions>& other) const {
    const typename DimensionTraits<dimensions, Float>::VectorType separating = _position - other.position();
    const Float dot = separating.dot();

    /* No collision occured */
    if(dot > Math::pow<2>(_radius)) return {};

    /* Actual distance from the center */
    const Float distance = Math::sqrt(dot);

    /* Separating normal. If can't decide on direction, just move up. */
    /** @todo How to handle this in a configurable way? */
    const typename DimensionTraits<dimensions, Float>::VectorType separatingNormal =
        Math::TypeTraits<Float>::equals(dot, 0.0f) ?
        DimensionTraits<dimensions, Float>::VectorType::yAxis() :
        separating/distance;

    /* Collision position is on the point */
    return Collision<dimensions>(other.position(), separatingNormal, _radius - distance);
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const Line<dimensions>& other) const {
    return Distance::linePointSquared(other.a(), other.b(), _position) < Math::pow<2>(_radius);
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const LineSegment<dimensions>& other) const {
    return Distance::lineSegmentPointSquared(other.a(), other.b(), _position) < Math::pow<2>(_radius);
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const Sphere<dimensions>& other) const {
    return (_position - other._position).dot() < Math::pow<2>(_radius + other._radius);
}

template<UnsignedInt dimensions> Collision<dimensions> Sphere<dimensions>::operator/(const Sphere<dimensions>& other) const {
    const Float minDistance = _radius + other._radius;
    const typename DimensionTraits<dimensions, Float>::VectorType separating = _position - other._position;
    const Float dot = separating.dot();

    /* No collision occured */
    if(dot > Math::pow<2>(minDistance)) return {};

    /* Actual distance */
    const Float distance = Math::sqrt(dot);

    /* Separating normal. If can't decide on direction, just move up. */
    /** @todo How to handle this in a configurable way? */
    const typename DimensionTraits<dimensions, Float>::VectorType separatingNormal =
        Math::TypeTraits<Float>::equals(dot, 0.0f) ?
        DimensionTraits<dimensions, Float>::VectorType::yAxis() :
        separating/distance;

    /* Contact position is on the surface of `other`, minDistace > distance */
    return Collision<dimensions>(other._position + separatingNormal*other._radius, separatingNormal, minDistance - distance);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_SHAPES_EXPORT Sphere<2>;
template class MAGNUM_SHAPES_EXPORT Sphere<3>;
#endif

}}
