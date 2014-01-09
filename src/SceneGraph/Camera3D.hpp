#ifndef Magnum_SceneGraph_Camera3D_hpp
#define Magnum_SceneGraph_Camera3D_hpp
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief @ref compilation-speedup-hpp "Template implementation" for @ref Camera3D.h
 */

#include "AbstractCamera.hpp"
#include "Camera3D.h"

/** @todo Use AbstractBasicCamera3D<T> when support for GCC 4.6 is dropped (also in header) */

namespace Magnum { namespace SceneGraph {

template<class T> BasicCamera3D<T>::BasicCamera3D(AbstractObject<3, T>& object): AbstractCamera<3, T>(object), _near(T(0)), _far(T(0)) {}

template<class T> BasicCamera3D<T>& BasicCamera3D<T>::setOrthographic(const Math::Vector2<T>& size, T near, T far) {
    /** @todo Get near/far from the matrix */
    _near = near;
    _far = far;

    AbstractCamera<3, T>::rawProjectionMatrix = Math::Matrix4<T>::orthographicProjection(size, near, far);
    AbstractCamera<3, T>::fixAspectRatio();
    return *this;
}

template<class T> BasicCamera3D<T>& BasicCamera3D<T>::setPerspective(const Math::Vector2<T>& size, T near, T far) {
    /** @todo Get near/far from the matrix */
    _near = near;
    _far = far;

    AbstractCamera<3, T>::rawProjectionMatrix = Math::Matrix4<T>::perspectiveProjection(size, near, far);
    AbstractCamera<3, T>::fixAspectRatio();
    return *this;
}

template<class T> BasicCamera3D<T>& BasicCamera3D<T>::setPerspective(Math::Rad<T> fov, T aspectRatio, T near, T far) {
    /** @todo Get near/far from the matrix */
    _near = near;
    _far = far;

    AbstractCamera<3, T>::rawProjectionMatrix = Math::Matrix4<T>::perspectiveProjection(fov, aspectRatio, near, far);
    AbstractCamera<3, T>::fixAspectRatio();
    return *this;
}

}}

#endif
