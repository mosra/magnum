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

#include <cstring>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Algorithms.h>

#include "RemoveDuplicates.h"

namespace Magnum { namespace MeshTools {

struct ArrayEqual {
    bool operator()(Containers::ArrayView<const char> a, Containers::ArrayView<const char> b) const {
        CORRADE_INTERNAL_ASSERT(a.size() == b.size());
        return std::memcmp(a, b, a.size()) == 0;
    }
};

struct ArrayHash {
    std::size_t operator()(Containers::ArrayView<const char> a) const {
        return *reinterpret_cast<const std::size_t*>(Utility::MurmurHash2{}(a, a.size()).byteArray());
    }
};

std::size_t removeDuplicatesInPlaceInto(const Containers::StridedArrayView2D<char>& data, const Containers::StridedArrayView1D<UnsignedInt>& indices) {
    /* Assuming the second dimension is contiguous so we can calculate the
       hashes easily */
    CORRADE_ASSERT(data.empty()[0] || data.isContiguous<1>(),
        "MeshTools::removeDuplicatesInPlaceInto(): second data view dimension is not contiguous", {});

    const std::size_t dataSize = data.size()[0];
    CORRADE_ASSERT(indices.size() == dataSize,
        "MeshTools::removeDuplicatesInPlaceInto(): output index array has" << indices.size() << "elements but expected" << dataSize, {});

    /* Table containing index of first occurence for each unique entry.
       Reserving more buckets than necessary (i.e. as if each entry was
       unique). */
    std::unordered_map<Containers::ArrayView<const char>, UnsignedInt, ArrayHash, ArrayEqual> table{dataSize};

    /* Go through all entries */
    for(std::size_t i = 0; i != dataSize; ++i) {
        /* Try to insert new entry into the table */
        const Containers::ArrayView<const char> entry = data[i].asContiguous();
        const auto result = table.emplace(entry, table.size());

        /* Add the (either new or already existing) index into the array */
        indices[i] = result.first->second;

        /* If this is a new combination, copy the data to new (earlier)
           position in the array. Data in [table.size()-1, i) are already
           present in the [0, table.size()-1) range from previous iterations so
           we aren't overwriting anything. */
        if(result.second && i != table.size() - 1)
            Utility::copy(entry, data[table.size() - 1].asContiguous());
    }

    CORRADE_INTERNAL_ASSERT(dataSize >= table.size());
    return table.size();
}

std::pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesInPlace(const Containers::StridedArrayView2D<char>& data) {
    Containers::Array<UnsignedInt> indices{Containers::NoInit, data.size()[0]};
    const std::size_t size = removeDuplicatesInPlaceInto(data, indices);
    return {std::move(indices), size};
}

namespace {

template<class IndexType> std::size_t removeDuplicatesIndexedInPlaceImplementation(const Containers::StridedArrayView1D<IndexType>& indices, const Containers::StridedArrayView2D<char>& data) {
    /* Somehow ~IndexType{} doesn't work for < 4byte types, as the result is
       int(-1) instead of the type I want */
    CORRADE_ASSERT(data.size()[0] <= IndexType(-1),
        "MeshTools::removeDuplicatesIndexedInPlace(): a" << sizeof(IndexType) << Debug::nospace << "-byte index type is too small for" << data.size()[0] << "vertices", {});

    /* There's no way to avoid the additional allocation, unfortunately ---
       iterating over the indices instead of data would not preserve the
       original order, which is an useful property. The float version has this
       inverted (having the *Indexed() variant as the main implementation)
       because the remapping there has to be done once for every dimension. */
    std::pair<Containers::Array<UnsignedInt>, std::size_t> result = removeDuplicatesInPlace(data);
    for(auto& i: indices) i = result.first[i];
    return result.second;
}

}

std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView2D<char>& data) {
    return removeDuplicatesIndexedInPlaceImplementation(indices, data);
}

std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView2D<char>& data) {
    return removeDuplicatesIndexedInPlaceImplementation(indices, data);
}

std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView2D<char>& data) {
    return removeDuplicatesIndexedInPlaceImplementation(indices, data);
}

}}
