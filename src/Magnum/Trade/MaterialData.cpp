/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "MaterialData.h"

#include <cstring>
#include <algorithm> /* std::sort() */
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/GrowableArray.h>

#include "Magnum/Math/Vector4.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/Data.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

namespace {

using namespace Containers::Literals;

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

}

namespace Implementation {

Containers::StringView materialLayerNameInternal(const MaterialLayer layer) {
    #ifndef CORRADE_NO_ASSERT
    if(UnsignedInt(layer) - 1 >= Containers::arraySize(LayerMap))
        return nullptr;
    #endif
    return LayerMap[UnsignedInt(layer) - 1];
}

Containers::StringView materialAttributeNameInternal(const MaterialAttribute attribute) {
    #ifndef CORRADE_NO_ASSERT
    if(UnsignedInt(attribute) - 1 >= Containers::arraySize(AttributeMap))
        return nullptr;
    #endif
    return AttributeMap[UnsignedInt(attribute) - 1].name;
}

}

Containers::StringView materialLayerName(const MaterialLayer layer) {
    CORRADE_ASSERT(UnsignedInt(layer) - 1 < Containers::arraySize(LayerMap),
        "Trade::materialLayerName(): invalid layer" << layer, {});
    return LayerMap[UnsignedInt(layer) - 1];
}

Containers::StringView materialAttributeName(const MaterialAttribute attribute) {
    CORRADE_ASSERT(UnsignedInt(attribute) - 1 < Containers::arraySize(AttributeMap),
        "Trade::materialAttributeName(): invalid attribute" << attribute, {});
    return AttributeMap[UnsignedInt(attribute) - 1].name;
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
        case MaterialAttributeType::Buffer:
            CORRADE_ASSERT_UNREACHABLE("Trade::materialAttributeTypeSize(): string and buffer size is unknown", {});
    }

    CORRADE_ASSERT_UNREACHABLE("Trade::materialAttributeTypeSize(): invalid type" << type, {});
}

MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const MaterialAttributeType type, const std::size_t size, const void* const value) noexcept: _data{} /* zero-initialized */ {
    /* It would sort before " LayerName" and that's not desirable */
    CORRADE_ASSERT(!name.isEmpty(), "Trade::MaterialAttributeData: name is not allowed to be empty", );

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

    /* Special handling for buffers. Similar to strings, except that the size
       is stored right after the null-terminated name, and the value has no
       null terminator. */
    } else if(type == MaterialAttributeType::Buffer) {
        const auto& bufferValue = *static_cast<const Containers::ArrayView<const void>*>(value);
        /* The 3 extra bytes are for a null byte after the name, a type and a
           value size */
        CORRADE_ASSERT(name.size() + bufferValue.size() + 3 <= Implementation::MaterialAttributeDataSize,
        "Trade::MaterialAttributeData: name" << name << "and a" << bufferValue.size() << Debug::nospace << "-byte value too long, expected at most" << Implementation::MaterialAttributeDataSize - 3 << "bytes in total but got" << name.size() + bufferValue.size(), );
        _data.type = MaterialAttributeType::Buffer;
        std::memcpy(_data.data + 1, name.data(), name.size());
        _data.data[name.size() + 2] = bufferValue.size();
        std::memcpy(_data.data + Implementation::MaterialAttributeDataSize - bufferValue.size(), bufferValue.data(), bufferValue.size());

    /* Fixed-size values */
    } else {
        /* The 2 extra bytes are for a null byte after the name and a type */
        CORRADE_ASSERT(name.size() + size + 2 <= Implementation::MaterialAttributeDataSize,
            "Trade::MaterialAttributeData: name" << name << "too long, expected at most" << Implementation::MaterialAttributeDataSize - size - 2 << "bytes for" << type << "but got" << name.size(), );
        _data.type = type;
        std::memcpy(_data.data + 1, name.data(), name.size());
        std::memcpy(_data.data + Implementation::MaterialAttributeDataSize - size, value, size);
    }
}

MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const MaterialAttributeType type, const void* const value) noexcept: MaterialAttributeData{name, type, type == MaterialAttributeType::String || type == MaterialAttributeType::Buffer ? ~std::size_t{} : materialAttributeTypeSize(type), value} {}

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
    if(_data.type == MaterialAttributeType::Buffer) {
        /* Using an internal StringView API because it's never slower than
           memchr() */
        /** @todo have an optimized aligned fixed-size variant? */
        const char* const nameEnd = Containers::Implementation::stringFindCharacter(_data.data, Implementation::MaterialAttributeDataSize, '\0');
        CORRADE_INTERNAL_ASSERT(nameEnd);
        const std::size_t size = *(nameEnd + 1);
        return _data.data + Implementation::MaterialAttributeDataSize - size;
    }
    return _data.data + Implementation::MaterialAttributeDataSize - materialAttributeTypeSize(_data.type);
}

