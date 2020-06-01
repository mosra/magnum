#ifndef Magnum_SceneGraph_AbstractFeature_hpp
#define Magnum_SceneGraph_AbstractFeature_hpp
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
 * @brief @ref compilation-speedup-hpp "Template implementation" for @ref AbstractFeature.h
 */

#include "Magnum/SceneGraph/AbstractFeature.h"

namespace Magnum { namespace SceneGraph {

template<UnsignedInt dimensions, class T> AbstractFeature<dimensions, T>::AbstractFeature(AbstractObject<dimensions, T>& object) {
    object.Containers::template LinkedList<AbstractFeature<dimensions, T>>::insert(this);
}

template<UnsignedInt dimensions, class T> AbstractFeature<dimensions, T>::~AbstractFeature() = default;

template<UnsignedInt dimensions, class T> void AbstractFeature<dimensions, T>::markDirty() {}

template<UnsignedInt dimensions, class T> void AbstractFeature<dimensions, T>::clean(const MatrixTypeFor<dimensions, T>&) {}

template<UnsignedInt dimensions, class T> void AbstractFeature<dimensions, T>::cleanInverted(const MatrixTypeFor<dimensions, T>&) {}

}}

#endif
