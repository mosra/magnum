/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "MeshData.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Algorithms.h>
#ifndef CORRADE_NO_ASSERT
#include <Corrade/Utility/Format.h>
#endif

#include "Magnum/Math/Color.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

MeshIndexData::MeshIndexData(const Containers::StridedArrayView2D<const char>& data) noexcept:
    /* Delegating to the constexpr function in order to reuse the stride size
       assert. The index type is not checked there so we can set it to nothing
       and then overwrite below. */
    MeshIndexData{MeshIndexType{}, {{nullptr, ~std::size_t{}}, data.data(), data.size()[0], data.stride()[0]}}
{
    /* Second dimension being zero indicates a non-indexed mesh */
    if(data.size()[1] == 0) {
        _type = MeshIndexType{};
        return;
    }

    if(data.size()[1] == 4) _type = MeshIndexType::UnsignedInt;
    else if(data.size()[1] == 2) _type = MeshIndexType::UnsignedShort;
    else if(data.size()[1] == 1) _type = MeshIndexType::UnsignedByte;
    else CORRADE_ASSERT_UNREACHABLE("Trade::MeshIndexData: expected index type size 1, 2 or 4 but got" << data.size()[1], );

    CORRADE_ASSERT(data.isContiguous<1>(),
        "Trade::MeshIndexData: second view dimension is not contiguous", );
}

MeshAttributeData::MeshAttributeData(const MeshAttribute name, const VertexFormat format, const Containers::StridedArrayView1D<const void>& data, UnsignedShort arraySize) noexcept: MeshAttributeData{nullptr, name, format, data, arraySize} {
    /* Yes, this calls into a constexpr function defined in the header --
       because I feel that makes more sense than duplicating the full assert
       logic */
}

MeshAttributeData::MeshAttributeData(const MeshAttribute name, const VertexFormat format, const Containers::StridedArrayView2D<const char>& data, UnsignedShort arraySize) noexcept: MeshAttributeData{nullptr, name, format, Containers::StridedArrayView1D<const void>{{data.data(), ~std::size_t{}}, data.size()[0], data.stride()[0]}, arraySize} {
    /* Yes, this calls into a constexpr function defined in the header --
       because I feel that makes more sense than duplicating the full assert
       logic */
    #ifndef CORRADE_NO_ASSERT
    if(arraySize) CORRADE_ASSERT(data.isEmpty()[0] || isVertexFormatImplementationSpecific(format) || data.size()[1] == vertexFormatSize(format)*arraySize,
        "Trade::MeshAttributeData: second view dimension size" << data.size()[1] << "doesn't match" << format << "and array size" << arraySize, );
    else CORRADE_ASSERT(data.isEmpty()[0] || isVertexFormatImplementationSpecific(format) || data.size()[1] == vertexFormatSize(format),
        "Trade::MeshAttributeData: second view dimension size" << data.size()[1] << "doesn't match" << format, );
    #endif
    CORRADE_ASSERT(data.isContiguous<1>(),
        "Trade::MeshAttributeData: second view dimension is not contiguous", );
}

