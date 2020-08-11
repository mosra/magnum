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

#include "MaterialData.h"

#include <cstring>
#include <algorithm>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/GrowableArray.h>

#include "Magnum/Math/Vector4.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/Data.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

namespace {

using namespace Containers::Literals;

#ifndef DOXYGEN_GENERATING_OUTPUT /* It gets *really* confused */
constexpr Containers::StringView LayerMap[]{
    #define _c(name) #name ## _s,
    #include "Magnum/Trade/Implementation/materialLayerProperties.hpp"
    #undef _c
};

constexpr struct {
    Containers::StringView name;
    MaterialAttributeType type;
    UnsignedByte size;
} AttributeMap[]{
    #define _c(name, type) {#name ## _s, MaterialAttributeType::type, sizeof(type)},
    #define _ct(name, typeName, type) {#name ## _s, MaterialAttributeType::typeName, sizeof(type)},
    #define _cnt(name, string, typeName, type) {string ## _s, MaterialAttributeType::typeName, 255},
    #include "Magnum/Trade/Implementation/materialAttributeProperties.hpp"
    #undef _c
    #undef _ct
    #undef _cnt
};
#endif

}

UnsignedInt materialTextureSwizzleComponentCount(const MaterialTextureSwizzle swizzle) {
    return (UnsignedInt(swizzle) & 0xff000000u ? 1 : 0) +
           (UnsignedInt(swizzle) & 0x00ff0000u ? 1 : 0) +
           (UnsignedInt(swizzle) & 0x0000ff00u ? 1 : 0) +
           (UnsignedInt(swizzle) & 0x000000ffu ? 1 : 0);
}

std::size_t materialAttributeTypeSize(const MaterialAttributeType type) {
    switch(type) {
        case MaterialAttributeType::Bool:
            return 1;

        case MaterialAttributeType::Float:
        case MaterialAttributeType::Deg:
        case MaterialAttributeType::Rad:
        case MaterialAttributeType::UnsignedInt:
        case MaterialAttributeType::Int:
        case MaterialAttributeType::TextureSwizzle:
            return 4;

        case MaterialAttributeType::UnsignedLong:
        case MaterialAttributeType::Long:
        case MaterialAttributeType::Vector2:
        case MaterialAttributeType::Vector2ui:
        case MaterialAttributeType::Vector2i:
            return 8;

        case MaterialAttributeType::Vector3:
        case MaterialAttributeType::Vector3ui:
        case MaterialAttributeType::Vector3i:
            return 12;

        case MaterialAttributeType::Vector4:
        case MaterialAttributeType::Vector4ui:
        case MaterialAttributeType::Vector4i:
        case MaterialAttributeType::Matrix2x2:
            return 16;

        case MaterialAttributeType::Matrix2x3:
        case MaterialAttributeType::Matrix3x2:
            return 24;

        case MaterialAttributeType::Matrix2x4:
        case MaterialAttributeType::Matrix4x2:
            return 32;

        case MaterialAttributeType::Matrix3x3:
            return 36;

        case MaterialAttributeType::Matrix3x4:
        case MaterialAttributeType::Matrix4x3:
            return 48;

        case MaterialAttributeType::Pointer:
        case MaterialAttributeType::MutablePointer:
            return sizeof(void*);

        case MaterialAttributeType::String:
            CORRADE_ASSERT_UNREACHABLE("Trade::materialAttributeTypeSize(): string size is unknown", {});
    }

    CORRADE_ASSERT_UNREACHABLE("Trade::materialAttributeTypeSize(): invalid type" << type, {});
}

MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const MaterialAttributeType type, const std::size_t size, const void* const value) noexcept: _data{} /* zero-initialized */ {
    /* Special handling for strings */
    if(type == MaterialAttributeType::String) {
        const auto& stringValue = *static_cast<const Containers::StringView*>(value);
        /* The 4 extra bytes are for a null byte after both the name and value,
           a type and a string size */
        CORRADE_ASSERT(name.size() + stringValue.size() + 4 <= Implementation::MaterialAttributeDataSize,
        "Trade::MaterialAttributeData: name" << name << "and value" << stringValue << "too long, expected at most" << Implementation::MaterialAttributeDataSize - 4 << "bytes in total but got" << name.size() + stringValue.size(), );
        _data.type = MaterialAttributeType::String;
        std::memcpy(_data.data + 1, name.data(), name.size());
        std::memcpy(_data.data + Implementation::MaterialAttributeDataSize - stringValue.size() - 2, stringValue.data(), stringValue.size());
        _data.data[Implementation::MaterialAttributeDataSize - 1] = stringValue.size();
        return;
    }

    /* The 2 extra bytes are for a null byte after the name and a type */
    CORRADE_ASSERT(name.size() + size + 2 <= Implementation::MaterialAttributeDataSize,
        "Trade::MaterialAttributeData: name" << name << "too long, expected at most" << Implementation::MaterialAttributeDataSize - size - 2 << "bytes for" << type << "but got" << name.size(), );
    _data.type = type;
    std::memcpy(_data.data + 1, name.data(), name.size());
    std::memcpy(_data.data + Implementation::MaterialAttributeDataSize - size, value, size);
}

MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const MaterialAttributeType type, const void* const value) noexcept: MaterialAttributeData{name, type, type == MaterialAttributeType::String ? ~std::size_t{} : materialAttributeTypeSize(type), value} {}

MaterialAttributeData::MaterialAttributeData(const MaterialAttribute name, const MaterialAttributeType type, const void* const value) noexcept {
    CORRADE_ASSERT(UnsignedInt(name) - 1 < Containers::arraySize(AttributeMap),
        "Trade::MaterialAttributeData: invalid name" << name, );
    CORRADE_ASSERT(AttributeMap[UnsignedInt(name) - 1].type == type,
        "Trade::MaterialAttributeData: expected" << AttributeMap[UnsignedInt(name) - 1].type << "for" << name << "but got" << type, );

    _data.type = type;
    const Containers::StringView stringName = AttributeMap[UnsignedInt(name) - 1].name;
    std::memcpy(_data.data + 1, stringName.data(), stringName.size());

    /* Special handling for strings, in that case it's sot known in advance
       that we fit and has to be checked */
    if(type == MaterialAttributeType::String) {
        const auto& stringValue = *static_cast<const Containers::StringView*>(value);
        /* The 4 extra bytes are for a null byte after both the name and value,
           a type and a string size */
        CORRADE_ASSERT(stringName.size() + stringValue.size() + 4 <= Implementation::MaterialAttributeDataSize,
        "Trade::MaterialAttributeData: name" << stringName << "and value" << stringValue << "too long, expected at most" << Implementation::MaterialAttributeDataSize - 4 << "bytes in total but got" << stringName.size() + stringValue.size(), );
        _data.type = MaterialAttributeType::String;
        std::memcpy(_data.data + Implementation::MaterialAttributeDataSize - stringValue.size() - 2, stringValue.data(), stringValue.size());
        _data.data[Implementation::MaterialAttributeDataSize - 1] = stringValue.size();
        return;
    } else {
        std::memcpy(_data.data + Implementation::MaterialAttributeDataSize - AttributeMap[UnsignedInt(name) - 1].size, value, AttributeMap[UnsignedInt(name) - 1].size);
    }
}

/* Interestingly enough, [[ is an invalid syntax in C++11? */
MaterialAttributeData::MaterialAttributeData(const MaterialLayer layerName) noexcept: MaterialAttributeData{MaterialAttribute::LayerName, LayerMap[([](const MaterialLayer layerName){
    CORRADE_ASSERT(UnsignedInt(layerName) - 1 < Containers::arraySize(LayerMap), "Trade::MaterialAttributeData: invalid name" << layerName, UnsignedInt{});
    return UnsignedInt(layerName) - 1;
}(layerName))]} {}

