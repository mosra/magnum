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
#include "Magnum/Math/Color.h"
#include "Magnum/MaterialTools/Merge.h"
#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools { namespace Test { namespace {

struct MergeTest: TestSuite::Tester {
    explicit MergeTest();

    void singleLayer();

    void multipleLayersIntoSingleLayer();
    void multipleLayers();

    void conflictsSameType();
    void conflictsDifferentType();
    void conflictsFail();

    void emptyInput();
};

using namespace Math::Literals;

MergeTest::MergeTest() {
    addTests({&MergeTest::singleLayer,

              &MergeTest::multipleLayersIntoSingleLayer,
              &MergeTest::multipleLayers,

              &MergeTest::conflictsSameType,
              &MergeTest::conflictsDifferentType,
              &MergeTest::conflictsFail,

              &MergeTest::emptyInput});
}

void MergeTest::singleLayer() {
    Trade::MaterialData a{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        /* These two go at the end */
        {Trade::MaterialAttribute::RoughnessTexture, 7u},
        {Trade::MaterialAttribute::Roughness, 0.3f}
    }};

    Trade::MaterialData b{Trade::MaterialType::Phong|Trade::MaterialType::Flat, {
        /* This attribute goes first */
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::DiffuseColor, 0x808080ff_rgbaf},
        {Trade::MaterialAttribute::BaseColorTexture, 3u},
        {Trade::MaterialAttribute::BaseColorTextureCoordinates, 2u},
    }};

    Trade::MaterialData expected{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::Phong|Trade::MaterialType::Flat, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::BaseColorTexture, 3u},
        {Trade::MaterialAttribute::BaseColorTextureCoordinates, 2u},
        {Trade::MaterialAttribute::DiffuseColor, 0x808080ff_rgbaf},
        {Trade::MaterialAttribute::Roughness, 0.3f},
        {Trade::MaterialAttribute::RoughnessTexture, 7u},
    }};

    /* It should give the same result both ways */
    {
        Containers::Optional<Trade::MaterialData> actual = merge(a, b);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, expected, DebugTools::CompareMaterial);
    } {
        Containers::Optional<Trade::MaterialData> actual = merge(b, a);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, expected, DebugTools::CompareMaterial);
    }
}

void MergeTest::multipleLayersIntoSingleLayer() {
    Trade::MaterialData a{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::RoughnessTexture, 7u},
        {Trade::MaterialAttribute::Roughness, 0.3f},
    }};

    Trade::MaterialData b{Trade::MaterialType::PbrClearCoat|Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        /* These two layers are only in this material */
        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::LayerFactor, 0.1f},
        {Trade::MaterialAttribute::Roughness, 0.5f},
        {Trade::MaterialAttribute::LayerFactor, 1.0f},
        {"layerBlendApproach", "irreversibly"}
    }, {1, 4, 6}};

    Trade::MaterialData expected{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::Roughness, 0.3f},
        {Trade::MaterialAttribute::RoughnessTexture, 7u},
        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::LayerFactor, 0.1f},
        {Trade::MaterialAttribute::Roughness, 0.5f},
        {Trade::MaterialAttribute::LayerFactor, 1.0f},
        {"layerBlendApproach", "irreversibly"}
    }, {4, 7, 9}};

    /* It should give the same result both ways */
    {
        Containers::Optional<Trade::MaterialData> actual = merge(a, b);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, expected, DebugTools::CompareMaterial);
    } {
        Containers::Optional<Trade::MaterialData> actual = merge(b, a);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, expected, DebugTools::CompareMaterial);
    }
}

void MergeTest::multipleLayers() {
    Trade::MaterialData a{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::RoughnessTexture, 7u},
        {Trade::MaterialAttribute::Roughness, 0.3f},
        /* This layer has no name but it'll get it from the other material
           (and that's fine) */
        {Trade::MaterialAttribute::LayerFactor, 0.1f},
        {Trade::MaterialAttribute::Roughness, 0.5f},
    }, {3, 5}};

    Trade::MaterialData b{Trade::MaterialType::PbrClearCoat|Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        /* These two layers are only in this material */
        {Trade::MaterialLayer::ClearCoat},
        /* Here's an empty layer that ends up being empty as well */
        {Trade::MaterialAttribute::LayerFactor, 1.0f},
        {"layerBlendApproach", "irreversibly"}
    }, {1, 2, 2, 4}};

    Trade::MaterialData expected{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::Roughness, 0.3f},
        {Trade::MaterialAttribute::RoughnessTexture, 7u},
        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::LayerFactor, 0.1f},
        {Trade::MaterialAttribute::Roughness, 0.5f},
        /* Empty layer here */
        {Trade::MaterialAttribute::LayerFactor, 1.0f},
        {"layerBlendApproach", "irreversibly"}
    }, {4, 7, 7, 9}};

    /* It should give the same result both ways */
    {
        Containers::Optional<Trade::MaterialData> actual = merge(a, b);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, expected, DebugTools::CompareMaterial);
    } {
        Containers::Optional<Trade::MaterialData> actual = merge(b, a);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, expected, DebugTools::CompareMaterial);
    }
}

