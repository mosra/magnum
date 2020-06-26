#ifndef Magnum_MeshTools_CombineIndexedArrays_h
#define Magnum_MeshTools_CombineIndexedArrays_h
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

#ifdef MAGNUM_BUILD_DEPRECATED
/** @file
 * @brief Function @ref Magnum::MeshTools::combineIndexArrays(), @ref Magnum::MeshTools::combineIndexedArrays()
 * @m_deprecated_since{2020,06} Use @ref Magnum/MeshTools/Combine.h and
 *      @ref Magnum::MeshTools::combineIndexedAttributes() instead.
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <functional>
#include <tuple>
#include <vector>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Macros.h>

#include "Magnum/Types.h"
#include "Magnum/MeshTools/visibility.h"

#ifndef _MAGNUM_NO_DEPRECATED_COMBINEINDEXEDARRAYS
CORRADE_DEPRECATED_FILE("use Magnum/Trade/MeshData.h and combineIndexedAttributes() instead")
#endif

namespace Magnum { namespace MeshTools {

/**
@brief Combine index arrays
@param[in,out] arrays   Index arrays to combine. These arrays are updated
    in-place to contain unique combinations of the original indices.
@return Resulting combined index array
@m_deprecated_since{2020,06} Use @ref combineIndexedAttributes() instead.

Creates new combined index array and updates the original ones with translation
to new ones. For example, when you have position and normal array, each indexed
with separate indices and you want to index both of them with single index
array:

    a b c d e f         // positions
    A B C D E F G       // normals

    0 2 5 0 0 1 3 2 2   // position indices
    1 3 4 1 4 6 1 3 1   // normal indices

In particular, first triangle in the mesh will have positions `a c f` and
normals `B D E`. You can see that not all combinations are unique and also that
there are some vertices unused. When you pass the two index arrays above to
this function, the following combined index array is returned:

    0 1 2 0 3 4 5 1 6

And the original arrays are cleaned up to have only unique combinations:

    0 2 5 0 1 3 2
    1 3 4 4 6 1 1

You can use these as translation table to create new vertex and normal arrays
which can be then indexed with the combined index array:

    a c f a b d c
    B D E E G B B

Again, first triangle in the mesh will have positions `a c f` and normals
`B D E`.

This function calls @ref combineIndexArrays(const std::vector<UnsignedInt>&, UnsignedInt)
internally. See also @ref combineIndexedArrays() which does the vertex data
reordering automatically.
*/
CORRADE_DEPRECATED("use combineIndexedAttributes() instead") MAGNUM_MESHTOOLS_EXPORT std::vector<UnsignedInt> combineIndexArrays(const std::vector<std::reference_wrapper<std::vector<UnsignedInt>>>& arrays);

/**
@overload
@m_deprecated_since{2020,06} Use @ref combineIndexedAttributes() instead.
*/
CORRADE_DEPRECATED("use combineIndexedAttributes() instead") MAGNUM_MESHTOOLS_EXPORT std::vector<UnsignedInt> combineIndexArrays(std::initializer_list<std::reference_wrapper<std::vector<UnsignedInt>>> arrays);

/**
@brief Combine interleaved index arrays
@m_deprecated_since{2020,06} Use @ref combineIndexedAttributes() instead.

Unlike above, this function takes one interleaved array instead of separate
index arrays. Continuing with the above example, you would call this function
with the following array (odd value is vertex index, even is normal index,
@p stride is thus 2):

    0 1 2 3 5 4 0 1 0 4 1 6 3 1 2 3 2 1

Similarly to above this function will return the following combined index
array as first pair value:

    0 1 2 0 3 4 5 1 6

And second pair value is the cleaned up interleaved array:

    0 1 2 3 5 4 0 4 1 6 3 1 2 1

@see @ref combineIndexedArrays()
*/
CORRADE_DEPRECATED("use combineIndexedAttributes() instead") MAGNUM_MESHTOOLS_EXPORT std::pair<std::vector<UnsignedInt>, std::vector<UnsignedInt>> combineIndexArrays(const std::vector<UnsignedInt>& interleavedArrays, UnsignedInt stride);

namespace Implementation {

MAGNUM_MESHTOOLS_EXPORT std::pair<std::vector<UnsignedInt>, std::vector<UnsignedInt>> interleaveAndCombineIndexArrays(const std::reference_wrapper<const std::vector<UnsignedInt>>* begin, const std::reference_wrapper<const std::vector<UnsignedInt>>* end);

template<class T> void writeCombinedArray(const UnsignedInt stride, const UnsignedInt offset, const std::vector<UnsignedInt>& interleavedCombinedIndexArrays, std::vector<T>& array) {
    /* Can't use duplicate() here because we aren't accessing the index data sequentially */
    std::vector<T> output;
    output.reserve(interleavedCombinedIndexArrays.size()/stride);
    for(std::size_t i = 0, max = interleavedCombinedIndexArrays.size()/stride; i != max; ++i) {
        const UnsignedInt index = interleavedCombinedIndexArrays[offset + i*stride];
        CORRADE_ASSERT(index < array.size(), "MeshTools::combineIndexedArrays(): index out of range", );
        output.push_back(array[index]);
    }
    using std::swap;
    swap(output, array);
}

/* Terminator for recursive calls */
inline void writeCombinedArrays(UnsignedInt, UnsignedInt, const std::vector<UnsignedInt>&) {}

template<class T, class ...U> inline void writeCombinedArrays(UnsignedInt stride, UnsignedInt offset, const std::vector<UnsignedInt>& interleavedCombinedIndexArrays, std::vector<T>& first, std::vector<U>&... next) {
    writeCombinedArray(stride, offset, interleavedCombinedIndexArrays, first);
    writeCombinedArrays(stride, offset + 1, interleavedCombinedIndexArrays, next...);
}

}

/**
@brief Combine indexed arrays
@param[in,out] indexedArrays Index and attribute arrays
@return Array with resulting indices
@m_deprecated_since{2020,06} Use @ref combineIndexedAttributes() instead.

Creates new combined index array and reorders original attribute arrays so they
can be indexed with the new single index array.

The index array must be passed as const reference (to avoid copying) and
attribute array as reference, so it can be replaced with combined data. To
avoid explicit verbose specification of tuple type, you can write it with help
of some STL functions like shown below. Also if one index array is shared by
more than one attribute array, just pass the index array more times. Example:

@snippet MagnumMeshTools.cpp combineIndexedArrays

See @ref combineIndexArrays() documentation for more information about the
procedure.
@todo Invent a way which avoids these overly verbose parameters (`std::pair`
    doesn't help)
*/
/* Implementation note: It's done using tuples because it is more clear which
   parameter is index array and which is attribute array, mainly when both are
   of the same type. */
template<class ...T> CORRADE_DEPRECATED("use combineIndexedAttributes() instead") std::vector<UnsignedInt> combineIndexedArrays(const std::pair<const std::vector<UnsignedInt>&, std::vector<T>&>&... indexedArrays) {
    /* Interleave and combine index arrays */
    std::vector<UnsignedInt> combinedIndices;
    std::vector<UnsignedInt> interleavedCombinedIndexArrays;
    auto i = {std::ref(indexedArrays.first)...};
    std::tie(combinedIndices, interleavedCombinedIndexArrays) = Implementation::interleaveAndCombineIndexArrays(i.begin(), i.end());

    /* Write combined arrays */
    Implementation::writeCombinedArrays(sizeof...(T), 0, interleavedCombinedIndexArrays, indexedArrays.second...);

    return combinedIndices;
}

}}
#else
#error use functions in Combine.h instead
#endif

#endif
