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

Matrix4 Camera::cameraMatrix() const {
    /** @todo How to do that? */
    return Matrix4();
}

void Camera::setViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

}
