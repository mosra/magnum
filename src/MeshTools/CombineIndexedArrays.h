#ifndef Magnum_MeshTools_CombineIndexedArrays_h
#define Magnum_MeshTools_CombineIndexedArrays_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Function Magnum::MeshTools::combineIndexedArrays()
 */

#include <vector>
#include <numeric>
#include <tuple>

#include "Math/Vector.h"
#include "MeshTools/Clean.h"

namespace Magnum { namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

class CombineIndexedArrays {
    public:
        template<class ...T> std::vector<UnsignedInt> operator()(const std::tuple<const std::vector<UnsignedInt>&, std::vector<T>&>&... indexedArrays) {
            /* Compute index count */
            std::size_t _indexCount = indexCount(std::get<0>(indexedArrays)...);

            /* Resulting index array */
            std::vector<UnsignedInt> result;
            result.resize(_indexCount);
            std::iota(result.begin(), result.end(), 0);

            /* All index combinations */
            std::vector<Math::Vector<sizeof...(indexedArrays), UnsignedInt> > indexCombinations(_indexCount);
            writeCombinedIndices(indexCombinations, std::get<0>(indexedArrays)...);

            /* Make the combinations unique */
            MeshTools::clean(result, indexCombinations);

            /* Write combined arrays */
            writeCombinedArrays(indexCombinations, std::get<1>(indexedArrays)...);

            return result;
        }

    private:
        template<class ...T> inline static std::size_t indexCount(const std::vector<UnsignedInt>& first, const std::vector<T>&... next) {
            CORRADE_ASSERT(sizeof...(next) == 0 || indexCount(next...) == first.size(), "MeshTools::combineIndexedArrays(): index arrays don't have the same length, nothing done.", 0);

            return first.size();
        }

        template<std::size_t size, class ...T> static void writeCombinedIndices(std::vector<Math::Vector<size, UnsignedInt>>& output, const std::vector<UnsignedInt>& first, const std::vector<T>&... next) {
            /* Copy the data to output */
            for(std::size_t i = 0; i != output.size(); ++i)
                output[i][size-sizeof...(next)-1] = first[i];

            writeCombinedIndices(output, next...);
        }

        template<std::size_t size, class T, class ...U> static void writeCombinedArrays(const std::vector<Math::Vector<size, UnsignedInt>>& combinedIndices, std::vector<T>& first, std::vector<U>&... next) {
            /* Rewrite output array */
            std::vector<T> output;
            for(std::size_t i = 0; i != combinedIndices.size(); ++i)
                output.push_back(first[combinedIndices[i][size-sizeof...(next)-1]]);
            std::swap(output, first);

            writeCombinedArrays(combinedIndices, next...);
        }

        /* Terminator functions for recursive calls */
        inline static std::size_t indexCount() { return 0; }
        template<std::size_t size> inline static void writeCombinedIndices(std::vector<Math::Vector<size, UnsignedInt>>&) {}
        template<std::size_t size> inline static void writeCombinedArrays(const std::vector<Math::Vector<size, UnsignedInt>>&) {}
};

}
#endif

/**
@brief Combine indexed arrays
@param[in,out] indexedArrays Index and attribute arrays
@return %Array with resulting indices

When you have e.g. vertex, normal and texture array, each indexed with
different indices, you can use this function to combine them to use the same
indices. The function returns array with resulting indices and replaces
original attribute arrays with combined ones.

The index array must be passed as const reference (to avoid copying) and
attribute array as reference, so it can be replaced with combined data. To
avoid explicit verbose specification of tuple type, you can write it with help
of some STL functions like shown below. Also if one index array is shader by
more than one attribute array, just pass the index array more times. Example:
@code
std::vector<UnsignedInt> vertexIndices;
std::vector<Vector3> positions;
std::vector<UnsignedInt> normalTextureIndices;
std::vector<Vector3> normals;
std::vector<Vector2> textureCoordinates;

std::vector<UnsignedInt> indices = MeshTools::combineIndexedArrays(
    std::make_tuple(std::cref(vertexIndices), std::ref(positions)),
    std::make_tuple(std::cref(normalTextureIndices), std::ref(normals)),
    std::make_tuple(std::cref(normalTextureIndices), std::ref(textureCoordinates))
);
@endcode
`positions`, `normals` and `textureCoordinates` will then contain combined
attributes indexed with `indices`.

@attention The function expects that all arrays have the same size.
*/
/* Implementation note: It's done using tuples because it is more clear which
   parameter is index array and which is attribute array, mainly when both are
   of the same type. */
template<class ...T> std::vector<UnsignedInt> combineIndexedArrays(const std::tuple<const std::vector<UnsignedInt>&, std::vector<T>&>&... indexedArrays) {
    return Implementation::CombineIndexedArrays()(indexedArrays...);
}

}}

#endif
