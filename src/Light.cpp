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

#include "Light.h"
#include "Camera.h"

namespace Magnum {

Vector3 Light::position(Camera* camera) {
    CORRADE_ASSERT(scene() && camera->scene() == scene(), "Light: camera and light aren't in the same scene!", Vector3())

    if(camera != _camera) {
        _camera = camera;
        setDirty();
    }

    setClean();
    return _position;
}

void Light::clean(const Matrix4& absoluteTransformation) {
    Object::clean(absoluteTransformation);

    _position = (absoluteTransformation*_camera->cameraMatrix())[3].xyz();
}

}