/* On Windows (MSVC, clang-cl and MinGw) it needs an explicit export otherwise
   the symbol doesn't get exported. */
template<> MAGNUM_TRADE_EXPORT Containers::StringView MaterialAttributeData::value<Containers::StringView>() const {
    CORRADE_ASSERT(_data.type == MaterialAttributeType::String,
        "Trade::MaterialAttributeData::value():" << (_data.data + 1) << "of" << _data.type << "can't be retrieved as a string", {});
    return {_data.s.nameValue + Implementation::MaterialAttributeDataSize - _data.s.size - 3, _data.s.size, Containers::StringViewFlag::NullTerminated};
}
template<> MAGNUM_TRADE_EXPORT Containers::ArrayView<const void> MaterialAttributeData::value<Containers::ArrayView<const void>>() const {
    CORRADE_ASSERT(_data.type == MaterialAttributeType::Buffer,
        "Trade::MaterialAttributeData::value():" << (_data.data + 1) << "of" << _data.type << "can't be retrieved as a buffer", {});
    /* Using an internal StringView API because it's never slower than
       memchr() */
    /** @todo have an optimized aligned fixed-size variant? */
    const char* const nameEnd = Containers::Implementation::stringFindCharacter(_data.data, Implementation::MaterialAttributeDataSize, '\0');
    CORRADE_INTERNAL_ASSERT(nameEnd);
    const std::size_t size = *(nameEnd + 1);
    return {_data.data + Implementation::MaterialAttributeDataSize - size, size};
}

MaterialData::MaterialData(const MaterialTypes types, Containers::Array<MaterialAttributeData>&& attributeData, Containers::Array<UnsignedInt>&& layerData, const void* const importerState) noexcept: _data{Utility::move(attributeData)}, _layerOffsets{Utility::move(layerData)}, _types{types}, _attributeDataFlags{DataFlag::Owned|DataFlag::Mutable}, _layerDataFlags{DataFlag::Owned|DataFlag::Mutable}, _importerState{importerState} {
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
        if(end - begin > 1) for(std::size_t j = begin + 1; j != end; ++j) {
            if(_data[j - 1].name() < _data[j].name()) continue;

            std::sort(_data + begin, _data + end, [i](const MaterialAttributeData& a, const MaterialAttributeData& b) {
                /* Need to check here (instead of in the outer for loop) as we
                   exit the loop right after the sort and thus duplicates that
                   occur after the first non-sorted pair wouldn't get detected.

                   Additionally an *extra paranoid* check for shitty STL
                   implementations that might call this comparator with both
                   arguments the same (in which case the assert would fire,
                   which is undesirable). Apparently libc++ is responsible for
                   such atrocities. */
                CORRADE_ASSERT(&a == &b || a.name() != b.name(),
                    "Trade::MaterialData: duplicate attribute" << a.name() << "in layer" << i, false);
                return a.name() < b.name();
            });
            break;
        }

        begin = end;
    }

    CORRADE_ASSERT(layerOffsets.back() == _data.size(),
        "Trade::MaterialData: last layer offset" << layerOffsets.back() << "too short for" << _data.size() << "attributes in total", );
}

MaterialData::MaterialData(const MaterialTypes types, const std::initializer_list<MaterialAttributeData> attributeData, const std::initializer_list<UnsignedInt> layerData, const void* const importerState): MaterialData{types, Implementation::initializerListToArrayWithDefaultDeleter(attributeData), Implementation::initializerListToArrayWithDefaultDeleter(layerData), importerState} {}

