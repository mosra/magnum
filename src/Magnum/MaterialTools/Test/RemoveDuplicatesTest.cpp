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

#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/DebugTools/CompareMaterial.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/MaterialTools/RemoveDuplicates.h"
#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools { namespace Test { namespace {

struct RemoveDuplicatesTest: TestSuite::Tester {
    explicit RemoveDuplicatesTest();

    void empty();

    void emptyMaterial();
    void emptyMaterialLayers();

    void differentAttributeName();
    void differentAttributeType();
    void differentAttributeValue();
    void differentAttributeValueFuzzy();
    void extraAttributes();

    void implicitBaseLayerSize();
    void multipleLayersSameContents();
    void multipleLayersDifferentContents();

    /* All cases above test the removeDuplicatesInto() variant, these the
       remaining 3 */
    void asArray();
    void inPlace();
    void inPlaceAsArray();

    void invalidSize();
};

using namespace Math::Literals;

constexpr Int A = 3, B = 4;
constexpr const Int *PointerA = &A, *PointerB = &B;

const struct {
    const char* name;
    Trade::MaterialAttributeData attribute, different;
} DifferentAttributeValueData[]{
    {"bool",
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::AlphaBlend, false}},
    {"scalar",
        {Trade::MaterialAttribute::BaseColorTexture, 2u},
        {Trade::MaterialAttribute::BaseColorTexture, 1u}},
    {"vector",
        {"objectIds", Vector3ui{3, 7, 9}},
        {"objectIds", Vector3ui{3, 6, 9}}},
    /* Matrices are only floating-point, tested in
       DifferentAttributeValueFuzzyData instead */
    {"pointer",
        /* Takes a pointer to a pointer, not the pointer itself */
        {"objectPtr", &PointerA},
        {"objectPtr", &PointerB}},
    {"mutable pointer",
        /* The pointed-to-locations aren't actually mutable, but as the value
           isn't used anywhere it should be okay */
        {"objectPtr", Trade::MaterialAttributeType::MutablePointer, &PointerA},
        {"objectPtr", Trade::MaterialAttributeType::MutablePointer, &PointerB}},
    {"string",
        {"name", "hellO"},
        {"name", "hell0"}},
    {"buffer",
        {"data", Containers::ArrayView<const void>{"\x0a\x0b\x0c"}},
        {"data", Containers::ArrayView<const void>{"\x0a\x0c\x0b"}}},
};

const struct {
    const char* name;
    Trade::MaterialAttributeData attribute, same, different;
} DifferentAttributeValueFuzzyData[]{
    {"scalar",
        {Trade::MaterialAttribute::Roughness, 0.7f},
        {Trade::MaterialAttribute::Roughness, 0.7f + Math::TypeTraits<Float>::epsilon()*0.5f},
        {Trade::MaterialAttribute::Roughness, 0.7f + Math::TypeTraits<Float>::epsilon()*2.0f}},
    {"vector",
        {Trade::MaterialAttribute::BaseColor, Vector4{0.5f, 0.9f, 0.7f, 0.9f}},
        {Trade::MaterialAttribute::BaseColor, Vector4{0.5f, 0.9f, 0.7f + Math::TypeTraits<Float>::epsilon()*0.5f, 0.9f}},
        {Trade::MaterialAttribute::BaseColor, Vector4{0.5f, 0.9f, 0.7f + Math::TypeTraits<Float>::epsilon()*2.0f, 0.9f}}},
    {"matrix",
        {Trade::MaterialAttribute::TextureMatrix,
            Matrix3::translation({5.0f, 9.0f})},
        {Trade::MaterialAttribute::TextureMatrix,
            Matrix3::translation({5.0f, 9.0f + Math::TypeTraits<Float>::epsilon()*5.0f})},
        {Trade::MaterialAttribute::TextureMatrix,
            Matrix3::translation({5.0f, 9.0f + Math::TypeTraits<Float>::epsilon()*20.0f})}},
};

