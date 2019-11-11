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

#include "MeshData.h"

#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

MeshIndexData::MeshIndexData(const MeshIndexType type, const Containers::ArrayView<const void> data) noexcept: type{type}, data{reinterpret_cast<const Containers::ArrayView<const char>&>(data)} {
    CORRADE_ASSERT(!data.empty(),
        "Trade::MeshIndexData: index array can't be empty, create a non-indexed mesh instead", );
    CORRADE_ASSERT(data.size()%meshIndexTypeSize(type) == 0,
        "Trade::MeshIndexData: view size" << data.size() << "does not correspond to" << type, );
}

MeshAttributeData::MeshAttributeData(const MeshAttribute name, const VertexFormat format, const Containers::StridedArrayView1D<const char>& data) noexcept: name{name}, format{format}, data{data} {
    /** @todo support zero / negative stride? would be hard to transfer to GL */
    CORRADE_ASSERT(data.empty() || std::ptrdiff_t(vertexFormatSize(format)) <= data.stride(),
        "Trade::MeshAttributeData: view stride" << data.stride() << "is not large enough to contain" << format, );
    CORRADE_ASSERT(
        (name == MeshAttribute::Position &&
            (format == VertexFormat::Vector2 ||
             format == VertexFormat::Vector3)) ||
        (name == MeshAttribute::Normal &&
            (format == VertexFormat::Vector3)) ||
        (name == MeshAttribute::Color &&
            (format == VertexFormat::Vector3 ||
             format == VertexFormat::Vector4)) ||
        (name == MeshAttribute::TextureCoordinates &&
            (format == VertexFormat::Vector2)) ||
        isMeshAttributeCustom(name) /* can be any format */,
        "Trade::MeshAttributeData:" << format << "is not a valid format for" << name, );
}

