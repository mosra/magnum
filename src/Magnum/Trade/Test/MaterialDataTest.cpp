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

#include <algorithm>
#include <sstream>
#include <Corrade/Containers/StaticArray.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/FlatMaterialData.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/PbrClearCoatMaterialData.h"
#include "Magnum/Trade/PbrMetallicRoughnessMaterialData.h"
#include "Magnum/Trade/PbrSpecularGlossinessMaterialData.h"
#include "Magnum/Trade/PhongMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class MaterialDataTest: public TestSuite::Tester {
    public:
        explicit MaterialDataTest();

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
        void constructAttributeNameStringValue();
        void constructAttributeTextureSwizzle();
        void constructAttributeLayer();

        void constructAttributeInvalidName();
        void constructAttributeInvalidLayerName();
        void constructAttributeWrongTypeForName();
        void constructAttributeInvalidType();
        void constructAttributeTooLarge();
        void constructAttributeTooLargeString();
        void constructAttributeTooLargeNameString();
        void constructAttributeWrongAccessType();
        void constructAttributeWrongAccessPointerType();
        void constructAttributeWrongAccessTypeString();

        void construct();
        void constructEmptyAttribute();
        void constructDuplicateAttribute();
        void constructFromImmutableSortedArray();

        void constructLayers();
        void constructLayersNotMonotonic();
        void constructLayersOffsetOutOfBounds();

        void constructNonOwned();
        void constructNonOwnedLayers();
        void constructNonOwnedEmptyAttribute();
        void constructNonOwnedNotSorted();
        void constructNonOwnedDuplicateAttribute();
        void constructNonOwnedLayersNotMonotonic();
        void constructNonOwnedLayersOffsetOutOfBounds();

        void constructCopy();
        void constructMove();

        void as();
        void asRvalue();

        void access();
        void accessPointer();
        void accessString();
        void accessTextureSwizzle();
        void accessOptional();
        void accessOutOfBounds();
        void accessNotFound();
        void accessInvalidAttributeName();
        void accessWrongType();
        void accessWrongPointerType();
        void accessWrongTypeString();

        void accessLayers();
        void accessLayersDefaults();
        void accessLayersTextured();
        void accessLayersTexturedDefault();
        void accessLayersTexturedSingleMatrixCoordinates();
        void accessLayersTexturedBaseMaterialMatrixCoordinates();
        void accessLayersInvalidTextures();

        void accessLayerLayerNameInBaseMaterial();
        void accessLayerEmptyLayer();
        void accessLayerIndexOptional();
        void accessLayerNameOptional();
        void accessLayerStringOptional();
        void accessLayerOutOfBounds();
        void accessLayerNotFound();
        void accessInvalidLayerName();
        void accessOutOfBoundsInLayerIndex();
        void accessOutOfBoundsInLayerString();
        void accessNotFoundInLayerIndex();
        void accessNotFoundInLayerString();

        void releaseAttributes();
        void releaseLayers();

        #ifdef MAGNUM_BUILD_DEPRECATED
        void constructPhongDeprecated();
        void constructPhongDeprecatedTextured();
        void constructPhongDeprecatedTexturedTextureTransform();
        void constructPhongDeprecatedTexturedCoordinates();
        void constructPhongDeprecatedTextureTransformNoTextures();
        void constructPhongDeprecatedNoTextureTransformationFlag();
        void constructPhongDeprecatedNoTextureCoordinatesFlag();
        #endif

        void pbrMetallicRoughnessAccess();
        void pbrMetallicRoughnessAccessDefaults();
        void pbrMetallicRoughnessAccessTextured();
        void pbrMetallicRoughnessAccessTexturedDefaults();
        void pbrMetallicRoughnessAccessTexturedImplicitPackedMetallicRoughness();
        void pbrMetallicRoughnessAccessTexturedExplicitPackedMetallicRoughness();
        void pbrMetallicRoughnessAccessTexturedExplicitPackedRoughnessMetallicOcclusion();
        void pbrMetallicRoughnessAccessTexturedExplicitPackedOcclusionRoughnessMetallic();
        void pbrMetallicRoughnessAccessTexturedExplicitPackedNormalRoughnessMetallic();
        void pbrMetallicRoughnessAccessTexturedSingleMatrixCoordinates();
        void pbrMetallicRoughnessAccessInvalidTextures();
        void pbrMetallicRoughnessAccessCommonTransformationCoordinatesNoTextures();
        void pbrMetallicRoughnessAccessCommonTransformationCoordinatesOneTexture();
        void pbrMetallicRoughnessAccessCommonTransformationCoordinatesOneDifferentTexture();
        void pbrMetallicRoughnessAccessNoCommonTransformationCoordinates();

        void pbrSpecularGlossinessAccess();
        void pbrSpecularGlossinessAccessDefaults();
        void pbrSpecularGlossinessAccessTextured();
        void pbrSpecularGlossinessAccessTexturedDefaults();
        void pbrSpecularGlossinessAccessTexturedImplicitPackedSpecularGlossiness();
        void pbrSpecularGlossinessAccessTexturedExplicitPackedSpecularGlossiness();
        void pbrSpecularGlossinessAccessTexturedSingleMatrixCoordinates();
        void pbrSpecularGlossinessAccessInvalidTextures();
        void pbrSpecularGlossinessAccessCommonTransformationCoordinatesNoTextures();
        void pbrSpecularGlossinessAccessCommonTransformationCoordinatesOneTexture();
        void pbrSpecularGlossinessAccessCommonTransformationCoordinatesOneDifferentTexture();
        void pbrSpecularGlossinessAccessNoCommonTransformationCoordinates();

        void phongAccess();
        void phongAccessDefaults();
        void phongAccessTextured();
        void phongAccessTexturedDefaults();
        void phongAccessTexturedSingleMatrixCoordinates();
        void phongAccessTexturedImplicitPackedSpecularGlossiness();
        void phongAccessInvalidTextures();
        void phongAccessCommonTransformationCoordinatesNoTextures();
        void phongAccessCommonTransformationCoordinatesOneTexture();
        void phongAccessCommonTransformationCoordinatesOneDifferentTexture();
        void phongAccessNoCommonTransformationCoordinates();

        void flatAccessBaseColor();
        void flatAccessDiffuseColor();
        void flatAccessDefaults();
        void flatAccessTexturedBaseColor();
        void flatAccessTexturedDiffuseColor();
        void flatAccessTexturedDefaults();
        void flatAccessTexturedBaseColorSingleMatrixCoordinates();
        void flatAccessTexturedDiffuseColorSingleMatrixCoordinates();
        void flatAccessTexturedMismatchedMatrixCoordinates();
        void flatAccessInvalidTextures();

        void templateLayerAccess();

        void pbrClearCoatAccess();
        void pbrClearCoatAccessDefaults();
        void pbrClearCoatAccessTextured();
        void pbrClearCoatAccessTexturedDefaults();
        void pbrClearCoatAccessTexturedExplicitPackedLayerFactorRoughness();
        void pbrClearCoatAccessTexturedSingleMatrixCoordinates();
        void pbrClearCoatAccessTexturedBaseMaterialMatrixCoordinates();
        void pbrClearCoatAccessInvalidTextures();
        void pbrClearCoatAccessCommonTransformationCoordinatesNoTextures();
        void pbrClearCoatAccessCommonTransformationCoordinatesOneTexture();
        void pbrClearCoatAccessCommonTransformationCoordinatesOneDifferentTexture();
        void pbrClearCoatAccessNoCommonTransformationCoordinates();

        void debugLayer();
        void debugAttribute();
        void debugTextureSwizzle();
        void debugAttributeType();

        void debugType();
        void debugTypes();
        #ifdef MAGNUM_BUILD_DEPRECATED
        void debugFlag();
        void debugFlags();
        #endif
        void debugAlphaMode();

        #ifdef MAGNUM_BUILD_DEPRECATED
        void debugPhongFlag();
        void debugPhongFlags();
        #endif
};

const Containers::StringView PbrMetallicRoughnessTextureData[] {
    "BaseColorTexture",
    "MetalnessTexture",
    "RoughnessTexture",
    "NormalTexture",
    "OcclusionTexture",
    "EmissiveTexture"
};

const Containers::StringView PbrSpecularGlossinessTextureData[] {
    "DiffuseTexture",
    "SpecularTexture",
    "GlossinessTexture",
    "NormalTexture",
    "OcclusionTexture",
    "EmissiveTexture"
};

const Containers::StringView PhongTextureData[] {
    "AmbientTexture",
    "DiffuseTexture",
    "SpecularTexture",
    "NormalTexture"
};

const Containers::StringView PbrClearCoatTextureData[] {
    "LayerFactorTexture",
    "RoughnessTexture",
    "NormalTexture"
};

MaterialDataTest::MaterialDataTest() {
    addTests({&MaterialDataTest::textureSwizzleComponentCount,

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
              &MaterialDataTest::constructAttributeNameStringValue,
              &MaterialDataTest::constructAttributeTextureSwizzle,
              &MaterialDataTest::constructAttributeLayer,

              &MaterialDataTest::constructAttributeInvalidName,
              &MaterialDataTest::constructAttributeInvalidLayerName,
              &MaterialDataTest::constructAttributeWrongTypeForName,
              &MaterialDataTest::constructAttributeInvalidType,
              &MaterialDataTest::constructAttributeTooLarge,
              &MaterialDataTest::constructAttributeTooLargeString,
              &MaterialDataTest::constructAttributeTooLargeNameString,
              &MaterialDataTest::constructAttributeWrongAccessType,
              &MaterialDataTest::constructAttributeWrongAccessPointerType,
              &MaterialDataTest::constructAttributeWrongAccessTypeString,

              &MaterialDataTest::construct,
              &MaterialDataTest::constructEmptyAttribute});

    addRepeatedTests({&MaterialDataTest::constructDuplicateAttribute},
        5*4*3*2);

    addTests({&MaterialDataTest::constructFromImmutableSortedArray,

              &MaterialDataTest::constructLayers,
              &MaterialDataTest::constructLayersNotMonotonic,
              &MaterialDataTest::constructLayersOffsetOutOfBounds,

              &MaterialDataTest::constructNonOwned,
              &MaterialDataTest::constructNonOwnedLayers,
              &MaterialDataTest::constructNonOwnedEmptyAttribute,
              &MaterialDataTest::constructNonOwnedNotSorted,
              &MaterialDataTest::constructNonOwnedDuplicateAttribute,
              &MaterialDataTest::constructNonOwnedLayersNotMonotonic,
              &MaterialDataTest::constructNonOwnedLayersOffsetOutOfBounds,

              &MaterialDataTest::constructCopy,
              &MaterialDataTest::constructMove,

              &MaterialDataTest::as,
              &MaterialDataTest::asRvalue,

              &MaterialDataTest::access,
              &MaterialDataTest::accessPointer,
              &MaterialDataTest::accessString,
              &MaterialDataTest::accessTextureSwizzle,
              &MaterialDataTest::accessOptional,
              &MaterialDataTest::accessOutOfBounds,
              &MaterialDataTest::accessNotFound,
              &MaterialDataTest::accessInvalidAttributeName,
              &MaterialDataTest::accessWrongType,
              &MaterialDataTest::accessWrongPointerType,
              &MaterialDataTest::accessWrongTypeString,

              &MaterialDataTest::accessLayers,
              &MaterialDataTest::accessLayersDefaults,
              &MaterialDataTest::accessLayersTextured,
              &MaterialDataTest::accessLayersTexturedDefault,
              &MaterialDataTest::accessLayersTexturedSingleMatrixCoordinates,
              &MaterialDataTest::accessLayersTexturedBaseMaterialMatrixCoordinates,
              &MaterialDataTest::accessLayersInvalidTextures,

              &MaterialDataTest::accessLayerLayerNameInBaseMaterial,
              &MaterialDataTest::accessLayerEmptyLayer,
              &MaterialDataTest::accessLayerIndexOptional,
              &MaterialDataTest::accessLayerNameOptional,
              &MaterialDataTest::accessLayerStringOptional,
              &MaterialDataTest::accessLayerOutOfBounds,
              &MaterialDataTest::accessLayerNotFound,
              &MaterialDataTest::accessInvalidLayerName,
              &MaterialDataTest::accessOutOfBoundsInLayerIndex,
              &MaterialDataTest::accessOutOfBoundsInLayerString,
              &MaterialDataTest::accessNotFoundInLayerIndex,
              &MaterialDataTest::accessNotFoundInLayerString,

              &MaterialDataTest::releaseAttributes,
              &MaterialDataTest::releaseLayers,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &MaterialDataTest::constructPhongDeprecated,
              &MaterialDataTest::constructPhongDeprecatedTextured,
              &MaterialDataTest::constructPhongDeprecatedTexturedTextureTransform,
              &MaterialDataTest::constructPhongDeprecatedTexturedCoordinates,
              &MaterialDataTest::constructPhongDeprecatedTextureTransformNoTextures,
              &MaterialDataTest::constructPhongDeprecatedNoTextureTransformationFlag,
              &MaterialDataTest::constructPhongDeprecatedNoTextureCoordinatesFlag,
              #endif

              &MaterialDataTest::pbrMetallicRoughnessAccess,
              &MaterialDataTest::pbrMetallicRoughnessAccessDefaults,
              &MaterialDataTest::pbrMetallicRoughnessAccessTextured,
              &MaterialDataTest::pbrMetallicRoughnessAccessTexturedDefaults,
              &MaterialDataTest::pbrMetallicRoughnessAccessTexturedImplicitPackedMetallicRoughness,
              &MaterialDataTest::pbrMetallicRoughnessAccessTexturedExplicitPackedMetallicRoughness,
              &MaterialDataTest::pbrMetallicRoughnessAccessTexturedExplicitPackedRoughnessMetallicOcclusion,
              &MaterialDataTest::pbrMetallicRoughnessAccessTexturedExplicitPackedOcclusionRoughnessMetallic,
              &MaterialDataTest::pbrMetallicRoughnessAccessTexturedExplicitPackedNormalRoughnessMetallic,
              &MaterialDataTest::pbrMetallicRoughnessAccessTexturedSingleMatrixCoordinates,
              &MaterialDataTest::pbrMetallicRoughnessAccessInvalidTextures,
              &MaterialDataTest::pbrMetallicRoughnessAccessCommonTransformationCoordinatesNoTextures});

    addInstancedTests({
        &MaterialDataTest::pbrMetallicRoughnessAccessCommonTransformationCoordinatesOneTexture,
        &MaterialDataTest::pbrMetallicRoughnessAccessCommonTransformationCoordinatesOneDifferentTexture},
        Containers::arraySize(PbrMetallicRoughnessTextureData));

    addTests({&MaterialDataTest::pbrMetallicRoughnessAccessNoCommonTransformationCoordinates,

              &MaterialDataTest::pbrSpecularGlossinessAccess,
              &MaterialDataTest::pbrSpecularGlossinessAccessDefaults,
              &MaterialDataTest::pbrSpecularGlossinessAccessTextured,
              &MaterialDataTest::pbrSpecularGlossinessAccessTexturedDefaults,
              &MaterialDataTest::pbrSpecularGlossinessAccessTexturedImplicitPackedSpecularGlossiness,
              &MaterialDataTest::pbrSpecularGlossinessAccessTexturedExplicitPackedSpecularGlossiness,
              &MaterialDataTest::pbrSpecularGlossinessAccessTexturedSingleMatrixCoordinates,
              &MaterialDataTest::pbrSpecularGlossinessAccessInvalidTextures,
              &MaterialDataTest::pbrSpecularGlossinessAccessCommonTransformationCoordinatesNoTextures});

    addInstancedTests({
        &MaterialDataTest::pbrSpecularGlossinessAccessCommonTransformationCoordinatesOneTexture,
        &MaterialDataTest::pbrSpecularGlossinessAccessCommonTransformationCoordinatesOneDifferentTexture},
        Containers::arraySize(PbrSpecularGlossinessTextureData));

    addTests({&MaterialDataTest::pbrSpecularGlossinessAccessNoCommonTransformationCoordinates,

              &MaterialDataTest::phongAccess,
              &MaterialDataTest::phongAccessDefaults,
              &MaterialDataTest::phongAccessTextured,
              &MaterialDataTest::phongAccessTexturedDefaults,
              &MaterialDataTest::phongAccessTexturedSingleMatrixCoordinates,
              &MaterialDataTest::phongAccessTexturedImplicitPackedSpecularGlossiness,
              &MaterialDataTest::phongAccessInvalidTextures,
              &MaterialDataTest::phongAccessCommonTransformationCoordinatesNoTextures});

    addInstancedTests({
        &MaterialDataTest::phongAccessCommonTransformationCoordinatesOneTexture,
        &MaterialDataTest::phongAccessCommonTransformationCoordinatesOneDifferentTexture},
        Containers::arraySize(PhongTextureData));

    addTests({&MaterialDataTest::phongAccessNoCommonTransformationCoordinates,

              &MaterialDataTest::flatAccessBaseColor,
              &MaterialDataTest::flatAccessDiffuseColor,
              &MaterialDataTest::flatAccessDefaults,
              &MaterialDataTest::flatAccessTexturedBaseColor,
              &MaterialDataTest::flatAccessTexturedDiffuseColor,
              &MaterialDataTest::flatAccessTexturedDefaults,
              &MaterialDataTest::flatAccessTexturedBaseColorSingleMatrixCoordinates,
              &MaterialDataTest::flatAccessTexturedDiffuseColorSingleMatrixCoordinates,
              &MaterialDataTest::flatAccessTexturedMismatchedMatrixCoordinates,
              &MaterialDataTest::flatAccessInvalidTextures,

              &MaterialDataTest::templateLayerAccess,

              &MaterialDataTest::pbrClearCoatAccess,
              &MaterialDataTest::pbrClearCoatAccessDefaults,
              &MaterialDataTest::pbrClearCoatAccessTextured,
              &MaterialDataTest::pbrClearCoatAccessTexturedDefaults,
              &MaterialDataTest::pbrClearCoatAccessTexturedExplicitPackedLayerFactorRoughness,
              &MaterialDataTest::pbrClearCoatAccessTexturedSingleMatrixCoordinates,
              &MaterialDataTest::pbrClearCoatAccessTexturedBaseMaterialMatrixCoordinates,
              &MaterialDataTest::pbrClearCoatAccessInvalidTextures,
              &MaterialDataTest::pbrClearCoatAccessCommonTransformationCoordinatesNoTextures});

    addInstancedTests({
        &MaterialDataTest::pbrClearCoatAccessCommonTransformationCoordinatesOneTexture,
        &MaterialDataTest::pbrClearCoatAccessCommonTransformationCoordinatesOneDifferentTexture},
        Containers::arraySize(PbrClearCoatTextureData));

    addTests({&MaterialDataTest::pbrClearCoatAccessNoCommonTransformationCoordinates,

              &MaterialDataTest::debugLayer,
              &MaterialDataTest::debugAttribute,
              &MaterialDataTest::debugTextureSwizzle,
              &MaterialDataTest::debugAttributeType,

              &MaterialDataTest::debugType,
              &MaterialDataTest::debugTypes,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &MaterialDataTest::debugFlag,
              &MaterialDataTest::debugFlags,
              #endif
              &MaterialDataTest::debugAlphaMode,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &MaterialDataTest::debugPhongFlag,
              &MaterialDataTest::debugPhongFlags
              #endif
              });
}

