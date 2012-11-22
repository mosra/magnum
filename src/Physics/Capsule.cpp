/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Capsule.h"

#include "Math/Constants.h"
#include "Math/Math.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Geometry/Distance.h"
#include "Point.h"
#include "Sphere.h"

using namespace Magnum::Math::Geometry;

namespace Magnum { namespace Physics {

template<std::uint8_t dimensions> void Capsule<dimensions>::applyTransformation(const typename DimensionTraits<dimensions>::MatrixType& transformation) {
    _transformedA = (transformation*typename DimensionTraits<dimensions>::PointType(_a)).vector();
    _transformedB = (transformation*typename DimensionTraits<dimensions>::PointType(_b)).vector();
    float scaling = (transformation.rotationScaling()*typename DimensionTraits<dimensions>::VectorType(1/Constants::sqrt3())).length();
    _transformedRadius = scaling*_radius;
}

template<std::uint8_t dimensions> bool Capsule<dimensions>::collides(const AbstractShape<dimensions>* other) const {
    if(other->type() == AbstractShape<dimensions>::Type::Point)
        return *this % *static_cast<const Point<dimensions>*>(other);
    if(other->type() == AbstractShape<dimensions>::Type::Sphere)
        return *this % *static_cast<const Sphere<dimensions>*>(other);

    return AbstractShape<dimensions>::collides(other);
}

template<std::uint8_t dimensions> bool Capsule<dimensions>::operator%(const Point<dimensions>& other) const {
    return Distance::lineSegmentPointSquared(transformedA(), transformedB(), other.transformedPosition()) <
        Math::pow<2>(transformedRadius());
}

template<std::uint8_t dimensions> bool Capsule<dimensions>::operator%(const Sphere<dimensions>& other) const {
    return Distance::lineSegmentPointSquared(transformedA(), transformedB(), other.transformedPosition()) <
        Math::pow<2>(transformedRadius()+other.transformedRadius());
}

template class Capsule<2>;
template class Capsule<3>;

}}
