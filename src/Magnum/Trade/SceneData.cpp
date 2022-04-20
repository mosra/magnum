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

#include "SceneData.h"

#include <algorithm> /* std::lower_bound() */
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayViewStl.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/DualComplex.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/ArrayViewStl.h>
#endif

namespace Magnum { namespace Trade {

Debug& operator<<(Debug& debug, const SceneMappingType value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::SceneMappingType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SceneMappingType::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(UnsignedByte)
        _c(UnsignedInt)
        _c(UnsignedShort)
        _c(UnsignedLong)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << (packed ? "" : ")");
}

UnsignedInt sceneMappingTypeSize(const SceneMappingType type) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(type) {
        case SceneMappingType::UnsignedByte: return 1;
        case SceneMappingType::UnsignedShort: return 2;
        case SceneMappingType::UnsignedInt: return 4;
        case SceneMappingType::UnsignedLong: return 8;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("Trade::sceneMappingTypeSize(): invalid type" << type, {});
}

UnsignedInt sceneMappingTypeAlignment(const SceneMappingType type) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(type) {
        case SceneMappingType::UnsignedByte: return 1;
        case SceneMappingType::UnsignedShort: return 2;
        case SceneMappingType::UnsignedInt: return 4;
        case SceneMappingType::UnsignedLong: return 8;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("Trade::sceneMappingTypeAlignment(): invalid type" << type, {});
}

Debug& operator<<(Debug& debug, const SceneField value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::SceneField" << Debug::nospace;

    if(isSceneFieldCustom(value))
        return debug << (packed ? "Custom(" : "::Custom(") << Debug::nospace << sceneFieldCustom(value) << Debug::nospace << ")";

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SceneField::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(Parent)
        _c(Transformation)
        _c(Translation)
        _c(Rotation)
        _c(Scaling)
        _c(Mesh)
        _c(MeshMaterial)
        _c(Light)
        _c(Camera)
        _c(Skin)
        _c(ImporterState)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const SceneFieldType value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::SceneFieldType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SceneFieldType::value: return debug << (debug.immediateFlags() & Debug::Flag::Packed ? "" : "::") << Debug::nospace << #value;
        _c(Float)
        _c(Half)
        _c(Double)
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(UnsignedLong)
        _c(Long)
        _c(Vector2)
        _c(Vector2h)
        _c(Vector2d)
        _c(Vector2ub)
        _c(Vector2b)
        _c(Vector2us)
        _c(Vector2s)
        _c(Vector2ui)
        _c(Vector2i)
        _c(Vector3)
        _c(Vector3h)
        _c(Vector3d)
        _c(Vector3ub)
        _c(Vector3b)
        _c(Vector3us)
        _c(Vector3s)
        _c(Vector3ui)
        _c(Vector3i)
        _c(Vector4)
        _c(Vector4h)
        _c(Vector4d)
        _c(Vector4ub)
        _c(Vector4b)
        _c(Vector4us)
        _c(Vector4s)
        _c(Vector4ui)
        _c(Vector4i)
        _c(Matrix2x2)
        _c(Matrix2x2h)
        _c(Matrix2x2d)
        _c(Matrix2x3)
        _c(Matrix2x3h)
        _c(Matrix2x3d)
        _c(Matrix2x4)
        _c(Matrix2x4h)
        _c(Matrix2x4d)
        _c(Matrix3x2)
        _c(Matrix3x2h)
        _c(Matrix3x2d)
        _c(Matrix3x3)
        _c(Matrix3x3h)
        _c(Matrix3x3d)
        _c(Matrix3x4)
        _c(Matrix3x4h)
        _c(Matrix3x4d)
        _c(Matrix4x2)
        _c(Matrix4x2h)
        _c(Matrix4x2d)
        _c(Matrix4x3)
        _c(Matrix4x3h)
        _c(Matrix4x3d)
        _c(Matrix4x4)
        _c(Matrix4x4h)
        _c(Matrix4x4d)
        _c(Range1D)
        _c(Range1Dh)
        _c(Range1Dd)
        _c(Range1Di)
        _c(Range2D)
        _c(Range2Dh)
        _c(Range2Dd)
        _c(Range2Di)
        _c(Range3D)
        _c(Range3Dh)
        _c(Range3Dd)
        _c(Range3Di)
        _c(Complex)
        _c(Complexd)
        _c(DualComplex)
        _c(DualComplexd)
        _c(Quaternion)
        _c(Quaterniond)
        _c(DualQuaternion)
        _c(DualQuaterniond)
        _c(Deg)
        _c(Degh)
        _c(Degd)
        _c(Rad)
        _c(Radh)
        _c(Radd)
        _c(Pointer)
        _c(MutablePointer)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << (packed ? "" : ")");
}

UnsignedInt sceneFieldTypeSize(const SceneFieldType type) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(type) {
        case SceneFieldType::UnsignedByte:
        case SceneFieldType::Byte:
            return 1;
        case SceneFieldType::UnsignedShort:
        case SceneFieldType::Short:
        case SceneFieldType::Half:
        case SceneFieldType::Vector2ub:
        case SceneFieldType::Vector2b:
        case SceneFieldType::Degh:
        case SceneFieldType::Radh:
            return 2;
        case SceneFieldType::Vector3ub:
        case SceneFieldType::Vector3b:
            return 3;
        case SceneFieldType::UnsignedInt:
        case SceneFieldType::Int:
        case SceneFieldType::Float:
        case SceneFieldType::Vector2us:
        case SceneFieldType::Vector2s:
        case SceneFieldType::Vector2h:
        case SceneFieldType::Vector4ub:
        case SceneFieldType::Vector4b:
        case SceneFieldType::Range1Dh:
        case SceneFieldType::Deg:
        case SceneFieldType::Rad:
            return 4;
        case SceneFieldType::Vector3us:
        case SceneFieldType::Vector3s:
        case SceneFieldType::Vector3h:
            return 6;
        case SceneFieldType::UnsignedLong:
        case SceneFieldType::Long:
        case SceneFieldType::Double:
        case SceneFieldType::Vector2:
        case SceneFieldType::Vector2ui:
        case SceneFieldType::Vector2i:
        case SceneFieldType::Vector4us:
        case SceneFieldType::Vector4s:
        case SceneFieldType::Vector4h:
        case SceneFieldType::Matrix2x2h:
        case SceneFieldType::Range1D:
        case SceneFieldType::Range1Di:
        case SceneFieldType::Range2Dh:
        case SceneFieldType::Complex:
        case SceneFieldType::Degd:
        case SceneFieldType::Radd:
            return 8;
        case SceneFieldType::Vector3:
        case SceneFieldType::Vector3ui:
        case SceneFieldType::Vector3i:
        case SceneFieldType::Matrix2x3h:
        case SceneFieldType::Matrix3x2h:
        case SceneFieldType::Range3Dh:
            return 12;
        case SceneFieldType::Vector2d:
        case SceneFieldType::Vector4:
        case SceneFieldType::Vector4ui:
        case SceneFieldType::Vector4i:
        case SceneFieldType::Matrix2x2:
        case SceneFieldType::Matrix2x4h:
        case SceneFieldType::Matrix4x2h:
        case SceneFieldType::Range1Dd:
        case SceneFieldType::Range2D:
        case SceneFieldType::Range2Di:
        case SceneFieldType::Complexd:
        case SceneFieldType::DualComplex:
        case SceneFieldType::Quaternion:
            return 16;
        case SceneFieldType::Matrix3x3h:
            return 18;
        case SceneFieldType::Vector3d:
        case SceneFieldType::Matrix2x3:
        case SceneFieldType::Matrix3x4h:
        case SceneFieldType::Matrix3x2:
        case SceneFieldType::Matrix4x3h:
        case SceneFieldType::Range3D:
        case SceneFieldType::Range3Di:
            return 24;
        case SceneFieldType::Vector4d:
        case SceneFieldType::Matrix2x2d:
        case SceneFieldType::Matrix2x4:
        case SceneFieldType::Matrix4x2:
        case SceneFieldType::Matrix4x4h:
        case SceneFieldType::Range2Dd:
        case SceneFieldType::DualComplexd:
        case SceneFieldType::Quaterniond:
        case SceneFieldType::DualQuaternion:
            return 32;
        case SceneFieldType::Matrix3x3:
            return 36;
        case SceneFieldType::Matrix2x3d:
        case SceneFieldType::Matrix3x4:
        case SceneFieldType::Matrix3x2d:
        case SceneFieldType::Matrix4x3:
        case SceneFieldType::Range3Dd:
            return 48;
        case SceneFieldType::Matrix2x4d:
        case SceneFieldType::Matrix4x2d:
        case SceneFieldType::Matrix4x4:
        case SceneFieldType::DualQuaterniond:
            return 64;
        case SceneFieldType::Matrix3x3d:
            return 72;
        case SceneFieldType::Matrix3x4d:
        case SceneFieldType::Matrix4x3d:
            return 96;
        case SceneFieldType::Matrix4x4d:
            return 128;
        case SceneFieldType::Pointer:
        case SceneFieldType::MutablePointer:
            return sizeof(void*);
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("Trade::sceneFieldTypeSize(): invalid type" << type, {});
}

UnsignedInt sceneFieldTypeAlignment(const SceneFieldType type) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(type) {
        case SceneFieldType::UnsignedByte:
        case SceneFieldType::Vector2ub:
        case SceneFieldType::Vector3ub:
        case SceneFieldType::Vector4ub:
        case SceneFieldType::Byte:
        case SceneFieldType::Vector2b:
        case SceneFieldType::Vector3b:
        case SceneFieldType::Vector4b:
            return 1;
        case SceneFieldType::UnsignedShort:
        case SceneFieldType::Vector2us:
        case SceneFieldType::Vector3us:
        case SceneFieldType::Vector4us:
        case SceneFieldType::Short:
        case SceneFieldType::Vector2s:
        case SceneFieldType::Vector3s:
        case SceneFieldType::Vector4s:
        case SceneFieldType::Half:
        case SceneFieldType::Vector2h:
        case SceneFieldType::Vector3h:
        case SceneFieldType::Vector4h:
        case SceneFieldType::Matrix2x2h:
        case SceneFieldType::Matrix2x3h:
        case SceneFieldType::Matrix2x4h:
        case SceneFieldType::Matrix3x2h:
        case SceneFieldType::Matrix3x3h:
        case SceneFieldType::Matrix3x4h:
        case SceneFieldType::Matrix4x2h:
        case SceneFieldType::Matrix4x3h:
        case SceneFieldType::Matrix4x4h:
        case SceneFieldType::Range1Dh:
        case SceneFieldType::Range2Dh:
        case SceneFieldType::Range3Dh:
        case SceneFieldType::Degh:
        case SceneFieldType::Radh:
            return 2;
        case SceneFieldType::UnsignedInt:
        case SceneFieldType::Vector2ui:
        case SceneFieldType::Vector3ui:
        case SceneFieldType::Vector4ui:
        case SceneFieldType::Int:
        case SceneFieldType::Vector2i:
        case SceneFieldType::Vector3i:
        case SceneFieldType::Vector4i:
        case SceneFieldType::Float:
        case SceneFieldType::Vector2:
        case SceneFieldType::Vector3:
        case SceneFieldType::Vector4:
        case SceneFieldType::Matrix2x2:
        case SceneFieldType::Matrix2x3:
        case SceneFieldType::Matrix2x4:
        case SceneFieldType::Matrix3x2:
        case SceneFieldType::Matrix3x3:
        case SceneFieldType::Matrix3x4:
        case SceneFieldType::Matrix4x2:
        case SceneFieldType::Matrix4x3:
        case SceneFieldType::Matrix4x4:
        case SceneFieldType::Range1Di:
        case SceneFieldType::Range2Di:
        case SceneFieldType::Range3Di:
        case SceneFieldType::Range1D:
        case SceneFieldType::Range2D:
        case SceneFieldType::Range3D:
        case SceneFieldType::Complex:
        case SceneFieldType::Quaternion:
        case SceneFieldType::DualComplex:
        case SceneFieldType::DualQuaternion:
        case SceneFieldType::Deg:
        case SceneFieldType::Rad:
            return 4;
        case SceneFieldType::UnsignedLong:
        case SceneFieldType::Long:
        case SceneFieldType::Double:
        case SceneFieldType::Vector2d:
        case SceneFieldType::Vector3d:
        case SceneFieldType::Vector4d:
        case SceneFieldType::Matrix2x2d:
        case SceneFieldType::Matrix2x3d:
        case SceneFieldType::Matrix2x4d:
        case SceneFieldType::Matrix3x2d:
        case SceneFieldType::Matrix3x3d:
        case SceneFieldType::Matrix3x4d:
        case SceneFieldType::Matrix4x2d:
        case SceneFieldType::Matrix4x3d:
        case SceneFieldType::Matrix4x4d:
        case SceneFieldType::Range1Dd:
        case SceneFieldType::Range2Dd:
        case SceneFieldType::Range3Dd:
        case SceneFieldType::Complexd:
        case SceneFieldType::Quaterniond:
        case SceneFieldType::DualComplexd:
        case SceneFieldType::DualQuaterniond:
        case SceneFieldType::Degd:
        case SceneFieldType::Radd:
            return 8;
        case SceneFieldType::Pointer:
        case SceneFieldType::MutablePointer:
            return sizeof(void*);
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("Trade::sceneFieldTypeAlignment(): invalid type" << type, {});
}

Debug& operator<<(Debug& debug, const SceneFieldFlag value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::SceneFieldFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SceneFieldFlag::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(OffsetOnly)
        _c(ImplicitMapping)
        _c(OrderedMapping)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const SceneFieldFlags value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "Trade::SceneFieldFlags{}", {
        SceneFieldFlag::OffsetOnly,
        SceneFieldFlag::ImplicitMapping,
        /* This one is implied by ImplicitMapping, so has to be after */
        SceneFieldFlag::OrderedMapping
    });
}

SceneFieldData::SceneFieldData(const SceneField name, const Containers::StridedArrayView2D<const char>& mappingData, const SceneFieldType fieldType, const Containers::StridedArrayView2D<const char>& fieldData, const UnsignedShort fieldArraySize, const SceneFieldFlags flags) noexcept: SceneFieldData{name, {}, Containers::StridedArrayView1D<const void>{{mappingData.data(), ~std::size_t{}}, mappingData.size()[0], mappingData.stride()[0]}, fieldType, Containers::StridedArrayView1D<const void>{{fieldData.data(), ~std::size_t{}}, fieldData.size()[0], fieldData.stride()[0]}, fieldArraySize, flags} {
    /* Yes, this calls into a constexpr function defined in the header --
       because I feel that makes more sense than duplicating the full assert
       logic */
    #ifndef CORRADE_NO_ASSERT
    if(fieldArraySize) CORRADE_ASSERT(fieldData.isEmpty()[0] || fieldData.size()[1] == sceneFieldTypeSize(fieldType)*fieldArraySize,
        "Trade::SceneFieldData: second field view dimension size" << fieldData.size()[1] << "doesn't match" << fieldType << "and field array size" << fieldArraySize, );
    else CORRADE_ASSERT(fieldData.isEmpty()[0] || fieldData.size()[1] == sceneFieldTypeSize(fieldType),
        "Trade::SceneFieldData: second field view dimension size" << fieldData.size()[1] << "doesn't match" << fieldType, );
    #endif

    if(mappingData.size()[1] == 8) _mappingType = SceneMappingType::UnsignedLong;
    else if(mappingData.size()[1] == 4) _mappingType = SceneMappingType::UnsignedInt;
    else if(mappingData.size()[1] == 2) _mappingType = SceneMappingType::UnsignedShort;
    else if(mappingData.size()[1] == 1) _mappingType = SceneMappingType::UnsignedByte;
    else CORRADE_ASSERT_UNREACHABLE("Trade::SceneFieldData: expected second mapping view dimension size 1, 2, 4 or 8 but got" << mappingData.size()[1], );

    CORRADE_ASSERT(mappingData.isContiguous<1>(), "Trade::SceneFieldData: second mapping view dimension is not contiguous", );
    CORRADE_ASSERT(fieldData.isContiguous<1>(), "Trade::SceneFieldData: second field view dimension is not contiguous", );
}

Containers::Array<SceneFieldData> sceneFieldDataNonOwningArray(const Containers::ArrayView<const SceneFieldData> view) {
    return Containers::Array<SceneFieldData>{const_cast<SceneFieldData*>(view.data()), view.size(), Implementation::nonOwnedArrayDeleter};
}

SceneData::SceneData(const SceneMappingType mappingType, const UnsignedLong mappingBound, Containers::Array<char>&& data, Containers::Array<SceneFieldData>&& fields, const void* const importerState) noexcept: _dataFlags{DataFlag::Owned|DataFlag::Mutable}, _mappingType{mappingType}, _dimensions{}, _mappingBound{mappingBound}, _importerState{importerState}, _fields{std::move(fields)}, _data{std::move(data)} {
    /* Check that mapping type is large enough */
    CORRADE_ASSERT(
        (mappingType == SceneMappingType::UnsignedByte && mappingBound <= 0xffull) ||
        (mappingType == SceneMappingType::UnsignedShort && mappingBound <= 0xffffull) ||
        (mappingType == SceneMappingType::UnsignedInt && mappingBound <= 0xffffffffull) ||
        mappingType == SceneMappingType::UnsignedLong,
        "Trade::SceneData:" << mappingType << "is too small for" << mappingBound << "objects", );

    #ifndef CORRADE_NO_ASSERT
    /* Check various assumptions about field data */
    Math::BoolVector<12> fieldsPresent; /** @todo some constant for this */
    const UnsignedInt mappingTypeSize = sceneMappingTypeSize(_mappingType);
    #endif
    UnsignedInt transformationField = ~UnsignedInt{};
    UnsignedInt translationField = ~UnsignedInt{};
    UnsignedInt rotationField = ~UnsignedInt{};
    UnsignedInt scalingField = ~UnsignedInt{};
    #ifndef CORRADE_NO_ASSERT
    UnsignedInt meshField = ~UnsignedInt{};
    UnsignedInt meshMaterialField = ~UnsignedInt{};
    UnsignedInt skinField = ~UnsignedInt{};
    #endif
    for(std::size_t i = 0; i != _fields.size(); ++i) {
        const SceneFieldData& field = _fields[i];

        #ifndef CORRADE_NO_ASSERT
        /* The mapping type has to be the same among all fields. Technically it
           wouldn't need to be, but if there's 60k objects then using a 8bit
           type for certain fields would mean only the first 256 objects can be
           referenced, which makes no practical sense, and to improve the
           situation there would need to be some additional per-field object
           offset and ... it's simpler to just require the mapping type to be
           large enough to reference all objects (checked outside of the loop
           above) and that it's the same for all fields. This also makes it
           more convenient for the user. */
        CORRADE_ASSERT(field._mappingType == _mappingType,
            "Trade::SceneData: inconsistent mapping type, got" << field._mappingType << "for field" << i << "but expected" << _mappingType, );

        /* We could check that object indices are in bounds, but that's rather
           expensive. OTOH it's fine if field size is larger than object count,
           as a single object can have multiple instances of the same field
           attached, so checking that would be wrong. */

        /* Check that there are only unique fields. To avoid a O(n^2) operation
           always (or allocating a sorted field map), builtin fields are
           checked against a map and only custom fields are checked in an
           O(n^2) way with the assumption there isn't many of them (and that
           they'll gradually become builtin). */
        if(!isSceneFieldCustom(_fields[i]._name)) {
            CORRADE_INTERNAL_ASSERT(UnsignedInt(_fields[i]._name) < fieldsPresent.Size);
            CORRADE_ASSERT(!fieldsPresent[UnsignedInt(_fields[i]._name)],
                "Trade::SceneData: duplicate field" << _fields[i]._name, );
            fieldsPresent.set(UnsignedInt(_fields[i]._name), true);
        } else for(std::size_t j = 0; j != i; ++j) {
            CORRADE_ASSERT(_fields[j]._name != _fields[i]._name,
                "Trade::SceneData: duplicate field" << _fields[i]._name, );
        }

        /* Check that both the mapping and field view fits into the provided
           data array. If the field is empty, we don't check anything --
           accessing the memory would be invalid anyway and enforcing this
           would lead to unnecessary friction with optional fields. */
        if(field._size) {
            const UnsignedInt fieldTypeSize = sceneFieldTypeSize(field._fieldType)*
                (field._fieldArraySize ? field._fieldArraySize : 1);

            /* Both pointer and offset-only rely on basically same calculation,
               do it with offsets in a single place and only interpret as
               pointers in the non-offset-only check. Yes, yes, this may read
               the `pointer` union member through `offset`. */
            std::size_t mappingBegin = field._mappingData.offset;
            std::size_t fieldBegin = field._fieldData.offset;
            std::size_t mappingEnd = mappingBegin + (field._size - 1)*field._mappingStride;
            std::size_t fieldEnd = fieldBegin + (field._size - 1)*field._fieldStride;
            /* Flip for negative strides */
            if(mappingBegin > mappingEnd) std::swap(mappingBegin, mappingEnd);
            if(fieldBegin > fieldEnd) std::swap(fieldBegin, fieldEnd);
            /* Add the last element size to the higher address */
            mappingEnd += mappingTypeSize;
            fieldEnd += fieldTypeSize;

            if(field._flags & SceneFieldFlag::OffsetOnly) {
                CORRADE_ASSERT(mappingEnd <= _data.size(),
                    "Trade::SceneData: offset-only mapping data of field" << i << "span" << mappingEnd << "bytes but passed data array has only" << _data.size(), );
                CORRADE_ASSERT(fieldEnd <= _data.size(),
                    "Trade::SceneData: offset-only field data of field" << i << "span" << fieldEnd << "bytes but passed data array has only" << _data.size(), );
            } else {
                CORRADE_ASSERT(reinterpret_cast<const void*>(mappingBegin) >= _data.begin() && reinterpret_cast<const void*>(mappingEnd) <= _data.end(),
                    "Trade::SceneData: mapping data [" << Debug::nospace << reinterpret_cast<const void*>(mappingBegin) << Debug::nospace << ":" << Debug::nospace << reinterpret_cast<const void*>(mappingEnd) << Debug::nospace << "] of field" << i << "are not contained in passed data array [" << Debug::nospace << static_cast<const void*>(_data.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_data.end()) << Debug::nospace << "]", );
                CORRADE_ASSERT(reinterpret_cast<const void*>(fieldBegin) >= _data.begin() && reinterpret_cast<const void*>(fieldEnd) <= _data.end(),
                    "Trade::SceneData: field data [" << Debug::nospace << reinterpret_cast<const void*>(fieldBegin) << Debug::nospace << ":" << Debug::nospace << reinterpret_cast<const void*>(fieldEnd) << Debug::nospace << "] of field" << i << "are not contained in passed data array [" << Debug::nospace << static_cast<const void*>(_data.begin()) << Debug::nospace << ":" << Debug::nospace << static_cast<const void*>(_data.end()) << Debug::nospace << "]", );
            }
        }
        #endif

        /* Remember TRS and mesh/material fields to figure out whether the
           scene is 2D or 3D and check their object mapping consistency outside
           of the loop below */
        if(_fields[i]._name == SceneField::Transformation) {
            transformationField = i;
        } else if(_fields[i]._name == SceneField::Translation) {
            translationField = i;
        } else if(_fields[i]._name == SceneField::Rotation) {
            rotationField = i;
        } else if(_fields[i]._name == SceneField::Scaling) {
            scalingField = i;
        }
        #ifndef CORRADE_NO_ASSERT
        else if(_fields[i]._name == SceneField::Mesh) {
            meshField = i;
        } else if(_fields[i]._name == SceneField::MeshMaterial) {
            meshMaterialField = i;
        } else if(_fields[i]._name == SceneField::Skin) {
            skinField = i;
        }
        #endif
    }

    #ifndef CORRADE_NO_ASSERT
    /* Check that certain fields share the same object mapping. Printing as if
       all would be pointers (and not offset-only), it's not worth the extra
       effort just for an assert message. Also, compared to above, where
       "begin" was always zero, here we're always comparing four values, so the
       message for offset-only wouldn't be simpler either. */
    const auto checkFieldMappingDataMatch = [](const SceneFieldData& a, const SceneFieldData& b) {
        const std::size_t mappingTypeSize = sceneMappingTypeSize(a._mappingType);
        const void* const aBegin = a._mappingData.pointer;
        const void* const bBegin = b._mappingData.pointer;
        const void* const aEnd = static_cast<const char*>(a._mappingData.pointer) + a._size*mappingTypeSize;
        const void* const bEnd = static_cast<const char*>(b._mappingData.pointer) + b._size*mappingTypeSize;
        CORRADE_ASSERT(aBegin == bBegin && aEnd == bEnd,
            "Trade::SceneData:" << b._name << "mapping data [" << Debug::nospace << bBegin << Debug::nospace << ":" << Debug::nospace << bEnd << Debug::nospace << "] is different from" << a._name << "mapping data [" << Debug::nospace << aBegin << Debug::nospace << ":" << Debug::nospace << aEnd << Debug::nospace << "]", );
    };

    /* All present TRS fields should share the same object mapping */
    if(translationField != ~UnsignedInt{}) {
        if(rotationField != ~UnsignedInt{})
            checkFieldMappingDataMatch(_fields[translationField], _fields[rotationField]);
        if(scalingField != ~UnsignedInt{})
            checkFieldMappingDataMatch(_fields[translationField], _fields[scalingField]);
    }
    if(rotationField != ~UnsignedInt{} && scalingField != ~UnsignedInt{})
        checkFieldMappingDataMatch(_fields[rotationField], _fields[scalingField]);

    /* Mesh and materials also */
    if(meshField != ~UnsignedInt{} && meshMaterialField != ~UnsignedInt{})
        checkFieldMappingDataMatch(_fields[meshField], _fields[meshMaterialField]);
    #endif

    /* Decide about dimensionality based on transformation type, if present */
    if(transformationField != ~UnsignedInt{}) {
        const SceneFieldType fieldType = _fields[transformationField]._fieldType;
        if(fieldType == SceneFieldType::Matrix3x3 ||
           fieldType == SceneFieldType::Matrix3x3d ||
           fieldType == SceneFieldType::Matrix3x2 ||
           fieldType == SceneFieldType::Matrix3x2d ||
           fieldType == SceneFieldType::DualComplex ||
           fieldType == SceneFieldType::DualComplexd)
            _dimensions = 2;
        else if(fieldType == SceneFieldType::Matrix4x4 ||
                fieldType == SceneFieldType::Matrix4x4d ||
                fieldType == SceneFieldType::Matrix4x3 ||
                fieldType == SceneFieldType::Matrix4x3d ||
                fieldType == SceneFieldType::DualQuaternion ||
                fieldType == SceneFieldType::DualQuaterniond)
            _dimensions = 3;
        else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    /* Use TRS fields to decide about dimensionality, if the transformation
       field is not present. If it is, verify that they match it. */
    if(translationField != ~UnsignedInt{}) {
        const SceneFieldType fieldType = _fields[translationField]._fieldType;
        if(fieldType == SceneFieldType::Vector2 ||
           fieldType == SceneFieldType::Vector2d) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 2,
                "Trade::SceneData: expected a 3D translation field but got" << fieldType, );
            _dimensions = 2;
        } else if(fieldType == SceneFieldType::Vector3 ||
                  fieldType == SceneFieldType::Vector3d) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 3,
                "Trade::SceneData: expected a 2D translation field but got" << fieldType, );
            _dimensions = 3;
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
    if(rotationField != ~UnsignedInt{}) {
        const SceneFieldType fieldType = _fields[rotationField]._fieldType;
        if(fieldType == SceneFieldType::Complex ||
           fieldType == SceneFieldType::Complexd) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 2,
                "Trade::SceneData: expected a 3D rotation field but got" << fieldType, );
            _dimensions = 2;
        } else if(fieldType == SceneFieldType::Quaternion ||
                  fieldType == SceneFieldType::Quaterniond) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 3,
                "Trade::SceneData: expected a 2D rotation field but got" << fieldType, );
            _dimensions = 3;
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
    if(scalingField != ~UnsignedInt{}) {
        const SceneFieldType fieldType = _fields[scalingField]._fieldType;
        if(fieldType == SceneFieldType::Vector2 ||
           fieldType == SceneFieldType::Vector2d) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 2,
                "Trade::SceneData: expected a 3D scaling field but got" << fieldType, );
            _dimensions = 2;
        } else if(fieldType == SceneFieldType::Vector3 ||
                  fieldType == SceneFieldType::Vector3d) {
            CORRADE_ASSERT(!_dimensions || _dimensions == 3,
                "Trade::SceneData: expected a 2D scaling field but got" << fieldType, );
            _dimensions = 3;
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    /* A skin field requires some transformation field to exist in order to
       disambiguate between 2D and 3D */
    CORRADE_ASSERT(skinField == ~UnsignedInt{} || _dimensions,
        "Trade::SceneData: a skin field requires some transformation field to be present in order to disambiguate between 2D and 3D", );
}

