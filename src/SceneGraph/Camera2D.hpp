#ifndef Magnum_SceneGraph_Camera2D_hpp
#define Magnum_SceneGraph_Camera2D_hpp
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
 * @brief @ref compilation-speedup-hpp "Template implementation" for Camera2D.h
 */

#include "AbstractCamera.hpp"
#include "Camera2D.h"

using namespace std;

namespace Magnum { namespace SceneGraph {

template<class T> Camera2D<T>* Camera2D<T>::setProjection(const Math::Vector2<T>& size) {
    AbstractCamera<2, T>::rawProjectionMatrix = Math::Matrix3<T>::projection(size);

    AbstractCamera<2, T>::fixAspectRatio();
    return this;
}

}}

#endif