const void* MaterialAttributeData::value() const {
    if(_data.type == MaterialAttributeType::String)
        return _data.s.nameValue + Implementation::MaterialAttributeDataSize - _data.s.size - 3;
    return _data.data + Implementation::MaterialAttributeDataSize - materialAttributeTypeSize(_data.type);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
/* On Windows (MSVC, clang-cl and MinGw) it needs an explicit export otherwise
   the symbol doesn't get exported. */
template<> MAGNUM_TRADE_EXPORT Containers::StringView MaterialAttributeData::value<Containers::StringView>() const {
    CORRADE_ASSERT(_data.type == MaterialAttributeType::String,
        "Trade::MaterialAttributeData::value():" << (_data.data + 1) << "of" << _data.type << "can't be retrieved as a string", {});
    return {_data.s.nameValue + Implementation::MaterialAttributeDataSize - _data.s.size - 3, _data.s.size, Containers::StringViewFlag::NullTerminated};
}
#endif

MaterialData::MaterialData(const MaterialTypes types, Containers::Array<MaterialAttributeData>&& attributeData, Containers::Array<UnsignedInt>&& layerData, const void* const importerState) noexcept: _data{std::move(attributeData)}, _layerOffsets{std::move(layerData)}, _types{types}, _importerState{importerState} {
    #ifndef CORRADE_NO_ASSERT
    /* Not checking what's already done in MaterialAttributeData constructor.
       Done before sorting so the index refers to the actual input index. */
    for(std::size_t i = 0; i != _data.size(); ++i)
        CORRADE_ASSERT(_data[i]._data.type != MaterialAttributeType{},
            "Trade::MaterialData: attribute" << i << "doesn't specify anything", );
    #endif

    /* Go through all layers and sort each independently */
    const UnsignedInt implicitLayerData[]{UnsignedInt(_data.size())};
    const Containers::ArrayView<const UnsignedInt> layerOffsets =
        _layerOffsets ? _layerOffsets : Containers::arrayView(implicitLayerData);
    UnsignedInt begin = 0;
    for(std::size_t i = 0; i != layerOffsets.size(); ++i) {
        const UnsignedInt end = layerOffsets[i];
        CORRADE_ASSERT(begin <= end && end <= _data.size(),
            "Trade::MaterialData: invalid range (" << Debug::nospace << begin << Debug::nospace << "," << end << Debug::nospace <<") for layer" << i << "with" << _data.size() << "attributes in total", );

        /* Check if attributes are sorted already. If not, sort them. Can't
           just call std::sort() unconditionally because if the data would be
           immutable (for example when acquiring release()d immutable data from
           another instance) it could cause crashes. (I expected not, but
           apparently ASan blows up on that.) */
        if(end - begin > 1) for(std::size_t i = begin + 1; i != end; ++i) {
            if(_data[i - 1].name() < _data[i].name()) continue;

            std::sort(_data + begin, _data + end, [](const MaterialAttributeData& a, const MaterialAttributeData& b) {
                /* Need to check here (instead of in the outer for loop) as we
                   exit the loop right after the sort and thus duplicates that
                   occur after the first non-sorted pair wouldn't get detected. */
                CORRADE_ASSERT(a.name() != b.name(),
                    "Trade::MaterialData: duplicate attribute" << a.name(), false);
                return a.name() < b.name();
            });
            break;
        }

        begin = end;
    }
}

MaterialData::MaterialData(const MaterialTypes types, const std::initializer_list<MaterialAttributeData> attributeData, const std::initializer_list<UnsignedInt> layerData, const void* const importerState): MaterialData{types, Implementation::initializerListToArrayWithDefaultDeleter(attributeData), Implementation::initializerListToArrayWithDefaultDeleter(layerData), importerState} {}

MaterialData::MaterialData(const MaterialTypes types, DataFlags, const Containers::ArrayView<const MaterialAttributeData> attributeData, DataFlags, Containers::ArrayView<const UnsignedInt> layerData, const void* const importerState) noexcept: _data{Containers::Array<MaterialAttributeData>{const_cast<MaterialAttributeData*>(attributeData.data()), attributeData.size(), reinterpret_cast<void(*)(MaterialAttributeData*, std::size_t)>(Implementation::nonOwnedArrayDeleter)}}, _layerOffsets{Containers::Array<UnsignedInt>{const_cast<UnsignedInt*>(layerData.data()), layerData.size(), reinterpret_cast<void(*)(UnsignedInt*, std::size_t)>(Implementation::nonOwnedArrayDeleter)}}, _types{types}, _importerState{importerState} {
    #ifndef CORRADE_NO_ASSERT
    /* Not checking what's already done in MaterialAttributeData constructor */
    for(std::size_t i = 0; i != _data.size(); ++i)
        CORRADE_ASSERT(_data[i]._data.type != MaterialAttributeType{},
            "Trade::MaterialData: attribute" << i << "doesn't specify anything", );

    /* Go through all layers and verify that each is independently sorted */
    const UnsignedInt implicitLayerData[]{UnsignedInt(_data.size())};
    const Containers::ArrayView<const UnsignedInt> layerOffsets =
        _layerOffsets ? _layerOffsets : Containers::arrayView(implicitLayerData);
    UnsignedInt begin = 0;
    for(std::size_t i = 0; i != layerOffsets.size(); ++i) {
        const UnsignedInt end = layerOffsets[i];
        CORRADE_ASSERT(begin <= end && end <= _data.size(),
            "Trade::MaterialData: invalid range (" << Debug::nospace << begin << Debug::nospace << "," << end << Debug::nospace <<") for layer" << i << "with" << _data.size() << "attributes in total", );

        if(end - begin > 1) for(std::size_t i = begin + 1; i != end; ++i) {
            CORRADE_ASSERT(_data[i - 1].name() != _data[i].name(),
                "Trade::MaterialData: duplicate attribute" << _data[i].name(), );
            CORRADE_ASSERT(_data[i - 1].name() < _data[i].name(),
                "Trade::MaterialData:" << _data[i].name() << "has to be sorted before" << _data[i - 1].name() << "if passing non-owned data", );
        }

        begin = end;
    }
    #endif
}

MaterialData::MaterialData(MaterialData&&) noexcept = default;

MaterialData::~MaterialData() = default;

MaterialData& MaterialData::operator=(MaterialData&&) noexcept = default;

Containers::StringView MaterialData::layerString(const MaterialLayer name) {
    #ifndef CORRADE_NO_ASSERT
    if(UnsignedInt(name) - 1 >= Containers::arraySize(LayerMap))
        return nullptr;
    #endif
    return LayerMap[UnsignedInt(name) - 1];
}

Containers::StringView MaterialData::attributeString(const MaterialAttribute name) {
    #ifndef CORRADE_NO_ASSERT
    if(UnsignedInt(name) - 1 >= Containers::arraySize(AttributeMap))
        return nullptr;
    #endif
    return AttributeMap[UnsignedInt(name) - 1].name;
}

UnsignedInt MaterialData::layerFor(const Containers::StringView layer) const {
    for(std::size_t i = 1; i < _layerOffsets.size(); ++i) {
        if(_layerOffsets[i] > _layerOffsets[i - 1] &&
           _data[_layerOffsets[i - 1]].name() == "$LayerName"_s &&
           _data[_layerOffsets[i - 1]].value<Containers::StringView>() == layer)
            return i;
    }
    return ~UnsignedInt{};
}

bool MaterialData::hasLayer(const Containers::StringView layer) const {
    return layerFor(layer) != ~UnsignedInt{};
}

bool MaterialData::hasLayer(const MaterialLayer layer) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::hasLayer(): invalid name" << layer, {});
    return hasLayer(string);
}

