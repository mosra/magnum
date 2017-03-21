#ifndef Magnum_MeshTools_Duplicate_h
#define Magnum_MeshTools_Duplicate_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Function @ref Magnum::MeshTools::duplicate()
 */

#include <vector>
#include <Corrade/Utility/Assert.h>

#include "Magnum/Types.h"

namespace Magnum { namespace MeshTools {

/**
@brief Duplicate data using index array

Converts indexed array to non-indexed, for example data `{a, b, c, d}` with
index array `{1, 1, 0, 3, 2, 2}` will be converted to `{b, b, a, d, c, c}`.
@see @ref removeDuplicates(), @ref combineIndexedArrays()
*/
template<class T> std::vector<T> duplicate(const std::vector<UnsignedInt>& indices, const std::vector<T>& data) {
    std::vector<T> out;
    out.reserve(indices.size());
    for(const UnsignedInt index: indices) {
        CORRADE_ASSERT(index < data.size(), "MeshTools::duplicate(): index out of range", out);
        out.push_back(data[index]);
    }
    return out;
}

}}

#endif
