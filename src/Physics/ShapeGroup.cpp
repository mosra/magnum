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

#include "ShapeGroup.h"

namespace Magnum { namespace Physics {

ShapeGroup::ShapeGroup(ShapeGroup&& other): operation(other.operation), a(other.a), b(other.b) {
    other.operation = AlwaysFalse;
    other.a = nullptr;
    other.b = nullptr;
}

ShapeGroup::~ShapeGroup() {
    if(!(operation & RefA)) delete a;
    if(!(operation & RefB)) delete b;
}

ShapeGroup& ShapeGroup::operator=(ShapeGroup&& other) {
    if(!(operation & RefA)) delete a;
    if(!(operation & RefB)) delete b;

    operation = other.operation;
    a = other.a;
    b = other.b;

    other.operation = AlwaysFalse;
    other.a = nullptr;
    other.b = nullptr;

    return *this;
}

void ShapeGroup::applyTransformation(const Matrix4& transformation) {
    if(a) a->applyTransformation(transformation);
    if(b) b->applyTransformation(transformation);
}

bool ShapeGroup::collides(const AbstractShape* other) const {
    switch(operation & ~RefAB) {
        case Complement: return !a->collides(other);
        case Union: return a->collides(other) || b->collides(other);
        case Intersection: return a->collides(other) && b->collides(other);
        case Difference: return a->collides(other) && !b->collides(other);
        case Xor: return a->collides(other) != b->collides(other);
        case FirstObjectOnly: return a->collides(other);

        default:
            return false;
    }
}

}}