RemoveDuplicatesTest::RemoveDuplicatesTest() {
    addTests({&RemoveDuplicatesTest::empty,

              &RemoveDuplicatesTest::emptyMaterial,
              &RemoveDuplicatesTest::emptyMaterialLayers,

              &RemoveDuplicatesTest::differentAttributeName,
              &RemoveDuplicatesTest::differentAttributeType});

    addInstancedTests({&RemoveDuplicatesTest::differentAttributeValue},
        Containers::arraySize(DifferentAttributeValueData));

    addInstancedTests({&RemoveDuplicatesTest::differentAttributeValueFuzzy},
        Containers::arraySize(DifferentAttributeValueFuzzyData));

    addTests({&RemoveDuplicatesTest::extraAttributes,

              &RemoveDuplicatesTest::implicitBaseLayerSize,
              &RemoveDuplicatesTest::multipleLayersSameContents,
              &RemoveDuplicatesTest::multipleLayersDifferentContents,

              &RemoveDuplicatesTest::asArray,
              &RemoveDuplicatesTest::inPlace,
              &RemoveDuplicatesTest::inPlaceAsArray,

              &RemoveDuplicatesTest::invalidSize});
}

void RemoveDuplicatesTest::empty() {
    CORRADE_COMPARE(removeDuplicatesInPlaceInto({}, {}), 0);
}

