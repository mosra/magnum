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

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/DebugTools/CompareMaterial.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct CompareMaterialTest: TestSuite::Tester {
    explicit CompareMaterialTest();

    void same();
    void different();
    void differentReverse();
};

using namespace Containers::Literals;
using namespace Math::Literals;

const struct {
    const char* name;
    Trade::MaterialData actual;
    Trade::MaterialData expected;
} SameData[]{
    {"empty",
        Trade::MaterialData{{}, {}},
        Trade::MaterialData{{}, {}}},
    {"empty with types",
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {}}},
    {"base attributes",
        Trade::MaterialData{Trade::MaterialType::Phong, {
            {Trade::MaterialAttribute::DiffuseColor, 0x556699aa_rgbaf},
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {"name", "hello"_s},
        }},
        Trade::MaterialData{Trade::MaterialType::Phong, {
            {"name", "hello"_s},
            {Trade::MaterialAttribute::DiffuseColor, 0x556699aa_rgbaf},
            {Trade::MaterialAttribute::NormalTexture, 5u},
        }}},
    {"layers",
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {Trade::MaterialLayer::ClearCoat},
            {Trade::MaterialAttribute::LayerFactor, 0.76f},
            {"name", "hello"_s},
        }, {1, 3, 4}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {Trade::MaterialAttribute::LayerFactor, 0.76f},
            {Trade::MaterialLayer::ClearCoat},
            {"name", "hello"_s},
        }, {1, 3, 4}}},
    {"fuzzy compare",
        Trade::MaterialData{Trade::MaterialType::Phong, {
            {Trade::MaterialAttribute::Shininess, 2000.0f},
            {Trade::MaterialAttribute::AlphaMask, 0.5f},
            {Trade::MaterialAttribute::DiffuseTexture, 3u},
        }},
        Trade::MaterialData{Trade::MaterialType::Phong, {
            {Trade::MaterialAttribute::Shininess, 2000.0f + 0.01f},
            {Trade::MaterialAttribute::AlphaMask, 0.5f - 1.0e-6f},
            {Trade::MaterialAttribute::DiffuseTexture, 3u},
        }},
    },
};