UnsignedInt MaterialData::layerId(const Containers::StringView layer) const {
    const UnsignedInt id = layerFor(layer);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::layerId(): layer" << layer << "not found", {});
    return id;
}

UnsignedInt MaterialData::layerId(const MaterialLayer layer) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::layerId(): invalid name" << layer, {});
    return layerId(string);
}

Containers::StringView MaterialData::layerName(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerName(): index" << layer << "out of range for" << layerCount() << "layers", {});
    /* Deliberately ignore this attribute in the base material */
    if(layer && _layerOffsets[layer] > _layerOffsets[layer - 1] && _data[_layerOffsets[layer - 1]].name() == "$LayerName")
        return _data[_layerOffsets[layer - 1]].value<Containers::StringView>();
    return {};
}

Float MaterialData::layerFactor(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerFactor(): index" << layer << "out of range for" << layerCount() << "layers", {});
    return attributeOr(layer, MaterialAttribute::LayerFactor, 1.0f);
}

Float MaterialData::layerFactor(const Containers::StringView layer) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactor(): layer" << layer << "not found", {});
    return layerFactor(layerId);
}

Float MaterialData::layerFactor(const MaterialLayer layer) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::layerFactor(): invalid name" << layer, {});
    return layerFactor(string);
}

UnsignedInt MaterialData::layerFactorTexture(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerFactorTexture(): index" << layer << "out of range for" << layerCount() << "layers", {});
    return attribute<UnsignedInt>(layer, MaterialAttribute::LayerFactorTexture);
}