MaterialData::MaterialData(const MaterialTypes types, const DataFlags attributeDataFlags, const Containers::ArrayView<const MaterialAttributeData> attributeData, const DataFlags layerDataFlags, Containers::ArrayView<const UnsignedInt> layerData, const void* const importerState) noexcept: _data{Containers::Array<MaterialAttributeData>{const_cast<MaterialAttributeData*>(attributeData.data()), attributeData.size(), Implementation::nonOwnedArrayDeleter}}, _layerOffsets{Containers::Array<UnsignedInt>{const_cast<UnsignedInt*>(layerData.data()), layerData.size(), Implementation::nonOwnedArrayDeleter}}, _types{types}, _importerState{importerState} {
    CORRADE_ASSERT(!(attributeDataFlags & DataFlag::Owned),
        "Trade::MaterialData: can't construct with non-owned attribute data but" << attributeDataFlags, );
    CORRADE_ASSERT(!(layerDataFlags & DataFlag::Owned),
        "Trade::MaterialData: can't construct with non-owned layer data but" << layerDataFlags, );
    _attributeDataFlags = attributeDataFlags;
    _layerDataFlags = layerDataFlags;

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

        if(end - begin > 1) for(std::size_t j = begin + 1; j != end; ++j) {
            CORRADE_ASSERT(_data[j - 1].name() != _data[j].name(),
                "Trade::MaterialData: duplicate attribute" << _data[j].name(), );
            CORRADE_ASSERT(_data[j - 1].name() < _data[j].name(),
                "Trade::MaterialData:" << _data[j].name() << "has to be sorted before" << _data[j - 1].name() << "if passing non-owned data", );
        }

        begin = end;
    }

    CORRADE_ASSERT(layerOffsets.back() == _data.size(),
        "Trade::MaterialData: last layer offset" << layerOffsets.back() << "too short for" << _data.size() << "attributes in total", );
    #endif
}

MaterialData::MaterialData(MaterialData&&) noexcept = default;

MaterialData::~MaterialData() = default;

MaterialData& MaterialData::operator=(MaterialData&&) noexcept = default;

UnsignedInt MaterialData::attributeDataOffset(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer <= layerCount(),
        "Trade::MaterialData::attributeDataOffset(): index" << layer << "out of range for" << layerCount() << "layers", {});
    if(!_layerOffsets)
        return layer == 0 ? 0 : _data.size();
    return layer == 0 ? 0 : _layerOffsets[layer - 1];
}

UnsignedInt MaterialData::findLayerIdInternal(const Containers::StringView layer) const {
    for(std::size_t i = 1; i < _layerOffsets.size(); ++i) {
        if(_layerOffsets[i] > _layerOffsets[i - 1] &&
           _data[_layerOffsets[i - 1]].name() == " LayerName"_s &&
           _data[_layerOffsets[i - 1]].value<Containers::StringView>() == layer)
            return i;
    }
    return ~UnsignedInt{};
}

bool MaterialData::hasLayer(const Containers::StringView layer) const {
    return findLayerIdInternal(layer) != ~UnsignedInt{};
}

bool MaterialData::hasLayer(const MaterialLayer layer) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::hasLayer(): invalid name" << layer, {});
    return hasLayer(string);
}

Containers::Optional<UnsignedInt> MaterialData::findLayerId(const Containers::StringView layer) const {
    const UnsignedInt id = findLayerIdInternal(layer);
    return id == ~UnsignedInt{} ? Containers::Optional<UnsignedInt>{} : id;
}

Containers::Optional<UnsignedInt> MaterialData::findLayerId(const MaterialLayer layer) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::findLayerId(): invalid name" << layer, {});
    return findLayerId(string);
}

UnsignedInt MaterialData::layerId(const Containers::StringView layer) const {
    const UnsignedInt id = findLayerIdInternal(layer);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::layerId(): layer" << layer << "not found", {});
    return id;
}

UnsignedInt MaterialData::layerId(const MaterialLayer layer) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::layerId(): invalid name" << layer, {});
    return layerId(string);
}

Containers::StringView MaterialData::layerName(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerName(): index" << layer << "out of range for" << layerCount() << "layers", {});
    /* Deliberately ignore this attribute in the base material */
    if(layer && _layerOffsets[layer] > _layerOffsets[layer - 1] && _data[_layerOffsets[layer - 1]].name() == " LayerName")
        return _data[_layerOffsets[layer - 1]].value<Containers::StringView>();
    return {};
}

Float MaterialData::layerFactor(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerFactor(): index" << layer << "out of range for" << layerCount() << "layers", {});
    return attributeOr(layer, MaterialAttribute::LayerFactor, 1.0f);
}

Float MaterialData::layerFactor(const Containers::StringView layer) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactor(): layer" << layer << "not found", {});
    return layerFactor(layerId);
}

Float MaterialData::layerFactor(const MaterialLayer layer) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::layerFactor(): invalid name" << layer, {});
    return layerFactor(string);
}

