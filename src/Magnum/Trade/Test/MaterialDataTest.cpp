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

#include <algorithm> /* std::next_permutation() */
#include <Corrade/Containers/StaticArray.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/MaterialData.h"

#include "Magnum/Trade/MaterialLayerData.h"
/* used by as(), asRvalue() */
#include "Magnum/Trade/PbrSpecularGlossinessMaterialData.h"
#include "Magnum/Trade/PhongMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct MaterialDataTest: TestSuite::Tester {
    explicit MaterialDataTest();

    void layerName();
    void layerNameInvalid();
    void attributeName();
    void attributeNameInvalid();
    void textureSwizzleComponentCount();
    void attributeTypeSize();
    void attributeTypeSizeInvalid();

    void attributeMap();
    void layerMap();

    void constructAttributeDefault();
    void constructAttributeString();
    void constructAttributeName();
    void constructAttributeTypeErasedString();
    void constructAttributeTypeErasedName();

    template<class T> void constructAttributeStringConstexpr();

    void constructAttributePointer();
    void constructAttributeMutablePointer();
    void constructAttributeStringNameStringValue();
    void constructAttributeStringNameBufferValue();
    void constructAttributeNameStringValue();
    void constructAttributeNameBufferValue();
    void constructAttributeTextureSwizzle();
    void constructAttributeLayer();

    void constructAttributeInvalidName();
    void constructAttributeInvalidLayerName();
    void constructAttributeWrongTypeForName();
    void constructAttributeInvalidType();
    void constructAttributeEmptyName();
    void constructAttributeEmptyNameString();
    void constructAttributeEmptyNameBuffer();
    void constructAttributeTooLarge();
    void constructAttributeTooLargeString();
    void constructAttributeTooLargeBuffer();
    void constructAttributeTooLargeNameString();
    void constructAttributeTooLargeNameBuffer();
    void constructAttributeWrongAccessType();
    void constructAttributeWrongAccessPointerType();
    void constructAttributeWrongAccessTypeString();
    void constructAttributeWrongAccessTypeBuffer();

    void construct();
    void constructEmptyAttribute();
    void constructDuplicateAttribute();
    void constructFromImmutableSortedArray();

    void constructLayers();
    void constructLayersNotMonotonic();
    void constructLayersOffsetOutOfRange();
    void constructLayersLastOffsetTooShort();

    void constructNonOwned();
    void constructNonOwnedLayers();
    void constructNonOwnedEmptyAttribute();
    void constructNonOwnedNotSorted();
    void constructNonOwnedDuplicateAttribute();
    void constructNonOwnedLayersNotMonotonic();
    void constructNonOwnedLayersOffsetOutOfRange();
    void constructNonOwnedLayersLastOffsetTooShort();
    void constructNonOwnedAttributeFlagOwned();
    void constructNonOwnedLayerFlagOwned();

    void constructCopy();
    void constructMove();

    void as();
    void asRvalue();

    void access();
    void accessPointer();
    void accessString();
    void accessBuffer();
    void accessTextureSwizzle();
    void accessMutable();
    void accessOptional();
    void accessOutOfRange();
    void accessNotFound();
    void accessInvalidAttributeName();
    void accessWrongType();
    void accessWrongPointerType();
    void accessWrongTypeString();
    void accessWrongTypeBuffer();

    void accessLayers();
    void accessLayersDefaults();
    void accessLayersTextured();
    void accessLayersTexturedDefault();
    void accessLayersTexturedSingleMatrixCoordinatesLayer();
    void accessLayersTexturedBaseMaterialMatrixCoordinatesLayer();
    void accessLayersInvalidTextures();

    void accessLayerLayerNameInBaseMaterial();
    void accessLayerEmptyLayer();
    void accessLayerIndexMutable();
    void accessLayerNameMutable();
    void accessLayerStringMutable();
    void accessLayerIndexOptional();
    void accessLayerNameOptional();
    void accessLayerStringOptional();
    void accessLayerOutOfRange();
    void accessLayerNotFound();
    void accessInvalidLayerName();
    void accessOutOfRangeInLayerIndex();
    void accessOutOfRangeInLayerString();
    void accessNotFoundInLayerIndex();
    void accessNotFoundInLayerString();
    void accessMutableNotAllowed();

    void releaseAttributes();
    void releaseLayers();

    void templateLayerAccess();
    void templateLayerAccessMutable();

    void debugLayer();
    /* No packed version, as layer name is stored as a string */
    void debugAttribute();
    /* No packed version, as attribute name is stored as a string */
    void debugTextureSwizzle();
    void debugTextureSwizzlePacked();
    void debugAttributeType();
    void debugAttributeTypePacked();

    void debugType();
    void debugTypePacked();
    void debugTypes();
    void debugTypesPacked();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void debugFlag();
    void debugFlags();
    #endif
    void debugAlphaMode();
    void debugAlphaModePacked();
};

MaterialDataTest::MaterialDataTest() {
    addTests({&MaterialDataTest::layerName,
              &MaterialDataTest::layerNameInvalid,

              &MaterialDataTest::attributeName,
              &MaterialDataTest::attributeNameInvalid,

              &MaterialDataTest::textureSwizzleComponentCount,
              &MaterialDataTest::attributeTypeSize,
              &MaterialDataTest::attributeTypeSizeInvalid,

              &MaterialDataTest::attributeMap,
              &MaterialDataTest::layerMap,

              &MaterialDataTest::constructAttributeDefault,
              &MaterialDataTest::constructAttributeString,
              &MaterialDataTest::constructAttributeName,
              &MaterialDataTest::constructAttributeTypeErasedString,
              &MaterialDataTest::constructAttributeTypeErasedName,

              &MaterialDataTest::constructAttributeStringConstexpr<bool>,
              &MaterialDataTest::constructAttributeStringConstexpr<Float>,
              &MaterialDataTest::constructAttributeStringConstexpr<Deg>,
              &MaterialDataTest::constructAttributeStringConstexpr<Rad>,
              &MaterialDataTest::constructAttributeStringConstexpr<UnsignedInt>,
              &MaterialDataTest::constructAttributeStringConstexpr<Int>,
              &MaterialDataTest::constructAttributeStringConstexpr<UnsignedLong>,
              &MaterialDataTest::constructAttributeStringConstexpr<Long>,
              &MaterialDataTest::constructAttributeStringConstexpr<Vector2>,
              &MaterialDataTest::constructAttributeStringConstexpr<Vector2ui>,
              &MaterialDataTest::constructAttributeStringConstexpr<Vector2i>,
              &MaterialDataTest::constructAttributeStringConstexpr<Vector3>,
              &MaterialDataTest::constructAttributeStringConstexpr<Vector3ui>,
              &MaterialDataTest::constructAttributeStringConstexpr<Vector3i>,
              &MaterialDataTest::constructAttributeStringConstexpr<Vector4>,
              &MaterialDataTest::constructAttributeStringConstexpr<Vector4ui>,
              &MaterialDataTest::constructAttributeStringConstexpr<Vector4i>,
              &MaterialDataTest::constructAttributeStringConstexpr<Matrix2x2>,
              &MaterialDataTest::constructAttributeStringConstexpr<Matrix2x3>,
              &MaterialDataTest::constructAttributeStringConstexpr<Matrix2x4>,
              &MaterialDataTest::constructAttributeStringConstexpr<Matrix3x2>,
              &MaterialDataTest::constructAttributeStringConstexpr<Matrix3x3>,
              &MaterialDataTest::constructAttributeStringConstexpr<Matrix3x4>,
              &MaterialDataTest::constructAttributeStringConstexpr<Matrix4x2>,
              &MaterialDataTest::constructAttributeStringConstexpr<Matrix4x3>,

              &MaterialDataTest::constructAttributePointer,
              &MaterialDataTest::constructAttributeMutablePointer,
              &MaterialDataTest::constructAttributeStringNameStringValue,
              &MaterialDataTest::constructAttributeStringNameBufferValue,
              &MaterialDataTest::constructAttributeNameStringValue,
              &MaterialDataTest::constructAttributeNameBufferValue,
              &MaterialDataTest::constructAttributeTextureSwizzle,
              &MaterialDataTest::constructAttributeLayer,

              &MaterialDataTest::constructAttributeInvalidName,
              &MaterialDataTest::constructAttributeInvalidLayerName,
              &MaterialDataTest::constructAttributeWrongTypeForName,
              &MaterialDataTest::constructAttributeInvalidType,
              &MaterialDataTest::constructAttributeEmptyName,
              &MaterialDataTest::constructAttributeEmptyNameString,
              &MaterialDataTest::constructAttributeEmptyNameBuffer,
              &MaterialDataTest::constructAttributeTooLarge,
              &MaterialDataTest::constructAttributeTooLargeString,
              &MaterialDataTest::constructAttributeTooLargeBuffer,
              &MaterialDataTest::constructAttributeTooLargeNameString,
              &MaterialDataTest::constructAttributeTooLargeNameBuffer,
              &MaterialDataTest::constructAttributeWrongAccessType,
              &MaterialDataTest::constructAttributeWrongAccessPointerType,
              &MaterialDataTest::constructAttributeWrongAccessTypeString,
              &MaterialDataTest::constructAttributeWrongAccessTypeBuffer,

              &MaterialDataTest::construct,
              &MaterialDataTest::constructEmptyAttribute});

    addRepeatedTests({&MaterialDataTest::constructDuplicateAttribute},
        5*4*3*2);

    addTests({&MaterialDataTest::constructFromImmutableSortedArray,

              &MaterialDataTest::constructLayers,
              &MaterialDataTest::constructLayersNotMonotonic,
              &MaterialDataTest::constructLayersOffsetOutOfRange,
              &MaterialDataTest::constructLayersLastOffsetTooShort,

              &MaterialDataTest::constructNonOwned,
              &MaterialDataTest::constructNonOwnedLayers,
              &MaterialDataTest::constructNonOwnedEmptyAttribute,
              &MaterialDataTest::constructNonOwnedNotSorted,
              &MaterialDataTest::constructNonOwnedDuplicateAttribute,
              &MaterialDataTest::constructNonOwnedLayersNotMonotonic,
              &MaterialDataTest::constructNonOwnedLayersOffsetOutOfRange,
              &MaterialDataTest::constructNonOwnedLayersLastOffsetTooShort,
              &MaterialDataTest::constructNonOwnedAttributeFlagOwned,
              &MaterialDataTest::constructNonOwnedLayerFlagOwned,

              &MaterialDataTest::constructCopy,
              &MaterialDataTest::constructMove,

              &MaterialDataTest::as,
              &MaterialDataTest::asRvalue,

              &MaterialDataTest::access,
              &MaterialDataTest::accessPointer,
              &MaterialDataTest::accessString,
              &MaterialDataTest::accessBuffer,
              &MaterialDataTest::accessTextureSwizzle,
              &MaterialDataTest::accessMutable,
              &MaterialDataTest::accessOptional,
              &MaterialDataTest::accessOutOfRange,
              &MaterialDataTest::accessNotFound,
              &MaterialDataTest::accessInvalidAttributeName,
              &MaterialDataTest::accessWrongType,
              &MaterialDataTest::accessWrongPointerType,
              &MaterialDataTest::accessWrongTypeString,
              &MaterialDataTest::accessWrongTypeBuffer,

              &MaterialDataTest::accessLayers,
              &MaterialDataTest::accessLayersDefaults,
              &MaterialDataTest::accessLayersTextured,
              &MaterialDataTest::accessLayersTexturedDefault,
              &MaterialDataTest::accessLayersTexturedSingleMatrixCoordinatesLayer,
              &MaterialDataTest::accessLayersTexturedBaseMaterialMatrixCoordinatesLayer,
              &MaterialDataTest::accessLayersInvalidTextures,

              &MaterialDataTest::accessLayerLayerNameInBaseMaterial,
              &MaterialDataTest::accessLayerEmptyLayer,
              &MaterialDataTest::accessLayerIndexMutable,
              &MaterialDataTest::accessLayerNameMutable,
              &MaterialDataTest::accessLayerStringMutable,
              &MaterialDataTest::accessLayerIndexOptional,
              &MaterialDataTest::accessLayerNameOptional,
              &MaterialDataTest::accessLayerStringOptional,
              &MaterialDataTest::accessLayerOutOfRange,
              &MaterialDataTest::accessLayerNotFound,
              &MaterialDataTest::accessInvalidLayerName,
              &MaterialDataTest::accessOutOfRangeInLayerIndex,
              &MaterialDataTest::accessOutOfRangeInLayerString,
              &MaterialDataTest::accessNotFoundInLayerIndex,
              &MaterialDataTest::accessNotFoundInLayerString,
              &MaterialDataTest::accessMutableNotAllowed,

              &MaterialDataTest::releaseAttributes,
              &MaterialDataTest::releaseLayers,

              &MaterialDataTest::templateLayerAccess,
              &MaterialDataTest::templateLayerAccessMutable,

              &MaterialDataTest::debugLayer,
              &MaterialDataTest::debugAttribute,
              &MaterialDataTest::debugTextureSwizzle,
              &MaterialDataTest::debugTextureSwizzlePacked,
              &MaterialDataTest::debugAttributeType,
              &MaterialDataTest::debugAttributeTypePacked,

              &MaterialDataTest::debugType,
              &MaterialDataTest::debugTypePacked,
              &MaterialDataTest::debugTypes,
              &MaterialDataTest::debugTypesPacked,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &MaterialDataTest::debugFlag,
              &MaterialDataTest::debugFlags,
              #endif
              &MaterialDataTest::debugAlphaMode,
              &MaterialDataTest::debugAlphaModePacked});
}

using namespace Containers::Literals;
using namespace Math::Literals;

void MaterialDataTest::layerName() {
    CORRADE_COMPARE(materialLayerName(MaterialLayer::ClearCoat), "ClearCoat");
}

void MaterialDataTest::layerNameInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    materialLayerName(MaterialLayer(0x0));
    materialLayerName(MaterialLayer(0xdeadbeef));
    CORRADE_COMPARE(out,
        "Trade::materialLayerName(): invalid layer Trade::MaterialLayer(0x0)\n"
        "Trade::materialLayerName(): invalid layer Trade::MaterialLayer(0xdeadbeef)\n");
}

void MaterialDataTest::attributeName() {
    CORRADE_COMPARE(materialAttributeName(MaterialAttribute::BaseColorTextureMatrix), "BaseColorTextureMatrix");
}

void MaterialDataTest::attributeNameInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    materialAttributeName(MaterialAttribute(0x0));
    materialAttributeName(MaterialAttribute(0xdeadbeef));
    CORRADE_COMPARE(out,
        "Trade::materialAttributeName(): invalid attribute Trade::MaterialAttribute(0x0)\n"
        "Trade::materialAttributeName(): invalid attribute Trade::MaterialAttribute(0xdeadbeef)\n");
}

void MaterialDataTest::textureSwizzleComponentCount() {
    CORRADE_COMPARE(materialTextureSwizzleComponentCount(MaterialTextureSwizzle::B), 1);
    CORRADE_COMPARE(materialTextureSwizzleComponentCount(MaterialTextureSwizzle::RG), 2);
    CORRADE_COMPARE(materialTextureSwizzleComponentCount(MaterialTextureSwizzle::GA), 2);
    CORRADE_COMPARE(materialTextureSwizzleComponentCount(MaterialTextureSwizzle::GBA), 3);
    CORRADE_COMPARE(materialTextureSwizzleComponentCount(MaterialTextureSwizzle::RGBA), 4);
}

void MaterialDataTest::attributeTypeSize() {
    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::Bool), 1);
    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::Deg), 4);
    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::Vector2i), 8);
    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::Vector3), 12);
    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::Vector4ui), 16);
    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::Matrix2x3), 24);
    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::Matrix4x2), 32);
    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::Matrix3x3), 36);
    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::Matrix3x4), 48);
}

void MaterialDataTest::attributeTypeSizeInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    materialAttributeTypeSize(MaterialAttributeType(0x0));
    materialAttributeTypeSize(MaterialAttributeType(0xfe));
    materialAttributeTypeSize(MaterialAttributeType::String);
    materialAttributeTypeSize(MaterialAttributeType::Buffer);
    CORRADE_COMPARE(out,
        "Trade::materialAttributeTypeSize(): invalid type Trade::MaterialAttributeType(0x0)\n"
        "Trade::materialAttributeTypeSize(): invalid type Trade::MaterialAttributeType(0xfe)\n"
        "Trade::materialAttributeTypeSize(): string and buffer size is unknown\n"
        "Trade::materialAttributeTypeSize(): string and buffer size is unknown\n");
}

void MaterialDataTest::attributeMap() {
    /* Ensure all attribute names are:
        - present in the map,
        - that their translated string name corresponds to the enum value name,
        - that the calculated type size corresponds to the actual type,
        - and that the name together with the type fits.
       This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        /* Attribute 0 reserved for an invalid value */

        const auto attribute = MaterialAttribute(i);
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(attribute) {
            #define _c(name_, type) \
                case MaterialAttribute::name_: \
                    CORRADE_COMPARE((MaterialAttributeData{MaterialAttribute::name_, type{}}.name()), #name_); \
                    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::type), sizeof(type)); \
                    CORRADE_COMPARE_AS(sizeof(type) + Containers::arraySize(#name_) + sizeof(MaterialAttributeType), sizeof(MaterialAttributeData), TestSuite::Compare::LessOrEqual); \
                    break;
            #define _ct(name_, typeName, type) \
                case MaterialAttribute::name_: \
                    CORRADE_COMPARE((MaterialAttributeData{MaterialAttribute::name_, type{}}.name()), #name_); \
                    CORRADE_COMPARE(materialAttributeTypeSize(MaterialAttributeType::typeName), sizeof(type)); \
                    CORRADE_COMPARE_AS(sizeof(type) + Containers::arraySize(#name_) + sizeof(MaterialAttributeType), sizeof(MaterialAttributeData), TestSuite::Compare::LessOrEqual); \
                    break;
            #define _cnt(name_, string, typeName, type) \
                case MaterialAttribute::name_: \
                    CORRADE_COMPARE((MaterialAttributeData{MaterialAttribute::name_, type{}}.name()), string); \
                    break;
            #include "Magnum/Trade/Implementation/materialAttributeProperties.hpp"
            #undef _c
            #undef _ct
            #undef _cnt
        }
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic pop
        #endif
    }
}

void MaterialDataTest::layerMap() {
    /* Ensure all layer names are:
        - present in the map,
        - and that their translated string name corresponds to the enum value
          name
       This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        /* Attribute 0 reserved for an invalid value */

        const auto attribute = MaterialLayer(i);
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(attribute) {
            #define _c(name_) \
                case MaterialLayer::name_: \
                    CORRADE_COMPARE((MaterialAttributeData{MaterialLayer::name_}.value<Containers::StringView>()), #name_); \
                    break;
            #include "Magnum/Trade/Implementation/materialLayerProperties.hpp"
            #undef _c
        }
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic pop
        #endif
    }
}

void MaterialDataTest::constructAttributeDefault() {
    MaterialAttributeData attribute;
    CORRADE_COMPARE(attribute.name(), "");
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType{});

    constexpr MaterialAttributeData cattribute;
    CORRADE_COMPARE(cattribute.name(), "");
    CORRADE_COMPARE(cattribute.type(), MaterialAttributeType{});
}

void MaterialDataTest::constructAttributeString() {
    MaterialAttributeData attribute{"colorTransform", Matrix3::scaling({2.0f, 0.3f})};
    CORRADE_COMPARE(attribute.name(), "colorTransform");
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::Matrix3x3);
    CORRADE_COMPARE(attribute.value<Matrix3>(), Matrix3::scaling({2.0f, 0.3f}));
    CORRADE_COMPARE(*reinterpret_cast<const Matrix3*>(attribute.value()), Matrix3::scaling({2.0f, 0.3f}));
}

void MaterialDataTest::constructAttributeName() {
    MaterialAttributeData attribute{MaterialAttribute::DiffuseColor, 0xff3366aa_rgbaf};
    CORRADE_COMPARE(attribute.name(), "DiffuseColor"_s);
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::Vector4);
    CORRADE_COMPARE(attribute.value<Color4>(), 0xff3366aa_rgbaf);
    CORRADE_COMPARE(*reinterpret_cast<const Color4*>(attribute.value()), 0xff3366aa_rgbaf);
}

