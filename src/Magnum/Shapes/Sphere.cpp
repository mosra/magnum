/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "Sphere.h"

#include "Magnum/Magnum.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Geometry/Distance.h"
#include "Magnum/Shapes/LineSegment.h"
#include "Magnum/Shapes/Point.h"

using namespace Magnum::Math::Geometry;

namespace Magnum { namespace Shapes {

template<UnsignedInt dimensions> Sphere<dimensions> Sphere<dimensions>::transformed(const MatrixTypeFor<dimensions, Float>& matrix) const {
    return Sphere<dimensions>(matrix.transformPoint(_position), matrix.uniformScaling()*_radius);
}

template<UnsignedInt dimensions> bool Sphere<dimensions>::operator%(const Point<dimensions>& other) const {
    return (_position - other.position()).dot() < Math::pow<2>(_radius);
}

template<UnsignedInt dimensions> bool InvertedSphere<dimensions>::operator%(const Point<dimensions>& other) const {
    return (other.position() - position()).dot() > Math::pow<2>(radius());
}

template<UnsignedInt dimensions> Collision<dimensions> Sphere<dimensions>::operator/(const Point<dimensions>& other) const {
    const VectorTypeFor<dimensions, Float> separating = _position - other.position();
    const Float dot = separating.dot();

    /* No collision occured */
    if(dot > Math::pow<2>(_radius)) return {};

    /* Actual distance from the center */
    const Float distance = Math::sqrt(dot);

    /* Separating normal. If can't decide on direction, just move up. */
    /** @todo How to handle this in a configurable way? */
    const VectorTypeFor<dimensions, Float> separatingNormal =
        Math::TypeTraits<Float>::equals(dot, 0.0f) ?
        VectorTypeFor<dimensions, Float>::yAxis() :
        separating/distance;

    /* Collision position is on the point */
    return Collision<dimensions>(other.position(), separatingNormal, _radius - distance);
}

template<UnsignedInt dimensions> Collision<dimensions> InvertedSphere<dimensions>::operator/(const Point<dimensions>& other) const {
    const VectorTypeFor<dimensions, Float> separating = other.position() - position();
    const Float dot = separating.dot();

    /* No collision occured */
    if(dot < Math::pow<2>(radius())) return {};

    /* Actual distance from the center */
    const Float distance = Math::sqrt(dot);

    /* Separating normal */
    const VectorTypeFor<dimensions, Float> separatingNormal = separating/distance;

    /* Collision position is on the point */
    return Collision<dimensions>(other.position(), separatingNormal, distance - radius());
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

template<UnsignedInt dimensions> bool InvertedSphere<dimensions>::operator%(const Sphere<dimensions>& other) const {
    return (position() - other.position()).dot() > Math::pow<2>(radius() - other.radius());
}

template<UnsignedInt dimensions> Collision<dimensions> Sphere<dimensions>::operator/(const Sphere<dimensions>& other) const {
    const Float minDistance = _radius + other._radius;
    const VectorTypeFor<dimensions, Float> separating = _position - other._position;
    const Float dot = separating.dot();

    /* No collision occured */
    if(dot > Math::pow<2>(minDistance)) return {};

    /* Actual distance */
    const Float distance = Math::sqrt(dot);

    /* Separating normal. If can't decide on direction, just move up. */
    /** @todo How to handle this in a configurable way? */
    const VectorTypeFor<dimensions, Float> separatingNormal =
        Math::TypeTraits<Float>::equals(dot, 0.0f) ?
        VectorTypeFor<dimensions, Float>::yAxis() :
        separating/distance;

    /* Contact position is on the surface of `other`, minDistace > distance */
    return Collision<dimensions>(other._position + separatingNormal*other._radius, separatingNormal, minDistance - distance);
}

template<UnsignedInt dimensions> Collision<dimensions> InvertedSphere<dimensions>::operator/(const Sphere<dimensions>& other) const {
    const Float maxDistance = radius() - other.radius();
    /** @todo How to handle inseparable shapes or shapes which can't be separated by movement only (i.e. two half-spaces)? */
    CORRADE_INTERNAL_ASSERT(maxDistance > 0.0f);
    const VectorTypeFor<dimensions, Float> separating = other.position() - position();
    const Float dot = separating.dot();

    /* No collision occured */
    if(dot < Math::pow<2>(maxDistance)) return {};

    /* Actual distance */
    const Float distance = Math::sqrt(dot);

    /* Separating normal */
    const VectorTypeFor<dimensions, Float> separatingNormal = separating/distance;

    /* Contact position is on the surface of `other`, distance > maxDistance */
    return Collision<dimensions>(other.position() + separatingNormal*other.radius(), separatingNormal, distance - maxDistance);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_SHAPES_EXPORT Sphere<2>;
template class MAGNUM_SHAPES_EXPORT Sphere<3>;
template class MAGNUM_SHAPES_EXPORT InvertedSphere<2>;
template class MAGNUM_SHAPES_EXPORT InvertedSphere<3>;
#endif

}}
