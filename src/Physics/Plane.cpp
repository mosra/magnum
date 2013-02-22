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

#include "Plane.h"

#include <limits>

#include "Math/Matrix4.h"
#include "Math/Geometry/Intersection.h"
#include "LineSegment.h"

using namespace Magnum::Math::Geometry;

namespace Magnum { namespace Physics {

void Plane::applyTransformationMatrix(const Matrix4& matrix) {
    _transformedPosition = matrix.transformPoint(_position);
    _transformedNormal = matrix.rotation()*_normal;
}

bool Plane::collides(const AbstractShape<3>* other) const {
    if(other->type() == Type::Line)
        return *this % *static_cast<const Line3D*>(other);
    if(other->type() == Type::LineSegment)
        return *this % *static_cast<const LineSegment3D*>(other);

    return AbstractShape<3>::collides(other);
}

bool Plane::operator%(const Line3D& other) const {
    float t = Intersection::planeLine(transformedPosition(), transformedNormal(), other.transformedA(), other.transformedB());
    return t != t || (t != std::numeric_limits<float>::infinity() && t != -std::numeric_limits<float>::infinity());
}

bool Plane::operator%(const LineSegment3D& other) const {
    float t = Intersection::planeLine(transformedPosition(), transformedNormal(), other.transformedA(), other.transformedB());
    return t > 0.0f && t < 1.0f;
}

}}