UnsignedInt MaterialData::layerFactorTexture(const Containers::StringView layer) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactorTexture(): layer" << layer << "not found", {});
    /* Not delegating into layerFactorTexture() in order to have layer name
       caught in the assert */
    return attribute<UnsignedInt>(layer, MaterialAttribute::LayerFactorTexture);
}

UnsignedInt MaterialData::layerFactorTexture(const MaterialLayer layer) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::layerFactorTexture(): invalid name" << layer, {});
    return layerFactorTexture(string);
}

MaterialTextureSwizzle MaterialData::layerFactorTextureSwizzle(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerFactorTextureSwizzle(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(hasAttribute(layer, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureSwizzle(): layer" << layer << "doesn't have a factor texture", {});
    return attributeOr(layer, MaterialAttribute::LayerFactorTextureSwizzle, MaterialTextureSwizzle::R);
}

MaterialTextureSwizzle MaterialData::layerFactorTextureSwizzle(const Containers::StringView layer) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactorTextureSwizzle(): layer" << layer << "not found", {});
    CORRADE_ASSERT(hasAttribute(layerId, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureSwizzle(): layer" << layer << "doesn't have a factor texture", {});
    /* Not delegating into layerFactorTextureSwizzle() because we have a
       different variant of the assert here */
    return attributeOr(layer, MaterialAttribute::LayerFactorTextureSwizzle, MaterialTextureSwizzle::R);
}

MaterialTextureSwizzle MaterialData::layerFactorTextureSwizzle(const MaterialLayer layer) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::layerFactorTextureSwizzle(): invalid name" << layer, {});
    return layerFactorTextureSwizzle(string);
}

Matrix3 MaterialData::layerFactorTextureMatrix(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerFactorTextureMatrix(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(hasAttribute(layer, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureMatrix(): layer" << layer << "doesn't have a factor texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(layer, MaterialAttribute::LayerFactorTextureMatrix))
        return *value;
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(layer, MaterialAttribute::TextureMatrix))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureMatrix, Matrix3{});
}

Matrix3 MaterialData::layerFactorTextureMatrix(const Containers::StringView layer) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactorTextureMatrix(): layer" << layer << "not found", {});
    CORRADE_ASSERT(hasAttribute(layerId, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureMatrix(): layer" << layer << "doesn't have a factor texture", {});
    /* Not delegating into layerFactorTextureMatrix() because we have a
       different variant of the assert here */
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(layerId, MaterialAttribute::LayerFactorTextureMatrix))
        return *value;
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(layerId, MaterialAttribute::TextureMatrix))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureMatrix, Matrix3{});
}

Matrix3 MaterialData::layerFactorTextureMatrix(const MaterialLayer layer) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::layerFactorTextureMatrix(): invalid name" << layer, {});
    return layerFactorTextureMatrix(string);
}

UnsignedInt MaterialData::layerFactorTextureCoordinates(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerFactorTextureCoordinates(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(hasAttribute(layer, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer" << layer << "doesn't have a factor texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(layer, MaterialAttribute::LayerFactorTextureCoordinates))
        return *value;
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(layer, MaterialAttribute::TextureCoordinates))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt MaterialData::layerFactorTextureCoordinates(const Containers::StringView layer) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer" << layer << "not found", {});
    CORRADE_ASSERT(hasAttribute(layerId, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer" << layer << "doesn't have a factor texture", {});
    /* Not delegating into layerFactorTextureCoordinates() because we have a
       different variant of the assert here */
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(layerId, MaterialAttribute::LayerFactorTextureCoordinates))
        return *value;
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(layerId, MaterialAttribute::TextureCoordinates))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt MaterialData::layerFactorTextureCoordinates(const MaterialLayer layer) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::layerFactorTextureCoordinates(): invalid name" << layer, {});
    return layerFactorTextureCoordinates(string);
}

UnsignedInt MaterialData::attributeCount(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeCount(): index" << layer << "out of range for" << layerCount() << "layers", {});
    if(!_layerOffsets) return _data.size();
    if(!layer) return _layerOffsets[0];
    return _layerOffsets[layer] - _layerOffsets[layer - 1];
}

UnsignedInt MaterialData::attributeCount(const Containers::StringView layer) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeCount(): layer" << layer << "not found", {});
    return attributeCount(layerId);
}

UnsignedInt MaterialData::attributeCount(const MaterialLayer layer) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeCount(): invalid name" << layer, {});
    return attributeCount(string);
}

UnsignedInt MaterialData::attributeFor(const UnsignedInt layer, const Containers::StringView name) const {
    const MaterialAttributeData* begin = _data.begin() +
        (layer && _layerOffsets ? _layerOffsets[layer - 1] : 0);
    const MaterialAttributeData* end =
        (_layerOffsets ? _data.begin() + _layerOffsets[layer] : _data.end());
    const MaterialAttributeData* const found = std::lower_bound(begin, end, name, [](const MaterialAttributeData& a, const Containers::StringView& b) {
        return a.name() < b;
    });
    if(found == end || found->name() != name) return ~UnsignedInt{};
    return found - begin;
}

bool MaterialData::hasAttribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::hasAttribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    return attributeFor(layer, name) != ~UnsignedInt{};
}

