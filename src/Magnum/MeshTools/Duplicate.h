#ifndef Magnum_MeshTools_Duplicate_h
#define Magnum_MeshTools_Duplicate_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Function @ref Magnum::MeshTools::duplicate(), @ref Magnum::MeshTools::duplicateInto()
 */

#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Assert.h>

#include "Magnum/Magnum.h"

namespace Magnum { namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Fwdecl so we can have duplicateInto() ordered after duplicate() */
template<class IndexType, class T> void duplicateInto(const Containers::StridedArrayView1D<const IndexType>&, const Containers::StridedArrayView1D<const T>&, const Containers::StridedArrayView1D<T>&);
#endif

/**
@brief Duplicate data using given index array
@m_since{2019,10}

Converts indexed array to non-indexed, for example data `{a, b, c, d}` with
index array `{1, 1, 0, 3, 2, 2}` will be converted to `{b, b, a, d, c, c}`.
The resulting array size is the same as size of @p indices, expects that all
indices are in range for the @p data array.

If you want to fill an existing memory (or, for example a @ref std::vector),
use @ref duplicateInto().
@see @ref removeDuplicates(), @ref combineIndexedArrays()
*/
template<class IndexType, class T> Containers::Array<T> duplicate(const Containers::StridedArrayView1D<const IndexType>& indices, const Containers::StridedArrayView1D<const T>& data) {
    Containers::Array<T> out{Containers::NoInit, indices.size()};
    duplicateInto<IndexType, T>(indices, data, out);
    return out;
}

/**
@brief Duplicate data using given index array

Like @ref duplicate(const Containers::StridedArrayView1D<const IndexType>&, const Containers::StridedArrayView1D<const T>&),
but putting the result into a @ref std::vector.
*/
template<class T> std::vector<T> duplicate(const std::vector<UnsignedInt>& indices, const std::vector<T>& data) {
    std::vector<T> out(indices.size());
    duplicateInto<UnsignedInt, T>(indices, data, out);
    return out;
}

/**
@brief Duplicate data using an index array into given output array
@param[in]  indices Index array to use
@param[in]  data    Input data
@param[out] out     Where to store the output
@m_since{2019,10}

A variant of @ref duplicate() that fills existing memory instead of allocating
a new array.
*/
template<class IndexType, class T> void duplicateInto(const Containers::StridedArrayView1D<const IndexType>& indices, const Containers::StridedArrayView1D<const T>& data, const Containers::StridedArrayView1D<T>& out) {
    CORRADE_ASSERT(out.size() == indices.size(),
        "MeshTools::duplicateInto(): bad output size, expected" << indices.size() << "but got" << out.size(), );
    for(std::size_t i = 0; i != indices.size(); ++i) {
        const std::size_t index = indices[i];
        CORRADE_ASSERT(index < data.size(), "MeshTools::duplicateInto(): index" << index << "out of bounds for" << data.size() << "elements", );
        out[i] = data[index];
    }
}

}}

#endif
