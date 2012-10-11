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

#include "ShapedObject.h"

#include <algorithm>

#include "AbstractShape.h"
#include "ShapedObjectGroup.h"

using namespace std;

namespace Magnum { namespace Physics {

template<uint8_t dimensions> ShapedObject<dimensions>::ShapedObject(ShapedObjectGroup<dimensions>* group, typename SceneGraph::AbstractObject<dimensions>::ObjectType* parent): SceneGraph::AbstractObject<dimensions>::ObjectType(parent), group(group), _shape(nullptr) {
    group->objects.push_back(this);
}

template<uint8_t dimensions> ShapedObject<dimensions>::~ShapedObject() {
    group->objects.erase(find(group->objects.begin(), group->objects.end(), this));
    delete _shape;
}

template<uint8_t dimensions> void ShapedObject<dimensions>::setDirty() {
    SceneGraph::AbstractObject<dimensions>::ObjectType::setDirty();

    group->setDirty();
}

template<uint8_t dimensions> void ShapedObject<dimensions>::clean(const typename DimensionTraits<dimensions, GLfloat>::MatrixType& absoluteTransformation) {
    SceneGraph::AbstractObject<dimensions>::ObjectType::clean(absoluteTransformation);

    if(_shape) _shape->applyTransformation(absoluteTransformation);
}

template class ShapedObject<2>;
template class ShapedObject<3>;

}}
