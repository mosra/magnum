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

#include "Math/Geometry/Intersection.h"

using namespace std;
using namespace Magnum::Math::Geometry;

namespace Magnum { namespace Physics {

void Plane::applyTransformation(const Matrix4& transformation) {
    _transformedPosition = (transformation*Vector4(_position)).xyz();
    _transformedNormal = transformation.rotation()*_normal;
}

bool Plane::collides(const AbstractShape* other) const {
    if(other->type() == Type::Line)
        return *this % *static_cast<const Line*>(other);
    if(other->type() == Type::LineSegment)
        return *this % *static_cast<const LineSegment*>(other);

    return AbstractShape::collides(other);
}

bool Plane::operator%(const Line& other) const {
    float t = Intersection::planeLine(transformedPosition(), transformedNormal(), other.transformedA(), other.transformedB());
    return t != t || (t != numeric_limits<float>::infinity() && t != -numeric_limits<float>::infinity());
}

bool Plane::operator%(const LineSegment& other) const {
    float t = Intersection::planeLine(transformedPosition(), transformedNormal(), other.transformedA(), other.transformedB());
    return t > 0.0f && t < 1.0f;
}

}}
