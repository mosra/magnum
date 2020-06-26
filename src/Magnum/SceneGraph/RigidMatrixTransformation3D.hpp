#ifndef Magnum_SceneGraph_RigidMatrixTransformation3D_hpp
#define Magnum_SceneGraph_RigidMatrixTransformation3D_hpp
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief @ref compilation-speedup-hpp "Template implementation" for @ref RigidMatrixTransformation3D.hpp
 * @m_since{2020,06}
 */

#include "RigidMatrixTransformation3D.h"

#include "Magnum/Math/Quaternion.h"

namespace Magnum { namespace SceneGraph {

/* These are here to avoid including Quaternion in the header */

template<class T> Object<BasicRigidMatrixTransformation3D<T>>& BasicRigidMatrixTransformation3D<T>::rotate(const Math::Quaternion<T>& quaternion) {
    return transform(Matrix4::from(quaternion.toMatrix(), {}));
}

template<class T> Object<BasicRigidMatrixTransformation3D<T>>& BasicRigidMatrixTransformation3D<T>::rotateLocal(const Math::Quaternion<T>& quaternion) {
    return transformLocal(Matrix4::from(quaternion.toMatrix(), {}));
}

}}

#endif