void MaterialDataTest::constructAttributeTypeErasedString() {
    const Vector2i data{37, -458};
    MaterialAttributeData attribute{"millibitsOfInformation", MaterialAttributeType::Vector2i, &data};
    CORRADE_COMPARE(attribute.name(), "millibitsOfInformation");
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::Vector2i);
    CORRADE_COMPARE(attribute.value<Vector2i>(), (Vector2i{37, -458}));
}

void MaterialDataTest::constructAttributeTypeErasedName() {
    const Float data = 85.1f;
    MaterialAttributeData attribute{MaterialAttribute::Shininess, MaterialAttributeType::Float, &data};
    CORRADE_COMPARE(attribute.name(), "Shininess");
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::Float);
    CORRADE_COMPARE(attribute.value<Float>(), 85.1f);
}

template<class T> struct TypeName {
    static const char* name() { return Math::TypeTraits<T>::name(); }
};
template<> struct TypeName<bool> {
    static const char* name() { return "bool"; }
};
#define _c(type) template<> struct TypeName<type> {                         \
    static const char* name() { return #type; }                             \
};
_c(Deg)
_c(Rad)
_c(Vector2)
_c(Vector2i)
_c(Vector2ui)
_c(Vector3)
_c(Vector3i)
_c(Vector3ui)
_c(Vector4)
_c(Vector4i)
_c(Vector4ui)
_c(Matrix2x2)
_c(Matrix2x3)
_c(Matrix2x4)
_c(Matrix3x2)
_c(Matrix3x3)
_c(Matrix3x4)
_c(Matrix4x2)
_c(Matrix4x3)
#undef _c

template<class T> void MaterialDataTest::constructAttributeStringConstexpr() {
    setTestCaseTemplateName(TypeName<T>::name());

    /* "templateAttrib" is 14 chars, which is the maximum for 48-bit types */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    /* Unfortunately this doesn't always work, for example Vector3. Disabling
       unconditionally, might revisit later. */
    constexpr
    #endif
    MaterialAttributeData attribute{"templateAttrib"_s, T(15)};
    CORRADE_COMPARE(attribute.name(), "templateAttrib");
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.value<T>(), T(15));
}

constexpr Int SomeData = 3;

void MaterialDataTest::constructAttributePointer() {
    MaterialAttributeData attribute{"pointer!", &SomeData};
    CORRADE_COMPARE(attribute.name(), "pointer!");
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::Pointer);
    CORRADE_COMPARE(*static_cast<const Int* const*>(attribute.value()), &SomeData);
    CORRADE_COMPARE(attribute.value<const Int*>(), &SomeData);
    /* Any type works */
    CORRADE_COMPARE(attribute.value<const void*>(), &SomeData);

    constexpr MaterialAttributeData cattribute{"pointer!"_s, &SomeData};
    CORRADE_COMPARE(cattribute.name(), "pointer!");
    CORRADE_COMPARE(cattribute.type(), MaterialAttributeType::Pointer);
    CORRADE_COMPARE(*static_cast<const Int* const*>(cattribute.value()), &SomeData);
    CORRADE_COMPARE(cattribute.value<const Int*>(), &SomeData);

    /* Type-erased variant */
    const Int* pointer = &SomeData;
    MaterialAttributeData typeErased{"pointer!", MaterialAttributeType::Pointer, &pointer};
    CORRADE_COMPARE(typeErased.name(), "pointer!");
    CORRADE_COMPARE(typeErased.type(), MaterialAttributeType::Pointer);
    CORRADE_COMPARE(typeErased.value<const Int*>(), &SomeData);
    /* Any type works */
    CORRADE_COMPARE(typeErased.value<const void*>(), &SomeData);
}

void MaterialDataTest::constructAttributeMutablePointer() {
    Float data = 85.1f;

    MaterialAttributeData attribute{"pointer!", &data};
    CORRADE_COMPARE(attribute.name(), "pointer!");
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::MutablePointer);
    CORRADE_COMPARE(*static_cast<Float* const*>(attribute.value()), &data);
    CORRADE_COMPARE(attribute.value<Float*>(), &data);
    /* Any type works */
    CORRADE_COMPARE(attribute.value<void*>(), &data);

    /* Type-erased variant */
    Float* pointer = &data;
    MaterialAttributeData typeErased{"pointer!", MaterialAttributeType::MutablePointer, &pointer};
    CORRADE_COMPARE(typeErased.name(), "pointer!");
    CORRADE_COMPARE(typeErased.type(), MaterialAttributeType::MutablePointer);
    CORRADE_COMPARE(typeErased.value<Float*>(), &data);
    /* Any type works */
    CORRADE_COMPARE(typeErased.value<void*>(), &data);
}

void MaterialDataTest::constructAttributeStringNameStringValue() {
    /* Explicitly using a non-null-terminated view on input to check the null
       byte isn't read by accident*/
    MaterialAttributeData attribute{"name that's long", "and a value\0that's also long but still fits!!"_s.exceptSuffix(1)};
    CORRADE_COMPARE(attribute.name(), "name that's long");
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::String);
    /* Pointer access will stop at the first null byte, but typed access won't */
    CORRADE_COMPARE(static_cast<const char*>(attribute.value()), "and a value"_s);
    CORRADE_COMPARE(attribute.value<Containers::StringView>(), "and a value\0that's also long but still fits!"_s);
    CORRADE_COMPARE(attribute.value<Containers::StringView>().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.value<Containers::StringView>()[attribute.value<Containers::StringView>().size()], '\0');

    constexpr MaterialAttributeData cattribute{"name that's long"_s, "and a value\0that's also long but still fits!!"_s.exceptSuffix(1)};
    CORRADE_COMPARE(cattribute.name(), "name that's long");
    CORRADE_COMPARE(cattribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(cattribute.name()[cattribute.name().size()], '\0');
    CORRADE_COMPARE(cattribute.type(), MaterialAttributeType::String);
    CORRADE_COMPARE(cattribute.value<Containers::StringView>(), "and a value\0that's also long but still fits!"_s);
    CORRADE_COMPARE(cattribute.value<Containers::StringView>().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(cattribute.value<Containers::StringView>()[cattribute.value<Containers::StringView>().size()], '\0');

    /* Type-erased variant */
    const Containers::StringView value = "and a value\0that's also long but still fits!!"_s.exceptSuffix(1);
    MaterialAttributeData typeErased{"name that's long", MaterialAttributeType::String, &value};
    CORRADE_COMPARE(typeErased.name(), "name that's long");
    CORRADE_COMPARE(typeErased.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(typeErased.name()[typeErased.name().size()], '\0');
    CORRADE_COMPARE(typeErased.type(), MaterialAttributeType::String);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>(), "and a value\0that's also long but still fits!"_s);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>()[typeErased.value<Containers::StringView>().size()], '\0');
}

void MaterialDataTest::constructAttributeStringNameBufferValue() {
    /* Explicitly using a non-null-terminated view on input to check the null
       byte isn't read by accident*/
    MaterialAttributeData attribute{"name that's long", Containers::arrayView({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f})};
    CORRADE_COMPARE(attribute.name(), "name that's long");
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::Buffer);
    /* The pointer should be aligned */
    CORRADE_COMPARE_AS(attribute.value(), 4, TestSuite::Compare::Aligned);
    CORRADE_COMPARE(static_cast<const Float*>(attribute.value())[5], 5.0f);
    CORRADE_COMPARE_AS(Containers::arrayCast<const Float>(attribute.value<Containers::ArrayView<const void>>()), Containers::arrayView({
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f
    }), TestSuite::Compare::Container);

    /* Compared to a StringView attribute there's no constexpr variant of the
       constructor */

    /* Type-erased variant. The above overload delegates into this one so it's
       testing the same code path, but keep it here for consistency with
       the constructAttributeStringNameStringValue() case. */
    const Float value[]{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    Containers::ArrayView<const void> view = value;
    MaterialAttributeData typeErased{"name that's long", MaterialAttributeType::Buffer, &view};
    CORRADE_COMPARE(typeErased.name(), "name that's long");
    CORRADE_COMPARE(typeErased.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(typeErased.name()[typeErased.name().size()], '\0');
    CORRADE_COMPARE(typeErased.type(), MaterialAttributeType::Buffer);
    /* The pointer should be aligned */
    CORRADE_COMPARE_AS(attribute.value(), 4, TestSuite::Compare::Aligned);
    CORRADE_COMPARE(static_cast<const Float*>(attribute.value())[5], 5.0f);
    CORRADE_COMPARE_AS(Containers::arrayCast<const Float>(attribute.value<Containers::ArrayView<const void>>()), Containers::arrayView({
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f
    }), TestSuite::Compare::Container);
}

void MaterialDataTest::constructAttributeNameStringValue() {
    /* Explicitly using a non-null-terminated view on input to check the null
       byte isn't read by accident*/

    MaterialAttributeData attribute{MaterialAttribute::LayerName, "a value\0that's long but still fits!!"_s.exceptSuffix(1)};
    CORRADE_COMPARE(attribute.name(), " LayerName");
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::String);
    /* Pointer access will stop at the first null byte, but typed access won't */
    CORRADE_COMPARE(static_cast<const char*>(attribute.value()), "a value"_s);
    CORRADE_COMPARE(attribute.value<Containers::StringView>(), "a value\0that's long but still fits!"_s);
    CORRADE_COMPARE(attribute.value<Containers::StringView>().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.value<Containers::StringView>()[attribute.value<Containers::StringView>().size()], '\0');

    /* Type-erased variant */
    const Containers::StringView value = "a value\0that's long but still fits!!"_s.exceptSuffix(1);
    MaterialAttributeData typeErased{MaterialAttribute::LayerName, MaterialAttributeType::String, &value};
    CORRADE_COMPARE(typeErased.name(), " LayerName");
    CORRADE_COMPARE(typeErased.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(typeErased.name()[typeErased.name().size()], '\0');
    CORRADE_COMPARE(typeErased.type(), MaterialAttributeType::String);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>(), "a value\0that's long but still fits!"_s);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>()[typeErased.value<Containers::StringView>().size()], '\0');
}

void MaterialDataTest::constructAttributeNameBufferValue() {
    CORRADE_SKIP("No builtin attributes with" << MaterialAttributeType::Buffer << "at the moment.");
}

void MaterialDataTest::constructAttributeTextureSwizzle() {
    MaterialAttributeData attribute{"swizzle", MaterialTextureSwizzle::GBA};
    CORRADE_COMPARE(attribute.name(), "swizzle");
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::TextureSwizzle);
    CORRADE_COMPARE(*static_cast<const MaterialTextureSwizzle*>(attribute.value()), MaterialTextureSwizzle::GBA);
    CORRADE_COMPARE(attribute.value<MaterialTextureSwizzle>(), MaterialTextureSwizzle::GBA);

    constexpr MaterialAttributeData cattribute{"swizzle"_s, MaterialTextureSwizzle::GBA};
    CORRADE_COMPARE(cattribute.name(), "swizzle");
    CORRADE_COMPARE(cattribute.type(), MaterialAttributeType::TextureSwizzle);
    CORRADE_COMPARE(*static_cast<const MaterialTextureSwizzle*>(cattribute.value()), MaterialTextureSwizzle::GBA);
    CORRADE_COMPARE(cattribute.value<MaterialTextureSwizzle>(), MaterialTextureSwizzle::GBA);

    /* Type-erased variant */
    const MaterialTextureSwizzle swizzle = MaterialTextureSwizzle::GBA;
    MaterialAttributeData typeErased{"swizzle", MaterialAttributeType::TextureSwizzle, &swizzle};
    CORRADE_COMPARE(typeErased.name(), "swizzle");
    CORRADE_COMPARE(typeErased.type(), MaterialAttributeType::TextureSwizzle);
    CORRADE_COMPARE(typeErased.value<MaterialTextureSwizzle>(), MaterialTextureSwizzle::GBA);
}

void MaterialDataTest::constructAttributeLayer() {
    MaterialAttributeData attribute{MaterialLayer::ClearCoat};
    CORRADE_COMPARE(attribute.name(), " LayerName");
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::String);
    CORRADE_COMPARE(attribute.value<Containers::StringView>(), "ClearCoat");
}

void MaterialDataTest::constructAttributeInvalidName() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{MaterialAttribute(0x0), 5};
    MaterialAttributeData{MaterialAttribute(0xfefe), 5};
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialAttributeData: invalid name Trade::MaterialAttribute(0xfefe)\n");
}

void MaterialDataTest::constructAttributeInvalidLayerName() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{MaterialLayer(0x0)};
    MaterialAttributeData{MaterialLayer(0xfefe)};
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: invalid name Trade::MaterialLayer(0x0)\n"
        "Trade::MaterialAttributeData: invalid name Trade::MaterialLayer(0xfefe)\n");
}

void MaterialDataTest::constructAttributeWrongTypeForName() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{MaterialAttribute::DiffuseColor, Vector3ui{255, 16, 24}};
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: expected Trade::MaterialAttributeType::Vector4 for Trade::MaterialAttribute::DiffuseColor but got Trade::MaterialAttributeType::Vector3ui\n");
}

void MaterialDataTest::constructAttributeInvalidType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{"bla", MaterialAttributeType(0x0), nullptr};
    MaterialAttributeData{"bla", MaterialAttributeType(0xfe),  nullptr};
    CORRADE_COMPARE(out,
        "Trade::materialAttributeTypeSize(): invalid type Trade::MaterialAttributeType(0x0)\n"
        "Trade::materialAttributeTypeSize(): invalid type Trade::MaterialAttributeType(0xfe)\n");
}

void MaterialDataTest::constructAttributeEmptyName() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{"", Int{}};
    /* Constexpr variant has the same assert, but in the header. It should have
       the same output. */
    /*constexpr*/ MaterialAttributeData{""_s, Int{}};
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: name is not allowed to be empty\n"
        "Trade::MaterialAttributeData: name is not allowed to be empty\n");
}

void MaterialDataTest::constructAttributeEmptyNameString() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* This has no reason to not be allowed */
    MaterialAttributeData{"hello this string is empty", ""};
    MaterialAttributeData{"hello this string is empty"_s, ""_s};

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{"", "hello"};
    /* Constexpr variant has the same assert, but in the header. It should have
       the same output. */
    /*constexpr*/ MaterialAttributeData{""_s, "hello"_s};
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: name is not allowed to be empty\n"
        "Trade::MaterialAttributeData: name is not allowed to be empty\n");
}

void MaterialDataTest::constructAttributeEmptyNameBuffer() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* This has no reason to not be allowed */
    MaterialAttributeData{"hello this buffer is empty", Containers::ArrayView<const void>{}};

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{"", Containers::ArrayView<const void>{"E", 2}};
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: name is not allowed to be empty\n");
}

void MaterialDataTest::constructAttributeTooLarge() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{"attributeIsLong", Matrix3x4{}};
    /* Constexpr variant has the same assert, but in the header. It should have
       the same output. Except on MSVC 2015, which is a crap thing and gets
       lost when encountering T in there, so the assert is less useful. */
    /*constexpr*/ MaterialAttributeData{"attributeIsLong"_s, Matrix3x4{}};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: name attributeIsLong too long, expected at most 14 bytes for Trade::MaterialAttributeType::Matrix3x4 but got 15\n"
        "Trade::MaterialAttributeData: name attributeIsLong too long, expected at most 14 bytes for Trade::MaterialAttributeType::Matrix3x4 but got 15\n");
    #else
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: name attributeIsLong too long, expected at most 14 bytes for Trade::MaterialAttributeType::Matrix3x4 but got 15\n"
        "Trade::MaterialAttributeData: name attributeIsLong too long, got 15 bytes\n");
    #endif
}

void MaterialDataTest::constructAttributeTooLargeString() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{"attribute is long", "This is a problem, got a long piece of text!"};
    /* Constexpr variant has the same assert, but in the header. It should have
       the same output. */
    /*constexpr*/ MaterialAttributeData{"attribute is long"_s, "This is a problem, got a long piece of text!"_s};
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: name attribute is long and value This is a problem, got a long piece of text! too long, expected at most 60 bytes in total but got 61\n"
        "Trade::MaterialAttributeData: name attribute is long and value This is a problem, got a long piece of text! too long, expected at most 60 bytes in total but got 61\n");
}

void MaterialDataTest::constructAttributeTooLargeBuffer() {
    CORRADE_SKIP_IF_NO_ASSERT();

    int data[10]; /* 40 bytes */

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{"attribute is very long", data};
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: name attribute is very long and a 40-byte value too long, expected at most 61 bytes in total but got 62\n");
}

void MaterialDataTest::constructAttributeTooLargeNameString() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{MaterialAttribute::LayerName, "This is a problem, got a huge, yuuge value to store"};
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData: name  LayerName and value This is a problem, got a huge, yuuge value to store too long, expected at most 60 bytes in total but got 61\n");
}

void MaterialDataTest::constructAttributeTooLargeNameBuffer() {
    CORRADE_SKIP("No builtin attributes with" << MaterialAttributeType::Buffer << "at the moment.");
}

void MaterialDataTest::constructAttributeWrongAccessType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{"thing3", Matrix4x3{}}.value<Int>();
    CORRADE_COMPARE(out, "Trade::MaterialAttributeData::value(): thing3 is Trade::MaterialAttributeType::Matrix4x3 but requested a type equivalent to Trade::MaterialAttributeType::Int\n");
}

void MaterialDataTest::constructAttributeWrongAccessPointerType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Int a = 3;
    const Float b = 57.0f;

    MaterialAttributeData thing3{"thing3", &a};
    MaterialAttributeData boom{"boom", &b};

    /* These are fine (type is not checked) */
    thing3.value<Float*>();
    boom.value<const Int*>();

    Containers::String out;
    Error redirectError{&out};
    thing3.value<Int>();
    thing3.value<const Int*>();
    boom.value<Float*>();
    CORRADE_COMPARE(out,
        "Trade::MaterialAttributeData::value(): thing3 is Trade::MaterialAttributeType::MutablePointer but requested a type equivalent to Trade::MaterialAttributeType::Int\n"
        "Trade::MaterialAttributeData::value(): thing3 is Trade::MaterialAttributeType::MutablePointer but requested a type equivalent to Trade::MaterialAttributeType::Pointer\n"
        "Trade::MaterialAttributeData::value(): boom is Trade::MaterialAttributeType::Pointer but requested a type equivalent to Trade::MaterialAttributeType::MutablePointer\n");
}

void MaterialDataTest::constructAttributeWrongAccessTypeString() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{"thing3", Matrix4x3{}}.value<Containers::StringView>();
    CORRADE_COMPARE(out, "Trade::MaterialAttributeData::value(): thing3 of Trade::MaterialAttributeType::Matrix4x3 can't be retrieved as a string\n");
}

