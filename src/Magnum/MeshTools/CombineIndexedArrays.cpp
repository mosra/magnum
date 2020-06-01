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

#define _MAGNUM_NO_DEPRECATED_COMBINEINDEXEDARRAYS

#include "CombineIndexedArrays.h"

#include <cstring>
#include <unordered_map>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/MurmurHash2.h>

#include "Magnum/Magnum.h"

namespace Magnum { namespace MeshTools {

namespace Implementation {

std::pair<std::vector<UnsignedInt>, std::vector<UnsignedInt>> interleaveAndCombineIndexArrays(const std::reference_wrapper<const std::vector<UnsignedInt>>* begin, const std::reference_wrapper<const std::vector<UnsignedInt>>* end) {
    /* Array stride and size */
    const UnsignedInt stride = end - begin;
    const UnsignedInt inputSize = begin->get().size();
    #if !defined(CORRADE_NO_ASSERT) || defined(CORRADE_GRACEFUL_ASSERT)
    for(auto it = begin; it != end; ++it)
        CORRADE_ASSERT(it->get().size() == inputSize, "MeshTools::combineIndexArrays(): the arrays don't have the same size", {});
    #endif

    /* Interleave the arrays */
    std::vector<UnsignedInt> interleavedArrays;
    interleavedArrays.resize(inputSize*stride);
    for(UnsignedInt offset = 0; offset != stride; ++offset) {
        const auto& array = (begin+offset)->get();
        for(UnsignedInt i = 0; i != inputSize; ++i)
            interleavedArrays[offset + i*stride] = array[i];
    }

    /* Combine them */
    std::vector<UnsignedInt> combinedIndices;
    CORRADE_IGNORE_DEPRECATED_PUSH
    std::tie(combinedIndices, interleavedArrays) = MeshTools::combineIndexArrays(interleavedArrays, stride);
    CORRADE_IGNORE_DEPRECATED_POP
    return {combinedIndices, interleavedArrays};
}

}

namespace {

std::vector<UnsignedInt> combineIndexArrays(const std::reference_wrapper<std::vector<UnsignedInt>>* const begin, const std::reference_wrapper<std::vector<UnsignedInt>>* const end) {
    /* Interleave and combine the arrays */
    std::vector<UnsignedInt> combinedIndices;
    std::vector<UnsignedInt> interleavedCombinedArrays;
    std::tie(combinedIndices, interleavedCombinedArrays) = Implementation::interleaveAndCombineIndexArrays(
        /* This will bite me hard once. */
        reinterpret_cast<const std::reference_wrapper<const std::vector<UnsignedInt>>*>(begin),
        reinterpret_cast<const std::reference_wrapper<const std::vector<UnsignedInt>>*>(end));

    /* Update the original indices */
    const UnsignedInt stride = end - begin;
    const UnsignedInt outputSize = interleavedCombinedArrays.size()/stride;
    for(UnsignedInt offset = 0; offset != stride; ++offset) {
        auto& array = (begin+offset)->get();
        CORRADE_INTERNAL_ASSERT(array.size() >= outputSize);
        array.resize(outputSize);
        for(UnsignedInt i = 0; i != outputSize; ++i)
            array[i] = interleavedCombinedArrays[offset + i*stride];
    }

    return combinedIndices;
}

class IndexHash {
    public:
        explicit IndexHash(const std::vector<UnsignedInt>& indices, UnsignedInt stride): indices(indices), stride(stride) {}

        std::size_t operator()(UnsignedInt key) const {
            return *reinterpret_cast<const std::size_t*>(Utility::MurmurHash2()(reinterpret_cast<const char*>(indices.data()+key*stride), sizeof(UnsignedInt)*stride).byteArray());
        }

    private:
        const std::vector<UnsignedInt>& indices;
        UnsignedInt stride;
};

class IndexEqual {
    public:
        explicit IndexEqual(const std::vector<UnsignedInt>& indices, UnsignedInt stride): indices(indices), stride(stride) {}

        bool operator()(UnsignedInt a, UnsignedInt b) const {
            return std::memcmp(indices.data()+a*stride, indices.data()+b*stride, sizeof(UnsignedInt)*stride) == 0;
        }

    private:
        const std::vector<UnsignedInt>& indices;
        UnsignedInt stride;
};

}

std::vector<UnsignedInt> combineIndexArrays(const std::vector<std::reference_wrapper<std::vector<UnsignedInt>>>& arrays) {
    return combineIndexArrays(&arrays[0], &arrays[0] + arrays.size());
}

std::vector<UnsignedInt> combineIndexArrays(std::initializer_list<std::reference_wrapper<std::vector<UnsignedInt>>> arrays) {
    return combineIndexArrays(arrays.begin(), arrays.end());
}

std::pair<std::vector<UnsignedInt>, std::vector<UnsignedInt>> combineIndexArrays(const std::vector<UnsignedInt>& interleavedArrays, const UnsignedInt stride) {
    CORRADE_ASSERT(stride != 0, "MeshTools::combineIndexArrays(): stride can't be zero", {});
    CORRADE_ASSERT(interleavedArrays.size() % stride == 0, "MeshTools::combineIndexArrays(): array size is not divisible by stride", {});

    /* Hash map with index combinations, containing just indices into
       interleavedArrays vector, hashing and comparison is done using IndexHash
       and IndexEqual functors. Reserving more buckets than necessary (i.e. as
       if each combination was unique). */
    std::unordered_map<UnsignedInt, UnsignedInt, IndexHash, IndexEqual> indexCombinations(
        interleavedArrays.size()/stride,
        IndexHash(interleavedArrays, stride),
        IndexEqual(interleavedArrays, stride));

    /* Make the index combinations unique. Original indices into original
       `interleavedArrays` array were 0, 1, 2, 3, ..., `combinedIndices`
       contains new ones into new (shorter) `newInterleavedArrays` array. */
    std::vector<UnsignedInt> combinedIndices;
    combinedIndices.reserve(interleavedArrays.size()/stride);
    std::vector<UnsignedInt> newInterleavedArrays;
    for(std::size_t oldIndex = 0, end = interleavedArrays.size()/stride; oldIndex != end; ++oldIndex) {
        /* Try to insert new index combination to the map */
        const auto result = indexCombinations.emplace(oldIndex, indexCombinations.size());

        /* Add the (either new or already existing) index to resulting index array */
        combinedIndices.push_back(result.first->second);

        /* If this is new combination, copy it to new interleaved arrays */
        if(result.second) newInterleavedArrays.insert(newInterleavedArrays.end(),
            interleavedArrays.begin()+oldIndex*stride,
            interleavedArrays.begin()+(oldIndex+1)*stride);
    }

    CORRADE_INTERNAL_ASSERT(combinedIndices.size() == interleavedArrays.size()/stride &&
                            newInterleavedArrays.size() <= interleavedArrays.size());

    return {std::move(combinedIndices), std::move(newInterleavedArrays)};
}

}}