SceneData::SceneData(const SceneMappingType mappingType, const UnsignedLong mappingBound, Containers::Array<char>&& data, const std::initializer_list<SceneFieldData> fields, const void* const importerState): SceneData{mappingType, mappingBound, std::move(data), Implementation::initializerListToArrayWithDefaultDeleter(fields), importerState} {}

SceneData::SceneData(const SceneMappingType mappingType, const UnsignedLong mappingBound, const DataFlags dataFlags, const Containers::ArrayView<const void> data, Containers::Array<SceneFieldData>&& fields, const void* const importerState) noexcept: SceneData{mappingType, mappingBound, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, std::move(fields), importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::SceneData: can't construct with non-owned data but" << dataFlags, );
    _dataFlags = dataFlags;
}

SceneData::SceneData(const SceneMappingType mappingType, const UnsignedLong mappingBound, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const std::initializer_list<SceneFieldData> fields, const void* const importerState): SceneData{mappingType, mappingBound, dataFlags, data, Implementation::initializerListToArrayWithDefaultDeleter(fields), importerState} {}

#ifdef MAGNUM_BUILD_DEPRECATED
SceneData::SceneData(std::vector<UnsignedInt> children2D, std::vector<UnsignedInt> children3D, const void* const importerState): _dataFlags{DataFlag::Owned|DataFlag::Mutable}, _mappingType {SceneMappingType::UnsignedInt}, _importerState{importerState} {
    /* Assume nobody ever created a scene with both 2D and 3D children.
       (PrimitiveImporter did, but that's an exception and blame goes on me.)
       If this blows up for you, please complain. Or rather upgrade to the new
       API which ... forces you to have a separate scene for 2D and 3D. But you
       can share the data among the two, at least. */
    CORRADE_ASSERT(children2D.empty() || children3D.empty(),
        "Trade::SceneData: it's no longer possible to have a scene with both 2D and 3D objects", );
    Containers::ArrayView<const UnsignedInt> children;
    if(!children2D.empty()) {
        _dimensions = 2;
        children = children2D;
    } else if(!children3D.empty()) {
        _dimensions = 3;
        children = children3D;
    } else _dimensions = 0;

    /* Set mapping bound to the max found child index. It's not great as it
       doesn't take any nested object into account but SceneData created this
       way is expected to be used only through the deprecated APIs anyway,
       which don't care about this value. */
    _mappingBound = children.isEmpty() ? 0 : Math::max(children) + 1;

    /* Convert the vector with top-level object IDs to the parent field, where
       all have -1 as a parent. This way the (also deprecated) children2D() /
       children3D() will return the desired values. */
    Containers::ArrayView<UnsignedInt> mapping;
    Containers::ArrayView<Int> parents;
    _data = Containers::ArrayTuple{
        {NoInit, children.size(), mapping},
        {NoInit, children.size(), parents},
    };
    /* Can't use InPlaceInit as that creates an Array with a non-default
       deleter, which then trips up on an assertion when such an instance gets
       returned from AbstractImporter */
    _fields = Containers::Array<SceneFieldData>{1};
    _fields[0] = SceneFieldData{SceneField::Parent, mapping, parents};
    Utility::copy(children, mapping);
    constexpr Int parent[]{-1};
    Utility::copy(Containers::stridedArrayView(parent).broadcasted<0>(parents.size()), parents);
}
#endif