const struct {
    const char* name;
    Trade::MaterialData actual;
    Trade::MaterialData expected;
    const char* message;
    const char* messageReverse;
} DifferentData[]{
    {"empty, different types",
        Trade::MaterialData{Trade::MaterialType::Flat, {}},
        Trade::MaterialData{Trade::MaterialType::PbrClearCoat|Trade::MaterialType::PbrMetallicRoughness, {}},
        "Materials a and b have different types. Actual (+) vs expected (-):\n"
        "       -Types: PbrMetallicRoughness|PbrClearCoat\n"
        "       +Types: Flat\n",
        "Materials b and a have different types. Actual (+) vs expected (-):\n"
        "       -Types: Flat\n"
        "       +Types: PbrMetallicRoughness|PbrClearCoat\n"},
    {"different types",
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff00ffff_rgbaf},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrClearCoat|Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::BaseColor, 0xff00ffff_rgbaf},
        }},
        "Materials a and b have different types. Actual (+) vs expected (-):\n"
        "       -Types: PbrMetallicRoughness|PbrClearCoat\n"
        "       +Types: Flat\n"
        "        Base layer:\n"
        "          BaseColor @ Vector4: {1, 0, 1, 1}\n",
        "Materials b and a have different types. Actual (+) vs expected (-):\n"
        "       -Types: Flat\n"
        "       +Types: PbrMetallicRoughness|PbrClearCoat\n"
        "        Base layer:\n"
        "          BaseColor @ Vector4: {1, 0, 1, 1}\n"},
    {"different types, one empty",
        Trade::MaterialData{Trade::MaterialType::Flat, {
            {Trade::MaterialAttribute::BaseColor, 0xff00ffff_rgbaf}
        }},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::BaseColor, 0xff00ffff_rgbaf}
        }},
        "Materials a and b have different types. Actual (+) vs expected (-):\n"
        "       +Types: Flat\n"
        "        Base layer:\n"
        "          BaseColor @ Vector4: {1, 0, 1, 1}\n",
        "Materials b and a have different types. Actual (+) vs expected (-):\n"
        "       -Types: Flat\n"
        "        Base layer:\n"
        "          BaseColor @ Vector4: {1, 0, 1, 1}\n"},
    {"different attributes",
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::DoubleSided, true},
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {Trade::MaterialAttribute::NormalTextureScale, 0.5f},
            {Trade::MaterialAttribute::OcclusionTexture, 3u},
        }},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::Metalness, 5.5f},
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {Trade::MaterialAttribute::NormalTextureLayer, 2u},
        }},
        "Materials a and b have different attributes. Actual (+) vs expected (-):\n"
        "        Types: PbrMetallicRoughness\n"
        "        Base layer:\n"
        "       +  DoubleSided @ Bool: true\n"
        "       -  Metalness @ Float: 5.5\n"
        "          NormalTexture @ UnsignedInt: 5\n"
        "       -  NormalTextureLayer @ UnsignedInt: 2\n"
        "       +  NormalTextureScale @ Float: 0.5\n"
        "       +  OcclusionTexture @ UnsignedInt: 3\n",
        "Materials b and a have different attributes. Actual (+) vs expected (-):\n"
        "        Types: PbrMetallicRoughness\n"
        "        Base layer:\n"
        "       -  DoubleSided @ Bool: true\n"
        "       +  Metalness @ Float: 5.5\n"
        "          NormalTexture @ UnsignedInt: 5\n"
        "       +  NormalTextureLayer @ UnsignedInt: 2\n"
        "       -  NormalTextureScale @ Float: 0.5\n"
        "       -  OcclusionTexture @ UnsignedInt: 3\n"},
    {"different attributes, empty base layer",
        /* Same as above, except everything is in the second layer to verify
           we're not accidentally checking just the first layer */
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::DoubleSided, true},
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {Trade::MaterialAttribute::NormalTextureScale, 0.5f},
            {Trade::MaterialAttribute::OcclusionTexture, 3u},
        }, {0, 4}},
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
            {Trade::MaterialAttribute::Metalness, 5.5f},
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {Trade::MaterialAttribute::NormalTextureLayer, 2u},
        }, {0, 3}},
        "Materials a and b have different attributes. Actual (+) vs expected (-):\n"
        "        Types: PbrMetallicRoughness\n"
        "        Base layer:\n"
        "        Layer 1:\n"
        "       +  DoubleSided @ Bool: true\n"
        "       -  Metalness @ Float: 5.5\n"
        "          NormalTexture @ UnsignedInt: 5\n"
        "       -  NormalTextureLayer @ UnsignedInt: 2\n"
        "       +  NormalTextureScale @ Float: 0.5\n"
        "       +  OcclusionTexture @ UnsignedInt: 3\n",
        "Materials b and a have different attributes. Actual (+) vs expected (-):\n"
        "        Types: PbrMetallicRoughness\n"
        "        Base layer:\n"
        "        Layer 1:\n"
        "       -  DoubleSided @ Bool: true\n"
        "       +  Metalness @ Float: 5.5\n"
        "          NormalTexture @ UnsignedInt: 5\n"
        "       +  NormalTextureLayer @ UnsignedInt: 2\n"
        "       -  NormalTextureScale @ Float: 0.5\n"
        "       -  OcclusionTexture @ UnsignedInt: 3\n"},
    {"different attribute types",
        Trade::MaterialData{{}, {
            {"pointer", reinterpret_cast<void*>(std::size_t{0xdead})},
            {"integer", 5u},
            {"scale", 0.5f},
        }},
        Trade::MaterialData{{}, {
            {"pointer", reinterpret_cast<const void*>(std::size_t{0xdead})},
            {"integer", 5},
            {"scale", "small"},
        }},
        "Materials a and b have different attribute types. Actual (+) vs expected (-):\n"
        "        Base layer:\n"
        "       -  integer @ Int: 5\n"
        "       +  integer @ UnsignedInt: 5\n"
        "       -  pointer @ Pointer: 0xdead\n"
        "       +  pointer @ MutablePointer: 0xdead\n"
        "       -  scale @ String: small\n"
        "       +  scale @ Float: 0.5\n",
        "Materials b and a have different attribute types. Actual (+) vs expected (-):\n"
        "        Base layer:\n"
        "       -  integer @ UnsignedInt: 5\n"
        "       +  integer @ Int: 5\n"
        "       -  pointer @ MutablePointer: 0xdead\n"
        "       +  pointer @ Pointer: 0xdead\n"
        "       -  scale @ Float: 0.5\n"
        "       +  scale @ String: small\n"},
    {"different attribute values",
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::Metalness, 0.3f},
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {Trade::MaterialAttribute::NormalTextureSwizzle, Trade::MaterialTextureSwizzle::RGB},
            {"buffer", Containers::ArrayView<const void>{"\x56\x78\x22"}},
            {"pointer", reinterpret_cast<const void*>(std::size_t{0xbeef})},
            {"pointerMutable", reinterpret_cast<void*>(std::size_t{0xdead})},
        }},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::Metalness, 5.5f},
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {Trade::MaterialAttribute::NormalTextureSwizzle, Trade::MaterialTextureSwizzle::RG},
            {"buffer", Containers::ArrayView<const void>{"\x56\x78\x22"}},
            {"pointer", reinterpret_cast<const void*>(std::size_t{0xbeef})},
            {"pointerMutable", reinterpret_cast<void*>(std::size_t{0xdead})},
        }},
        "Materials a and b have different attribute values. Actual (+) vs expected (-):\n"
        "        Base layer:\n"
        "       -  Metalness @ Float: 5.5\n"
        "       +  Metalness @ Float: 0.3\n"
        "          NormalTexture @ UnsignedInt: 5\n"
        "       -  NormalTextureSwizzle @ TextureSwizzle: RG\n"
        "       +  NormalTextureSwizzle @ TextureSwizzle: RGB\n"
        "          buffer @ Buffer: {86, 120, 34, 0}\n"
        "          pointer @ Pointer: 0xbeef\n"
        "          pointerMutable @ MutablePointer: 0xdead\n",
        "Materials b and a have different attribute values. Actual (+) vs expected (-):\n"
        "        Base layer:\n"
        "       -  Metalness @ Float: 0.3\n"
        "       +  Metalness @ Float: 5.5\n"
        "          NormalTexture @ UnsignedInt: 5\n"
        "       -  NormalTextureSwizzle @ TextureSwizzle: RGB\n"
        "       +  NormalTextureSwizzle @ TextureSwizzle: RG\n"
        "          buffer @ Buffer: {86, 120, 34, 0}\n"
        "          pointer @ Pointer: 0xbeef\n"
        "          pointerMutable @ MutablePointer: 0xdead\n"},
    {"different attributes in layers",
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DoubleSided, true},
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {Trade::MaterialAttribute::NormalTextureScale, 0.5f},
            {Trade::MaterialAttribute::OcclusionTexture, 3u},
            {"texturePointer", reinterpret_cast<void*>(std::size_t{0xdead})},
        }, {3, 5}},
        Trade::MaterialData{{}, {
            {Trade::MaterialAttribute::DoubleSided, false},
            {Trade::MaterialAttribute::NormalTexture, 5u},
            {Trade::MaterialAttribute::OcclusionTexture, 3u},
            {"texturePointer", reinterpret_cast<const void*>(std::size_t{0xdead})},
            {Trade::MaterialAttribute::NormalTextureLayer, 2u},
        }, {2, 4, 5}},
        "Materials a and b have different layers. Actual (+) vs expected (-):\n"
        "        Base layer:\n"
        "       -  DoubleSided @ Bool: false\n"
        "       +  DoubleSided @ Bool: true\n"
        "          NormalTexture @ UnsignedInt: 5\n"
        "       +  NormalTextureScale @ Float: 0.5\n"
        "        Layer 1:\n"
        "          OcclusionTexture @ UnsignedInt: 3\n"
        "       -  texturePointer @ Pointer: 0xdead\n"
        "       +  texturePointer @ MutablePointer: 0xdead\n"
        "       -Layer 2:\n"
        "       -  NormalTextureLayer @ UnsignedInt: 2\n",
        "Materials b and a have different layers. Actual (+) vs expected (-):\n"
        "        Base layer:\n"
        "       -  DoubleSided @ Bool: true\n"
        "       +  DoubleSided @ Bool: false\n"
        "          NormalTexture @ UnsignedInt: 5\n"
        "       -  NormalTextureScale @ Float: 0.5\n"
        "        Layer 1:\n"
        "          OcclusionTexture @ UnsignedInt: 3\n"
        "       -  texturePointer @ MutablePointer: 0xdead\n"
        "       +  texturePointer @ Pointer: 0xdead\n"
        "       +Layer 2:\n"
        "       +  NormalTextureLayer @ UnsignedInt: 2\n"},
    {"different (empty) layer count",
        Trade::MaterialData{{}, {}},
        Trade::MaterialData{{}, {}, {0, 0, 0}},
        "Materials a and b have different layers. Actual (+) vs expected (-):\n"
        "        Base layer:\n"
        "       -Layer 1:\n"
        "       -Layer 2:\n",
        "Materials b and a have different layers. Actual (+) vs expected (-):\n"
        "        Base layer:\n"
        "       +Layer 1:\n"
        "       +Layer 2:\n"},
};

