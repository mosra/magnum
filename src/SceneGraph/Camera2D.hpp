#ifndef Magnum_SceneGraph_Camera2D_hpp
#define Magnum_SceneGraph_Camera2D_hpp
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
 * @brief @ref compilation-speedup-hpp "Template implementation" for Camera2D.h
 */

#include "AbstractCamera.hpp"
#include "Camera2D.h"

using namespace std;

namespace Magnum { namespace SceneGraph {

template<class T> Camera2D<T>::Camera2D(AbstractObject<2, T>* object): AbstractCamera<2, T>(object) {}

template<class T> Camera2D<T>* Camera2D<T>::setProjection(const Math::Vector2<T>& size) {
    AbstractCamera<2, T>::rawProjectionMatrix = Math::Matrix3<T>::projection(size);

    AbstractCamera<2, T>::fixAspectRatio();
    return this;
}

}}

#endif
