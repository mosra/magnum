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

#include "ShapeGroup.h"

namespace Magnum { namespace Physics {

template<UnsignedInt dimensions> ShapeGroup<dimensions>::ShapeGroup(ShapeGroup<dimensions>&& other): operation(other.operation), a(other.a), b(other.b) {
    other.operation = Implementation::GroupOperation::AlwaysFalse;
    other.a = nullptr;
    other.b = nullptr;
}

template<UnsignedInt dimensions> ShapeGroup<dimensions>::~ShapeGroup() {
    if(!(operation & Implementation::GroupOperation::RefA)) delete a;
    if(!(operation & Implementation::GroupOperation::RefB)) delete b;
}

template<UnsignedInt dimensions> ShapeGroup<dimensions>& ShapeGroup<dimensions>::operator=(ShapeGroup<dimensions>&& other) {
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

template<UnsignedInt dimensions> void ShapeGroup<dimensions>::applyTransformationMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) {
    if(a) a->applyTransformationMatrix(matrix);
    if(b) b->applyTransformationMatrix(matrix);
}

template<UnsignedInt dimensions> bool ShapeGroup<dimensions>::collides(const AbstractShape<dimensions>* other) const {
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
