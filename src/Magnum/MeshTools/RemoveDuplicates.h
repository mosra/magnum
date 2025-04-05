#ifndef Magnum_MeshTools_RemoveDuplicates_h
#define Magnum_MeshTools_RemoveDuplicates_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Function @ref Magnum::MeshTools::removeDuplicatesInPlace(), @ref Magnum::MeshTools::removeDuplicatesIndexedInPlace()
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/TypeTraits.h"
#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>

/* The function used to return a std::pair */
#include <Corrade/Containers/PairStl.h>
#endif

namespace Magnum { namespace MeshTools {

/**
@brief Remove duplicate data from given array in-place
@param[in,out] data Data array. Unique items get moved to the front, preserving
    their relative order.
@return Resulting index array and size of the unique prefix in the processed
    @p data array
@m_since{2020,06}

Removes duplicate data from given array by comparing the second dimension of
each item. Usage example:

@snippet MeshTools.cpp removeDuplicates

For a visual example, assuming the following (two-dimensional) input, the
function puts the four unique items at the front of the array, preserving their
order. The returned index array then maps the unique back to the input
locations, the second returned value is the count of unique items left in the
input, thus four:

@code{.cpp}
// Original input
{{172, 26}, {212, -183}, {172, 26}, {42, 13}, {112, 9}, {112, 9}, {212, -183}}

// After processing, contents following the unique prefix are left unspecified
{{172, 26}, {212, -183}, {42, 13}, {112, 9}, ...}

// Matching index array and number of unique items
{{0, 1, 0, 2, 3, 3, 1}, 4}
@endcode

The second dimension is expected to be contiguous. A plain bit-exact
matching is used, if you need fuzzy comparison for floating-point data, use
@ref removeDuplicatesFuzzyInPlace() instead. If you want to remove duplicate
data from an already indexed array, use
@ref removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView2D<char>&)
instead. See @ref removeDuplicates(const Containers::StridedArrayView2D<const char>&)
for a variant that doesn't modify the input data in any way but instead returns
an index array pointing to original data locations. Use
@ref removeDuplicatesInPlaceInto() to place the indices into existing memory
instead of allocating a new array.
@see @relativeref{Corrade,Containers::StridedArrayView::isContiguous()},
    @ref meshtools-duplicates
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesInPlace(const Containers::StridedArrayView2D<char>& data);

/**
@brief Remove duplicate data from given array in-place into given output index array
@param[in,out] data     Data array, duplicate items will be cut away with order
    preserved
@param[out]    indices  Where to put the resulting index array
@return Size of unique prefix in the cleaned up @p data array
@m_since{2020,06}

Like @ref removeDuplicatesInPlace(), except that the index array is not
allocated but put into @p indices instead. Expects that @p indices has the same
size as @p data.
@see @ref removeDuplicatesInto()
*/
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesInPlaceInto(const Containers::StridedArrayView2D<char>& data, const Containers::StridedArrayView1D<UnsignedInt>& indices);

/**
@brief Remove duplicate data from given array
@param[in] data     Data array
@return The resulting index array and count of unique items in the original
    @p data array
@m_since{2020,06}

Compared to @ref removeDuplicatesInPlace(const Containers::StridedArrayView2D<char>&)
this function doesn't modify the input data array in any way but instead
returns an index array pointing to original data locations. Use
@ref removeDuplicatesInto() to place the indices into existing memory instead
of allocating a new array.

Compared to
@ref removeDuplicatesInPlace(const Containers::StridedArrayView2D<char>&) "removeDuplicatesInPlace()"
the returned index buffer would look like this with the same input:

@code{.cpp}
// Input that doesn't get modified
{{172, 26}, {212, -183}, {172, 26}, {42, 13}, {112, 9}, {112, 9}, {212, -183}}

// Matching index array that points back to the input, number of unique items
{{0, 1, 0, 3, 4, 4, 1}, 4}
@endcode
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicates(const Containers::StridedArrayView2D<const char>& data);

/**
@brief Remove duplicate data from given array into given output index array
@param[in]  data    Data array
@param[out] indices Where to put the resulting index array
@return Count of unique items in the original @p data array
@m_since{2020,06}

Compared to @ref removeDuplicatesInPlaceInto(const Containers::StridedArrayView2D<char>&, const Containers::StridedArrayView1D<UnsignedInt>&)
this function doesn't modify the input data array in any way but instead
makes an index array pointing to original data locations.
*/
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesInto(const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView1D<UnsignedInt>& indices);

/**
@brief Remove duplicates from indexed data in-place
@param[in,out] indices  Index array, which will get remapped to list just
    unique data
@param[in,out] data     Data array, duplicate items will be cut away with order
    preserved
@return Size of unique prefix in the cleaned up @p data array
@m_since{2020,06}

Compared to @ref removeDuplicatesInPlace(const Containers::StridedArrayView2D<char>&)
this variant is more suited for data that is already indexed as it works on
the existing index array instead of allocating a new one.
*/
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView2D<char>& data);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView2D<char>& data);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView2D<char>& data);