void MergeTest::conflictsSameType() {
    Trade::MaterialData a{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::RoughnessTexture, 7u},
        /* Second layer */
        {Trade::MaterialAttribute::Roughness, 0.3f},
        {"customAttribute", 15.0f},
    }, {2, 4}};

    Trade::MaterialData b{Trade::MaterialType::PbrClearCoat, {
        {Trade::MaterialAttribute::Roughness, 0.5f},
        {Trade::MaterialAttribute::RoughnessTexture, 777u},
        {Trade::MaterialAttribute::LayerFactor, 1.0f},
        {"customAttribute", 223.0f},
    }, {2, 4}};

    /* If called swapped it'll use the other values */
    {
        Containers::Optional<Trade::MaterialData> actual = merge(a, b, MergeConflicts::KeepFirstIfSameType);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, (Trade::MaterialData {Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {
            {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
            {Trade::MaterialAttribute::Roughness, 0.5f},
            {Trade::MaterialAttribute::RoughnessTexture, 7u},
            {Trade::MaterialAttribute::LayerFactor, 1.0f},
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {"customAttribute", 15.0f},
        }, {3, 6}}), DebugTools::CompareMaterial);
    } {
        Containers::Optional<Trade::MaterialData> actual = merge(b, a, MergeConflicts::KeepFirstIfSameType);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, (Trade::MaterialData {Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {
            {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
            {Trade::MaterialAttribute::Roughness, 0.5f},
            {Trade::MaterialAttribute::RoughnessTexture, 777u}, /* different */
            {Trade::MaterialAttribute::LayerFactor, 1.0f},
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {"customAttribute", 223.0f}, /* different */
        }, {3, 6}}), DebugTools::CompareMaterial);
    }
}

void MergeTest::conflictsDifferentType() {
    Trade::MaterialData a{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::RoughnessTexture, 7u},
        /* Second layer */
        {Trade::MaterialAttribute::Roughness, 0.3f},
        {"customAttribute", 15.0f},
    }, {2, 4}};

    /* Builtin attributes have an enforced type so this can only happen with
       custom ones. It should however handle (builtin) attributes of the same
       type as well */
    Trade::MaterialData b{Trade::MaterialType::PbrClearCoat, {
        {Trade::MaterialAttribute::Roughness, 0.5f},
        {Trade::MaterialAttribute::RoughnessTexture, 777u},
        {Trade::MaterialAttribute::LayerFactor, 1.0f},
        {"customAttribute", "hello!"},
    }, {2, 4}};

    /* If called swapped it'll use the other values */
    {
        Containers::Optional<Trade::MaterialData> actual = merge(a, b, MergeConflicts::KeepFirstIgnoreType);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, (Trade::MaterialData {Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {
            {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
            {Trade::MaterialAttribute::Roughness, 0.5f},
            {Trade::MaterialAttribute::RoughnessTexture, 7u},
            {Trade::MaterialAttribute::LayerFactor, 1.0f},
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {"customAttribute", 15.0f},
        }, {3, 6}}), DebugTools::CompareMaterial);
    } {
        Containers::Optional<Trade::MaterialData> actual = merge(b, a, MergeConflicts::KeepFirstIgnoreType);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, (Trade::MaterialData {Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {
            {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
            {Trade::MaterialAttribute::Roughness, 0.5f},
            {Trade::MaterialAttribute::RoughnessTexture, 777u}, /* different */
            {Trade::MaterialAttribute::LayerFactor, 1.0f},
            {Trade::MaterialAttribute::Roughness, 0.3f},
            {"customAttribute", "hello!"}, /* different */
        }, {3, 6}}), DebugTools::CompareMaterial);
    }
}

void MergeTest::conflictsFail() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MaterialData a{{}, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::RoughnessTexture, 7u},
        /* Second layer */
        {Trade::MaterialAttribute::Roughness, 0.3f},
        {"customAttribute", 15.0f},
    }, {2, 4}};

    /* Contains Roughness but in another layer (which should be fine),
       but has a conflicitng BaseColorTexture even though it's the same
       value */
    Trade::MaterialData b{{}, {
        {Trade::MaterialAttribute::Roughness, 0.5f},
        {Trade::MaterialAttribute::RoughnessTexture, 7u},
        {Trade::MaterialAttribute::LayerFactor, 1.0f},
    }, {2, 3}};

    /* Contains customAttribute in second layer which is of a different type.
       The RoughnessTexture is also conflicting but that shouldn't produce a
       message since it's the same type. */
    Trade::MaterialData c{{}, {
        {Trade::MaterialAttribute::Roughness, 0.5f},
        {Trade::MaterialAttribute::RoughnessTexture, 1u},
        {"customAttribute", "hello"},
    }, {2, 3}};

    /* Verify that it fails in all variants */
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!merge(a, b));
    CORRADE_VERIFY(!merge(b, a));
    CORRADE_VERIFY(!merge(a, c, MergeConflicts::KeepFirstIfSameType));
    CORRADE_VERIFY(!merge(c, a, MergeConflicts::KeepFirstIfSameType));
    CORRADE_COMPARE(out,
        "MaterialTools::merge(): conflicting attribute RoughnessTexture in layer 0\n"
        "MaterialTools::merge(): conflicting attribute RoughnessTexture in layer 0\n"
        "MaterialTools::merge(): conflicting type Trade::MaterialAttributeType::Float vs String of attribute customAttribute in layer 1\n"
        "MaterialTools::merge(): conflicting type Trade::MaterialAttributeType::String vs Float of attribute customAttribute in layer 1\n");
}

void MergeTest::emptyInput() {
    Trade::MaterialData a{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::Roughness, 0.3f},
        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::LayerFactor, 0.1f},
    }, {2, 4}};

    Trade::MaterialData empty{Trade::MaterialType::PbrClearCoat, {}};

    /* The result has just the types changed, nothing else */
    Trade::MaterialData expected{Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrClearCoat, {}, a.attributeData(), {}, a.layerData()};

    /* It should give the same result both ways */
    {
        Containers::Optional<Trade::MaterialData> actual = merge(a, empty);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, expected, DebugTools::CompareMaterial);
    } {
        Containers::Optional<Trade::MaterialData> actual = merge(empty, a);
        CORRADE_VERIFY(actual);
        CORRADE_COMPARE_AS(*actual, expected, DebugTools::CompareMaterial);
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::MaterialTools::Test::MergeTest)
