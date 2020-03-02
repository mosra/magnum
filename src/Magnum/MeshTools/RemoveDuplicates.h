#ifndef Magnum_MeshTools_RemoveDuplicates_h
#define Magnum_MeshTools_RemoveDuplicates_h
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
 * @brief Function @ref Magnum::MeshTools::removeDuplicatesInPlace(), @ref Magnum::MeshTools::removeDuplicatesIndexedInPlace()
 */

#include <limits>
#include <numeric>
#include <unordered_map>
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Utility/MurmurHash2.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/MeshTools/visibility.h"

namespace Magnum { namespace MeshTools {

namespace Implementation {
    template<std::size_t size> class VectorHash {
        public:
            std::size_t operator()(const Math::Vector<size, std::size_t>& data) const {
                return *reinterpret_cast<const std::size_t*>(Utility::MurmurHash2()(reinterpret_cast<const char*>(&data), sizeof(data)).byteArray());
            }
    };
}

/**
@brief Remove duplicate data from given array in-place
@param[in,out] data Data array, duplicate items will be cut away with order
    preserved
@return Size of unique prefix in the cleaned up @p data array and the resulting
    index array
@m_since_latest

Removes duplicate data from given array by comparing the second dimension of
each item, the second dimension is expected to be contiguous. A plain bit-exact
matching is used, if you need fuzzy comparison for floating-point data, use
@ref removeDuplicatesInPlace(const Containers::StridedArrayView1D<Vector>&, typename Vector::Type)
instead. If you want to remove duplicate data from an already indexed array,
use @ref removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView2D<char>&)
instead. Usage example:

@snippet MagnumMeshTools.cpp removeDuplicates

@see @ref Corrade::Containers::StridedArrayView::isContiguous()
*/
MAGNUM_MESHTOOLS_EXPORT std::pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesInPlace(const Containers::StridedArrayView2D<char>& data);

/**
@brief Remove duplicate data from given array in-place
@param[in,out] data     Data array, duplicate items will be cut away with order
    preserved
@param[out]    indices  Where to put the resulting index array
@return Size of unique prefix in the cleaned up @p data array
@m_since_latest

Same as above, except that the index array is not allocated but put into
@p indices instead. Expects that @p indices has the same size as @p data.
*/
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesInPlaceInto(const Containers::StridedArrayView2D<char>& data, const Containers::StridedArrayView1D<UnsignedInt>& indices);

/**
@brief Remove duplicates from indexed data in-place
@param[in,out] indices  Index array, which will get remapped to list just
    unique data
@param[in,out] data     Data array, duplicate items will be cut away with order
    preserved
@return Size of unique prefix in the cleaned up @p data array
@m_since_latest

Compared to @ref removeDuplicatesInPlace(const Containers::StridedArrayView2D<char>&)
this variant is more suited for data that are already indexed as it works on
the existing index array instead of allocating a new one.
*/
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView2D<char>& data);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView2D<char>& data);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView2D<char>& data);

/**
@brief Remove duplicate floating-point vector data from given array in-place
@param[in,out] data Data array, duplicate items will be cut away with order
    preserved
@param[in] epsilon  Epsilon value, vertices closer than this distance will be
    melt together
@return Size of unique prefix in the cleaned up @p data array and the resulting
    index array
@m_since_latest

Removes duplicate data from the array by collapsing them into buckets of size
@p epsilon. First vector in given bucket is used, other ones are thrown away,
no interpolation is done. Note that this function is meant to be used for
floating-point data (or generally with non-zero @p epsilon), for data where
bit-exact matching is sufficient use @ref removeDuplicatesInPlace(const Containers::StridedArrayView2D<char>&)
instead.

If you want to remove duplicate data from an already indexed array, use
@ref removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<IndexType>&, const Containers::StridedArrayView1D<Vector>&, typename Vector::Type) instead.

If you want to remove duplicates in multiple incidental arrays, first remove
duplicates in each array separately and then combine the resulting index arrays
back into a single one using @ref combineIndexedAttributes().
*/
template<class Vector> std::pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesInPlace(const Containers::StridedArrayView1D<Vector>& data, typename Vector::Type epsilon = Math::TypeTraits<typename Vector::Type>::epsilon());

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Remove duplicate floating-point vector data from a STL vector in-place
@param[in,out] data Data array, duplicate items will be cut away with order
    preserved and the size shrunk to just the unique prefix
@param[in] epsilon  Epsilon value, vertices closer than this distance will be
    melt together
@return Resulting index array
@m_deprecated_since_latest Use @ref removeDuplicatesInPlace() instead.

Similar to the above, except that it's operating on a @ref std::vector, which
gets shrunk as a result (instead of the prefix size being returned). This
variant is useful together with @ref combineIndexedArrays() to remove
duplicates in multiple incidental arrays --- first remove duplicates in each
array separately and then combine the resulting index arrays to single index
array, and reorder the data accordingly:

