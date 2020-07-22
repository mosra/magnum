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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Trade/PhongMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class MaterialDataTest: public TestSuite::Tester {
    public:
        explicit MaterialDataTest();

        void attributeTypeSize();
        void attributeTypeSizeInvalid();

        void attributeMap();

        void constructAttributeDefault();
        void constructAttributeString();
        void constructAttributeName();
        void constructAttributeTypeErasedString();
        void constructAttributeTypeErasedName();

        template<class T> void constructAttributeStringConstexpr();

        void constructAttributeInvalidName();
        void constructAttributeWrongTypeForName();
        void constructAttributeInvalidType();
        void constructAttributeTooLarge();
        void constructAttributeWrongAccessType();

        void construct();
        void constructEmptyAttribute();
        void constructDuplicateAttribute();
        void constructFromImmutableSortedArray();

        void constructNonOwned();
        void constructNonOwnedEmptyAttribute();
        void constructNonOwnedNotSorted();
        void constructNonOwnedDuplicateAttribute();

        void constructCopy();
        void constructMove();

        void access();
        void accessOptional();
        void accessOutOfBounds();
        void accessInvalidAttributeName();
        void accessNotFound();
        void accessWrongType();

        void release();

        #ifdef MAGNUM_BUILD_DEPRECATED
        void constructPhongDeprecated();
        void constructPhongDeprecatedTextured();
        void constructPhongDeprecatedTexturedTextureTransform();
        void constructPhongDeprecatedTexturedCoordinates();
        void constructPhongDeprecatedTextureTransformNoTextures();
        void constructPhongDeprecatedNoTextureTransformationFlag();
        void constructPhongDeprecatedNoTextureCoordinatesFlag();
        #endif

        void phongAccess();
        void phongAccessDefaults();
        void phongAccessTextured();
        void phongAccessTexturedDefaults();
        void phongAccessTexturedSingleMatrixCoordinates();
        void phongAccessInvalidTextures();

        void debugAttribute();
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

MaterialDataTest::MaterialDataTest() {
    addTests({&MaterialDataTest::attributeTypeSize,
              &MaterialDataTest::attributeTypeSizeInvalid,
              &MaterialDataTest::attributeMap,

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

              &MaterialDataTest::constructAttributeInvalidName,
              &MaterialDataTest::constructAttributeWrongTypeForName,
              &MaterialDataTest::constructAttributeInvalidType,
              &MaterialDataTest::constructAttributeTooLarge,
              &MaterialDataTest::constructAttributeWrongAccessType,

              &MaterialDataTest::construct,
              &MaterialDataTest::constructEmptyAttribute});

    addRepeatedTests({&MaterialDataTest::constructDuplicateAttribute},
        5*4*3*2);

    addTests({&MaterialDataTest::constructFromImmutableSortedArray,

              &MaterialDataTest::constructNonOwned,
              &MaterialDataTest::constructNonOwnedEmptyAttribute,
              &MaterialDataTest::constructNonOwnedNotSorted,
              &MaterialDataTest::constructNonOwnedDuplicateAttribute,

              &MaterialDataTest::constructCopy,
              &MaterialDataTest::constructMove,

              &MaterialDataTest::access,
              &MaterialDataTest::accessOptional,
              &MaterialDataTest::accessOutOfBounds,
              &MaterialDataTest::accessInvalidAttributeName,
              &MaterialDataTest::accessNotFound,
              &MaterialDataTest::accessWrongType,

              &MaterialDataTest::release,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &MaterialDataTest::constructPhongDeprecated,
              &MaterialDataTest::constructPhongDeprecatedTextured,
              &MaterialDataTest::constructPhongDeprecatedTexturedTextureTransform,
              &MaterialDataTest::constructPhongDeprecatedTexturedCoordinates,
              &MaterialDataTest::constructPhongDeprecatedTextureTransformNoTextures,
              &MaterialDataTest::constructPhongDeprecatedNoTextureTransformationFlag,
              &MaterialDataTest::constructPhongDeprecatedNoTextureCoordinatesFlag,
              #endif

              &MaterialDataTest::phongAccess,
              &MaterialDataTest::phongAccessDefaults,
              &MaterialDataTest::phongAccessTextured,
              &MaterialDataTest::phongAccessTexturedDefaults,
              &MaterialDataTest::phongAccessTexturedSingleMatrixCoordinates,
              &MaterialDataTest::phongAccessInvalidTextures,

              &MaterialDataTest::debugAttribute,
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
    CORRADE_COMPARE(out.str(),
        "Trade::materialAttributeTypeSize(): invalid type Trade::MaterialAttributeType(0x0)\n"
        "Trade::materialAttributeTypeSize(): invalid type Trade::MaterialAttributeType(0xfe)\n");
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
            #include "Magnum/Trade/Implementation/materialAttributeProperties.hpp"
            #undef _c
            #undef _ct
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
    constexpr MaterialAttributeData attribute{"templateAttrib"_s, T(15)};
    CORRADE_COMPARE(attribute.name(), "templateAttrib");
    CORRADE_COMPARE(attribute.name().flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(attribute.name()[attribute.name().size()], '\0');
    CORRADE_COMPARE(attribute.value<T>(), T(15));
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
       the same output. */
    /*constexpr*/ MaterialAttributeData{"attributeIsLong"_s, Matrix3x4{}};
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialAttributeData: name attributeIsLong too long, expected at most 14 bytes for Trade::MaterialAttributeType::Matrix3x4 but got 15\n"
        "Trade::MaterialAttributeData: name attributeIsLong too long, expected at most 14 bytes for Trade::MaterialAttributeType::Matrix3x4 but got 15\n");
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

void MaterialDataTest::construct() {
    int state;
    MaterialData data{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {MaterialAttribute::DiffuseTextureCoordinates, 5u},
        {"highlightColor", 0x335566ff_rgbaf},
        {MaterialAttribute::AmbientTextureMatrix, Matrix3::scaling({0.5f, 1.0f})}
    }, &state};

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_COMPARE(data.attributeCount(), 4);
    CORRADE_COMPARE(data.data().size(), 4);
    CORRADE_COMPARE(data.importerState(), &state);

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
    CORRADE_COMPARE(data.attributeCount(), 4);
    CORRADE_COMPARE(data.data().size(), 4);
    CORRADE_COMPARE(data.data().data(), attributes);
    CORRADE_COMPARE(data.importerState(), &state);

    /* We sorted the input already */
    CORRADE_COMPARE(data.attributeName(0), "AmbientTextureMatrix");
    CORRADE_COMPARE(data.attributeName(1), "DiffuseTextureCoordinates");
    CORRADE_COMPARE(data.attributeName(2), "DoubleSided");
    CORRADE_COMPARE(data.attributeName(3), "highlightColor");

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

void MaterialDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<MaterialData, const MaterialData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<MaterialData, const MaterialData&>{}));
}

void MaterialDataTest::constructMove() {
    int state;
    MaterialData a{MaterialType::Phong, {
        {MaterialAttribute::DoubleSided, true},
        {"boredomFactor", 5}
    }, &state};

    MaterialData b{std::move(a)};
    CORRADE_COMPARE(a.attributeCount(), 0);
    CORRADE_COMPARE(b.types(), MaterialType::Phong);
    CORRADE_COMPARE(b.attributeCount(), 2);
    CORRADE_COMPARE(b.attributeName(0), "DoubleSided");
    CORRADE_COMPARE(b.importerState(), &state);

    MaterialData c{MaterialTypes{}, {
        {MaterialAttribute::AlphaMask, 0.5f}
    }};
    c = std::move(b);
    CORRADE_COMPARE(b.attributeCount(), 1);
    CORRADE_COMPARE(c.types(), MaterialType::Phong);
    CORRADE_COMPARE(c.attributeCount(), 2);
    CORRADE_COMPARE(c.attributeName(0), "DoubleSided");
    CORRADE_COMPARE(c.importerState(), &state);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<MaterialData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<MaterialData>::value);
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
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attributeName(): index 2 out of range for 2 attributes\n"
        "Trade::MaterialData::attributeType(): index 2 out of range for 2 attributes\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes\n"
        "Trade::MaterialData::attribute(): index 2 out of range for 2 attributes\n");
}