SceneData::SceneData(SceneData&&) noexcept = default;

SceneData::~SceneData() = default;

SceneData& SceneData::operator=(SceneData&&) noexcept = default;

Containers::ArrayView<char> SceneData::mutableData() & {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::SceneData::mutableData(): data not mutable", {});
    return _data;
}

Containers::StridedArrayView1D<const void> SceneData::fieldDataMappingViewInternal(const SceneFieldData& field, const std::size_t offset, const std::size_t size) const {
    CORRADE_INTERNAL_ASSERT(offset + size <= field._size);
    return Containers::StridedArrayView1D<const void>{
        /* We're *sure* the view is correct, so faking the view size */
        {static_cast<const char*>(field._flags & SceneFieldFlag::OffsetOnly ?
            _data.data() + field._mappingData.offset : field._mappingData.pointer)
            + field._mappingStride*offset, ~std::size_t{}},
        size, field._mappingStride
    };
}

Containers::StridedArrayView1D<const void> SceneData::fieldDataMappingViewInternal(const SceneFieldData& field) const {
    return fieldDataMappingViewInternal(field, 0, field._size);
}

Containers::StridedArrayView1D<const void> SceneData::fieldDataFieldViewInternal(const SceneFieldData& field, const std::size_t offset, const std::size_t size) const {
    CORRADE_INTERNAL_ASSERT(offset + size <= field._size);
    return Containers::StridedArrayView1D<const void>{
        /* We're *sure* the view is correct, so faking the view size */
        {static_cast<const char*>(field._flags & SceneFieldFlag::OffsetOnly ?
            _data.data() + field._fieldData.offset : field._fieldData.pointer)
            + field._fieldStride*offset, ~std::size_t{}},
        size, field._fieldStride};
}

Containers::StridedArrayView1D<const void> SceneData::fieldDataFieldViewInternal(const SceneFieldData& field) const {
    return fieldDataFieldViewInternal(field, 0, field._size);
}

SceneFieldData SceneData::fieldData(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldData(): index" << id << "out of range for" << _fields.size() << "fields", SceneFieldData{});
    const SceneFieldData& field = _fields[id];
    return SceneFieldData{field._name, field._mappingType, fieldDataMappingViewInternal(field), field._fieldType, fieldDataFieldViewInternal(field), field._fieldArraySize, field._flags & ~SceneFieldFlag::OffsetOnly};
}

SceneField SceneData::fieldName(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldName(): index" << id << "out of range for" << _fields.size() << "fields", {});
    return _fields[id]._name;
}

SceneFieldFlags SceneData::fieldFlags(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldFlags(): index" << id << "out of range for" << _fields.size() << "fields", {});
    return _fields[id]._flags;
}

SceneFieldType SceneData::fieldType(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldType(): index" << id << "out of range for" << _fields.size() << "fields", {});
    return _fields[id]._fieldType;
}

std::size_t SceneData::fieldSize(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldSize(): index" << id << "out of range for" << _fields.size() << "fields", {});
    return _fields[id]._size;
}

UnsignedShort SceneData::fieldArraySize(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::fieldArraySize(): index" << id << "out of range for" << _fields.size() << "fields", {});
    return _fields[id]._fieldArraySize;
}

UnsignedInt SceneData::findFieldIdInternal(const SceneField name) const {
    for(std::size_t i = 0; i != _fields.size(); ++i)
        if(_fields[i]._name == name) return i;
    return ~UnsignedInt{};
}

Containers::Optional<UnsignedInt> SceneData::findFieldId(const SceneField name) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    return fieldId == ~UnsignedInt{} ? Containers::Optional<UnsignedInt>{} : fieldId;
}

UnsignedInt SceneData::fieldId(const SceneField name) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::fieldId(): field" << name << "not found", {});
    return fieldId;
}

bool SceneData::hasField(const SceneField name) const {
    return findFieldIdInternal(name) != ~UnsignedInt{};
}

namespace {

/* The `objects` view is already adjusted for `offset`, the offset is needed
   only to return the correct value for ImplicitMapping */
template<class T> std::size_t findObject(const SceneFieldFlags flags, const Containers::StridedArrayView1D<const void>& mapping, const std::size_t offset, const UnsignedLong object) {
    const std::size_t max = mapping.size();

    /* Implicit mapping, position equals object ID (if in bounds) and thus an
       O(1)-complexity search. A superset of OrderedMapping so has to be
       before. */
    if(flags >= SceneFieldFlag::ImplicitMapping)
        return object >= offset && object - offset < mapping.size() ?
            object - offset : max;

    const Containers::StridedArrayView1D<const T> mappingT = Containers::arrayCast<const T>(mapping);

    /* Ordered mapping, so an O(log n)-complexity search. It also has to be
       noted that STL algorithms generally suck (this needs a
       std::iterator_traits specialization for the StridedArrayView, FFS!),
       std::binary_search() is useless because it returns just a bool (!!) and
       std::lower_bound() is error prone beyond any reason. */
    if(flags >= SceneFieldFlag::OrderedMapping) {
        const Containers::StridedIterator<1, const T> found = std::lower_bound(mappingT.begin(), mappingT.end(), T(object));
        if(found == mappingT.end() || *found != object) return max;
        return found - mappingT.begin();
    }

    /* Generally unordered container, O(n)-complexity search. */
    for(std::size_t i = 0; i != max; ++i)
        if(mappingT[i] == object) return i;
    return max;
}

}

std::size_t SceneData::findFieldObjectOffsetInternal(const SceneFieldData& field, const UnsignedLong object, const std::size_t offset) const {
    const Containers::StridedArrayView1D<const void> mapping = fieldDataMappingViewInternal(field, offset, field._size - offset);
    if(field._mappingType == SceneMappingType::UnsignedInt)
        return offset + findObject<UnsignedInt>(field._flags, mapping, offset, object);
    else if(field._mappingType == SceneMappingType::UnsignedShort)
        return offset + findObject<UnsignedShort>(field._flags, mapping, offset, object);
    else if(field._mappingType == SceneMappingType::UnsignedByte)
        return offset + findObject<UnsignedByte>(field._flags, mapping, offset, object);
    else if(field._mappingType == SceneMappingType::UnsignedLong)
        return offset + findObject<UnsignedLong>(field._flags, mapping, offset, object);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Optional<std::size_t> SceneData::findFieldObjectOffset(const UnsignedInt fieldId, const UnsignedLong object, const std::size_t offset) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::findFieldObjectOffset(): object" << object << "out of bounds for" << _mappingBound << "objects", {});
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::findFieldObjectOffset(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});

    const SceneFieldData& field = _fields[fieldId];
    CORRADE_ASSERT(offset <= field._size,
        "Trade::SceneData::findFieldObjectOffset(): offset" << offset << "out of bounds for a field of size" << field._size, {});

    const std::size_t found = findFieldObjectOffsetInternal(field, object, offset);
    return found == field._size ? Containers::Optional<std::size_t>{} : found;
}

Containers::Optional<std::size_t> SceneData::findFieldObjectOffset(const SceneField fieldName, const UnsignedLong object, const std::size_t offset) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::findFieldObjectOffset(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    const UnsignedInt fieldId = findFieldIdInternal(fieldName);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::findFieldObjectOffset(): field" << fieldName << "not found", {});

    const SceneFieldData& field = _fields[fieldId];
    CORRADE_ASSERT(offset <= field._size,
        "Trade::SceneData::findFieldObjectOffset(): offset" << offset << "out of bounds for a field of size" << field._size, {});

    const std::size_t found = findFieldObjectOffsetInternal(field, object, offset);
    return found == field._size ? Containers::Optional<std::size_t>{} : found;
}