void MaterialDataTest::constructAttributeWrongAccessTypeBuffer() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialAttributeData{"thing3", Matrix4x3{}}.value<Containers::ArrayView<const void>>();
    CORRADE_COMPARE(out, "Trade::MaterialAttributeData::value(): thing3 of Trade::MaterialAttributeType::Matrix4x3 can't be retrieved as a buffer\n");
}

void MaterialDataTest::construct() {
    int state;
    MaterialData data{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {"highlightColor", 0x335566ff_rgbaf},
        {MaterialAttribute::AmbientTextureMatrix, Matrix3::scaling({0.5f, 1.0f})}
    }, &state};

    CORRADE_COMPARE(data.attributeDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.layerDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_COMPARE(data.layerCount(), 1);
    CORRADE_VERIFY(!data.layerData());
    CORRADE_COMPARE(data.attributeCount(), 4);
    CORRADE_COMPARE(data.attributeData().size(), 4);
    CORRADE_COMPARE(data.importerState(), &state);

    CORRADE_COMPARE(data.layerName(0), "");
    CORRADE_VERIFY(!data.hasLayer(""));

    CORRADE_COMPARE(data.attributeDataOffset(0), 0);
    CORRADE_COMPARE(data.attributeDataOffset(1), 4);

    /* Verify sorting */
    CORRADE_COMPARE(data.attributeName(0), "AmbientTextureMatrix");
    CORRADE_COMPARE(data.attributeName(1), "DiffuseTextureCoordinates");
    CORRADE_COMPARE(data.attributeName(2), "DoubleSided");
    CORRADE_COMPARE(data.attributeName(3), "highlightColor");

    /* Access by ID */
    CORRADE_COMPARE(data.attributeData(0).name(), "AmbientTextureMatrix");
    CORRADE_COMPARE(data.attributeData(1).name(), "DiffuseTextureCoordinates");
    CORRADE_COMPARE(data.attributeData(2).name(), "DoubleSided");
    CORRADE_COMPARE(data.attributeData(3).name(), "highlightColor");

    CORRADE_COMPARE(data.attributeType(0), MaterialAttributeType::Matrix3x3);
    CORRADE_COMPARE(data.attributeType(1), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType(2), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType(3), MaterialAttributeType::Vector4);

    CORRADE_COMPARE(data.attribute<Matrix3>(0), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.attribute<UnsignedInt>(1), 5);
    CORRADE_COMPARE(data.attribute<bool>(2), true);
    CORRADE_COMPARE(data.attribute<Color4>(3), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.mutableAttribute<Matrix3>(0), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.mutableAttribute<UnsignedInt>(1), 5);
    CORRADE_COMPARE(data.mutableAttribute<bool>(2), true);
    CORRADE_COMPARE(data.mutableAttribute<Color4>(3), 0x335566ff_rgbaf);

    CORRADE_COMPARE(*static_cast<const Matrix3*>(data.attribute(0)), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(1)), 5);
    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(2)), true);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute(3)), 0x335566ff_rgbaf);
    CORRADE_COMPARE(*static_cast<Matrix3*>(data.mutableAttribute(0)), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(*static_cast<UnsignedInt*>(data.mutableAttribute(1)), 5);
    CORRADE_COMPARE(*static_cast<bool*>(data.mutableAttribute(2)), true);
    CORRADE_COMPARE(*static_cast<Color4*>(data.mutableAttribute(3)), 0x335566ff_rgbaf);

    /* Access by name */
    CORRADE_VERIFY(data.hasAttribute(MaterialAttribute::DoubleSided));
    CORRADE_VERIFY(data.hasAttribute(MaterialAttribute::AmbientTextureMatrix));
    CORRADE_VERIFY(data.hasAttribute(MaterialAttribute::DiffuseTextureCoordinates));
    CORRADE_VERIFY(!data.hasAttribute(MaterialAttribute::TextureMatrix));

    CORRADE_COMPARE(data.findAttributeId(MaterialAttribute::DoubleSided), 2);
    CORRADE_COMPARE(data.findAttributeId(MaterialAttribute::AmbientTextureMatrix), 0);
    CORRADE_COMPARE(data.findAttributeId(MaterialAttribute::DiffuseTextureCoordinates), 1);
    CORRADE_VERIFY(!data.findAttributeId(MaterialAttribute::TextureMatrix));

    CORRADE_COMPARE(data.attributeId(MaterialAttribute::DoubleSided), 2);
    CORRADE_COMPARE(data.attributeId(MaterialAttribute::AmbientTextureMatrix), 0);
    CORRADE_COMPARE(data.attributeId(MaterialAttribute::DiffuseTextureCoordinates), 1);

    CORRADE_COMPARE(data.attributeType(MaterialAttribute::AmbientTextureMatrix), MaterialAttributeType::Matrix3x3);
    CORRADE_COMPARE(data.attributeType(MaterialAttribute::DiffuseTextureCoordinates), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType(MaterialAttribute::DoubleSided), MaterialAttributeType::Bool);

    CORRADE_COMPARE(data.attribute<Matrix3>(MaterialAttribute::AmbientTextureMatrix), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.attribute<UnsignedInt>(MaterialAttribute::DiffuseTextureCoordinates), 5);
    CORRADE_COMPARE(data.attribute<bool>(MaterialAttribute::DoubleSided), true);
    CORRADE_COMPARE(data.mutableAttribute<Matrix3>(MaterialAttribute::AmbientTextureMatrix), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.mutableAttribute<UnsignedInt>(MaterialAttribute::DiffuseTextureCoordinates), 5);
    CORRADE_COMPARE(data.mutableAttribute<bool>(MaterialAttribute::DoubleSided), true);

    CORRADE_COMPARE(*static_cast<const Matrix3*>(data.attribute(MaterialAttribute::AmbientTextureMatrix)), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(MaterialAttribute::DiffuseTextureCoordinates)), 5);
    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(MaterialAttribute::DoubleSided)), true);
    CORRADE_COMPARE(*static_cast<Matrix3*>(data.mutableAttribute(MaterialAttribute::AmbientTextureMatrix)), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(*static_cast<UnsignedInt*>(data.mutableAttribute(MaterialAttribute::DiffuseTextureCoordinates)), 5);
    CORRADE_COMPARE(*static_cast<bool*>(data.mutableAttribute(MaterialAttribute::DoubleSided)), true);

    /* Access by string */
    CORRADE_VERIFY(data.hasAttribute("DoubleSided"));
    CORRADE_VERIFY(data.hasAttribute("highlightColor"));
    CORRADE_VERIFY(data.hasAttribute("DiffuseTextureCoordinates"));
    CORRADE_VERIFY(data.hasAttribute("highlightColor"));
    CORRADE_VERIFY(!data.hasAttribute("TextureMatrix"));

    CORRADE_COMPARE(data.findAttributeId("DoubleSided"), 2);
    CORRADE_COMPARE(data.findAttributeId("AmbientTextureMatrix"), 0);
    CORRADE_COMPARE(data.findAttributeId("DiffuseTextureCoordinates"), 1);
    CORRADE_COMPARE(data.findAttributeId("highlightColor"), 3);
    CORRADE_VERIFY(!data.findAttributeId("TextureMatrix"));

    CORRADE_COMPARE(data.attributeId("DoubleSided"), 2);
    CORRADE_COMPARE(data.attributeId("AmbientTextureMatrix"), 0);
    CORRADE_COMPARE(data.attributeId("DiffuseTextureCoordinates"), 1);
    CORRADE_COMPARE(data.attributeId("highlightColor"), 3);

    CORRADE_COMPARE(data.attributeType("AmbientTextureMatrix"), MaterialAttributeType::Matrix3x3);
    CORRADE_COMPARE(data.attributeType("DiffuseTextureCoordinates"), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType("DoubleSided"), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType("highlightColor"), MaterialAttributeType::Vector4);

    CORRADE_COMPARE(data.attribute<Matrix3>("AmbientTextureMatrix"), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.attribute<UnsignedInt>("DiffuseTextureCoordinates"), 5);
    CORRADE_COMPARE(data.attribute<bool>("DoubleSided"), true);
    CORRADE_COMPARE(data.attribute<Color4>("highlightColor"), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.mutableAttribute<Matrix3>("AmbientTextureMatrix"), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.mutableAttribute<UnsignedInt>("DiffuseTextureCoordinates"), 5);
    CORRADE_COMPARE(data.mutableAttribute<bool>("DoubleSided"), true);
    CORRADE_COMPARE(data.mutableAttribute<Color4>("highlightColor"), 0x335566ff_rgbaf);

    CORRADE_COMPARE(*static_cast<const Matrix3*>(data.attribute("AmbientTextureMatrix")), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute("DiffuseTextureCoordinates")), 5);
    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute("DoubleSided")), true);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute("highlightColor")), 0x335566ff_rgbaf);
    CORRADE_COMPARE(*static_cast<Matrix3*>(data.mutableAttribute("AmbientTextureMatrix")), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(*static_cast<UnsignedInt*>(data.mutableAttribute("DiffuseTextureCoordinates")), 5);
    CORRADE_COMPARE(*static_cast<bool*>(data.mutableAttribute("DoubleSided")), true);
    CORRADE_COMPARE(*static_cast<Color4*>(data.mutableAttribute("highlightColor")), 0x335566ff_rgbaf);
}

void MaterialDataTest::constructEmptyAttribute() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialData{{}, {
        {"DiffuseTexture"_s, 12u},
        MaterialAttributeData{}
    }};
    CORRADE_COMPARE(out, "Trade::MaterialData: attribute 1 doesn't specify anything\n");
}

void MaterialDataTest::constructDuplicateAttribute() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::Array<MaterialAttributeData> attributes{InPlaceInit, {
        /* This attribute is in the first layer, so it should not be reported
           as duplicated */
        {MaterialAttribute::DoubleSided, true},

        /* Second layer is empty, this is the third layer (index 2) */
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {"highlightColor", 0x335566ff_rgbaf},
        {MaterialAttribute::AmbientTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u}
    }};

    /* Testing that it asserts in all input permutations */
    for(std::size_t i = 0; i != testCaseRepeatId(); ++i)
        std::next_permutation(attributes.begin() + 1, attributes.end(), [](const MaterialAttributeData& a, const MaterialAttributeData& b) {
            return a.name() < b.name();
        });

    Containers::String out;
    Error redirectError{&out};
    MaterialData data{{}, Utility::move(attributes), Containers::array<UnsignedInt>({1, 1, 6})};
    /* Because with graceful asserts it doesn't exit on error, the assertion
       might get printed multiple times */
    CORRADE_COMPARE(out.partition('\n')[0], "Trade::MaterialData: duplicate attribute DiffuseTextureCoordinates in layer 2"_s);
}

void MaterialDataTest::constructFromImmutableSortedArray() {
    constexpr MaterialAttributeData attributes[]{
        {"hello this is first"_s, 1},
        {"yay this is last"_s, Vector4{0.2f, 0.6f, 0.4f, 1.0f}}
    };

    MaterialData data{{}, Containers::Array<MaterialAttributeData>{const_cast<MaterialAttributeData*>(attributes), Containers::arraySize(attributes), [](MaterialAttributeData*, std::size_t) {}}};

    CORRADE_COMPARE(data.attributeCount(), 2);
    CORRADE_COMPARE(data.attributeName(0), "hello this is first");
    CORRADE_COMPARE(data.attributeName(1), "yay this is last");
}