/**
@brief Remove duplicates from indexed data in-place on a type-erased index array
@m_since{2020,06}

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView2D<char>&)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView2D<char>& data);

/**
@brief Remove duplicate data from given array using fuzzy comparison in-place
@param[in,out] data Data array to process. Unique items get moved to the front,
    preserving their relative order.
@param[in] epsilon  Epsilon value, data closer than this distance will be
    deduplicated
@return Resulting index array and size of the unique prefix in the processed
    @p data array
@m_since{2020,06}

Removes duplicate data from the array by collapsing them into buckets of size
@p epsilon. First vector in given bucket is used, other ones are thrown away,
no interpolation is done. Usage example:

@snippet MeshTools.cpp removeDuplicatesFuzzy

Assuming the following input and an epsilon of @cpp 0.01f @ce, the function
puts the five unique items at the front of the array, preserving their order.
The returned index array then maps the unique back to the input locations, the
second returned value is the count of unique items left in the input, thus
five:

@code{.cpp}
// Original input
{1.720f, 21.199f, 1.729f, 42.2f, 1.121f, 1.120f, 21.2f, 1.705f}

// After processing, contents following the unique prefix are left unspecified
{1.720f, 21.199f, 42.2f, 1.121f, 1.705f, ...}

// Matching index array and number of unique items
{{0, 1, 0, 2, 3, 3, 1, 4}, 5}
@endcode

Note that this function is meant to be used for floating-point data (or
generally with non-zero @p epsilon), for data where bit-exact matching is
sufficient use @ref removeDuplicatesInPlace(const Containers::StridedArrayView2D<char>&)
instead. If you want to remove duplicate data from an already indexed array,
use @ref removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView2D<Float>&, Float)
and friends instead. Use @ref removeDuplicatesFuzzyInPlaceInto() to place the
indices into existing memory instead of allocating a new array.

Overloads for vector, matrix and other math types are not provided because
there would be too many variants. Instead you can slice to
@ref Math::Vector::data(), @ref Math::RectangularMatrix::data() etc., which is
then convertible to a 2D view:

@snippet MeshTools.cpp removeDuplicatesFuzzy-vector-slice

If you want to remove duplicates in multiple incidental arrays, first remove
duplicates in each array separately and then combine the resulting index arrays
back into a single one using @ref combineIndexedAttributes().
@see @ref meshtools-duplicates
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesFuzzyInPlace(const Containers::StridedArrayView2D<Float>& data, Float epsilon = Math::TypeTraits<Float>::epsilon());

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesFuzzyInPlace(const Containers::StridedArrayView2D<Double>& data, Double epsilon = Math::TypeTraits<Double>::epsilon());

/**
@brief Remove duplicate data from given array using fuzzy comparison in-place into given output index array
@param[in,out] data Data array to process. Unique items get moved to the front,
    preserving their relative order.
@param[out] indices Where to put the resulting index array
@param[in] epsilon  Epsilon value, data closer than this distance will be
    deduplicated
@return Size of unique prefix in the cleaned up @p data array
@m_since{2020,06}

Like @ref removeDuplicatesFuzzyInPlace(), except that the index array is not
allocated but put into @p indices instead. Expects that @p indices has the same
size as @p data.
*/
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesFuzzyInPlaceInto(const Containers::StridedArrayView2D<Float>& data, const Containers::StridedArrayView1D<UnsignedInt>& indices, Float epsilon = Math::TypeTraits<Float>::epsilon());

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesFuzzyInPlaceInto(const Containers::StridedArrayView2D<Double>& data, const Containers::StridedArrayView1D<UnsignedInt>& indices, Double epsilon = Math::TypeTraits<Double>::epsilon());

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Remove duplicate data from a STL vector using fuzzy comparison in-place
@param[in,out] data Data array, duplicate items will be cut away with order
    preserved and the size shrunk to just the unique prefix
@param[in] epsilon  Epsilon value, vertices closer than this distance will be
    melt together
@return Resulting index array
@m_deprecated_since{2020,06} Use @ref removeDuplicatesFuzzyInPlace() instead.

Similar to the above, except that it's operating on a @ref std::vector, which
gets shrunk as a result (instead of the prefix size being returned). This
variant is useful together with @ref combineIndexedArrays() to remove
duplicates in multiple incidental arrays --- first remove duplicates in each
array separately and then combine the resulting index arrays to single index
array, and reorder the data accordingly:

@snippet MeshTools.cpp removeDuplicates-multiple
*/
template<class Vector> CORRADE_DEPRECATED("use removeDuplicatesInPlace() instead") std::vector<UnsignedInt> removeDuplicates(std::vector<Vector>& data, typename Vector::Type epsilon = Math::TypeTraits<typename Vector::Type>::epsilon());
#endif

/**
@brief Remove duplicates from indexed data using fuzzy comparison in-place
@param[in,out] indices  Index array, which will get remapped to list just
    unique vertices
@param[in,out] data     Data array to process. Unique items get moved to the
    front, preserving their relative order.
@param[in] epsilon      Epsilon value, items closer than this distance will be
    deduplicated
@return Size of unique prefix in the processed up @p data array
@m_since{2020,06}

Compared to @ref removeDuplicatesFuzzyInPlace(const Containers::StridedArrayView2D<Float>&, Float)
this variant is more suited for data that is already indexed as it works on
the existing index array instead of allocating a new one.
*/
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView2D<Float>& data, Float epsilon = Math::TypeTraits<Float>::epsilon());

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView2D<Float>& data, Float epsilon = Math::TypeTraits<Float>::epsilon());

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView2D<Float>& data, Float epsilon = Math::TypeTraits<Float>::epsilon());

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView2D<Double>& data, Double epsilon = Math::TypeTraits<Double>::epsilon());

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView2D<Double>& data, Double epsilon = Math::TypeTraits<Double>::epsilon());

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView2D<Double>& data, Double epsilon = Math::TypeTraits<Double>::epsilon());

/**
@brief Remove duplicates from indexed data using fuzzy comparison in-place on a type-erased index array
@m_since{2020,06}

Calls @ref removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView2D<Float>&, Float)
or the other overloads based on size of the second dimension of @p indices.
Expects that the second dimension is contiguous and represents the actual
1/2/4-byte index type.
*/
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView2D<Float>& data, Float epsilon = Math::TypeTraits<Float>::epsilon());

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView2D<Double>& data, Double epsilon = Math::TypeTraits<Double>::epsilon());

/**
@brief Remove mesh data duplicates
@m_since{2020,06}

Equivalent to calling @ref removeDuplicatesInPlace() (or
@ref removeDuplicatesIndexedInPlace(), in case the mesh is indexed) on a
mutable copy of every attribute and then putting the unique prefix and
newly generated index buffer into a new @ref Trade::MeshData instance. If the
mesh is indexed, the original index type is preserved, otherwise the mesh gets
@ref MeshIndexType::UnsignedInt indices. The resulting mesh is always
interleaved and owned, if the input is already interleaved attribute offsets
and paddings are preserved. An index buffer, if present, is expected to not
have an implementation-specific index type. All attributes are expected to not
have an implementation-specific format.

In order to remove random padding values from the input and make the vertices
suitable for fast in-place duplicate removal, this function unconditionally
copies and interleaves the input vertex and index data.
@see @ref isMeshIndexTypeImplementationSpecific(),
    @ref isVertexFormatImplementationSpecific(), @ref meshtools-duplicates
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData removeDuplicates(const Trade::MeshData& mesh);

/**
@brief Remove mesh data duplicates with fuzzy comparison for floating-point attributes
@m_since{2020,06}

Compared to @ref removeDuplicates(const Trade::MeshData&), calls
@ref removeDuplicatesFuzzyInPlace() or @ref removeDuplicatesFuzzyIndexedInPlace()
on floating-point attributes. For attributes with a known range (such as
@ref Trade::MeshAttribute::Normal being always @f$ [-1, 1] @f$ in each
direction) the @p floatEpsilon / @p doubleEpsilon is scaled appropriately,
otherwise it's scaled to calculated value range.
@see @ref meshtools-duplicates
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData removeDuplicatesFuzzy(const Trade::MeshData& mesh, Float floatEpsilon = Math::TypeTraits<Float>::epsilon(), Double doubleEpsilon = Math::TypeTraits<Double>::epsilon());

#ifdef MAGNUM_BUILD_DEPRECATED
template<class Vector> std::vector<UnsignedInt> removeDuplicates(std::vector<Vector>& data, typename Vector::Type epsilon) {
    /* A trivial index array that'll be remapped and returned after */
    std::vector<UnsignedInt> indices(data.size());
    const std::size_t size = removeDuplicatesFuzzyInPlaceInto(
        Containers::arrayCast<2, typename Vector::Type>(Containers::stridedArrayView(data)),
        Containers::stridedArrayView(indices), epsilon);
    data.resize(size);
    return indices;
}
#endif

}}

#endif
