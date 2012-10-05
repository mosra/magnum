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

template<size_t dimensions> ShapeGroup<dimensions>::ShapeGroup(ShapeGroup<dimensions>&& other): operation(other.operation), a(other.a), b(other.b) {
    other.operation = Implementation::GroupOperation::AlwaysFalse;
    other.a = nullptr;
    other.b = nullptr;
}

template<size_t dimensions> ShapeGroup<dimensions>::~ShapeGroup() {
    if(!(operation & Implementation::GroupOperation::RefA)) delete a;
    if(!(operation & Implementation::GroupOperation::RefB)) delete b;
}

template<size_t dimensions> ShapeGroup<dimensions>& ShapeGroup<dimensions>::operator=(ShapeGroup<dimensions>&& other) {
    if(!(operation & Implementation::GroupOperation::RefA)) delete a;
    if(!(operation & Implementation::GroupOperation::RefB)) delete b;

    operation = other.operation;
    a = other.a;
    b = other.b;

    other.operation = Implementation::GroupOperation::AlwaysFalse;
    other.a = nullptr;
    other.b = nullptr;

    return *this;
}

template<size_t dimensions> void ShapeGroup<dimensions>::applyTransformation(const typename DimensionTraits<dimensions, GLfloat>::MatrixType& transformation) {
    if(a) a->applyTransformation(transformation);
    if(b) b->applyTransformation(transformation);
}

template<size_t dimensions> bool ShapeGroup<dimensions>::collides(const AbstractShape<dimensions>* other) const {
    switch(operation & ~Implementation::GroupOperation::RefAB) {
        case Implementation::GroupOperation::And: return a->collides(other) && b->collides(other);
        case Implementation::GroupOperation::Or: return a->collides(other) || b->collides(other);
        case Implementation::GroupOperation::Not: return !a->collides(other);
        case Implementation::GroupOperation::FirstObjectOnly: return a->collides(other);

        default:
            return false;
    }
}

template class ShapeGroup<2>;
template class ShapeGroup<3>;

}}