Containers::Array<MeshAttributeData> meshAttributeDataNonOwningArray(const Containers::ArrayView<const MeshAttributeData> view) {
    return Containers::Array<MeshAttributeData>{const_cast<MeshAttributeData*>(view.data()), view.size(), Implementation::nonOwnedArrayDeleter};
}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const UnsignedInt vertexCount, const void* const importerState) noexcept:
    _primitive{primitive}, _indexType{indices._type},
    /* Bounds of index stride are checked in MeshIndexData already, so the
       cast alone is fine */
    _indexStride{Short(indices._data.stride())},
    _indexDataFlags{DataFlag::Owned|DataFlag::Mutable},
    _vertexDataFlags{DataFlag::Owned|DataFlag::Mutable},
    _importerState{importerState},
    _indices{static_cast<const char*>(indices._data.data())},
    _attributes{std::move(attributes)},
    _indexData{std::move(indexData)},
    _vertexData{std::move(vertexData)}
{
    /* Save index count, only if the indices are actually specified */
    if(_indexType != MeshIndexType{})
        _indexCount = indices._data.size();
    else _indexCount = 0;

    /* Save vertex count. If it's passed explicitly, use that (but still check
       that all attributes have the same vertex count for safety), otherwise
       expect at least one attribute  */
    #ifndef CORRADE_NO_ASSERT
    UnsignedInt expectedAttributeVertexCount;
    #endif
    if(_attributes.isEmpty()) {
        CORRADE_ASSERT(vertexCount != ImplicitVertexCount,
            "Trade::MeshData: vertex count can't be implicit if there are no attributes", );
        _vertexCount = vertexCount;
        #ifndef CORRADE_NO_ASSERT
        /* No attributes, so we won't be checking anything. GCC 11 however
           insists that "warning: ‘expectedAttributeVertexCount’ may be used
           uninitialized in this function" so I initialize the variable here to
           make the damn thing shut up. YOUR NEW WARNINGS ARE NOT USEFUL,
           GCC. */
        expectedAttributeVertexCount = ~UnsignedInt{};
        #endif
    } else if(vertexCount != ImplicitVertexCount) {
        _vertexCount = vertexCount;
        #ifndef CORRADE_NO_ASSERT
        expectedAttributeVertexCount = _attributes[0]._vertexCount;
        #endif
    } else {
        _vertexCount = _attributes[0]._vertexCount;
        #ifndef CORRADE_NO_ASSERT
        expectedAttributeVertexCount = _vertexCount;
        #endif
    }

    #ifndef CORRADE_NO_ASSERT
    CORRADE_ASSERT(_indexCount || _indexData.isEmpty(),
        "Trade::MeshData: indexData passed for a non-indexed mesh", );
    if(_indexCount) {
        const UnsignedInt typeSize =
            isMeshIndexTypeImplementationSpecific(_indexType) ? 0 :
                meshIndexTypeSize(_indexType);

        const void* begin = _indices;
        /* C integer promotion rules are weird, without the Int the result is
           an unsigned 32-bit value that messes things up on 64bit */
        const void* end = _indices + Int(_indexCount - 1)*_indexStride;
        /* Flip for negative stride */
        if(begin > end) std::swap(begin, end);
        /* Add the last element size to the higher address */
        end = static_cast<const char*>(end) + typeSize;

        CORRADE_ASSERT(begin >= _indexData.begin() && end <= _indexData.end(),
            "Trade::MeshData: indices [" << Debug::nospace << begin << Debug::nospace << ":" << Debug::nospace << end << Debug::nospace << "] are not contained in passed indexData array [" << Debug::nospace << static_cast<const void*>(_indexData.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_indexData.end()) << Debug::nospace << "]", );
    }

    /* Not checking what's already checked in MeshIndexData / MeshAttributeData
       constructors */
    for(std::size_t i = 0; i != _attributes.size(); ++i) {
        const MeshAttributeData& attribute = _attributes[i];
        CORRADE_ASSERT(attribute._format != VertexFormat{},
            "Trade::MeshData: attribute" << i << "doesn't specify anything", );
        CORRADE_ASSERT(attribute._vertexCount == expectedAttributeVertexCount,
            "Trade::MeshData: attribute" << i << "has" << attribute._vertexCount << "vertices but" << expectedAttributeVertexCount << "expected", );
        /* Check that the view fits into the provided vertex data array. For
           implementation-specific formats we don't know the size so use 0 to
           check at least partially. If the mesh has zero vertices, we don't
           check anything -- accessing the memory would be invalid anyway and
           enforcing this would lead to unnecessary friction with interleaved
           attributes of empty meshes. */
        if(_vertexCount) {
            const UnsignedInt typeSize =
                isVertexFormatImplementationSpecific(attribute._format) ? 0 :
                (vertexFormatSize(attribute._format)*
                (attribute._arraySize ? attribute._arraySize : 1));

            /* Both pointer and offset-only rely on basically same calculation,
               do it with offsets in a single place and only interpret as
               pointers in the non-offset-only check. Yes, yes, this may read
               the `pointer` union member through `offset`. */
            std::size_t begin = attribute._data.offset;
            /* C integer promotion rules are weird, without the Int the result
               is an unsigned 32-bit value that messes things up on 64bit */
            std::size_t end = begin + Int(_vertexCount - 1)*attribute._stride;
            /* Flip for negative stride */
            if(begin > end) std::swap(begin, end);
            /* Add the last element size to the higher address */
            end += typeSize;

            if(attribute._isOffsetOnly) {
                CORRADE_ASSERT(end <= _vertexData.size(),
                    "Trade::MeshData: offset-only attribute" << i << "spans" << end << "bytes but passed vertexData array has only" << _vertexData.size(), );
            } else {
                CORRADE_ASSERT(reinterpret_cast<const void*>(begin) >= _vertexData.begin() && reinterpret_cast<const void*>(end) <= _vertexData.end(),
                    "Trade::MeshData: attribute" << i << "[" << Debug::nospace << reinterpret_cast<const void*>(begin) << Debug::nospace << ":" << Debug::nospace << reinterpret_cast<const void*>(end) << Debug::nospace << "] is not contained in passed vertexData array [" << Debug::nospace << static_cast<const void*>(_vertexData.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_vertexData.end()) << Debug::nospace << "]", );
            }
        }
    }
    #endif
}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, std::initializer_list<MeshAttributeData> attributes, const UnsignedInt vertexCount, const void* const importerState): MeshData{primitive, std::move(indexData), indices, std::move(vertexData), Implementation::initializerListToArrayWithDefaultDeleter(attributes), vertexCount, importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, const Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const UnsignedInt vertexCount, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(indexData.data())), indexData.size(), Implementation::nonOwnedArrayDeleter}, indices, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(vertexData.data())), vertexData.size(), Implementation::nonOwnedArrayDeleter}, std::move(attributes), vertexCount, importerState} {
    CORRADE_ASSERT(!(indexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned index data but" << indexDataFlags, );
    CORRADE_ASSERT(!(vertexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned vertex data but" << vertexDataFlags, );
    _indexDataFlags = indexDataFlags;
    _vertexDataFlags = vertexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, const Containers::ArrayView<const void> vertexData, const std::initializer_list<MeshAttributeData> attributes, const UnsignedInt vertexCount, const void* const importerState): MeshData{primitive, indexDataFlags, indexData, indices, vertexDataFlags, vertexData, Implementation::initializerListToArrayWithDefaultDeleter(attributes), vertexCount, importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const UnsignedInt vertexCount, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(indexData.data())), indexData.size(), Implementation::nonOwnedArrayDeleter}, indices, std::move(vertexData), std::move(attributes), vertexCount, importerState} {
    CORRADE_ASSERT(!(indexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned index data but" << indexDataFlags, );
    _indexDataFlags = indexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, const std::initializer_list<MeshAttributeData> attributes, const UnsignedInt vertexCount, const void* const importerState): MeshData{primitive, indexDataFlags, indexData, indices, std::move(vertexData), Implementation::initializerListToArrayWithDefaultDeleter(attributes), vertexCount, importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const UnsignedInt vertexCount, const void* const importerState) noexcept: MeshData{primitive, std::move(indexData), indices, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(vertexData.data())), vertexData.size(), Implementation::nonOwnedArrayDeleter}, std::move(attributes), vertexCount, importerState} {
    CORRADE_ASSERT(!(vertexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned vertex data but" << vertexDataFlags, );
    _vertexDataFlags = vertexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, const Containers::ArrayView<const void> vertexData, const std::initializer_list<MeshAttributeData> attributes, const UnsignedInt vertexCount, const void* const importerState): MeshData{primitive, std::move(indexData), indices, vertexDataFlags, vertexData, Implementation::initializerListToArrayWithDefaultDeleter(attributes), vertexCount, importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const UnsignedInt vertexCount, const void* const importerState) noexcept: MeshData{primitive, {}, MeshIndexData{}, std::move(vertexData), std::move(attributes), vertexCount, importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& vertexData, const std::initializer_list<MeshAttributeData> attributes, const UnsignedInt vertexCount, const void* const importerState): MeshData{primitive, std::move(vertexData), Implementation::initializerListToArrayWithDefaultDeleter(attributes), vertexCount, importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const UnsignedInt vertexCount, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(vertexData.data())), vertexData.size(), Implementation::nonOwnedArrayDeleter}, std::move(attributes), vertexCount, importerState} {
    CORRADE_ASSERT(!(vertexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned vertex data but" << vertexDataFlags, );
    _vertexDataFlags = vertexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, std::initializer_list<MeshAttributeData> attributes, const UnsignedInt vertexCount, const void* const importerState): MeshData{primitive, vertexDataFlags, vertexData, Implementation::initializerListToArrayWithDefaultDeleter(attributes), vertexCount, importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, const UnsignedInt vertexCount, const void* const importerState) noexcept: MeshData{primitive, std::move(indexData), indices, {}, {}, vertexCount, importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, const UnsignedInt vertexCount, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(indexData.data())), indexData.size(), Implementation::nonOwnedArrayDeleter}, indices, vertexCount, importerState} {
    CORRADE_ASSERT(!(indexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned index data but" << indexDataFlags, );
    _indexDataFlags = indexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const UnsignedInt vertexCount, const void* const importerState) noexcept: MeshData{primitive, {}, MeshIndexData{}, {}, {}, vertexCount, importerState} {}

MeshData::MeshData(MeshData&&) noexcept = default;

MeshData::~MeshData() = default;

MeshData& MeshData::operator=(MeshData&&) noexcept = default;

Containers::ArrayView<char> MeshData::mutableIndexData() & {
    CORRADE_ASSERT(_indexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableIndexData(): index data not mutable", {});
    return _indexData;
}

Containers::ArrayView<char> MeshData::mutableVertexData() & {
    CORRADE_ASSERT(_vertexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableVertexData(): vertex data not mutable", {});
    return _vertexData;
}

UnsignedInt MeshData::indexCount() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indexCount(): the mesh is not indexed", {});
    return _indexCount;
}

MeshIndexType MeshData::indexType() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indexType(): the mesh is not indexed", {});
    return _indexType;
}

std::size_t MeshData::indexOffset() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indexOffset(): the mesh is not indexed", {});
    return _indices - _indexData.data();
}

Short MeshData::indexStride() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indexStride(): the mesh is not indexed", {});
    return _indexStride;
}

Containers::StridedArrayView2D<const char> MeshData::indices() const {
    /* For a non-indexed mesh returning zero size in both dimensions, indexed
       mesh with zero indices still has the second dimension non-zero */
    if(!isIndexed()) return {};
    const std::size_t indexTypeSize =
        isMeshIndexTypeImplementationSpecific(_indexType) ?
            Math::abs(_indexStride) : meshIndexTypeSize(_indexType);
    /* Build a 2D view using information about index type size and stride.
       We're *sure* the view is correct, so faking the view size */
    return {{_indices, ~std::size_t{}}, {_indexCount, indexTypeSize}, {_indexStride, 1}};
}

Containers::StridedArrayView2D<char> MeshData::mutableIndices() {
    CORRADE_ASSERT(_indexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableIndices(): index data not mutable", {});
    /* For a non-indexed mesh returning zero size in both dimensions, indexed
       mesh with zero indices still has the second dimension non-zero */
    if(!isIndexed()) return {};
    const std::size_t indexTypeSize =
        isMeshIndexTypeImplementationSpecific(_indexType) ?
            Math::abs(_indexStride) : meshIndexTypeSize(_indexType);
    /* Build a 2D view using information about index type size and stride.
       We're *sure* the view is correct, so faking the view size */
    Containers::StridedArrayView2D<const char> out{{_indices, ~std::size_t{}}, {_indexCount, indexTypeSize}, {_indexStride, 1}};
    /** @todo some arrayConstCast? UGH */
    return Containers::StridedArrayView2D<char>{
        /* The view size is there only for a size assert, we're pretty sure the
           view is valid */
        {static_cast<char*>(const_cast<void*>(out.data())), ~std::size_t{}},
        out.size(), out.stride()};
}

Containers::StridedArrayView1D<const void> MeshData::attributeDataViewInternal(const MeshAttributeData& attribute) const {
    return Containers::StridedArrayView1D<const void>{
        /* We're *sure* the view is correct, so faking the view size */
        /** @todo better ideas for the StridedArrayView API? */
        {attribute._isOffsetOnly ? _vertexData.data() + attribute._data.offset :
            attribute._data.pointer, ~std::size_t{}},
        /* Not using attribute._vertexCount because that gets stale after
           releaseVertexData() gets called, and then we would need to slice the
           result inside attribute() and elsewhere anyway */
        _vertexCount, attribute._stride};
}

MeshAttributeData MeshData::attributeData(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeData(): index" << id << "out of range for" << _attributes.size() << "attributes", MeshAttributeData{});
    const MeshAttributeData& attribute = _attributes[id];
    return MeshAttributeData{attribute._name, attribute._format, attributeDataViewInternal(attribute), attribute._arraySize};
}

MeshAttribute MeshData::attributeName(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeName(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id]._name;
}

UnsignedInt MeshData::attributeId(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeId(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    const MeshAttribute name = _attributes[id]._name;
    UnsignedInt count = 0;
    for(UnsignedInt i = 0; i != id; ++i)
        if(_attributes[i]._name == name) ++count;
    return count;
}

VertexFormat MeshData::attributeFormat(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeFormat(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id]._format;
}

std::size_t MeshData::attributeOffset(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeOffset(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id]._isOffsetOnly ? _attributes[id]._data.offset :
        static_cast<const char*>(_attributes[id]._data.pointer) - _vertexData.data();
}

Short MeshData::attributeStride(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeStride(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id]._stride;
}

UnsignedShort MeshData::attributeArraySize(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeArraySize(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id]._arraySize;
}

UnsignedInt MeshData::attributeCount(const MeshAttribute name) const {
    UnsignedInt count = 0;
    for(const MeshAttributeData& attribute: _attributes)
        if(attribute._name == name) ++count;
    return count;
}

UnsignedInt MeshData::findAttributeIdInternal(const MeshAttribute name, UnsignedInt id) const {
    for(std::size_t i = 0; i != _attributes.size(); ++i) {
        if(_attributes[i]._name != name) continue;
        if(id-- == 0) return i;
    }
    return ~UnsignedInt{};
}

Containers::Optional<UnsignedInt> MeshData::findAttributeId(const MeshAttribute name, UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id);
    return attributeId == ~UnsignedInt{} ? Containers::Optional<UnsignedInt>{} : attributeId;
}

UnsignedInt MeshData::attributeId(const MeshAttribute name, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeId(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attributeId;
}

VertexFormat MeshData::attributeFormat(const MeshAttribute name, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeFormat(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return _attributes[attributeId]._format;
}

std::size_t MeshData::attributeOffset(const MeshAttribute name, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeOffset(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    /* Calculation is non-trivial, delegating */
    return attributeOffset(attributeId);
}

Short MeshData::attributeStride(const MeshAttribute name, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeStride(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return _attributes[attributeId]._stride;
}

UnsignedShort MeshData::attributeArraySize(const MeshAttribute name, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeArraySize(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return _attributes[attributeId]._arraySize;
}

Containers::StridedArrayView2D<const char> MeshData::attribute(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attribute(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    const MeshAttributeData& attribute = _attributes[id];
    /* Build a 2D view using information about attribute type size */
    return Containers::arrayCast<2, const char>(
        attributeDataViewInternal(attribute),
        isVertexFormatImplementationSpecific(attribute._format) ?
            Math::abs(attribute._stride) : vertexFormatSize(attribute._format)*
                (attribute._arraySize ? attribute._arraySize : 1));
}

Containers::StridedArrayView2D<char> MeshData::mutableAttribute(const UnsignedInt id) {
    CORRADE_ASSERT(_vertexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableAttribute(): vertex data not mutable", {});
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::mutableAttribute(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    const MeshAttributeData& attribute = _attributes[id];
    /* Build a 2D view using information about attribute type size */
    const auto out = Containers::arrayCast<2, const char>(
        attributeDataViewInternal(attribute),
        isVertexFormatImplementationSpecific(attribute._format) ?
            Math::abs(attribute._stride) : vertexFormatSize(attribute._format)*
                (attribute._arraySize ? attribute._arraySize : 1));
    /** @todo some arrayConstCast? UGH */
    return Containers::StridedArrayView2D<char>{
        /* The view size is there only for a size assert, we're pretty sure the
           view is valid */
        {static_cast<char*>(const_cast<void*>(out.data())), ~std::size_t{}},
        out.size(), out.stride()};
}

Containers::StridedArrayView2D<const char> MeshData::attribute(const MeshAttribute name, UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attribute(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attribute(attributeId);
}

Containers::StridedArrayView2D<char> MeshData::mutableAttribute(const MeshAttribute name, UnsignedInt id) {
    CORRADE_ASSERT(_vertexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableAttribute(): vertex data not mutable", {});
    const UnsignedInt attributeId = findAttributeIdInternal(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::mutableAttribute(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return mutableAttribute(attributeId);
}

void MeshData::indicesInto(const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indicesInto(): the mesh is not indexed", );
    CORRADE_ASSERT(destination.size() == indexCount(), "Trade::MeshData::indicesInto(): expected a view with" << indexCount() << "elements but got" << destination.size(), );
    CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(_indexType),
        "Trade::MeshData::indicesInto(): can't extract data out of an implementation-specific index type" << reinterpret_cast<void*>(meshIndexTypeUnwrap(_indexType)), );
    const auto destination1ui = Containers::arrayCast<2, UnsignedInt>(destination);

    const Containers::StridedArrayView2D<const char> indexData = indices();

    if(_indexType == MeshIndexType::UnsignedInt)
        return Utility::copy(Containers::arrayCast<1, const UnsignedInt>(indexData), destination);
    else if(_indexType == MeshIndexType::UnsignedShort)
        return Math::castInto(Containers::arrayCast<2, const UnsignedShort>(indexData), destination1ui);
    else if(_indexType == MeshIndexType::UnsignedByte)
        return Math::castInto(Containers::arrayCast<2, const UnsignedByte>(indexData), destination1ui);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<UnsignedInt> MeshData::indicesAsArray() const {
    /* Repeating the assert here because otherwise it would fire in
       indexCount() which may be confusing */
    CORRADE_ASSERT(isIndexed(), "Trade::MeshData::indicesAsArray(): the mesh is not indexed", {});
    Containers::Array<UnsignedInt> output{NoInit, indexCount()};
    indicesInto(output);
    return output;
}

void MeshData::positions2DInto(const Containers::StridedArrayView1D<Vector2>& destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(MeshAttribute::Position, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::positions2DInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::Position) << "position attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::positions2DInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::positions2DInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );
    const Containers::StridedArrayView1D<const void> attributeData = attributeDataViewInternal(attribute);
    const auto destination2f = Containers::arrayCast<2, Float>(destination);

    /* Copy 2D positions as-is, for 3D positions ignore Z */
    if(attribute._format == VertexFormat::Vector2 ||
       attribute._format == VertexFormat::Vector3)
        Utility::copy(Containers::arrayCast<const Vector2>(attributeData), destination);
    else if(attribute._format == VertexFormat::Vector2h ||
            attribute._format == VertexFormat::Vector3h)
        Math::unpackHalfInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2ub ||
            attribute._format == VertexFormat::Vector3ub)
        Math::castInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2b ||
            attribute._format == VertexFormat::Vector3b)
        Math::castInto(Containers::arrayCast<2, const Byte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2us ||
            attribute._format == VertexFormat::Vector3us)
        Math::castInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2s ||
            attribute._format == VertexFormat::Vector3s)
        Math::castInto(Containers::arrayCast<2, const Short>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2ubNormalized ||
            attribute._format == VertexFormat::Vector3ubNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2bNormalized ||
            attribute._format == VertexFormat::Vector3bNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Byte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2usNormalized ||
            attribute._format == VertexFormat::Vector3usNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2sNormalized ||
            attribute._format == VertexFormat::Vector3sNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Short>(attributeData, 2), destination2f);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<Vector2> MeshData::positions2DAsArray(const UnsignedInt id) const {
    Containers::Array<Vector2> out{NoInit, _vertexCount};
    positions2DInto(out, id);
    return out;
}

void MeshData::positions3DInto(const Containers::StridedArrayView1D<Vector3>& destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(MeshAttribute::Position, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::positions3DInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::Position) << "position attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::positions3DInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::positions3DInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );
    const Containers::StridedArrayView1D<const void> attributeData = attributeDataViewInternal(attribute);
    const Containers::StridedArrayView2D<Float> destination2f = Containers::arrayCast<2, Float>(Containers::arrayCast<Vector2>(destination));
    const Containers::StridedArrayView2D<Float> destination3f = Containers::arrayCast<2, Float>(destination);

    /* For 2D positions copy the XY part to the first two components */
    if(attribute._format == VertexFormat::Vector2)
        Utility::copy(Containers::arrayCast<const Vector2>(attributeData),
                      Containers::arrayCast<Vector2>(destination));
    else if(attribute._format == VertexFormat::Vector2h)
        Math::unpackHalfInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2ub)
        Math::castInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2b)
        Math::castInto(Containers::arrayCast<2, const Byte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2us)
        Math::castInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2s)
        Math::castInto(Containers::arrayCast<2, const Short>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2ubNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2bNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Byte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2usNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2sNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Short>(attributeData, 2), destination2f);

    /* Copy 3D positions as-is */
    else if(attribute._format == VertexFormat::Vector3)
        Utility::copy(Containers::arrayCast<const Vector3>(attributeData), destination);
    else if(attribute._format == VertexFormat::Vector3h)
        Math::unpackHalfInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 3), destination3f);
    else if(attribute._format == VertexFormat::Vector3ub)
        Math::castInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 3), destination3f);
    else if(attribute._format == VertexFormat::Vector3b)
        Math::castInto(Containers::arrayCast<2, const Byte>(attributeData, 3), destination3f);
    else if(attribute._format == VertexFormat::Vector3us)
        Math::castInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 3), destination3f);
    else if(attribute._format == VertexFormat::Vector3s)
        Math::castInto(Containers::arrayCast<2, const Short>(attributeData, 3), destination3f);
    else if(attribute._format == VertexFormat::Vector3ubNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 3), destination3f);
    else if(attribute._format == VertexFormat::Vector3bNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Byte>(attributeData, 3), destination3f);
    else if(attribute._format == VertexFormat::Vector3usNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 3), destination3f);
    else if(attribute._format == VertexFormat::Vector3sNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Short>(attributeData, 3), destination3f);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* For 2D positions finally fill the Z with a single value */
    if(attribute._format == VertexFormat::Vector2 ||
       attribute._format == VertexFormat::Vector2h ||
       attribute._format == VertexFormat::Vector2ub ||
       attribute._format == VertexFormat::Vector2b ||
       attribute._format == VertexFormat::Vector2us ||
       attribute._format == VertexFormat::Vector2s ||
       attribute._format == VertexFormat::Vector2ubNormalized ||
       attribute._format == VertexFormat::Vector2bNormalized ||
       attribute._format == VertexFormat::Vector2usNormalized ||
       attribute._format == VertexFormat::Vector2sNormalized) {
        constexpr Float z[1]{0.0f};
        Utility::copy(
            Containers::stridedArrayView(z).broadcasted<0>(_vertexCount),
            destination3f.transposed<0, 1>()[2]);
    }
}

Containers::Array<Vector3> MeshData::positions3DAsArray(const UnsignedInt id) const {
    Containers::Array<Vector3> out{NoInit, _vertexCount};
    positions3DInto(out, id);
    return out;
}

namespace {

void tangentsOrNormalsInto(const Containers::StridedArrayView1D<const void>& attributeData, const Containers::StridedArrayView1D<Vector3>& destination, VertexFormat format) {
    const auto destination3f = Containers::arrayCast<2, Float>(destination);

    if(format == VertexFormat::Vector3)
        Utility::copy(Containers::arrayCast<const Vector3>(attributeData), destination);
    else if(format == VertexFormat::Vector3h)
        Math::unpackHalfInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 3), destination3f);
    else if(format == VertexFormat::Vector3bNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Byte>(attributeData, 3), destination3f);
    else if(format == VertexFormat::Vector3sNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Short>(attributeData, 3), destination3f);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

}

void MeshData::tangentsInto(const Containers::StridedArrayView1D<Vector3>& destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(MeshAttribute::Tangent, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::tangentsInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::Tangent) << "tangent attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::tangentsInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::tangentsInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );

    /* If the tangent is four-component, ignore the last component; otherwise
       copy/unpack given format directly */
    VertexFormat format;
    if(attribute._format == VertexFormat::Vector4)
        format = VertexFormat::Vector3;
    else if(attribute._format == VertexFormat::Vector4h)
        format = VertexFormat::Vector3h;
    else if(attribute._format == VertexFormat::Vector4bNormalized)
        format = VertexFormat::Vector3bNormalized;
    else if(attribute._format == VertexFormat::Vector4sNormalized)
        format = VertexFormat::Vector3sNormalized;
    else format = attribute._format;
    tangentsOrNormalsInto(attributeDataViewInternal(attribute), destination, format);
}

Containers::Array<Vector3> MeshData::tangentsAsArray(const UnsignedInt id) const {
    Containers::Array<Vector3> out{NoInit, _vertexCount};
    tangentsInto(out, id);
    return out;
}

void MeshData::bitangentSignsInto(const Containers::StridedArrayView1D<Float>& destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(MeshAttribute::Tangent, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::bitangentSignsInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::Tangent) << "tangent attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::bitangentSignsInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::bitangentSignsInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );
    const Containers::StridedArrayView1D<const void> attributeData = attributeDataViewInternal(attribute);
    const auto destination1f = Containers::arrayCast<2, Float>(destination);

    if(attribute._format == VertexFormat::Vector4)
        Utility::copy(Containers::arrayCast<2, const Float>(attributeData, 4).transposed<0, 1>()[3], destination);
    else if(attribute._format == VertexFormat::Vector4h)
        Math::unpackHalfInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 4).exceptPrefix({0, 3}), destination1f);
    else if(attribute._format == VertexFormat::Vector4bNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Byte>(attributeData, 4).exceptPrefix({0, 3}), destination1f);
    else if(attribute._format == VertexFormat::Vector4sNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Short>(attributeData, 4).exceptPrefix({0, 3}), destination1f);
    else CORRADE_ASSERT_UNREACHABLE("Trade::MeshData::bitangentSignsInto(): expected four-component tangents, but got" << attribute._format, );
}