CompareMaterialTest::CompareMaterialTest() {
    addInstancedTests({&CompareMaterialTest::same},
        Containers::arraySize(SameData));

    addInstancedTests({&CompareMaterialTest::different,
                       &CompareMaterialTest::differentReverse},
        Containers::arraySize(DifferentData));
}

void CompareMaterialTest::same() {
    auto&& data = SameData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_COMPARE_AS(data.actual, data.expected, CompareMaterial);
}

void CompareMaterialTest::different() {
    auto&& data = DifferentData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    TestSuite::Comparator<CompareMaterial> compare;
    TestSuite::ComparisonStatusFlags flags = compare(data.actual, data.expected);
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);

    CORRADE_INFO("Visual color verification:");
    {
        Debug out;
        compare.printMessage(flags, out, "a", "b");
    }

    Containers::String out;
    {
        Debug dc{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, dc, "a", "b");
    }
    CORRADE_COMPARE(out, data.message);
}

void CompareMaterialTest::differentReverse() {
    auto&& data = DifferentData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    TestSuite::Comparator<CompareMaterial> compare;
    TestSuite::ComparisonStatusFlags flags = compare(data.expected, data.actual);
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);

    CORRADE_INFO("Visual color verification:");
    {
        Debug out;
        compare.printMessage(flags, out, "b", "a");
    }

    Containers::String out;
    {
        Debug dc{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, dc, "b", "a");
    }
    CORRADE_COMPARE(out, data.messageReverse);
}

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::CompareMaterialTest)