UnsignedInt MaterialData::layerFactorTexture(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerFactorTexture(): index" << layer << "out of range for" << layerCount() << "layers", {});
    return attribute<UnsignedInt>(layer, MaterialAttribute::LayerFactorTexture);
}

UnsignedInt MaterialData::layerFactorTexture(const Containers::StringView layer) const {
    #ifndef CORRADE_NO_ASSERT
    const UnsignedInt layerId = findLayerIdInternal(layer);
    #endif
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactorTexture(): layer" << layer << "not found", {});
    /* Not delegating into layerFactorTexture() in order to have layer name
       caught in the assert */
    return attribute<UnsignedInt>(layer, MaterialAttribute::LayerFactorTexture);
}

UnsignedInt MaterialData::layerFactorTexture(const MaterialLayer layer) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::layerFactorTexture(): invalid name" << layer, {});
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
    #ifndef CORRADE_NO_ASSERT
    const UnsignedInt layerId = findLayerIdInternal(layer);
    #endif
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactorTextureSwizzle(): layer" << layer << "not found", {});
    CORRADE_ASSERT(hasAttribute(layerId, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureSwizzle(): layer" << layer << "doesn't have a factor texture", {});
    /* Not delegating into layerFactorTextureSwizzle() because we have a
       different variant of the assert here */
    return attributeOr(layer, MaterialAttribute::LayerFactorTextureSwizzle, MaterialTextureSwizzle::R);
}

MaterialTextureSwizzle MaterialData::layerFactorTextureSwizzle(const MaterialLayer layer) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::layerFactorTextureSwizzle(): invalid name" << layer, {});
    return layerFactorTextureSwizzle(string);
}

Matrix3 MaterialData::layerFactorTextureMatrix(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerFactorTextureMatrix(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(hasAttribute(layer, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureMatrix(): layer" << layer << "doesn't have a factor texture", {});
    if(Containers::Optional<Matrix3> value = findAttribute<Matrix3>(layer, MaterialAttribute::LayerFactorTextureMatrix))
        return *value;
    if(Containers::Optional<Matrix3> value = findAttribute<Matrix3>(layer, MaterialAttribute::TextureMatrix))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureMatrix, Matrix3{});
}

Matrix3 MaterialData::layerFactorTextureMatrix(const Containers::StringView layer) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactorTextureMatrix(): layer" << layer << "not found", {});
    CORRADE_ASSERT(hasAttribute(layerId, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureMatrix(): layer" << layer << "doesn't have a factor texture", {});
    /* Not delegating into layerFactorTextureMatrix() because we have a
       different variant of the assert here */
    if(Containers::Optional<Matrix3> value = findAttribute<Matrix3>(layerId, MaterialAttribute::LayerFactorTextureMatrix))
        return *value;
    if(Containers::Optional<Matrix3> value = findAttribute<Matrix3>(layerId, MaterialAttribute::TextureMatrix))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureMatrix, Matrix3{});
}

Matrix3 MaterialData::layerFactorTextureMatrix(const MaterialLayer layer) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::layerFactorTextureMatrix(): invalid name" << layer, {});
    return layerFactorTextureMatrix(string);
}

UnsignedInt MaterialData::layerFactorTextureCoordinates(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerFactorTextureCoordinates(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(hasAttribute(layer, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer" << layer << "doesn't have a factor texture", {});
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(layer, MaterialAttribute::LayerFactorTextureCoordinates))
        return *value;
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(layer, MaterialAttribute::TextureCoordinates))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt MaterialData::layerFactorTextureCoordinates(const Containers::StringView layer) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer" << layer << "not found", {});
    CORRADE_ASSERT(hasAttribute(layerId, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer" << layer << "doesn't have a factor texture", {});
    /* Not delegating into layerFactorTextureCoordinates() because we have a
       different variant of the assert here */
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(layerId, MaterialAttribute::LayerFactorTextureCoordinates))
        return *value;
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(layerId, MaterialAttribute::TextureCoordinates))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt MaterialData::layerFactorTextureCoordinates(const MaterialLayer layer) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::layerFactorTextureCoordinates(): invalid name" << layer, {});
    return layerFactorTextureCoordinates(string);
}

UnsignedInt MaterialData::layerFactorTextureLayer(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::layerFactorTextureLayer(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(hasAttribute(layer, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureLayer(): layer" << layer << "doesn't have a factor texture", {});
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(layer, MaterialAttribute::LayerFactorTextureLayer))
        return *value;
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(layer, MaterialAttribute::TextureLayer))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureLayer, 0u);
}

UnsignedInt MaterialData::layerFactorTextureLayer(const Containers::StringView layer) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::layerFactorTextureLayer(): layer" << layer << "not found", {});
    CORRADE_ASSERT(hasAttribute(layerId, MaterialAttribute::LayerFactorTexture),
        "Trade::MaterialData::layerFactorTextureLayer(): layer" << layer << "doesn't have a factor texture", {});
    /* Not delegating into layerFactorTextureLayer() because we have a
       different variant of the assert here */
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(layerId, MaterialAttribute::LayerFactorTextureLayer))
        return *value;
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(layerId, MaterialAttribute::TextureLayer))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureLayer, 0u);
}