void MaterialDataTest::constructLayers() {
    int state;
    MaterialData data{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},

        /* Layer name gets sorted first by the constructor */
        {"highlightColor", 0x335566ff_rgbaf},
        {MaterialAttribute::AlphaBlend, true},
        {MaterialLayer::ClearCoat},

        /* Empty layer here */

        /* Unnamed but nonempty layer */
        {"thickness", 0.015f},
        {MaterialAttribute::NormalTexture, 3u}
    }, {
        2, 5, 5, 7
    }, &state};

    CORRADE_COMPARE(data.attributeDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.layerDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_COMPARE(data.importerState(), &state);

    CORRADE_COMPARE(data.layerCount(), 4);
    CORRADE_COMPARE(data.layerData().size(), 4);

    CORRADE_COMPARE(data.attributeDataOffset(0), 0);
    CORRADE_COMPARE(data.attributeDataOffset(1), 2);
    CORRADE_COMPARE(data.attributeDataOffset(2), 5);
    CORRADE_COMPARE(data.attributeDataOffset(3), 5);
    CORRADE_COMPARE(data.attributeDataOffset(4), 7);

    CORRADE_COMPARE(data.attributeData().size(), 7);
    CORRADE_COMPARE(data.attributeCount(0), 2);
    CORRADE_COMPARE(data.attributeCount(1), 3);
    CORRADE_COMPARE(data.attributeCount(2), 0);
    CORRADE_COMPARE(data.attributeCount(3), 2);
    CORRADE_COMPARE(data.attributeCount("ClearCoat"), 3);
    CORRADE_COMPARE(data.attributeCount(MaterialLayer::ClearCoat), 3);

    /* Layer access */
    CORRADE_COMPARE(data.layerName(0), "");
    CORRADE_COMPARE(data.layerName(1), "ClearCoat");
    CORRADE_COMPARE(data.layerName(2), "");
    CORRADE_COMPARE(data.layerName(3), "");

    CORRADE_VERIFY(data.hasLayer("ClearCoat"));
    CORRADE_VERIFY(data.hasLayer(MaterialLayer::ClearCoat));
    CORRADE_VERIFY(!data.hasLayer(""));
    CORRADE_VERIFY(!data.hasLayer("DoubleSided"));
    /** @todo test hasLayer(MaterialLayer) once there's more than ClearCoat */

    CORRADE_COMPARE(data.findLayerId("ClearCoat"), 1);
    CORRADE_COMPARE(data.findLayerId(MaterialLayer::ClearCoat), 1);
    CORRADE_VERIFY(!data.findLayerId(""));
    CORRADE_VERIFY(!data.findLayerId("DoubleSided"));
    /** @todo test findLayerId(MaterialLayer) once there's more than ClearCoat */

    CORRADE_COMPARE(data.layerId("ClearCoat"), 1);
    CORRADE_COMPARE(data.layerId(MaterialLayer::ClearCoat), 1);

    /* Verify sorting in each layer */
    CORRADE_COMPARE(data.attributeName(0, 0), "DiffuseTextureCoordinates");
    CORRADE_COMPARE(data.attributeName(0, 1), "DoubleSided");

    CORRADE_COMPARE(data.attributeName(1, 0), " LayerName");
    CORRADE_COMPARE(data.attributeName(1, 1), "AlphaBlend");
    CORRADE_COMPARE(data.attributeName(1, 2), "highlightColor");

    CORRADE_COMPARE(data.attributeName(3, 0), "NormalTexture");
    CORRADE_COMPARE(data.attributeName(3, 1), "thickness");

    /* Access by layer ID and attribute ID */
    CORRADE_COMPARE(data.attributeData(0, 0).name(), "DiffuseTextureCoordinates");
    CORRADE_COMPARE(data.attributeData(1, 2).name(), "highlightColor");
    CORRADE_COMPARE(data.attributeData(3, 1).name(), "thickness");

    CORRADE_COMPARE(data.attributeType(0, 0), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType(1, 2), MaterialAttributeType::Vector4);
    CORRADE_COMPARE(data.attributeType(3, 1), MaterialAttributeType::Float);

    CORRADE_COMPARE(data.attribute<UnsignedInt>(0, 0), 5);
    CORRADE_COMPARE(data.attribute<Color4>(1, 2), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.attribute<Float>(3, 1), 0.015f);
    CORRADE_COMPARE(data.mutableAttribute<UnsignedInt>(0, 0), 5);
    CORRADE_COMPARE(data.mutableAttribute<Color4>(1, 2), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.mutableAttribute<Float>(3, 1), 0.015f);

    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(0, 0)), 5);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute(1, 2)), 0x335566ff_rgbaf);
    CORRADE_COMPARE(*static_cast<const Float*>(data.attribute(3, 1)), 0.015f);
    CORRADE_COMPARE(*static_cast<UnsignedInt*>(data.mutableAttribute(0, 0)), 5);
    CORRADE_COMPARE(*static_cast<Color4*>(data.mutableAttribute(1, 2)), 0x335566ff_rgbaf);
    CORRADE_COMPARE(*static_cast<Float*>(data.mutableAttribute(3, 1)), 0.015f);

    /* Access by layer ID and attribute name */
    CORRADE_VERIFY(data.hasAttribute(0, MaterialAttribute::DiffuseTextureCoordinates));
    CORRADE_VERIFY(!data.hasAttribute(0, MaterialAttribute::AlphaBlend));
    CORRADE_VERIFY(data.hasAttribute(1, MaterialAttribute::AlphaBlend));
    CORRADE_VERIFY(data.hasAttribute(1, MaterialAttribute::LayerName));
    CORRADE_VERIFY(!data.hasAttribute(2, MaterialAttribute::LayerName));
    CORRADE_VERIFY(!data.hasAttribute(2, MaterialAttribute::NormalTexture));
    CORRADE_VERIFY(data.hasAttribute(3, MaterialAttribute::NormalTexture));

    CORRADE_COMPARE(data.findAttributeId(0, MaterialAttribute::DiffuseTextureCoordinates), 0);
    CORRADE_VERIFY(!data.findAttributeId(0, MaterialAttribute::AlphaBlend));
    CORRADE_COMPARE(data.findAttributeId(1, MaterialAttribute::AlphaBlend), 1);
    CORRADE_COMPARE(data.findAttributeId(1, MaterialAttribute::LayerName), 0);
    CORRADE_VERIFY(!data.findAttributeId(2, MaterialAttribute::LayerName));
    CORRADE_VERIFY(!data.findAttributeId(2, MaterialAttribute::NormalTexture));
    CORRADE_COMPARE(data.findAttributeId(3, MaterialAttribute::NormalTexture), 0);

    CORRADE_COMPARE(data.attributeId(0, MaterialAttribute::DiffuseTextureCoordinates), 0);
    CORRADE_COMPARE(data.attributeId(1, MaterialAttribute::AlphaBlend), 1);
    CORRADE_COMPARE(data.attributeId(1, MaterialAttribute::LayerName), 0);
    CORRADE_COMPARE(data.attributeId(3, MaterialAttribute::NormalTexture), 0);

    CORRADE_COMPARE(data.attributeType(0, MaterialAttribute::DiffuseTextureCoordinates), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType(1, MaterialAttribute::AlphaBlend), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType(1, MaterialAttribute::LayerName), MaterialAttributeType::String);
    CORRADE_COMPARE(data.attributeType(3, MaterialAttribute::NormalTexture), MaterialAttributeType::UnsignedInt);

    CORRADE_COMPARE(data.attribute<UnsignedInt>(0, MaterialAttribute::DiffuseTextureCoordinates), 5);
    CORRADE_COMPARE(data.attribute<bool>(1, MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(data.attribute<Containers::StringView>(1, MaterialAttribute::LayerName), "ClearCoat");
    CORRADE_COMPARE(data.attribute<UnsignedInt>(3, MaterialAttribute::NormalTexture), 3);
    CORRADE_COMPARE(data.mutableAttribute<UnsignedInt>(0, MaterialAttribute::DiffuseTextureCoordinates), 5);
    CORRADE_COMPARE(data.mutableAttribute<bool>(1, MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(data.mutableAttribute<Containers::MutableStringView>(1, MaterialAttribute::LayerName), "ClearCoat"_s);
    CORRADE_COMPARE(data.mutableAttribute<UnsignedInt>(3, MaterialAttribute::NormalTexture), 3);

    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(0, MaterialAttribute::DiffuseTextureCoordinates)), 5);
    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(1, MaterialAttribute::AlphaBlend)), true);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute(1, MaterialAttribute::LayerName)), "ClearCoat"_s);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(3, MaterialAttribute::NormalTexture)), 3);
    CORRADE_COMPARE(*static_cast<UnsignedInt*>(data.mutableAttribute(0, MaterialAttribute::DiffuseTextureCoordinates)), 5);
    CORRADE_COMPARE(*static_cast<bool*>(data.mutableAttribute(1, MaterialAttribute::AlphaBlend)), true);
    CORRADE_COMPARE(static_cast<char*>(data.mutableAttribute(1, MaterialAttribute::LayerName)), "ClearCoat"_s);
    CORRADE_COMPARE(*static_cast<UnsignedInt*>(data.mutableAttribute(3, MaterialAttribute::NormalTexture)), 3);

    /* Access by layer ID and attribute string */
    CORRADE_VERIFY(data.hasAttribute(0, "DoubleSided"));
    CORRADE_VERIFY(!data.hasAttribute(0, "highlightColor"));
    CORRADE_VERIFY(data.hasAttribute(1, "highlightColor"));
    CORRADE_VERIFY(data.hasAttribute(1, " LayerName"));
    CORRADE_VERIFY(!data.hasAttribute(2, " LayerName"));
    CORRADE_VERIFY(!data.hasAttribute(2, "NormalTexture"));
    CORRADE_VERIFY(data.hasAttribute(3, "NormalTexture"));

    CORRADE_COMPARE(data.findAttributeId(0, "DoubleSided"), 1);
    CORRADE_VERIFY(!data.findAttributeId(0, "highlightColor"));
    CORRADE_COMPARE(data.findAttributeId(1, "highlightColor"), 2);
    CORRADE_COMPARE(data.findAttributeId(1, " LayerName"), 0);
    CORRADE_VERIFY(!data.findAttributeId(2, " LayerName"));
    CORRADE_VERIFY(!data.findAttributeId(2, " NormalTexture"));
    CORRADE_COMPARE(data.findAttributeId(3, "NormalTexture"), 0);

    CORRADE_COMPARE(data.attributeId(0, "DoubleSided"), 1);
    CORRADE_COMPARE(data.attributeId(1, "highlightColor"), 2);
    CORRADE_COMPARE(data.attributeId(1, " LayerName"), 0);
    CORRADE_COMPARE(data.attributeId(3, "NormalTexture"), 0);

    CORRADE_COMPARE(data.attributeType(0, "DoubleSided"), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType(1, "highlightColor"), MaterialAttributeType::Vector4);
    CORRADE_COMPARE(data.attributeType(1, " LayerName"), MaterialAttributeType::String);
    CORRADE_COMPARE(data.attributeType(3, "NormalTexture"), MaterialAttributeType::UnsignedInt);

    CORRADE_COMPARE(data.attribute<bool>(0, "DoubleSided"), true);
    CORRADE_COMPARE(data.attribute<Color4>(1, "highlightColor"), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.attribute<Containers::StringView>(1, " LayerName"), "ClearCoat");
    CORRADE_COMPARE(data.attribute<UnsignedInt>(3, "NormalTexture"), 3);
    CORRADE_COMPARE(data.mutableAttribute<bool>(0, "DoubleSided"), true);
    CORRADE_COMPARE(data.mutableAttribute<Color4>(1, "highlightColor"), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.mutableAttribute<Containers::MutableStringView>(1, " LayerName"), "ClearCoat"_s);
    CORRADE_COMPARE(data.mutableAttribute<UnsignedInt>(3, "NormalTexture"), 3);

    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(0, "DoubleSided")), true);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute(1, "highlightColor")), 0x335566ff_rgbaf);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute(1, " LayerName")), "ClearCoat"_s);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(3, "NormalTexture")), 3);
    CORRADE_COMPARE(*static_cast<bool*>(data.mutableAttribute(0, "DoubleSided")), true);
    CORRADE_COMPARE(*static_cast<Color4*>(data.mutableAttribute(1, "highlightColor")), 0x335566ff_rgbaf);
    CORRADE_COMPARE(static_cast<char*>(data.mutableAttribute(1, " LayerName")), "ClearCoat"_s);
    CORRADE_COMPARE(*static_cast<UnsignedInt*>(data.mutableAttribute(3, "NormalTexture")), 3);

    /* Access by layer name and attribute ID */
    CORRADE_COMPARE(data.attributeName(MaterialLayer::ClearCoat, 1), "AlphaBlend");
    CORRADE_COMPARE(data.attributeName(MaterialLayer::ClearCoat, 2), "highlightColor");

    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, 1), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, 2), MaterialAttributeType::Vector4);

    CORRADE_COMPARE(data.attribute<bool>(MaterialLayer::ClearCoat, 1), true);
    CORRADE_COMPARE(data.attribute<Color4>(MaterialLayer::ClearCoat, 2), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.mutableAttribute<bool>(MaterialLayer::ClearCoat, 1), true);
    CORRADE_COMPARE(data.mutableAttribute<Color4>(MaterialLayer::ClearCoat, 2), 0x335566ff_rgbaf);

    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(MaterialLayer::ClearCoat, 1)), true);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute(MaterialLayer::ClearCoat, 2)), 0x335566ff_rgbaf);
    CORRADE_COMPARE(*static_cast<bool*>(data.mutableAttribute(MaterialLayer::ClearCoat, 1)), true);
    CORRADE_COMPARE(*static_cast<Color4*>(data.mutableAttribute(MaterialLayer::ClearCoat, 2)), 0x335566ff_rgbaf);

    /* Access by layer name and attribute name */
    CORRADE_VERIFY(data.hasAttribute(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend));
    CORRADE_VERIFY(data.hasAttribute(MaterialLayer::ClearCoat, MaterialAttribute::LayerName));
    CORRADE_VERIFY(!data.hasAttribute(MaterialLayer::ClearCoat, MaterialAttribute::BaseColor));

    CORRADE_COMPARE(data.findAttributeId(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend), 1);
    CORRADE_COMPARE(data.findAttributeId(MaterialLayer::ClearCoat, MaterialAttribute::LayerName), 0);
    CORRADE_VERIFY(!data.findAttributeId(MaterialLayer::ClearCoat, MaterialAttribute::BaseColor));

    CORRADE_COMPARE(data.attributeId(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend), 1);
    CORRADE_COMPARE(data.attributeId(MaterialLayer::ClearCoat, MaterialAttribute::LayerName), 0);

    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, MaterialAttribute::LayerName), MaterialAttributeType::String);

    CORRADE_COMPARE(data.attribute<bool>(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(data.attribute<Containers::StringView>(MaterialLayer::ClearCoat, MaterialAttribute::LayerName), "ClearCoat");
    CORRADE_COMPARE(data.mutableAttribute<bool>(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(data.mutableAttribute<Containers::MutableStringView>(MaterialLayer::ClearCoat, MaterialAttribute::LayerName), "ClearCoat"_s);

    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend)), true);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute(MaterialLayer::ClearCoat, MaterialAttribute::LayerName)), "ClearCoat"_s);
    CORRADE_COMPARE(*static_cast<bool*>(data.mutableAttribute(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend)), true);
    CORRADE_COMPARE(static_cast<char*>(data.mutableAttribute(MaterialLayer::ClearCoat, MaterialAttribute::LayerName)), "ClearCoat"_s);

    /* Access by layer name and attribute string */
    CORRADE_VERIFY(data.hasAttribute(MaterialLayer::ClearCoat, "highlightColor"));
    CORRADE_VERIFY(data.hasAttribute(MaterialLayer::ClearCoat, " LayerName"));
    CORRADE_VERIFY(!data.hasAttribute(MaterialLayer::ClearCoat, "BaseColor"));

    CORRADE_COMPARE(data.findAttributeId(MaterialLayer::ClearCoat, "highlightColor"), 2);
    CORRADE_COMPARE(data.findAttributeId(MaterialLayer::ClearCoat, " LayerName"), 0);
    CORRADE_VERIFY(!data.findAttributeId(MaterialLayer::ClearCoat, "BaseColor"));

    CORRADE_COMPARE(data.attributeId(MaterialLayer::ClearCoat, "highlightColor"), 2);
    CORRADE_COMPARE(data.attributeId(MaterialLayer::ClearCoat, " LayerName"), 0);

    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, "highlightColor"), MaterialAttributeType::Vector4);
    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, " LayerName"), MaterialAttributeType::String);

    CORRADE_COMPARE(data.attribute<Color4>(MaterialLayer::ClearCoat, "highlightColor"), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.attribute<Containers::StringView>(MaterialLayer::ClearCoat, " LayerName"), "ClearCoat");
    CORRADE_COMPARE(data.mutableAttribute<Color4>(MaterialLayer::ClearCoat, "highlightColor"), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.mutableAttribute<Containers::MutableStringView>(MaterialLayer::ClearCoat, " LayerName"), "ClearCoat"_s);

    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute(MaterialLayer::ClearCoat, "highlightColor")), 0x335566ff_rgbaf);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute(MaterialLayer::ClearCoat, " LayerName")), "ClearCoat"_s);
    CORRADE_COMPARE(*static_cast<Color4*>(data.mutableAttribute(MaterialLayer::ClearCoat, "highlightColor")), 0x335566ff_rgbaf);
    CORRADE_COMPARE(static_cast<char*>(data.mutableAttribute(MaterialLayer::ClearCoat, " LayerName")), "ClearCoat"_s);

    /* Access by layer string and attribute ID */
    CORRADE_COMPARE(data.attributeName("ClearCoat", 1), "AlphaBlend");
    CORRADE_COMPARE(data.attributeName("ClearCoat", 2), "highlightColor");

    CORRADE_COMPARE(data.attributeType("ClearCoat", 1), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType("ClearCoat", 2), MaterialAttributeType::Vector4);

    CORRADE_COMPARE(data.attribute<bool>("ClearCoat", 1), true);
    CORRADE_COMPARE(data.attribute<Color4>("ClearCoat", 2), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.mutableAttribute<bool>("ClearCoat", 1), true);
    CORRADE_COMPARE(data.mutableAttribute<Color4>("ClearCoat", 2), 0x335566ff_rgbaf);

    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute("ClearCoat", 1)), true);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute("ClearCoat", 2)), 0x335566ff_rgbaf);
    CORRADE_COMPARE(*static_cast<bool*>(data.mutableAttribute("ClearCoat", 1)), true);
    CORRADE_COMPARE(*static_cast<Color4*>(data.mutableAttribute("ClearCoat", 2)), 0x335566ff_rgbaf);

    /* Access by layer string and attribute name */
    CORRADE_VERIFY(data.hasAttribute("ClearCoat", MaterialAttribute::AlphaBlend));
    CORRADE_VERIFY(data.hasAttribute("ClearCoat", MaterialAttribute::LayerName));
    CORRADE_VERIFY(!data.hasAttribute("ClearCoat", MaterialAttribute::BaseColor));

    CORRADE_COMPARE(data.findAttributeId("ClearCoat", MaterialAttribute::AlphaBlend), 1);
    CORRADE_COMPARE(data.findAttributeId("ClearCoat", MaterialAttribute::LayerName), 0);
    CORRADE_VERIFY(!data.findAttributeId("ClearCoat", MaterialAttribute::BaseColor));

    CORRADE_COMPARE(data.attributeId("ClearCoat", MaterialAttribute::AlphaBlend), 1);
    CORRADE_COMPARE(data.attributeId("ClearCoat", MaterialAttribute::LayerName), 0);

    CORRADE_COMPARE(data.attributeType("ClearCoat", MaterialAttribute::AlphaBlend), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType("ClearCoat", MaterialAttribute::LayerName), MaterialAttributeType::String);

    CORRADE_COMPARE(data.attribute<bool>("ClearCoat", MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(data.attribute<Containers::StringView>("ClearCoat", MaterialAttribute::LayerName), "ClearCoat");
    CORRADE_COMPARE(data.mutableAttribute<bool>("ClearCoat", MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(data.mutableAttribute<Containers::MutableStringView>("ClearCoat", MaterialAttribute::LayerName), "ClearCoat"_s);

    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute("ClearCoat", MaterialAttribute::AlphaBlend)), true);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute("ClearCoat", MaterialAttribute::LayerName)), "ClearCoat"_s);
    CORRADE_COMPARE(*static_cast<bool*>(data.mutableAttribute("ClearCoat", MaterialAttribute::AlphaBlend)), true);
    CORRADE_COMPARE(static_cast<char*>(data.mutableAttribute("ClearCoat", MaterialAttribute::LayerName)), "ClearCoat"_s);

    /* Access by layer string and attribute string */
    CORRADE_VERIFY(data.hasAttribute("ClearCoat", "highlightColor"));
    CORRADE_VERIFY(data.hasAttribute("ClearCoat", " LayerName"));
    CORRADE_VERIFY(!data.hasAttribute("ClearCoat", "BaseColor"));

    CORRADE_COMPARE(data.findAttributeId("ClearCoat", "highlightColor"), 2);
    CORRADE_COMPARE(data.findAttributeId("ClearCoat", " LayerName"), 0);
    CORRADE_VERIFY(!data.findAttributeId("ClearCoat", "BaseColor"));

    CORRADE_COMPARE(data.attributeId("ClearCoat", "highlightColor"), 2);
    CORRADE_COMPARE(data.attributeId("ClearCoat", " LayerName"), 0);

    CORRADE_COMPARE(data.attributeType("ClearCoat", "highlightColor"), MaterialAttributeType::Vector4);
    CORRADE_COMPARE(data.attributeType("ClearCoat", " LayerName"), MaterialAttributeType::String);

    CORRADE_COMPARE(data.attribute<Color4>("ClearCoat", "highlightColor"), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.attribute<Containers::StringView>("ClearCoat", " LayerName"), "ClearCoat");
    CORRADE_COMPARE(data.mutableAttribute<Color4>("ClearCoat", "highlightColor"), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.mutableAttribute<Containers::MutableStringView>("ClearCoat", " LayerName"), "ClearCoat"_s);

    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute("ClearCoat", "highlightColor")), 0x335566ff_rgbaf);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute("ClearCoat", " LayerName")), "ClearCoat"_s);
    CORRADE_COMPARE(*static_cast<Color4*>(data.mutableAttribute("ClearCoat", "highlightColor")), 0x335566ff_rgbaf);
    CORRADE_COMPARE(static_cast<char*>(data.mutableAttribute("ClearCoat", " LayerName")), "ClearCoat"_s);
}

void MaterialDataTest::constructLayersNotMonotonic() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialData data{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {MaterialAttribute::AlphaBlend, true},
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::NormalTexture, 3u}
    }, {2, 5, 4, 5}};
    CORRADE_COMPARE(out, "Trade::MaterialData: invalid range (5, 4) for layer 2 with 5 attributes in total\n");
}

void MaterialDataTest::constructLayersOffsetOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialData data{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {MaterialAttribute::AlphaBlend, true},
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::NormalTexture, 3u}
    }, {2, 6}};
    CORRADE_COMPARE(out, "Trade::MaterialData: invalid range (2, 6) for layer 1 with 5 attributes in total\n");
}

void MaterialDataTest::constructLayersLastOffsetTooShort() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MaterialData data{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::LayerFactor, 0.5f},
        {MaterialAttribute::NormalTexture, 3u}
    }, {1, 3}};
    CORRADE_COMPARE(out, "Trade::MaterialData: last layer offset 3 too short for 4 attributes in total\n");
}

void MaterialDataTest::constructNonOwned() {
    constexpr MaterialAttributeData attributes[]{
        {"AmbientTextureMatrix"_s, Matrix3{{0.5f, 0.0f, 0.0f},
                                           {0.0f, 1.0f, 0.0f},
                                           {0.0f, 0.0f, 1.0f}}},
        {"DiffuseTextureCoordinates"_s, 5u},
        {"DoubleSided"_s, true},
        {"highlightColor"_s, Vector4{0.2f, 0.6f, 0.4f, 1.0f}}
    };

    int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    MaterialData data{MaterialType::Phong, {}, attributes, &state};

    CORRADE_COMPARE(data.attributeDataFlags(), DataFlags{});
    CORRADE_COMPARE(data.layerDataFlags(), DataFlags{});
    /* Expecting the same output as in construct() */
    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_COMPARE(data.layerCount(), 1);
    CORRADE_VERIFY(!data.layerData());
    CORRADE_COMPARE(data.attributeCount(), 4);
    CORRADE_COMPARE(data.attributeData().size(), 4);
    CORRADE_COMPARE(data.attributeData().data(), attributes);
    CORRADE_COMPARE(data.importerState(), &state);

    /* We sorted the input already */
    CORRADE_COMPARE(data.attributeName(0), "AmbientTextureMatrix");
    CORRADE_COMPARE(data.attributeName(1), "DiffuseTextureCoordinates");
    CORRADE_COMPARE(data.attributeName(2), "DoubleSided");
    CORRADE_COMPARE(data.attributeName(3), "highlightColor");

    /* No need to verify the contents as there's no difference in access in
       owned vs non-owned */
}

void MaterialDataTest::constructNonOwnedLayers() {
    constexpr MaterialAttributeData attributes[]{
        {"DiffuseCoordinateSet"_s, 5u},
        {"DoubleSided"_s, true},

        {" LayerName"_s, "ClearCoat"_s},
        {"AlphaBlend"_s, true},
        {"highlightColor"_s, Vector4{0.2f, 0.6f, 0.4f, 1.0f}},

        /* Empty layer here */

        /* Unnamed but nonempty layer */
        {"NormalTexture"_s, 3u},
        {"thickness"_s, 0.015f}
    };

    constexpr UnsignedInt layers[]{
        2, 5, 5, 7
    };

    int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    MaterialData data{MaterialType::Phong,
        {}, attributes,
        {}, layers, &state};

    CORRADE_COMPARE(data.attributeDataFlags(), DataFlags{});
    CORRADE_COMPARE(data.layerDataFlags(), DataFlags{});
    /* Expecting the same output as in constructLayers() */
    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_COMPARE(data.importerState(), &state);

    CORRADE_COMPARE(data.layerCount(), 4);
    CORRADE_COMPARE(data.layerData().size(), 4);
    CORRADE_COMPARE(data.layerData().data(), layers);

    CORRADE_COMPARE(data.attributeData().size(), 7);
    CORRADE_COMPARE(data.attributeData().data(), attributes);
    CORRADE_COMPARE(data.attributeCount(0), 2);
    CORRADE_COMPARE(data.attributeCount(1), 3);
    CORRADE_COMPARE(data.attributeCount(2), 0);
    CORRADE_COMPARE(data.attributeCount(3), 2);

    /* Layer access */
    CORRADE_COMPARE(data.layerName(0), "");
    CORRADE_COMPARE(data.layerName(1), "ClearCoat");
    CORRADE_COMPARE(data.layerName(2), "");
    CORRADE_COMPARE(data.layerName(3), "");

    /* We sorted the input already */
    CORRADE_COMPARE(data.attributeName(0, 0), "DiffuseCoordinateSet");
    CORRADE_COMPARE(data.attributeName(0, 1), "DoubleSided");

    CORRADE_COMPARE(data.attributeName(1, 0), " LayerName");
    CORRADE_COMPARE(data.attributeName(1, 1), "AlphaBlend");
    CORRADE_COMPARE(data.attributeName(1, 2), "highlightColor");

    CORRADE_COMPARE(data.attributeName(3, 0), "NormalTexture");
    CORRADE_COMPARE(data.attributeName(3, 1), "thickness");

    /* No need to verify the contents as there's no difference in access in
       owned vs non-owned */
}