void MaterialDataTest::accessInvalidAttributeName() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MaterialData data{{}, {}};

    std::ostringstream out;
    Error redirectError{&out};
    data.hasAttribute(MaterialAttribute(0x0));
    data.hasAttribute(MaterialAttribute(0xfefe));
    data.attributeId(MaterialAttribute(0x0));
    data.attributeId(MaterialAttribute(0xfefe));
    data.attributeType(MaterialAttribute(0x0));
    data.attributeType(MaterialAttribute(0xfefe));
    data.attribute(MaterialAttribute(0x0));
    data.attribute(MaterialAttribute(0xfefe));
    data.attribute<Int>(MaterialAttribute(0x0));
    data.attribute<Int>(MaterialAttribute(0xfefe));
    data.tryAttribute(MaterialAttribute(0x0));
    data.tryAttribute(MaterialAttribute(0xfefe));
    data.tryAttribute<Int>(MaterialAttribute(0x0));
    data.tryAttribute<Int>(MaterialAttribute(0xfefe));
    data.attributeOr(MaterialAttribute(0x0), 42);
    data.attributeOr(MaterialAttribute(0xfefe), 42);
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
        "Trade::MaterialData::attributeId(): attribute DiffuseColour not found\n"
        "Trade::MaterialData::attributeType(): attribute DiffuseColour not found\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found\n"
        "Trade::MaterialData::attribute(): attribute DiffuseColour not found\n");
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

