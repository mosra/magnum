#ifndef Magnum_MeshTools_Duplicate_h
#define Magnum_MeshTools_Duplicate_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
 * @brief Function Magnum::MeshTools::duplicate()
 */

#include <vector>

#include "Magnum/Types.h"

namespace Magnum { namespace MeshTools {

/**
@brief Duplicate vertices using index array

Converts indexed array to non-indexed, for example vertices `{a, b, c, d}` with
index array `{1, 1, 0, 3, 2, 2}` will be converted to `{b, b, a, d, c, c}`.
@see removeDuplicates()
*/
template<class T> std::vector<T> duplicate(const std::vector<UnsignedInt>& indices, const std::vector<T>& vertices) {
    std::vector<T> out;
    out.reserve(indices.size());
    for(auto it = indices.begin(); it != indices.end(); ++it)
        out.push_back(vertices[*it]);
    return std::move(out);
}

}}

#endif