void MaterialDataTest::constructNonOwnedEmptyAttribute() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialAttributeData attributes[]{
        {"DiffuseTexture"_s, 12u},
        MaterialAttributeData{}
    };

    Containers::String out;
    Error redirectError{&out};
    /* nullptr to avoid attributes interpreted as importerState */
    MaterialData{{}, {}, attributes, nullptr};
    CORRADE_COMPARE(out, "Trade::MaterialData: attribute 1 doesn't specify anything\n");
}

void MaterialDataTest::constructNonOwnedNotSorted() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialAttributeData attributes[]{
        {"DiffuseTextureCoordinates"_s, 5u},
        {"DiffuseTexture"_s, 12u}
    };

    Containers::String out;
    Error redirectError{&out};
    /* nullptr to avoid attributes interpreted as importerState */
    MaterialData{{}, {}, attributes, nullptr};
    CORRADE_COMPARE(out, "Trade::MaterialData: DiffuseTexture has to be sorted before DiffuseTextureCoordinates if passing non-owned data\n");
}

void MaterialDataTest::constructNonOwnedDuplicateAttribute() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialAttributeData attributes[]{
        {"DiffuseTexture"_s, 35u},
        {"DiffuseTextureCoordinates"_s, 5u},
        {"DiffuseTextureCoordinates"_s, 12u}
    };

    Containers::String out;
    Error redirectError{&out};
    /* nullptr to avoid attributes interpreted as importerState */
    MaterialData{{}, {}, attributes, nullptr};
    CORRADE_COMPARE(out, "Trade::MaterialData: duplicate attribute DiffuseTextureCoordinates\n");
}

void MaterialDataTest::constructNonOwnedLayersNotMonotonic() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialAttributeData attributes[]{
        {MaterialAttribute::AlphaBlend, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::NormalTexture, 3u}
    };

    UnsignedInt layers[]{
        2, 5, 4, 5
    };

    Containers::String out;
    Error redirectError{&out};
    MaterialData data{MaterialType::Phong,
        {}, attributes,
        {}, layers};
    CORRADE_COMPARE(out, "Trade::MaterialData: invalid range (5, 4) for layer 2 with 5 attributes in total\n");
}

void MaterialDataTest::constructNonOwnedLayersOffsetOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialAttributeData attributes[]{
        {MaterialAttribute::AlphaBlend, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::NormalTexture, 3u}
    };

    UnsignedInt layers[]{
        2, 6
    };

    Containers::String out;
    Error redirectError{&out};
    MaterialData data{MaterialType::Phong,
        {}, attributes,
        {}, layers};
    CORRADE_COMPARE(out, "Trade::MaterialData: invalid range (2, 6) for layer 1 with 5 attributes in total\n");
}

void MaterialDataTest::constructNonOwnedLayersLastOffsetTooShort() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialAttributeData attributes[]{
        {MaterialAttribute::DoubleSided, true},
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::LayerFactor, 0.5f},
        {MaterialAttribute::NormalTexture, 3u}
    };

    UnsignedInt layers[]{
        1, 3
    };

    Containers::String out;
    Error redirectError{&out};
    MaterialData data{MaterialType::Phong,
        {}, attributes,
        {}, layers};
    CORRADE_COMPARE(out, "Trade::MaterialData: last layer offset 3 too short for 4 attributes in total\n");
}

void MaterialDataTest::constructNonOwnedAttributeFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialAttributeData attributes[]{
        {MaterialAttribute::DoubleSided, true}
    };

    Containers::String out;
    Error redirectError{&out};
    MaterialData data{{}, DataFlag::Owned, attributes};
    CORRADE_COMPARE(out, "Trade::MaterialData: can't construct with non-owned attribute data but Trade::DataFlag::Owned\n");
}

void MaterialDataTest::constructNonOwnedLayerFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialAttributeData attributes[]{
        {MaterialAttribute::DoubleSided, true}
    };

    UnsignedInt layers[]{
        0, 1
    };

    Containers::String out;
    Error redirectError{&out};
    MaterialData data{{}, {}, attributes, DataFlag::Owned, layers};
    CORRADE_COMPARE(out, "Trade::MaterialData: can't construct with non-owned layer data but Trade::DataFlag::Owned\n");
}

void MaterialDataTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<MaterialData>{});
    CORRADE_VERIFY(!std::is_copy_assignable<MaterialData>{});
}

void MaterialDataTest::constructMove() {
    int state;
    MaterialData a{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::AlphaBlend, true},
        {"boredomFactor", 5}
    }, {
        1, 1, 3
    }, &state};

    MaterialData b{Utility::move(a)};
    CORRADE_COMPARE(b.attributeDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(b.layerDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(a.layerCount(), 1);
    CORRADE_COMPARE(a.attributeCount(), 0);
    CORRADE_COMPARE(b.types(), MaterialType::Phong);
    CORRADE_COMPARE(b.layerCount(), 3);
    CORRADE_COMPARE(b.attributeCount(2), 2);
    CORRADE_COMPARE(b.attributeName(2, 0), "AlphaBlend");
    CORRADE_COMPARE(b.importerState(), &state);

    MaterialData c{MaterialTypes{}, {
        {MaterialAttribute::AlphaMask, 0.5f}
    }, {1}};
    c = Utility::move(b);
    CORRADE_COMPARE(b.attributeCount(), 1);
    CORRADE_COMPARE(b.layerCount(), 1);
    CORRADE_COMPARE(c.attributeDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(c.layerDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(c.types(), MaterialType::Phong);
    CORRADE_COMPARE(c.layerCount(), 3);
    CORRADE_COMPARE(c.attributeCount(2), 2);
    CORRADE_COMPARE(c.attributeName(2, 0), "AlphaBlend");
    CORRADE_COMPARE(c.importerState(), &state);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<MaterialData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<MaterialData>::value);
}

void MaterialDataTest::as() {
    int state;
    MaterialData data{MaterialType::Phong|MaterialType::PbrSpecularGlossiness, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::SpecularColor, 0x33556600_rgbaf},

        {MaterialAttribute::LayerName, "ClearCoat"},
        {"highlightColor", 0x335566ff_rgbaf}
    }, {
        2, 4
    }, &state};

    auto& phong = data.as<PhongMaterialData>();
    CORRADE_COMPARE(phong.importerState(), &state);
    CORRADE_COMPARE(phong.layerCount(), 2);
    CORRADE_COMPARE(phong.diffuseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(phong.attribute<Color4>("ClearCoat", "highlightColor"), 0x335566ff_rgbaf);

    auto& specularGlossiness = data.as<PbrSpecularGlossinessMaterialData>();
    CORRADE_COMPARE(specularGlossiness.importerState(), &state);
    CORRADE_COMPARE(specularGlossiness.layerCount(), 2);
    CORRADE_COMPARE(specularGlossiness.diffuseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(specularGlossiness.attribute<Color4>("ClearCoat", "highlightColor"), 0x335566ff_rgbaf);
}

void MaterialDataTest::asRvalue() {
    int state;
    MaterialData data{MaterialType::Phong|MaterialType::PbrSpecularGlossiness, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::SpecularColor, 0x33556600_rgbaf},

        {MaterialAttribute::LayerName, "ClearCoat"},
        {"highlightColor", 0x335566ff_rgbaf}
    }, {
        2, 4
    }, &state};

    auto phong = Utility::move(data).as<PhongMaterialData>();
    CORRADE_COMPARE(data.layerCount(), 1);
    CORRADE_COMPARE(phong.layerCount(), 2);
    CORRADE_COMPARE(phong.diffuseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(phong.attribute<Color4>("ClearCoat", "highlightColor"), 0x335566ff_rgbaf);

    auto specularGlossiness = Utility::move(phong).as<PbrSpecularGlossinessMaterialData>();
    CORRADE_COMPARE(phong.layerCount(), 1);
    CORRADE_COMPARE(specularGlossiness.layerCount(), 2);
    CORRADE_COMPARE(specularGlossiness.diffuseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(specularGlossiness.attribute<Color4>("ClearCoat", "highlightColor"), 0x335566ff_rgbaf);
}

void MaterialDataTest::access() {
    MaterialData a{{}, {
        {MaterialAttribute::DoubleSided, false},
        {MaterialAttribute::AlphaBlend, true},
        {MaterialAttribute::AlphaMask, 0.9f}
    }};
    CORRADE_VERIFY(!a.isDoubleSided());
    CORRADE_COMPARE(a.alphaMode(), MaterialAlphaMode::Blend);
    CORRADE_COMPARE(a.alphaMask(), 0.9f);

    MaterialData b{{}, {
        {MaterialAttribute::AlphaBlend, false},
        {MaterialAttribute::AlphaMask, 0.3f}
    }};
    CORRADE_VERIFY(!b.isDoubleSided());
    CORRADE_COMPARE(b.alphaMode(), MaterialAlphaMode::Mask);
    CORRADE_COMPARE(b.alphaMask(), 0.3f);

    MaterialData c{{}, {
        {MaterialAttribute::DoubleSided, true},
    }};
    CORRADE_VERIFY(c.isDoubleSided());
    CORRADE_COMPARE(c.alphaMode(), MaterialAlphaMode::Opaque);
    CORRADE_COMPARE(c.alphaMask(), 0.5f);
}

void MaterialDataTest::accessPointer() {
    const Float a = 3.0f;
    Long b = -4;

    MaterialData data{{}, {
        {"pointer", &a},
        {"mutable", &b}
    }};
    CORRADE_COMPARE(data.attributeType("pointer"), MaterialAttributeType::Pointer);
    CORRADE_COMPARE(data.attributeType("mutable"), MaterialAttributeType::MutablePointer);

    CORRADE_COMPARE(*static_cast<const Float* const*>(data.attribute("pointer")), &a);
    CORRADE_COMPARE(*static_cast<Long* const*>(data.attribute("mutable")), &b);
    CORRADE_COMPARE(data.attribute<const Float*>("pointer"), &a);
    CORRADE_COMPARE(data.attribute<Long*>("mutable"), &b);
}

void MaterialDataTest::accessString() {
    MaterialData data{{}, {
        {"name?", "THIS IS\0WHO I AM!"_s}
    }};
    CORRADE_COMPARE(data.attributeType("name?"), MaterialAttributeType::String);

    /* Pointer access will stop at the first null byte, but typed access won't */
    CORRADE_COMPARE(static_cast<const char*>(data.attribute(0)), "THIS IS"_s);
    CORRADE_COMPARE(data.attribute<Containers::StringView>(0), "THIS IS\0WHO I AM!"_s);
    CORRADE_COMPARE(data.attribute<Containers::StringView>(0).flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(data.attribute<Containers::StringView>(0)[data.attribute<Containers::StringView>(0).size()], '\0');
}

void MaterialDataTest::accessBuffer() {
    MaterialData data{{}, {
        {"name?", Containers::arrayView({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f})}
    }};
    CORRADE_COMPARE(data.attributeType("name?"), MaterialAttributeType::Buffer);

    /* The pointer should be aligned */
    CORRADE_COMPARE_AS(data.attribute(0), 4, TestSuite::Compare::Aligned);
    CORRADE_COMPARE(static_cast<const Float*>(data.attribute(0))[5], 5.0f);
    CORRADE_COMPARE_AS(Containers::arrayCast<const Float>(data.attribute<Containers::ArrayView<const void>>(0)), Containers::arrayView({
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f
    }), TestSuite::Compare::Container);
}

void MaterialDataTest::accessTextureSwizzle() {
    MaterialData data{{}, {
        {"normalSwizzle", MaterialTextureSwizzle::BA}
    }};
    CORRADE_COMPARE(data.attributeType("normalSwizzle"), MaterialAttributeType::TextureSwizzle);

    /* Pointer access will stop at the first null byte, printing the string
       value */
    CORRADE_COMPARE(static_cast<const char*>(data.attribute(0)), "BA"_s);
    CORRADE_COMPARE(*static_cast<const MaterialTextureSwizzle*>(data.attribute(0)), MaterialTextureSwizzle::BA);
    CORRADE_COMPARE(data.attribute<MaterialTextureSwizzle>(0), MaterialTextureSwizzle::BA);
}

void MaterialDataTest::accessMutable() {
    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "aye"_s},
        {MaterialAttribute::Roughness, 1.0f},
        /** @todo test builtin buffer attribute once it exists */
        {"data", Containers::arrayView({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f})},
    }};

    *static_cast<Float*>(data.mutableAttribute(1)) *= 2.0f;
    *static_cast<Float*>(data.mutableAttribute(MaterialAttribute::Roughness)) *= 2.0f;
    *static_cast<Float*>(data.mutableAttribute("Roughness")) *= 2.0f;
    data.mutableAttribute<Float>(1) *= 2.0f;
    data.mutableAttribute<Float>(MaterialAttribute::Roughness) *= 2.0f;
    data.mutableAttribute<Float>("Roughness") *= 2.0f;
    CORRADE_COMPARE(data.attribute<Float>(MaterialAttribute::Roughness), 64.0f);

    ++*static_cast<char*>(data.mutableAttribute(0));
    ++*static_cast<char*>(data.mutableAttribute(MaterialAttribute::LayerName));
    ++*static_cast<char*>(data.mutableAttribute(" LayerName"));
    ++data.mutableAttribute<Containers::MutableStringView>(0)[0];
    ++data.mutableAttribute<Containers::MutableStringView>(MaterialAttribute::LayerName)[0];
    ++data.mutableAttribute<Containers::MutableStringView>(" LayerName")[0];
    CORRADE_COMPARE(data.attribute<Containers::StringView>(MaterialAttribute::LayerName), "gye"_s);

    static_cast<Float*>(data.mutableAttribute(2))[1] *= 2.0f;
    /** @todo test also builtin buffer attribute access once it exists */
    static_cast<Float*>(data.mutableAttribute("data"))[2] *= 2.0f;
    Containers::arrayCast<Float>(data.mutableAttribute<Containers::ArrayView<void>>(2))[3] *= 2.0f;
    /** @todo test also builtin buffer attribute access once it exists */
    Containers::arrayCast<Float>(data.mutableAttribute<Containers::ArrayView<void>>("data"))[4] *= 2.0f;
    CORRADE_COMPARE_AS(Containers::arrayCast<const Float>(data.attribute<Containers::ArrayView<const void>>("data")), Containers::arrayView({
        0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 5.0f, 6.0f
    }), TestSuite::Compare::Container);
}

void MaterialDataTest::accessOptional() {
    MaterialData data{{}, {
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }};

    /* This exists */
    CORRADE_VERIFY(data.findAttribute("SpecularTexture"));
    CORRADE_VERIFY(data.findAttribute(MaterialAttribute::SpecularTexture));
    CORRADE_COMPARE(*static_cast<const Int*>(data.findAttribute("SpecularTexture")), 3);
    CORRADE_COMPARE(*static_cast<const Int*>(data.findAttribute(MaterialAttribute::SpecularTexture)), 3);
    CORRADE_COMPARE(data.findAttribute<UnsignedInt>("SpecularTexture"), 3);
    CORRADE_COMPARE(data.findAttribute<UnsignedInt>(MaterialAttribute::SpecularTexture), 3);
    CORRADE_COMPARE(data.attributeOr("SpecularTexture", 5u), 3);
    CORRADE_COMPARE(data.attributeOr(MaterialAttribute::SpecularTexture, 5u), 3);

    /* This doesn't */
    CORRADE_VERIFY(!data.findAttribute("DiffuseTexture"));
    CORRADE_VERIFY(!data.findAttribute(MaterialAttribute::DiffuseTexture));
    CORRADE_VERIFY(!data.findAttribute<UnsignedInt>("DiffuseTexture"));
    CORRADE_VERIFY(!data.findAttribute<UnsignedInt>(MaterialAttribute::DiffuseTexture));
    CORRADE_COMPARE(data.attributeOr("DiffuseTexture", 5u), 5);
    CORRADE_COMPARE(data.attributeOr(MaterialAttribute::DiffuseTexture, 5u), 5);
}

void MaterialDataTest::accessOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }};

    Containers::String out;
    Error redirectError{&out};
    data.attributeName(2);
    data.attributeType(2);
    data.attribute(2);
    data.attribute<Int>(2);
    data.attribute<Containers::StringView>(2);
    data.mutableAttribute(2);
    data.mutableAttribute<Int>(2);
    data.mutableAttribute<Containers::MutableStringView>(2);
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attributeName(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::attributeType(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 attributes in layer 0\n");
}

void MaterialDataTest::accessNotFound() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {"DiffuseColor", 0xff3366aa_rgbaf}
    }};

    /* These are fine */
    CORRADE_VERIFY(!data.hasAttribute("DiffuseColour"));
    CORRADE_VERIFY(!data.findAttributeId("DiffuseColour"));

    Containers::String out;
    Error redirectError{&out};
    data.attributeId("DiffuseColour");
    data.attributeType("DiffuseColour");
    data.attribute("DiffuseColour");
    data.attribute<Color4>("DiffuseColour");
    data.mutableAttribute("DiffuseColour");
    data.mutableAttribute<Color4>("DiffuseColour");
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attributeId(): attribute DiffuseColour not found in layer 0\n"
        "Trade::MaterialData::attributeType(): attribute DiffuseColour not found in layer 0\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer 0\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer 0\n"
        "Trade::MaterialData::mutableAttribute(): attribute DiffuseColour not found in layer 0\n"
        "Trade::MaterialData::mutableAttribute(): attribute DiffuseColour not found in layer 0\n");
}

void MaterialDataTest::accessWrongType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {"DiffuseColor", 0xff3366aa_rgbaf}
    }};

    Containers::String out;
    Error redirectError{&out};
    data.attribute<Color3>(0);
    data.attribute<Color3>(MaterialAttribute::DiffuseColor);
    data.attribute<Color3>("DiffuseColor");
    data.mutableAttribute<Color3>(0);
    data.mutableAttribute<Color3>(MaterialAttribute::DiffuseColor);
    data.mutableAttribute<Color3>("DiffuseColor");
    data.findAttribute<Color3>(MaterialAttribute::DiffuseColor);
    data.findAttribute<Color3>("DiffuseColor");
    data.attributeOr(MaterialAttribute::DiffuseColor, Color3{1.0f});
    data.attributeOr("DiffuseColor", Color3{1.0f});
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attribute(): DiffuseColor is Trade::MaterialAttributeType::Vector4 but requested a type equivalent to Trade::MaterialAttributeType::Vector3\n"
        "Trade::MaterialData::attribute(): DiffuseColor is Trade::MaterialAttributeType::Vector4 but requested a type equivalent to Trade::MaterialAttributeType::Vector3\n"
        "Trade::MaterialData::attribute(): DiffuseColor is Trade::MaterialAttributeType::Vector4 but requested a type equivalent to Trade::MaterialAttributeType::Vector3\n"
        "Trade::MaterialData::mutableAttribute(): DiffuseColor is Trade::MaterialAttributeType::Vector4 but requested a type equivalent to Trade::MaterialAttributeType::Vector3\n"
        "Trade::MaterialData::mutableAttribute(): DiffuseColor is Trade::MaterialAttributeType::Vector4 but requested a type equivalent to Trade::MaterialAttributeType::Vector3\n"
        "Trade::MaterialData::mutableAttribute(): DiffuseColor is Trade::MaterialAttributeType::Vector4 but requested a type equivalent to Trade::MaterialAttributeType::Vector3\n"
        /* findAttribute() and attributeOr() delegate to attribute() so the
           assert is the same */
        "Trade::MaterialData::attribute(): DiffuseColor is Trade::MaterialAttributeType::Vector4 but requested a type equivalent to Trade::MaterialAttributeType::Vector3\n"
        "Trade::MaterialData::attribute(): DiffuseColor is Trade::MaterialAttributeType::Vector4 but requested a type equivalent to Trade::MaterialAttributeType::Vector3\n"
        "Trade::MaterialData::attribute(): DiffuseColor is Trade::MaterialAttributeType::Vector4 but requested a type equivalent to Trade::MaterialAttributeType::Vector3\n"
        "Trade::MaterialData::attribute(): DiffuseColor is Trade::MaterialAttributeType::Vector4 but requested a type equivalent to Trade::MaterialAttributeType::Vector3\n");
}

