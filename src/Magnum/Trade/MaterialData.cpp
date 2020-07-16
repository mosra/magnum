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

#include "Magnum/Math/Vector4.h"
#include "Magnum/Math/Matrix.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

namespace {

using namespace Containers::Literals;

#ifndef DOXYGEN_GENERATING_OUTPUT /* It gets *really* confused */
constexpr struct {
    Containers::StringView name;
    MaterialAttributeType type;
    UnsignedByte size;
} AttributeMap[]{
    #define _c(name, type) {#name ## _s, MaterialAttributeType::type, sizeof(type)},
    #define _ct(name, typeName, type) {#name ## _s, MaterialAttributeType::typeName, sizeof(type)},
    #include "Magnum/Trade/Implementation/materialAttributeProperties.hpp"
    #undef _c
    #undef _ct
};
#endif

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
            return 4;

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
    }

    CORRADE_ASSERT_UNREACHABLE("Trade::materialAttributeTypeSize(): invalid type" << type, {});
}

MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const MaterialAttributeType type, const std::size_t size, const void* const value) noexcept: _data{} /* zero-initialized */ {
    /* The 2 extra bytes are for a null byte after the name and a type */
    CORRADE_ASSERT(name.size() + size + 2 <= Implementation::MaterialAttributeDataSize,
        "Trade::MaterialAttributeData: name" << name << "too long, expected at most" << Implementation::MaterialAttributeDataSize - size - 2 << "bytes for" << type << "but got" << name.size(), );
    _data.type = type;
    std::memcpy(_data.data + 1, name.data(), name.size());
    std::memcpy(_data.data + Implementation::MaterialAttributeDataSize - size, value, size);
}

MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const MaterialAttributeType type, const void* const value) noexcept: MaterialAttributeData{name, type, materialAttributeTypeSize(type), value} {}

MaterialAttributeData::MaterialAttributeData(const MaterialAttribute name, const MaterialAttributeType type, const void* const value) noexcept {
    CORRADE_ASSERT(UnsignedInt(name) - 1 < Containers::arraySize(AttributeMap),
        "Trade::MaterialAttributeData: invalid name" << name, );
    CORRADE_ASSERT(AttributeMap[UnsignedInt(name) - 1].type == type,
        "Trade::MaterialAttributeData: expected" << AttributeMap[UnsignedInt(name) - 1].type << "for" << name << "but got" << type, );
    _data.type = type;
    std::memcpy(_data.data + 1, AttributeMap[UnsignedInt(name) - 1].name.data(), AttributeMap[UnsignedInt(name) - 1].name.size());
    std::memcpy(_data.data + Implementation::MaterialAttributeDataSize - AttributeMap[UnsignedInt(name) - 1].size, value, AttributeMap[UnsignedInt(name) - 1].size);
}

const void* MaterialAttributeData::value() const {
    return _data.data + Implementation::MaterialAttributeDataSize - materialAttributeTypeSize(_data.type);
}

MaterialData::MaterialData(Containers::Array<MaterialAttributeData>&& data, const void* const importerState) noexcept: _data{std::move(data)}, _importerState{importerState} {
    #ifndef CORRADE_NO_ASSERT
    /* Not checking what's already done in MaterialAttributeData constructor.
       Done before sorting so the index refers to the actual input index. */
    for(std::size_t i = 0; i != _data.size(); ++i)
        CORRADE_ASSERT(_data[i]._data.type != MaterialAttributeType{},
            "Trade::MaterialData: attribute" << i << "doesn't specify anything", );
    #endif

    /* Check if attributes are sorted already. If not, sort them. Can't just
       call std::sort() unconditionally because if the data would be immutable
       (for example when acquiring release()d immutable data from another
       instance) it could cause crashes. (I expected not, but apparently ASan
       blows up on that.) */
    if(_data.size() > 1) for(std::size_t i = 1; i != _data.size(); ++i) {
        if(_data[i - 1].name() < _data[i].name()) continue;

        std::sort(_data.begin(), _data.end(), [](const MaterialAttributeData& a, const MaterialAttributeData& b) {
            /* Need to check here (instead of in the outer for loop) as we
               exit the loop right after the sort and thus duplicates that
               occur after the first non-sorted pair wouldn't get detected. */
            CORRADE_ASSERT(a.name() != b.name(),
                "Trade::MaterialData: duplicate attribute" << a.name(), false);
            return a.name() < b.name();
        });
        break;
    }
}

