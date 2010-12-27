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

#include "Scene.h"

using namespace std;

namespace Magnum {

void Scene::setFeature(Scene::Feature feature, bool enabled) {
    GLenum _feature;
    switch(feature) {
        case AlphaBlending: _feature = GL_BLEND;        break;
        case DepthTest:     _feature = GL_DEPTH_TEST;   break;
        case FaceCulling:   _feature = GL_CULL_FACE;    break;
        default: return;
    }

    enabled ? glEnable(_feature) : glDisable(_feature);
}

void Scene::setClearColor(const Magnum::Vector4& color) {
    glClearColor(color.r(), color.g(), color.b(), color.a());
    _clearColor = color;
}

void Scene::setCamera(Camera* camera) {
    camera->setViewport(viewportWidth, viewportHeight);
    _camera = camera;
}

void Scene::draw() {
    /* No camera available, nothing to do */
    if(!_camera) return;

    /** @todo Clear only set features */
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    /* Recursively draw child objects */
    drawChildren(this, _camera->cameraMatrix());
}

void Scene::drawChildren(AbstractObject* object, const Matrix4& transformationMatrix) {
    for(set<AbstractObject*>::const_iterator it = object->children().begin(); it != object->children().end(); ++it) {
        /* Transformation matrix for the object */
        Matrix4 matrix = (*it)->transformation()*transformationMatrix;

        /* Draw the object and its children */
        (*it)->draw(matrix, _camera->projectionMatrix());
        drawChildren(*it, matrix);
    }
}

}