Containers::Array<MeshAttributeData> meshAttributeDataNonOwningArray(const Containers::ArrayView<const MeshAttributeData> view) {
    /* Ugly, eh? */
    return Containers::Array<Trade::MeshAttributeData>{const_cast<Trade::MeshAttributeData*>(view.data()), view.size(), reinterpret_cast<void(*)(Trade::MeshAttributeData*, std::size_t)>(Trade::Implementation::nonOwnedArrayDeleter)};
}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: _indexType{indices.type}, _primitive{primitive}, _indexDataFlags{DataFlag::Owned|DataFlag::Mutable}, _vertexDataFlags{DataFlag::Owned|DataFlag::Mutable}, _importerState{importerState}, _indexData{std::move(indexData)}, _vertexData{std::move(vertexData)}, _attributes{std::move(attributes)}, _indices{indices.data} {
    /* Save vertex count. It's a strided array view, so the size is not
       depending on type. */
    if(_attributes.empty()) {
        CORRADE_ASSERT(indices.type != MeshIndexType{},
            "Trade::MeshData: indices are expected to be valid if there are no attributes and vertex count isn't passed explicitly", );
        /** @todo some better value? attributeless indexed with defined vertex count? */
        _vertexCount = 0;
    } else _vertexCount = _attributes[0].data.size();

    CORRADE_ASSERT(!_indices.empty() || !_indexData,
        "Trade::MeshData: indexData passed for a non-indexed mesh", );
    CORRADE_ASSERT(_indices.empty() || (_indices.begin() >= _indexData.begin() && _indices.end() <= _indexData.end()),
        "Trade::MeshData: indices [" << Debug::nospace << static_cast<const void*>(_indices.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_indices.end()) << Debug::nospace << "] are not contained in passed indexData array [" << Debug::nospace << static_cast<const void*>(_indexData.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_indexData.end()) << Debug::nospace << "]", );
    CORRADE_ASSERT(!_attributes.empty() || !_vertexData,
        "Trade::MeshData: vertexData passed for an attribute-less mesh", );
    CORRADE_ASSERT(_vertexCount || !_vertexData,
        "Trade::MeshData: vertexData passed for a mesh with zero vertices", );

    #ifndef CORRADE_NO_ASSERT
    /* Not checking what's already checked in MeshIndexData / MeshAttributeData
       constructors */
    for(std::size_t i = 0; i != _attributes.size(); ++i) {
        const MeshAttributeData& attribute = _attributes[i];
        CORRADE_ASSERT(attribute.data.size() == _vertexCount,
            "Trade::MeshData: attribute" << i << "has" << attribute.data.size() << "vertices but" << _vertexCount << "expected", );
        CORRADE_ASSERT(attribute.data.empty() || (&attribute.data.front() >= _vertexData.begin() && &attribute.data.back() + vertexFormatSize(attribute.format) <= _vertexData.end()),
            "Trade::MeshData: attribute" << i << "[" << Debug::nospace << static_cast<const void*>(&attribute.data.front()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(&attribute.data.back() + vertexFormatSize(attribute.format)) << Debug::nospace << "] is not contained in passed vertexData array [" << Debug::nospace << static_cast<const void*>(_vertexData.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_vertexData.end()) << Debug::nospace << "]", );
    }
    #endif
}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, std::move(indexData), indices, std::move(vertexData), Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, const Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(indexData.data())), indexData.size(), Implementation::nonOwnedArrayDeleter}, indices, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(vertexData.data())), vertexData.size(), Implementation::nonOwnedArrayDeleter}, std::move(attributes), importerState} {
    CORRADE_ASSERT(!(indexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned index data but" << indexDataFlags, );
    CORRADE_ASSERT(!(vertexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned vertex data but" << vertexDataFlags, );
    _indexDataFlags = indexDataFlags;
    _vertexDataFlags = vertexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, const Containers::ArrayView<const void> vertexData, const std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, indexDataFlags, indexData, indices, vertexDataFlags, vertexData, Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(indexData.data())), indexData.size(), Implementation::nonOwnedArrayDeleter}, indices, std::move(vertexData), std::move(attributes), importerState} {
    CORRADE_ASSERT(!(indexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned index data but" << indexDataFlags, );
    _indexDataFlags = indexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, const std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, indexDataFlags, indexData, indices, std::move(vertexData), Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: MeshData{primitive, std::move(indexData), indices, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(vertexData.data())), vertexData.size(), Implementation::nonOwnedArrayDeleter}, std::move(attributes), importerState} {
    CORRADE_ASSERT(!(vertexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned vertex data but" << vertexDataFlags, );
    _vertexDataFlags = vertexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, const Containers::ArrayView<const void> vertexData, const std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, std::move(indexData), indices, vertexDataFlags, vertexData, Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: MeshData{primitive, {}, MeshIndexData{}, std::move(vertexData), std::move(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& vertexData, const std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, std::move(vertexData), Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(vertexData.data())), vertexData.size(), Implementation::nonOwnedArrayDeleter}, std::move(attributes), importerState} {
    CORRADE_ASSERT(!(vertexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned vertex data but" << vertexDataFlags, );
    _vertexDataFlags = vertexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, vertexDataFlags, vertexData, Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, const void* const importerState) noexcept: MeshData{primitive, std::move(indexData), indices, {}, {}, importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(indexData.data())), indexData.size(), Implementation::nonOwnedArrayDeleter}, indices, importerState} {
    CORRADE_ASSERT(!(indexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned index data but" << indexDataFlags, );
    _indexDataFlags = indexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const UnsignedInt vertexCount, const void* const importerState) noexcept: _vertexCount{vertexCount}, _indexType{}, _primitive{primitive}, _indexDataFlags{DataFlag::Owned|DataFlag::Mutable}, _vertexDataFlags{DataFlag::Owned|DataFlag::Mutable}, _importerState{importerState} {}

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
    return _indices.size()/meshIndexTypeSize(_indexType);
}

MeshIndexType MeshData::indexType() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indexType(): the mesh is not indexed", {});
    return _indexType;
}

MeshAttribute MeshData::attributeName(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeName(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id].name;
}

VertexFormat MeshData::attributeFormat(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeFormat(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id].format;
}

std::size_t MeshData::attributeOffset(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeOffset(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return static_cast<const char*>(_attributes[id].data.data()) - _vertexData.data();
}

UnsignedInt MeshData::attributeStride(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeStride(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id].data.stride();
}

UnsignedInt MeshData::attributeCount(const MeshAttribute name) const {
    UnsignedInt count = 0;
    for(const MeshAttributeData& attribute: _attributes)
        if(attribute.name == name) ++count;
    return count;
}

UnsignedInt MeshData::attributeFor(const MeshAttribute name, UnsignedInt id) const {
    for(std::size_t i = 0; i != _attributes.size(); ++i) {
        if(_attributes[i].name != name) continue;
        if(id-- == 0) return i;
    }

    #ifdef CORRADE_NO_ASSERT
    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #else
    return ~UnsignedInt{};
    #endif
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

namespace {

template<class T> void convertIndices(const Containers::ArrayView<const char> data, const Containers::ArrayView<UnsignedInt> destination) {
    const auto input = Containers::arrayCast<const T>(data);
    for(std::size_t i = 0; i != input.size(); ++i) destination[i] = input[i];
}

}

void MeshData::indicesInto(const Containers::ArrayView<UnsignedInt> destination) const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indicesInto(): the mesh is not indexed", );
    CORRADE_ASSERT(destination.size() == indexCount(), "Trade::MeshData::indicesInto(): expected a view with" << indexCount() << "elements but got" << destination.size(), );

    switch(_indexType) {
        case MeshIndexType::UnsignedByte: return convertIndices<UnsignedByte>(_indices, destination);
        case MeshIndexType::UnsignedShort: return convertIndices<UnsignedShort>(_indices, destination);
        case MeshIndexType::UnsignedInt: return convertIndices<UnsignedInt>(_indices, destination);
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
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

    /* Copy 2D positions as-is, for 3D positions ignore Z */
    if(attribute.format == VertexFormat::Vector2 ||
       attribute.format == VertexFormat::Vector3)
        Utility::copy(Containers::arrayCast<const Vector2>(attribute.data), destination);
    else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
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

    /* For 2D positions copy the XY part to the first two components and then
       fill the Z with a single value */
    if(attribute.format == VertexFormat::Vector2) {
        Utility::copy(Containers::arrayCast<const Vector2>(attribute.data),
                      Containers::arrayCast<Vector2>(destination));
        constexpr Float z[1]{0.0f};
        Utility::copy(
            Containers::stridedArrayView(z).broadcasted<0>(_vertexCount),
            Containers::arrayCast<2, Float>(destination).transposed<0, 1>()[2]);
    /* Copy 3D positions as-is */
    } else if(attribute.format == VertexFormat::Vector3) {
        Utility::copy(Containers::arrayCast<const Vector3>(attribute.data), destination);
    } else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<Vector3> MeshData::positions3DAsArray(const UnsignedInt id) const {
    Containers::Array<Vector3> out{_vertexCount};
    positions3DInto(out, id);
    return out;
}

void MeshData::normalsInto(const Containers::StridedArrayView1D<Vector3> destination, const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttribute::Normal, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::normalsInto(): index" << id << "out of range for" << attributeCount(MeshAttribute::Normal) << "normal attributes", );
    CORRADE_ASSERT(destination.size() == _vertexCount, "Trade::MeshData::normalsInto(): expected a view with" << _vertexCount << "elements but got" << destination.size(), );
    const MeshAttributeData& attribute = _attributes[attributeId];

    if(attribute.format == VertexFormat::Vector3)
        Utility::copy(Containers::arrayCast<const Vector3>(attribute.data), destination);
    else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
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

    if(attribute.format == VertexFormat::Vector2)
        Utility::copy(Containers::arrayCast<const Vector2>(attribute.data), destination);
    else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
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

    /* For three-component colors copy the RGB part to the first three
       components and then fill the alpha with a single value */
    if(attribute.format == VertexFormat::Vector3) {
        Utility::copy(Containers::arrayCast<const Vector3>(attribute.data),
                      Containers::arrayCast<Vector3>(destination));
        constexpr Float alpha[1]{1.0f};
        Utility::copy(
            Containers::stridedArrayView(alpha).broadcasted<0>(_vertexCount),
            Containers::arrayCast<2, Float>(destination).transposed<0, 1>()[3]);
    /* Copy four-component colors as-is */
    } else if(attribute.format == VertexFormat::Vector4) {
        Utility::copy(Containers::arrayCast<const Vector4>(attribute.data),
                      Containers::arrayCast<Vector4>(destination));
    } else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<Color4> MeshData::colorsAsArray(const UnsignedInt id) const {
    Containers::Array<Color4> out{_vertexCount};
    colorsInto(out, id);
    return out;
}

Containers::Array<char> MeshData::releaseIndexData() {
    _indexType = MeshIndexType{}; /* so isIndexed() returns false */
    _indices = nullptr;
    return std::move(_indexData);
}

Containers::Array<char> MeshData::releaseVertexData() {
    _attributes = nullptr;
    return std::move(_vertexData);
}

Debug& operator<<(Debug& debug, const MeshAttribute value) {
    debug << "Trade::MeshAttribute" << Debug::nospace;

    if(UnsignedShort(value) >= UnsignedShort(MeshAttribute::Custom))
        return debug << "::Custom(" << Debug::nospace << (UnsignedShort(value) - UnsignedShort(MeshAttribute::Custom)) << Debug::nospace << ")";

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MeshAttribute::value: return debug << "::" << Debug::nospace << #value;
        _c(Position)
        _c(Normal)
        _c(TextureCoordinates)
        _c(Color)
        #undef _c
        /* LCOV_EXCL_STOP */

        /* To silence compiler warning about unhandled values */
        case MeshAttribute::Custom: CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

}}