UnsignedInt MaterialData::layerFactorTextureLayer(const MaterialLayer layer) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::layerFactorTextureLayer(): invalid name" << layer, {});
    return layerFactorTextureLayer(string);
}

UnsignedInt MaterialData::attributeCount(const UnsignedInt layer) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeCount(): index" << layer << "out of range for" << layerCount() << "layers", {});
    if(!_layerOffsets) return _data.size();
    if(!layer) return _layerOffsets[0];
    return _layerOffsets[layer] - _layerOffsets[layer - 1];
}

UnsignedInt MaterialData::attributeCount(const Containers::StringView layer) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeCount(): layer" << layer << "not found", {});
    return attributeCount(layerId);
}

UnsignedInt MaterialData::attributeCount(const MaterialLayer layer) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeCount(): invalid name" << layer, {});
    return attributeCount(string);
}

UnsignedInt MaterialData::findAttributeIdInternal(const UnsignedInt layer, const Containers::StringView name) const {
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
    return findAttributeIdInternal(layer, name) != ~UnsignedInt{};
}

bool MaterialData::hasAttribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::hasAttribute(): invalid name" << name, {});
    return hasAttribute(layer, string);
}

bool MaterialData::hasAttribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::hasAttribute(): layer" << layer << "not found", {});
    return hasAttribute(layerId, name);
}

bool MaterialData::hasAttribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::hasAttribute(): invalid name" << name, {});
    return hasAttribute(layer, string);
}

bool MaterialData::hasAttribute(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::hasAttribute(): invalid name" << layer, {});
    return hasAttribute(string, name);
}

bool MaterialData::hasAttribute(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::hasAttribute(): invalid name" << layer, {});
    return hasAttribute(string, name);
}

