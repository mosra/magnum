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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/DebugTools/CompareMaterial.h"
#include "Magnum/MaterialTools/PhongToPbrMetallicRoughness.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools { namespace Test { namespace {

struct PhongToPbrMetallicRoughnessTest: TestSuite::Tester {
    explicit PhongToPbrMetallicRoughnessTest();

    void convert();
    void warning();
    void fail();
};

using namespace Math::Literals;

const struct {
    const char* name;
    PhongToPbrMetallicRoughnessFlags flags;
    Trade::MaterialData material;
    Trade::MaterialData expected;
} ConvertData[]{
    {"empty with no type", {},
        Trade::MaterialData{{}, {}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {}}},
    {"empty with Phong and other types", {},
        Trade::MaterialData{Trade::MaterialType::Phong|Trade::MaterialType::PbrClearCoat, {}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {}}},
    {"diffuse color", {},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366ff_rgbaf}
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366ff_rgbaf}
        }}},
    {"diffuse texture + matrix", {},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseTexture, 5u},
            {Trade::MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling(Vector2{0.5f})},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColorTexture, 5u},
            {Trade::MaterialAttribute::BaseColorTextureMatrix, Matrix3::scaling(Vector2{0.5f})},
        }}},
    {"diffuse color + texture + coordinates + layer", {},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::DiffuseTexture, 7u},
            {Trade::MaterialAttribute::DiffuseTextureCoordinates, 2u},
            {Trade::MaterialAttribute::DiffuseTextureLayer, 155u},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::BaseColorTexture, 7u},
            {Trade::MaterialAttribute::BaseColorTextureCoordinates, 2u},
            {Trade::MaterialAttribute::BaseColorTextureLayer, 155u},
        }}},
    {"both diffuse and base color", {},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366ff_rgbaf},
            {Trade::MaterialAttribute::BaseColor, 0x3366ffff_rgbaf}
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            /* It's not overwritten as it's assumed to be more correct */
            {Trade::MaterialAttribute::BaseColor, 0x3366ffff_rgbaf}
        }}},
    {"both diffuse and base color texture but different texture properties", {},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseTexture, 8u},
            {Trade::MaterialAttribute::DiffuseTextureLayer, 12u},
            {Trade::MaterialAttribute::DiffuseTextureMatrix, Matrix3{}},
            {Trade::MaterialAttribute::BaseColorTexture, 11u},
            {Trade::MaterialAttribute::BaseColorTextureCoordinates, 1u},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            /* It's not overwritten as it's assumed to be more correct;
               texture-related attributes are not mixed together */
            {Trade::MaterialAttribute::BaseColorTexture, 11u},
            {Trade::MaterialAttribute::BaseColorTextureCoordinates, 1u},
        }}},
    {"keep original attributes", PhongToPbrMetallicRoughnessFlag::KeepOriginalAttributes,
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::DiffuseTexture, 7u},
            {Trade::MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling(Vector2{0.5f})},
            {Trade::MaterialAttribute::DiffuseTextureCoordinates, 2u},
            {Trade::MaterialAttribute::DiffuseTextureLayer, 155u},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::BaseColorTexture, 7u},
            {Trade::MaterialAttribute::BaseColorTextureMatrix, Matrix3::scaling(Vector2{0.5f})},
            {Trade::MaterialAttribute::BaseColorTextureCoordinates, 2u},
            {Trade::MaterialAttribute::BaseColorTextureLayer, 155u},
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::DiffuseTexture, 7u},
            {Trade::MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling(Vector2{0.5f})},
            {Trade::MaterialAttribute::DiffuseTextureCoordinates, 2u},
            {Trade::MaterialAttribute::DiffuseTextureLayer, 155u},
        }}},
    {"extra attributes and layers", {},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366ff_rgbaf},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            {Trade::MaterialAttribute::DiffuseTexture, 7u},
            {Trade::MaterialLayer::ClearCoat},
            {Trade::MaterialAttribute::LayerFactor, 0.35f},
        }, {2, 3, 5}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366ff_rgbaf},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            /* Shouldn't get converted because it's a different layer */
            {Trade::MaterialAttribute::DiffuseTexture, 7u},
            {Trade::MaterialLayer::ClearCoat},
            {Trade::MaterialAttribute::LayerFactor, 0.35f},
        }, {2, 3, 5}}},
    {"extra attributes and layers, keep original attributes", PhongToPbrMetallicRoughnessFlag::KeepOriginalAttributes,
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366ff_rgbaf},
            {Trade::MaterialAttribute::DiffuseTexture, 0u},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            {Trade::MaterialLayer::ClearCoat},
            {Trade::MaterialAttribute::LayerFactor, 0.35f},
        }, {3, 5}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366ff_rgbaf},
            {Trade::MaterialAttribute::BaseColorTexture, 0u},
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366ff_rgbaf},
            {Trade::MaterialAttribute::DiffuseTexture, 0u},
            {Trade::MaterialAttribute::AlphaMask, 0.7f},
            {Trade::MaterialLayer::ClearCoat},
            {Trade::MaterialAttribute::LayerFactor, 0.35f},
        }, {5, 7}}},
    {"diffuse texture properties without texture", {},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling(Vector2{0.5f})},
            {Trade::MaterialAttribute::DiffuseTextureCoordinates, 2u},
            {Trade::MaterialAttribute::DiffuseTextureLayer, 155u},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366cc_rgbaf},
            /* Those are kept and don't produce any warning because the
               texture wasn't found and thus the branch wasn't taken */
            {Trade::MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling(Vector2{0.5f})},
            {Trade::MaterialAttribute::DiffuseTextureCoordinates, 2u},
            {Trade::MaterialAttribute::DiffuseTextureLayer, 155u},
        }}},
    {"ambient texture properties without texture, fail on unconvertible", PhongToPbrMetallicRoughnessFlag::FailOnUnconvertibleAttributes,
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::AmbientTextureMatrix, Matrix3::scaling(Vector2{0.5f})},
            {Trade::MaterialAttribute::AmbientTextureLayer, 356u},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366cc_rgbaf},
            /* These are kept and don't produce any failure because the
               texture wasn't found and thus the branch wasn't taken */
            {Trade::MaterialAttribute::AmbientTextureMatrix, Matrix3::scaling(Vector2{0.5f})},
            {Trade::MaterialAttribute::AmbientTextureLayer, 356u},
        }}},
};

