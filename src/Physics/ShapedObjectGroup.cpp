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

#include "ShapedObjectGroup.h"

#include "ShapedObject.h"

namespace Magnum { namespace Physics {

template<size_t dimensions> void ShapedObjectGroup<dimensions>::setClean() {
    for(ShapedObject<dimensions>* object: objects)
        if(object->isDirty()) object->setClean();

    dirty = false;
}

template class ShapedObjectGroup<2>;
template class ShapedObjectGroup<3>;

}}