std::size_t SceneData::fieldObjectOffset(const UnsignedInt fieldId, const UnsignedLong object, const std::size_t offset) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::fieldObjectOffset(): object" << object << "out of bounds for" << _mappingBound << "objects", {});
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::fieldObjectOffset(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});

    const SceneFieldData& field = _fields[fieldId];
    CORRADE_ASSERT(offset <= field._size,
        "Trade::SceneData::fieldObjectOffset(): offset" << offset << "out of bounds for a field of size" << field._size, {});

    const std::size_t found = findFieldObjectOffsetInternal(field, object, offset);
    CORRADE_ASSERT(found != field._size,
        "Trade::SceneData::fieldObjectOffset(): object" << object << "not found in field" << field._name << "starting at offset" << offset, {});
    return found;
}

std::size_t SceneData::fieldObjectOffset(const SceneField fieldName, const UnsignedLong object, const std::size_t offset) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::fieldObjectOffset(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    const UnsignedInt fieldId = findFieldIdInternal(fieldName);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::fieldObjectOffset(): field" << fieldName << "not found", {});

    const SceneFieldData& field = _fields[fieldId];
    CORRADE_ASSERT(offset <= field._size,
        "Trade::SceneData::fieldObjectOffset(): offset" << offset << "out of bounds for a field of size" << field._size, {});

    const std::size_t found = findFieldObjectOffsetInternal(field, object, offset);
    CORRADE_ASSERT(found != field._size,
        "Trade::SceneData::fieldObjectOffset(): object" << object << "not found in field" << field._name << "starting at offset" << offset, {});
    return found;
}

bool SceneData::hasFieldObject(const UnsignedInt fieldId, const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::hasFieldObject(): object" << object << "out of bounds for" << _mappingBound << "objects", {});
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::hasFieldObject(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});

    const SceneFieldData& field = _fields[fieldId];
    return findFieldObjectOffsetInternal(field, object, 0) != field._size;
}

bool SceneData::hasFieldObject(const SceneField fieldName, const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::hasFieldObject(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    const UnsignedInt fieldId = findFieldIdInternal(fieldName);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::hasFieldObject(): field" << fieldName << "not found", {});

    const SceneFieldData& field = _fields[fieldId];
    return findFieldObjectOffsetInternal(field, object, 0) != field._size;
}

SceneFieldFlags SceneData::fieldFlags(const SceneField name) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::fieldFlags(): field" << name << "not found", {});
    return _fields[fieldId]._flags;
}

SceneFieldType SceneData::fieldType(const SceneField name) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::fieldType(): field" << name << "not found", {});
    return _fields[fieldId]._fieldType;
}

std::size_t SceneData::fieldSize(const SceneField name) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::fieldSize(): field" << name << "not found", {});
    return _fields[fieldId]._size;
}

UnsignedShort SceneData::fieldArraySize(const SceneField name) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::fieldArraySize(): field" << name << "not found", {});
    return _fields[fieldId]._fieldArraySize;
}

Containers::StridedArrayView2D<const char> SceneData::mapping(const UnsignedInt fieldId) const {
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::mapping(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});
    const SceneFieldData& field = _fields[fieldId];
    /* Build a 2D view using information about mapping type size */
    return Containers::arrayCast<2, const char>(
        fieldDataMappingViewInternal(field),
        sceneMappingTypeSize(field._mappingType));
}

Containers::StridedArrayView2D<char> SceneData::mutableMapping(const UnsignedInt fieldId) {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::SceneData::mutableMapping(): data not mutable", {});
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::mutableMapping(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});
    const SceneFieldData& field = _fields[fieldId];
    /* Build a 2D view using information about attribute type size */
    const auto out = Containers::arrayCast<2, const char>(
        fieldDataMappingViewInternal(field),
        sceneMappingTypeSize(field._mappingType));
    /** @todo some arrayConstCast? UGH */
    return Containers::StridedArrayView2D<char>{
        /* The view size is there only for a size assert, we're pretty sure the
           view is valid */
        {static_cast<char*>(const_cast<void*>(out.data())), ~std::size_t{}},
        out.size(), out.stride()};
}

Containers::StridedArrayView2D<const char> SceneData::mapping(const SceneField fieldName) const {
    const UnsignedInt fieldId = findFieldIdInternal(fieldName);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::mapping(): field" << fieldName << "not found", {});
    return mapping(fieldId);
}

Containers::StridedArrayView2D<char> SceneData::mutableMapping(const SceneField fieldName) {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::SceneData::mutableMapping(): data not mutable", {});
    const UnsignedInt fieldId = findFieldIdInternal(fieldName);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{}, "Trade::SceneData::mutableMapping(): field" << fieldName << "not found", {});
    return mutableMapping(fieldId);
}

Containers::StridedArrayView2D<const char> SceneData::field(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::field(): index" << id << "out of range for" << _fields.size() << "fields", {});
    const SceneFieldData& field = _fields[id];
    /* Build a 2D view using information about mapping type size */
    return Containers::arrayCast<2, const char>(
        fieldDataFieldViewInternal(field),
        sceneFieldTypeSize(field._fieldType)*(field._fieldArraySize ? field._fieldArraySize : 1));
}

Containers::StridedArrayView2D<char> SceneData::mutableField(const UnsignedInt id) {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::SceneData::mutableField(): data not mutable", {});
    CORRADE_ASSERT(id < _fields.size(),
        "Trade::SceneData::mutableField(): index" << id << "out of range for" << _fields.size() << "fields", {});
    const SceneFieldData& field = _fields[id];
    /* Build a 2D view using information about attribute type size */
    const auto out = Containers::arrayCast<2, const char>(
        fieldDataFieldViewInternal(field),
        sceneFieldTypeSize(field._fieldType)*(field._fieldArraySize ? field._fieldArraySize : 1));
    /** @todo some arrayConstCast? UGH */
    return Containers::StridedArrayView2D<char>{
        /* The view size is there only for a size assert, we're pretty sure the
           view is valid */
        {static_cast<char*>(const_cast<void*>(out.data())), ~std::size_t{}},
        out.size(), out.stride()};
}

Containers::StridedArrayView2D<const char> SceneData::field(const SceneField name) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::field(): field" << name << "not found", {});
    return field(fieldId);
}

Containers::StridedArrayView2D<char> SceneData::mutableField(const SceneField name) {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::SceneData::mutableField(): data not mutable", {});
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::mutableField(): field" << name << "not found", {});
    return mutableField(fieldId);
}

void SceneData::mappingIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    /* fieldId, offset and destination.size() is assumed to be in bounds,
       checked by the callers */

    const SceneFieldData& field = _fields[fieldId];
    const Containers::StridedArrayView1D<const void> mappingData = fieldDataMappingViewInternal(field, offset, destination.size());
    const auto destination1ui = Containers::arrayCast<2, UnsignedInt>(destination);

    if(field._mappingType == SceneMappingType::UnsignedInt)
        Utility::copy(Containers::arrayCast<const UnsignedInt>(mappingData), destination);
    else if(field._mappingType == SceneMappingType::UnsignedShort)
        Math::castInto(Containers::arrayCast<2, const UnsignedShort>(mappingData, 1), destination1ui);
    else if(field._mappingType == SceneMappingType::UnsignedByte)
        Math::castInto(Containers::arrayCast<2, const UnsignedByte>(mappingData, 1), destination1ui);
    else if(field._mappingType == SceneMappingType::UnsignedLong) {
        Math::castInto(Containers::arrayCast<2, const UnsignedLong>(mappingData, 1), destination1ui);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void SceneData::mappingInto(const UnsignedInt fieldId, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::mappingInto(): index" << fieldId << "out of range for" << _fields.size() << "fields", );
    CORRADE_ASSERT(destination.size() == _fields[fieldId]._size,
        "Trade::SceneData::mappingInto(): expected a view with" << _fields[fieldId]._size << "elements but got" << destination.size(), );
    mappingIntoInternal(fieldId, 0, destination);
}

std::size_t SceneData::mappingInto(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    CORRADE_ASSERT(fieldId < _fields.size(),
        "Trade::SceneData::mappingInto(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});
    CORRADE_ASSERT(offset <= _fields[fieldId]._size,
        "Trade::SceneData::mappingInto(): offset" << offset << "out of bounds for a field of size" << _fields[fieldId]._size, {});
    const std::size_t size = Math::min(destination.size(), std::size_t(_fields[fieldId]._size) - offset);
    mappingIntoInternal(fieldId, offset, destination.prefix(size));
    return size;
}

Containers::Array<UnsignedInt> SceneData::mappingAsArray(const UnsignedInt fieldId) const {
    CORRADE_ASSERT(fieldId < _fields.size(),
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::mappingInto(): index" << fieldId << "out of range for" << _fields.size() << "fields", {});
    Containers::Array<UnsignedInt> out{NoInit, std::size_t(_fields[fieldId]._size)};
    mappingIntoInternal(fieldId, 0, out);
    return out;
}

void SceneData::mappingInto(const SceneField name, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::mappingInto(): field" << name << "not found", );
    mappingInto(fieldId, destination);
}

std::size_t SceneData::mappingInto(const SceneField name, std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::mappingInto(): field" << name << "not found", {});
    return mappingInto(fieldId, offset, destination);
}

Containers::Array<UnsignedInt> SceneData::mappingAsArray(const SceneField name) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::mappingInto(): field" << name << "not found", {});
    return mappingAsArray(fieldId);
}

void SceneData::parentsIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<Int>& destination) const {
    /* fieldId, offset and destination.size() is assumed to be in bounds,
       checked by the callers */

    const SceneFieldData& field = _fields[fieldId];
    const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());
    const auto destination1i = Containers::arrayCast<2, Int>(destination);

    if(field._fieldType == SceneFieldType::Int)
        Utility::copy(Containers::arrayCast<const Int>(fieldData), destination);
    else if(field._fieldType == SceneFieldType::Short)
        Math::castInto(Containers::arrayCast<2, const Short>(fieldData, 1), destination1i);
    else if(field._fieldType == SceneFieldType::Byte)
        Math::castInto(Containers::arrayCast<2, const Byte>(fieldData, 1), destination1i);
    else if(field._fieldType == SceneFieldType::Long) {
        Math::castInto(Containers::arrayCast<2, const Long>(fieldData, 1), destination1i);
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void SceneData::parentsInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Int>& fieldDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Parent);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::parentsInto(): field not found", );
    CORRADE_ASSERT(!mappingDestination || mappingDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::parentsInto(): expected mapping destination view either empty or with" << _fields[fieldId]._size << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(!fieldDestination || fieldDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::parentsInto(): expected field destination view either empty or with" << _fields[fieldId]._size << "elements but got" << fieldDestination.size(), );
    mappingIntoInternal(fieldId, 0, mappingDestination);
    parentsIntoInternal(fieldId, 0, fieldDestination);
}

std::size_t SceneData::parentsInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Int>& fieldDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Parent);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::parentsInto(): field not found", {});
    const std::size_t fieldSize = _fields[fieldId]._size;
    CORRADE_ASSERT(offset <= fieldSize,
        "Trade::SceneData::parentsInto(): offset" << offset << "out of bounds for a field of size" << fieldSize, {});
    CORRADE_ASSERT(!mappingDestination != !fieldDestination|| mappingDestination.size() == fieldDestination.size(),
        "Trade::SceneData::parentsInto(): mapping and field destination views have different size," << mappingDestination.size() << "vs" << fieldDestination.size(), {});
    const std::size_t size = Math::min(Math::max(mappingDestination.size(), fieldDestination.size()), fieldSize - offset);
    if(mappingDestination) mappingIntoInternal(fieldId, offset, mappingDestination.prefix(size));
    if(fieldDestination) parentsIntoInternal(fieldId, offset, fieldDestination.prefix(size));
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, Int>> SceneData::parentsAsArray() const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Parent);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::parentsInto(): field not found", {});
    Containers::Array<Containers::Pair<UnsignedInt, Int>> out{NoInit, std::size_t(_fields[fieldId]._size)};
    /* Explicit slice() template parameters needed by GCC 4.8 and MSVC 2015 */
    mappingIntoInternal(fieldId, 0, stridedArrayView(out).slice<UnsignedInt>(&decltype(out)::Type::first));
    parentsIntoInternal(fieldId, 0, stridedArrayView(out).slice<Int>(&decltype(out)::Type::second));
    return out;
}

namespace {

template<class Source, class Destination> void convertTransformation(const Containers::StridedArrayView1D<const void>& source, const Containers::StridedArrayView1D<Destination>& destination) {
    CORRADE_INTERNAL_ASSERT(source.size() == destination.size());
    const auto sourceT = Containers::arrayCast<const Source>(source);
    for(std::size_t i = 0; i != sourceT.size(); ++i)
        destination[i] = Destination{sourceT[i].toMatrix()};
}

/* Expands 2x3 to 3x3, 4x3 to 4x4, while potentially also converting floats to
   doubles */
/** @todo also have some optimized batched API for this? doing a strided copy
    and a fill seemed silly as that would cause the whole memory go through
    cache twice */
template<class Source, class Destination> void expandTransformationMatrix(const Containers::StridedArrayView1D<const void>& source, const Containers::StridedArrayView1D<Destination>& destination) {
    CORRADE_INTERNAL_ASSERT(source.size() == destination.size());
    const auto sourceT = Containers::arrayCast<const Source>(source);
    for(std::size_t i = 0; i != sourceT.size(); ++i)
        destination[i] = Destination{Math::RectangularMatrix<Source::Cols, Source::Rows, Float>{sourceT[i]}};
}

/** @todo these (or the float variants at least) should eventually be replaced
    with optimized batched APIs (applyTranslationsInto() updating just the
    last matrix column etc.) */

template<class Source, class Destination> void applyTranslation(const Containers::StridedArrayView1D<const void>& source, const Containers::StridedArrayView1D<Destination>& destination) {
    CORRADE_INTERNAL_ASSERT(source.size() == destination.size());
    const auto sourceT = Containers::arrayCast<const Source>(source);
    for(std::size_t i = 0; i != sourceT.size(); ++i)
        destination[i] = Destination::translation(Math::Vector<Destination::Size - 1, Float>{sourceT[i]})*destination[i];
}

template<class Source, class Destination> void applyRotation(const Containers::StridedArrayView1D<const void>& source, const Containers::StridedArrayView1D<Destination>& destination) {
    CORRADE_INTERNAL_ASSERT(source.size() == destination.size());
    const auto sourceT = Containers::arrayCast<const Source>(source);
    for(std::size_t i = 0; i != sourceT.size(); ++i)
        destination[i] = Destination{Math::Matrix<Destination::Size - 1, Float>{ sourceT[i].toMatrix()}}*destination[i];
}

template<class Source, class Destination> void applyScaling(const Containers::StridedArrayView1D<const void>& source, const Containers::StridedArrayView1D<Destination>& destination) {
    CORRADE_INTERNAL_ASSERT(source.size() == destination.size());
    const auto sourceT = Containers::arrayCast<const Source>(source);
    for(std::size_t i = 0; i != sourceT.size(); ++i)
        destination[i] = Destination::scaling(Math::Vector<Destination::Size - 1, Float>{sourceT[i]})*destination[i];
}

}

