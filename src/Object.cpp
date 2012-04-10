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

#include "Object.h"
#include "Scene.h"
#include "Camera.h"

using namespace std;

namespace Magnum {

void Object::setParent(Object* parent) {
    /* Skip if nothing to do or this is scene */
    if(_parent == parent || _parent == this) return;

    /* Add the object to children list of new parent */
    if(parent != nullptr) {

        /* Only Fry can be his own grandfather */
        Object* p = parent;
        while(p != nullptr && p->parent() != p) {
            if(p == this) return;
            p = p->parent();
        }

        parent->_children.insert(this);
    }

    /* Remove the object from old parent children list */
    if(_parent != nullptr)
        _parent->_children.erase(this);

    /* Set new parent */
    _parent = parent;

    setDirty();
}

Matrix4 Object::absoluteTransformation(Camera* camera) {
    Matrix4 t = _transformation;

    /* Shortcut for absolute transformation of camera relative to itself */
    if(camera == this) return Matrix4();

    Object* p = parent();
    while(p != nullptr) {
        t = p->transformation()*t;

        /* We got to the scene, multiply with camera matrix */
        if(p->parent() == p) {
            if(camera && camera->scene() == scene())
                t = camera->cameraMatrix()*t;

            break;
        }

        p = p->parent();
    }

    return t;
}

Object::~Object() {
    /* Remove the object from parent's children */
    setParent(nullptr);

    /* Delete all children */
    while(!_children.empty())
        delete *_children.begin();
}

Scene* Object::scene() {
    /* Goes up the family tree until it finds object which is parent of itself
       (that's the scene) */
    Object* p = parent();
    while(p != nullptr) {
        if(p->parent() == p) return static_cast<Scene*>(p);
        p = p->parent();
    }

    return nullptr;
}

void Object::setTransformation(const Matrix4& transformation) {
    if(_parent == this) return;

    _transformation = transformation;
    setDirty();
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
    if(_parent != nullptr && _parent != this) _parent->setClean();
}

}