Containers::Array<Float> MeshData::bitangentSignsAsArray(const UnsignedInt id) const {
    Containers::Array<Float> out{NoInit, _vertexCount};
    bitangentSignsInto(out, id);
    return out;
}

void MeshData::bitangentsInto(const Containers::StridedArrayView1D<Vector3>& destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(MeshAttribute::Bitangent, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::bitangentsInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::Bitangent) << "bitangent attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::bitangentsInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::bitangentsInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );
    tangentsOrNormalsInto(attributeDataViewInternal(attribute), destination, attribute._format);
}

Containers::Array<Vector3> MeshData::bitangentsAsArray(const UnsignedInt id) const {
    Containers::Array<Vector3> out{NoInit, _vertexCount};
    bitangentsInto(out, id);
    return out;
}

void MeshData::normalsInto(const Containers::StridedArrayView1D<Vector3>& destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(MeshAttribute::Normal, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::normalsInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::Normal) << "normal attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::normalsInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::normalsInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );
    tangentsOrNormalsInto(attributeDataViewInternal(attribute), destination, attribute._format);
}

Containers::Array<Vector3> MeshData::normalsAsArray(const UnsignedInt id) const {
    Containers::Array<Vector3> out{NoInit, _vertexCount};
    normalsInto(out, id);
    return out;
}