using namespace Containers::Literals;
using namespace Math::Literals;

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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    materialAttributeTypeSize(MaterialAttributeType(0x0));
    materialAttributeTypeSize(MaterialAttributeType(0xfe));
    materialAttributeTypeSize(MaterialAttributeType::String);
    CORRADE_COMPARE(out.str(),
        "Trade::materialAttributeTypeSize(): invalid type Trade::MaterialAttributeType(0x0)\n"
        "Trade::materialAttributeTypeSize(): invalid type Trade::MaterialAttributeType(0xfe)\n"
        "Trade::materialAttributeTypeSize(): string size is unknown\n");
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
        #ifdef __GNUC__
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
        #ifdef __GNUC__
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
        #ifdef __GNUC__
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
        #ifdef __GNUC__
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
    MaterialAttributeData attribute{"name that's long", "and a value\0that's also long but still fits!!"_s.except(1)};
    CORRADE_COMPARE(attribute.name(), "name that's long");
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::String);
    /* Pointer access will stop at the first null byte, but typed access won't */
    CORRADE_COMPARE(static_cast<const char*>(attribute.value()), "and a value"_s);
    CORRADE_COMPARE(attribute.value<Containers::StringView>(), "and a value\0that's also long but still fits!"_s);
    CORRADE_COMPARE(attribute.value<Containers::StringView>().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.value<Containers::StringView>()[attribute.value<Containers::StringView>().size()], '\0');

    constexpr MaterialAttributeData cattribute{"name that's long"_s, "and a value\0that's also long but still fits!!"_s.except(1)};
    CORRADE_COMPARE(cattribute.name(), "name that's long");
    CORRADE_COMPARE(cattribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(cattribute.name()[cattribute.name().size()], '\0');
    CORRADE_COMPARE(cattribute.type(), MaterialAttributeType::String);
    CORRADE_COMPARE(cattribute.value<Containers::StringView>(), "and a value\0that's also long but still fits!"_s);
    CORRADE_COMPARE(cattribute.value<Containers::StringView>().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(cattribute.value<Containers::StringView>()[cattribute.value<Containers::StringView>().size()], '\0');

    /* Type-erased variant */
    const Containers::StringView value = "and a value\0that's also long but still fits!!"_s.except(1);
    MaterialAttributeData typeErased{"name that's long", MaterialAttributeType::String, &value};
    CORRADE_COMPARE(typeErased.name(), "name that's long");
    CORRADE_COMPARE(typeErased.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(typeErased.name()[typeErased.name().size()], '\0');
    CORRADE_COMPARE(typeErased.type(), MaterialAttributeType::String);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>(), "and a value\0that's also long but still fits!"_s);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>()[typeErased.value<Containers::StringView>().size()], '\0');
}

void MaterialDataTest::constructAttributeNameStringValue() {
    /* Explicitly using a non-null-terminated view on input to check the null
       byte isn't read by accident*/

    MaterialAttributeData attribute{MaterialAttribute::LayerName, "a value\0that's long but still fits!!"_s.except(1)};
    CORRADE_COMPARE(attribute.name(), "$LayerName");
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::String);
    /* Pointer access will stop at the first null byte, but typed access won't */
    CORRADE_COMPARE(static_cast<const char*>(attribute.value()), "a value"_s);
    CORRADE_COMPARE(attribute.value<Containers::StringView>(), "a value\0that's long but still fits!"_s);
    CORRADE_COMPARE(attribute.value<Containers::StringView>().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.value<Containers::StringView>()[attribute.value<Containers::StringView>().size()], '\0');

    /* Type-erased variant */
    const Containers::StringView value = "a value\0that's long but still fits!!"_s.except(1);
    MaterialAttributeData typeErased{MaterialAttribute::LayerName, MaterialAttributeType::String, &value};
    CORRADE_COMPARE(typeErased.name(), "$LayerName");
    CORRADE_COMPARE(typeErased.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(typeErased.name()[typeErased.name().size()], '\0');
    CORRADE_COMPARE(typeErased.type(), MaterialAttributeType::String);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>(), "a value\0that's long but still fits!"_s);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(typeErased.value<Containers::StringView>()[typeErased.value<Containers::StringView>().size()], '\0');
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
    CORRADE_COMPARE(attribute.name(), "$LayerName");
    CORRADE_COMPARE(attribute.type(), MaterialAttributeType::String);
    CORRADE_COMPARE(attribute.value<Containers::StringView>(), "ClearCoat");
}

void MaterialDataTest::constructAttributeInvalidName() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialAttributeData{MaterialAttribute(0x0), 5};
    MaterialAttributeData{MaterialAttribute(0xfefe), 5};
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialAttributeData: invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialAttributeData: invalid name Trade::MaterialAttribute(0xfefe)\n");
}

void MaterialDataTest::constructAttributeInvalidLayerName() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialAttributeData{MaterialLayer(0x0)};
    MaterialAttributeData{MaterialLayer(0xfefe)};
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialAttributeData: invalid name Trade::MaterialLayer(0x0)\n"
        "Trade::MaterialAttributeData: invalid name Trade::MaterialLayer(0xfefe)\n");
}

void MaterialDataTest::constructAttributeWrongTypeForName() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialAttributeData{MaterialAttribute::DiffuseColor, Vector3ui{255, 16, 24}};
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialAttributeData: expected Trade::MaterialAttributeType::Vector4 for Trade::MaterialAttribute::DiffuseColor but got Trade::MaterialAttributeType::Vector3ui\n");
}

void MaterialDataTest::constructAttributeInvalidType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialAttributeData{"bla", MaterialAttributeType(0x0), nullptr};
    MaterialAttributeData{"bla", MaterialAttributeType(0xfe),  nullptr};
    CORRADE_COMPARE(out.str(),
        "Trade::materialAttributeTypeSize(): invalid type Trade::MaterialAttributeType(0x0)\n"
        "Trade::materialAttributeTypeSize(): invalid type Trade::MaterialAttributeType(0xfe)\n");
}

void MaterialDataTest::constructAttributeTooLarge() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialAttributeData{"attributeIsLong", Matrix3x4{}};
    /* Constexpr variant has the same assert, but in the header. It should have
       the same output. Except on MSVC 2015, which is a crap thing and gets
       lost when encountering T in there, so the assert is less useful. */
    /*constexpr*/ MaterialAttributeData{"attributeIsLong"_s, Matrix3x4{}};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialAttributeData: name attributeIsLong too long, expected at most 14 bytes for Trade::MaterialAttributeType::Matrix3x4 but got 15\n"
        "Trade::MaterialAttributeData: name attributeIsLong too long, expected at most 14 bytes for Trade::MaterialAttributeType::Matrix3x4 but got 15\n");
    #else
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialAttributeData: name attributeIsLong too long, expected at most 14 bytes for Trade::MaterialAttributeType::Matrix3x4 but got 15\n"
        "Trade::MaterialAttributeData: name attributeIsLong too long, got 15 bytes\n");
    #endif
}

void MaterialDataTest::constructAttributeTooLargeString() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialAttributeData{"attribute is long", "This is a problem, got a long piece of text!"};
    /* Constexpr variant has the same assert, but in the header. It should have
       the same output. */
    /*constexpr*/ MaterialAttributeData{"attribute is long"_s, "This is a problem, got a long piece of text!"_s};
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialAttributeData: name attribute is long and value This is a problem, got a long piece of text! too long, expected at most 60 bytes in total but got 61\n"
        "Trade::MaterialAttributeData: name attribute is long and value This is a problem, got a long piece of text! too long, expected at most 60 bytes in total but got 61\n");
}

void MaterialDataTest::constructAttributeTooLargeNameString() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialAttributeData{MaterialAttribute::LayerName, "This is a problem, got a huge, yuuge value to store"};
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialAttributeData: name $LayerName and value This is a problem, got a huge, yuuge value to store too long, expected at most 60 bytes in total but got 61\n");
}

void MaterialDataTest::constructAttributeWrongAccessType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialAttributeData{"thing3", Matrix4x3{}}.value<Int>();
    CORRADE_COMPARE(out.str(), "Trade::MaterialAttributeData::value(): improper type requested for thing3 of Trade::MaterialAttributeType::Matrix4x3\n");
}

void MaterialDataTest::constructAttributeWrongAccessPointerType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Int a = 3;
    const Float b = 57.0f;

    std::ostringstream out;
    Error redirectError{&out};
    MaterialAttributeData{"thing3", &a}.value<Int>();
    MaterialAttributeData{"boom", &b}.value<Float>();
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialAttributeData::value(): improper type requested for thing3 of Trade::MaterialAttributeType::MutablePointer\n"
        "Trade::MaterialAttributeData::value(): improper type requested for boom of Trade::MaterialAttributeType::Pointer\n");
}

void MaterialDataTest::constructAttributeWrongAccessTypeString() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialAttributeData{"thing3", Matrix4x3{}}.value<Containers::StringView>();
    CORRADE_COMPARE(out.str(), "Trade::MaterialAttributeData::value(): thing3 of Trade::MaterialAttributeType::Matrix4x3 can't be retrieved as a string\n");
}

void MaterialDataTest::construct() {
    int state;
    MaterialData data{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {"highlightColor", 0x335566ff_rgbaf},
        {MaterialAttribute::AmbientTextureMatrix, Matrix3::scaling({0.5f, 1.0f})}
    }, &state};

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_COMPARE(data.layerCount(), 1);
    CORRADE_VERIFY(!data.layerData());
    CORRADE_COMPARE(data.attributeCount(), 4);
    CORRADE_COMPARE(data.attributeData().size(), 4);
    CORRADE_COMPARE(data.importerState(), &state);

    CORRADE_COMPARE(data.layerName(0), "");
    CORRADE_VERIFY(!data.hasLayer(""));

    /* Verify sorting */
    CORRADE_COMPARE(data.attributeName(0), "AmbientTextureMatrix");
    CORRADE_COMPARE(data.attributeName(1), "DiffuseTextureCoordinates");
    CORRADE_COMPARE(data.attributeName(2), "DoubleSided");
    CORRADE_COMPARE(data.attributeName(3), "highlightColor");

    /* Access by ID */
    CORRADE_COMPARE(data.attributeType(0), MaterialAttributeType::Matrix3x3);
    CORRADE_COMPARE(data.attributeType(1), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType(2), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType(3), MaterialAttributeType::Vector4);

    CORRADE_COMPARE(data.attribute<Matrix3>(0), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.attribute<UnsignedInt>(1), 5);
    CORRADE_COMPARE(data.attribute<bool>(2), true);
    CORRADE_COMPARE(data.attribute<Color4>(3), 0x335566ff_rgbaf);

    CORRADE_COMPARE(*static_cast<const Matrix3*>(data.attribute(0)), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(1)), 5);
    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(2)), true);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute(3)), 0x335566ff_rgbaf);

    /* Access by name */
    CORRADE_VERIFY(data.hasAttribute(MaterialAttribute::DoubleSided));
    CORRADE_VERIFY(data.hasAttribute(MaterialAttribute::AmbientTextureMatrix));
    CORRADE_VERIFY(!data.hasAttribute(MaterialAttribute::TextureMatrix));

    CORRADE_COMPARE(data.attributeId(MaterialAttribute::DoubleSided), 2);
    CORRADE_COMPARE(data.attributeId(MaterialAttribute::AmbientTextureMatrix), 0);
    CORRADE_COMPARE(data.attributeId(MaterialAttribute::DiffuseTextureCoordinates), 1);

    CORRADE_COMPARE(data.attributeType(MaterialAttribute::AmbientTextureMatrix), MaterialAttributeType::Matrix3x3);
    CORRADE_COMPARE(data.attributeType(MaterialAttribute::DiffuseTextureCoordinates), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType(MaterialAttribute::DoubleSided), MaterialAttributeType::Bool);

    CORRADE_COMPARE(data.attribute<Matrix3>(MaterialAttribute::AmbientTextureMatrix), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.attribute<UnsignedInt>(MaterialAttribute::DiffuseTextureCoordinates), 5);
    CORRADE_COMPARE(data.attribute<bool>(MaterialAttribute::DoubleSided), true);

    CORRADE_COMPARE(*static_cast<const Matrix3*>(data.attribute(MaterialAttribute::AmbientTextureMatrix)), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(MaterialAttribute::DiffuseTextureCoordinates)), 5);
    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(MaterialAttribute::DoubleSided)), true);

    /* Access by string */
    CORRADE_VERIFY(data.hasAttribute("DoubleSided"));
    CORRADE_VERIFY(data.hasAttribute("highlightColor"));
    CORRADE_VERIFY(!data.hasAttribute("TextureMatrix"));

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

    CORRADE_COMPARE(*static_cast<const Matrix3*>(data.attribute("AmbientTextureMatrix")), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute("DiffuseTextureCoordinates")), 5);
    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute("DoubleSided")), true);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute("highlightColor")), 0x335566ff_rgbaf);
}

void MaterialDataTest::constructEmptyAttribute() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialData{{}, {
        {"DiffuseTexture"_s, 12u},
        MaterialAttributeData{}
    }};
    CORRADE_COMPARE(out.str(), "Trade::MaterialData: attribute 1 doesn't specify anything\n");
}