MaterialData::MaterialData(const std::initializer_list<MaterialAttributeData> data, const void* const importerState): MaterialData{Implementation::initializerListToArrayWithDefaultDeleter(data), importerState} {}

MaterialData::MaterialData(DataFlags, const Containers::ArrayView<const MaterialAttributeData> data, const void* const importerState) noexcept: _data{Containers::Array<MaterialAttributeData>{const_cast<MaterialAttributeData*>(data.data()), data.size(), [](MaterialAttributeData*, std::size_t){}}}, _importerState{importerState} {
    #ifndef CORRADE_NO_ASSERT
    /* Not checking what's already done in MaterialAttributeData constructor */
    for(std::size_t i = 0; i != _data.size(); ++i)
        CORRADE_ASSERT(_data[i]._data.type != MaterialAttributeType{},
            "Trade::MaterialData: attribute" << i << "doesn't specify anything", );

    if(_data.size() > 1) for(std::size_t i = 1; i != _data.size(); ++i) {
        CORRADE_ASSERT(_data[i - 1].name() != _data[i].name(),
            "Trade::MaterialData: duplicate attribute" << _data[i].name(), );
        CORRADE_ASSERT(_data[i - 1].name() < _data[i].name(),
            "Trade::MaterialData:" << _data[i].name() << "has to be sorted before" << _data[i - 1].name() << "if passing non-owned data", );
    }
    #endif
}

MaterialData::MaterialData(MaterialData&&) noexcept = default;

MaterialData::~MaterialData() = default;

MaterialData& MaterialData::operator=(MaterialData&&) noexcept = default;

Containers::StringView MaterialData::attributeString(const MaterialAttribute name) {
    #ifndef CORRADE_NO_ASSERT
    if(UnsignedInt(name) - 1 >= Containers::arraySize(AttributeMap))
        return nullptr;
    #endif
    return AttributeMap[UnsignedInt(name) - 1].name;
}

UnsignedInt MaterialData::attributeFor(const Containers::StringView name) const {
    const MaterialAttributeData* const found = std::lower_bound(_data.begin(), _data.end(), name, [](const MaterialAttributeData& a, const Containers::StringView& b) {
        return a.name() < b;
    });
    if(found == _data.end() || found->name() != name) return ~UnsignedInt{};
    return found - _data.begin();
}

bool MaterialData::hasAttribute(const Containers::StringView name) const {
    return attributeFor(name) != ~UnsignedInt{};
}

bool MaterialData::hasAttribute(const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::hasAttribute(): invalid name" << name, {});
    return hasAttribute(string);
}

UnsignedInt MaterialData::attributeId(const Containers::StringView name) const {
    const UnsignedInt id = attributeFor(name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attributeId(): attribute" << name << "not found", {});
    return id;
}

UnsignedInt MaterialData::attributeId(const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeId(): invalid name" << name, {});
    return attributeId(string);
}

Containers::StringView MaterialData::attributeName(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _data.size(),
        "Trade::MaterialData::attributeName(): index" << id << "out of range for" << _data.size() << "attributes", {});
    return _data[id]._data.data + 1;
}

MaterialAttributeType MaterialData::attributeType(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _data.size(),
        "Trade::MaterialData::attributeType(): index" << id << "out of range for" << _data.size() << "attributes", {});
    return _data[id]._data.type;
}

MaterialAttributeType MaterialData::attributeType(const Containers::StringView name) const {
    const UnsignedInt id = attributeFor(name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attributeType(): attribute" << name << "not found", {});
    return _data[id]._data.type;
}

MaterialAttributeType MaterialData::attributeType(const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeType(): invalid name" << name, {});
    return attributeType(string);
}

const void* MaterialData::attribute(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _data.size(),
        "Trade::MaterialData::attribute(): index" << id << "out of range for" << _data.size() << "attributes", {});
    return _data[id].value();
}

const void* MaterialData::attribute(const Containers::StringView name) const {
    const UnsignedInt id = attributeFor(name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found", {});
    return _data[id].value();
}

const void* MaterialData::attribute(const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute(string);
}

Containers::Array<MaterialAttributeData> MaterialData::release() {
    return std::move(_data);
}

Debug& operator<<(Debug& debug, const MaterialAttribute value) {
    debug << "Trade::MaterialAttribute" << Debug::nospace;

    if(UnsignedInt(value) - 1 >= Containers::arraySize(AttributeMap))
        return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";

    return debug << "::" << Debug::nospace << AttributeMap[UnsignedInt(value) - 1].name;
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
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

}}