void MeshData::textureCoordinates2DInto(const Containers::StridedArrayView1D<Vector2>& destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(MeshAttribute::TextureCoordinates, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::textureCoordinates2DInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::TextureCoordinates) << "texture coordinate attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::textureCoordinates2DInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::textureCoordinatesInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );
    const Containers::StridedArrayView1D<const void> attributeData = attributeDataViewInternal(attribute);
    const auto destination2f = Containers::arrayCast<2, Float>(destination);

    if(attribute._format == VertexFormat::Vector2)
        Utility::copy(Containers::arrayCast<const Vector2>(attributeData), destination);
    else if(attribute._format == VertexFormat::Vector2h)
        Math::unpackHalfInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2ub)
        Math::castInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2b)
        Math::castInto(Containers::arrayCast<2, const Byte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2us)
        Math::castInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2s)
        Math::castInto(Containers::arrayCast<2, const Short>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2ubNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2bNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Byte>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2usNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 2), destination2f);
    else if(attribute._format == VertexFormat::Vector2sNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Short>(attributeData, 2), destination2f);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<Vector2> MeshData::textureCoordinates2DAsArray(const UnsignedInt id) const {
    Containers::Array<Vector2> out{NoInit, _vertexCount};
    textureCoordinates2DInto(out, id);
    return out;
}