UnsignedInt SceneData::findTransformationFields(UnsignedInt& transformationFieldId, UnsignedInt& translationFieldId, UnsignedInt& rotationFieldId, UnsignedInt& scalingFieldId) const {
    UnsignedInt fieldWithObjectMapping = ~UnsignedInt{};
    transformationFieldId = ~UnsignedInt{};
    translationFieldId = ~UnsignedInt{};
    rotationFieldId = ~UnsignedInt{};
    scalingFieldId = ~UnsignedInt{};
    for(std::size_t i = 0; i != _fields.size(); ++i) {
        /* If we find a transformation field, we don't need to look any
           further */
        if(_fields[i]._name == SceneField::Transformation) {
            fieldWithObjectMapping = transformationFieldId = i;
            break;
        } else if(_fields[i]._name == SceneField::Translation) {
            fieldWithObjectMapping = translationFieldId = i;
        } else if(_fields[i]._name == SceneField::Rotation) {
            fieldWithObjectMapping = rotationFieldId = i;
        } else if(_fields[i]._name == SceneField::Scaling) {
            fieldWithObjectMapping = scalingFieldId = i;
        }
    }

    return fieldWithObjectMapping;
}

UnsignedInt SceneData::findTranslationRotationScalingFields(UnsignedInt& translationFieldId, UnsignedInt& rotationFieldId, UnsignedInt& scalingFieldId) const {
    UnsignedInt fieldWithObjectMapping = ~UnsignedInt{};
    translationFieldId = ~UnsignedInt{};
    rotationFieldId = ~UnsignedInt{};
    scalingFieldId = ~UnsignedInt{};
    for(std::size_t i = 0; i != _fields.size(); ++i) {
        if(_fields[i]._name == SceneField::Translation) {
            fieldWithObjectMapping = translationFieldId = i;
        } else if(_fields[i]._name == SceneField::Rotation) {
            fieldWithObjectMapping = rotationFieldId = i;
        } else if(_fields[i]._name == SceneField::Scaling) {
            fieldWithObjectMapping = scalingFieldId = i;
        }
    }

    return fieldWithObjectMapping;
}

std::size_t SceneData::transformationFieldSize() const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTransformationFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        "Trade::SceneData::transformationFieldSize(): no transformation-related field found", {});
    return _fields[fieldWithObjectMapping]._size;
}

void SceneData::transformations2DIntoInternal(const UnsignedInt transformationFieldId, const UnsignedInt translationFieldId, const UnsignedInt rotationFieldId, const UnsignedInt scalingFieldId, std::size_t offset, const Containers::StridedArrayView1D<Matrix3>& destination) const {
    /* *FieldId, offset and destination.size() is assumed to be in bounds (or
       an invalid field ID), checked by the callers */

    /* If is2D() returned false as well, all *FieldId would be invalid, which
       the caller is assumed to check. */
    CORRADE_ASSERT(!is3D(), "Trade::SceneData::transformations2DInto(): scene has a 3D transformation type", );

    /** @todo apply scalings as well if dual complex? */

    /* Prefer the transformation field, if present */
    if(transformationFieldId != ~UnsignedInt{}) {
        const SceneFieldData& field = _fields[transformationFieldId];
        const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());
        const auto destination1f = Containers::arrayCast<2, Float>(destination);

        if(field._fieldType == SceneFieldType::Matrix3x3) {
            Utility::copy(Containers::arrayCast<const Matrix3>(fieldData), destination);
        } else if(field._fieldType == SceneFieldType::Matrix3x3d) {
            Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 9), destination1f);
        } else if(field._fieldType == SceneFieldType::Matrix3x2) {
            expandTransformationMatrix<Matrix3x2>(fieldData, destination);
        } else if(field._fieldType == SceneFieldType::Matrix3x2d) {
            expandTransformationMatrix<Matrix3x2d>(fieldData, destination);
        } else if(field._fieldType == SceneFieldType::DualComplex) {
            convertTransformation<DualComplex>(fieldData, destination);
        } else if(field._fieldType == SceneFieldType::DualComplexd) {
            convertTransformation<DualComplexd>(fieldData, destination);
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* If not, combine from TRS components */
    } else if(translationFieldId != ~UnsignedInt{} || rotationFieldId != ~UnsignedInt{} || scalingFieldId != ~UnsignedInt{}) {
        /* First fill the destination with identity matrices */
        const Matrix3 identity[1]{Matrix3{Math::IdentityInit}};
        Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(destination.size()), destination);

        /* Apply scaling first, if present */
        if(scalingFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[scalingFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Vector2) {
                applyScaling<Vector2>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Vector2d) {
                applyScaling<Vector2d>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        /* Apply rotation second, if present */
        if(rotationFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[rotationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Complex) {
                applyRotation<Complex>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Complexd) {
                applyRotation<Complexd>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        /* Apply translation last, if present */
        if(translationFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[translationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Vector2) {
                applyTranslation<Vector2>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Vector2d) {
                applyTranslation<Vector2d>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

    /* Checked in the caller */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void SceneData::transformations2DInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix3>& fieldDestination) const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTransformationFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        "Trade::SceneData::transformations2DInto(): no transformation-related field found", );
    CORRADE_ASSERT(!mappingDestination || mappingDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::transformations2DInto(): expected mapping destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(!fieldDestination || fieldDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::transformations2DInto(): expected field destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << fieldDestination.size(), );
    mappingIntoInternal(fieldWithObjectMapping, 0, mappingDestination);
    transformations2DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, 0, fieldDestination);
}

std::size_t SceneData::transformations2DInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix3>& fieldDestination) const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTransformationFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        "Trade::SceneData::transformations2DInto(): no transformation-related field found", {});
    const std::size_t fieldSize = _fields[fieldWithObjectMapping]._size;
    CORRADE_ASSERT(offset <= fieldSize,
        "Trade::SceneData::transformations2DInto(): offset" << offset << "out of bounds for a field of size" << fieldSize, {});
    CORRADE_ASSERT(!mappingDestination != !fieldDestination|| mappingDestination.size() == fieldDestination.size(),
        "Trade::SceneData::transformations2DInto(): mapping and field destination views have different size," << mappingDestination.size() << "vs" << fieldDestination.size(), {});
    const std::size_t size = Math::min(Math::max(mappingDestination.size(), fieldDestination.size()), fieldSize - offset);
    if(mappingDestination) mappingIntoInternal(fieldWithObjectMapping, offset, mappingDestination.prefix(size));
    if(fieldDestination) transformations2DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, offset, fieldDestination.prefix(size));
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, Matrix3>> SceneData::transformations2DAsArray() const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTransformationFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::transformations2DInto(): no transformation-related field found", {});
    Containers::Array<Containers::Pair<UnsignedInt, Matrix3>> out{NoInit, std::size_t(_fields[fieldWithObjectMapping]._size)};
    /* Explicit slice() template parameters needed by GCC 4.8 and MSVC 2015 */
    mappingIntoInternal(fieldWithObjectMapping, 0, stridedArrayView(out).slice<UnsignedInt>(&decltype(out)::Type::first));
    transformations2DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, 0, stridedArrayView(out).slice<Matrix3>(&decltype(out)::Type::second));
    return out;
}

void SceneData::translationsRotationsScalings2DIntoInternal(const UnsignedInt translationFieldId, const UnsignedInt rotationFieldId, const UnsignedInt scalingFieldId, const std::size_t offset, const Containers::StridedArrayView1D<Vector2>& translationDestination, const Containers::StridedArrayView1D<Complex>& rotationDestination, const Containers::StridedArrayView1D<Vector2>& scalingDestination) const {
    /* *FieldId, offset and *Destination.size() is assumed to be in bounds (or
       an invalid field ID), checked by the callers */

    /* If is2D() returned false as well, all *FieldId would be invalid, which
       the caller is assumed to check. */
    CORRADE_ASSERT(!is3D(), "Trade::SceneData::translationsRotationsScalings2DInto(): scene has a 3D transformation type", );

    /* Retrieve translation, if desired. If no field is present, output a zero
       vector for all objects. */
    if(translationDestination) {
        if(translationFieldId == ~UnsignedInt{}) {
            constexpr Vector2 identity[]{Vector2{0.0f}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(translationDestination.size()), translationDestination);
        } else {
            const SceneFieldData& field = _fields[translationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, translationDestination.size());

            if(field._fieldType == SceneFieldType::Vector2) {
                Utility::copy(Containers::arrayCast<const Vector2>(fieldData), translationDestination);
            } else if(field._fieldType == SceneFieldType::Vector2d) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 2), Containers::arrayCast<2, Float>(translationDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }

    /* Retrieve rotation, if desired. If no field is present, output an
       identity rotation for all objects. */
    if(rotationDestination) {
        if(rotationFieldId == ~UnsignedInt{}) {
            constexpr Complex identity[]{Complex{Math::IdentityInit}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(rotationDestination.size()), rotationDestination);
        } else {
            const SceneFieldData& field = _fields[rotationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, rotationDestination.size());

            if(field._fieldType == SceneFieldType::Complex) {
                Utility::copy(Containers::arrayCast<const Complex>(fieldData), rotationDestination);
            } else if(field._fieldType == SceneFieldType::Complexd) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 2), Containers::arrayCast<2, Float>(rotationDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }

    /* Retrieve scaling, if desired. If no field is present, output an identity
       scaling for all objects. */
    if(scalingDestination) {
        if(scalingFieldId == ~UnsignedInt{}) {
            constexpr Vector2 identity[]{Vector2{1.0f}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(scalingDestination.size()), scalingDestination);
        } else {
            const SceneFieldData& field = _fields[scalingFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, scalingDestination.size());

            if(field._fieldType == SceneFieldType::Vector2) {
                Utility::copy(Containers::arrayCast<const Vector2>(fieldData), scalingDestination);
            } else if(field._fieldType == SceneFieldType::Vector2d) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 2), Containers::arrayCast<2, Float>(scalingDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }
}

void SceneData::translationsRotationsScalings2DInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Vector2>& translationDestination, const Containers::StridedArrayView1D<Complex>& rotationDestination, const Containers::StridedArrayView1D<Vector2>& scalingDestination) const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        "Trade::SceneData::translationsRotationsScalings2DInto(): no transformation-related field found", );
    CORRADE_ASSERT(!mappingDestination || mappingDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected mapping destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(!translationDestination || translationDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected translation destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << translationDestination.size(), );
    CORRADE_ASSERT(!rotationDestination || rotationDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected rotation destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << rotationDestination.size(), );
    CORRADE_ASSERT(!scalingDestination || scalingDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected scaling destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << scalingDestination.size(), );
    mappingIntoInternal(fieldWithObjectMapping, 0, mappingDestination);
    translationsRotationsScalings2DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, 0, translationDestination, rotationDestination, scalingDestination);
}