void MaterialDataTest::constructDuplicateAttribute() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    auto attributes = Containers::Array<MaterialAttributeData>{Containers::InPlaceInit, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {"highlightColor", 0x335566ff_rgbaf},
        {MaterialAttribute::AmbientTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u}
    }};

    /* Testing that it asserts in all input permutations */
    for(std::size_t i = 0; i != testCaseRepeatId(); ++i)
        std::next_permutation(attributes.begin(), attributes.end(), [](const MaterialAttributeData& a, const MaterialAttributeData& b) {
            return a.name() < b.name();
        });

    std::ostringstream out;
    Error redirectError{&out};
    MaterialData data{{}, std::move(attributes)};
    /* Because with graceful asserts it doesn't exit on error, the assertion
       might get printed multiple times */
    CORRADE_COMPARE(Utility::String::partition(out.str(), '\n')[0], "Trade::MaterialData: duplicate attribute DiffuseTextureCoordinates");
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

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_COMPARE(data.importerState(), &state);

    CORRADE_COMPARE(data.layerCount(), 4);
    CORRADE_COMPARE(data.layerData().size(), 4);

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

    CORRADE_COMPARE(data.layerId("ClearCoat"), 1);
    CORRADE_COMPARE(data.layerId(MaterialLayer::ClearCoat), 1);

    /* Verify sorting in each layer */
    CORRADE_COMPARE(data.attributeName(0, 0), "DiffuseTextureCoordinates");
    CORRADE_COMPARE(data.attributeName(0, 1), "DoubleSided");

    CORRADE_COMPARE(data.attributeName(1, 0), "$LayerName");
    CORRADE_COMPARE(data.attributeName(1, 1), "AlphaBlend");
    CORRADE_COMPARE(data.attributeName(1, 2), "highlightColor");

    CORRADE_COMPARE(data.attributeName(3, 0), "NormalTexture");
    CORRADE_COMPARE(data.attributeName(3, 1), "thickness");

    /* Access by layer ID and attribute ID */
    CORRADE_COMPARE(data.attributeType(0, 0), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType(1, 2), MaterialAttributeType::Vector4);
    CORRADE_COMPARE(data.attributeType(3, 1), MaterialAttributeType::Float);

    CORRADE_COMPARE(data.attribute<UnsignedInt>(0, 0), 5);
    CORRADE_COMPARE(data.attribute<Color4>(1, 2), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.attribute<Float>(3, 1), 0.015f);

    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(0, 0)), 5);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute(1, 2)), 0x335566ff_rgbaf);
    CORRADE_COMPARE(*static_cast<const Float*>(data.attribute(3, 1)), 0.015f);

    /* Access by layer ID and attribute name */
    CORRADE_VERIFY(data.hasAttribute(0, MaterialAttribute::DiffuseTextureCoordinates));
    CORRADE_VERIFY(!data.hasAttribute(0, MaterialAttribute::AlphaBlend));
    CORRADE_VERIFY(data.hasAttribute(1, MaterialAttribute::AlphaBlend));
    CORRADE_VERIFY(data.hasAttribute(1, MaterialAttribute::LayerName));
    CORRADE_VERIFY(!data.hasAttribute(2, MaterialAttribute::LayerName));
    CORRADE_VERIFY(!data.hasAttribute(2, MaterialAttribute::NormalTexture));
    CORRADE_VERIFY(data.hasAttribute(3, MaterialAttribute::NormalTexture));

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

    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(0, MaterialAttribute::DiffuseTextureCoordinates)), 5);
    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(1, MaterialAttribute::AlphaBlend)), true);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute(1, MaterialAttribute::LayerName)), "ClearCoat"_s);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(3, MaterialAttribute::NormalTexture)), 3);

    /* Access by layer ID and attribute string */
    CORRADE_VERIFY(data.hasAttribute(0, "DoubleSided"));
    CORRADE_VERIFY(!data.hasAttribute(0, "highlightColor"));
    CORRADE_VERIFY(data.hasAttribute(1, "highlightColor"));
    CORRADE_VERIFY(data.hasAttribute(1, "$LayerName"));
    CORRADE_VERIFY(!data.hasAttribute(2, "$LayerName"));
    CORRADE_VERIFY(!data.hasAttribute(2, "NormalTexture"));
    CORRADE_VERIFY(data.hasAttribute(3, "NormalTexture"));

    CORRADE_COMPARE(data.attributeId(0, "DoubleSided"), 1);
    CORRADE_COMPARE(data.attributeId(1, "highlightColor"), 2);
    CORRADE_COMPARE(data.attributeId(1, "$LayerName"), 0);
    CORRADE_COMPARE(data.attributeId(3, "NormalTexture"), 0);

    CORRADE_COMPARE(data.attributeType(0, "DoubleSided"), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType(1, "highlightColor"), MaterialAttributeType::Vector4);
    CORRADE_COMPARE(data.attributeType(1, "$LayerName"), MaterialAttributeType::String);
    CORRADE_COMPARE(data.attributeType(3, "NormalTexture"), MaterialAttributeType::UnsignedInt);

    CORRADE_COMPARE(data.attribute<bool>(0, "DoubleSided"), true);
    CORRADE_COMPARE(data.attribute<Color4>(1, "highlightColor"), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.attribute<Containers::StringView>(1, "$LayerName"), "ClearCoat");
    CORRADE_COMPARE(data.attribute<UnsignedInt>(3, "NormalTexture"), 3);

    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(0, "DoubleSided")), true);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute(1, "highlightColor")), 0x335566ff_rgbaf);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute(1, "$LayerName")), "ClearCoat"_s);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(3, "NormalTexture")), 3);

    /* Access by layer name and attribute ID */
    CORRADE_COMPARE(data.attributeName(MaterialLayer::ClearCoat, 1), "AlphaBlend");
    CORRADE_COMPARE(data.attributeName(MaterialLayer::ClearCoat, 2), "highlightColor");

    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, 1), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, 2), MaterialAttributeType::Vector4);

    CORRADE_COMPARE(data.attribute<bool>(MaterialLayer::ClearCoat, 1), true);
    CORRADE_COMPARE(data.attribute<Color4>(MaterialLayer::ClearCoat, 2), 0x335566ff_rgbaf);

    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(MaterialLayer::ClearCoat, 1)), true);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute(MaterialLayer::ClearCoat, 2)), 0x335566ff_rgbaf);

    /* Access by layer name and attribute name */
    CORRADE_VERIFY(data.hasAttribute(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend));
    CORRADE_VERIFY(data.hasAttribute(MaterialLayer::ClearCoat, MaterialAttribute::LayerName));

    CORRADE_COMPARE(data.attributeId(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend), 1);
    CORRADE_COMPARE(data.attributeId(MaterialLayer::ClearCoat, MaterialAttribute::LayerName), 0);

    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, MaterialAttribute::LayerName), MaterialAttributeType::String);

    CORRADE_COMPARE(data.attribute<bool>(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(data.attribute<Containers::StringView>(MaterialLayer::ClearCoat, MaterialAttribute::LayerName), "ClearCoat");

    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute(MaterialLayer::ClearCoat, MaterialAttribute::AlphaBlend)), true);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute(MaterialLayer::ClearCoat, MaterialAttribute::LayerName)), "ClearCoat"_s);

    /* Access by layer name and attribute string */
    CORRADE_VERIFY(data.hasAttribute(MaterialLayer::ClearCoat, "highlightColor"));
    CORRADE_VERIFY(data.hasAttribute(MaterialLayer::ClearCoat, "$LayerName"));

    CORRADE_COMPARE(data.attributeId(MaterialLayer::ClearCoat, "highlightColor"), 2);
    CORRADE_COMPARE(data.attributeId(MaterialLayer::ClearCoat, "$LayerName"), 0);

    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, "highlightColor"), MaterialAttributeType::Vector4);
    CORRADE_COMPARE(data.attributeType(MaterialLayer::ClearCoat, "$LayerName"), MaterialAttributeType::String);

    CORRADE_COMPARE(data.attribute<Color4>(MaterialLayer::ClearCoat, "highlightColor"), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.attribute<Containers::StringView>(MaterialLayer::ClearCoat, "$LayerName"), "ClearCoat");

    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute(MaterialLayer::ClearCoat, "highlightColor")), 0x335566ff_rgbaf);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute(MaterialLayer::ClearCoat, "$LayerName")), "ClearCoat"_s);

    /* Access by layer string and attribute ID */
    CORRADE_COMPARE(data.attributeName("ClearCoat", 1), "AlphaBlend");
    CORRADE_COMPARE(data.attributeName("ClearCoat", 2), "highlightColor");

    CORRADE_COMPARE(data.attributeType("ClearCoat", 1), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType("ClearCoat", 2), MaterialAttributeType::Vector4);

    CORRADE_COMPARE(data.attribute<bool>("ClearCoat", 1), true);
    CORRADE_COMPARE(data.attribute<Color4>("ClearCoat", 2), 0x335566ff_rgbaf);

    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute("ClearCoat", 1)), true);
    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute("ClearCoat", 2)), 0x335566ff_rgbaf);

    /* Access by layer string and attribute name */
    CORRADE_VERIFY(data.hasAttribute("ClearCoat", MaterialAttribute::AlphaBlend));
    CORRADE_VERIFY(data.hasAttribute("ClearCoat", MaterialAttribute::LayerName));

    CORRADE_COMPARE(data.attributeId("ClearCoat", MaterialAttribute::AlphaBlend), 1);
    CORRADE_COMPARE(data.attributeId("ClearCoat", MaterialAttribute::LayerName), 0);

    CORRADE_COMPARE(data.attributeType("ClearCoat", MaterialAttribute::AlphaBlend), MaterialAttributeType::Bool);
    CORRADE_COMPARE(data.attributeType("ClearCoat", MaterialAttribute::LayerName), MaterialAttributeType::String);

    CORRADE_COMPARE(data.attribute<bool>("ClearCoat", MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(data.attribute<Containers::StringView>("ClearCoat", MaterialAttribute::LayerName), "ClearCoat");

    CORRADE_COMPARE(*static_cast<const bool*>(data.attribute("ClearCoat", MaterialAttribute::AlphaBlend)), true);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute("ClearCoat", MaterialAttribute::LayerName)), "ClearCoat"_s);

    /* Access by layer string and attribute string */
    CORRADE_VERIFY(data.hasAttribute("ClearCoat", "highlightColor"));
    CORRADE_VERIFY(data.hasAttribute("ClearCoat", "$LayerName"));

    CORRADE_COMPARE(data.attributeId("ClearCoat", "highlightColor"), 2);
    CORRADE_COMPARE(data.attributeId("ClearCoat", "$LayerName"), 0);

    CORRADE_COMPARE(data.attributeType("ClearCoat", "highlightColor"), MaterialAttributeType::Vector4);
    CORRADE_COMPARE(data.attributeType("ClearCoat", "$LayerName"), MaterialAttributeType::String);

    CORRADE_COMPARE(data.attribute<Color4>("ClearCoat", "highlightColor"), 0x335566ff_rgbaf);
    CORRADE_COMPARE(data.attribute<Containers::StringView>("ClearCoat", "$LayerName"), "ClearCoat");

    CORRADE_COMPARE(*static_cast<const Color4*>(data.attribute("ClearCoat", "highlightColor")), 0x335566ff_rgbaf);
    CORRADE_COMPARE(static_cast<const char*>(data.attribute("ClearCoat", "$LayerName")), "ClearCoat"_s);
}

void MaterialDataTest::constructLayersNotMonotonic() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialData data{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {MaterialAttribute::AlphaBlend, true},
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::NormalTexture, 3u}
    }, {2, 5, 4, 5}};
    CORRADE_COMPARE(out.str(), "Trade::MaterialData: invalid range (5, 4) for layer 2 with 5 attributes in total\n");
}

void MaterialDataTest::constructLayersOffsetOutOfBounds() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MaterialData data{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {MaterialAttribute::AlphaBlend, true},
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::NormalTexture, 3u}
    }, {2, 6}};
    CORRADE_COMPARE(out.str(), "Trade::MaterialData: invalid range (2, 6) for layer 1 with 5 attributes in total\n");
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

    int state;
    MaterialData data{MaterialType::Phong, {}, attributes, &state};

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

        {"$LayerName"_s, "ClearCoat"_s},
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

    int state;
    MaterialData data{MaterialType::Phong,
        {}, attributes,
        {}, layers, &state};

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

    CORRADE_COMPARE(data.attributeName(1, 0), "$LayerName");
    CORRADE_COMPARE(data.attributeName(1, 1), "AlphaBlend");
    CORRADE_COMPARE(data.attributeName(1, 2), "highlightColor");

    CORRADE_COMPARE(data.attributeName(3, 0), "NormalTexture");
    CORRADE_COMPARE(data.attributeName(3, 1), "thickness");

    /* No need to verify the contents as there's no difference in access in
       owned vs non-owned */
}

void MaterialDataTest::constructNonOwnedEmptyAttribute() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialAttributeData attributes[]{
        {"DiffuseTexture"_s, 12u},
        MaterialAttributeData{}
    };

    std::ostringstream out;
    Error redirectError{&out};
    /* nullptr to avoid attributes interpreted as importerState */
    MaterialData{{}, {}, attributes, nullptr};
    CORRADE_COMPARE(out.str(), "Trade::MaterialData: attribute 1 doesn't specify anything\n");
}

void MaterialDataTest::constructNonOwnedNotSorted() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialAttributeData attributes[]{
        {"DiffuseTextureCoordinates"_s, 5u},
        {"DiffuseTexture"_s, 12u}
    };

    std::ostringstream out;
    Error redirectError{&out};
    /* nullptr to avoid attributes interpreted as importerState */
    MaterialData{{}, {}, attributes, nullptr};
    CORRADE_COMPARE(out.str(), "Trade::MaterialData: DiffuseTexture has to be sorted before DiffuseTextureCoordinates if passing non-owned data\n");
}

void MaterialDataTest::constructNonOwnedDuplicateAttribute() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialAttributeData attributes[]{
        {"DiffuseTexture"_s, 35u},
        {"DiffuseTextureCoordinates"_s, 5u},
        {"DiffuseTextureCoordinates"_s, 12u}
    };

    std::ostringstream out;
    Error redirectError{&out};
    /* nullptr to avoid attributes interpreted as importerState */
    MaterialData{{}, {}, attributes, nullptr};
    CORRADE_COMPARE(out.str(), "Trade::MaterialData: duplicate attribute DiffuseTextureCoordinates\n");
}

void MaterialDataTest::constructNonOwnedLayersNotMonotonic() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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

    std::ostringstream out;
    Error redirectError{&out};
    MaterialData data{MaterialType::Phong,
        {}, attributes,
        {}, layers};
    CORRADE_COMPARE(out.str(), "Trade::MaterialData: invalid range (5, 4) for layer 2 with 5 attributes in total\n");
}

void MaterialDataTest::constructNonOwnedLayersOffsetOutOfBounds() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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

    std::ostringstream out;
    Error redirectError{&out};
    MaterialData data{MaterialType::Phong,
        {}, attributes,
        {}, layers};
    CORRADE_COMPARE(out.str(), "Trade::MaterialData: invalid range (2, 6) for layer 1 with 5 attributes in total\n");
}

void MaterialDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<MaterialData, const MaterialData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<MaterialData, const MaterialData&>{}));
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

    MaterialData b{std::move(a)};
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
    c = std::move(b);
    CORRADE_COMPARE(b.attributeCount(), 1);
    CORRADE_COMPARE(b.layerCount(), 1);
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

    auto phong = std::move(data).as<PhongMaterialData>();
    CORRADE_COMPARE(data.layerCount(), 1);
    CORRADE_COMPARE(phong.layerCount(), 2);
    CORRADE_COMPARE(phong.diffuseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(phong.attribute<Color4>("ClearCoat", "highlightColor"), 0x335566ff_rgbaf);

    auto specularGlossiness = std::move(phong).as<PbrSpecularGlossinessMaterialData>();
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

void MaterialDataTest::accessOptional() {
    MaterialData data{{}, {
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }};

    /* This exists */
    CORRADE_VERIFY(data.tryAttribute("SpecularTexture"));
    CORRADE_VERIFY(data.tryAttribute(MaterialAttribute::SpecularTexture));
    CORRADE_COMPARE(*static_cast<const Int*>(data.tryAttribute("SpecularTexture")), 3);
    CORRADE_COMPARE(*static_cast<const Int*>(data.tryAttribute(MaterialAttribute::SpecularTexture)), 3);
    CORRADE_COMPARE(data.tryAttribute<UnsignedInt>("SpecularTexture"), 3);
    CORRADE_COMPARE(data.tryAttribute<UnsignedInt>(MaterialAttribute::SpecularTexture), 3);
    CORRADE_COMPARE(data.attributeOr("SpecularTexture", 5u), 3);
    CORRADE_COMPARE(data.attributeOr(MaterialAttribute::SpecularTexture, 5u), 3);

    /* This doesn't */
    CORRADE_VERIFY(!data.tryAttribute("DiffuseTexture"));
    CORRADE_VERIFY(!data.tryAttribute(MaterialAttribute::DiffuseTexture));
    CORRADE_VERIFY(!data.tryAttribute<UnsignedInt>("DiffuseTexture"));
    CORRADE_VERIFY(!data.tryAttribute<UnsignedInt>(MaterialAttribute::DiffuseTexture));
    CORRADE_COMPARE(data.attributeOr("DiffuseTexture", 5u), 5);
    CORRADE_COMPARE(data.attributeOr(MaterialAttribute::DiffuseTexture, 5u), 5);
}

void MaterialDataTest::accessOutOfBounds() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    data.attributeName(2);
    data.attributeType(2);
    data.attribute(2);
    data.attribute<Int>(2);
    data.attribute<Containers::StringView>(2);
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attributeName(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::attributeType(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 0\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 0\n");
}

void MaterialDataTest::accessNotFound() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {"DiffuseColor", 0xff3366aa_rgbaf}
    }};

    CORRADE_VERIFY(!data.hasAttribute("DiffuseColour"));

    std::ostringstream out;
    Error redirectError{&out};
    data.attributeId("DiffuseColour");
    data.attributeType("DiffuseColour");
    data.attribute("DiffuseColour");
    data.attribute<Color4>("DiffuseColour");
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attributeId(): attribute DiffuseColour not found in layer 0\n"
        "Trade::MaterialData::attributeType(): attribute DiffuseColour not found in layer 0\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer 0\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer 0\n");
}