Containers::Optional<UnsignedInt> MaterialData::findAttributeId(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::findAttributeId(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = findAttributeIdInternal(layer, name);
    return id == ~UnsignedInt{} ? Containers::Optional<UnsignedInt>{} : id;
}

Containers::Optional<UnsignedInt> MaterialData::findAttributeId(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::findAttributeId(): invalid name" << name, {});
    return findAttributeId(layer, string);
}

Containers::Optional<UnsignedInt> MaterialData::findAttributeId(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::findAttributeId(): layer" << layer << "not found", {});
    const UnsignedInt id = findAttributeIdInternal(layerId, name);
    return id == ~UnsignedInt{} ? Containers::Optional<UnsignedInt>{} : id;
}

Containers::Optional<UnsignedInt> MaterialData::findAttributeId(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::findAttributeId(): invalid name" << name, {});
    return findAttributeId(layer, string);
}

Containers::Optional<UnsignedInt> MaterialData::findAttributeId(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::findAttributeId(): invalid name" << layer, {});
    return findAttributeId(string, name);
}

Containers::Optional<UnsignedInt> MaterialData::findAttributeId(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::findAttributeId(): invalid name" << layer, {});
    return findAttributeId(string, name);
}

Containers::Optional<UnsignedInt> MaterialData::findAttributeId(const Containers::StringView name) const {
    return findAttributeId(0, name);
}

Containers::Optional<UnsignedInt> MaterialData::findAttributeId(const MaterialAttribute name) const {
    return findAttributeId(0, name);
}

UnsignedInt MaterialData::attributeId(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeId(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = findAttributeIdInternal(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attributeId(): attribute" << name << "not found in layer" << layer, {});
    return id;
}

UnsignedInt MaterialData::attributeId(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeId(): invalid name" << name, {});
    return attributeId(layer, string);
}

UnsignedInt MaterialData::attributeId(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeId(): layer" << layer << "not found", {});
    const UnsignedInt id = findAttributeIdInternal(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attributeId(): attribute" << name << "not found in layer" << layer, {});
    return id;
}

UnsignedInt MaterialData::attributeId(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeId(): invalid name" << name, {});
    return attributeId(layer, string);
}

UnsignedInt MaterialData::attributeId(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeId(): invalid name" << layer, {});
    return attributeId(string, name);
}

UnsignedInt MaterialData::attributeId(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeId(): invalid name" << layer, {});
    return attributeId(string, name);
}

const MaterialAttributeData& MaterialData::attributeData(const UnsignedInt layer, const UnsignedInt id) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeData(): index" << layer << "out of range for" << layerCount() << "layers", _data[0]);
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attributeData(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, _data[0]);
    return _data[layerOffset(layer) + id];
}

Containers::StringView MaterialData::attributeName(const UnsignedInt layer, const UnsignedInt id) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeName(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attributeName(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layer) + id]._data.data + 1;
}

Containers::StringView MaterialData::attributeName(const Containers::StringView layer, const UnsignedInt id) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeName(): layer" << layer << "not found", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attributeName(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layerId) + id]._data.data + 1;
}

Containers::StringView MaterialData::attributeName(const MaterialLayer layer, const UnsignedInt id) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeName(): invalid name" << layer, {});
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
    const UnsignedInt id = findAttributeIdInternal(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attributeType(): attribute" << name << "not found in layer" << layer, {});
    return _data[layerOffset(layer) + id]._data.type;
}

MaterialAttributeType MaterialData::attributeType(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeType(): invalid name" << name, {});
    return attributeType(layer, string);
}

MaterialAttributeType MaterialData::attributeType(const Containers::StringView layer, const UnsignedInt id) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeType(): layer" << layer << "not found", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attributeType(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layerId) + id]._data.type;
}

MaterialAttributeType MaterialData::attributeType(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeType(): layer" << layer << "not found", {});
    const UnsignedInt id = findAttributeIdInternal(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attributeType(): attribute" << name << "not found in layer" << layer, {});
    return _data[layerOffset(layerId) + id]._data.type;
}

MaterialAttributeType MaterialData::attributeType(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeType(): invalid name" << name, {});
    return attributeType(layer, string);
}

MaterialAttributeType MaterialData::attributeType(const MaterialLayer layer, const UnsignedInt id) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeType(): invalid name" << layer, {});
    return attributeType(string, id);
}

MaterialAttributeType MaterialData::attributeType(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeType(): invalid name" << layer, {});
    return attributeType(string, name);
}

MaterialAttributeType MaterialData::attributeType(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::attributeType(): invalid name" << layer, {});
    return attributeType(string, name);
}

const void* MaterialData::attribute(const UnsignedInt layer, const UnsignedInt id) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layer) + id].value();
}

void* MaterialData::mutableAttribute(const UnsignedInt layer, const UnsignedInt id) {
    CORRADE_ASSERT(_attributeDataFlags & DataFlag::Mutable,
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable", {});
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::mutableAttribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::mutableAttribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return const_cast<void*>(_data[layerOffset(layer) + id].value());
}

const void* MaterialData::attribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = findAttributeIdInternal(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found in layer" << layer, {});
    return _data[layerOffset(layer) + id].value();
}

void* MaterialData::mutableAttribute(const UnsignedInt layer, const Containers::StringView name) {
    CORRADE_ASSERT(_attributeDataFlags & DataFlag::Mutable,
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable", {});
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::mutableAttribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = findAttributeIdInternal(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::mutableAttribute(): attribute" << name << "not found in layer" << layer, {});
    return const_cast<void*>(_data[layerOffset(layer) + id].value());
}

const void* MaterialData::attribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute(layer, string);
}

void* MaterialData::mutableAttribute(const UnsignedInt layer, const MaterialAttribute name) {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::mutableAttribute(): invalid name" << name, {});
    return mutableAttribute(layer, string);
}

const void* MaterialData::attribute(const Containers::StringView layer, const UnsignedInt id) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): layer" << layer << "not found", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return _data[layerOffset(layerId) + id].value();
}