std::size_t SceneData::translationsRotationsScalings2DInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Vector2>& translationDestination, const Containers::StridedArrayView1D<Complex>& rotationDestination, const Containers::StridedArrayView1D<Vector2>& scalingDestination) const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        "Trade::SceneData::translationsRotationsScalings2DInto(): no transformation-related field found", {});
    const std::size_t fieldSize = _fields[fieldWithObjectMapping]._size;
    CORRADE_ASSERT(offset <= fieldSize,
        "Trade::SceneData::translationsRotationsScalings2DInto(): offset" << offset << "out of bounds for a field of size" << fieldSize, {});
    CORRADE_ASSERT(!mappingDestination != !translationDestination || mappingDestination.size() == translationDestination.size(),
        "Trade::SceneData::translationsRotationsScalings2DInto(): mapping and translation destination views have different size," << mappingDestination.size() << "vs" << translationDestination.size(), {});
    CORRADE_ASSERT(!mappingDestination != !rotationDestination || mappingDestination.size() == rotationDestination.size(),
        "Trade::SceneData::translationsRotationsScalings2DInto(): mapping and rotation destination views have different size," << mappingDestination.size() << "vs" << rotationDestination.size(), {});
    CORRADE_ASSERT(!mappingDestination != !scalingDestination || mappingDestination.size() == scalingDestination.size(),
        "Trade::SceneData::translationsRotationsScalings2DInto(): mapping and scaling destination views have different size," << mappingDestination.size() << "vs" << scalingDestination.size(), {});
    CORRADE_ASSERT(!translationDestination != !rotationDestination || translationDestination.size() == rotationDestination.size(),
        "Trade::SceneData::translationsRotationsScalings2DInto(): translation and rotation destination views have different size," << translationDestination.size() << "vs" << rotationDestination.size(), {});
    CORRADE_ASSERT(!translationDestination != !scalingDestination || translationDestination.size() == scalingDestination.size(),
        "Trade::SceneData::translationsRotationsScalings2DInto(): translation and scaling destination views have different size," << translationDestination.size() << "vs" << scalingDestination.size(), {});
    CORRADE_ASSERT(!rotationDestination != !scalingDestination || rotationDestination.size() == scalingDestination.size(),
        "Trade::SceneData::translationsRotationsScalings2DInto(): rotation and scaling destination views have different size," << rotationDestination.size() << "vs" << scalingDestination.size(), {});
    const std::size_t size = Math::min(Math::max({mappingDestination.size(), translationDestination.size(), rotationDestination.size(), scalingDestination.size()}), fieldSize - offset);
    if(mappingDestination) mappingIntoInternal(fieldWithObjectMapping, offset, mappingDestination.prefix(size));
    translationsRotationsScalings2DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, offset,
        translationDestination ? translationDestination.prefix(size) : nullptr,
        rotationDestination ? rotationDestination.prefix(size) : nullptr,
        scalingDestination ? scalingDestination.prefix(size) : nullptr);
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, Containers::Triple<Vector2, Complex, Vector2>>> SceneData::translationsRotationsScalings2DAsArray() const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::translationsRotationsScalings2DInto(): no transformation-related field found", {});
    Containers::Array<Containers::Pair<UnsignedInt, Containers::Triple<Vector2, Complex, Vector2>>> out{NoInit, std::size_t(_fields[fieldWithObjectMapping]._size)};
    /* Explicit slice() template parameters needed by GCC 4.8 and MSVC 2015 */
    mappingIntoInternal(fieldWithObjectMapping, 0, stridedArrayView(out).slice<UnsignedInt>(&decltype(out)::Type::first));
    const auto outSecond = stridedArrayView(out).slice<Containers::Triple<Vector2, Complex, Vector2>>(&decltype(out)::Type::second);
    translationsRotationsScalings2DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, 0,
        outSecond.slice<Vector2>(&decltype(outSecond)::Type::first),
        outSecond.slice<Complex>(&decltype(outSecond)::Type::second),
        outSecond.slice<Vector2>(&decltype(outSecond)::Type::third));
    return out;
}

void SceneData::transformations3DIntoInternal(const UnsignedInt transformationFieldId, const UnsignedInt translationFieldId, const UnsignedInt rotationFieldId, const UnsignedInt scalingFieldId, const std::size_t offset, const Containers::StridedArrayView1D<Matrix4>& destination) const {
    /* *FieldId, offset and destination.size() is assumed to be in bounds (or
       an invalid field ID), checked by the callers */

    /* If is3D() returned false as well, all *FieldId would be invalid, which
       the caller is assumed to check. */
    CORRADE_ASSERT(!is2D(), "Trade::SceneData::transformations3DInto(): scene has a 2D transformation type", );

    /** @todo apply scalings as well if dual quat? */

    /* Prefer the transformation field, if present */
    if(transformationFieldId != ~UnsignedInt{}) {
        const SceneFieldData& field = _fields[transformationFieldId];
        const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());
        const auto destination1f = Containers::arrayCast<2, Float>(destination);

        if(field._fieldType == SceneFieldType::Matrix4x4) {
            Utility::copy(Containers::arrayCast<const Matrix4>(fieldData), destination);
        } else if(field._fieldType == SceneFieldType::Matrix4x4d) {
            Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 16), destination1f);
        } else if(field._fieldType == SceneFieldType::Matrix4x3) {
            expandTransformationMatrix<Matrix4x3>(fieldData, destination);
        } else if(field._fieldType == SceneFieldType::Matrix4x3d) {
            expandTransformationMatrix<Matrix4x3d>(fieldData, destination);
        } else if(field._fieldType == SceneFieldType::DualQuaternion) {
            convertTransformation<DualQuaternion>(fieldData, destination);
        } else if(field._fieldType == SceneFieldType::DualQuaterniond) {
            convertTransformation<DualQuaterniond>(fieldData, destination);
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* If not, combine from TRS components */
    } else if(translationFieldId != ~UnsignedInt{} || rotationFieldId != ~UnsignedInt{} || scalingFieldId != ~UnsignedInt{}) {
        /* First fill the destination with identity matrices */
        const Matrix4 identity[1]{Matrix4{Math::IdentityInit}};
        Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(destination.size()), destination);

        /* Apply scaling first, if present */
        if(scalingFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[scalingFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Vector3) {
                applyScaling<Vector3>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Vector3d) {
                applyScaling<Vector3d>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        /* Apply rotation second, if present */
        if(rotationFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[rotationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Quaternion) {
                applyRotation<Quaternion>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Quaterniond) {
                applyRotation<Quaterniond>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        /* Apply translation last, if present */
        if(translationFieldId != ~UnsignedInt{}) {
            const SceneFieldData& field = _fields[translationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());

            if(field._fieldType == SceneFieldType::Vector3) {
                applyTranslation<Vector3>(fieldData, destination);
            } else if(field._fieldType == SceneFieldType::Vector3d) {
                applyTranslation<Vector3d>(fieldData, destination);
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

    /* Checked in the caller */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void SceneData::transformations3DInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix4>& fieldDestination) const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const std::size_t fieldWithObjectMapping = findTransformationFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        "Trade::SceneData::transformations3DInto(): no transformation-related field found", );
    CORRADE_ASSERT(!mappingDestination || mappingDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::transformations3DInto(): expected mapping destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(!fieldDestination || fieldDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::transformations3DInto(): expected field destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << fieldDestination.size(), );
    mappingIntoInternal(fieldWithObjectMapping, 0, mappingDestination);
    transformations3DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, 0, fieldDestination);
}

std::size_t SceneData::transformations3DInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix4>& fieldDestination) const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTransformationFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        "Trade::SceneData::transformations3DInto(): no transformation-related field found", {});
    const std::size_t fieldSize = _fields[fieldWithObjectMapping]._size;
    CORRADE_ASSERT(offset <= fieldSize,
        "Trade::SceneData::transformations3DInto(): offset" << offset << "out of bounds for a field of size" << fieldSize, {});
    CORRADE_ASSERT(!mappingDestination != !fieldDestination|| mappingDestination.size() == fieldDestination.size(),
        "Trade::SceneData::transformations3DInto(): mapping and field destination views have different size," << mappingDestination.size() << "vs" << fieldDestination.size(), {});
    const std::size_t size = Math::min(Math::max(mappingDestination.size(), fieldDestination.size()), fieldSize - offset);
    if(mappingDestination) mappingIntoInternal(fieldWithObjectMapping, offset, mappingDestination.prefix(size));
    if(fieldDestination) transformations3DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, offset, fieldDestination.prefix(size));
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, Matrix4>> SceneData::transformations3DAsArray() const {
    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTransformationFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::transformations3DInto(): no transformation-related field found", {});
    Containers::Array<Containers::Pair<UnsignedInt, Matrix4>> out{NoInit, std::size_t(_fields[fieldWithObjectMapping]._size)};
    /* Explicit slice() template parameters needed by GCC 4.8 and MSVC 2015 */
    mappingIntoInternal(fieldWithObjectMapping, 0, stridedArrayView(out).slice<UnsignedInt>(&decltype(out)::Type::first));
    transformations3DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, 0, stridedArrayView(out).slice<Matrix4>(&decltype(out)::Type::second));
    return out;
}

void SceneData::translationsRotationsScalings3DIntoInternal(const UnsignedInt translationFieldId, const UnsignedInt rotationFieldId, const UnsignedInt scalingFieldId, const std::size_t offset, const Containers::StridedArrayView1D<Vector3>& translationDestination, const Containers::StridedArrayView1D<Quaternion>& rotationDestination, const Containers::StridedArrayView1D<Vector3>& scalingDestination) const {
    /* *FieldId, offset and *Destination.size() is assumed to be in bounds (or
       an invalid field ID), checked by the callers */

    /* If is3D() returned false as well, all *FieldId would be invalid, which
       the caller is assumed to check. */
    CORRADE_ASSERT(!is2D(), "Trade::SceneData::translationsRotationsScalings3DInto(): scene has a 2D transformation type", );

    /* Retrieve translation, if desired. If no field is present, output a zero
       vector for all objects. */
    if(translationDestination) {
        if(translationFieldId == ~UnsignedInt{}) {
            constexpr Vector3 identity[]{Vector3{0.0f}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(translationDestination.size()), translationDestination);
        } else {
            const SceneFieldData& field = _fields[translationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, translationDestination.size());

            if(field._fieldType == SceneFieldType::Vector3) {
                Utility::copy(Containers::arrayCast<const Vector3>(fieldData), translationDestination);
            } else if(field._fieldType == SceneFieldType::Vector3d) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 3), Containers::arrayCast<2, Float>(translationDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }

    /* Retrieve rotation, if desired. If no field is present, output an
       identity rotation for all objects. */
    if(rotationDestination) {
        if(rotationFieldId == ~UnsignedInt{}) {
            constexpr Quaternion identity[]{Quaternion{Math::IdentityInit}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(rotationDestination.size()), rotationDestination);
        } else {
            const SceneFieldData& field = _fields[rotationFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, rotationDestination.size());

            if(field._fieldType == SceneFieldType::Quaternion) {
                Utility::copy(Containers::arrayCast<const Quaternion>(fieldData), rotationDestination);
            } else if(field._fieldType == SceneFieldType::Quaterniond) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 4), Containers::arrayCast<2, Float>(rotationDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }

    /* Retrieve scaling, if desired. If no field is present, output an identity
       scaling for all objects. */
    if(scalingDestination) {
        if(scalingFieldId == ~UnsignedInt{}) {
            constexpr Vector3 identity[]{Vector3{1.0f}};
            Utility::copy(Containers::stridedArrayView(identity).broadcasted<0>(scalingDestination.size()), scalingDestination);
        } else {
            const SceneFieldData& field = _fields[scalingFieldId];
            const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, scalingDestination.size());

            if(field._fieldType == SceneFieldType::Vector3) {
                Utility::copy(Containers::arrayCast<const Vector3>(fieldData), scalingDestination);
            } else if(field._fieldType == SceneFieldType::Vector3d) {
                Math::castInto(Containers::arrayCast<2, const Double>(fieldData, 3), Containers::arrayCast<2, Float>(scalingDestination));
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }
}

void SceneData::translationsRotationsScalings3DInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Vector3>& translationDestination, const Containers::StridedArrayView1D<Quaternion>& rotationDestination, const Containers::StridedArrayView1D<Vector3>& scalingDestination) const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        "Trade::SceneData::translationsRotationsScalings3DInto(): no transformation-related field found", );
    CORRADE_ASSERT(!mappingDestination || mappingDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected mapping destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(!translationDestination || translationDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected translation destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << translationDestination.size(), );
    CORRADE_ASSERT(!rotationDestination || rotationDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected rotation destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << rotationDestination.size(), );
    CORRADE_ASSERT(!scalingDestination || scalingDestination.size() == _fields[fieldWithObjectMapping]._size,
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected scaling destination view either empty or with" << _fields[fieldWithObjectMapping]._size << "elements but got" << scalingDestination.size(), );
    mappingIntoInternal(fieldWithObjectMapping, 0, mappingDestination);
    translationsRotationsScalings3DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, 0, translationDestination, rotationDestination, scalingDestination);
}

std::size_t SceneData::translationsRotationsScalings3DInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Vector3>& translationDestination, const Containers::StridedArrayView1D<Quaternion>& rotationDestination, const Containers::StridedArrayView1D<Vector3>& scalingDestination) const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        "Trade::SceneData::translationsRotationsScalings3DInto(): no transformation-related field found", {});
    const std::size_t fieldSize = _fields[fieldWithObjectMapping]._size;
    CORRADE_ASSERT(offset <= fieldSize,
        "Trade::SceneData::translationsRotationsScalings3DInto(): offset" << offset << "out of bounds for a field of size" << fieldSize, {});
    CORRADE_ASSERT(!mappingDestination != !translationDestination || mappingDestination.size() == translationDestination.size(),
        "Trade::SceneData::translationsRotationsScalings3DInto(): mapping and translation destination views have different size," << mappingDestination.size() << "vs" << translationDestination.size(), {});
    CORRADE_ASSERT(!mappingDestination != !rotationDestination || mappingDestination.size() == rotationDestination.size(),
        "Trade::SceneData::translationsRotationsScalings3DInto(): mapping and rotation destination views have different size," << mappingDestination.size() << "vs" << rotationDestination.size(), {});
    CORRADE_ASSERT(!mappingDestination != !scalingDestination || mappingDestination.size() == scalingDestination.size(),
        "Trade::SceneData::translationsRotationsScalings3DInto(): mapping and scaling destination views have different size," << mappingDestination.size() << "vs" << scalingDestination.size(), {});
    CORRADE_ASSERT(!translationDestination != !rotationDestination || translationDestination.size() == rotationDestination.size(),
        "Trade::SceneData::translationsRotationsScalings3DInto(): translation and rotation destination views have different size," << translationDestination.size() << "vs" << rotationDestination.size(), {});
    CORRADE_ASSERT(!translationDestination != !scalingDestination || translationDestination.size() == scalingDestination.size(),
        "Trade::SceneData::translationsRotationsScalings3DInto(): translation and scaling destination views have different size," << translationDestination.size() << "vs" << scalingDestination.size(), {});
    CORRADE_ASSERT(!rotationDestination != !scalingDestination || rotationDestination.size() == scalingDestination.size(),
        "Trade::SceneData::translationsRotationsScalings3DInto(): rotation and scaling destination views have different size," << rotationDestination.size() << "vs" << scalingDestination.size(), {});
    const std::size_t size = Math::min(Math::max({mappingDestination.size(), translationDestination.size(), rotationDestination.size(), scalingDestination.size()}), fieldSize - offset);
    if(mappingDestination) mappingIntoInternal(fieldWithObjectMapping, offset, mappingDestination.prefix(size));
    translationsRotationsScalings3DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, offset,
        translationDestination ? translationDestination.prefix(size) : nullptr,
        rotationDestination ? rotationDestination.prefix(size) : nullptr,
        scalingDestination ? scalingDestination.prefix(size) : nullptr);
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, Containers::Triple<Vector3, Quaternion, Vector3>>> SceneData::translationsRotationsScalings3DAsArray() const {
    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    CORRADE_ASSERT(fieldWithObjectMapping != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::translationsRotationsScalings3DInto(): no transformation-related field found", {});
    Containers::Array<Containers::Pair<UnsignedInt, Containers::Triple<Vector3, Quaternion, Vector3>>> out{NoInit, std::size_t(_fields[fieldWithObjectMapping]._size)};
    /* Explicit slice() template parameters needed by GCC 4.8 and MSVC 2015 */
    mappingIntoInternal(fieldWithObjectMapping, 0, stridedArrayView(out).slice<UnsignedInt>(&decltype(out)::Type::first));
    const auto outSecond = stridedArrayView(out).slice<Containers::Triple<Vector3, Quaternion, Vector3>>(&decltype(out)::Type::second);
    translationsRotationsScalings3DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, 0,
        outSecond.slice<Vector3>(&decltype(outSecond)::Type::first),
        outSecond.slice<Quaternion>(&decltype(outSecond)::Type::second),
        outSecond.slice<Vector3>(&decltype(outSecond)::Type::third));
    return out;
}