void MaterialDataTest::accessWrongType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {"DiffuseColor", 0xff3366aa_rgbaf}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    data.attribute<Color3>(0);
    data.attribute<Color3>(MaterialAttribute::DiffuseColor);
    data.attribute<Color3>("DiffuseColor");
    data.tryAttribute<Color3>(MaterialAttribute::DiffuseColor);
    data.tryAttribute<Color3>("DiffuseColor");
    data.attributeOr(MaterialAttribute::DiffuseColor, Color3{1.0f});
    data.attributeOr("DiffuseColor", Color3{1.0f});
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attribute(): improper type requested for DiffuseColor of Trade::MaterialAttributeType::Vector4\n"
        "Trade::MaterialData::attribute(): improper type requested for DiffuseColor of Trade::MaterialAttributeType::Vector4\n"
        "Trade::MaterialData::attribute(): improper type requested for DiffuseColor of Trade::MaterialAttributeType::Vector4\n"
        /* tryAttribute() and attributeOr() delegate to attribute() so the
           assert is the same */
        "Trade::MaterialData::attribute(): improper type requested for DiffuseColor of Trade::MaterialAttributeType::Vector4\n"
        "Trade::MaterialData::attribute(): improper type requested for DiffuseColor of Trade::MaterialAttributeType::Vector4\n"
        "Trade::MaterialData::attribute(): improper type requested for DiffuseColor of Trade::MaterialAttributeType::Vector4\n"
        "Trade::MaterialData::attribute(): improper type requested for DiffuseColor of Trade::MaterialAttributeType::Vector4\n");
}

void MaterialDataTest::accessWrongPointerType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Int a = 3;
    const Double b = 57.0;

    MaterialData data{{}, {
        {"mutablePointer", &a},
        {"pointer", &b}
    }};

    /* These are fine (type is not checked) */
    data.attribute<Byte*>("mutablePointer");
    data.attribute<const Float*>("pointer");

    std::ostringstream out;
    Error redirectError{&out};
    data.attribute<const Int*>("mutablePointer");
    data.attribute<Double*>("pointer");
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attribute(): improper type requested for mutablePointer of Trade::MaterialAttributeType::MutablePointer\n"
        "Trade::MaterialData::attribute(): improper type requested for pointer of Trade::MaterialAttributeType::Pointer\n");
}

void MaterialDataTest::accessWrongTypeString() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {"Shininess", 0.0f}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    data.attribute<Containers::StringView>(0);
    data.attribute<Containers::StringView>(MaterialAttribute::Shininess);
    data.attribute<Containers::StringView>("Shininess");
    data.tryAttribute<Containers::StringView>(MaterialAttribute::Shininess);
    data.tryAttribute<Containers::StringView>("Shininess");
    data.attributeOr(MaterialAttribute::Shininess, Containers::StringView{});
    data.attributeOr("Shininess", Containers::StringView{});
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        /* tryAttribute() and attributeOr() delegate to attribute() so the
           assert is the same */
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n"
        "Trade::MaterialData::attribute(): Shininess of Trade::MaterialAttributeType::Float can't be retrieved as a string\n");
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
    }, {
        0, 1, 7
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
}

void MaterialDataTest::accessLayersTexturedSingleMatrixCoordinates() {
    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::LayerFactorTexture, 4u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::TextureCoordinates, 2u},
    }, {
        0, 4
    }};

    CORRADE_COMPARE(data.layerFactorTextureMatrix(1), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureMatrix("ClearCoat"), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureMatrix(MaterialLayer::ClearCoat), Matrix3::scaling({0.5f, 1.0f}));

    CORRADE_COMPARE(data.layerFactorTextureCoordinates(1), 2u);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates("ClearCoat"), 2u);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates(MaterialLayer::ClearCoat), 2u);
}

void MaterialDataTest::accessLayersTexturedBaseMaterialMatrixCoordinates() {
    MaterialData data{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::TextureCoordinates, 2u},

        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::LayerFactorTexture, 4u},
    }, {
        2, 4
    }};

    CORRADE_COMPARE(data.layerFactorTextureMatrix(1), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureMatrix("ClearCoat"), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureMatrix(MaterialLayer::ClearCoat), Matrix3::scaling({0.5f, 1.0f}));

    CORRADE_COMPARE(data.layerFactorTextureCoordinates(1), 2u);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates("ClearCoat"), 2u);
    CORRADE_COMPARE(data.layerFactorTextureCoordinates(MaterialLayer::ClearCoat), 2u);
}

void MaterialDataTest::accessLayersInvalidTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "ClearCoat"},
    }, {0, 1}};

    std::ostringstream out;
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
    CORRADE_COMPARE(out.str(),
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
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer ClearCoat doesn't have a factor texture\n");
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

void MaterialDataTest::accessLayerIndexOptional() {
    MaterialData data{{}, {
        {MaterialAttribute::DiffuseColor, 0x335566ff_rgbaf},
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }, {1, 3}};

    /* This exists */
    CORRADE_VERIFY(data.tryAttribute(1, "SpecularTexture"));
    CORRADE_VERIFY(data.tryAttribute(1, MaterialAttribute::SpecularTexture));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.tryAttribute(1, "SpecularTexture")), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.tryAttribute(1, MaterialAttribute::SpecularTexture)), 3);
    CORRADE_COMPARE(data.tryAttribute<UnsignedInt>(1, "SpecularTexture"), 3);
    CORRADE_COMPARE(data.tryAttribute<UnsignedInt>(1, MaterialAttribute::SpecularTexture), 3);
    CORRADE_COMPARE(data.attributeOr(1, "SpecularTexture", 5u), 3);
    CORRADE_COMPARE(data.attributeOr(1, MaterialAttribute::SpecularTexture, 5u), 3);

    /* This doesn't */
    CORRADE_VERIFY(!data.tryAttribute(1, "DiffuseTexture"));
    CORRADE_VERIFY(!data.tryAttribute(1, MaterialAttribute::DiffuseTexture));
    CORRADE_VERIFY(!data.tryAttribute<UnsignedInt>(1, "DiffuseTexture"));
    CORRADE_VERIFY(!data.tryAttribute<UnsignedInt>(1, MaterialAttribute::DiffuseTexture));
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
    CORRADE_VERIFY(data.tryAttribute(MaterialLayer::ClearCoat, "SpecularTexture"));
    CORRADE_VERIFY(data.tryAttribute(MaterialLayer::ClearCoat, MaterialAttribute::SpecularTexture));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.tryAttribute(MaterialLayer::ClearCoat, "SpecularTexture")), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.tryAttribute(MaterialLayer::ClearCoat, MaterialAttribute::SpecularTexture)), 3);
    CORRADE_COMPARE(data.tryAttribute<UnsignedInt>(MaterialLayer::ClearCoat, "SpecularTexture"), 3);
    CORRADE_COMPARE(data.tryAttribute<UnsignedInt>(MaterialLayer::ClearCoat, MaterialAttribute::SpecularTexture), 3);
    CORRADE_COMPARE(data.attributeOr(MaterialLayer::ClearCoat, "SpecularTexture", 5u), 3);
    CORRADE_COMPARE(data.attributeOr(MaterialLayer::ClearCoat, MaterialAttribute::SpecularTexture, 5u), 3);

    /* This doesn't */
    CORRADE_VERIFY(!data.tryAttribute(MaterialLayer::ClearCoat, "DiffuseTexture"));
    CORRADE_VERIFY(!data.tryAttribute(MaterialLayer::ClearCoat, MaterialAttribute::DiffuseTexture));
    CORRADE_VERIFY(!data.tryAttribute<UnsignedInt>(MaterialLayer::ClearCoat, "DiffuseTexture"));
    CORRADE_VERIFY(!data.tryAttribute<UnsignedInt>(MaterialLayer::ClearCoat, MaterialAttribute::DiffuseTexture));
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
    CORRADE_VERIFY(data.tryAttribute("ClearCoat", "SpecularTexture"));
    CORRADE_VERIFY(data.tryAttribute("ClearCoat", MaterialAttribute::SpecularTexture));
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.tryAttribute("ClearCoat", "SpecularTexture")), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.tryAttribute("ClearCoat", MaterialAttribute::SpecularTexture)), 3);
    CORRADE_COMPARE(data.tryAttribute<UnsignedInt>("ClearCoat", "SpecularTexture"), 3);
    CORRADE_COMPARE(data.tryAttribute<UnsignedInt>("ClearCoat", MaterialAttribute::SpecularTexture), 3);
    CORRADE_COMPARE(data.attributeOr("ClearCoat", "SpecularTexture", 5u), 3);
    CORRADE_COMPARE(data.attributeOr("ClearCoat", MaterialAttribute::SpecularTexture, 5u), 3);

    /* This doesn't */
    CORRADE_VERIFY(!data.tryAttribute("ClearCoat", "DiffuseTexture"));
    CORRADE_VERIFY(!data.tryAttribute("ClearCoat", MaterialAttribute::DiffuseTexture));
    CORRADE_VERIFY(!data.tryAttribute<UnsignedInt>("ClearCoat", "DiffuseTexture"));
    CORRADE_VERIFY(!data.tryAttribute<UnsignedInt>("ClearCoat", MaterialAttribute::DiffuseTexture));
    CORRADE_COMPARE(data.attributeOr("ClearCoat", "DiffuseTexture", 5u), 5);
    CORRADE_COMPARE(data.attributeOr("ClearCoat", MaterialAttribute::DiffuseTexture, 5u), 5);
}

void MaterialDataTest::accessLayerOutOfBounds() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }, {0, 2}};

    std::ostringstream out;
    Error redirectError{&out};
    data.layerName(2);
    data.layerFactor(2);
    data.layerFactorTexture(2);
    data.layerFactorTextureSwizzle(2);
    data.layerFactorTextureMatrix(2);
    data.layerFactorTextureCoordinates(2);
    data.attributeCount(2);
    data.hasAttribute(2, "AlphaMask");
    data.hasAttribute(2, MaterialAttribute::AlphaMask);
    data.attributeId(2, "AlphaMask");
    data.attributeId(2, MaterialAttribute::AlphaMask);
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
    data.tryAttribute(2, "AlphaMask");
    data.tryAttribute(2, MaterialAttribute::AlphaMask);
    data.tryAttribute<bool>(2, "AlphaMask");
    data.tryAttribute<bool>(2, MaterialAttribute::AlphaMask);
    data.attributeOr(2, "AlphaMask", false);
    data.attributeOr(2, MaterialAttribute::AlphaMask, false);
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::layerName(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactor(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactorTexture(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactorTextureSwizzle(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactorTextureMatrix(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::layerFactorTextureCoordinates(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeCount(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::hasAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::hasAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeId(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeId(): index 2 out of range for 2 layers\n"
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
        "Trade::MaterialData::tryAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::tryAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::tryAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::tryAttribute(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeOr(): index 2 out of range for 2 layers\n"
        "Trade::MaterialData::attributeOr(): index 2 out of range for 2 layers\n");
}

void MaterialDataTest::accessLayerNotFound() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "clearCoat"},
        {MaterialAttribute::AlphaMask, 0.5f},
    }, {0, 2}};

    std::ostringstream out;
    Error redirectError{&out};
    data.layerId("ClearCoat");
    data.layerFactor("ClearCoat");
    data.layerFactorTexture("ClearCoat");
    data.layerFactorTextureSwizzle("ClearCoat");
    data.layerFactorTextureMatrix("ClearCoat");
    data.layerFactorTextureCoordinates("ClearCoat");
    data.attributeCount("ClearCoat");
    data.hasAttribute("ClearCoat", "AlphaMask");
    data.hasAttribute("ClearCoat", MaterialAttribute::AlphaMask);
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
    data.tryAttribute("ClearCoat", "AlphaMask");
    data.tryAttribute("ClearCoat", MaterialAttribute::AlphaMask);
    data.tryAttribute<bool>("ClearCoat", "AlphaMask");
    data.tryAttribute<bool>("ClearCoat", MaterialAttribute::AlphaMask);
    data.attributeOr("ClearCoat", "AlphaMask", false);
    data.attributeOr("ClearCoat", MaterialAttribute::AlphaMask, false);
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::layerId(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactor(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactorTexture(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactorTextureSwizzle(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactorTextureMatrix(): layer ClearCoat not found\n"
        "Trade::MaterialData::layerFactorTextureCoordinates(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeCount(): layer ClearCoat not found\n"
        "Trade::MaterialData::hasAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::hasAttribute(): layer ClearCoat not found\n"
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
        "Trade::MaterialData::tryAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::tryAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::tryAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::tryAttribute(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeOr(): layer ClearCoat not found\n"
        "Trade::MaterialData::attributeOr(): layer ClearCoat not found\n");
}

void MaterialDataTest::accessInvalidLayerName() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {}};

    std::ostringstream out;
    Error redirectError{&out};
    data.layerId(MaterialLayer(0x0));
    data.layerId(MaterialLayer(0xfefe));
    data.layerFactor(MaterialLayer(0xfefe));
    data.layerFactorTexture(MaterialLayer(0xfefe));
    data.layerFactorTextureSwizzle(MaterialLayer(0xfefe));
    data.layerFactorTextureMatrix(MaterialLayer(0xfefe));
    data.layerFactorTextureCoordinates(MaterialLayer(0xfefe));
    data.attributeCount(MaterialLayer(0xfefe));
    data.hasAttribute(MaterialLayer(0xfefe), "AlphaMask");
    data.hasAttribute(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
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
    data.tryAttribute(MaterialLayer(0xfefe), "AlphaMask");
    data.tryAttribute(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.tryAttribute<bool>(MaterialLayer(0xfefe), "AlphaMask");
    data.tryAttribute<bool>(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask);
    data.attributeOr(MaterialLayer(0xfefe), "AlphaMask", false);
    data.attributeOr(MaterialLayer(0xfefe), MaterialAttribute::AlphaMask, false);
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::layerId(): invalid name Trade::MaterialLayer(0x0)\n"
        "Trade::MaterialData::layerId(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactor(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactorTexture(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactorTextureSwizzle(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactorTextureMatrix(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::layerFactorTextureCoordinates(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeCount(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::hasAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::hasAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
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
        "Trade::MaterialData::tryAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::tryAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::tryAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::tryAttribute(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeOr(): invalid name Trade::MaterialLayer(0xfefe)\n"
        "Trade::MaterialData::attributeOr(): invalid name Trade::MaterialLayer(0xfefe)\n");
}

void MaterialDataTest::accessOutOfBoundsInLayerIndex() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {MaterialAttribute::AlphaMask, 0.5f},
        {MaterialAttribute::SpecularTexture, 3u}
    }, {0, 2}};

    std::ostringstream out;
    Error redirectError{&out};
    data.attributeName(1, 2);
    data.attributeType(1, 2);
    data.attribute(1, 2);
    data.attribute<Int>(1, 2);
    data.attribute<Containers::StringView>(1, 2);
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attributeName(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::attributeType(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 1\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer 1\n");
}

void MaterialDataTest::accessOutOfBoundsInLayerString() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "ClearCoat"},
        {MaterialAttribute::AlphaMask, 0.5f}
    }, {0, 2}};

    std::ostringstream out;
    Error redirectError{&out};
    data.attributeName("ClearCoat", 2);
    data.attributeType("ClearCoat", 2);
    data.attribute("ClearCoat", 2);
    data.attribute<Int>("ClearCoat", 2);
    data.attribute<Containers::StringView>("ClearCoat", 2);
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attributeName(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::attributeType(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes in layer ClearCoat\n");
}

void MaterialDataTest::accessNotFoundInLayerIndex() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {"DiffuseColor", 0xff3366aa_rgbaf}
    }, {0, 1}};

    CORRADE_VERIFY(!data.hasAttribute(1, "DiffuseColour"));

    std::ostringstream out;
    Error redirectError{&out};
    data.attributeId(1, "DiffuseColour");
    data.attributeType(1, "DiffuseColour");
    data.attribute(1, "DiffuseColour");
    data.attribute<Color4>(1, "DiffuseColour");
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attributeId(): attribute DiffuseColour not found in layer 1\n"
        "Trade::MaterialData::attributeType(): attribute DiffuseColour not found in layer 1\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer 1\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer 1\n");
}