@snippet MagnumMeshTools.cpp removeDuplicates-multiple
*/
template<class Vector> CORRADE_DEPRECATED("use removeDuplicatesInPlace() instead") std::vector<UnsignedInt> removeDuplicates(std::vector<Vector>& data, typename Vector::Type epsilon = Math::TypeTraits<typename Vector::Type>::epsilon());
#endif

/**
@brief Remove duplicates from indexed floating-point vector data in-place
@param[in,out] indices  Index array, which will get remapped to list just
    unique vertices
@param[in,out] data     Data array, duplicate items will be cut away with order
    preserved
@param[in] epsilon      Epsilon value, vertices closer than this distance will
    be melt together
@return Size of unique prefix in the cleaned up @p data array
@m_since_latest

Compared to @ref removeDuplicatesInPlace(const Containers::StridedArrayView1D<Vector>&, typename Vector::Type)
this variant is more suited for data that are already indexed as it works on
the existing index array instead of allocating a new one.
*/
template<class IndexType, class Vector> std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<IndexType>& indices, const Containers::StridedArrayView1D<Vector>& data, typename Vector::Type epsilon = Math::TypeTraits<typename Vector::Type>::epsilon()) {
    /* Somehow ~IndexType{} doesn't work for < 4byte types, as the result is
       int(-1) instead of the type I want */
    CORRADE_ASSERT(data.size() <= IndexType(-1), "MeshTools::removeDuplicatesIndexedInPlace(): a" << sizeof(IndexType) << Debug::nospace << "-byte index type is too small for" << data.size() << "vertices", {});

    /* Get bounds. When NaNs appear, those will get collapsed together when
       you're lucky, or cause the whole data to disappear when you're not -- it
       needs a much more specialized handling to be robust. */
    std::pair<Vector, Vector> minmax = Math::minmax(data);

    /* Make epsilon so large that std::size_t can index all vectors inside the
       bounds. */
    epsilon = Math::max(epsilon, typename Vector::Type((minmax.second-minmax.first).max()/~std::size_t{}));

    /* Table containing original vector index for each discretized vector.
       Reserving more buckets than necessary (i.e. as if each vector was
       unique). */
    std::size_t dataSize = data.size();
    std::unordered_map<Math::Vector<Vector::Size, std::size_t>, UnsignedInt, Implementation::VectorHash<Vector::Size>> table(dataSize);

    /* Index array that'll be filled in each pass and then used for remapping
       the `indices` */
    Containers::Array<UnsignedInt> remapping{Containers::NoInit, dataSize};

    /* First go with original coordinates, then move them by epsilon/2 in each
       direction. */
    Vector moved;
    for(std::size_t moving = 0; moving <= Vector::Size; ++moving) {
        /* Go through all vectors */
        for(std::size_t i = 0; i != dataSize; ++i) {
            /* Try to insert new vertex into the table */
            const Math::Vector<Vector::Size, std::size_t> v{(data[i] + moved - minmax.first)/epsilon};
            const auto result = table.emplace(v, table.size());

            /* Add the (either new or already existing) index into the array */
            remapping[i] = result.first->second;

            /* If this is a new combination, copy the data to new (earlier)
               position in the array. Data in [table.size()-1, i) are already
               present in the [0, table.size()-1) range from previous
               iterations so we aren't overwriting anything. */
            if(result.second && i != table.size() - 1)
                data[table.size() - 1] = data[i];
        }

        /* Remap the resulting index array */
        for(auto& i: indices) i = remapping[i];

        /* Finished */
        if(moving == Vector::Size) continue;

        /* Move vertex coordinates by epsilon/2 in the next direction */
        moved = Vector();
        moved[moving] = epsilon/2;

        /* Next time go only through the unique prefix; clear the table for the
           next pass */
        dataSize = table.size();
        table.clear();
    }

    CORRADE_INTERNAL_ASSERT(data.size() >= dataSize);
    return dataSize;
}

template<class Vector> std::pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesInPlace(const Containers::StridedArrayView1D<Vector>& data, typename Vector::Type epsilon) {
    /* A trivial index array that'll be remapped and returned after */
    Containers::Array<UnsignedInt> indices{Containers::NoInit, data.size()};
    std::iota(indices.begin(), indices.end(), 0);
    const std::size_t size = removeDuplicatesIndexedInPlace(Containers::stridedArrayView(indices), data, epsilon);
    return {std::move(indices), size};
}

#ifdef MAGNUM_BUILD_DEPRECATED
template<class Vector> std::vector<UnsignedInt> removeDuplicates(std::vector<Vector>& data, typename Vector::Type epsilon) {
    /* A trivial index array that'll be remapped and returned after */
    std::vector<UnsignedInt> indices(data.size());
    std::iota(indices.begin(), indices.end(), 0);
    const std::size_t size = removeDuplicatesIndexedInPlace(Containers::stridedArrayView(indices), Containers::stridedArrayView(data), epsilon);
    data.resize(size);
    return indices;
}
#endif

}}

#endif
