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

#include "MeshData.h"

#include <Corrade/Utility/Algorithms.h>
#ifndef CORRADE_NO_ASSERT
#include <Corrade/Utility/Format.h>
#endif

#include "Magnum/Math/Color.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

MeshIndexData::MeshIndexData(const MeshIndexType type, const Containers::ArrayView<const void> data) noexcept: _type{type}, _data{data} {
    /* Yes, this calls into a constexpr function defined in the header --
       because I feel that makes more sense than duplicating the full assert
       logic */
    CORRADE_ASSERT(data.size()%meshIndexTypeSize(type) == 0,
        "Trade::MeshIndexData: view size" << data.size() << "does not correspond to" << type, );
}

MeshIndexData::MeshIndexData(const Containers::StridedArrayView2D<const char>& data) noexcept {
    /* Second dimension being zero indicates a non-indexed mesh */
    if(data.size()[1] == 0) {
        _type = MeshIndexType{};
        return;
    }

    if(data.size()[1] == 4) _type = MeshIndexType::UnsignedInt;
    else if(data.size()[1] == 2) _type = MeshIndexType::UnsignedShort;
    else if(data.size()[1] == 1) _type = MeshIndexType::UnsignedByte;
    else CORRADE_ASSERT_UNREACHABLE("Trade::MeshIndexData: expected index type size 1, 2 or 4 but got" << data.size()[1], );

    CORRADE_ASSERT(data.isContiguous(), "Trade::MeshIndexData: view is not contiguous", );
    _data = data.asContiguous();
}

MeshAttributeData::MeshAttributeData(const MeshAttribute name, const VertexFormat format, const Containers::StridedArrayView1D<const void>& data, UnsignedShort arraySize) noexcept: MeshAttributeData{nullptr, name, format, data, arraySize} {
    /* Yes, this calls into a constexpr function defined in the header --
       because I feel that makes more sense than duplicating the full assert
       logic */
    /** @todo support zero / negative stride? would be hard to transfer to GL */
    CORRADE_ASSERT(data.empty() || isVertexFormatImplementationSpecific(format) || std::ptrdiff_t(vertexFormatSize(format))*(arraySize ? arraySize : 1) <= data.stride(),
        "Trade::MeshAttributeData: expected stride to be positive and enough to fit" << format << Debug::nospace << (arraySize ? Utility::format("[{}]", arraySize).data() : "") << Debug::nospace << ", got" << data.stride(), );
}

MeshAttributeData::MeshAttributeData(const MeshAttribute name, const VertexFormat format, const Containers::StridedArrayView2D<const char>& data, UnsignedShort arraySize) noexcept: MeshAttributeData{nullptr, name, format, Containers::StridedArrayView1D<const void>{{data.data(), ~std::size_t{}}, data.size()[0], data.stride()[0]}, arraySize} {
    /* Yes, this calls into a constexpr function defined in the header --
       because I feel that makes more sense than duplicating the full assert
       logic */
    #ifndef CORRADE_NO_ASSERT
    if(arraySize) CORRADE_ASSERT(data.empty()[0] || isVertexFormatImplementationSpecific(format) || data.size()[1] == vertexFormatSize(format)*arraySize,
        "Trade::MeshAttributeData: second view dimension size" << data.size()[1] << "doesn't match" << format << "and array size" << arraySize, );
    else CORRADE_ASSERT(data.empty()[0] || isVertexFormatImplementationSpecific(format) || data.size()[1] == vertexFormatSize(format),
        "Trade::MeshAttributeData: second view dimension size" << data.size()[1] << "doesn't match" << format, );
    #endif
    CORRADE_ASSERT(data.isContiguous<1>(),
        "Trade::MeshAttributeData: second view dimension is not contiguous", );
}