void MaterialDataTest::accessNotFoundInLayerString() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {
        {MaterialAttribute::LayerName, "ClearCoat"},
        {"DiffuseColor", 0xff3366aa_rgbaf}
    }, {0, 1}};

    CORRADE_VERIFY(!data.hasAttribute(1, "DiffuseColour"));

    std::ostringstream out;
    Error redirectError{&out};
    data.attributeId("ClearCoat", "DiffuseColour");
    data.attributeType("ClearCoat", "DiffuseColour");
    data.attribute("ClearCoat", "DiffuseColour");
    data.attribute<Color4>("ClearCoat", "DiffuseColour");
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attributeId(): attribute DiffuseColour not found in layer ClearCoat\n"
        "Trade::MaterialData::attributeType(): attribute DiffuseColour not found in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer ClearCoat\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found in layer ClearCoat\n");
}

void MaterialDataTest::accessInvalidAttributeName() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {}};

    /* The name should be converted to a string first and foremost and only
       then delegated to another overload. Which means all asserts should
       print the leaf function name. */
    std::ostringstream out;
    Error redirectError{&out};
    data.hasAttribute(0, MaterialAttribute(0x0));
    data.hasAttribute("Layer", MaterialAttribute(0xfefe));
    data.attributeId(0, MaterialAttribute(0x0));
    data.attributeId("Layer", MaterialAttribute(0xfefe));
    data.attributeType(0, MaterialAttribute(0x0));
    data.attributeType("Layer", MaterialAttribute(0xfefe));
    data.attribute(0, MaterialAttribute(0x0));
    data.attribute("Layer", MaterialAttribute(0xfefe));
    data.attribute<Int>(0, MaterialAttribute(0x0));
    data.attribute<Int>("Layer", MaterialAttribute(0xfefe));
    data.tryAttribute(0, MaterialAttribute(0x0));
    data.tryAttribute("Layer", MaterialAttribute(0xfefe));
    data.tryAttribute<Int>(0, MaterialAttribute(0x0));
    data.tryAttribute<Int>("Layer", MaterialAttribute(0xfefe));
    data.attributeOr(0, MaterialAttribute(0x0), 42);
    data.attributeOr("Layer", MaterialAttribute(0xfefe), 42);
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::hasAttribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::hasAttribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::attributeId(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::attributeId(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::attributeType(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::attributeType(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::attribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::tryAttribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::tryAttribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::tryAttribute(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::tryAttribute(): invalid name Trade::MaterialAttribute(0xfefe)\n"
        "Trade::MaterialData::attributeOr(): invalid name Trade::MaterialAttribute(0x0)\n"
        "Trade::MaterialData::attributeOr(): invalid name Trade::MaterialAttribute(0xfefe)\n");
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

#ifdef MAGNUM_BUILD_DEPRECATED
void MaterialDataTest::constructPhongDeprecated() {
    const int a{};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData data{PhongMaterialData::Flag::DoubleSided,
        0xccffbb_rgbf, {},
        0xebefbf_rgbf, {},
        0xacabad_rgbf, {}, {}, {},
        MaterialAlphaMode::Mask, 0.3f, 80.0f, &a};
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::DoubleSided);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.ambientColor(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xebefbf_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Mask);
    CORRADE_COMPARE(data.alphaMask(), 0.3f);
    CORRADE_COMPARE(data.shininess(), 80.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void MaterialDataTest::constructPhongDeprecatedTextured() {
    const int a{};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData data{
        PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture,
        0x111111_rgbf, 42,
        0xeebbff_rgbf, {},
        0xacabad_rgbf, 17, {}, {},
        MaterialAlphaMode::Blend, 0.37f, 96.0f, &a};
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.ambientTexture(), 42);
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 0);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureCoordinates(), 0);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Blend);
    CORRADE_COMPARE(data.alphaMask(), 0.37f);
    CORRADE_COMPARE(data.shininess(), 96.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void MaterialDataTest::constructPhongDeprecatedTexturedTextureTransform() {
    const int a{};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData data{
        PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureTransformation,
        0x111111_rgbf, {},
        0xeebbff_rgbf, 42,
        0xacabad_rgbf, {}, 17,
        Matrix3::rotation(90.0_degf),
        MaterialAlphaMode::Mask, 0.5f, 96.0f, &a};
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureTransformation);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.diffuseTexture(), 42);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.normalTexture(), 17);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::rotation(90.0_degf));
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Mask);
    CORRADE_COMPARE(data.alphaMask(), 0.5f);
    CORRADE_COMPARE(data.shininess(), 96.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void MaterialDataTest::constructPhongDeprecatedTexturedCoordinates() {
    const int a{};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData data{
        PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureCoordinates,
        0x111111_rgbf, 42, 3,
        0xeebbff_rgbf, {}, 6,
        0xacabad_rgbf, 17, 1,
        0, 8, {},
        MaterialAlphaMode::Blend, 0.37f, 96.0f, &a};
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureCoordinates);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.ambientTexture(), 42);
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 3);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 6);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureCoordinates(), 1);
    CORRADE_COMPARE(data.normalTexture(), 0);
    CORRADE_COMPARE(data.normalTextureCoordinates(), 8);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 3);
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Blend);
    CORRADE_COMPARE(data.alphaMask(), 0.37f);
    CORRADE_COMPARE(data.shininess(), 96.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void MaterialDataTest::constructPhongDeprecatedTextureTransformNoTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData a{PhongMaterialData::Flag::TextureTransformation,
        {}, {},
        {}, {},
        {}, {}, {}, {},
        {}, 0.5f, 80.0f};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(),
        "Trade::PhongMaterialData: texture transformation enabled but the material has no textures\n");
}

void MaterialDataTest::constructPhongDeprecatedNoTextureTransformationFlag() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData a{{},
        {}, {},
        {}, {},
        {}, {}, {}, Matrix3::rotation(90.0_degf),
        {}, 0.5f, 80.0f};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(),
        "PhongMaterialData::PhongMaterialData: non-default texture matrix requires Flag::TextureTransformation to be enabled\n");
}

void MaterialDataTest::constructPhongDeprecatedNoTextureCoordinatesFlag() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData a{{},
        {}, {}, 1,
        {}, {}, 2,
        {}, {}, 3, {}, 4, {},
        {}, 0.5f, 80.0f};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(),
        "PhongMaterialData::PhongMaterialData: non-zero texture coordinate sets require Flag::TextureCoordinates to be enabled\n");
}
#endif

void MaterialDataTest::pbrMetallicRoughnessAccess() {
    MaterialData base{MaterialType::PbrMetallicRoughness, {
        {MaterialAttribute::BaseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::Metalness, 0.5f},
        {MaterialAttribute::Roughness, 0.79f},
        {MaterialAttribute::EmissiveColor, 0x111111_rgbf}
    }};

    CORRADE_COMPARE(base.types(), MaterialType::PbrMetallicRoughness);
    const auto& data = base.as<PbrMetallicRoughnessMaterialData>();

    CORRADE_VERIFY(!data.hasMetalnessTexture());
    CORRADE_VERIFY(!data.hasRoughnessTexture());
    CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.baseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(data.metalness(), 0.5f);
    CORRADE_COMPARE(data.roughness(), 0.79f);
    CORRADE_COMPARE(data.emissiveColor(), 0x111111_rgbf);
}

void MaterialDataTest::pbrMetallicRoughnessAccessDefaults() {
    MaterialData base{{}, {}};

    CORRADE_COMPARE(base.types(), MaterialTypes{});
    /* Casting is fine even if the type doesn't include PbrMetallicRoughness */
    const auto& data = base.as<PbrMetallicRoughnessMaterialData>();

    CORRADE_VERIFY(!data.hasMetalnessTexture());
    CORRADE_VERIFY(!data.hasRoughnessTexture());
    CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.baseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.metalness(), 1.0f);
    CORRADE_COMPARE(data.roughness(), 1.0f);
    CORRADE_COMPARE(data.emissiveColor(), 0x000000_rgbf);
}

void MaterialDataTest::pbrMetallicRoughnessAccessTextured() {
    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::BaseColorTexture, 0u},
        {MaterialAttribute::BaseColorTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::BaseColorTextureCoordinates, 2u},
        {MaterialAttribute::Metalness, 0.5f},
        {MaterialAttribute::MetalnessTexture, 1u},
        {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
        {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::MetalnessTextureCoordinates, 3u},
        {MaterialAttribute::Roughness, 0.79f},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::A},
        {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({1.0f, 1.0f})},
        {MaterialAttribute::RoughnessTextureCoordinates, 4u},
        {MaterialAttribute::NormalTexture, 3u},
        {MaterialAttribute::NormalTextureScale, 0.35f},
        {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::BA},
        {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({1.0f, 0.5f})},
        {MaterialAttribute::NormalTextureCoordinates, 5u},
        {MaterialAttribute::OcclusionTexture, 4u},
        {MaterialAttribute::OcclusionTextureStrength, 0.66f},
        {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({1.0f, 0.75f})},
        {MaterialAttribute::OcclusionTextureCoordinates, 6u},
        {MaterialAttribute::EmissiveColor, 0x111111_rgbf},
        {MaterialAttribute::EmissiveTexture, 5u},
        {MaterialAttribute::EmissiveTextureMatrix, Matrix3::scaling({0.75f, 0.5f})},
        {MaterialAttribute::EmissiveTextureCoordinates, 7u}
    }};

    CORRADE_VERIFY(data.hasMetalnessTexture());
    CORRADE_VERIFY(data.hasRoughnessTexture());
    CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.baseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(data.baseColorTexture(), 0);
    CORRADE_COMPARE(data.baseColorTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.baseColorTextureCoordinates(), 2);
    CORRADE_COMPARE(data.metalness(), 0.5f);
    CORRADE_COMPARE(data.metalnessTexture(), 1);
    CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::G);
    CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.metalnessTextureCoordinates(), 3);
    CORRADE_COMPARE(data.roughness(), 0.79f);
    CORRADE_COMPARE(data.roughnessTexture(), 2);
    CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::A);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({1.0f, 1.0f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 4);
    CORRADE_COMPARE(data.normalTexture(), 3);
    CORRADE_COMPARE(data.normalTextureScale(), 0.35f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::BA);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({1.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 5);
    CORRADE_COMPARE(data.occlusionTexture(), 4);
    CORRADE_COMPARE(data.occlusionTextureStrength(), 0.66f);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3::scaling({1.0f, 0.75f}));
    CORRADE_COMPARE(data.occlusionTextureSwizzle(), MaterialTextureSwizzle::B);
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 6);
    CORRADE_COMPARE(data.emissiveColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3::scaling({0.75f, 0.5f}));
    CORRADE_COMPARE(data.emissiveTexture(), 5);
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 7);
}

void MaterialDataTest::pbrMetallicRoughnessAccessTexturedDefaults() {
    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 1u},
        {MaterialAttribute::MetalnessTexture, 2u},
        {MaterialAttribute::RoughnessTexture, 3u},
        {MaterialAttribute::NormalTexture, 4u},
        {MaterialAttribute::OcclusionTexture, 5u},
        {MaterialAttribute::EmissiveTexture, 6u}
    }};

    CORRADE_VERIFY(data.hasMetalnessTexture());
    CORRADE_VERIFY(data.hasRoughnessTexture());
    CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.baseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.baseColorTexture(), 1);
    CORRADE_COMPARE(data.baseColorTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.baseColorTextureCoordinates(), 0);
    CORRADE_COMPARE(data.metalness(), 1.0f);
    CORRADE_COMPARE(data.metalnessTexture(), 2);
    CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.metalnessTextureCoordinates(), 0);
    CORRADE_COMPARE(data.roughness(), 1.0f);
    CORRADE_COMPARE(data.roughnessTexture(), 3);
    CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 0);
    CORRADE_COMPARE(data.normalTexture(), 4);
    CORRADE_COMPARE(data.normalTextureScale(), 1.0f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.normalTextureCoordinates(), 0);
    CORRADE_COMPARE(data.occlusionTexture(), 5);
    CORRADE_COMPARE(data.occlusionTextureStrength(), 1.0f);
    CORRADE_COMPARE(data.occlusionTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 0);
    CORRADE_COMPARE(data.emissiveColor(), 0x000000_rgbf);
    CORRADE_COMPARE(data.emissiveTexture(), 6);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 0);
}

void MaterialDataTest::pbrMetallicRoughnessAccessTexturedSingleMatrixCoordinates() {
    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 1u},
        {MaterialAttribute::MetalnessTexture, 2u},
        {MaterialAttribute::RoughnessTexture, 3u},
        {MaterialAttribute::NormalTexture, 4u},
        {MaterialAttribute::OcclusionTexture, 5u},
        {MaterialAttribute::EmissiveTexture, 6u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u}
    }};

    CORRADE_COMPARE(data.baseColorTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.baseColorTextureCoordinates(), 7);
    CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.metalnessTextureCoordinates(), 7);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 7);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 7);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 7);
}

void MaterialDataTest::pbrMetallicRoughnessAccessTexturedImplicitPackedMetallicRoughness() {
    /* Just the texture ID, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());
        CORRADE_COMPARE(data.roughnessTexture(), 2);
        CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::G);
        CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.roughnessTextureCoordinates(), 0);
        CORRADE_COMPARE(data.metalnessTexture(), 2);
        CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::B);
        CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.metalnessTextureCoordinates(), 0);

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u}
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());
        CORRADE_COMPARE(data.roughnessTexture(), 2);
        CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::G);
        CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.roughnessTextureCoordinates(), 3);
        CORRADE_COMPARE(data.metalnessTexture(), 2);
        CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::B);
        CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.metalnessTextureCoordinates(), 3);

    /* Swizzle is ignored when the combined texture is specified, so this is
       fine */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::RoughnessTextureCoordinates, 1u},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    }
}

void MaterialDataTest::pbrMetallicRoughnessAccessTexturedExplicitPackedMetallicRoughness() {
    /* Just the texture IDs and swizzles, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B}
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u}
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Different texture ID */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 3u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B}
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* One texture missing */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Unexpected swizzle */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::R},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureCoordinates, 1u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    }
}

void MaterialDataTest::pbrMetallicRoughnessAccessTexturedExplicitPackedRoughnessMetallicOcclusion() {
    /* Just the texture IDs and swizzles, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(data.hasRoughnessMetallicOcclusionTexture());
        /* This isn't a superset */
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::R},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::OcclusionTextureCoordinates, 3u}
        }};
        CORRADE_VERIFY(data.hasRoughnessMetallicOcclusionTexture());
        /* This isn't a superset */
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Different texture ID */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 3u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());

    /* One texture missing */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());

    /* Unexpected swizzle */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 1u},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    }
}

void MaterialDataTest::pbrMetallicRoughnessAccessTexturedExplicitPackedOcclusionRoughnessMetallic() {
    /* Just the texture IDs and swizzles, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(data.hasOcclusionRoughnessMetallicTexture());
        /* This is a superset */
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::R},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::OcclusionTextureCoordinates, 3u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u}
        }};
        CORRADE_VERIFY(data.hasOcclusionRoughnessMetallicTexture());
        /* This is a superset */
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Different texture ID */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::RoughnessTexture, 3u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* One texture missing */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected swizzle */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 1u},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    }
}

void MaterialDataTest::pbrMetallicRoughnessAccessTexturedExplicitPackedNormalRoughnessMetallic() {
    /* Just the texture IDs and swizzles, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A}
        }};
        CORRADE_VERIFY(data.hasNormalRoughnessMetallicTexture());

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::NormalTextureCoordinates, 3u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u}
        }};
        CORRADE_VERIFY(data.hasNormalRoughnessMetallicTexture());

    /* Different texture ID */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 3u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A}
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());

    /* One texture missing */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());

    /* Unexpected swizzle */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RGB},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::MetalnessTextureCoordinates, 1u},
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    }
}

