#ifndef Magnum_SceneGraph_Camera3D_hpp
#define Magnum_SceneGraph_Camera3D_hpp
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

/** @file
 * @brief @ref compilation-speedup-hpp "Template implementation" for Camera3D.h
 */

#include "AbstractCamera.hpp"
#include "Camera3D.h"

using namespace std;

namespace Magnum { namespace SceneGraph {

template<class T> Camera3D<T>* Camera3D<T>::setOrthographic(const Math::Vector2<T>& size, T near, T far) {
    /** @todo Get near/far from the matrix */
    _near = near;
    _far = far;

    AbstractCamera<3, T>::rawProjectionMatrix = Math::Matrix4<T>::orthographicProjection(size, near, far);
    AbstractCamera<3, T>::fixAspectRatio();
    return this;
}

template<class T> Camera3D<T>* Camera3D<T>::setPerspective(const Math::Vector2<T>& size, T near, T far) {
    /** @todo Get near/far from the matrix */
    _near = near;
    _far = far;

    AbstractCamera<3, T>::rawProjectionMatrix = Math::Matrix4<T>::perspectiveProjection(size, near, far);
    AbstractCamera<3, T>::fixAspectRatio();
    return this;
}

template<class T> Camera3D<T>* Camera3D<T>::setPerspective(T fov, T aspectRatio, T near, T far) {
    /** @todo Get near/far from the matrix */
    _near = near;
    _far = far;

    AbstractCamera<3, T>::rawProjectionMatrix = Math::Matrix4<T>::perspectiveProjection(fov, aspectRatio, near, far);
    AbstractCamera<3, T>::fixAspectRatio();
    return this;
}

}}

#endif
