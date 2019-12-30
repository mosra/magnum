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
 * @brief Function @ref Magnum::MeshTools::removeDuplicates()
 */

#include <limits>
#include <numeric>
#include <unordered_map>
#include <vector>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Utility/MurmurHash2.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/FunctionsBatch.h"

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
@brief Remove duplicate floating-point vector data from given array
@param[in,out] data Input data array
@param[in] epsilon  Epsilon value, vertices nearer than this distance will be
    melt together
@return Index array and unique data

Removes duplicate data from the array by collapsing them into buckets of size
@p epsilon. First vector in given bucket is used, other ones are thrown away,
no interpolation is done. Note that this function is meant to be used for
floating-point data (or generally with non-zero @p epsilon), for discrete data
the usual sorting method is much more efficient.

If you want to remove duplicate data from an already indexed array, first
remove duplicates as if the array wasn't indexed at all and then use
@ref duplicate() to combine the two index arrays:

@snippet MagnumMeshTools.cpp removeDuplicates1

Removing duplicates in multiple indcidental arrays is also possible --- first
remove duplicates in each array separately and then use @ref combineIndexedArrays()
to combine the resulting index arrays to single index array, and reorder the
data accordingly:

@snippet MagnumMeshTools.cpp removeDuplicates2
*/
template<class Vector> std::vector<UnsignedInt> removeDuplicates(std::vector<Vector>& data, typename Vector::Type epsilon = Math::TypeTraits<typename Vector::Type>::epsilon()) {
    /* Get bounds. When NaNs appear, those will get collapsed together when
       you're lucky, or cause the whole data to disappear when you're not -- it
       needs a much more specialized handling to be robust. */
    std::pair<Vector, Vector> minmax = Math::minmax<Vector>(data);

    /* Make epsilon so large that std::size_t can index all vectors inside the
       bounds. */
    epsilon = Math::max(epsilon, typename Vector::Type((minmax.second-minmax.first).max()/~std::size_t{}));

    /* Resulting index array. Because we'll be remapping these, we need to
       start from a 0..n sequence. */
    std::vector<UnsignedInt> indices(data.size());
    std::iota(indices.begin(), indices.end(), 0);

    /* Table containing original vector index for each discretized vector.
       Reserving more buckets than necessary (i.e. as if each vector was
       unique). */
    std::unordered_map<Math::Vector<Vector::Size, std::size_t>, UnsignedInt, Implementation::VectorHash<Vector::Size>> table(data.size());

    /* Index array that'll be filled in each pass and then used for remapping
       the `indices` */
    std::vector<UnsignedInt> remapping(data.size());

    /* First go with original coordinates, then move them by epsilon/2 in each
       direction. */
    Vector moved;
    for(std::size_t moving = 0; moving <= Vector::Size; ++moving) {
        /* Clear the table for this pass */
        table.clear();

        /* Go through all vectors */
        for(std::size_t i = 0; i != data.size(); ++i) {
            /* Try to insert new vertex into the table */
            const Math::Vector<Vector::Size, std::size_t> v{(data[i] + moved - minmax.first)/epsilon};
            const auto result = table.emplace(v, table.size());

            /* Add the (either new or already existing) index into index array */
            remapping[i] = result.first->second;

            /* If this is a new combination, copy the data to new (earlier)
               position in the array. Data in [table.size()-1, i) are already
               present in the [0, table.size()-1) range from previous
               iterations so we aren't overwriting anything. */
            if(result.second && i != table.size() - 1)
                data[table.size()-1] = data[i];
        }

        /* Shrink the data array */
        CORRADE_INTERNAL_ASSERT(data.size() >= table.size());
        data.resize(table.size());

        /* Remap the resulting index array */
        for(auto& i: indices) i = remapping[i];

        /* Finished */
        if(moving == Vector::Size) continue;

        /* Move vertex coordinates by epsilon/2 in the next direction */
        moved = Vector();
        moved[moving] = epsilon/2;
    }

    return indices;
}

}}

#endif