void RemoveDuplicatesTest::emptyMaterial() {
    const Trade::MaterialData materials[]{
        Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {}},
        Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {}},
        Trade::MaterialData{{}, {}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {}},
        /* This one has an importer state compared to the first. It's ignored
           so it should also be treated as the same. */
        Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {}, &A},
    };

    UnsignedInt mapping[6];
    CORRADE_COMPARE(removeDuplicatesInto(materials, mapping), 3);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 1u, 0u, 3u, 1u, 0u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::emptyMaterialLayers() {
    const Trade::MaterialData materials[]{
        Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {}, {0, 0}},
        Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {}, {0}},
        Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {}, {0, 0}},
        /* This one has the same prefix as the first but different count */
        Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {}, {0, 0, 0}},
        /* This one is the same as second but has different type so it
           shouldn't match */
        Trade::MaterialData{{}, {}, {0}},
        /* This one is the same as the second, it just has the base layer size
           implicit */
        Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {}},
    };

    UnsignedInt mapping[6];
    CORRADE_COMPARE(removeDuplicatesInto(materials, mapping), 4);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 1u, 0u, 3u, 4u, 1u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::differentAttributeName() {
    const Trade::MaterialData materials[]{
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        /* This one has the same attribute value and type at the same position
           but different attribute name, should be treated as different. Both
           instances of it are the same tho, so they should be treated as
           same. */
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        /* This one should be treated as equivalent to the first */
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        /* This one has everything the same as the first but has a different
           type, should be treated different also */
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
    };

    UnsignedInt mapping[5];
    CORRADE_COMPARE(removeDuplicatesInto(materials, mapping), 3);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 1u, 1u, 0u, 4u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::differentAttributeType() {
    const Trade::MaterialData materials[]{
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.0f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        /* This one has the same attribute name and bit-exact value at the same
           position but different attribute name, should be treated as
           different */
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::MetalnessTexture, 0u},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        /* This one should be treated as equivalent to the first */
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.0f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        /* This one has everything the same as the third first but has a
           different type for the last attribute, should be treated different
           also */
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.0f},
            /* Different type allowed only with a string name, not with enum */
            {"SpecularColor", "brown"},
        }},
    };

    UnsignedInt mapping[4];
    CORRADE_COMPARE(removeDuplicatesInto(materials, mapping), 3);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 1u, 0u, 3u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::differentAttributeValue() {
    auto&& data = DifferentAttributeValueData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const Trade::MaterialData materials[]{
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::BaseColorTextureCoordinates, 3u},
            {Trade::MaterialAttribute::Glossiness, 3.7f},
            data.attribute
        }},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::BaseColorTextureCoordinates, 3u},
            {Trade::MaterialAttribute::Glossiness, 3.7f},
            data.different
        }},
        /* It's sorted on construction, so this should compare equal */
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::BaseColorTextureCoordinates, 3u},
            data.attribute,
            {Trade::MaterialAttribute::Glossiness, 3.7f},
        }},
    };

    UnsignedInt mapping[3];
    CORRADE_COMPARE(removeDuplicatesInto(materials, mapping), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 1u, 0u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::differentAttributeValueFuzzy() {
    auto&& data = DifferentAttributeValueFuzzyData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const Trade::MaterialData materials[]{
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            data.attribute
        }},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            data.different
        }},
        /* Not bit-exact but should be treated as the same */
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            data.same
        }},
    };

    UnsignedInt mapping[3];
    CORRADE_COMPARE(removeDuplicatesInto(materials, mapping), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 1u, 0u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::extraAttributes() {
    const Trade::MaterialData materials[]{
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        /* This one has the same attribute prefix as the first but one more
           attribute after, should be treated as different */
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
            {Trade::MaterialAttribute::TextureLayer, 0u},
        }},
        /* This one has the same attribute prefix as the second, but one
           attribute less, should be treated as different */
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Roughness, 0.3f}
        }},
        /* This one is the same again, just with (ignored) importer state */
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }, {}, &B},
    };

    UnsignedInt mapping[4];
    CORRADE_COMPARE(removeDuplicatesInto(materials, mapping), 3);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 1u, 2u, 0u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::implicitBaseLayerSize() {
    const Trade::MaterialData materials[]{
        /* Implicit layer size after explicit, should be treated the same. Not
           the one at the end though, which has a different attribute value. */
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.0f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }, {3}},
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.0f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        /* Explicit layer size after implicit, should be treated the same. Not
           the one in the middle though, which has a different attribute
           value. */
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
        }},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::TextureCoordinates, 4u},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
        }, {2}},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
        }, {2}},
    };

    UnsignedInt mapping[6];
    CORRADE_COMPARE(removeDuplicatesInto(materials, mapping), 4);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 0u, 2u, 3u, 4u, 3u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::multipleLayersSameContents() {
    const Trade::MaterialAttributeData attributes[]{
        {Trade::MaterialAttribute::AlphaBlend, false},
        {Trade::MaterialAttribute::AlphaMask, 0.7f},
        {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
        {Trade::MaterialAttribute::Metalness, 0.3f},
        {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        {Trade::MaterialAttribute::TextureCoordinates, 3u},
    };
    const UnsignedInt layers[]{3, 6};

    const Trade::MaterialData materials[]{
        /* The attributes are deliberately ordered alphabetically to ensure
           they retain the same order even if different layers */
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AlphaBlend, false},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
        }, {2, 6}},
        /* The first layer has 3 elements instead of 2, should be different */
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AlphaBlend, false},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
        }, {3, 6}},
        /* There's an empty base layer before, should be different */
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AlphaBlend, false},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
        }, {0, 2, 6}},
        /* There's an empty layer at the end, should be different */
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AlphaBlend, false},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
        }, {2, 6, 6}},
        /* Same as the second, just with externally owned data */
        Trade::MaterialData{{}, {}, attributes, {}, layers},
        /* Everything in one layer, should be different */
        Trade::MaterialData{{}, {}, attributes},
    };

    UnsignedInt mapping[6];
    CORRADE_COMPARE(removeDuplicatesInto(materials, mapping), 5);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 1u, 2u, 3u, 1u, 5u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::multipleLayersDifferentContents() {
    const Trade::MaterialData materials[]{
        /* Same thing, twice */
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AlphaBlend, false},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
        }, {3, 4, 6}},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AlphaBlend, false},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
        }, {3, 4, 6}},
        /* Same layer order, different value in one layer. Should be treated
           as different. */
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AlphaBlend, false},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            {Trade::MaterialAttribute::BaseColor, 0xff3366aa_rgbaf},
            {Trade::MaterialAttribute::Metalness, 0.4f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
            {Trade::MaterialAttribute::TextureCoordinates, 3u},
        }, {3, 4, 6}},
    };

    UnsignedInt mapping[3];
    CORRADE_COMPARE(removeDuplicatesInto(materials, mapping), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 0u, 2u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::asArray() {
    const Trade::MaterialData materials[]{
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColorTexture, 2u},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColorTexture, 2u},
        }},
        Trade::MaterialData{Trade::MaterialType::Flat, {}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        Trade::MaterialData{Trade::MaterialType::Flat, {}},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AlphaBlend, false},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
        }}
    };

    Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> out = removeDuplicates(materials);
    CORRADE_COMPARE(out.second(), 4);
    CORRADE_COMPARE_AS(out.first(), Containers::arrayView({
        0u, 1u, 0u, 3u, 1u, 3u, 6u
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::inPlace() {
    Trade::MaterialData materials[]{
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColorTexture, 2u},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColorTexture, 2u},
        }},
        Trade::MaterialData{Trade::MaterialType::Flat, {}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        Trade::MaterialData{Trade::MaterialType::Flat, {}},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AlphaBlend, false},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
        }}
    };

    UnsignedInt mapping[7];
    CORRADE_COMPARE(removeDuplicatesInPlaceInto(materials, mapping), 4);
    CORRADE_COMPARE_AS(Containers::arrayView(mapping), Containers::arrayView({
        0u, 1u, 0u, 2u, 1u, 2u, 3u
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(materials)[0], (Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::BaseColorTexture, 2u},
    }}), DebugTools::CompareMaterial);
    CORRADE_COMPARE_AS(Containers::arrayView(materials)[1], (Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::Roughness, 0.3f},
        {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
    }}), DebugTools::CompareMaterial);
    CORRADE_COMPARE_AS(Containers::arrayView(materials)[2],
        (Trade::MaterialData{Trade::MaterialType::Flat, {}}),
        DebugTools::CompareMaterial);
    CORRADE_COMPARE_AS(Containers::arrayView(materials)[3], (Trade::MaterialData{{}, {
        {Trade::MaterialAttribute::AlphaBlend, false},
        {Trade::MaterialAttribute::AlphaMask, 0.7f},
    }}), DebugTools::CompareMaterial);
}

