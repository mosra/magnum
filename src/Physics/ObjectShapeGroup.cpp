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

#include "ObjectShapeGroup.h"

#include "Physics/AbstractShape.h"
#include "Physics/ObjectShape.h"

namespace Magnum { namespace Physics {

template<std::uint8_t dimensions> void ObjectShapeGroup<dimensions>::setClean() {
    /* Clean all objects */
    if(!this->isEmpty()) {
        std::vector<SceneGraph::AbstractObject<dimensions>*> objects(this->size());
        for(std::size_t i = 0; i != this->size(); ++i)
            objects[i] = (*this)[i]->object();

        objects[0]->setClean(objects);
    }

    dirty = false;
}

template<std::uint8_t dimensions> ObjectShape<dimensions>* ObjectShapeGroup<dimensions>::firstCollision(const ObjectShape<dimensions>* shape) {
    /* Nothing to test with, done */
    if(!shape->shape()) return nullptr;

    setClean();
    for(std::size_t i = 0; i != this->size(); ++i)
        if((*this)[i] != shape && (*this)[i]->shape() && (*this)[i]->shape()->collides(shape->shape()))
            return (*this)[i];

    return nullptr;
}

template class ObjectShapeGroup<2>;
template class ObjectShapeGroup<3>;

}}