void* MaterialData::mutableAttribute(const Containers::StringView layer, const UnsignedInt id) {
    CORRADE_ASSERT(_attributeDataFlags & DataFlag::Mutable,
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable", {});
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::mutableAttribute(): layer" << layer << "not found", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::mutableAttribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return const_cast<void*>(_data[layerOffset(layerId) + id].value());
}

const void* MaterialData::attribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): layer" << layer << "not found", {});
    const UnsignedInt id = findAttributeIdInternal(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found in layer" << layer, {});
    return _data[layerOffset(layerId) + id].value();
}

void* MaterialData::mutableAttribute(const Containers::StringView layer, const Containers::StringView name) {
    CORRADE_ASSERT(_attributeDataFlags & DataFlag::Mutable,
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable", {});
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::mutableAttribute(): layer" << layer << "not found", {});
    const UnsignedInt id = findAttributeIdInternal(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::mutableAttribute(): attribute" << name << "not found in layer" << layer, {});
    return const_cast<void*>(_data[layerOffset(layerId) + id].value());
}

const void* MaterialData::attribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute(layer, string);
}

void* MaterialData::mutableAttribute(const Containers::StringView layer, const MaterialAttribute name) {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::mutableAttribute(): invalid name" << name, {});
    return mutableAttribute(layer, string);
}

const void* MaterialData::attribute(const MaterialLayer layer, const UnsignedInt id) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute(string, id);
}

void* MaterialData::mutableAttribute(const MaterialLayer layer, const UnsignedInt id) {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::mutableAttribute(): invalid name" << layer, {});
    return mutableAttribute(string, id);
}

const void* MaterialData::attribute(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute(string, name);
}

void* MaterialData::mutableAttribute(const MaterialLayer layer, const Containers::StringView name) {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::mutableAttribute(): invalid name" << layer, {});
    return mutableAttribute(string, name);
}

const void* MaterialData::attribute(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute(string, name);
}

void* MaterialData::mutableAttribute(const MaterialLayer layer, const MaterialAttribute name) {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::mutableAttribute(): invalid name" << layer, {});
    return mutableAttribute(string, name);
}

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

template<> MAGNUM_TRADE_EXPORT Containers::MutableStringView MaterialData::mutableAttribute<Containers::MutableStringView>(const UnsignedInt layer, const UnsignedInt id) {
    CORRADE_ASSERT(_attributeDataFlags & DataFlag::Mutable,
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable", {});
    /* Can't delegate to mutableAttribute() returning void* because that
       doesn't include the size */
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::mutableAttribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::mutableAttribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    const Trade::MaterialAttributeData& data = _data[layerOffset(layer) + id];
    CORRADE_ASSERT(data._data.type == MaterialAttributeType::String,
        "Trade::MaterialData::mutableAttribute():" << (data._data.data + 1) << "of" << data._data.type << "can't be retrieved as a string", {});
    return {const_cast<char*>(data._data.s.nameValue) + Implementation::MaterialAttributeDataSize - data._data.s.size - 3, data._data.s.size, Containers::StringViewFlag::NullTerminated};
}

template<> MAGNUM_TRADE_EXPORT Containers::ArrayView<const void> MaterialData::attribute<Containers::ArrayView<const void>>(const UnsignedInt layer, const UnsignedInt id) const {
    /* Can't delegate to attribute() returning const void* because that doesn't
       include the size */
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    const Trade::MaterialAttributeData& data = _data[layerOffset(layer) + id];
    CORRADE_ASSERT(data._data.type == MaterialAttributeType::Buffer,
        "Trade::MaterialData::attribute():" << (data._data.data + 1) << "of" << data._data.type << "can't be retrieved as a buffer", {});
    /* Using an internal StringView API because it's never slower than
       memchr() */
    /** @todo have an optimized aligned fixed-size variant? */
    const char* const nameEnd = Containers::Implementation::stringFindCharacter(data._data.data, Implementation::MaterialAttributeDataSize, '\0');
    CORRADE_INTERNAL_ASSERT(nameEnd);
    const std::size_t size = *(nameEnd + 1);
    return {data._data.data + Implementation::MaterialAttributeDataSize - size, size};
}

template<> MAGNUM_TRADE_EXPORT Containers::ArrayView<void> MaterialData::mutableAttribute<Containers::ArrayView<void>>(const UnsignedInt layer, const UnsignedInt id) {
    CORRADE_ASSERT(_attributeDataFlags & DataFlag::Mutable,
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable", {});
    /* Can't delegate to mutableAttribute() returning void* because that
       doesn't include the size */
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::mutableAttribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::mutableAttribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    const Trade::MaterialAttributeData& data = _data[layerOffset(layer) + id];
    CORRADE_ASSERT(data._data.type == MaterialAttributeType::Buffer,
        "Trade::MaterialData::mutableAttribute():" << (data._data.data + 1) << "of" << data._data.type << "can't be retrieved as a buffer", {});
    /* Using an internal StringView API because it's never slower than
       memchr() */
    /** @todo have an optimized aligned fixed-size variant? */
    const char* const nameEnd = Containers::Implementation::stringFindCharacter(data._data.data, Implementation::MaterialAttributeDataSize, '\0');
    CORRADE_INTERNAL_ASSERT(nameEnd);
    const std::size_t size = *(nameEnd + 1);
    return {const_cast<char*>(data._data.data) + Implementation::MaterialAttributeDataSize - size, size};
}

const void* MaterialData::findAttribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::findAttribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = findAttributeIdInternal(layer, name);
    if(id == ~UnsignedInt{}) return nullptr;
    return _data[layerOffset(layer) + id].value();
}

const void* MaterialData::findAttribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::findAttribute(): invalid name" << name, {});
    return findAttribute(layer, string);
}

