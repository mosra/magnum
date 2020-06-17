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

#include "RemoveDuplicates.h"

#include <cstring>
#include <limits>
#include <numeric>
#include <unordered_map>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/MurmurHash2.h>

#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Range.h"
#include "Magnum/MeshTools/Reference.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

struct ArrayEqual {
    explicit ArrayEqual(std::size_t size): _size{size} {}

    bool operator()(const void* a, const void* b) const {
        return std::memcmp(a, b, _size) == 0;
    }

    private: std::size_t _size;
};

struct ArrayHash {
    explicit ArrayHash(std::size_t size): _size{size} {}

    std::size_t operator()(const void* a) const {
        return *reinterpret_cast<const std::size_t*>(Utility::MurmurHash2{}(static_cast<const char*>(a), _size).byteArray());
    }

    private: std::size_t _size;
};

std::size_t removeDuplicatesInto(const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView1D<UnsignedInt>& indices) {
    /* Assuming the second dimension is contiguous so we can calculate the
       hashes easily */
    CORRADE_ASSERT(data.empty()[0] || data.isContiguous<1>(),
        "MeshTools::removeDuplicatesInto(): second data view dimension is not contiguous", {});

    const std::size_t dataSize = data.size()[0];
    CORRADE_ASSERT(indices.size() == dataSize,
        "MeshTools::removeDuplicatesInto(): output index array has" << indices.size() << "elements but expected" << dataSize, {});

    /* Table containing index of first occurence for each unique entry.
       Reserving more buckets than necessary (i.e. as if each entry was
       unique). */
    std::unordered_map<const void*, UnsignedInt, ArrayHash, ArrayEqual> table{
        dataSize,
        ArrayHash{data.size()[1]},
        ArrayEqual{data.size()[1]}};

    /* Go through all entries */
    for(std::size_t i = 0; i != dataSize; ++i) {
        /* Try to insert new entry into the table. The inserted index points
           into the original unchanged data array. */
        const Containers::ArrayView<const char> entry = data[i].asContiguous();
        const auto result = table.emplace(entry, i);

        /* Put the (either new or already existing) index into the output
           index array */
        indices[i] = result.first->second;
    }

    CORRADE_INTERNAL_ASSERT(dataSize >= table.size());
    return table.size();
}