void MaterialDataTest::accessWrongPointerType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Int a = 3;
    const Double b = 57.0;

    MaterialData data{{}, {
        {"mutablePointer", &a},
        {"pointer", &b}
    }};

    /* These are fine (type is not checked) */
    data.attribute<Byte*>("mutablePointer");
    data.attribute<const Float*>("pointer");
    data.mutableAttribute<Byte*>("mutablePointer");
    data.mutableAttribute<const Float*>("pointer");

    Containers::String out;
    Error redirectError{&out};
    data.attribute<Int>("mutablePointer");
    data.attribute<const Int*>("mutablePointer");
    data.attribute<Double*>("pointer");
    data.mutableAttribute<Int>("mutablePointer");
    data.mutableAttribute<const Int*>("mutablePointer");
    data.mutableAttribute<Double*>("pointer");
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attribute(): mutablePointer is Trade::MaterialAttributeType::MutablePointer but requested a type equivalent to Trade::MaterialAttributeType::Int\n"
        "Trade::MaterialData::attribute(): mutablePointer is Trade::MaterialAttributeType::MutablePointer but requested a type equivalent to Trade::MaterialAttributeType::Pointer\n"
        "Trade::MaterialData::attribute(): pointer is Trade::MaterialAttributeType::Pointer but requested a type equivalent to Trade::MaterialAttributeType::MutablePointer\n"
        "Trade::MaterialData::mutableAttribute(): mutablePointer is Trade::MaterialAttributeType::MutablePointer but requested a type equivalent to Trade::MaterialAttributeType::Int\n"
        "Trade::MaterialData::mutableAttribute(): mutablePointer is Trade::MaterialAttributeType::MutablePointer but requested a type equivalent to Trade::MaterialAttributeType::Pointer\n"
        "Trade::MaterialData::mutableAttribute(): pointer is Trade::MaterialAttributeType::Pointer but requested a type equivalent to Trade::MaterialAttributeType::MutablePointer\n");
}

void MaterialDataTest::accessWrongTypeString() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {"Shininess", 0.0f}
    }};

    Containers::String out;
    Error redirectError{&out};
    data.attribute<Containers::StringView>(0);
    data.attribute<Containers::StringView>(MaterialAttribute::Shininess);
    data.attribute<Containers::StringView>("Shininess");
    data.mutableAttribute<Containers::MutableStringView>(0);
    data.mutableAttribute<Containers::MutableStringView>(MaterialAttribute::Shininess);
    data.mutableAttribute<Containers::MutableStringView>("Shininess");
    data.findAttribute<Containers::StringView>(MaterialAttribute::Shininess);
    data.findAttribute<Containers::StringView>("Shininess");
    data.attributeOr(MaterialAttribute::Shininess, Containers::StringView{});
    data.attributeOr("Shininess", Containers::StringView{});
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::mutableAttribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::mutableAttribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::mutableAttribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        /* findAttribute() and attributeOr() delegate to attribute() so the
           assert is the same */
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n");
}

void MaterialDataTest::accessWrongTypeBuffer() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {"Shininess", 0.0f}
    }};

    Containers::String out;
    Error redirectError{&out};
    data.attribute<Containers::ArrayView<const void>>(0);
    data.attribute<Containers::ArrayView<const void>>(MaterialAttribute::Shininess);
    data.attribute<Containers::ArrayView<const void>>("Shininess");
    data.mutableAttribute<Containers::ArrayView<void>>(0);
    data.mutableAttribute<Containers::ArrayView<void>>(MaterialAttribute::Shininess);
    data.mutableAttribute<Containers::ArrayView<void>>("Shininess");
    data.findAttribute<Containers::ArrayView<const void>>(MaterialAttribute::Shininess);
    data.findAttribute<Containers::ArrayView<const void>>("Shininess");
    data.attributeOr(MaterialAttribute::Shininess, Containers::ArrayView<const void>{});
    data.attributeOr("Shininess", Containers::ArrayView<const void>{});
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a buffer\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a buffer\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a buffer\n"
        "Trade::MaterialData::mutableAttribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a buffer\n"
        "Trade::MaterialData::mutableAttribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a buffer\n"
        "Trade::MaterialData::mutableAttribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a buffer\n"
        /* findAttribute() and attributeOr() delegate to attribute() so the
           assert is the same */
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a buffer\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a buffer\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a buffer\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a buffer\n");
}

void MaterialDataTest::accessLayers() {
    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "decals"},

        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::LayerFactor, 0.5f},
    }, {
        0, 1, 3
    }};

    CORRADE_COMPARE(data.layerCount(), 3);

    CORRADE_COMPARE(data.layerFactor(2), 0.5f);
    CORRADE_COMPARE(data.layerFactor("ClearCoat"), 0.5f);
    CORRADE_COMPARE(data.layerFactor(MaterialLayer::ClearCoat), 0.5f);
}

void MaterialDataTest::accessLayersDefaults() {
    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "ClearCoat"},
    }, {
        0, 1
    }};

    CORRADE_COMPARE(data.layerCount(), 2);

    CORRADE_COMPARE(data.layerFactor(1), 1.0f);
    CORRADE_COMPARE(data.layerFactor("ClearCoat"), 1.0f);
    CORRADE_COMPARE(data.layerFactor(MaterialLayer::ClearCoat), 1.0f);
}

void MaterialDataTest::accessLayersTextured() {
    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "decals"},

        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::LayerFactor, 0.5f},
        {MaterialAttribute::LayerFactorTexture, 4u},
        {MaterialAttribute::LayerFactorTextureSwizzle, MaterialTextureSwizzle::A},
        {MaterialAttribute::LayerFactorTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::LayerFactorTextureCoordinates, 2u},
        {MaterialAttribute::LayerFactorTextureLayer, 3u},
    }, {
        0, 1, 8
    }};

    CORRADE_COMPARE(data.layerCount(), 3);

    CORRADE_COMPARE(data.layerFactor(2), 0.5f);
    CORRADE_COMPARE(data.layerFactor("ClearCoat"), 0.5f);
    CORRADE_COMPARE(data.layerFactor(MaterialLayer::ClearCoat), 0.5f);

    CORRADE_COMPARE(data.layerFactorTexture(2), 4u);
    CORRADE_COMPARE(data.layerFactorTexture("ClearCoat"), 4u);
    CORRADE_COMPARE(data.layerFactorTexture(MaterialLayer::ClearCoat), 4u);

    CORRADE_COMPARE(data.layerFactorTextureSwizzle(2), MaterialTextureSwizzle::A);
    CORRADE_COMPARE(data.layerFactorTextureSwizzle("ClearCoat"), MaterialTextureSwizzle::A);
    CORRADE_COMPARE(data.layerFactorTextureSwizzle(MaterialLayer::ClearCoat), MaterialTextureSwizzle::A);

    CORRADE_COMPARE(data.layerFactorTextureMatrix(2), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureMatrix("ClearCoat"), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureMatrix(MaterialLayer::ClearCoat), Matrix3::scaling({0.5f, 1.0f}));

    CORRADE_COMPARE(data.layerFactorTextureCoordinates(2), 2u);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates("ClearCoat"), 2u);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates(MaterialLayer::ClearCoat), 2u);

    CORRADE_COMPARE(data.layerFactorTextureLayer(2), 3u);
    CORRADE_COMPARE(data.layerFactorTextureLayer("ClearCoat"), 3u);
    CORRADE_COMPARE(data.layerFactorTextureLayer(MaterialLayer::ClearCoat), 3u);
}

void MaterialDataTest::accessLayersTexturedDefault() {
    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "decals"},

        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::LayerFactorTexture, 3u},
    }, {
        0, 1, 3
    }};

    CORRADE_COMPARE(data.layerCount(), 3);

    CORRADE_COMPARE(data.layerFactor(2), 1.0f);
    CORRADE_COMPARE(data.layerFactor("ClearCoat"), 1.0f);
    CORRADE_COMPARE(data.layerFactor(MaterialLayer::ClearCoat), 1.0f);

    CORRADE_COMPARE(data.layerFactorTexture(2), 3u);
    CORRADE_COMPARE(data.layerFactorTexture("ClearCoat"), 3u);
    CORRADE_COMPARE(data.layerFactorTexture(MaterialLayer::ClearCoat), 3u);

    CORRADE_COMPARE(data.layerFactorTextureSwizzle(2), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.layerFactorTextureSwizzle("ClearCoat"), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.layerFactorTextureSwizzle(MaterialLayer::ClearCoat), MaterialTextureSwizzle::R);

    CORRADE_COMPARE(data.layerFactorTextureMatrix(2), Matrix3{});
    CORRADE_COMPARE(data.layerFactorTextureMatrix("ClearCoat"), Matrix3{});
    CORRADE_COMPARE(data.layerFactorTextureMatrix(MaterialLayer::ClearCoat), Matrix3{});

    CORRADE_COMPARE(data.layerFactorTextureCoordinates(2), 0);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates("ClearCoat"), 0);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates(MaterialLayer::ClearCoat), 0);

    CORRADE_COMPARE(data.layerFactorTextureLayer(2), 0);
    CORRADE_COMPARE(data.layerFactorTextureLayer("ClearCoat"), 0);
    CORRADE_COMPARE(data.layerFactorTextureLayer(MaterialLayer::ClearCoat), 0);
}

void MaterialDataTest::accessLayersTexturedSingleMatrixCoordinatesLayer() {
    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::LayerFactorTexture, 4u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::TextureCoordinates, 2u},
        {MaterialAttribute::TextureLayer, 3u},
    }, {
        0, 5
    }};

    CORRADE_COMPARE(data.layerFactorTextureMatrix(1), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureMatrix("ClearCoat"), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureMatrix(MaterialLayer::ClearCoat), Matrix3::scaling({0.5f, 1.0f}));

    CORRADE_COMPARE(data.layerFactorTextureCoordinates(1), 2u);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates("ClearCoat"), 2u);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates(MaterialLayer::ClearCoat), 2u);

    CORRADE_COMPARE(data.layerFactorTextureLayer(1), 3u);
    CORRADE_COMPARE(data.layerFactorTextureLayer("ClearCoat"), 3u);
    CORRADE_COMPARE(data.layerFactorTextureLayer(MaterialLayer::ClearCoat), 3u);
}

void MaterialDataTest::accessLayersTexturedBaseMaterialMatrixCoordinatesLayer() {
    MaterialData data{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::TextureCoordinates, 2u},
        {MaterialAttribute::TextureLayer, 3u},

        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::LayerFactorTexture, 4u},
    }, {
        3, 5
    }};

    CORRADE_COMPARE(data.layerFactorTextureMatrix(1), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureMatrix("ClearCoat"), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureMatrix(MaterialLayer::ClearCoat), Matrix3::scaling({0.5f, 1.0f}));

    CORRADE_COMPARE(data.layerFactorTextureCoordinates(1), 2u);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates("ClearCoat"), 2u);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates(MaterialLayer::ClearCoat), 2u);

    CORRADE_COMPARE(data.layerFactorTextureLayer(1), 3u);
    CORRADE_COMPARE(data.layerFactorTextureLayer("ClearCoat"), 3u);
    CORRADE_COMPARE(data.layerFactorTextureLayer(MaterialLayer::ClearCoat), 3u);
}

void MaterialDataTest::accessLayersInvalidTextures() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "ClearCoat"},
    }, {0, 1}};

    Containers::String out;
    Error redirectError{&out};
    data.layerFactorTexture(1);
    data.layerFactorTexture("ClearCoat");
    data.layerFactorTexture(MaterialLayer::ClearCoat);
    data.layerFactorTextureSwizzle(1);
    data.layerFactorTextureSwizzle("ClearCoat");
    data.layerFactorTextureSwizzle(MaterialLayer::ClearCoat);
    data.layerFactorTextureMatrix(1);
    data.layerFactorTextureMatrix("ClearCoat");
    data.layerFactorTextureMatrix(MaterialLayer::ClearCoat);
    data.layerFactorTextureCoordinates(1);
    data.layerFactorTextureCoordinates("ClearCoat");
    data.layerFactorTextureCoordinates(MaterialLayer::ClearCoat);
    data.layerFactorTextureLayer(1);
    data.layerFactorTextureLayer("ClearCoat");
    data.layerFactorTextureLayer(MaterialLayer::ClearCoat);
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attribute(): attribute LayerFactorTexture not found in layer 1\n"
        "Trade::MaterialData::attribute(): attribute LayerFactorTexture not found in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): attribute LayerFactorTexture not found in layer ClearCoat\n"
        "Trade::MaterialData::layerFactorTextureSwizzle(): layer 1 doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureSwizzle(): layer ClearCoat doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureSwizzle(): layer ClearCoat doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureMatrix(): layer 1 doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureMatrix(): layer ClearCoat doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureMatrix(): layer ClearCoat doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer 1 doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer ClearCoat doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer ClearCoat doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureLayer(): layer 1 doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureLayer(): layer ClearCoat doesn't have a factor texture\n"
        "Trade::MaterialData::layerFactorTextureLayer(): layer ClearCoat doesn't have a factor texture\n");
}

void MaterialDataTest::accessLayerLayerNameInBaseMaterial() {
    MaterialData data{{}, {
        {MaterialAttribute::Shininess, 50.0f},
        {MaterialAttribute::LayerName, "base material name"}
    }};

    /* To avoid confusing the base material with a layer, LayerName is ignored
       for the base material. */
    CORRADE_COMPARE(data.layerName(0), "");
    CORRADE_VERIFY(!data.hasLayer("base material name"));
}

void MaterialDataTest::accessLayerEmptyLayer() {
    /* If a layer is empty, its contents shouldn't leak into upper layers */
    MaterialData data{{}, {
        {MaterialAttribute::NormalTexture, 3u},
        {MaterialAttribute::LayerName, "crumples"}
    }, {0, 0, 2}};

    CORRADE_COMPARE(data.layerName(0), "");
    CORRADE_COMPARE(data.layerName(1), "");
    CORRADE_COMPARE(data.layerName(2), "crumples");
    CORRADE_COMPARE(data.attributeCount(0), 0);
    CORRADE_COMPARE(data.attributeCount(1), 0);
    CORRADE_COMPARE(data.attributeCount(2), 2);
    CORRADE_COMPARE(data.layerId("crumples"), 2);
    CORRADE_COMPARE(data.attribute<UnsignedInt>("crumples", MaterialAttribute::NormalTexture), 3u);
}

void MaterialDataTest::accessLayerIndexMutable() {
    MaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::Roughness, 1.0f}
    }, {0, 2}};

    *static_cast<Float*>(data.mutableAttribute(1, 1)) *= 2.0f;
    *static_cast<Float*>(data.mutableAttribute(1, "Roughness")) *= 2.0f;
    *static_cast<Float*>(data.mutableAttribute(1, MaterialAttribute::Roughness)) *= 2.0f;
    data.mutableAttribute<Float>(1, 1) *= 2.0f;
    data.mutableAttribute<Float>(1, "Roughness") *= 2.0f;
    data.mutableAttribute<Float>(1, MaterialAttribute::Roughness) *= 2.0f;
    CORRADE_COMPARE(data.attribute<Float>(1, MaterialAttribute::Roughness), 64.0f);

    ++*static_cast<char*>(data.mutableAttribute(1, 0));
    ++*static_cast<char*>(data.mutableAttribute(1, " LayerName"));
    ++*static_cast<char*>(data.mutableAttribute(1, MaterialAttribute::LayerName));
    ++data.mutableAttribute<Containers::MutableStringView>(1, 0)[0];
    ++data.mutableAttribute<Containers::MutableStringView>(1, " LayerName")[0];
    ++data.mutableAttribute<Containers::MutableStringView>(1, MaterialAttribute::LayerName)[0];
    CORRADE_COMPARE(data.attribute<Containers::StringView>(1, MaterialAttribute::LayerName), "IlearCoat"_s);
}

void MaterialDataTest::accessLayerNameMutable() {
    MaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::Roughness, 1.0f}
    }, {0, 2}};

    *static_cast<Float*>(data.mutableAttribute(MaterialLayer::ClearCoat, 1)) *= 2.0f;
    *static_cast<Float*>(data.mutableAttribute(MaterialLayer::ClearCoat, "Roughness")) *= 2.0f;
    *static_cast<Float*>(data.mutableAttribute(MaterialLayer::ClearCoat, MaterialAttribute::Roughness)) *= 2.0f;
    data.mutableAttribute<Float>(MaterialLayer::ClearCoat, 1) *= 2.0f;
    data.mutableAttribute<Float>(MaterialLayer::ClearCoat, "Roughness") *= 2.0f;
    data.mutableAttribute<Float>(MaterialLayer::ClearCoat, MaterialAttribute::Roughness) *= 2.0f;
    CORRADE_COMPARE(data.attribute<Float>(MaterialLayer::ClearCoat, MaterialAttribute::Roughness), 64.0f);

    /* Resetting back so the layer name always stays the same so the next call
       can find it. Other than that, the result should be same as in
       accessLayerIndexMutable(). */
    {
        *static_cast<char*>(data.mutableAttribute(MaterialLayer::ClearCoat, 0)) = 'D';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "DlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        *static_cast<char*>(data.mutableAttribute(MaterialLayer::ClearCoat, " LayerName")) = 'E';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "ElearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        *static_cast<char*>(data.mutableAttribute(MaterialLayer::ClearCoat, MaterialAttribute::LayerName)) = 'F';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "FlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        data.mutableAttribute<Containers::MutableStringView>(MaterialLayer::ClearCoat, 0)[0] = 'G';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "GlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        data.mutableAttribute<Containers::MutableStringView>(MaterialLayer::ClearCoat, " LayerName")[0] = 'H';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "HlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        data.mutableAttribute<Containers::MutableStringView>(MaterialLayer::ClearCoat, " LayerName")[0] = 'I';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "IlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    }
}

void MaterialDataTest::accessLayerStringMutable() {
    MaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::Roughness, 1.0f}
    }, {0, 2}};

    *static_cast<Float*>(data.mutableAttribute("ClearCoat", 1)) *= 2.0f;
    *static_cast<Float*>(data.mutableAttribute("ClearCoat", "Roughness")) *= 2.0f;
    *static_cast<Float*>(data.mutableAttribute("ClearCoat", MaterialAttribute::Roughness)) *= 2.0f;
    data.mutableAttribute<Float>("ClearCoat", 1) *= 2.0f;
    data.mutableAttribute<Float>("ClearCoat", "Roughness") *= 2.0f;
    data.mutableAttribute<Float>("ClearCoat", MaterialAttribute::Roughness) *= 2.0f;
    CORRADE_COMPARE(data.attribute<Float>("ClearCoat", MaterialAttribute::Roughness), 64.0f);

    /* Resetting back so the layer name always stays the same so the next call
       can find it. Other than that, the result should be same as in
       accessLayerIndexMutable(). */
    {
        *static_cast<char*>(data.mutableAttribute("ClearCoat", 0)) = 'D';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "DlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        *static_cast<char*>(data.mutableAttribute("ClearCoat", " LayerName")) = 'E';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "ElearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        *static_cast<char*>(data.mutableAttribute("ClearCoat", MaterialAttribute::LayerName)) = 'F';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "FlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        data.mutableAttribute<Containers::MutableStringView>("ClearCoat", 0)[0] = 'G';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "GlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        data.mutableAttribute<Containers::MutableStringView>("ClearCoat", " LayerName")[0] = 'H';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "HlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        data.mutableAttribute<Containers::MutableStringView>("ClearCoat", " LayerName")[0] = 'I';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "IlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    }
}

