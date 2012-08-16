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

#include <stack>

#include "Scene.h"
#include "Camera.h"

using namespace std;
using namespace Magnum::Math;

namespace Magnum { namespace SceneGraph {

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> ObjectType* Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>::setParent(ObjectType* parent) {
    /* Skip if nothing to do or this is scene */
    if(_parent == parent || _parent == this) return static_cast<ObjectType*>(this);

    /* Add the object to children list of new parent */
    if(parent != nullptr) {

        /* Only Fry can be his own grandfather */
        ObjectType* p = parent;
        while(p != nullptr && p->parent() != p) {
            if(p == this) return static_cast<ObjectType*>(this);
            p = p->parent();
        }

        parent->_children.insert(static_cast<ObjectType*>(this));
    }

    /* Remove the object from old parent children list */
    if(_parent != nullptr)
        _parent->_children.erase(static_cast<ObjectType*>(this));

    /* Set new parent */
    _parent = parent;

    setDirty();
    return static_cast<ObjectType*>(this);
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> MatrixType Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>::absoluteTransformation(CameraType* camera) {
    /* Shortcut for absolute transformation of camera relative to itself */
    if(camera == this) return MatrixType();

    MatrixType t = _transformation;

    ObjectType* p = parent();
    while(p != nullptr) {
        t = p->transformation()*t;

        /* We got to the scene, multiply with camera matrix */
        if(p->parent() == p) {
            if(camera) {
                CORRADE_ASSERT(camera->scene() == scene(), "Object::absoluteTransformation(): the camera is not part of the same scene as object!", t);
                t = camera->cameraMatrix()*t;
            }

            break;
        }

        p = p->parent();
    }

    CORRADE_ASSERT(p != nullptr || camera == nullptr, "Object::absoluteTransformation(): the object is not part of camera scene!", t);

    return t;
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>::~Object() {
    /* Remove the object from parent's children */
    setParent(nullptr);

    /* Delete all children */
    while(!_children.empty())
        delete *_children.begin();
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> SceneType* Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>::scene() {
    /* Goes up the family tree until it finds object which is parent of itself
       (that's the scene) */
    ObjectType* p = parent();
    while(p != nullptr) {
        if(p->parent() == p) return static_cast<SceneType*>(p);
        p = p->parent();
    }

    return nullptr;
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> ObjectType* Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>::setTransformation(const MatrixType& transformation) {
    if(_parent == this) return static_cast<ObjectType*>(this);

    _transformation = transformation;
    setDirty();
    return static_cast<ObjectType*>(this);
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> void Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>::setDirty() {
    /* The object (and all its children) are already dirty, nothing to do */
    if(dirty) return;

    dirty = true;

    /* Make all children dirty */
    for(typename set<ObjectType*>::iterator it = _children.begin(); it != _children.end(); ++it)
        (*it)->setDirty();
}

template<class MatrixType, class VectorType, class ObjectType, class SceneType, class CameraType> void Object<MatrixType, VectorType, ObjectType, SceneType, CameraType>::setClean() {
    /* The object (and all its parents) are already clean, nothing to do */
    if(!dirty) return;

    /* Collect all parents */
    stack<ObjectType*> objects;
    ObjectType* p = static_cast<ObjectType*>(this);
    for(;;) {
        objects.push(p);

        /* Stop on root object / scene / clean object */
        if(p->parent() == nullptr || p->parent() == p || !p->parent()->isDirty())
            break;

        p = p->parent();
    }

    /* Call setClean(const Matrix4&) for every parent and also this object */
    ObjectType* o = objects.top();
    objects.pop();
    MatrixType absoluteTransformation = o->absoluteTransformation();
    o->clean(absoluteTransformation);
    while(!objects.empty()) {
        o = objects.top();
        objects.pop();
        absoluteTransformation = absoluteTransformation*o->transformation();
        o->clean(absoluteTransformation);
    }
}

/* Explicitly instantiate the templates */
template class Object<Matrix3, Vector2, Object2D, Scene2D, Camera2D>;
template class Object<Matrix4, Vector3, Object3D, Scene3D, Camera3D>;

}}