std::pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicates(const Containers::StridedArrayView2D<const char>& data) {
    Containers::Array<UnsignedInt> indices{Containers::NoInit, data.size()[0]};
    const std::size_t size = removeDuplicatesInto(data, indices);
    return {std::move(indices), size};
}

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
    std::unordered_map<const void*, UnsignedInt, ArrayHash, ArrayEqual> table{
        dataSize,
        ArrayHash{data.size()[1]},
        ArrayEqual{data.size()[1]}};

    /* Go through all entries and insert them into the table. Because the keys
       have runtime size, the table doesn't store a copy of the keys, only a
       reference. The reference is to the original data that we mutate
       in-place, so extra care needs to be taken to prevent already-inserted
       keys from getting modified. */
    for(std::size_t i = 0; i != dataSize; ++i) {
        /* First copy the key data to a potentially final no-longer-mutable
           place (except if the source and target location is the same). Data
           in [table.size()-1, i) is already present in the [0, table.size()-1)
           range from previous iterations so we aren't overwriting anything. If
           insertion succeeds, this location will not be touched ever again; if
           it fails the location isn't used as a key anywhere and so it can be
           reused next time for a different key.

           Alternatively we could first call find() and only then conditionally
           do a copy() and emplace(), but that means the hash & search would be
           performed twice, which is never faster than a plain memory copy. */
        const Containers::ArrayView<char> dst = data[table.size()].asContiguous();
        if(i != table.size())
            Utility::copy(data[i].asContiguous(), dst);

        /* Insert the new entry into the table. If it succeeds, dst is
           guaranteed to not change anymore. */
        const auto result = table.emplace(dst, table.size());

        /* Put the (either new or already existing) index into the output index
           array */
        indices[i] = result.first->second;
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

std::size_t removeDuplicatesIndexedInPlace(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView2D<char>& data) {
    CORRADE_ASSERT(indices.isContiguous<1>(), "MeshTools::removeDuplicatesIndexedInPlace(): second index view dimension is not contiguous", {});
    if(indices.size()[1] == 4)
        return removeDuplicatesIndexedInPlace(Containers::arrayCast<1, UnsignedInt>(indices), data);
    else if(indices.size()[1] == 2)
        return removeDuplicatesIndexedInPlace(Containers::arrayCast<1, UnsignedShort>(indices), data);
    else {
        CORRADE_ASSERT(indices.size()[1] == 1, "MeshTools::removeDuplicatesIndexedInPlace(): expected index type size 1, 2 or 4 but got" << indices.size()[1], {});
        return removeDuplicatesIndexedInPlace(Containers::arrayCast<1, UnsignedByte>(indices), data);
    }
}

namespace {

template<class IndexType, class T> std::size_t removeDuplicatesFuzzyIndexedInPlaceImplementation(const Containers::StridedArrayView1D<IndexType>& indices, const Containers::StridedArrayView2D<T>& data, T epsilon) {
    /* Compared to the discrete version, we don't require the second dimension
       to be contiguous, as we calculate the hash from a discretized contiguous
       copy */

    /* Somehow ~IndexType{} doesn't work for < 4byte types, as the result is
       int(-1) instead of the type I want */
    CORRADE_ASSERT(data.size()[0] <= IndexType(-1),
        "MeshTools::removeDuplicatesFuzzyIndexedInPlace(): a" << sizeof(IndexType) << Debug::nospace << "-byte index type is too small for" << data.size()[0] << "vertices", {});

    /* Get bounds across all dimensions. When NaNs appear, those will get
       collapsed together when you're lucky, or cause the whole data to
       disappear when you're not -- it needs a much more specialized handling
       to be robust. */
    const std::size_t vectorSize = data.size()[1];
    T range = T(0.0);
    Containers::Array<T> offsets{Containers::NoInit, vectorSize};
    {
        /** @todo this isn't really cache-efficient, do differently */
        std::size_t i = 0;
        for(Containers::StridedArrayView1D<T> dimension: data.template transposed<0, 1>()) {
            const Math::Range1D<T> minmax = Math::minmax(dimension);
            range = Math::max(minmax.size(), range);
            offsets[i++] = minmax.min();
        }
    }

    /* Make epsilon so large that std::size_t can index all vectors inside the
       bounds. */
    epsilon = Math::max(epsilon, range/T(~std::size_t{}));

    /* Table containing original vector index for each discretized vector.
       Reserving more buckets than necessary (i.e. as if each vector was
       unique). */
    std::size_t dataSize = data.size()[0];
    std::unordered_map<const void*, UnsignedInt, ArrayHash, ArrayEqual> table{
        dataSize,
        ArrayHash{data.size()[1]*sizeof(std::size_t)},
        ArrayEqual{data.size()[1]*sizeof(std::size_t)}};

    /* Index array that'll be filled in each pass and then used for remapping
       the `indices`; discretized storage for all map keys. */
    Containers::Array<UnsignedInt> remapping{Containers::NoInit, dataSize};
    Containers::Array<std::size_t> discretized{Containers::NoInit, dataSize*vectorSize};

    /* First go with original coordinates, then move them by epsilon/2 in each
       dimension. */
    T moveAmount = T(0.0);
    for(std::size_t moving = 0; moving <= vectorSize; ++moving) {
        for(std::size_t i = 0; i != dataSize; ++i) {
            /* Take the original vector and discretize it -- append the move
               amount to given dimension, subtract the minmal offset and divide
               by epsilon. */
            const Containers::StridedArrayView1D<T> entry = data[i];
            const Containers::ArrayView<std::size_t> discretizedEntry = discretized.slice(i*vectorSize, (i + 1)*vectorSize);
            for(std::size_t vi = 0; vi != vectorSize; ++vi) {
                T c = entry[vi];
                /* In iteration `0` we're not moving in any dimension, in
                   iteration `vectorSize` we're moving in `vectorSize - 1`
                   dimension */
                if(vi + 1 == moving) c += moveAmount;
                discretizedEntry[vi] = (c - offsets[vi])/epsilon;
            }

            /* Try to insert new entry into the table. The inserted index
               points into the new data array that has all duplicates removed.
               This is a similar workflow to removeDuplicatesInPlaceInto() with
               the only difference that we're remapping an existing index array
               several times over instead of creating a new one */
            const auto result = table.emplace(discretizedEntry, table.size());

            /* Add the (either new or already existing) index into the array */
            remapping[i] = result.first->second;

            /* If this is a new combination, copy the data to new (earlier)
               position in the array. Data in [table.size()-1, i) are already
               present in the [0, table.size()-1) range from previous
               iterations so we aren't overwriting anything. */
            if(result.second && i != table.size() - 1)
                Utility::copy(entry, data[table.size() - 1]);
        }

        /* Remap the resulting index array */
        for(auto& i: indices) i = remapping[i];

        /* Move vertex coordinates by epsilon/2 in the next dimension (which
           is moving + 1 in the next loop iteration) */
        moveAmount = epsilon/2;

        /* Next time go only through the unique prefix; clear the table for the
           next pass */
        dataSize = table.size();
        table.clear();
    }

    CORRADE_INTERNAL_ASSERT(data.size()[0] >= dataSize);
    return dataSize;
}

}

std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView2D<Float>& data, const Float epsilon) {
    return removeDuplicatesFuzzyIndexedInPlaceImplementation(indices, data, epsilon);
}

