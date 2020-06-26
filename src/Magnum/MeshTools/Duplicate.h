#ifndef Magnum_MeshTools_Duplicate_h
#define Magnum_MeshTools_Duplicate_h
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
 * @brief Function @ref Magnum::MeshTools::duplicate(), @ref Magnum::MeshTools::duplicateInto()
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#include <Corrade/Containers/ArrayViewStl.h>
#endif

namespace Magnum { namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Fwdecl so we can have duplicateInto() ordered after duplicate() */
template<class IndexType, class T> void duplicateInto(const Containers::StridedArrayView1D<const IndexType>&, const Containers::StridedArrayView1D<const T>&, const Containers::StridedArrayView1D<T>&);
#endif

/**
@brief Duplicate data using given index array
@m_since{2019,10}

Converts indexed array to non-indexed, for example data @cpp {a, b, c, d} @ce
with index array @cpp {1, 1, 0, 3, 2, 2} @ce will be converted to
@cpp {b, b, a, d, c, c} @ce. The resulting array size is the same as size of
@p indices, expects that all indices are in range for the @p data array.

If you want to fill an existing memory (or, for example a @ref std::vector),
use @ref duplicateInto().
@see @ref removeDuplicatesInPlace(), @ref combineIndexedAttributes()
*/
template<class IndexType, class T> Containers::Array<T> duplicate(const Containers::StridedArrayView1D<const IndexType>& indices, const Containers::StridedArrayView1D<const T>& data) {
    Containers::Array<T> out{Containers::NoInit, indices.size()};
    duplicateInto<IndexType, T>(indices, data, out);
    return out;
}

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Duplicate data using given index array
@m_deprecated_since{2020,06} Use @ref duplicate(const Containers::StridedArrayView1D<const IndexType>&, const Containers::StridedArrayView1D<const T>&)
    or @ref duplicateInto() instead.
*/
template<class T> CORRADE_DEPRECATED("use duplicate() taking a StridedArrayView instead") std::vector<T> duplicate(const std::vector<UnsignedInt>& indices, const std::vector<T>& data) {
    std::vector<T> out(indices.size());
    duplicateInto<UnsignedInt, T>(indices, data, out);
    return out;
}
#endif

/**
@brief Duplicate data using an index array into given output array
@param[in]  indices Index array to use
@param[in]  data    Input data
@param[out] out     Where to store the output
@m_since{2019,10}

A variant of @ref duplicate() that fills existing memory instead of allocating
a new array. Expects that @p out has the same size as @p indices and all
indices are in range for the @p data array.
*/
template<class IndexType, class T> void duplicateInto(const Containers::StridedArrayView1D<const IndexType>& indices, const Containers::StridedArrayView1D<const T>& data, const Containers::StridedArrayView1D<T>& out);

/**
@brief Duplicate type-erased data using an index array into given output array
@param[in]  indices Index array to use
@param[in]  data    Input data
@param[out] out     Where to store the output
@m_since{2020,06}

Compared to @ref duplicateInto(const Containers::StridedArrayView1D<const IndexType>&, const Containers::StridedArrayView1D<const T>&, const Containers::StridedArrayView1D<T>&)
accepts a 2D view, where the second dimension spans the actual type. Expects
that @p out has the same size as @p indices and all indices are in range for
the @p data array, and that the second dimension of both @p data and @p out
is contiguous and has the same size.
*/
MAGNUM_MESHTOOLS_EXPORT void duplicateInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView2D<char>& out);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT void duplicateInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView2D<char>& out);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT void duplicateInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView2D<char>& out);

/**
@brief Duplicate type-erased data using a type-erased index array into given output array
@m_since{2020,06}

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref duplicateInto(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView2D<const char>&, const Containers::StridedArrayView2D<char>&)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT void duplicateInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView2D<char>& out);

/**
@brief Duplicate indexed mesh data
@m_since{2020,06}

Returns a copy of @p data that's not indexed and has all attributes interleaved
and duplicated according to @p data's index buffer. The @p extra attributes, if
any, are duplicated and interleaved together with existing attributes (or, in
case the attribute view is empty, only the corresponding space for given
attribute type is reserved, with memory left uninitialized). The data layouting
is done by @ref interleavedLayout(), see its documentation for detailed
behavior description. Note that offset-only @ref Trade::MeshAttributeData
instances are not supported in the @p extra array.

Expects that @p data is indexed and each attribute in @p extra has either the
same amount of elements as @p data vertex count (*not* index count) or has
none.
@see @ref Trade::MeshData::attributeData()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData duplicate(const Trade::MeshData& data, Containers::ArrayView<const Trade::MeshAttributeData> extra = {});

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData duplicate(const Trade::MeshData& data, std::initializer_list<Trade::MeshAttributeData> extra);

template<class IndexType, class T> inline void duplicateInto(const Containers::StridedArrayView1D<const IndexType>& indices, const Containers::StridedArrayView1D<const T>& data, const Containers::StridedArrayView1D<T>& out) {
    duplicateInto(indices, Containers::arrayCast<2, const char>(data), Containers::arrayCast<2, char>(out));
}

}}

#endif