void SceneData::unsignedIndexFieldIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const {
    /* fieldId, offset and destination.size() is assumed to be in bounds,
       checked by the callers */

    const SceneFieldData& field = _fields[fieldId];
    const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());
    const auto destination1ui = Containers::arrayCast<2, UnsignedInt>(destination);

    if(field._fieldType == SceneFieldType::UnsignedInt)
        Utility::copy(Containers::arrayCast<const UnsignedInt>(fieldData), destination);
    else if(field._fieldType == SceneFieldType::UnsignedShort)
        Math::castInto(Containers::arrayCast<2, const UnsignedShort>(fieldData, 1), destination1ui);
    else if(field._fieldType == SceneFieldType::UnsignedByte)
        Math::castInto(Containers::arrayCast<2, const UnsignedByte>(fieldData, 1), destination1ui);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void SceneData::indexFieldIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<Int>& destination) const {
    /* fieldId, offset and destination.size() is assumed to be in bounds,
       checked by the callers */

    const SceneFieldData& field = _fields[fieldId];
    const Containers::StridedArrayView1D<const void> fieldData = fieldDataFieldViewInternal(field, offset, destination.size());
    const auto destination1ui = Containers::arrayCast<2, Int>(destination);

    if(field._fieldType == SceneFieldType::Int)
        Utility::copy(Containers::arrayCast<const Int>(fieldData), destination);
    else if(field._fieldType == SceneFieldType::Short)
        Math::castInto(Containers::arrayCast<2, const Short>(fieldData, 1), destination1ui);
    else if(field._fieldType == SceneFieldType::Byte)
        Math::castInto(Containers::arrayCast<2, const Byte>(fieldData, 1), destination1ui);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> SceneData::unsignedIndexFieldAsArrayInternal(const UnsignedInt fieldId) const {
    Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> out{NoInit, std::size_t(_fields[fieldId]._size)};
    /* Explicit slice() template parameters needed by GCC 4.8 and MSVC 2015 */
    mappingIntoInternal(fieldId, 0, stridedArrayView(out).slice<UnsignedInt>(&decltype(out)::Type::first));
    unsignedIndexFieldIntoInternal(fieldId, 0, stridedArrayView(out).slice<UnsignedInt>(&decltype(out)::Type::second));
    return out;
}

void SceneData::meshesMaterialsIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& meshDestination, const Containers::StridedArrayView1D<Int>& meshMaterialDestination) const {
    /* fieldId, offset, meshDestination.size() and
       meshMaterialDestination.size() is assumed to be in bounds, checked by
       the callers */

    if(meshDestination)
        unsignedIndexFieldIntoInternal(fieldId, offset, meshDestination);

    /* Copy also the material, if desired. If no such field is present, output
       -1 for all meshes. */
    if(meshMaterialDestination) {
        const UnsignedInt materialFieldId = findFieldIdInternal(SceneField::MeshMaterial);
        if(materialFieldId == ~UnsignedInt{}) {
            constexpr Int invalid[]{-1};
            Utility::copy(Containers::stridedArrayView(invalid).broadcasted<0>(meshMaterialDestination.size()), meshMaterialDestination);
        } else indexFieldIntoInternal(materialFieldId, offset, meshMaterialDestination);
    }
}

void SceneData::meshesMaterialsInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& meshDestination, const Containers::StridedArrayView1D<Int>& meshMaterialDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Mesh);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::meshesMaterialsInto(): field" << SceneField::Mesh << "not found", );
    CORRADE_ASSERT(!mappingDestination || mappingDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::meshesMaterialsInto(): expected mapping destination view either empty or with" << _fields[fieldId]._size << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(!meshDestination || meshDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::meshesMaterialsInto(): expected mesh destination view either empty or with" << _fields[fieldId]._size << "elements but got" << meshDestination.size(), );
    CORRADE_ASSERT(!meshMaterialDestination || meshMaterialDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::meshesMaterialsInto(): expected mesh material destination view either empty or with" << _fields[fieldId]._size << "elements but got" << meshMaterialDestination.size(), );
    mappingIntoInternal(fieldId, 0, mappingDestination);
    meshesMaterialsIntoInternal(fieldId, 0, meshDestination, meshMaterialDestination);
}

std::size_t SceneData::meshesMaterialsInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& meshDestination, const Containers::StridedArrayView1D<Int>& meshMaterialDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Mesh);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::meshesMaterialsInto(): field" << SceneField::Mesh << "not found", {});
    const std::size_t fieldSize = _fields[fieldId]._size;
    CORRADE_ASSERT(offset <= fieldSize,
        "Trade::SceneData::meshesMaterialsInto(): offset" << offset << "out of bounds for a field of size" << fieldSize, {});
    CORRADE_ASSERT(!mappingDestination != !meshDestination || mappingDestination.size() == meshDestination.size(),
        "Trade::SceneData::meshesMaterialsInto(): mapping and mesh destination views have different size," << mappingDestination.size() << "vs" << meshDestination.size(), {});
    CORRADE_ASSERT(!mappingDestination != !meshMaterialDestination || mappingDestination.size() == meshMaterialDestination.size(),
        "Trade::SceneData::meshesMaterialsInto(): mapping and mesh material destination views have different size," << mappingDestination.size() << "vs" << meshMaterialDestination.size(), {});
    CORRADE_ASSERT(!meshDestination != !meshMaterialDestination || meshMaterialDestination.size() == meshDestination.size(),
        "Trade::SceneData::meshesMaterialsInto(): mesh and mesh material destination views have different size," << meshDestination.size() << "vs" << meshMaterialDestination.size(), {});
    const std::size_t size = Math::min(Math::max({mappingDestination.size(), meshDestination.size(), meshMaterialDestination.size()}), fieldSize - offset);
    if(mappingDestination) mappingIntoInternal(fieldId, offset, mappingDestination.prefix(size));
    meshesMaterialsIntoInternal(fieldId, offset,
        meshDestination ? meshDestination.prefix(size) : nullptr,
        meshMaterialDestination ? meshMaterialDestination.prefix(size) : nullptr);
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>> SceneData::meshesMaterialsAsArray() const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Mesh);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::meshesMaterialsInto(): field" << SceneField::Mesh << "not found", {});
    Containers::Array<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>> out{NoInit, std::size_t(_fields[fieldId]._size)};
    /* Explicit slice() template parameters needed by GCC 4.8 and MSVC 2015 */
    mappingIntoInternal(fieldId, 0, stridedArrayView(out).slice<UnsignedInt>(&decltype(out)::Type::first));
    const auto outSecond = stridedArrayView(out).slice<Containers::Pair<UnsignedInt, Int>>(&decltype(out)::Type::second);
    meshesMaterialsIntoInternal(fieldId, 0,
        outSecond.slice<UnsignedInt>(&decltype(outSecond)::Type::first),
        outSecond.slice<Int>(&decltype(outSecond)::Type::second));
    return out;
}

void SceneData::lightsInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Light);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::lightsInto(): field not found", );
    CORRADE_ASSERT(!mappingDestination || mappingDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::lightsInto(): expected mapping destination view either empty or with" << _fields[fieldId]._size << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(!fieldDestination || fieldDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::lightsInto(): expected field destination view either empty or with" << _fields[fieldId]._size << "elements but got" << fieldDestination.size(), );
    mappingIntoInternal(fieldId, 0, mappingDestination);
    unsignedIndexFieldIntoInternal(fieldId, 0, fieldDestination);
}

std::size_t SceneData::lightsInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Light);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::lightsInto(): field not found", {});
    const std::size_t fieldSize = _fields[fieldId]._size;
    CORRADE_ASSERT(offset <= fieldSize,
        "Trade::SceneData::lightsInto(): offset" << offset << "out of bounds for a field of size" << fieldSize, {});
    CORRADE_ASSERT(!mappingDestination != !fieldDestination|| mappingDestination.size() == fieldDestination.size(),
        "Trade::SceneData::lightsInto(): mapping and field destination views have different size," << mappingDestination.size() << "vs" << fieldDestination.size(), {});
    const std::size_t size = Math::min(Math::max(mappingDestination.size(), fieldDestination.size()), fieldSize - offset);
    if(mappingDestination) mappingIntoInternal(fieldId, offset, mappingDestination.prefix(size));
    if(fieldDestination) unsignedIndexFieldIntoInternal(fieldId, offset, fieldDestination.prefix(size));
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> SceneData::lightsAsArray() const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Light);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::lightsInto(): field not found", {});
    return unsignedIndexFieldAsArrayInternal(fieldId);
}

void SceneData::camerasInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Camera);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::camerasInto(): field not found", );
    CORRADE_ASSERT(!mappingDestination || mappingDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::camerasInto(): expected mapping destination view either empty or with" << _fields[fieldId]._size << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(!fieldDestination || fieldDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::camerasInto(): expected field destination view either empty or with" << _fields[fieldId]._size << "elements but got" << fieldDestination.size(), );
    mappingIntoInternal(fieldId, 0, mappingDestination);
    unsignedIndexFieldIntoInternal(fieldId, 0, fieldDestination);
}

std::size_t SceneData::camerasInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Camera);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::camerasInto(): field not found", {});
    const std::size_t fieldSize = _fields[fieldId]._size;
    CORRADE_ASSERT(offset <= fieldSize,
        "Trade::SceneData::camerasInto(): offset" << offset << "out of bounds for a field of size" << fieldSize, {});
    CORRADE_ASSERT(!mappingDestination != !fieldDestination|| mappingDestination.size() == fieldDestination.size(),
        "Trade::SceneData::camerasInto(): mapping and field destination views have different size," << mappingDestination.size() << "vs" << fieldDestination.size(), {});
    const std::size_t size = Math::min(Math::max(mappingDestination.size(), fieldDestination.size()), fieldSize - offset);
    if(mappingDestination) mappingIntoInternal(fieldId, offset, mappingDestination.prefix(size));
    if(fieldDestination) unsignedIndexFieldIntoInternal(fieldId, offset, fieldDestination.prefix(size));
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> SceneData::camerasAsArray() const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Camera);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::camerasInto(): field not found", {});
    return unsignedIndexFieldAsArrayInternal(fieldId);
}

void SceneData::skinsInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Skin);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::skinsInto(): field not found", );
    CORRADE_ASSERT(!mappingDestination || mappingDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::skinsInto(): expected mapping destination view either empty or with" << _fields[fieldId]._size << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(!fieldDestination || fieldDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::skinsInto(): expected field destination view either empty or with" << _fields[fieldId]._size << "elements but got" << fieldDestination.size(), );
    mappingIntoInternal(fieldId, 0, mappingDestination);
    unsignedIndexFieldIntoInternal(fieldId, 0, fieldDestination);
}

