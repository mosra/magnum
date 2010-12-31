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

#include "Object.h"

using namespace std;

namespace Magnum {

void Object::setParent(Object* parent) {
    if(_parent == parent) return;

    /* Set new parent and add the object to new parent children list */
    if(parent != 0) {

        /* Only Fry can be his own grandfather */
        Object* p = parent;
        while(p != 0 && p->parent() != p) {
            if(p == this) return;
            p = p->parent();
        }

        parent->_children.insert(this);
    }

    /* Remove the object from old parent children list */
    if(_parent != 0)
        _parent->_children.erase(this);

    _parent = parent;
}

Object::~Object() {
    /* Remove the object from parent's children */
    setParent(0);

    /* Delete all children */
    for(set<Object*>::const_iterator it = _children.begin(); it != _children.end(); ++it)
        delete *it;
}

}
