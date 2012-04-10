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

#include "Scene.h"

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

}
