#ifndef Magnum_SceneGraph_FeatureGroup_hpp
#define Magnum_SceneGraph_FeatureGroup_hpp
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief @ref compilation-speedup-hpp "Template implementation" for @ref FeatureGroup.h
 */

#include <algorithm>

#include "Magnum/SceneGraph/FeatureGroup.h"

namespace Magnum { namespace SceneGraph {

/* `= default` causes linker errors in GCC 4.5 */
template<UnsignedInt dimensions, class T> AbstractFeatureGroup<dimensions, T>::AbstractFeatureGroup() {}

/* `= default` causes linker errors in GCC 4.4 */
template<UnsignedInt dimensions, class T> AbstractFeatureGroup<dimensions, T>::~AbstractFeatureGroup() {}

template<UnsignedInt dimensions, class T> void AbstractFeatureGroup<dimensions, T>::add(AbstractFeature<dimensions, T>& feature) {
    /* GCC 4.4 has explicit constructor for std::reference_wrapper. WHY ON EARTH. WHY. */
    features.push_back(std::ref(feature));
}

namespace {
    template<UnsignedInt dimensions, class T> struct PointerCompare {
        PointerCompare(AbstractFeature<dimensions, T>& feature): feature(feature) {}

        bool operator()(AbstractFeature<dimensions, T>& f) {
            return &f == &feature;
        };

        private:
            AbstractFeature<dimensions, T>& feature;
    };
}

template<UnsignedInt dimensions, class T> void AbstractFeatureGroup<dimensions, T>::remove(AbstractFeature<dimensions, T>& feature) {
    features.erase(std::find_if(features.begin(), features.end(), PointerCompare<dimensions, T>{feature}));
}

}}

#endif