std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView2D<Float>& data, const Float epsilon) {
    return removeDuplicatesFuzzyIndexedInPlaceImplementation(indices, data, epsilon);
}

std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView2D<Float>& data, const Float epsilon) {
    return removeDuplicatesFuzzyIndexedInPlaceImplementation(indices, data, epsilon);
}

std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView2D<Double>& data, const Double epsilon) {
    return removeDuplicatesFuzzyIndexedInPlaceImplementation(indices, data, epsilon);
}

std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView2D<Double>& data, const Double epsilon) {
    return removeDuplicatesFuzzyIndexedInPlaceImplementation(indices, data, epsilon);
}

std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView2D<Double>& data, const Double epsilon) {
    return removeDuplicatesFuzzyIndexedInPlaceImplementation(indices, data, epsilon);
}

namespace {

template<class T> std::size_t removeDuplicatesFuzzyInPlaceIntoImplementation(const Containers::StridedArrayView2D<T>& data, const Containers::StridedArrayView1D<UnsignedInt>& indices, const T epsilon) {
    CORRADE_ASSERT(indices.size() == data.size()[0],
        "MeshTools::removeDuplicatesFuzzyInPlaceInto(): output index array has" << indices.size() << "elements but expected" << data.size()[0], {});

    /* A trivial index array that'll be remapped. Would use std::iota() here
       but on MSVC 2015 debug build it excepts StridedArrayIterator to have a
       member named iterator_category. Fuck that, using a loop instead. */
    UnsignedInt i = 0;
    for(UnsignedInt& index: indices) index = i++;

    const std::size_t size = removeDuplicatesFuzzyIndexedInPlaceImplementation(Containers::stridedArrayView(indices), data, epsilon);
    return size;
}

template<class T> std::pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesFuzzyInPlaceImplementation(const Containers::StridedArrayView2D<T>& data, const T epsilon) {
    Containers::Array<UnsignedInt> indices{Containers::NoInit, data.size()[0]};
    const std::size_t size = removeDuplicatesFuzzyInPlaceIntoImplementation(data, indices, epsilon);
    return {std::move(indices), size};
}

}

std::pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesFuzzyInPlace(const Containers::StridedArrayView2D<Float>& data, const Float epsilon) {
    return removeDuplicatesFuzzyInPlaceImplementation(data, epsilon);
}

std::pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesFuzzyInPlace(const Containers::StridedArrayView2D<Double>& data, const Double epsilon) {
    return removeDuplicatesFuzzyInPlaceImplementation(data, epsilon);
}

