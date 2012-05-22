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

#include "Math/Geometry/Distance.h"

using namespace Magnum::Math::Geometry;

namespace Magnum { namespace Physics {

void Capsule::applyTransformation(const Matrix4& transformation) {
    _transformedA = (transformation*Vector4(_a)).xyz();
    _transformedB = (transformation*Vector4(_b)).xyz();
    float scaling = (transformation.rotationScaling()*Vector3(1/Math::Constants<float>::sqrt3())).length();
    _transformedRadius = scaling*_radius;
}

bool Capsule::collides(const AbstractShape* other) const {
    if(other->type() == Type::Point)
        return *this % *static_cast<const Point*>(other);
    if(other->type() == Type::Sphere)
        return *this % *static_cast<const Sphere*>(other);

    return AbstractShape::collides(other);
}

bool Capsule::operator%(const Point& other) const {
    return Distance::lineSegmentPointSquared(transformedA(), transformedB(), other.transformedPosition()) <
        Math::pow<2>(transformedRadius());
}

bool Capsule::operator%(const Sphere& other) const {
    return Distance::lineSegmentPointSquared(transformedA(), transformedB(), other.transformedPosition()) <
        Math::pow<2>(transformedRadius()+other.transformedRadius());
}

}}