void RemoveDuplicatesTest::inPlaceAsArray() {
    Trade::MaterialData materials[]{
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColorTexture, 2u},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColorTexture, 2u},
        }},
        Trade::MaterialData{Trade::MaterialType::Flat, {}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
        }},
        Trade::MaterialData{Trade::MaterialType::Flat, {}},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AlphaBlend, false},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
        }}
    };

    Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> out = removeDuplicatesInPlace(materials);
    CORRADE_COMPARE(out.second(), 4);
    CORRADE_COMPARE_AS(out.first(), Containers::arrayView({
        0u, 1u, 0u, 2u, 1u, 2u, 3u
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(materials)[0], (Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::BaseColorTexture, 2u},
    }}), DebugTools::CompareMaterial);
    CORRADE_COMPARE_AS(Containers::arrayView(materials)[1], (Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::Roughness, 0.3f},
        {Trade::MaterialAttribute::SpecularColor, 0x66779900_rgbaf},
    }}), DebugTools::CompareMaterial);
    CORRADE_COMPARE_AS(Containers::arrayView(materials)[2],
        (Trade::MaterialData{Trade::MaterialType::Flat, {}}),
        DebugTools::CompareMaterial);
    CORRADE_COMPARE_AS(Containers::arrayView(materials)[3], (Trade::MaterialData{{}, {
        {Trade::MaterialAttribute::AlphaBlend, false},
        {Trade::MaterialAttribute::AlphaMask, 0.7f},
    }}), DebugTools::CompareMaterial);
}

void RemoveDuplicatesTest::invalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MaterialData data[]{
        Trade::MaterialData{{}, {}},
        Trade::MaterialData{{}, {}},
    };
    UnsignedInt mapping[3];

    Containers::String out;
    Error redirectError{&out};
    removeDuplicatesInto(data, mapping);
    removeDuplicatesInPlaceInto(data, mapping);
    CORRADE_COMPARE(out,
        "MaterialTools::removeDuplicatesInto(): bad output size, expected 2 but got 3\n"
        "MaterialTools::removeDuplicatesInPlaceInto(): bad output size, expected 2 but got 3\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MaterialTools::Test::RemoveDuplicatesTest)