bool MaterialData::hasAttribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::hasAttribute(): invalid name" << name, {});
    return hasAttribute(layer, string);
}

bool MaterialData::hasAttribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::hasAttribute(): layer" << layer << "not found", {});
    return hasAttribute(layerId, name);
}

bool MaterialData::hasAttribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::hasAttribute(): invalid name" << name, {});
    return hasAttribute(layer, string);
}

bool MaterialData::hasAttribute(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::hasAttribute(): invalid name" << layer, {});
    return hasAttribute(string, name);
}

bool MaterialData::hasAttribute(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::hasAttribute(): invalid name" << layer, {});
    return hasAttribute(string, name);
}

UnsignedInt MaterialData::attributeId(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeId(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = attributeFor(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attributeId(): attribute" << name << "not found in layer" << layer, {});
    return id;
}

UnsignedInt MaterialData::attributeId(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeId(): invalid name" << name, {});
    return attributeId(layer, string);
}

UnsignedInt MaterialData::attributeId(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeId(): layer" << layer << "not found", {});
    const UnsignedInt id = attributeFor(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attributeId(): attribute" << name << "not found in layer" << layer, {});
    return id;
}

UnsignedInt MaterialData::attributeId(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeId(): invalid name" << name, {});
    return attributeId(layer, string);
}

UnsignedInt MaterialData::attributeId(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeId(): invalid name" << layer, {});
    return attributeId(string, name);
}

UnsignedInt MaterialData::attributeId(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeId(): invalid name" << layer, {});
    return attributeId(string, name);
}

Containers::StringView MaterialData::attributeName(const UnsignedInt layer, const UnsignedInt id) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeName(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attributeName(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layer) + id]._data.data + 1;
}

Containers::StringView MaterialData::attributeName(const Containers::StringView layer, const UnsignedInt id) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeName(): layer" << layer << "not found", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attributeName(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layerId) + id]._data.data + 1;
}

Containers::StringView MaterialData::attributeName(const MaterialLayer layer, const UnsignedInt id) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeName(): invalid name" << layer, {});
    return attributeName(string, id);
}

MaterialAttributeType MaterialData::attributeType(const UnsignedInt layer, const UnsignedInt id) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeType(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attributeType(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layer) + id]._data.type;
}

