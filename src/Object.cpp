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
#include "Scene.h"

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

    setDirty();
}

Matrix4 Object::transformation(bool absolute) {
    if(!absolute) return _transformation;

    Matrix4 t = _transformation;

    Object* p = parent();
    while(p != 0) {
        t = p->transformation()*t;

        /* We got to the scene, multiply with camera matrix */
        if(p->parent() == p) {
            Camera* camera = static_cast<Scene*>(p)->camera();
            if(camera && camera != this) t = camera->cameraMatrix()*t;

            break;
        }

        p = p->parent();
    }

    return t;
}

Object::~Object() {
    /* Remove the object from parent's children */
    setParent(0);

    /* Delete all children */
    for(set<Object*>::const_iterator it = _children.begin(); it != _children.end(); ++it)
        delete *it;
}

Scene* Object::scene() const {
    /* Goes up the family tree until it finds object which is parent of itself
       (that's the scene) */
    Object* p = parent();
    while(p != 0) {
        if(p->parent() == p) return static_cast<Scene*>(p);
        p = p->parent();
    }

    return 0;
}

void Object::setDirty() {
    /* The object (and all its children) are already dirty, nothing to do */
    if(dirty) return;

    dirty = true;

    /* Make all children dirty */
    for(set<Object*>::iterator it = _children.begin(); it != _children.end(); ++it)
        (*it)->setDirty();
}

void Object::setClean() {
    /* The object (and all its parents) is already clean, nothing to do */
    if(!dirty) return;

    dirty = false;

    /* Make all parents clean */
    if(_parent != 0 && _parent != this) _parent->setClean();
}

}
