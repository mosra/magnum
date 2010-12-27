/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "AbstractObject.h"

using namespace std;

namespace Magnum {

void AbstractObject::setParent(AbstractObject* parent) {
    if(_parent == parent) return;

    /* Remove the object from old parent children list */
    if(_parent != 0)
        _parent->_children.erase(this);

    /* Set new parent and add the object to new parent children list */
    if(parent != 0) {
        parent->_children.insert(this);
    }

    _parent = parent;
}

AbstractObject::~AbstractObject() {
    /* Remove the object from parent's children */
    setParent(0);

    /* Delete all children */
    for(set<AbstractObject*>::const_iterator it = _children.begin(); it != _children.end(); ++it)
        delete *it;
}

}