std::size_t SceneData::skinsInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Skin);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::skinsInto(): field not found", {});
    const std::size_t fieldSize = _fields[fieldId]._size;
    CORRADE_ASSERT(offset <= fieldSize,
        "Trade::SceneData::skinsInto(): offset" << offset << "out of bounds for a field of size" << fieldSize, {});
    CORRADE_ASSERT(!mappingDestination != !fieldDestination|| mappingDestination.size() == fieldDestination.size(),
        "Trade::SceneData::skinsInto(): mapping and field destination views have different size," << mappingDestination.size() << "vs" << fieldDestination.size(), {});
    const std::size_t size = Math::min(Math::max(mappingDestination.size(), fieldDestination.size()), fieldSize - offset);
    if(mappingDestination) mappingIntoInternal(fieldId, offset, mappingDestination.prefix(size));
    if(fieldDestination) unsignedIndexFieldIntoInternal(fieldId, offset, fieldDestination.prefix(size));
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> SceneData::skinsAsArray() const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Skin);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::skinsInto(): field not found", {});
    return unsignedIndexFieldAsArrayInternal(fieldId);
}

void SceneData::importerStateIntoInternal(const UnsignedInt fieldId, const std::size_t offset, const Containers::StridedArrayView1D<const void*>& destination) const {
    /* fieldId, offset and destination.size() is assumed to be in bounds,
       checked by the callers */

    const SceneFieldData& field = _fields[fieldId];
    CORRADE_INTERNAL_ASSERT(field._fieldType == SceneFieldType::Pointer ||
                            field._fieldType == SceneFieldType::MutablePointer);
    Utility::copy(Containers::arrayCast<const void* const>(fieldDataFieldViewInternal(field, offset, destination.size())), destination);
}

void SceneData::importerStateInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<const void*>& fieldDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::ImporterState);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::importerStateInto(): field not found", );
    CORRADE_ASSERT(!mappingDestination || mappingDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::importerStateInto(): expected mapping destination view either empty or with" << _fields[fieldId]._size << "elements but got" << mappingDestination.size(), );
    CORRADE_ASSERT(!fieldDestination || fieldDestination.size() == _fields[fieldId]._size,
        "Trade::SceneData::importerStateInto(): expected field destination view either empty or with" << _fields[fieldId]._size << "elements but got" << fieldDestination.size(), );
    mappingIntoInternal(fieldId, 0, mappingDestination);
    importerStateIntoInternal(fieldId, 0, fieldDestination);
}

std::size_t SceneData::importerStateInto(const std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<const void*>& fieldDestination) const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::ImporterState);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::importerStateInto(): field not found", {});
    const std::size_t fieldSize = _fields[fieldId]._size;
    CORRADE_ASSERT(offset <= fieldSize,
        "Trade::SceneData::importerStateInto(): offset" << offset << "out of bounds for a field of size" << fieldSize, {});
    CORRADE_ASSERT(!mappingDestination != !fieldDestination|| mappingDestination.size() == fieldDestination.size(),
        "Trade::SceneData::importerStateInto(): mapping and field destination views have different size," << mappingDestination.size() << "vs" << fieldDestination.size(), {});
    const std::size_t size = Math::min(Math::max(mappingDestination.size(), fieldDestination.size()), fieldSize - offset);
    if(mappingDestination) mappingIntoInternal(fieldId, offset, mappingDestination.prefix(size));
    if(fieldDestination) importerStateIntoInternal(fieldId, offset, fieldDestination.prefix(size));
    return size;
}

Containers::Array<Containers::Pair<UnsignedInt, const void*>> SceneData::importerStateAsArray() const {
    const UnsignedInt fieldId = findFieldIdInternal(SceneField::ImporterState);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        /* Using the same message as in Into() to avoid too many redundant
           strings in the binary */
        "Trade::SceneData::importerStateInto(): field not found", {});
    Containers::Array<Containers::Pair<UnsignedInt, const void*>> out{
        /* There's padding before the pointer on 64bit, zero-initialize to
           avoid keeping random bytes in there */
        #ifdef CORRADE_TARGET_32BIT
        NoInit
        #else
        ValueInit
        #endif
    , std::size_t(_fields[fieldId]._size)};
    /* Explicit slice() template parameters needed by GCC 4.8 and MSVC 2015 */
    mappingIntoInternal(fieldId, 0, stridedArrayView(out).slice<UnsignedInt>(&decltype(out)::Type::first));
    importerStateIntoInternal(fieldId, 0, stridedArrayView(out).slice<const void*>(&decltype(out)::Type::second));
    return out;
}

Containers::Optional<Long> SceneData::parentFor(const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::parentFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Parent);
    if(fieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[fieldId];
    const std::size_t offset = findFieldObjectOffsetInternal(field, object, 0);
    if(offset == field._size) return {};

    Int index[1];
    parentsIntoInternal(fieldId, offset, index);
    return *index;
}

Containers::Array<UnsignedLong> SceneData::childrenFor(const Long object) const {
    CORRADE_ASSERT(object >= -1 && object < Long(_mappingBound),
        "Trade::SceneData::childrenFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    const UnsignedInt parentFieldId = findFieldIdInternal(SceneField::Parent);
    if(parentFieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& parentField = _fields[parentFieldId];

    /* Collect IDs of all objects that reference this object */
    Containers::Array<UnsignedLong> out;
    for(std::size_t offset = 0; offset != parentField.size(); ++offset) {
        Int parentIndex[1];
        parentsIntoInternal(parentFieldId, offset, parentIndex);
        if(*parentIndex == object) {
            /** @todo this drops the upper 64 bits, might be a problem
                eventually (at this point it's more important to have an API
                that won't change the return types in the future, breaking
                existing code) */
            UnsignedInt child[1];
            /** @todo bleh slow, use the children <-> parent field proxying
                when implemented */
            mappingIntoInternal(parentFieldId, offset, child);
            arrayAppend(out, *child);
        }
    }

    return out;
}

Containers::Optional<Matrix3> SceneData::transformation2DFor(const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::transformation2DFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTransformationFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    if(fieldWithObjectMapping == ~UnsignedInt{}) return {};

    /* If is2D() returned false as well, all *FieldId would be invalid, which
       is handled above. */
    CORRADE_ASSERT(!is3D(), "Trade::SceneData::transformation2DFor(): scene has a 3D transformation type", {});

    const SceneFieldData& field = _fields[fieldWithObjectMapping];
    const std::size_t offset = findFieldObjectOffsetInternal(field, object, 0);
    if(offset == field._size) return {};

    Matrix3 transformation[1];
    transformations2DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, offset, transformation);
    return *transformation;
}

Containers::Optional<Containers::Triple<Vector2, Complex, Vector2>> SceneData::translationRotationScaling2DFor(const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::translationRotationScaling2DFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    if(fieldWithObjectMapping == ~UnsignedInt{}) return {};

    /* If is2D() returned false as well, all *FieldId would be invalid, which
       is handled above. */
    CORRADE_ASSERT(!is3D(), "Trade::SceneData::translationRotationScaling2DFor(): scene has a 3D transformation type", {});

    const SceneFieldData& field = _fields[fieldWithObjectMapping];
    const std::size_t offset = findFieldObjectOffsetInternal(field, object, 0);
    if(offset == field._size) return {};

    Vector2 translation[1];
    Complex rotation[1];
    Vector2 scaling[1];
    translationsRotationsScalings2DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, offset, translation, rotation, scaling);
    return {InPlaceInit, *translation, *rotation, *scaling};
}

Containers::Optional<Matrix4> SceneData::transformation3DFor(const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::transformation3DFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    UnsignedInt transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTransformationFields(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId);
    if(fieldWithObjectMapping == ~UnsignedInt{}) return {};

    /* If is3D() returned false as well, all *FieldId would be invalid, which
       is handled above. */
    CORRADE_ASSERT(!is2D(), "Trade::SceneData::transformation3DFor(): scene has a 2D transformation type", {});

    const SceneFieldData& field = _fields[fieldWithObjectMapping];
    const std::size_t offset = findFieldObjectOffsetInternal(field, object, 0);
    if(offset == field._size) return {};

    Matrix4 transformation[1];
    transformations3DIntoInternal(transformationFieldId, translationFieldId, rotationFieldId, scalingFieldId, offset, transformation);
    return *transformation;
}

Containers::Optional<Containers::Triple<Vector3, Quaternion, Vector3>> SceneData::translationRotationScaling3DFor(const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::translationRotationScaling3DFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    UnsignedInt translationFieldId, rotationFieldId, scalingFieldId;
    const UnsignedInt fieldWithObjectMapping = findTranslationRotationScalingFields(translationFieldId, rotationFieldId, scalingFieldId);
    if(fieldWithObjectMapping == ~UnsignedInt{}) return {};

    /* If is3D() returned false as well, all *FieldId would be invalid, which
       is handled above. */
    CORRADE_ASSERT(!is2D(), "Trade::SceneData::translationRotationScaling3DFor(): scene has a 2D transformation type", {});

    const SceneFieldData& field = _fields[fieldWithObjectMapping];
    const std::size_t offset = findFieldObjectOffsetInternal(field, object, 0);
    if(offset == field._size) return {};

    Vector3 translation[1];
    Quaternion rotation[1];
    Vector3 scaling[1];
    translationsRotationsScalings3DIntoInternal(translationFieldId, rotationFieldId, scalingFieldId, offset, translation, rotation, scaling);
    return {InPlaceInit, *translation, *rotation, *scaling};
}

Containers::Array<Containers::Pair<UnsignedInt, Int>> SceneData::meshesMaterialsFor(const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::meshesMaterialsFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    const UnsignedInt meshFieldId = findFieldIdInternal(SceneField::Mesh);
    if(meshFieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[meshFieldId];
    Containers::Array<Containers::Pair<UnsignedInt, Int>> out;
    std::size_t offset = 0;
    for(;;) {
        offset = findFieldObjectOffsetInternal(field, object, offset);
        if(offset == field._size) break;

        UnsignedInt mesh[1];
        Int material[1];
        meshesMaterialsIntoInternal(meshFieldId, offset, mesh, material);
        arrayAppend(out, InPlaceInit, *mesh, *material);
        ++offset;
    }

    return out;
}

Containers::Array<UnsignedInt> SceneData::lightsFor(const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::lightsFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Light);
    if(fieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[fieldId];
    Containers::Array<UnsignedInt> out;
    std::size_t offset = 0;
    for(;;) {
        offset = findFieldObjectOffsetInternal(field, object, offset);
        if(offset == field._size) break;

        UnsignedInt index[1];
        unsignedIndexFieldIntoInternal(fieldId, offset, index);
        arrayAppend(out, InPlaceInit, *index);
        ++offset;
    }

    return out;
}

Containers::Array<UnsignedInt> SceneData::camerasFor(const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::camerasFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Camera);
    if(fieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[fieldId];
    Containers::Array<UnsignedInt> out;
    std::size_t offset = 0;
    for(;;) {
        offset = findFieldObjectOffsetInternal(field, object, offset);
        if(offset == field._size) break;

        UnsignedInt index[1];
        unsignedIndexFieldIntoInternal(fieldId, offset, index);
        arrayAppend(out, InPlaceInit, *index);
        ++offset;
    }

    return out;
}

Containers::Array<UnsignedInt> SceneData::skinsFor(const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::skinsFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    const UnsignedInt fieldId = findFieldIdInternal(SceneField::Skin);
    if(fieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[fieldId];
    Containers::Array<UnsignedInt> out;
    std::size_t offset = 0;
    for(;;) {
        offset = findFieldObjectOffsetInternal(field, object, offset);
        if(offset == field._size) break;

        UnsignedInt index[1];
        unsignedIndexFieldIntoInternal(fieldId, offset, index);
        arrayAppend(out, InPlaceInit, *index);
        ++offset;
    }

    return out;
}

Containers::Optional<const void*> SceneData::importerStateFor(const UnsignedLong object) const {
    CORRADE_ASSERT(object < _mappingBound,
        "Trade::SceneData::importerStateFor(): object" << object << "out of bounds for" << _mappingBound << "objects", {});

    const UnsignedInt fieldId = findFieldIdInternal(SceneField::ImporterState);
    if(fieldId == ~UnsignedInt{}) return {};

    const SceneFieldData& field = _fields[fieldId];
    const std::size_t offset = findFieldObjectOffsetInternal(field, object, 0);
    if(offset == field._size) return {};

    const void* importerState[1];
    importerStateIntoInternal(fieldId, offset, importerState);
    return *importerState;
}

#ifdef MAGNUM_BUILD_DEPRECATED
std::vector<UnsignedInt> SceneData::children2D() const {
    if(_dimensions != 2) return {};

    /* Even though (or exactly because?) this API is deprecated, it's better to
       warn than to spend several hours debugging what's wrong */
    if(!hasField(SceneField::Parent))
        Warning{} << "Trade::SceneData::children2D(): no parent field present, returned array will be empty";

    const Containers::Array<UnsignedLong> children = childrenFor(-1);
    return {children.begin(), children.end()};
}

std::vector<UnsignedInt> SceneData::children3D() const {
    if(_dimensions != 3) return {};

    /* Even though (or exactly because?) this API is deprecated, it's better to
       warn than to spend several hours debugging what's wrong */
    if(!hasField(SceneField::Parent))
        Warning{} << "Trade::SceneData::children3D(): no parent field present, returned array will be empty";

    const Containers::Array<UnsignedLong> children = childrenFor(-1);
    return {children.begin(), children.end()};
}
#endif

Containers::Array<SceneFieldData> SceneData::releaseFieldData() {
    Containers::Array<SceneFieldData> out = std::move(_fields);
    _fields = {};
    return out;
}

Containers::Array<char> SceneData::releaseData() {
    _fields = {};
    Containers::Array<char> out = std::move(_data);
    _data = {};
    return out;
}

}}