std::size_t removeDuplicatesFuzzyInPlaceInto(const Containers::StridedArrayView2D<Float>& data, const Containers::StridedArrayView1D<UnsignedInt>& indices, const Float epsilon) {
    return removeDuplicatesFuzzyInPlaceIntoImplementation(data, indices, epsilon);
}

std::size_t removeDuplicatesFuzzyInPlaceInto(const Containers::StridedArrayView2D<Double>& data, const Containers::StridedArrayView1D<UnsignedInt>& indices, const Double epsilon) {
    return removeDuplicatesFuzzyInPlaceIntoImplementation(data, indices, epsilon);
}

namespace {

template<class T> std::size_t removeDuplicatesFuzzyIndexedInPlaceImplementation(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView2D<T>& data, const T epsilon) {
    CORRADE_ASSERT(indices.isContiguous<1>(), "MeshTools::removeDuplicatesFuzzyIndexedInPlace(): second index view dimension is not contiguous", {});
    if(indices.size()[1] == 4)
        return removeDuplicatesFuzzyIndexedInPlaceImplementation(Containers::arrayCast<1, UnsignedInt>(indices), data, epsilon);
    else if(indices.size()[1] == 2)
        return removeDuplicatesFuzzyIndexedInPlaceImplementation(Containers::arrayCast<1, UnsignedShort>(indices), data, epsilon);
    else {
        CORRADE_ASSERT(indices.size()[1] == 1, "MeshTools::removeDuplicatesFuzzyIndexedInPlace(): expected index type size 1, 2 or 4 but got" << indices.size()[1], {});
        return removeDuplicatesFuzzyIndexedInPlaceImplementation(Containers::arrayCast<1, UnsignedByte>(indices), data, epsilon);
    }
}

}

std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView2D<Float>& data, const Float epsilon) {
    return removeDuplicatesFuzzyIndexedInPlaceImplementation(indices, data, epsilon);
}

std::size_t removeDuplicatesFuzzyIndexedInPlace(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView2D<Double>& data, const Double epsilon) {
    return removeDuplicatesFuzzyIndexedInPlaceImplementation(indices, data, epsilon);
}

Trade::MeshData removeDuplicates(const Trade::MeshData& data) {
    return removeDuplicates(Trade::MeshData{data.primitive(),
        {}, data.indexData(), Trade::MeshIndexData{data.indices()},
        {}, data.vertexData(), Trade::meshAttributeDataNonOwningArray(data.attributeData()),
        data.vertexCount()});
}