void MaterialDataTest::pbrMetallicRoughnessAccessInvalidTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PbrMetallicRoughnessMaterialData data{{}, {}};

    std::ostringstream out;
    Error redirectError{&out};
    data.baseColorTexture();
    data.baseColorTextureMatrix();
    data.baseColorTextureCoordinates();
    data.metalnessTexture();
    data.metalnessTextureSwizzle();
    data.metalnessTextureMatrix();
    data.metalnessTextureCoordinates();
    data.roughnessTexture();
    data.roughnessTextureSwizzle();
    data.roughnessTextureMatrix();
    data.roughnessTextureCoordinates();
    data.normalTexture();
    data.normalTextureScale();
    data.normalTextureSwizzle();
    data.normalTextureMatrix();
    data.normalTextureCoordinates();
    data.occlusionTexture();
    data.occlusionTextureStrength();
    data.occlusionTextureSwizzle();
    data.occlusionTextureMatrix();
    data.occlusionTextureCoordinates();
    data.emissiveTexture();
    data.emissiveTextureMatrix();
    data.emissiveTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attribute(): attribute BaseColorTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::baseColorTextureMatrix(): the material doesn't have a base color texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::baseColorTextureCoordinates(): the material doesn't have a base color texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTexture(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureSwizzle(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureMatrix(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureCoordinates(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTexture(): the material doesn't have a roughness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureSwizzle(): the material doesn't have a roughness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureMatrix(): the material doesn't have a roughness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureCoordinates(): the material doesn't have a roughness texture\n"
        "Trade::MaterialData::attribute(): attribute NormalTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureScale(): the material doesn't have a normal texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureSwizzle(): the material doesn't have a normal texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureMatrix(): the material doesn't have a normal texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture\n"
        "Trade::MaterialData::attribute(): attribute OcclusionTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureStrength(): the material doesn't have an occlusion texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureSwizzle(): the material doesn't have an occlusion texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureMatrix(): the material doesn't have an occlusion texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureCoordinates(): the material doesn't have an occlusion texture\n"
        "Trade::MaterialData::attribute(): attribute EmissiveTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::emissiveTextureMatrix(): the material doesn't have an emissive texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::emissiveTextureCoordinates(): the material doesn't have an emissive texture\n");
}

void MaterialDataTest::pbrMetallicRoughnessAccessCommonTransformationCoordinatesNoTextures() {
    PbrMetallicRoughnessMaterialData a{{}, {}};
    CORRADE_VERIFY(a.hasCommonTextureTransformation());
    CORRADE_VERIFY(a.hasCommonTextureCoordinates());
    CORRADE_COMPARE(a.commonTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(a.commonTextureCoordinates(), 0);

    PbrMetallicRoughnessMaterialData b{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u}
    }};
    CORRADE_VERIFY(b.hasCommonTextureTransformation());
    CORRADE_VERIFY(b.hasCommonTextureCoordinates());
    CORRADE_COMPARE(b.commonTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(b.commonTextureCoordinates(), 7);
}

void MaterialDataTest::pbrMetallicRoughnessAccessCommonTransformationCoordinatesOneTexture() {
    Containers::StringView textureName = PbrMetallicRoughnessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrMetallicRoughnessMaterialData data{{}, {
        {textureName, 5u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u},

        /* These shouldn't affect the above */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(data.hasCommonTextureTransformation());
    CORRADE_COMPARE(data.commonTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_COMPARE(data.commonTextureCoordinates(), 17u);
}

void MaterialDataTest::pbrMetallicRoughnessAccessCommonTransformationCoordinatesOneDifferentTexture() {
    Containers::StringView textureName = PbrMetallicRoughnessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 2u},
        {MaterialAttribute::MetalnessTexture, 3u},
        {MaterialAttribute::RoughnessTexture, 4u},
        {MaterialAttribute::NormalTexture, 5u},
        {MaterialAttribute::OcclusionTexture, 6u},
        {MaterialAttribute::EmissiveTexture, 7u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u},

        /* These are used by all textures except the one above, failing the
           check */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());
}

void MaterialDataTest::pbrMetallicRoughnessAccessNoCommonTransformationCoordinates() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 3u},
        {MaterialAttribute::BaseColorTextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::BaseColorTextureCoordinates, 3u},
        {MaterialAttribute::MetalnessTexture, 4u},
        {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::RoughnessTexture, 5u},
        {MaterialAttribute::RoughnessTextureCoordinates, 17u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());

    std::ostringstream out;
    Error redirectError{&out};
    data.commonTextureMatrix();
    data.commonTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::PbrMetallicRoughnessMaterialData::commonTextureMatrix(): the material doesn't have a common texture coordinate transformation\n"
        "Trade::PbrMetallicRoughnessMaterialData::commonTextureCoordinates(): the material doesn't have a common texture coordinate set\n");
}

void MaterialDataTest::pbrSpecularGlossinessAccess() {
    MaterialData base{MaterialType::PbrSpecularGlossiness, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::SpecularColor, 0xff336600_rgbaf},
        {MaterialAttribute::Glossiness, 0.79f},
        {MaterialAttribute::EmissiveColor, 0x111111_rgbf}
    }};

    CORRADE_COMPARE(base.types(), MaterialType::PbrSpecularGlossiness);
    const auto& data = base.as<PbrSpecularGlossinessMaterialData>();

    CORRADE_VERIFY(!data.hasSpecularTexture());
    CORRADE_VERIFY(!data.hasGlossinessTexture());
    CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.diffuseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(data.specularColor(), 0xff336600_rgbaf);
    CORRADE_COMPARE(data.glossiness(), 0.79f);
}

void MaterialDataTest::pbrSpecularGlossinessAccessDefaults() {
    MaterialData base{{}, {}};

    CORRADE_COMPARE(base.types(), MaterialTypes{});
    /* Casting is fine even if the type doesn't include PbrMetallicRoughness */
    const auto& data = base.as<PbrSpecularGlossinessMaterialData>();

    CORRADE_VERIFY(!data.hasSpecularTexture());
    CORRADE_VERIFY(!data.hasGlossinessTexture());
    CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.diffuseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.specularColor(), 0xffffff00_rgbaf);
    CORRADE_COMPARE(data.glossiness(), 1.0f);
}

void MaterialDataTest::pbrSpecularGlossinessAccessTextured() {
    PbrSpecularGlossinessMaterialData data{{}, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseTexture, 0u},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 2u},
        {MaterialAttribute::SpecularColor, 0x33556600_rgbaf},
        {MaterialAttribute::SpecularTexture, 1u},
        {MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGBA},
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::SpecularTextureCoordinates, 3u},
        {MaterialAttribute::Glossiness, 0.79f},
        {MaterialAttribute::GlossinessTexture, 2u},
        {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A},
        {MaterialAttribute::GlossinessTextureMatrix, Matrix3::scaling({1.0f, 1.0f})},
        {MaterialAttribute::GlossinessTextureCoordinates, 4u},
        {MaterialAttribute::NormalTexture, 3u},
        {MaterialAttribute::NormalTextureScale, 0.35f},
        {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::BA},
        {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({1.0f, 0.5f})},
        {MaterialAttribute::NormalTextureCoordinates, 5u},
        {MaterialAttribute::OcclusionTexture, 4u},
        {MaterialAttribute::OcclusionTextureStrength, 0.66f},
        {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({1.0f, 0.75f})},
        {MaterialAttribute::OcclusionTextureCoordinates, 6u},
        {MaterialAttribute::EmissiveColor, 0x111111_rgbf},
        {MaterialAttribute::EmissiveTexture, 5u},
        {MaterialAttribute::EmissiveTextureMatrix, Matrix3::scaling({0.75f, 0.5f})},
        {MaterialAttribute::EmissiveTextureCoordinates, 7u}
    }};

    CORRADE_VERIFY(data.hasSpecularTexture());
    CORRADE_VERIFY(data.hasGlossinessTexture());
    CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.diffuseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(data.diffuseTexture(), 0);
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 2);
    CORRADE_COMPARE(data.specularColor(), 0x33556600_rgbaf);
    CORRADE_COMPARE(data.specularTexture(), 1);
    CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGBA);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 3);
    CORRADE_COMPARE(data.glossiness(), 0.79f);
    CORRADE_COMPARE(data.glossinessTexture(), 2);
    CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::A);
    CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3::scaling({1.0f, 1.0f}));
    CORRADE_COMPARE(data.glossinessTextureCoordinates(), 4);
    CORRADE_COMPARE(data.normalTexture(), 3);
    CORRADE_COMPARE(data.normalTextureScale(), 0.35f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::BA);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({1.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 5);
    CORRADE_COMPARE(data.occlusionTexture(), 4);
    CORRADE_COMPARE(data.occlusionTextureStrength(), 0.66f);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3::scaling({1.0f, 0.75f}));
    CORRADE_COMPARE(data.occlusionTextureSwizzle(), MaterialTextureSwizzle::B);
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 6);
    CORRADE_COMPARE(data.emissiveColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3::scaling({0.75f, 0.5f}));
    CORRADE_COMPARE(data.emissiveTexture(), 5);
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 7);
}

void MaterialDataTest::pbrSpecularGlossinessAccessTexturedDefaults() {
    PbrSpecularGlossinessMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 1u},
        {MaterialAttribute::SpecularTexture, 2u},
        {MaterialAttribute::GlossinessTexture, 3u},
        {MaterialAttribute::NormalTexture, 4u},
        {MaterialAttribute::OcclusionTexture, 5u},
        {MaterialAttribute::EmissiveTexture, 6u}
    }};

    CORRADE_VERIFY(data.hasSpecularTexture());
    CORRADE_VERIFY(data.hasGlossinessTexture());
    CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.diffuseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.diffuseTexture(), 1);
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 0);
    CORRADE_COMPARE(data.specularColor(), 0xffffff00_rgbaf);
    CORRADE_COMPARE(data.specularTexture(), 2);
    CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.specularTextureCoordinates(), 0);
    CORRADE_COMPARE(data.glossiness(), 1.0f);
    CORRADE_COMPARE(data.glossinessTexture(), 3);
    CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.glossinessTextureCoordinates(), 0);
    CORRADE_COMPARE(data.normalTexture(), 4);
    CORRADE_COMPARE(data.normalTextureScale(), 1.0f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.normalTextureCoordinates(), 0);
    CORRADE_COMPARE(data.occlusionTexture(), 5);
    CORRADE_COMPARE(data.occlusionTextureStrength(), 1.0f);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.occlusionTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 0);
    CORRADE_COMPARE(data.emissiveColor(), 0x000000_rgbf);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.emissiveTexture(), 6);
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 0);
}

void MaterialDataTest::pbrSpecularGlossinessAccessTexturedSingleMatrixCoordinates() {
    PbrSpecularGlossinessMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 1u},
        {MaterialAttribute::SpecularTexture, 2u},
        {MaterialAttribute::GlossinessTexture, 3u},
        {MaterialAttribute::NormalTexture, 4u},
        {MaterialAttribute::OcclusionTexture, 5u},
        {MaterialAttribute::EmissiveTexture, 6u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u}
    }};

    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 7);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 7);
    CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.glossinessTextureCoordinates(), 7);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 7);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 7);
}

void MaterialDataTest::pbrSpecularGlossinessAccessTexturedImplicitPackedSpecularGlossiness() {
    /* Just the texture ID, the rest is implicit */
    {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularGlossinessTexture, 2u},
        }};
        CORRADE_VERIFY(data.hasSpecularGlossinessTexture());
        CORRADE_COMPARE(data.specularTexture(), 2);
        CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
        CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.specularTextureCoordinates(), 0);
        CORRADE_COMPARE(data.glossinessTexture(), 2);
        CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::A);
        CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.glossinessTextureCoordinates(), 0);

    /* Explicit parameters for everything, but all the same */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularGlossinessTexture, 2u},
            {MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGB},
            {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::SpecularTextureCoordinates, 3u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::GlossinessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::GlossinessTextureCoordinates, 3u}
        }};
        CORRADE_VERIFY(data.hasSpecularGlossinessTexture());
        CORRADE_COMPARE(data.specularTexture(), 2);
        CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
        CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.specularTextureCoordinates(), 3);
        CORRADE_COMPARE(data.glossinessTexture(), 2);
        CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::A);
        CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.glossinessTextureCoordinates(), 3);

    /* Swizzle is ignored when the combined texture is specified, so this is
       fine. */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularGlossinessTexture, 2u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(data.hasSpecularGlossinessTexture());

    /* Unexpected texture matrix */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularGlossinessTexture, 2u},
            {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        }};
        CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());

    /* Unexpected texture coordinates */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularGlossinessTexture, 2u},
            {MaterialAttribute::GlossinessTextureCoordinates, 1u},
        }};
        CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());
    }
}

void MaterialDataTest::pbrSpecularGlossinessAccessTexturedExplicitPackedSpecularGlossiness() {
    /* Just the texture ID and swizzles, the rest is implicit */
    {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularTexture, 2u},
            {MaterialAttribute::GlossinessTexture, 2u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A}
        }};
        CORRADE_VERIFY(data.hasSpecularGlossinessTexture());
        CORRADE_COMPARE(data.specularTexture(), 2);
        CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
        CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.specularTextureCoordinates(), 0);
        CORRADE_COMPARE(data.glossinessTexture(), 2);
        CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::A);
        CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.glossinessTextureCoordinates(), 0);

    /* Explicit parameters for everything, but all the same */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularTexture, 2u},
            {MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGB},
            {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::SpecularTextureCoordinates, 3u},
            {MaterialAttribute::GlossinessTexture, 2u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::GlossinessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::GlossinessTextureCoordinates, 3u}
        }};
        CORRADE_VERIFY(data.hasSpecularGlossinessTexture());
        CORRADE_COMPARE(data.specularTexture(), 2);
        CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
        CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.specularTextureCoordinates(), 3);
        CORRADE_COMPARE(data.glossinessTexture(), 2);
        CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::A);
        CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.glossinessTextureCoordinates(), 3);

    /* Different texture ID */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularTexture, 2u},
            {MaterialAttribute::GlossinessTexture, 3u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A}
        }};
        CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());

    /* Unexpected swizzle 1 */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularTexture, 2u},
            {MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGBA},
            {MaterialAttribute::GlossinessTexture, 2u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A}
        }};
        CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());

    /* Unexpected swizzle 2 */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularTexture, 2u},
            {MaterialAttribute::GlossinessTexture, 2u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::B}
        }};
        CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());

    /* Unexpected texture matrix */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularTexture, 2u},
            {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::GlossinessTexture, 2u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A}
        }};
        CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());

    /* Unexpected texture coordinates */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularTexture, 2u},
            {MaterialAttribute::GlossinessTexture, 2u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::GlossinessTextureCoordinates, 1u}
        }};
        CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());
    }
}

void MaterialDataTest::pbrSpecularGlossinessAccessInvalidTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PbrSpecularGlossinessMaterialData data{{}, {}};

    std::ostringstream out;
    Error redirectError{&out};
    data.diffuseTexture();
    data.diffuseTextureMatrix();
    data.diffuseTextureCoordinates();
    data.specularTexture();
    data.specularTextureSwizzle();
    data.specularTextureMatrix();
    data.specularTextureCoordinates();
    data.glossinessTexture();
    data.glossinessTextureSwizzle();
    data.glossinessTextureMatrix();
    data.glossinessTextureCoordinates();
    data.normalTexture();
    data.normalTextureScale();
    data.normalTextureSwizzle();
    data.normalTextureMatrix();
    data.normalTextureCoordinates();
    data.occlusionTexture();
    data.occlusionTextureStrength();
    data.occlusionTextureSwizzle();
    data.occlusionTextureMatrix();
    data.occlusionTextureCoordinates();
    data.emissiveTexture();
    data.emissiveTextureMatrix();
    data.emissiveTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attribute(): attribute DiffuseTexture not found in layer 0\n"
        "Trade::PbrSpecularGlossinessMaterialData::diffuseTextureMatrix(): the material doesn't have a diffuse texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::diffuseTextureCoordinates(): the material doesn't have a diffuse texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::specularTexture(): the material doesn't have a specular texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::specularTextureSwizzle(): the material doesn't have a specular texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::specularTextureMatrix(): the material doesn't have a specular texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::specularTextureCoordinates(): the material doesn't have a specular texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTexture(): the material doesn't have a glossiness texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTextureSwizzle(): the material doesn't have a glossiness texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTextureMatrix(): the material doesn't have a glossiness texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTextureCoordinates(): the material doesn't have a glossiness texture\n"
        "Trade::MaterialData::attribute(): attribute NormalTexture not found in layer 0\n"
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureScale(): the material doesn't have a normal texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureSwizzle(): the material doesn't have a normal texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureMatrix(): the material doesn't have a normal texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture\n"
        "Trade::MaterialData::attribute(): attribute OcclusionTexture not found in layer 0\n"
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureStrength(): the material doesn't have an occlusion texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureSwizzle(): the material doesn't have an occlusion texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureMatrix(): the material doesn't have an occlusion texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureCoordinates(): the material doesn't have an occlusion texture\n"
        "Trade::MaterialData::attribute(): attribute EmissiveTexture not found in layer 0\n"
        "Trade::PbrSpecularGlossinessMaterialData::emissiveTextureMatrix(): the material doesn't have an emissive texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::emissiveTextureCoordinates(): the material doesn't have an emissive texture\n");
}

void MaterialDataTest::pbrSpecularGlossinessAccessCommonTransformationCoordinatesNoTextures() {
    PbrSpecularGlossinessMaterialData a{{}, {}};
    CORRADE_VERIFY(a.hasCommonTextureTransformation());
    CORRADE_VERIFY(a.hasCommonTextureCoordinates());
    CORRADE_COMPARE(a.commonTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(a.commonTextureCoordinates(), 0);

    PbrSpecularGlossinessMaterialData b{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u}
    }};
    CORRADE_VERIFY(b.hasCommonTextureTransformation());
    CORRADE_VERIFY(b.hasCommonTextureCoordinates());
    CORRADE_COMPARE(b.commonTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(b.commonTextureCoordinates(), 7);
}