void MeshData::colorsInto(const Containers::StridedArrayView1D<Color4>& destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(MeshAttribute::Color, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::colorsInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::Color) << "color attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::colorsInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::colorsInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );
    const Containers::StridedArrayView1D<const void> attributeData = attributeDataViewInternal(attribute);
    const Containers::StridedArrayView2D<Float> destination3f = Containers::arrayCast<2, Float>(Containers::arrayCast<Vector3>(destination));
    const Containers::StridedArrayView2D<Float> destination4f = Containers::arrayCast<2, Float>(destination);

    /* For three-component colors copy the RGB part to the first three
       components */
    if(attribute._format == VertexFormat::Vector3)
        Utility::copy(Containers::arrayCast<const Vector3>(attributeData),
                      Containers::arrayCast<Vector3>(destination));
    else if(attribute._format == VertexFormat::Vector3h)
        Math::unpackHalfInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 3), destination3f);
    else if(attribute._format == VertexFormat::Vector3ubNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 3), destination3f);
    else if(attribute._format == VertexFormat::Vector3usNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 3), destination3f);

    /* Copy four-component colors as-is */
    else if(attribute._format == VertexFormat::Vector4)
        Utility::copy(Containers::arrayCast<const Vector4>(attributeData),
                      Containers::arrayCast<Vector4>(destination));
    else if(attribute._format == VertexFormat::Vector4h)
        Math::unpackHalfInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 4), destination4f);
    else if(attribute._format == VertexFormat::Vector4ubNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 4), destination4f);
    else if(attribute._format == VertexFormat::Vector4usNormalized)
        Math::unpackInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 4), destination4f);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* For three-component colors finally fill the alpha with a single value */
    if(attribute._format == VertexFormat::Vector3 ||
       attribute._format == VertexFormat::Vector3h ||
       attribute._format == VertexFormat::Vector3ubNormalized ||
       attribute._format == VertexFormat::Vector3usNormalized) {
        constexpr Float alpha[1]{1.0f};
        Utility::copy(
            Containers::stridedArrayView(alpha).broadcasted<0>(_vertexCount),
            destination4f.transposed<0, 1>()[3]);
    }
}