void MaterialDataTest::accessLayerIndexOptional() {
    MaterialData data{{}, {
        {MaterialAttribute::DiffuseColor, 0x335566ff_rgbaf},
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }, {1, 3}};

    /* This exists */
    CORRADE_VERIFY(data.findAttribute(1, "SpecularTexture"));
    CORRADE_VERIFY(data.findAttribute(1, MaterialAttribute::SpecularTexture));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.findAttribute(1, "SpecularTexture")), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.findAttribute(1, MaterialAttribute::SpecularTexture)), 3);
    CORRADE_COMPARE(data.findAttribute<UnsignedInt>(1, "SpecularTexture"), 3);
    CORRADE_COMPARE(data.findAttribute<UnsignedInt>(1, MaterialAttribute::SpecularTexture), 3);
    CORRADE_COMPARE(data.attributeOr(1, "SpecularTexture", 5u), 3);
    CORRADE_COMPARE(data.attributeOr(1, MaterialAttribute::SpecularTexture, 5u), 3);

    /* This doesn't */
    CORRADE_VERIFY(!data.findAttribute(1, "DiffuseTexture"));
    CORRADE_VERIFY(!data.findAttribute(1, MaterialAttribute::DiffuseTexture));
    CORRADE_VERIFY(!data.findAttribute<UnsignedInt>(1, "DiffuseTexture"));
    CORRADE_VERIFY(!data.findAttribute<UnsignedInt>(1, MaterialAttribute::DiffuseTexture));
    CORRADE_COMPARE(data.attributeOr(1, "DiffuseTexture", 5u), 5);
    CORRADE_COMPARE(data.attributeOr(1, MaterialAttribute::DiffuseTexture, 5u), 5);
}

void MaterialDataTest::accessLayerNameOptional() {
    MaterialData data{{}, {
        {MaterialAttribute::DiffuseColor, 0x335566ff_rgbaf},
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }, {1, 4}};

    /* This exists */
    CORRADE_VERIFY(data.findAttribute(MaterialLayer::ClearCoat, "SpecularTexture"));
    CORRADE_VERIFY(data.findAttribute(MaterialLayer::ClearCoat, MaterialAttribute::SpecularTexture));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.findAttribute(MaterialLayer::ClearCoat, "SpecularTexture")), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.findAttribute(MaterialLayer::ClearCoat, MaterialAttribute::SpecularTexture)), 3);
    CORRADE_COMPARE(data.findAttribute<UnsignedInt>(MaterialLayer::ClearCoat, "SpecularTexture"), 3);
    CORRADE_COMPARE(data.findAttribute<UnsignedInt>(MaterialLayer::ClearCoat, MaterialAttribute::SpecularTexture), 3);
    CORRADE_COMPARE(data.attributeOr(MaterialLayer::ClearCoat, "SpecularTexture", 5u), 3);
    CORRADE_COMPARE(data.attributeOr(MaterialLayer::ClearCoat, MaterialAttribute::SpecularTexture, 5u), 3);

    /* This doesn't */
    CORRADE_VERIFY(!data.findAttribute(MaterialLayer::ClearCoat, "DiffuseTexture"));
    CORRADE_VERIFY(!data.findAttribute(MaterialLayer::ClearCoat, MaterialAttribute::DiffuseTexture));
    CORRADE_VERIFY(!data.findAttribute<UnsignedInt>(MaterialLayer::ClearCoat, "DiffuseTexture"));
    CORRADE_VERIFY(!data.findAttribute<UnsignedInt>(MaterialLayer::ClearCoat, MaterialAttribute::DiffuseTexture));
    CORRADE_COMPARE(data.attributeOr(MaterialLayer::ClearCoat, "DiffuseTexture", 5u), 5);
    CORRADE_COMPARE(data.attributeOr(MaterialLayer::ClearCoat, MaterialAttribute::DiffuseTexture, 5u), 5);
}

void MaterialDataTest::accessLayerStringOptional() {
    MaterialData data{{}, {
        {MaterialAttribute::DiffuseColor, 0x335566ff_rgbaf},
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }, {1, 4}};

    /* This exists */
    CORRADE_VERIFY(data.findAttribute("ClearCoat", "SpecularTexture"));
    CORRADE_VERIFY(data.findAttribute("ClearCoat", MaterialAttribute::SpecularTexture));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.findAttribute("ClearCoat", "SpecularTexture")), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.findAttribute("ClearCoat", MaterialAttribute::SpecularTexture)), 3);
    CORRADE_COMPARE(data.findAttribute<UnsignedInt>("ClearCoat", "SpecularTexture"), 3);
    CORRADE_COMPARE(data.findAttribute<UnsignedInt>("ClearCoat", MaterialAttribute::SpecularTexture), 3);
    CORRADE_COMPARE(data.attributeOr("ClearCoat", "SpecularTexture", 5u), 3);
    CORRADE_COMPARE(data.attributeOr("ClearCoat", MaterialAttribute::SpecularTexture, 5u), 3);

    /* This doesn't */
    CORRADE_VERIFY(!data.findAttribute("ClearCoat", "DiffuseTexture"));
    CORRADE_VERIFY(!data.findAttribute("ClearCoat", MaterialAttribute::DiffuseTexture));
    CORRADE_VERIFY(!data.findAttribute<UnsignedInt>("ClearCoat", "DiffuseTexture"));
    CORRADE_VERIFY(!data.findAttribute<UnsignedInt>("ClearCoat", MaterialAttribute::DiffuseTexture));
    CORRADE_COMPARE(data.attributeOr("ClearCoat", "DiffuseTexture", 5u), 5);
    CORRADE_COMPARE(data.attributeOr("ClearCoat", MaterialAttribute::DiffuseTexture, 5u), 5);
}

void MaterialDataTest::accessLayerOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }, {0, 2}};

    /* This is fine */
    data.attributeDataOffset(2);

    Containers::String out;
    Error redirectError{&out};
    data.attributeDataOffset(3);
    data.layerName(2);
    data.layerFactor(2);
    data.layerFactorTexture(2);
    data.layerFactorTextureSwizzle(2);
    data.layerFactorTextureMatrix(2);
    data.layerFactorTextureCoordinates(2);
    data.layerFactorTextureLayer(2);
    data.attributeCount(2);
    data.hasAttribute(2, "AlphaMask");
    data.hasAttribute(2, MaterialAttribute::AlphaMask);
    data.findAttributeId(2, "AlphaMask");
    data.findAttributeId(2, MaterialAttribute::AlphaMask);
    data.attributeId(2, "AlphaMask");
    data.attributeId(2, MaterialAttribute::AlphaMask);
    data.attributeData(2, 0);
    data.attributeName(2, 0);
    data.attributeType(2, 0);
    data.attributeType(2, "AlphaMask");
    data.attributeType(2, MaterialAttribute::AlphaMask);
    data.attribute(2, 0);
    data.attribute(2, "AlphaMask");
    data.attribute(2, MaterialAttribute::AlphaMask);
    data.attribute<Int>(2, 0);
    data.attribute<Int>(2, "AlphaMask");
    data.attribute<Int>(2, MaterialAttribute::AlphaMask);
    data.attribute<Containers::StringView>(2, 0);
    data.mutableAttribute(2, 0);
    data.mutableAttribute(2, "AlphaMask");
    data.mutableAttribute(2, MaterialAttribute::AlphaMask);
    data.mutableAttribute<Int>(2, 0);
    data.mutableAttribute<Int>(2, "AlphaMask");
    data.mutableAttribute<Int>(2, MaterialAttribute::AlphaMask);
    data.mutableAttribute<Containers::MutableStringView>(2, 0);
    data.findAttribute(2, "AlphaMask");
    data.findAttribute(2, MaterialAttribute::AlphaMask);
    data.findAttribute<bool>(2, "AlphaMask");
    data.findAttribute<bool>(2, MaterialAttribute::AlphaMask);
    data.attributeOr(2, "AlphaMask", false);
    data.attributeOr(2, MaterialAttribute::AlphaMask, false);
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attributeDataOffset(): index 3 out of range for 2 layers\n"
        "Trade::MaterialData::layerName(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactor(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactorTexture(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactorTextureSwizzle(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactorTextureMatrix(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactorTextureCoordinates(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactorTextureLayer(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeCount(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::hasAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::hasAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::findAttributeId(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::findAttributeId(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeId(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeId(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeData(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeName(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeType(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeType(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeType(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::findAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::findAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::findAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::findAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeOr(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeOr(): index 2 out of range for 2 layers\n");
}

void MaterialDataTest::accessLayerNotFound() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "clearCoat"},
        {MaterialAttribute::AlphaMask, 0.5f},
    }, {0, 2}};

    /* This is fine */
    CORRADE_VERIFY(!data.findLayerId("ClearCoat"));

    Containers::String out;
    Error redirectError{&out};
    data.layerId("ClearCoat");
    data.layerFactor("ClearCoat");
    data.layerFactorTexture("ClearCoat");
    data.layerFactorTextureSwizzle("ClearCoat");
    data.layerFactorTextureMatrix("ClearCoat");
    data.layerFactorTextureCoordinates("ClearCoat");
    data.layerFactorTextureLayer("ClearCoat");
    data.attributeCount("ClearCoat");
    data.hasAttribute("ClearCoat", "AlphaMask");
    data.hasAttribute("ClearCoat", MaterialAttribute::AlphaMask);
    data.findAttributeId("ClearCoat", "AlphaMask");
    data.findAttributeId("ClearCoat", MaterialAttribute::AlphaMask);
    data.attributeId("ClearCoat", "AlphaMask");
    data.attributeId("ClearCoat", MaterialAttribute::AlphaMask);
    data.attributeName("ClearCoat", 0);
    data.attributeType("ClearCoat", 0);
    data.attributeType("ClearCoat", "AlphaMask");
    data.attributeType("ClearCoat", MaterialAttribute::AlphaMask);
    data.attribute("ClearCoat", 0);
    data.attribute("ClearCoat", "AlphaMask");
    data.attribute("ClearCoat", MaterialAttribute::AlphaMask);
    data.attribute<Int>("ClearCoat", 0);
    data.attribute<Int>("ClearCoat", "AlphaMask");
    data.attribute<Int>("ClearCoat", MaterialAttribute::AlphaMask);
    data.attribute<Containers::StringView>("ClearCoat", 0);
    data.mutableAttribute("ClearCoat", 0);
    data.mutableAttribute("ClearCoat", "AlphaMask");
    data.mutableAttribute("ClearCoat", MaterialAttribute::AlphaMask);
    data.mutableAttribute<Int>("ClearCoat", 0);
    data.mutableAttribute<Int>("ClearCoat", "AlphaMask");
    data.mutableAttribute<Int>("ClearCoat", MaterialAttribute::AlphaMask);
    data.mutableAttribute<Containers::MutableStringView>("ClearCoat", 0);
    data.findAttribute("ClearCoat", "AlphaMask");
    data.findAttribute("ClearCoat", MaterialAttribute::AlphaMask);
    data.findAttribute<bool>("ClearCoat", "AlphaMask");
    data.findAttribute<bool>("ClearCoat", MaterialAttribute::AlphaMask);
    data.attributeOr("ClearCoat", "AlphaMask", false);
    data.attributeOr("ClearCoat", MaterialAttribute::AlphaMask, false);
    CORRADE_COMPARE(out,
        "Trade::MaterialData::layerId(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactor(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactorTexture(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactorTextureSwizzle(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactorTextureMatrix(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactorTextureLayer(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeCount(): layer ClearCoat not found\n"
        "Trade::MaterialData::hasAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::hasAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::findAttributeId(): layer ClearCoat not found\n"
        "Trade::MaterialData::findAttributeId(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeId(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeId(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeName(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeType(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeType(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeType(): layer ClearCoat not found\n"
        "Trade::MaterialData::attribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::attribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::attribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::attribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::attribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::attribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::attribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::mutableAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::mutableAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::mutableAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::mutableAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::mutableAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::mutableAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::mutableAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::findAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::findAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::findAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::findAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeOr(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeOr(): layer ClearCoat not found\n");
}

void MaterialDataTest::accessInvalidLayerName() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {}};

    Containers::String out;
    Error redirectError{&out};
    data.findLayerId(MaterialLayer(0x0));
    data.findLayerId(MaterialLayer(0xfefe));
    data.layerId(MaterialLayer(0xfefe));
    data.layerFactor(MaterialLayer(0xfefe));
    data.layerFactorTexture(MaterialLayer(0xfefe));
    data.layerFactorTextureSwizzle(MaterialLayer(0xfefe));
    data.layerFactorTextureMatrix(MaterialLayer(0xfefe));
    data.layerFactorTextureCoordinates(MaterialLayer(0xfefe));
    data.layerFactorTextureLayer(MaterialLayer(0xfefe));
    data.attributeCount(MaterialLayer(0xfefe));
    data.hasAttribute(MaterialLayer(0xfefe), "AlphaMask");
    data.hasAttribute(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.findAttributeId(MaterialLayer(0xfefe), "AlphaMask");
    data.findAttributeId(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.attributeId(MaterialLayer(0xfefe), "AlphaMask");
    data.attributeId(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.attributeName(MaterialLayer(0xfefe), 0);
    data.attributeType(MaterialLayer(0xfefe), 0);
    data.attributeType(MaterialLayer(0xfefe), "AlphaMask");
    data.attributeType(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.attribute(MaterialLayer(0xfefe), 0);
    data.attribute(MaterialLayer(0xfefe), "AlphaMask");
    data.attribute(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.attribute<Int>(MaterialLayer(0xfefe), 0);
    data.attribute<Int>(MaterialLayer(0xfefe), "AlphaMask");
    data.attribute<Int>(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.attribute<Containers::StringView>(MaterialLayer(0xfefe), 0);
    data.mutableAttribute(MaterialLayer(0xfefe), 0);
    data.mutableAttribute(MaterialLayer(0xfefe), "AlphaMask");
    data.mutableAttribute(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.mutableAttribute<Int>(MaterialLayer(0xfefe), 0);
    data.mutableAttribute<Int>(MaterialLayer(0xfefe), "AlphaMask");
    data.mutableAttribute<Int>(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.mutableAttribute<Containers::MutableStringView>(MaterialLayer(0xfefe), 0);
    data.findAttribute(MaterialLayer(0xfefe), "AlphaMask");
    data.findAttribute(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.findAttribute<bool>(MaterialLayer(0xfefe), "AlphaMask");
    data.findAttribute<bool>(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.attributeOr(MaterialLayer(0xfefe), "AlphaMask", false);
    data.attributeOr(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask, false);
    CORRADE_COMPARE(out,
        "Trade::MaterialData::findLayerId(): invalid name Trade::MaterialLayer(0x0)\n"
        "Trade::MaterialData::findLayerId(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerId(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactor(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactorTexture(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactorTextureSwizzle(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactorTextureMatrix(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactorTextureCoordinates(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactorTextureLayer(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeCount(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::hasAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::hasAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::findAttributeId(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::findAttributeId(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeId(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeId(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeName(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeType(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeType(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeType(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::findAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::findAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::findAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::findAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeOr(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeOr(): invalid name Trade::MaterialLayer(0xfefe)\n");
}

void MaterialDataTest::accessOutOfRangeInLayerIndex() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }, {0, 2}};

    Containers::String out;
    Error redirectError{&out};
    data.attributeData(1, 2);
    data.attributeName(1, 2);
    data.attributeType(1, 2);
    data.attribute(1, 2);
    data.attribute<Int>(1, 2);
    data.attribute<Containers::StringView>(1, 2);
    data.mutableAttribute(1, 2);
    data.mutableAttribute<Int>(1, 2);
    data.mutableAttribute<Containers::MutableStringView>(1, 2);
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attributeData(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::attributeName(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::attributeType(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 attributes in layer 1\n");
}

void MaterialDataTest::accessOutOfRangeInLayerString() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::AlphaMask, 0.5f}
    }, {0, 2}};

    Containers::String out;
    Error redirectError{&out};
    data.attributeName("ClearCoat", 2);
    data.attributeType("ClearCoat", 2);
    data.attribute("ClearCoat", 2);
    data.attribute<Int>("ClearCoat", 2);
    data.attribute<Containers::StringView>("ClearCoat", 2);
    data.mutableAttribute("ClearCoat", 2);
    data.mutableAttribute<Int>("ClearCoat", 2);
    data.mutableAttribute<Containers::MutableStringView>("ClearCoat", 2);
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attributeName(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::attributeType(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::mutableAttribute(): index 2 out of range for 2 attributes in layer ClearCoat\n");
}

void MaterialDataTest::accessNotFoundInLayerIndex() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {"DiffuseColor", 0xff3366aa_rgbaf}
    }, {0, 1}};

    /* These are fine */
    CORRADE_VERIFY(!data.hasAttribute(1, "DiffuseColour"));
    CORRADE_VERIFY(!data.findAttributeId(1, "DiffuseColour"));

    Containers::String out;
    Error redirectError{&out};
    data.attributeId(1, "DiffuseColour");
    data.attributeType(1, "DiffuseColour");
    data.attribute(1, "DiffuseColour");
    data.attribute<Color4>(1, "DiffuseColour");
    data.mutableAttribute(1, "DiffuseColour");
    data.mutableAttribute<Color4>(1, "DiffuseColour");
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attributeId(): attribute DiffuseColour not found in layer 1\n"
        "Trade::MaterialData::attributeType(): attribute DiffuseColour not found in layer 1\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer 1\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer 1\n"
        "Trade::MaterialData::mutableAttribute(): attribute DiffuseColour not found in layer 1\n"
        "Trade::MaterialData::mutableAttribute(): attribute DiffuseColour not found in layer 1\n");
}

void MaterialDataTest::accessNotFoundInLayerString() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "ClearCoat"},
        {"DiffuseColor", 0xff3366aa_rgbaf}
    }, {0, 2}};

    /* These are fine */
    CORRADE_VERIFY(!data.hasAttribute("ClearCoat", "DiffuseColour"));
    CORRADE_VERIFY(!data.findAttributeId("ClearCoat", "DiffuseColour"));

    Containers::String out;
    Error redirectError{&out};
    data.attributeId("ClearCoat", "DiffuseColour");
    data.attributeType("ClearCoat", "DiffuseColour");
    data.attribute("ClearCoat", "DiffuseColour");
    data.attribute<Color4>("ClearCoat", "DiffuseColour");
    data.mutableAttribute("ClearCoat", "DiffuseColour");
    data.mutableAttribute<Color4>("ClearCoat", "DiffuseColour");
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attributeId(): attribute DiffuseColour not found in layer ClearCoat\n"
        "Trade::MaterialData::attributeType(): attribute DiffuseColour not found in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer ClearCoat\n"
        "Trade::MaterialData::mutableAttribute(): attribute DiffuseColour not found in layer ClearCoat\n"
        "Trade::MaterialData::mutableAttribute(): attribute DiffuseColour not found in layer ClearCoat\n");
}