Trade::MeshData removeDuplicates(Trade::MeshData&& data) {
    CORRADE_ASSERT(data.attributeCount(),
        "MeshTools::removeDuplicates(): can't remove duplicates in an attributeless mesh",
        (Trade::MeshData{MeshPrimitive::Points, 0}));

    /* Turn the passed data into an interleaved owned mutable instance we can
       operate on -- owned() alone only makes the data owned, interleave()
       alone only makes the data interleaved (but those can stay non-owned).
       There's a chance the original data are already like this, in which case
       this will be just a passthrough. */
    Trade::MeshData ownedInterleaved = owned(interleave(std::move(data)));

    const Containers::StridedArrayView2D<char> vertexData = MeshTools::interleavedMutableData(ownedInterleaved);

    UnsignedInt uniqueVertexCount;
    Containers::Array<char> indexData;
    MeshIndexType indexType;
    if(ownedInterleaved.isIndexed()) {
        uniqueVertexCount = removeDuplicatesIndexedInPlace(ownedInterleaved.mutableIndices(), vertexData);
        indexData = ownedInterleaved.releaseIndexData();
        indexType = ownedInterleaved.indexType();
    } else {
        indexData = Containers::Array<char>{Containers::NoInit, ownedInterleaved.vertexCount()*sizeof(UnsignedInt)};
        uniqueVertexCount = removeDuplicatesInPlaceInto(vertexData, Containers::arrayCast<UnsignedInt>(indexData));
        indexType = MeshIndexType::UnsignedInt;
    }

    /* Allocate a new, shorter vertex data and copy the prefix */
    /** @todo better idea? even if we would use growable arrays in duplicate()
        or interleave() above, arrayResize() wouldn't release the excessive
        memory in any way. This is basically equivalent to STL's
        shrink_to_fit(), which also copies */
    Containers::Array<char> uniqueVertexData{Containers::NoInit, uniqueVertexCount*vertexData.size()[1]};
    Utility::copy(vertexData.prefix(uniqueVertexCount),
        Containers::StridedArrayView2D<char>{uniqueVertexData, {uniqueVertexCount, vertexData.size()[1]}});

    /* Route all attributes to the new vertex data */
    Containers::Array<Trade::MeshAttributeData> attributeData{ownedInterleaved.attributeCount()};
    for(UnsignedInt i = 0; i != ownedInterleaved.attributeCount(); ++i)
        attributeData[i] = Trade::MeshAttributeData{ownedInterleaved.attributeName(i),
            ownedInterleaved.attributeFormat(i),
            Containers::StridedArrayView1D<void>{uniqueVertexData,
                uniqueVertexData.data() + ownedInterleaved.attributeOffset(i),
                uniqueVertexCount,
                ownedInterleaved.attributeStride(i)},
            ownedInterleaved.attributeArraySize(i)};

    Trade::MeshIndexData indices{indexType, indexData};
    return Trade::MeshData{ownedInterleaved.primitive(),
        std::move(indexData), indices,
        std::move(uniqueVertexData), std::move(attributeData),
        uniqueVertexCount};
}