Containers::Array<MeshAttributeData> meshAttributeDataNonOwningArray(const Containers::ArrayView<const MeshAttributeData> view) {
    /* Ugly, eh? */
    return Containers::Array<MeshAttributeData>{const_cast<MeshAttributeData*>(view.data()), view.size(), reinterpret_cast<void(*)(MeshAttributeData*, std::size_t)>(Implementation::nonOwnedArrayDeleter)};
}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const UnsignedInt vertexCount, const void* const importerState) noexcept: _primitive{primitive}, _indexType{indices._type}, _indexDataFlags{DataFlag::Owned|DataFlag::Mutable}, _vertexDataFlags{DataFlag::Owned|DataFlag::Mutable}, _importerState{importerState}, _indices{static_cast<const char*>(indices._data.data())}, _attributes{std::move(attributes)}, _indexData{std::move(indexData)}, _vertexData{std::move(vertexData)} {
    /* Save index count, only if the indices are actually specified */
    if(_indexType != MeshIndexType{})
        _indexCount = UnsignedInt(indices._data.size()/meshIndexTypeSize(indices._type));
    else _indexCount = 0;

    /* Save vertex count. If it's passed explicitly, use that (but still check
       that all attributes have the same vertex count for safety), otherwise
       expect at least one attribute  */
    #ifndef CORRADE_NO_ASSERT
    UnsignedInt expectedAttributeVertexCount;
    #endif
    if(_attributes.empty()) {
        CORRADE_ASSERT(vertexCount != ImplicitVertexCount,
            "Trade::MeshData: vertex count can't be implicit if there are no attributes", );
        _vertexCount = vertexCount;
        /* No attributes, so we won't be checking anything */
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

    CORRADE_ASSERT(_indexCount || _indexData.empty(),
        "Trade::MeshData: indexData passed for a non-indexed mesh", );
    CORRADE_ASSERT(!_indices || (_indices >= _indexData.begin() && _indices + indices._data.size() <= _indexData.end()),
        "Trade::MeshData: indices [" << Debug::nospace << static_cast<const void*>(_indices) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_indices + indices._data.size()) << Debug::nospace << "] are not contained in passed indexData array [" << Debug::nospace << static_cast<const void*>(_indexData.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_indexData.end()) << Debug::nospace << "]", );

    #ifndef CORRADE_NO_ASSERT
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
           check at least partially. */
        const UnsignedInt typeSize =
            isVertexFormatImplementationSpecific(attribute._format) ? 0 :
            vertexFormatSize(attribute._format);
        if(attribute._isOffsetOnly) {
            const std::size_t size = attribute._data.offset + (_vertexCount - 1)*attribute._stride + typeSize;
            CORRADE_ASSERT(!_vertexCount || size <= _vertexData.size(),
                "Trade::MeshData: offset attribute" << i << "spans" << size << "bytes but passed vertexData array has only" << _vertexData.size(), );
        } else {
            const void* const begin = static_cast<const char*>(attribute._data.pointer);
            const void* const end = static_cast<const char*>(attribute._data.pointer) + (_vertexCount - 1)*attribute._stride + typeSize;
            CORRADE_ASSERT(!_vertexCount || (begin >= _vertexData.begin() && end <= _vertexData.end()),
                "Trade::MeshData: attribute" << i << "[" << Debug::nospace << begin << Debug::nospace << ":" << Debug::nospace << end << Debug::nospace << "] is not contained in passed vertexData array [" << Debug::nospace << static_cast<const void*>(_vertexData.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_vertexData.end()) << Debug::nospace << "]", );
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

MeshData::~MeshData() = default;

MeshData::MeshData(MeshData&&) noexcept = default;

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

Containers::StridedArrayView2D<const char> MeshData::indices() const {
    /* For a non-indexed mesh returning zero size in both dimensions, indexed
       mesh with zero indices sill has the second dimension non-zero */
    if(!isIndexed()) return {};
    const std::size_t indexTypeSize = meshIndexTypeSize(_indexType);
    /* Build a 2D view using information about attribute type size */
    return {{_indices, _indexCount*indexTypeSize}, {_indexCount, indexTypeSize}};
}

Containers::StridedArrayView2D<char> MeshData::mutableIndices() {
    CORRADE_ASSERT(_indexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableIndices(): index data not mutable", {});
    /* For a non-indexed mesh returning zero size in both dimensions, indexed
       mesh with zero indices sill has the second dimension non-zero */
    if(!isIndexed()) return {};
    const std::size_t indexTypeSize = meshIndexTypeSize(_indexType);
    /* Build a 2D view using information about index type size */
    Containers::StridedArrayView2D<const char> out{{_indices, _indexCount*indexTypeSize}, {_indexCount, indexTypeSize}};
    /** @todo some arrayConstCast? UGH */
    return Containers::StridedArrayView2D<char>{
        /* The view size is there only for a size assert, we're pretty sure the
           view is valid */
        {static_cast<char*>(const_cast<void*>(out.data())), ~std::size_t{}},
        out.size(), out.stride()};
}

MeshAttributeData MeshData::attributeData(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeData(): index" << id << "out of range for" << _attributes.size() << "attributes", MeshAttributeData{});
    const MeshAttributeData& attribute = _attributes[id];
    return MeshAttributeData{attribute._name, attribute._format, attributeDataViewInternal(attribute)};
}

MeshAttribute MeshData::attributeName(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeName(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id]._name;
}

VertexFormat MeshData::attributeFormat(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeFormat(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id]._format;
}

std::size_t MeshData::attributeOffset(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeOffset(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id]._isOffsetOnly ? _attributes[id]._data.offset :
        static_cast<const char*>(_attributes[id]._data.pointer) - _vertexData.data();
}

UnsignedInt MeshData::attributeStride(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeStride(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id]._stride;
}

UnsignedShort MeshData::attributeArraySize(UnsignedInt id) const {
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

UnsignedInt MeshData::attributeFor(const MeshAttribute name, UnsignedInt id) const {
    for(std::size_t i = 0; i != _attributes.size(); ++i) {
        if(_attributes[i]._name != name) continue;
        if(id-- == 0) return i;
    }

    #ifdef CORRADE_NO_ASSERT
    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #else
    return ~UnsignedInt{};
    #endif
}

UnsignedInt MeshData::attributeId(const MeshAttribute name, UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeId(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attributeId;
}

VertexFormat MeshData::attributeFormat(MeshAttribute name, UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeFormat(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attributeFormat(attributeId);
}

std::size_t MeshData::attributeOffset(MeshAttribute name, UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeOffset(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attributeOffset(attributeId);
}

UnsignedInt MeshData::attributeStride(MeshAttribute name, UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeStride(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attributeStride(attributeId);
}

UnsignedShort MeshData::attributeArraySize(MeshAttribute name, UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeArraySize(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attributeArraySize(attributeId);
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

Containers::StridedArrayView2D<const char> MeshData::attribute(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attribute(): index" << id << "out of range for" << _attributes.size() << "attributes", nullptr);
    const MeshAttributeData& attribute = _attributes[id];
    /* Build a 2D view using information about attribute type size */
    return Containers::arrayCast<2, const char>(
        attributeDataViewInternal(attribute),
        isVertexFormatImplementationSpecific(attribute._format) ?
            attribute._stride : vertexFormatSize(attribute._format)*
                (attribute._arraySize ? attribute._arraySize : 1));
}

Containers::StridedArrayView2D<char> MeshData::mutableAttribute(UnsignedInt id) {
    CORRADE_ASSERT(_vertexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableAttribute(): vertex data not mutable", {});
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::mutableAttribute(): index" << id << "out of range for" << _attributes.size() << "attributes", nullptr);
    const MeshAttributeData& attribute = _attributes[id];
    /* Build a 2D view using information about attribute type size */
    auto out = Containers::arrayCast<2, const char>(
        attributeDataViewInternal(attribute),
        isVertexFormatImplementationSpecific(attribute._format) ?
            attribute._stride : vertexFormatSize(attribute._format)*
                (attribute._arraySize ? attribute._arraySize : 1));
    /** @todo some arrayConstCast? UGH */
    return Containers::StridedArrayView2D<char>{
        /* The view size is there only for a size assert, we're pretty sure the
           view is valid */
        {static_cast<char*>(const_cast<void*>(out.data())), ~std::size_t{}},
        out.size(), out.stride()};
}

Containers::StridedArrayView2D<const char> MeshData::attribute(MeshAttribute name, UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attribute(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attribute(attributeId);
}

Containers::StridedArrayView2D<char> MeshData::mutableAttribute(MeshAttribute name, UnsignedInt id) {
    CORRADE_ASSERT(_vertexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableAttribute(): vertex data not mutable", {});
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::mutableAttribute(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return mutableAttribute(attributeId);
}

void MeshData::indicesInto(const Containers::StridedArrayView1D<UnsignedInt> destination) const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indicesInto(): the mesh is not indexed", );
    CORRADE_ASSERT(destination.size() == indexCount(), "Trade::MeshData::indicesInto(): expected a view with" << indexCount() << "elements but got" << destination.size(), );
    auto destination1ui = Containers::arrayCast<2, UnsignedInt>(destination);

    if(_indexType == MeshIndexType::UnsignedInt)
        return Utility::copy(Containers::arrayView(reinterpret_cast<const UnsignedInt*>(_indices), _indexCount), destination);
    else if(_indexType == MeshIndexType::UnsignedShort)
        return Math::castInto(Containers::arrayCast<2, const UnsignedShort>(Containers::arrayView(reinterpret_cast<const UnsignedShort*>(_indices), _indexCount)), destination1ui);
    else if(_indexType == MeshIndexType::UnsignedByte)
        return Math::castInto(Containers::arrayCast<2, const UnsignedByte>(Containers::arrayView(reinterpret_cast<const UnsignedByte*>(_indices), _indexCount)), destination1ui);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<UnsignedInt> MeshData::indicesAsArray() const {
    /* Repeating the assert here because otherwise it would fire in
       indexCount() which may be confusing */
    CORRADE_ASSERT(isIndexed(), "Trade::MeshData::indicesAsArray(): the mesh is not indexed", {});
    Containers::Array<UnsignedInt> output{indexCount()};
    indicesInto(output);
    return output;
}

void MeshData::positions2DInto(const Containers::StridedArrayView1D<Vector2> destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttribute::Position, id);
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
    Containers::Array<Vector2> out{_vertexCount};
    positions2DInto(out, id);
    return out;
}

void MeshData::positions3DInto(const Containers::StridedArrayView1D<Vector3> destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttribute::Position, id);
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
    Containers::Array<Vector3> out{_vertexCount};
    positions3DInto(out, id);
    return out;
}

namespace {

void tangentsOrNormalsInto(const Containers::StridedArrayView1D<const void> attributeData, const Containers::StridedArrayView1D<Vector3> destination, VertexFormat format) {
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

void MeshData::tangentsInto(const Containers::StridedArrayView1D<Vector3> destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttribute::Tangent, id);
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
    Containers::Array<Vector3> out{_vertexCount};
    tangentsInto(out, id);
    return out;
}

void MeshData::bitangentSignsInto(const Containers::StridedArrayView1D<Float> destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttribute::Tangent, id);
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
        Math::unpackHalfInto(Containers::arrayCast<2, const UnsignedShort>(attributeData, 4).suffix({0, 3}), destination1f);
    else if(attribute._format == VertexFormat::Vector4bNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Byte>(attributeData, 4).suffix({0, 3}), destination1f);
    else if(attribute._format == VertexFormat::Vector4sNormalized)
        Math::unpackInto(Containers::arrayCast<2, const Short>(attributeData, 4).suffix({0, 3}), destination1f);
    else CORRADE_ASSERT_UNREACHABLE("Trade::MeshData::bitangentSignsInto(): expected four-component tangents, but got" << attribute._format, );
}

Containers::Array<Float> MeshData::bitangentSignsAsArray(const UnsignedInt id) const {
    Containers::Array<Float> out{_vertexCount};
    bitangentSignsInto(out, id);
    return out;
}

void MeshData::bitangentsInto(const Containers::StridedArrayView1D<Vector3> destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttribute::Bitangent, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::bitangentsInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::Bitangent) << "bitangent attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::bitangentsInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::bitangentsInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );
    tangentsOrNormalsInto(attributeDataViewInternal(attribute), destination, attribute._format);
}

Containers::Array<Vector3> MeshData::bitangentsAsArray(const UnsignedInt id) const {
    Containers::Array<Vector3> out{_vertexCount};
    bitangentsInto(out, id);
    return out;
}

void MeshData::normalsInto(const Containers::StridedArrayView1D<Vector3> destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttribute::Normal, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::normalsInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::Normal) << "normal attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::normalsInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(attribute._format),
        "Trade::MeshData::normalsInto(): can't extract data out of an implementation-specific vertex format" << reinterpret_cast<void*>(vertexFormatUnwrap(attribute._format)), );
    tangentsOrNormalsInto(attributeDataViewInternal(attribute), destination, attribute._format);
}

Containers::Array<Vector3> MeshData::normalsAsArray(const UnsignedInt id) const {
    Containers::Array<Vector3> out{_vertexCount};
    normalsInto(out, id);
    return out;
}

void MeshData::textureCoordinates2DInto(const Containers::StridedArrayView1D<Vector2> destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttribute::TextureCoordinates, id);
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
    Containers::Array<Vector2> out{_vertexCount};
    textureCoordinates2DInto(out, id);
    return out;
}

void MeshData::colorsInto(const Containers::StridedArrayView1D<Color4> destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttribute::Color, id);
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
    Containers::Array<Color4> out{_vertexCount};
    colorsInto(out, id);
    return out;
}

void MeshData::objectIdsInto(const Containers::StridedArrayView1D<UnsignedInt> destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttribute::ObjectId, id);
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
    Containers::Array<UnsignedInt> out{_vertexCount};
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
    debug << "Trade::MeshAttribute" << Debug::nospace;

    if(UnsignedShort(value) >= UnsignedShort(MeshAttribute::Custom))
        return debug << "::Custom(" << Debug::nospace << (UnsignedShort(value) - UnsignedShort(MeshAttribute::Custom)) << Debug::nospace << ")";

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MeshAttribute::value: return debug << "::" #value;
        _c(Position)
        _c(Tangent)
        _c(Bitangent)
        _c(Normal)
        _c(TextureCoordinates)
        _c(Color)
        _c(ObjectId)
        #undef _c
        /* LCOV_EXCL_STOP */

        /* To silence compiler warning about unhandled values */
        case MeshAttribute::Custom: CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

}}