void MaterialDataTest::pbrSpecularGlossinessAccessCommonTransformationCoordinatesOneTexture() {
    Containers::StringView textureName = PbrSpecularGlossinessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrSpecularGlossinessMaterialData data{{}, {
        {textureName, 5u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u},

        /* These shouldn't affect the above */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(data.hasCommonTextureTransformation());
    CORRADE_COMPARE(data.commonTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_COMPARE(data.commonTextureCoordinates(), 17u);
}

void MaterialDataTest::pbrSpecularGlossinessAccessCommonTransformationCoordinatesOneDifferentTexture() {
    Containers::StringView textureName = PbrSpecularGlossinessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrSpecularGlossinessMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 2u},
        {MaterialAttribute::SpecularTexture, 3u},
        {MaterialAttribute::GlossinessTexture, 4u},
        {MaterialAttribute::NormalTexture, 5u},
        {MaterialAttribute::OcclusionTexture, 6u},
        {MaterialAttribute::EmissiveTexture, 7u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u},

        /* These are used by all textures except the one above, failing the
           check */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());
}

void MaterialDataTest::pbrSpecularGlossinessAccessNoCommonTransformationCoordinates() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PbrSpecularGlossinessMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 3u},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 3u},
        {MaterialAttribute::SpecularTexture, 4u},
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::OcclusionTexture, 5u},
        {MaterialAttribute::OcclusionTextureCoordinates, 17u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());

    std::ostringstream out;
    Error redirectError{&out};
    data.commonTextureMatrix();
    data.commonTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::PbrSpecularGlossinessMaterialData::commonTextureMatrix(): the material doesn't have a common texture coordinate transformation\n"
        "Trade::PbrSpecularGlossinessMaterialData::commonTextureCoordinates(): the material doesn't have a common texture coordinate set\n");
}

void MaterialDataTest::phongAccess() {
    MaterialData base{MaterialType::Phong, {
        {MaterialAttribute::AmbientColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseColor, 0xebefbfff_rgbaf},
        {MaterialAttribute::SpecularColor, 0xacabadff_rgbaf},
        {MaterialAttribute::Shininess, 96.0f}
    }};

    CORRADE_COMPARE(base.types(), MaterialType::Phong);
    const auto& data = base.as<PhongMaterialData>();

    CORRADE_VERIFY(!data.hasSpecularTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientColor(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xebefbf_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.shininess(), 96.0f);
}

void MaterialDataTest::phongAccessDefaults() {
    MaterialData base{{}, {}};

    CORRADE_COMPARE(base.types(), MaterialTypes{});
    /* Casting is fine even if the type doesn't include Phong */
    const auto& data = base.as<PhongMaterialData>();

    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientColor(), 0x000000_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xffffff_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xffffff00_rgbaf);
    CORRADE_COMPARE(data.shininess(), 80.0f);
}

void MaterialDataTest::phongAccessTextured() {
    PhongMaterialData data{{}, {
        {MaterialAttribute::AmbientColor, 0x111111ff_rgbaf},
        {MaterialAttribute::AmbientTexture, 42u},
        {MaterialAttribute::AmbientTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::AmbientTextureCoordinates, 2u},
        {MaterialAttribute::DiffuseTexture, 33u},
        {MaterialAttribute::DiffuseColor, 0xeebbffff_rgbaf},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 3u},
        {MaterialAttribute::SpecularColor, 0xacabadff_rgbaf},
        {MaterialAttribute::SpecularTexture, 17u},
        {MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGBA},
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({1.0f, 1.0f})},
        {MaterialAttribute::SpecularTextureCoordinates, 4u},
        {MaterialAttribute::NormalTexture, 0u},
        {MaterialAttribute::NormalTextureScale, 0.5f},
        {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::GB},
        {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({1.0f, 0.5f})},
        {MaterialAttribute::NormalTextureCoordinates, 5u}
    }};

    CORRADE_VERIFY(data.hasSpecularTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.ambientTexture(), 42);
    CORRADE_COMPARE(data.ambientTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 2);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.diffuseTexture(), 33);
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 3);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGBA);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({1.0f, 1.0f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 4);
    CORRADE_COMPARE(data.normalTexture(), 0);
    CORRADE_COMPARE(data.normalTextureScale(), 0.5f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::GB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({1.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 5);
}

void MaterialDataTest::phongAccessTexturedDefaults() {
    PhongMaterialData data{{}, {
        {MaterialAttribute::AmbientTexture, 42u},
        {MaterialAttribute::DiffuseTexture, 33u},
        {MaterialAttribute::SpecularTexture, 17u},
        {MaterialAttribute::NormalTexture, 1u}
    }};

    CORRADE_VERIFY(data.hasSpecularTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.ambientTexture(), 42);
    CORRADE_COMPARE(data.ambientTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 0);
    CORRADE_COMPARE(data.diffuseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.diffuseTexture(), 33);
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 0);
    CORRADE_COMPARE(data.specularColor(), 0xffffff00_rgbaf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.specularTextureCoordinates(), 0);
    CORRADE_COMPARE(data.normalTexture(), 1);
    CORRADE_COMPARE(data.normalTextureScale(), 1.0f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.normalTextureCoordinates(), 0);
}

void MaterialDataTest::phongAccessTexturedSingleMatrixCoordinates() {
    PhongMaterialData data{{}, {
        {MaterialAttribute::AmbientTexture, 42u},
        {MaterialAttribute::DiffuseTexture, 33u},
        {MaterialAttribute::SpecularTexture, 17u},
        {MaterialAttribute::NormalTexture, 0u},
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 1.0f})},
        {MaterialAttribute::TextureCoordinates, 2u}
    }};

    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientTextureMatrix(), Matrix3::translation({0.5f, 1.0f}));
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 2);
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3::translation({0.5f, 1.0f}));
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 2);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::translation({0.5f, 1.0f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 2);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::translation({0.5f, 1.0f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 2);
}

void MaterialDataTest::phongAccessTexturedImplicitPackedSpecularGlossiness() {
    PhongMaterialData data{{}, {
        {MaterialAttribute::SpecularColor, 0xacabadff_rgbaf},
        {MaterialAttribute::SpecularGlossinessTexture, 17u},
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({1.0f, 1.0f})},
        {MaterialAttribute::SpecularTextureCoordinates, 4u},
    }};

    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::TextureCoordinates|PhongMaterialData::Flag::TextureTransformation);
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
    CORRADE_VERIFY(data.hasSpecularTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({1.0f, 1.0f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 4);
}

void MaterialDataTest::phongAccessInvalidTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PhongMaterialData data{{}, {}};

    std::ostringstream out;
    Error redirectError{&out};
    data.ambientTexture();
    data.ambientTextureMatrix();
    data.ambientTextureCoordinates();
    data.diffuseTexture();
    data.diffuseTextureMatrix();
    data.diffuseTextureCoordinates();
    data.specularTexture();
    data.specularTextureSwizzle();
    data.specularTextureMatrix();
    data.specularTextureCoordinates();
    data.normalTexture();
    data.normalTextureScale();
    data.normalTextureSwizzle();
    data.normalTextureMatrix();
    data.normalTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attribute(): attribute AmbientTexture not found in layer 0\n"
        "Trade::PhongMaterialData::ambientTextureMatrix(): the material doesn't have an ambient texture\n"
        "Trade::PhongMaterialData::ambientTextureCoordinates(): the material doesn't have an ambient texture\n"
        "Trade::MaterialData::attribute(): attribute DiffuseTexture not found in layer 0\n"
        "Trade::PhongMaterialData::diffuseTextureMatrix(): the material doesn't have a diffuse texture\n"
        "Trade::PhongMaterialData::diffuseTextureCoordinates(): the material doesn't have a diffuse texture\n"
        "Trade::PhongMaterialData::specularTexture(): the material doesn't have a specular texture\n"
        "Trade::PhongMaterialData::specularTextureSwizzle(): the material doesn't have a specular texture\n"
        "Trade::PhongMaterialData::specularTextureMatrix(): the material doesn't have a specular texture\n"
        "Trade::PhongMaterialData::specularTextureCoordinates(): the material doesn't have a specular texture\n"
        "Trade::MaterialData::attribute(): attribute NormalTexture not found in layer 0\n"
        "Trade::PhongMaterialData::normalTextureScale(): the material doesn't have a normal texture\n"
        "Trade::PhongMaterialData::normalTextureSwizzle(): the material doesn't have a normal texture\n"
        "Trade::PhongMaterialData::normalTextureMatrix(): the material doesn't have a normal texture\n"
        "Trade::PhongMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture\n");
}

void MaterialDataTest::phongAccessCommonTransformationCoordinatesNoTextures() {
    PhongMaterialData a{{}, {}};
    CORRADE_VERIFY(a.hasCommonTextureTransformation());
    CORRADE_VERIFY(a.hasCommonTextureCoordinates());
    CORRADE_COMPARE(a.commonTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(a.commonTextureCoordinates(), 0);

    #ifdef MAGNUM_BUILD_DEPRECATED
    /* textureMatrix() should return the common matrix, if possible, and
       fall back to the global one if not */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(a.textureMatrix(), Matrix3{});
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    PhongMaterialData b{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u}
    }};
    CORRADE_VERIFY(b.hasCommonTextureTransformation());
    CORRADE_VERIFY(b.hasCommonTextureCoordinates());
    CORRADE_COMPARE(b.commonTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(b.commonTextureCoordinates(), 7);

    #ifdef MAGNUM_BUILD_DEPRECATED
    /* textureMatrix() should return the common matrix, if possible, and
       fall back to the global one if not */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(b.textureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
}

void MaterialDataTest::phongAccessCommonTransformationCoordinatesOneTexture() {
    Containers::StringView textureName = PhongTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PhongMaterialData data{{}, {
        {textureName, 5u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u},

        /* These shouldn't affect the above */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(data.hasCommonTextureTransformation());
    CORRADE_COMPARE(data.commonTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_COMPARE(data.commonTextureCoordinates(), 17u);

    #ifdef MAGNUM_BUILD_DEPRECATED
    /* textureMatrix() should return the common matrix, if possible, and
       fall back to the global one if not */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
}

void MaterialDataTest::phongAccessCommonTransformationCoordinatesOneDifferentTexture() {
    Containers::StringView textureName = PhongTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PhongMaterialData data{{}, {
        {MaterialAttribute::AmbientTexture, 2u},
        {MaterialAttribute::DiffuseTexture, 3u},
        {MaterialAttribute::SpecularTexture, 4u},
        {MaterialAttribute::NormalTexture, 5u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u},

        /* These are used by all textures except the one above, failing the
           check */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());

    #ifdef MAGNUM_BUILD_DEPRECATED
    /* textureMatrix() should return the common matrix, if possible, and
       fall back to the global one if not */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::translation({0.5f, 0.0f}));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
}

void MaterialDataTest::phongAccessNoCommonTransformationCoordinates() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PhongMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 3u},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 3u},
        {MaterialAttribute::SpecularTexture, 4u},
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::NormalTexture, 5u},
        {MaterialAttribute::NormalTextureCoordinates, 17u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());

    std::ostringstream out;
    Error redirectError{&out};
    data.commonTextureMatrix();
    data.commonTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::PhongMaterialData::commonTextureMatrix(): the material doesn't have a common texture coordinate transformation\n"
        "Trade::PhongMaterialData::commonTextureCoordinates(): the material doesn't have a common texture coordinate set\n");
}

void MaterialDataTest::flatAccessBaseColor() {
    MaterialData base{MaterialType::Flat, {
        {MaterialAttribute::BaseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseColor, 0x33556600_rgbaf}, /* Ignored */
    }};

    CORRADE_COMPARE(base.types(), MaterialType::Flat);
    const auto& data = base.as<FlatMaterialData>();

    CORRADE_VERIFY(!data.hasTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
}

void MaterialDataTest::flatAccessDiffuseColor() {
    MaterialData base{MaterialType::Flat, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
    }};

    CORRADE_COMPARE(base.types(), MaterialType::Flat);
    const auto& data = base.as<FlatMaterialData>();

    CORRADE_VERIFY(!data.hasTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
}

void MaterialDataTest::flatAccessDefaults() {
    MaterialData base{{}, {}};

    CORRADE_COMPARE(base.types(), MaterialTypes{});
    /* Casting is fine even if the type doesn't include Flat */
    const auto& data = base.as<FlatMaterialData>();

    CORRADE_VERIFY(!data.hasTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xffffff_rgbf);
}

void MaterialDataTest::flatAccessTexturedBaseColor() {
    FlatMaterialData data{{}, {
        {MaterialAttribute::BaseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::BaseColorTexture, 5u},
        {MaterialAttribute::BaseColorTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::BaseColorTextureCoordinates, 2u},

        /* All this is ignored */
        {MaterialAttribute::DiffuseColor, 0x33556600_rgbaf},
        {MaterialAttribute::DiffuseTexture, 6u},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::translation({0.5f, 1.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(data.hasTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.texture(), 5);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.textureCoordinates(), 2);
}

void MaterialDataTest::flatAccessTexturedDiffuseColor() {
    FlatMaterialData data{{}, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseTexture, 5u},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 2u},

        /* Ignored, as we have a diffuse texture */
        {MaterialAttribute::BaseColor, 0x33556600_rgbaf}
    }};

    CORRADE_VERIFY(data.hasTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.texture(), 5);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.textureCoordinates(), 2);
}

void MaterialDataTest::flatAccessTexturedDefaults() {
    FlatMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 5u}
    }};

    CORRADE_VERIFY(data.hasTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xffffff_rgbf);
    CORRADE_COMPARE(data.texture(), 5);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.textureCoordinates(), 0);
}

void MaterialDataTest::flatAccessTexturedBaseColorSingleMatrixCoordinates() {
    FlatMaterialData data{{}, {
        {MaterialAttribute::BaseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::BaseColorTexture, 5u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::TextureCoordinates, 2u},

        /* This is ignored because it doesn't match the texture */
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::translation({0.5f, 1.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(data.hasTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.texture(), 5);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.textureCoordinates(), 2);
}

void MaterialDataTest::flatAccessTexturedDiffuseColorSingleMatrixCoordinates() {
    FlatMaterialData data{{}, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseTexture, 5u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::TextureCoordinates, 2u},

        /* This is ignored because it doesn't match the texture */
        {MaterialAttribute::BaseColorTextureMatrix, Matrix3::translation({0.5f, 1.0f})},
        {MaterialAttribute::BaseColorTextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(data.hasTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.texture(), 5);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.textureCoordinates(), 2);
}

void MaterialDataTest::flatAccessTexturedMismatchedMatrixCoordinates() {
    {
        FlatMaterialData data{{}, {
            {MaterialAttribute::BaseColorTexture, 5u},

            /* This is ignored because it doesn't match the texture */
            {MaterialAttribute::DiffuseColor, 0x33556600_rgbaf},
            {MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::DiffuseTextureCoordinates, 2u},
        }};

        CORRADE_VERIFY(data.hasTexture());
        CORRADE_VERIFY(!data.hasTextureTransformation());
        CORRADE_VERIFY(!data.hasTextureCoordinates());
        CORRADE_COMPARE(data.color(), 0xffffff_rgbf);
        CORRADE_COMPARE(data.texture(), 5);
        CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.textureCoordinates(), 0);
    } {
        FlatMaterialData data{{}, {
            {MaterialAttribute::DiffuseTexture, 5u},

            /* This is ignored because it doesn't match the texture */
            {MaterialAttribute::BaseColor, 0x33556600_rgbaf},
            {MaterialAttribute::BaseColorTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::BaseColorTextureCoordinates, 2u},
        }};

        CORRADE_VERIFY(data.hasTexture());
        CORRADE_VERIFY(!data.hasTextureTransformation());
        CORRADE_VERIFY(!data.hasTextureCoordinates());
        CORRADE_COMPARE(data.color(), 0xffffff_rgbf);
        CORRADE_COMPARE(data.texture(), 5);
        CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.textureCoordinates(), 0);
    }
}

void MaterialDataTest::flatAccessInvalidTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    FlatMaterialData data{{}, {}};

    std::ostringstream out;
    Error redirectError{&out};
    data.texture();
    data.textureMatrix();
    data.textureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::FlatMaterialData::texture(): the material doesn't have a texture\n"
        "Trade::FlatMaterialData::textureMatrix(): the material doesn't have a texture\n"
        "Trade::FlatMaterialData::textureCoordinates(): the material doesn't have a texture\n");
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
    }, {1, 7}};

    CORRADE_COMPARE(data.layerName(), "ClearCoat");
    CORRADE_COMPARE(data.layerFactor(), 0.35f);
    CORRADE_COMPARE(data.layerFactorTexture(), 3u);
    CORRADE_COMPARE(data.layerFactorTextureSwizzle(), MaterialTextureSwizzle::B);
    CORRADE_COMPARE(data.layerFactorTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.layerFactorTextureCoordinates(), 4u);

    CORRADE_COMPARE(data.attributeCount(), 6);
    CORRADE_VERIFY(data.hasAttribute(MaterialAttribute::LayerFactor));
    CORRADE_VERIFY(data.hasAttribute("LayerFactorTexture"));
    CORRADE_VERIFY(!data.hasAttribute(MaterialAttribute::BaseColor));
    CORRADE_VERIFY(!data.hasAttribute("BaseColor"));
    CORRADE_VERIFY(data.hasAttribute(0, MaterialAttribute::BaseColor));
    CORRADE_VERIFY(data.hasAttribute(0, "BaseColor"));

    CORRADE_COMPARE(data.attributeId(MaterialAttribute::LayerFactorTexture), 2);
    CORRADE_COMPARE(data.attributeId("LayerFactorTexture"), 2);

    CORRADE_COMPARE(data.attributeName(2), "LayerFactorTexture");

    CORRADE_COMPARE(data.attributeType(2), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType(MaterialAttribute::LayerFactorTexture), MaterialAttributeType::UnsignedInt);
    CORRADE_COMPARE(data.attributeType("LayerFactorTexture"), MaterialAttributeType::UnsignedInt);

    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(2)), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute(MaterialAttribute::LayerFactorTexture)), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.attribute("LayerFactorTexture")), 3);

    CORRADE_COMPARE(data.attribute<UnsignedInt>(2), 3);
    CORRADE_COMPARE(data.attribute<UnsignedInt>(MaterialAttribute::LayerFactorTexture), 3);
    CORRADE_COMPARE(data.attribute<UnsignedInt>("LayerFactorTexture"), 3);

    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.tryAttribute(MaterialAttribute::LayerFactorTexture)), 3);
    CORRADE_COMPARE(*static_cast<const UnsignedInt*>(data.tryAttribute("LayerFactorTexture")), 3);

    CORRADE_COMPARE(data.tryAttribute<UnsignedInt>(MaterialAttribute::LayerFactorTexture), 3);
    CORRADE_COMPARE(data.tryAttribute<UnsignedInt>("LayerFactorTexture"), 3);

    CORRADE_COMPARE(data.attributeOr(MaterialAttribute::LayerFactorTexture, 5u), 3);
    CORRADE_COMPARE(data.attributeOr("LayerFactorTexture", 5u), 3);
}