void MaterialDataTest::accessInvalidAttributeName() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MaterialData data{{}, {}};

    /* The name should be converted to a string first and foremost and only
       then delegated to another overload. Which means all asserts should
       print the leaf function name. */
    Containers::String out;
    Error redirectError{&out};
    data.hasAttribute(0, MaterialAttribute(0x0));
    data.hasAttribute("Layer", MaterialAttribute(0xfefe));
    data.findAttributeId(0, MaterialAttribute(0x0));
    data.findAttributeId("Layer", MaterialAttribute(0xfefe));
    data.attributeId(0, MaterialAttribute(0x0));
    data.attributeId("Layer", MaterialAttribute(0xfefe));
    data.attributeType(0, MaterialAttribute(0x0));
    data.attributeType("Layer", MaterialAttribute(0xfefe));
    data.attribute(0, MaterialAttribute(0x0));
    data.attribute("Layer", MaterialAttribute(0xfefe));
    data.attribute<Int>(0, MaterialAttribute(0x0));
    data.attribute<Int>("Layer", MaterialAttribute(0xfefe));
    data.mutableAttribute(0, MaterialAttribute(0x0));
    data.mutableAttribute("Layer", MaterialAttribute(0xfefe));
    data.mutableAttribute<Int>(0, MaterialAttribute(0x0));
    data.mutableAttribute<Int>("Layer", MaterialAttribute(0xfefe));
    data.findAttribute(0, MaterialAttribute(0x0));
    data.findAttribute("Layer", MaterialAttribute(0xfefe));
    data.findAttribute<Int>(0, MaterialAttribute(0x0));
    data.findAttribute<Int>("Layer", MaterialAttribute(0xfefe));
    data.attributeOr(0, MaterialAttribute(0x0), 42);
    data.attributeOr("Layer", MaterialAttribute(0xfefe), 42);
    CORRADE_COMPARE(out,
        "Trade::MaterialData::hasAttribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::hasAttribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::findAttributeId(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::findAttributeId(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::attributeId(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::attributeId(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::attributeType(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::attributeType(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::mutableAttribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::findAttribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::findAttribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::findAttribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::findAttribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::attributeOr(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::attributeOr(): invalid name Trade::MaterialAttribute(0xfefe)\n");
}

void MaterialDataTest::accessMutableNotAllowed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const MaterialAttributeData attributes[]{
        {MaterialAttribute::DiffuseColor, 0x335566ff_rgbaf},
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::Roughness, 0.5f},
        /** @todo test builtin buffer attribute once it exists */
        {"data", Containers::ArrayView<const void>{}},
    };

    const UnsignedInt layers[]{
        1, 4
    };

    MaterialData data{{}, {}, attributes, {}, layers};

    Containers::String out;
    Error redirectError{&out};
    data.mutableAttribute(0);
    data.mutableAttribute("DiffuseColor");
    data.mutableAttribute(MaterialAttribute::DiffuseColor);
    data.mutableAttribute<Color4>(0);
    data.mutableAttribute<Color4>("DiffuseColor");
    data.mutableAttribute<Color4>(MaterialAttribute::DiffuseColor);

    data.mutableAttribute(1, 1);
    data.mutableAttribute(1, "Roughness");
    data.mutableAttribute(1, MaterialAttribute::Roughness);
    data.mutableAttribute<Float>(1, 1);
    data.mutableAttribute<Float>(1, "Roughness");
    data.mutableAttribute<Float>(1, MaterialAttribute::Roughness);
    data.mutableAttribute<Containers::MutableStringView>(1, 0);
    data.mutableAttribute<Containers::MutableStringView>(1, " LayerName");
    data.mutableAttribute<Containers::MutableStringView>(1, MaterialAttribute::LayerName);
    data.mutableAttribute<Containers::ArrayView<void>>(1, 2);
    data.mutableAttribute<Containers::ArrayView<void>>(1, "data");
    /** @todo test also builtin buffer attribute access once it exists */

    data.mutableAttribute("ClearCoat", 1);
    data.mutableAttribute("ClearCoat", "Roughness");
    data.mutableAttribute("ClearCoat", MaterialAttribute::Roughness);
    data.mutableAttribute<Float>("ClearCoat", 1);
    data.mutableAttribute<Float>("ClearCoat", "Roughness");
    data.mutableAttribute<Float>("ClearCoat", MaterialAttribute::Roughness);
    data.mutableAttribute<Containers::MutableStringView>("ClearCoat", 0);
    data.mutableAttribute<Containers::MutableStringView>("ClearCoat", " LayerName");
    data.mutableAttribute<Containers::MutableStringView>("ClearCoat", MaterialAttribute::LayerName);
    data.mutableAttribute<Containers::ArrayView<void>>("ClearCoat", 2);
    data.mutableAttribute<Containers::ArrayView<void>>("ClearCoat", "data");
    /** @todo test also builtin buffer attribute access once it exists */

    data.mutableAttribute(MaterialLayer::ClearCoat, 1);
    data.mutableAttribute(MaterialLayer::ClearCoat, "Roughness");
    data.mutableAttribute(MaterialLayer::ClearCoat, MaterialAttribute::Roughness);
    data.mutableAttribute<Float>(MaterialLayer::ClearCoat, 1);
    data.mutableAttribute<Float>(MaterialLayer::ClearCoat, "Roughness");
    data.mutableAttribute<Float>(MaterialLayer::ClearCoat, MaterialAttribute::Roughness);
    data.mutableAttribute<Containers::MutableStringView>(MaterialLayer::ClearCoat, 0);
    data.mutableAttribute<Containers::MutableStringView>(MaterialLayer::ClearCoat, " LayerName");
    data.mutableAttribute<Containers::MutableStringView>(MaterialLayer::ClearCoat, MaterialAttribute::LayerName);
    data.mutableAttribute<Containers::ArrayView<void>>(MaterialLayer::ClearCoat, 2);
    data.mutableAttribute<Containers::ArrayView<void>>(MaterialLayer::ClearCoat, "data");
    /** @todo test also builtin buffer attribute access once it exists */
    CORRADE_COMPARE(out,
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"

        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"

        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"

        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n"
        "Trade::MaterialData::mutableAttribute(): attribute data not mutable\n");
}

void MaterialDataTest::releaseAttributes() {
    MaterialData data{{}, {
        {"DiffuseColor", 0xff3366aa_rgbaf},
        {MaterialAttribute::NormalTexture, 0u}
    }, {1, 2}};

    const void* pointer = data.attributeData().data();

    Containers::Array<MaterialAttributeData> released = data.releaseAttributeData();
    CORRADE_COMPARE(released.data(), pointer);
    CORRADE_COMPARE(released.size(), 2);
    CORRADE_VERIFY(data.layerData());
    CORRADE_COMPARE(data.layerCount(), 2);
    CORRADE_VERIFY(!data.attributeData());
    /* This is based on the layer offsets, not an actual attribute count, so
       it's inconsistent, yes */
    CORRADE_COMPARE(data.attributeCount(), 1);
}

void MaterialDataTest::releaseLayers() {
    MaterialData data{{}, {
        {"DiffuseColor", 0xff3366aa_rgbaf},
        {MaterialAttribute::NormalTexture, 0u}
    }, {1, 2}};

    const void* pointer = data.layerData().data();

    Containers::Array<UnsignedInt> released = data.releaseLayerData();
    CORRADE_COMPARE(released.data(), pointer);
    CORRADE_COMPARE(released.size(), 2);
    CORRADE_VERIFY(!data.layerData());
    /* Returns always at least 1 (now it sees no layer data and thus thinks
       there's just the implicit base material) */
    CORRADE_COMPARE(data.layerCount(), 1);
    CORRADE_VERIFY(data.attributeData());
    /* No layer offsets anymore, so this is the total attribute count instead
       of the base material attribute count. It's inconsistent, yes. */
    CORRADE_COMPARE(data.attributeCount(), 2);
}

void MaterialDataTest::templateLayerAccess() {
    MaterialLayerData<MaterialLayer::ClearCoat> data{{}, {
        {MaterialAttribute::BaseColor, 0x335566ff_rgbaf},

        {MaterialLayer::ClearCoat},
        {MaterialAttribute::LayerFactor, 0.35f},
        {MaterialAttribute::LayerFactorTexture, 3u},
        {MaterialAttribute::LayerFactorTextureSwizzle, MaterialTextureSwizzle::B},
        {MaterialAttribute::LayerFactorTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::LayerFactorTextureCoordinates, 4u},
        {MaterialAttribute::LayerFactorTextureLayer, 5u},
    }, {1, 8}};

    CORRADE_COMPARE(data.layerName(), "ClearCoat");
    CORRADE_COMPARE(data.layerFactor(), 0.35f);
    CORRADE_COMPARE(data.layerFactorTexture(), 3u);
    CORRADE_COMPARE(data.layerFactorTextureSwizzle(), MaterialTextureSwizzle::B);
    CORRADE_COMPARE(data.layerFactorTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureCoordinates(), 4u);
    CORRADE_COMPARE(data.layerFactorTextureLayer(), 5u);

    CORRADE_COMPARE(data.attributeCount(), 7);
    CORRADE_VERIFY(data.hasAttribute(MaterialAttribute::LayerFactor));
    CORRADE_VERIFY(data.hasAttribute("LayerFactorTexture"));
    CORRADE_VERIFY(!data.hasAttribute(MaterialAttribute::BaseColor));
    CORRADE_VERIFY(!data.hasAttribute("BaseColor"));
    CORRADE_VERIFY(data.hasAttribute(0, MaterialAttribute::BaseColor));
    CORRADE_VERIFY(data.hasAttribute(0, "BaseColor"));

    CORRADE_COMPARE(data.findAttributeId(MaterialAttribute::LayerFactorTexture), 2);
    CORRADE_COMPARE(data.findAttributeId("LayerFactorTexture"), 2);

    CORRADE_COMPARE(data.attributeId(MaterialAttribute::LayerFactorTexture), 2);
    CORRADE_COMPARE(data.attributeId("LayerFactorTexture"), 2);

    CORRADE_COMPARE(data.attributeName(2), "LayerFactorTexture");

    CORRADE_COMPARE(data.attributeType(2), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType(MaterialAttribute::LayerFactorTexture), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType("LayerFactorTexture"), MaterialAttributeType::UnsignedInt);

    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(2)), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(MaterialAttribute::LayerFactorTexture)), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute("LayerFactorTexture")), 3);
    CORRADE_COMPARE(*static_cast<UnsignedInt*>(data.mutableAttribute(2)), 3);
    CORRADE_COMPARE(*static_cast<UnsignedInt*>(data.mutableAttribute(MaterialAttribute::LayerFactorTexture)), 3);
    CORRADE_COMPARE(*static_cast<UnsignedInt*>(data.mutableAttribute("LayerFactorTexture")), 3);

    CORRADE_COMPARE(data.attribute<UnsignedInt>(2), 3);
    CORRADE_COMPARE(data.attribute<UnsignedInt>(MaterialAttribute::LayerFactorTexture), 3);
    CORRADE_COMPARE(data.attribute<UnsignedInt>("LayerFactorTexture"), 3);
    CORRADE_COMPARE(data.mutableAttribute<UnsignedInt>(2), 3);
    CORRADE_COMPARE(data.mutableAttribute<UnsignedInt>(MaterialAttribute::LayerFactorTexture), 3);
    CORRADE_COMPARE(data.mutableAttribute<UnsignedInt>("LayerFactorTexture"), 3);

    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.findAttribute(MaterialAttribute::LayerFactorTexture)), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.findAttribute("LayerFactorTexture")), 3);

    CORRADE_COMPARE(data.findAttribute<UnsignedInt>(MaterialAttribute::LayerFactorTexture), 3);
    CORRADE_COMPARE(data.findAttribute<UnsignedInt>("LayerFactorTexture"), 3);

    CORRADE_COMPARE(data.attributeOr(MaterialAttribute::LayerFactorTexture, 5u), 3);
    CORRADE_COMPARE(data.attributeOr("LayerFactorTexture", 5u), 3);
}

void MaterialDataTest::templateLayerAccessMutable() {
    MaterialLayerData<MaterialLayer::ClearCoat> data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::Roughness, 1.0f},
        /** @todo test builtin buffer attribute once it exists */
        {"data", Containers::arrayView({0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f})},
    }, {0, 3}};

    *static_cast<Float*>(data.mutableAttribute(1)) *= 2.0f;
    *static_cast<Float*>(data.mutableAttribute(MaterialAttribute::Roughness)) *= 2.0f;
    *static_cast<Float*>(data.mutableAttribute("Roughness")) *= 2.0f;
    data.mutableAttribute<Float>(1) *= 2.0f;
    data.mutableAttribute<Float>(MaterialAttribute::Roughness) *= 2.0f;
    data.mutableAttribute<Float>("Roughness") *= 2.0f;
    CORRADE_COMPARE(data.attribute<Float>(MaterialAttribute::Roughness), 64.0f);

    static_cast<Float*>(data.mutableAttribute(2))[1] *= 2.0f;
    /** @todo test also builtin buffer attribute access once it exists */
    static_cast<Float*>(data.mutableAttribute("data"))[2] *= 2.0f;
    Containers::arrayCast<Float>(data.mutableAttribute<Containers::ArrayView<void>>(2))[3] *= 2.0f;
    /** @todo test also builtin buffer attribute access once it exists */
    Containers::arrayCast<Float>(data.mutableAttribute<Containers::ArrayView<void>>("data"))[4] *= 2.0f;
    CORRADE_COMPARE_AS(Containers::arrayCast<const Float>(data.attribute<Containers::ArrayView<const void>>("data")), Containers::arrayView({
        0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 5.0f, 6.0f
    }), TestSuite::Compare::Container);

    /* Resetting back so the layer name always stays the same so the next call
       can find it. Other than that, the result should be same as in
       accessLayerIndexMutable(). */
    {
        *static_cast<char*>(data.mutableAttribute("ClearCoat", 0)) = 'D';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "DlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        *static_cast<char*>(data.mutableAttribute("ClearCoat", " LayerName")) = 'E';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "ElearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        *static_cast<char*>(data.mutableAttribute("ClearCoat", MaterialAttribute::LayerName)) = 'F';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "FlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        data.mutableAttribute<Containers::MutableStringView>("ClearCoat", 0)[0] = 'G';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "GlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        data.mutableAttribute<Containers::MutableStringView>("ClearCoat", " LayerName")[0] = 'H';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "HlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    } {
        data.mutableAttribute<Containers::MutableStringView>("ClearCoat", " LayerName")[0] = 'I';
        CORRADE_COMPARE(data.attribute<Containers::StringView>(1, 0), "IlearCoat");
        *static_cast<char*>(data.mutableAttribute(1, 0)) = 'C';
    }
}

void MaterialDataTest::debugLayer() {
    Containers::String out;

    Debug{&out} << MaterialLayer::ClearCoat << MaterialLayer(0xfefe) << MaterialLayer{};
    CORRADE_COMPARE(out, "Trade::MaterialLayer::ClearCoat Trade::MaterialLayer(0xfefe) Trade::MaterialLayer(0x0)\n");
}

void MaterialDataTest::debugAttribute() {
    Containers::String out;

    Debug{&out} << MaterialAttribute::DiffuseTextureCoordinates << MaterialAttribute::LayerName << MaterialAttribute(0xfefe) << MaterialAttribute{};
    CORRADE_COMPARE(out, "Trade::MaterialAttribute::DiffuseTextureCoordinates Trade::MaterialAttribute::LayerName Trade::MaterialAttribute(0xfefe) Trade::MaterialAttribute(0x0)\n");
}

void MaterialDataTest::debugTextureSwizzle() {
    Containers::String out;

    /* The swizzle is encoded as a fourCC, so it just prints the numerical
       value as a char. Worst case this will print nothing or four garbage
       letters. Sorry in that case. */
    Debug{&out} << MaterialTextureSwizzle::BA << MaterialTextureSwizzle{};
    CORRADE_COMPARE(out, "Trade::MaterialTextureSwizzle::BA Trade::MaterialTextureSwizzle::\n");
}

void MaterialDataTest::debugTextureSwizzlePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << MaterialTextureSwizzle::BA << Debug::packed << MaterialTextureSwizzle{} << MaterialTextureSwizzle::RG;
    CORRADE_COMPARE(out, "BA  Trade::MaterialTextureSwizzle::RG\n");
}

void MaterialDataTest::debugAttributeType() {
    Containers::String out;

    Debug{&out} << MaterialAttributeType::Matrix3x2 << MaterialAttributeType(0xfe);
    CORRADE_COMPARE(out, "Trade::MaterialAttributeType::Matrix3x2 Trade::MaterialAttributeType(0xfe)\n");
}

void MaterialDataTest::debugAttributeTypePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << MaterialAttributeType::Matrix3x2 << Debug::packed << MaterialAttributeType(0xfe) << MaterialAttributeType::Float;
    CORRADE_COMPARE(out, "Matrix3x2 0xfe Trade::MaterialAttributeType::Float\n");
}

void MaterialDataTest::debugType() {
    Containers::String out;

    Debug(&out) << MaterialType::Phong << MaterialType(0xbe);
    CORRADE_COMPARE(out, "Trade::MaterialType::Phong Trade::MaterialType(0xbe)\n");
}

void MaterialDataTest::debugTypePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << MaterialType::Phong << Debug::packed << MaterialType(0xbe) << MaterialType::Flat;
    CORRADE_COMPARE(out, "Phong 0xbe Trade::MaterialType::Flat\n");
}

void MaterialDataTest::debugTypes() {
    Containers::String out;

    Debug{&out} << (MaterialType::Phong|MaterialType(0xe0)) << MaterialTypes{};
    CORRADE_COMPARE(out, "Trade::MaterialType::Phong|Trade::MaterialType(0xe0) Trade::MaterialTypes{}\n");
}

void MaterialDataTest::debugTypesPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (MaterialType::Phong|MaterialType(0xe0)) << Debug::packed << MaterialTypes{} << MaterialType::Flat;
    CORRADE_COMPARE(out, "Phong|0xe0 {} Trade::MaterialType::Flat\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void MaterialDataTest::debugFlag() {
    Containers::String out;

    Debug{&out} << MaterialData::Flag::DoubleSided << MaterialData::Flag(0xf0);
    CORRADE_COMPARE(out, "Trade::MaterialData::Flag::DoubleSided Trade::MaterialData::Flag(0xf0)\n");
}

void MaterialDataTest::debugFlags() {
    Containers::String out;

    Debug{&out} << MaterialData::Flag::DoubleSided << MaterialData::Flags{};
    CORRADE_COMPARE(out, "Trade::MaterialData::Flag::DoubleSided Trade::MaterialData::Flags{}\n");
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void MaterialDataTest::debugAlphaMode() {
    Containers::String out;

    Debug{&out} << MaterialAlphaMode::Opaque << MaterialAlphaMode(0xee);
    CORRADE_COMPARE(out, "Trade::MaterialAlphaMode::Opaque Trade::MaterialAlphaMode(0xee)\n");
}

void MaterialDataTest::debugAlphaModePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << MaterialAlphaMode::Opaque << Debug::packed << MaterialAlphaMode(0xee) << MaterialAlphaMode::Blend;
    CORRADE_COMPARE(out, "Opaque 0xee Trade::MaterialAlphaMode::Blend\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::MaterialDataTest)