MaterialAttributeType MaterialData::attributeType(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeType(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = attributeFor(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attributeType(): attribute" << name << "not found in layer" << layer, {});
    return _data[layerOffset(layer) + id]._data.type;
}

MaterialAttributeType MaterialData::attributeType(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeType(): invalid name" << name, {});
    return attributeType(layer, string);
}

MaterialAttributeType MaterialData::attributeType(const Containers::StringView layer, const UnsignedInt id) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeType(): layer" << layer << "not found", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attributeType(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layerId) + id]._data.type;
}

MaterialAttributeType MaterialData::attributeType(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeType(): layer" << layer << "not found", {});
    const UnsignedInt id = attributeFor(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attributeType(): attribute" << name << "not found in layer" << layer, {});
    return _data[layerOffset(layerId) + id]._data.type;
}

MaterialAttributeType MaterialData::attributeType(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeType(): invalid name" << name, {});
    return attributeType(layer, string);
}

MaterialAttributeType MaterialData::attributeType(const MaterialLayer layer, const UnsignedInt id) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeType(): invalid name" << layer, {});
    return attributeType(string, id);
}

MaterialAttributeType MaterialData::attributeType(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeType(): invalid name" << layer, {});
    return attributeType(string, name);
}

MaterialAttributeType MaterialData::attributeType(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeType(): invalid name" << layer, {});
    return attributeType(string, name);
}

const void* MaterialData::attribute(const UnsignedInt layer, const UnsignedInt id) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layer) + id].value();
}

const void* MaterialData::attribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = attributeFor(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found in layer" << layer, {});
    return _data[layerOffset(layer) + id].value();
}

const void* MaterialData::attribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute(layer, string);
}

const void* MaterialData::attribute(const Containers::StringView layer, const UnsignedInt id) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): layer" << layer << "not found", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layerId) + id].value();
}

const void* MaterialData::attribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): layer" << layer << "not found", {});
    const UnsignedInt id = attributeFor(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found in layer" << layer, {});
    return _data[layerOffset(layerId) + id].value();
}

const void* MaterialData::attribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute(layer, string);
}

const void* MaterialData::attribute(const MaterialLayer layer, const UnsignedInt id) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute(string, id);
}

const void* MaterialData::attribute(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute(string, name);
}

const void* MaterialData::attribute(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute(string, name);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
/* On Windows (MSVC, clang-cl and MinGw) it needs an explicit export otherwise
   the symbol doesn't get exported. */
template<> MAGNUM_TRADE_EXPORT Containers::StringView MaterialData::attribute<Containers::StringView>(const UnsignedInt layer, const UnsignedInt id) const {
    /* Can't delegate to attribute() returning const void* because that doesn't
       include the size */
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    const Trade::MaterialAttributeData& data = _data[layerOffset(layer) + id];
    CORRADE_ASSERT(data._data.type == MaterialAttributeType::String,
        "Trade::MaterialData::attribute():" << (data._data.data + 1) << "of" << data._data.type << "can't be retrieved as a string", {});
    return {data._data.s.nameValue + Implementation::MaterialAttributeDataSize - data._data.s.size - 3, data._data.s.size, Containers::StringViewFlag::NullTerminated};
}

const void* MaterialData::tryAttribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::tryAttribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = attributeFor(layer, name);
    if(id == ~UnsignedInt{}) return nullptr;
    return _data[layerOffset(layer) + id].value();
}

const void* MaterialData::tryAttribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::tryAttribute(): invalid name" << name, {});
    return tryAttribute(layer, string);
}
#endif

const void* MaterialData::tryAttribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::tryAttribute(): layer" << layer << "not found", {});
    const UnsignedInt id = attributeFor(layerId, name);
    if(id == ~UnsignedInt{}) return nullptr;
    return _data[layerOffset(layerId) + id].value();
}

const void* MaterialData::tryAttribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::tryAttribute(): invalid name" << name, {});
    return tryAttribute(layer, string);
}

const void* MaterialData::tryAttribute(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::tryAttribute(): invalid name" << layer, {});
    return tryAttribute(string, name);
}