Containers::Array<Color4> MeshData::colorsAsArray(const UnsignedInt id) const {
    Containers::Array<Color4> out{NoInit, _vertexCount};
    colorsInto(out, id);
    return out;
}

void MeshData::objectIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = findAttributeIdInternal(MeshAttribute::ObjectId, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::objectIdsInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::ObjectId) << "object ID attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::objectIdsInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::objectIdsInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );
    const Containers::StridedArrayView1D<const void> attributeData = attributeDataViewInternal(attribute);
    const auto destination1ui = Containers::arrayCast<2, UnsignedInt>(destination);

    if(attribute._format == VertexFormat::UnsignedInt)
        Utility::copy(Containers::arrayCast<const UnsignedInt>(attributeData), destination);
    else if(attribute._format == VertexFormat::UnsignedShort)
        Math::castInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 1), destination1ui);
    else if(attribute._format == VertexFormat::UnsignedByte)
        Math::castInto(Containers::arrayCast<2, const UnsignedByte>(attributeData, 1), destination1ui);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<UnsignedInt> MeshData::objectIdsAsArray(const UnsignedInt id) const {
    Containers::Array<UnsignedInt> out{NoInit, _vertexCount};
    objectIdsInto(out, id);
    return out;
}

Containers::Array<char> MeshData::releaseIndexData() {
    _indexCount = 0;
    Containers::Array<char> out = std::move(_indexData);
    _indexData = Containers::Array<char>{out.data(), 0, Implementation::nonOwnedArrayDeleter};
    return out;
}

Containers::Array<MeshAttributeData> MeshData::releaseAttributeData() {
    return std::move(_attributes);
}

Containers::Array<char> MeshData::releaseVertexData() {
    _vertexCount = 0;
    Containers::Array<char> out = std::move(_vertexData);
    _vertexData = Containers::Array<char>{out.data(), 0, Implementation::nonOwnedArrayDeleter};
    return out;
}

Debug& operator<<(Debug& debug, const MeshAttribute value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::MeshAttribute" << Debug::nospace;

    if(isMeshAttributeCustom(value))
        return debug << (packed ? "Custom(" : "::Custom(") << Debug::nospace << meshAttributeCustom(value) << Debug::nospace << ")";

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MeshAttribute::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(Position)
        _c(Tangent)
        _c(Bitangent)
        _c(Normal)
        _c(TextureCoordinates)
        _c(Color)
        _c(ObjectId)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << (packed ? "" : ")");
}

}}