void MaterialDataTest::pbrClearCoatAccess() {
    MaterialData base{MaterialType::PbrClearCoat, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::Roughness, 0.7f}
    }, {0, 2}};

    CORRADE_COMPARE(base.types(), MaterialType::PbrClearCoat);
    const auto& data = base.as<PbrClearCoatMaterialData>();

    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.roughness(), 0.7f);
}

void MaterialDataTest::pbrClearCoatAccessDefaults() {
    MaterialData base{{}, {
        /* Needs to have at least the layer name, otherwise the queries will
           blow up */
        {MaterialLayer::ClearCoat}
    }, {0, 1}};

    CORRADE_COMPARE(base.types(), MaterialTypes{});
    const auto& data = base.as<PbrClearCoatMaterialData>();

    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.layerFactor(), 1.0f);
    CORRADE_COMPARE(data.roughness(), 0.0f);
}

void MaterialDataTest::pbrClearCoatAccessTextured() {
    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::Roughness, 0.7f},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::A},
        {MaterialAttribute::RoughnessTextureMatrix, Matrix3::translation({2.0f, 1.5f})},
        {MaterialAttribute::RoughnessTextureCoordinates, 6u},
        {MaterialAttribute::NormalTexture, 3u},
        {MaterialAttribute::NormalTextureScale, 0.5f},
        {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::B},
        {MaterialAttribute::NormalTextureMatrix, Matrix3::translation({0.0f, 0.5f})},
        {MaterialAttribute::NormalTextureCoordinates, 7u},
    }, {0, 11}};

    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.roughness(), 0.7f);
    CORRADE_COMPARE(data.roughnessTexture(), 2u);
    CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::A);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::translation({2.0f, 1.5f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 6u);
    CORRADE_COMPARE(data.normalTexture(), 3u);
    CORRADE_COMPARE(data.normalTextureScale(), 0.5f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::B);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7u);
}

void MaterialDataTest::pbrClearCoatAccessTexturedDefaults() {
    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::NormalTexture, 3u},
    }, {0, 3}};

    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.roughness(), 0.0f);
    CORRADE_COMPARE(data.roughnessTexture(), 2u);
    CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 0u);
    CORRADE_COMPARE(data.normalTexture(), 3u);
    CORRADE_COMPARE(data.normalTextureScale(), 1.0f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.normalTextureCoordinates(), 0u);
}

void MaterialDataTest::pbrClearCoatAccessTexturedExplicitPackedLayerFactorRoughness() {
    /* Just the texture ID and swizzles, the rest is implicit */
    {
        PbrClearCoatMaterialData data{{}, {
            {MaterialLayer::ClearCoat},
            {MaterialAttribute::LayerFactorTexture, 2u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G}
        }, {0, 4}};
        CORRADE_VERIFY(data.hasLayerFactorRoughnessTexture());
        CORRADE_COMPARE(data.layerFactorTexture(), 2);
        CORRADE_COMPARE(data.layerFactorTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.layerFactorTextureCoordinates(), 0);
        CORRADE_COMPARE(data.roughnessTexture(), 2);
        CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::G);
        CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.roughnessTextureCoordinates(), 0);

    /* Explicit parameters for everything, but all the same */
    } {
        PbrClearCoatMaterialData data{{}, {
            {MaterialLayer::ClearCoat},
            {MaterialAttribute::LayerFactorTexture, 2u},
            {MaterialAttribute::LayerFactorTextureSwizzle, MaterialTextureSwizzle::R},
            {MaterialAttribute::LayerFactorTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::LayerFactorTextureCoordinates, 3u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u}
        }, {0, 9}};
        CORRADE_VERIFY(data.hasLayerFactorRoughnessTexture());
        CORRADE_COMPARE(data.layerFactorTexture(), 2);
        CORRADE_COMPARE(data.layerFactorTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.layerFactorTextureCoordinates(), 3);
        CORRADE_COMPARE(data.roughnessTexture(), 2);
        CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::G);
        CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.roughnessTextureCoordinates(), 3);

    /* Different texture ID */
    } {
        PbrClearCoatMaterialData data{{}, {
            {MaterialLayer::ClearCoat},
            {MaterialAttribute::LayerFactorTexture, 2u},
            {MaterialAttribute::RoughnessTexture, 3u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
        }, {0, 4}};
        CORRADE_VERIFY(!data.hasLayerFactorRoughnessTexture());

    /* Unexpected swizzle 1 */
    } {
        PbrClearCoatMaterialData data{{}, {
            {MaterialLayer::ClearCoat},
            {MaterialAttribute::LayerFactorTexture, 2u},
            {MaterialAttribute::LayerFactorTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
        }, {0, 5}};
        CORRADE_VERIFY(!data.hasLayerFactorRoughnessTexture());

    /* Unexpected swizzle 2 */
    } {
        PbrClearCoatMaterialData data{{}, {
            {MaterialLayer::ClearCoat},
            {MaterialAttribute::LayerFactorTexture, 2u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B}
        }, {0, 4}};
        CORRADE_VERIFY(!data.hasLayerFactorRoughnessTexture());

    /* Unexpected texture matrix */
    } {
        PbrClearCoatMaterialData data{{}, {
            {MaterialLayer::ClearCoat},
            {MaterialAttribute::LayerFactorTexture, 2u},
            {MaterialAttribute::LayerFactorTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G}
        }, {0, 5}};
        CORRADE_VERIFY(!data.hasLayerFactorRoughnessTexture());

    /* Unexpected texture coordinates */
    } {
        PbrClearCoatMaterialData data{{}, {
            {MaterialLayer::ClearCoat},
            {MaterialAttribute::LayerFactorTexture, 2u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureCoordinates, 1u}
        }, {0, 5}};
        CORRADE_VERIFY(!data.hasLayerFactorRoughnessTexture());
    }
}

void MaterialDataTest::pbrClearCoatAccessTexturedSingleMatrixCoordinates() {
    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::NormalTexture, 3u},
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.0f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},
    }, {0, 5}};

    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 7u);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7u);
}

void MaterialDataTest::pbrClearCoatAccessTexturedBaseMaterialMatrixCoordinates() {
    PbrClearCoatMaterialData data{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.0f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},

        {MaterialLayer::ClearCoat},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::NormalTexture, 3u},
    }, {2, 5}};

    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 7u);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7u);

    CORRADE_VERIFY(data.hasCommonTextureTransformation());
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_COMPARE(data.commonTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.commonTextureCoordinates(), 7);
}

void MaterialDataTest::pbrClearCoatAccessInvalidTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
    }, {0, 1}};

    std::ostringstream out;
    Error redirectError{&out};
    data.roughnessTexture();
    data.roughnessTextureSwizzle();
    data.roughnessTextureMatrix();
    data.roughnessTextureCoordinates();
    data.normalTexture();
    data.normalTextureScale();
    data.normalTextureSwizzle();
    data.normalTextureMatrix();
    data.normalTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attribute(): attribute RoughnessTexture not found in layer ClearCoat\n"
        "Trade::PbrClearCoatMaterialData::roughnessTextureSwizzle(): the layer doesn't have a roughness texture\n"
        "Trade::PbrClearCoatMaterialData::roughnessTextureMatrix(): the layer doesn't have a roughness texture\n"
        "Trade::PbrClearCoatMaterialData::roughnessTextureCoordinates(): the layer doesn't have a roughness texture\n"
        "Trade::MaterialData::attribute(): attribute NormalTexture not found in layer ClearCoat\n"
        "Trade::PbrClearCoatMaterialData::normalTextureScale(): the layer doesn't have a normal texture\n"
        "Trade::PbrClearCoatMaterialData::normalTextureSwizzle(): the layer doesn't have a normal texture\n"
        "Trade::PbrClearCoatMaterialData::normalTextureMatrix(): the layer doesn't have a normal texture\n"
        "Trade::PbrClearCoatMaterialData::normalTextureCoordinates(): the layer doesn't have a normal texture\n");
}

void MaterialDataTest::pbrClearCoatAccessCommonTransformationCoordinatesNoTextures() {
    PbrClearCoatMaterialData a{{}, {
        {MaterialLayer::ClearCoat},
    }, {0, 1}};
    CORRADE_VERIFY(a.hasCommonTextureTransformation());
    CORRADE_VERIFY(a.hasCommonTextureCoordinates());
    CORRADE_COMPARE(a.commonTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(a.commonTextureCoordinates(), 0);

    PbrClearCoatMaterialData b{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},

        {MaterialLayer::ClearCoat}
    }, {2, 3}};
    CORRADE_VERIFY(b.hasCommonTextureTransformation());
    CORRADE_VERIFY(b.hasCommonTextureCoordinates());
    CORRADE_COMPARE(b.commonTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(b.commonTextureCoordinates(), 7);

    PbrClearCoatMaterialData c{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},
    }, {0, 3}};
    CORRADE_VERIFY(c.hasCommonTextureTransformation());
    CORRADE_VERIFY(c.hasCommonTextureCoordinates());
    CORRADE_COMPARE(c.commonTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(c.commonTextureCoordinates(), 7);
}

void MaterialDataTest::pbrClearCoatAccessCommonTransformationCoordinatesOneTexture() {
    Containers::StringView textureName = PbrClearCoatTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrClearCoatMaterialData data{{}, {
        /* These shouldn't affect the below */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u},

        {MaterialLayer::ClearCoat},
        {textureName, 5u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u},
    }, {2, 6}};

    CORRADE_VERIFY(data.hasCommonTextureTransformation());
    CORRADE_COMPARE(data.commonTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_COMPARE(data.commonTextureCoordinates(), 17u);
}

void MaterialDataTest::pbrClearCoatAccessCommonTransformationCoordinatesOneDifferentTexture() {
    Containers::StringView textureName = PbrClearCoatTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrClearCoatMaterialData data{{}, {
        /* These are used by all textures except the one below, failing the
           check */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u},

        {MaterialLayer::ClearCoat},
        {MaterialAttribute::LayerFactorTexture, 2u},
        {MaterialAttribute::RoughnessTexture, 3u},
        {MaterialAttribute::NormalTexture, 5u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u}
    }, {2, 8}};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());
}

void MaterialDataTest::pbrClearCoatAccessNoCommonTransformationCoordinates() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::LayerFactorTexture, 3u},
        {MaterialAttribute::LayerFactorTextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::LayerFactorTextureCoordinates, 3u},
        {MaterialAttribute::RoughnessTexture, 4u},
        {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::NormalTexture, 5u},
        {MaterialAttribute::NormalTextureCoordinates, 17u}
    }, {0, 8}};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());

    std::ostringstream out;
    Error redirectError{&out};
    data.commonTextureMatrix();
    data.commonTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::PbrClearCoatMaterialData::commonTextureMatrix(): the layer doesn't have a common texture coordinate transformation\n"
        "Trade::PbrClearCoatMaterialData::commonTextureCoordinates(): the layer doesn't have a common texture coordinate set\n");
}

void MaterialDataTest::debugLayer() {
    std::ostringstream out;

    Debug{&out} << MaterialLayer::ClearCoat << MaterialLayer(0xfefe) << MaterialLayer{};
    CORRADE_COMPARE(out.str(), "Trade::MaterialLayer::ClearCoat Trade::MaterialLayer(0xfefe) Trade::MaterialLayer(0x0)\n");
}

void MaterialDataTest::debugAttribute() {
    std::ostringstream out;

    Debug{&out} << MaterialAttribute::DiffuseTextureCoordinates << MaterialAttribute::LayerName << MaterialAttribute(0xfefe) << MaterialAttribute{};
    CORRADE_COMPARE(out.str(), "Trade::MaterialAttribute::DiffuseTextureCoordinates Trade::MaterialAttribute::LayerName Trade::MaterialAttribute(0xfefe) Trade::MaterialAttribute(0x0)\n");
}

void MaterialDataTest::debugTextureSwizzle() {
    std::ostringstream out;

    /* The swizzle is encoded as a fourCC, so it just prints the numerical
       value as a char. Worst case this will print nothing or four garbage
       letters. Sorry in that case. */
    Debug{&out} << MaterialTextureSwizzle::BA << MaterialTextureSwizzle{};
    CORRADE_COMPARE(out.str(), "Trade::MaterialTextureSwizzle::BA Trade::MaterialTextureSwizzle::\n");
}

void MaterialDataTest::debugAttributeType() {
    std::ostringstream out;

    Debug{&out} << MaterialAttributeType::Matrix3x2 << MaterialAttributeType(0xfe);
    CORRADE_COMPARE(out.str(), "Trade::MaterialAttributeType::Matrix3x2 Trade::MaterialAttributeType(0xfe)\n");
}

void MaterialDataTest::debugType() {
    std::ostringstream out;

    Debug(&out) << MaterialType::Phong << MaterialType(0xbe);
    CORRADE_COMPARE(out.str(), "Trade::MaterialType::Phong Trade::MaterialType(0xbe)\n");
}

void MaterialDataTest::debugTypes() {
    std::ostringstream out;

    Debug{&out} << (MaterialType::Phong|MaterialType(0xe0)) << MaterialTypes{};
    CORRADE_COMPARE(out.str(), "Trade::MaterialType::Phong|Trade::MaterialType(0xe0) Trade::MaterialTypes{}\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void MaterialDataTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << MaterialData::Flag::DoubleSided << MaterialData::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::MaterialData::Flag::DoubleSided Trade::MaterialData::Flag(0xf0)\n");
}

void MaterialDataTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << MaterialData::Flag::DoubleSided << MaterialData::Flags{};
    CORRADE_COMPARE(out.str(), "Trade::MaterialData::Flag::DoubleSided Trade::MaterialData::Flags{}\n");
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void MaterialDataTest::debugAlphaMode() {
    std::ostringstream out;

    Debug{&out} << MaterialAlphaMode::Opaque << MaterialAlphaMode(0xee);
    CORRADE_COMPARE(out.str(), "Trade::MaterialAlphaMode::Opaque Trade::MaterialAlphaMode(0xee)\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void MaterialDataTest::debugPhongFlag() {
    std::ostringstream out;

    Debug{&out} << PhongMaterialData::Flag::AmbientTexture << PhongMaterialData::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::PhongMaterialData::Flag::AmbientTexture Trade::PhongMaterialData::Flag(0xf0)\n");
}

void MaterialDataTest::debugPhongFlags() {
    std::ostringstream out;

    Debug{&out} << (PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture) << PhongMaterialData::Flags{};
    CORRADE_COMPARE(out.str(), "Trade::PhongMaterialData::Flag::DiffuseTexture|Trade::PhongMaterialData::Flag::SpecularTexture Trade::PhongMaterialData::Flags{}\n");
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::MaterialDataTest)