const void* MaterialData::tryAttribute(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::tryAttribute(): invalid name" << layer, {});
    return tryAttribute(string, name);
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
MaterialData::Flags MaterialData::flags() const {
    Flags flags;
    if(isDoubleSided())
        flags |= Flag::DoubleSided;
    return flags;
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

bool MaterialData::isDoubleSided() const {
    return attributeOr(MaterialAttribute::DoubleSided, false);
}

MaterialAlphaMode MaterialData::alphaMode() const {
    if(attributeOr(MaterialAttribute::AlphaBlend, false))
        return MaterialAlphaMode::Blend;
    if(hasAttribute(MaterialAttribute::AlphaMask))
        return MaterialAlphaMode::Mask;
    return MaterialAlphaMode::Opaque;
}

Float MaterialData::alphaMask() const {
    return attributeOr(MaterialAttribute::AlphaMask, 0.5f);
}

Containers::Array<UnsignedInt> MaterialData::releaseLayerData() {
    return std::move(_layerOffsets);
}

Containers::Array<MaterialAttributeData> MaterialData::releaseAttributeData() {
    return std::move(_data);
}

Debug& operator<<(Debug& debug, const MaterialLayer value) {
    debug << "Trade::MaterialLayer" << Debug::nospace;

    if(UnsignedInt(value) - 1 >= Containers::arraySize(LayerMap))
        return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";

    return debug << "::" << Debug::nospace << LayerMap[UnsignedInt(value) - 1];
}

Debug& operator<<(Debug& debug, const MaterialAttribute value) {
    debug << "Trade::MaterialAttribute" << Debug::nospace;

    if(UnsignedInt(value) - 1 >= Containers::arraySize(AttributeMap))
        return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";

    /* LayerName is prefixed with $, drop that */
    Containers::StringView string = AttributeMap[UnsignedInt(value) - 1].name;
    if(string[0] == '$') string = string.suffix(1);

    return debug << "::" << Debug::nospace << string;
}

Debug& operator<<(Debug& debug, const MaterialTextureSwizzle value) {
    /* The swizzle is encoded as a fourCC, so just print the numerical value as
       a char. Worst case this will print nothing or four garbage letters.
       Sorry in that case. GCC 4.8 doesn't understand just {}, wants to have
       a full MaterialTextureSwizzle{} here. */
    MaterialTextureSwizzle values[]{value, MaterialTextureSwizzle{}};
    return debug << "Trade::MaterialTextureSwizzle::" << Debug::nospace << reinterpret_cast<const char*>(values);
}

Debug& operator<<(Debug& debug, const MaterialAttributeType value) {
    debug << "Trade::MaterialAttributeType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MaterialAttributeType::value: return debug << "::" #value;
        _c(Bool)
        _c(Float)
        _c(Deg)
        _c(Rad)
        _c(UnsignedInt)
        _c(Int)
        _c(UnsignedLong)
        _c(Long)
        _c(Vector2)
        _c(Vector2ui)
        _c(Vector2i)
        _c(Vector3)
        _c(Vector3ui)
        _c(Vector3i)
        _c(Vector4)
        _c(Vector4ui)
        _c(Vector4i)
        _c(Matrix2x2)
        _c(Matrix2x3)
        _c(Matrix2x4)
        _c(Matrix3x2)
        _c(Matrix3x3)
        _c(Matrix3x4)
        _c(Matrix4x2)
        _c(Matrix4x3)
        _c(Pointer)
        _c(MutablePointer)
        _c(String)
        _c(TextureSwizzle)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MaterialType value) {
    debug << "Trade::MaterialType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MaterialType::value: return debug << "::" #value;
        _c(Flat)
        _c(Phong)
        _c(PbrMetallicRoughness)
        _c(PbrSpecularGlossiness)
        _c(PbrClearCoat)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MaterialTypes value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::MaterialTypes{}", {
        MaterialType::Flat,
        MaterialType::Phong,
        MaterialType::PbrMetallicRoughness,
        MaterialType::PbrSpecularGlossiness,
        MaterialType::PbrClearCoat
    });
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Debug& operator<<(Debug& debug, const MaterialData::Flag value) {
    debug << "Trade::MaterialData::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MaterialData::Flag::value: return debug << "::" #value;
        _c(DoubleSided)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MaterialData::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::MaterialData::Flags{}", {
        MaterialData::Flag::DoubleSided
    });
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

Debug& operator<<(Debug& debug, const MaterialAlphaMode value) {
    debug << "Trade::MaterialAlphaMode" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MaterialAlphaMode::value: return debug << "::" #value;
        _c(Opaque)
        _c(Mask)
        _c(Blend)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

}}
