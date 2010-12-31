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

#include "Camera.h"
#include "Scene.h"

namespace Magnum {

Camera::Camera(Object* parent): Object(parent), _active(0), viewportWidth(0), viewportHeight(0), _aspectRatioPolicy(Extend) {
    setOrthographic(2, 1, 1000);
}

void Camera::setActive(Scene* _scene) {
    if(_scene == _active || scene() != _scene) return;

    Scene* oldActive = _active;

    /* Set camera active in new scene */
    _active = _scene;
    if(_active) _active->setCamera(this);

    /* Remove the camera from current active scene, if the camera is still
       active there */
    if(oldActive && oldActive->camera() == this) oldActive->setCamera(0);

    /* Clean the path to scene */
    setClean();
}

void Camera::setOrthographic(GLfloat size, GLfloat near, GLfloat far) {
    /* Scale the volume down so it fits in (-1, 1) in all directions */
    GLfloat xyScale = 2/size;
    GLfloat zScale = 2/(far-near);
    rawProjectionMatrix = Matrix4::scaling(xyScale, xyScale, -zScale);

    /* Move the volume on z into (-1, 1) range */
    rawProjectionMatrix = Matrix4::translation(0, 0, -1-near*zScale)*rawProjectionMatrix;

    fixAspectRatio();
}

void Camera::setPerspective(GLfloat fov, GLfloat near, GLfloat far) {
    /* First move the volume on z in (-1, 1) range */
    rawProjectionMatrix = Matrix4::translation(0, 0, 2*far*near/(far+near));

    /* Then apply magic perspective matrix (with reversed Z) */
    static GLfloat a[] = {  1, 0,  0,  0,
                            0, 1,  0,  0,
                            0, 0,  -1,  -1,
                            0, 0,  0,  0  };
    rawProjectionMatrix = Matrix4(a)*rawProjectionMatrix;

    /* Then scale the volume down so it fits in (-1, 1) in all directions */
    GLfloat xyScale = 1/tan(fov/2);
    GLfloat zScale = 1+2*near/(far-near);
    rawProjectionMatrix = Matrix4::scaling(xyScale, xyScale, zScale)*rawProjectionMatrix;

    /* And... another magic */
    rawProjectionMatrix.set(3, 3, 0);

    fixAspectRatio();
}

void Camera::setViewport(int width, int height) {
    glViewport(0, 0, width, height);

    viewportWidth = width;
    viewportHeight = height;
    fixAspectRatio();
}

void Camera::setClean() {
    if(!isDirty()) return;
    _cameraMatrix = transformation(true).inverse();
    Object::setClean();
}

void Camera::setDirty() {
    Object::setDirty();

    /* Camera is active */
    if(_active) {
        Scene* currentScene = scene();

        /* Camera is not part of the scene anymore, remove it from there */
        if(!currentScene) _active->setCamera(0);

        /* Otherwise set the scene dirty */
        else _active->setDirty();

        /* Clean up the path to scene immediately */
        setClean();
    }
}

void Camera::fixAspectRatio() {
    /* Don't divide by zero */
    if(viewportWidth == 0 || viewportHeight == 0) {
        _projectionMatrix = rawProjectionMatrix;
        return;
    }

    /* Extend on larger side = scale larger side down */
    if(_aspectRatioPolicy == Extend) {
        _projectionMatrix = ((viewportWidth > viewportHeight) ?
            Matrix4::scaling(static_cast<GLfloat>(viewportHeight)/viewportWidth, 1, 1) :
            Matrix4::scaling(1, static_cast<GLfloat>(viewportWidth)/viewportHeight, 1)
        )*rawProjectionMatrix;

    /* Clip on smaller side = scale smaller side up */
    } else if(_aspectRatioPolicy == Clip) {
        _projectionMatrix = ((viewportWidth > viewportHeight) ?
            Matrix4::scaling(1, static_cast<GLfloat>(viewportWidth)/viewportHeight, 1) :
            Matrix4::scaling(static_cast<GLfloat>(viewportHeight)/viewportWidth, 1, 1)
        )*rawProjectionMatrix;

    /* Don't preserve anything */
    } else _projectionMatrix = rawProjectionMatrix;
}

}
