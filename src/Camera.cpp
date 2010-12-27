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