const struct {
    const char* name;
    PhongToPbrMetallicRoughnessFlags flags;
    Trade::MaterialData material;
    Trade::MaterialData expected;
    const char* message;
} WarningData[]{
    {"ambient color, specular texture + coordinates", {},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::AmbientColor, 0x0f030600_rgbaf},
            {Trade::MaterialAttribute::SpecularTexture, 3u},
            {Trade::MaterialAttribute::SpecularTextureCoordinates, 2u},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::AmbientColor, 0x0f030600_rgbaf},
            {Trade::MaterialAttribute::SpecularTexture, 3u},
            {Trade::MaterialAttribute::SpecularTextureCoordinates, 2u},
        }},
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::AmbientColor attribute, skipping\n"
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::SpecularTexture attribute, skipping\n"},
    {"specular color, texture + matrix + layer, ambient texture + coordinates; drop unconvertible", PhongToPbrMetallicRoughnessFlag::DropUnconvertibleAttributes,
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DiffuseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::SpecularColor, 0x3366ffff_rgbaf},
            {Trade::MaterialAttribute::SpecularTexture, 3u},
            {Trade::MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling(Vector2{0.5f})},
            {Trade::MaterialAttribute::SpecularTextureLayer, 156u},
            {Trade::MaterialAttribute::AmbientTexture, 1u},
            {Trade::MaterialAttribute::AmbientTextureCoordinates, 2u},
            {Trade::MaterialAttribute::DoubleSided, true}
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff3366cc_rgbaf},
            {Trade::MaterialAttribute::DoubleSided, true}
        }},
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::SpecularColor attribute, skipping\n"
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::AmbientTexture attribute, skipping\n"
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::SpecularTexture attribute, skipping\n"},
};

const struct {
    const char* name;
    PhongToPbrMetallicRoughnessFlags flags;
    Trade::MaterialData material;
    const char* message;
} FailData[]{
    {"ambient color, fail on unconvertible", PhongToPbrMetallicRoughnessFlag::FailOnUnconvertibleAttributes,
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::AmbientColor, Color4{}},
        }},
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::AmbientColor attribute\n"},
    {"specular texture, fail on unconvertible", PhongToPbrMetallicRoughnessFlag::FailOnUnconvertibleAttributes,
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::SpecularTexture, 0u},
        }},
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::SpecularTexture attribute\n"},
    {"shininess, fail on unconvertible", PhongToPbrMetallicRoughnessFlag::FailOnUnconvertibleAttributes,
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::Shininess, 0.5f},
        }},
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::Shininess attribute\n"},
    {"specular color, both drop & fail on unconvertible", PhongToPbrMetallicRoughnessFlag::FailOnUnconvertibleAttributes|PhongToPbrMetallicRoughnessFlag::DropUnconvertibleAttributes,
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::SpecularTexture, 0u},
        }},
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::SpecularTexture attribute\n"},
};

PhongToPbrMetallicRoughnessTest::PhongToPbrMetallicRoughnessTest() {
    addInstancedTests({&PhongToPbrMetallicRoughnessTest::convert},
        Containers::arraySize(ConvertData));

    addInstancedTests({&PhongToPbrMetallicRoughnessTest::warning},
        Containers::arraySize(WarningData));

    addInstancedTests({&PhongToPbrMetallicRoughnessTest::fail},
        Containers::arraySize(FailData));
}

void PhongToPbrMetallicRoughnessTest::convert() {
    auto&& data = ConvertData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Error redirectError{&out};
    Warning redirectWarning{&out};
    Containers::Optional<Trade::MaterialData> actual = phongToPbrMetallicRoughness(data.material, data.flags);
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE_AS(*actual, data.expected, DebugTools::CompareMaterial);
    CORRADE_COMPARE(out, "");
}

void PhongToPbrMetallicRoughnessTest::warning() {
    auto&& data = WarningData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Warning redirectWarning{&out};
    Containers::Optional<Trade::MaterialData> actual = phongToPbrMetallicRoughness(data.material, data.flags);
    CORRADE_VERIFY(actual);
    CORRADE_COMPARE_AS(*actual, data.expected, DebugTools::CompareMaterial);
    CORRADE_COMPARE(out, data.message);
}

void PhongToPbrMetallicRoughnessTest::fail() {
    auto&& data = FailData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!phongToPbrMetallicRoughness(data.material, data.flags));
    CORRADE_COMPARE(out, data.message);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MaterialTools::Test::PhongToPbrMetallicRoughnessTest)