void MaterialDataTest::release() {
    MaterialData data{{}, {
        {"DiffuseColor", 0xff3366aa_rgbaf},
        {MaterialAttribute::NormalTexture, 0u}
    }};

    const void* pointer = data.data().data();

    Containers::Array<MaterialAttributeData> released = data.release();
    CORRADE_COMPARE(released.data(), pointer);
    CORRADE_COMPARE(released.size(), 2);
    CORRADE_VERIFY(!data.data());
    CORRADE_COMPARE(data.attributeCount(), 0);
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
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
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
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
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
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::rotation(90.0_degf));
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
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
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

void MaterialDataTest::phongAccess() {
    MaterialData base{MaterialType::Phong, {
        {MaterialAttribute::AmbientColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseColor, 0xebefbfff_rgbaf},
        {MaterialAttribute::SpecularColor, 0xacabadff_rgbaf},
        {MaterialAttribute::Shininess, 96.0f}
    }};

    CORRADE_COMPARE(base.types(), MaterialType::Phong);
    const PhongMaterialData& data = static_cast<const PhongMaterialData&>(base);

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
    const PhongMaterialData& data = static_cast<const PhongMaterialData&>(base);

    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientColor(), 0x000000_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xffffff_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xffffff_rgbf);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.textureCoordinates(), 0);
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
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({1.0f, 1.0f})},
        {MaterialAttribute::SpecularTextureCoordinates, 4u},
        {MaterialAttribute::NormalTexture, 0u},
        {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({1.0f, 0.5f})},
        {MaterialAttribute::NormalTextureCoordinates, 5u}
    }};

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
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({1.0f, 1.0f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 4);
    CORRADE_COMPARE(data.normalTexture(), 0);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({1.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 5);

    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.textureCoordinates(), 0);
}

void MaterialDataTest::phongAccessTexturedDefaults() {
    PhongMaterialData data{{}, {
        {MaterialAttribute::AmbientTexture, 42u},
        {MaterialAttribute::DiffuseTexture, 33u},
        {MaterialAttribute::SpecularTexture, 17u},
        {MaterialAttribute::NormalTexture, 1u}
    }};

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
    CORRADE_COMPARE(data.specularColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.specularTextureCoordinates(), 0);
    CORRADE_COMPARE(data.normalTexture(), 1);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.normalTextureCoordinates(), 0);

    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.textureCoordinates(), 0);
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

    CORRADE_COMPARE(data.textureMatrix(), Matrix3::translation({0.5f, 1.0f}));
    CORRADE_COMPARE(data.textureCoordinates(), 2);
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
    data.specularTextureMatrix();
    data.specularTextureCoordinates();
    data.normalTexture();
    data.normalTextureMatrix();
    data.normalTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attribute(): attribute AmbientTexture not found\n"
        "Trade::PhongMaterialData::ambientTextureMatrix(): the material doesn't have an ambient texture\n"
        "Trade::PhongMaterialData::ambientTextureCoordinates(): the material doesn't have an ambient texture\n"
        "Trade::MaterialData::attribute(): attribute DiffuseTexture not found\n"
        "Trade::PhongMaterialData::diffuseTextureMatrix(): the material doesn't have a diffuse texture\n"
        "Trade::PhongMaterialData::diffuseTextureCoordinates(): the material doesn't have a diffuse texture\n"
        "Trade::MaterialData::attribute(): attribute SpecularTexture not found\n"
        "Trade::PhongMaterialData::specularTextureMatrix(): the material doesn't have a specular texture\n"
        "Trade::PhongMaterialData::specularTextureCoordinates(): the material doesn't have a specular texture\n"
        "Trade::MaterialData::attribute(): attribute NormalTexture not found\n"
        "Trade::PhongMaterialData::normalTextureMatrix(): the material doesn't have a normal texture\n"
        "Trade::PhongMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture\n");
}

void MaterialDataTest::debugAttribute() {
    std::ostringstream out;

    Debug{&out} << MaterialAttribute::DiffuseTextureCoordinates << MaterialAttribute(0xfefe) << MaterialAttribute{};
    CORRADE_COMPARE(out.str(), "Trade::MaterialAttribute::DiffuseTextureCoordinates Trade::MaterialAttribute(0xfefe) Trade::MaterialAttribute(0x0)\n");
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
