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

#include "ObjectShape.h"

#include <algorithm>

#include "AbstractShape.h"
#include "ObjectShapeGroup.h"

namespace Magnum { namespace Physics {

template<std::uint8_t dimensions> ObjectShape<dimensions>::ObjectShape(SceneGraph::AbstractObject<dimensions>* object, ObjectShapeGroup<dimensions>* group): SceneGraph::AbstractGroupedFeature<dimensions, ObjectShape<dimensions>>(object, group), _shape(nullptr) {
    this->setCachedTransformations(SceneGraph::AbstractFeature<dimensions>::CachedTransformation::Absolute);
}

template<std::uint8_t dimensions> ObjectShape<dimensions>::~ObjectShape() {
    delete _shape;
}

template<std::uint8_t dimensions> void ObjectShape<dimensions>::markDirty() {
    group()->setDirty();
}

template<std::uint8_t dimensions> void ObjectShape<dimensions>::clean(const typename DimensionTraits<dimensions>::MatrixType& absoluteTransformationMatrix) {
    if(_shape) _shape->applyTransformationMatrix(absoluteTransformationMatrix);
}

template class ObjectShape<2>;
template class ObjectShape<3>;

}}
