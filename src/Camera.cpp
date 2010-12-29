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

namespace Magnum {

Camera::Camera(AbstractObject* parent): AbstractObject(parent), viewportWidth(0), viewportHeight(0), _aspectRatioPolicy(Extend) {
    setOrthographic(2, 1, 1000);
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

Matrix4 Camera::cameraMatrix() const {
    /** @todo How to do that? */
    return Matrix4();
}

void Camera::setViewport(int width, int height) {
    glViewport(0, 0, width, height);

    viewportWidth = width;
    viewportHeight = height;
    fixAspectRatio();
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