const void* MaterialData::findAttribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::findAttribute(): layer" << layer << "not found", {});
    const UnsignedInt id = findAttributeIdInternal(layerId, name);
    if(id == ~UnsignedInt{}) return nullptr;
    return _data[layerOffset(layerId) + id].value();
}

const void* MaterialData::findAttribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string, "Trade::MaterialData::findAttribute(): invalid name" << name, {});
    return findAttribute(layer, string);
}

const void* MaterialData::findAttribute(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::findAttribute(): invalid name" << layer, {});
    return findAttribute(string, name);
}

const void* MaterialData::findAttribute(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string, "Trade::MaterialData::findAttribute(): invalid name" << layer, {});
    return findAttribute(string, name);
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
    return Utility::move(_layerOffsets);
}

Containers::Array<MaterialAttributeData> MaterialData::releaseAttributeData() {
    return Utility::move(_data);
}

Debug& operator<<(Debug& debug, const MaterialLayer value) {
    debug << "Trade::MaterialLayer" << Debug::nospace;

    if(UnsignedInt(value) - 1 >= Containers::arraySize(LayerMap))
        return debug << "(" << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << ")";

    return debug << "::" << Debug::nospace << LayerMap[UnsignedInt(value) - 1];
}

Debug& operator<<(Debug& debug, const MaterialAttribute value) {
    debug << "Trade::MaterialAttribute" << Debug::nospace;

    if(UnsignedInt(value) - 1 >= Containers::arraySize(AttributeMap))
        return debug << "(" << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << ")";

    /* LayerName is prefixed with a single space, drop that */
    Containers::StringView string = AttributeMap[UnsignedInt(value) - 1].name;
    if(string[0] == ' ') string = string.exceptPrefix(1);

    return debug << "::" << Debug::nospace << string;
}

Debug& operator<<(Debug& debug, const MaterialTextureSwizzle value) {
    /* The swizzle is encoded as a fourCC, so just print the numerical value as
       a char. Worst case this will print nothing or four garbage letters.
       Sorry in that case. GCC 4.8 doesn't understand just {}, wants to have
       a full MaterialTextureSwizzle{} here. */
    const MaterialTextureSwizzle values[]{value, MaterialTextureSwizzle{}};
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::MaterialTextureSwizzle::" << Debug::nospace;

    return debug << reinterpret_cast<const char*>(values);
}

Debug& operator<<(Debug& debug, const MaterialAttributeType value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::MaterialAttributeType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MaterialAttributeType::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
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
        _c(Buffer)
        _c(TextureSwizzle)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedShort(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const MaterialType value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::MaterialType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MaterialType::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(Flat)
        _c(Phong)
        _c(PbrMetallicRoughness)
        _c(PbrSpecularGlossiness)
        _c(PbrClearCoat)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const MaterialTypes value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "Trade::MaterialTypes{}", {
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

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MaterialData::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::MaterialData::Flags{}", {
        MaterialData::Flag::DoubleSided
    });
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

Debug& operator<<(Debug& debug, const MaterialAlphaMode value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::MaterialAlphaMode" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MaterialAlphaMode::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(Opaque)
        _c(Mask)
        _c(Blend)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << (packed ? "" : ")");
}

}}