Trade::MeshData removeDuplicatesFuzzy(const Trade::MeshData& data, const Float floatEpsilon, const Double doubleEpsilon) {
    CORRADE_ASSERT(data.attributeCount(),
        "MeshTools::removeDuplicatesFuzzy(): can't remove duplicates in an attributeless mesh",
        (Trade::MeshData{MeshPrimitive::Points, 0}));

    /* Turn the passed data into an owned mutable instance we can operate on.
       There's a chance the original data are already like this, in which case
       this will be just a passthrough. */
    Trade::MeshData owned = MeshTools::owned(std::move(data));

    /* Allocate an interleaved index array for all attribs. If the mesh is
       already indexed, use the existing index count and copy the original
       index array there so the algorithm can operate directly on it. */
    Containers::Array<UnsignedInt> combinedIndexStorage;
    Containers::StridedArrayView2D<UnsignedInt> combinedIndices;

    /* If the mesh is not indexed, allocate for vertex count and keep it
       unitialized */
    combinedIndexStorage = Containers::Array<UnsignedInt>{/*Containers::NoInit,*/
        owned.vertexCount()*owned.attributeCount()};
    combinedIndices = Containers::StridedArrayView2D<UnsignedInt>{
        combinedIndexStorage,
        {owned.vertexCount(), owned.attributeCount()}};

    /* For each attribute decide if it needs to be fuzzy-deduplicated or not,
       calculate the epsilon size and call the appropriate API */
    const Containers::StridedArrayView2D<UnsignedInt> perAttributeIndices = combinedIndices.transposed<0, 1>();
    for(UnsignedInt i = 0; i != owned.attributeCount(); ++i) {
        const VertexFormat format = owned.attributeFormat(i);
        CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
            "MeshTools::removeDuplicatesFuzzy(): can't remove duplicates in an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(format)),
            (Trade::MeshData{MeshPrimitive::Points, 0}));

        const Containers::StridedArrayView1D<UnsignedInt> outputIndices = perAttributeIndices[i];

        /* Floats, with special attribute-dependent handling */
        const VertexFormat componentFormat = vertexFormatComponentFormat(format);
        if(componentFormat == VertexFormat::Float) {
            const Containers::StridedArrayView2D<Float> attribute = Containers::arrayCast<2, Float>(owned.mutableAttribute(i));

            /* Calculate scaled epsilon */
            Float attributeEpsilon = 0.0f;
            switch(owned.attributeName(i)) {
                /* These are usually in [0, 1] (color can be HDR but we
                   definitely don't want the epsilon to be higher there,
                   texture coords can be higher and repeat but the same
                   applies), use epsilon as-is */
                case Trade::MeshAttribute::TextureCoordinates:
                case Trade::MeshAttribute::Color:
                    attributeEpsilon = floatEpsilon;
                    break;

                /* Those are all [-1, 1], scale the epsilon 2x */
                case Trade::MeshAttribute::Normal:
                case Trade::MeshAttribute::Tangent:
                case Trade::MeshAttribute::Bitangent:
                    attributeEpsilon = 2.0f*floatEpsilon;
                    break;

                /* These have unbounded range. Do nothing but enumerate all
                   these here to silence warnings about unused enum values. */
                case Trade::MeshAttribute::Position:
                case Trade::MeshAttribute::Custom:
                    break;

                /* These shouldn't be floating point */
                /* LCOV_EXCL_START */
                case Trade::MeshAttribute::ObjectId:
                    CORRADE_INTERNAL_ASSERT_UNREACHABLE();
                /* LCOV_EXCL_STOP */
            }

            /* For unbounded and custom attributes scale the epsilon by data
               range */
            if(attributeEpsilon == 0.0f) {
                Float range = 0.0f;
                for(Containers::StridedArrayView1D<const Float> component: attribute.transposed<0, 1>())
                    range = Math::max(Range1D{Math::minmax(component)}.size(), range);
                attributeEpsilon = floatEpsilon*range;
            }

            removeDuplicatesFuzzyInPlaceIntoImplementation(attribute, outputIndices, attributeEpsilon);

        /* Doubles. No builtin attributes support those at the moment, so
           there's just the epsilon scaling based on attribute value range */
        } else if(componentFormat == VertexFormat::Double) {
            const Containers::StridedArrayView2D<Double> attribute = Containers::arrayCast<2, Double>(owned.mutableAttribute(i));

            Double range = 0.0;
            for(Containers::StridedArrayView1D<const Double> component: attribute.transposed<0, 1>())
                range = Math::max(Range1Dd{Math::minmax(component)}.size(), range);

            removeDuplicatesFuzzyInPlaceIntoImplementation(attribute, outputIndices, doubleEpsilon*range);

        /* Other attributes (integer, packed, half floats). No fuzzy
           comparison */
        } else {
            const Containers::StridedArrayView2D<char> attribute = owned.mutableAttribute(i);

            removeDuplicatesInPlaceInto(attribute, outputIndices);
        }
    }

    /* Make the combined index array unique */
    Containers::Array<char> indexData;
    UnsignedInt vertexCount;
    MeshIndexType indexType;

    if(!owned.isIndexed()) {
        indexData = Containers::Array<char>{combinedIndices.size()[0]*sizeof(UnsignedInt)};
        vertexCount = removeDuplicatesInPlaceInto(
            Containers::arrayCast<2, char>(combinedIndices),
            Containers::arrayCast<UnsignedInt>(indexData));
        indexType = MeshIndexType::UnsignedInt;
    } else {
        vertexCount = removeDuplicatesIndexedInPlace(
            owned.mutableIndices(),
            Containers::arrayCast<2, char>(combinedIndices));
        indexData = owned.releaseIndexData();
        indexType = owned.indexType();
    }

    combinedIndices = combinedIndices.prefix(vertexCount);

    Trade::MeshData layout = interleavedLayout(owned, vertexCount);
    Trade::MeshIndexData indices{indexType, indexData};
    Trade::MeshData out{layout.primitive(),
        std::move(indexData), indices,
        layout.releaseVertexData(), layout.releaseAttributeData(), vertexCount};

    {
        /* Duplicate the attributes according to the combined index buffer */
        const Containers::StridedArrayView2D<UnsignedInt> perAttributeIndices = combinedIndices.transposed<0, 1>();
        for(UnsignedInt i = 0; i != owned.attributeCount(); ++i)
            duplicateInto(perAttributeIndices[i].prefix(vertexCount), owned.attribute(i), out.mutableAttribute(i));
    }

    return out;
}

}}
