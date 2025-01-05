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

#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/DebugTools/CompareMaterial.h"
#include "Magnum/MaterialTools/Filter.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools { namespace Test { namespace {

struct FilterTest: TestSuite::Tester {
    explicit FilterTest();

    void attributes();
    void attributesMultipleLayers();
    void attributesWrongBitCount();

    void layers();
    void layersRemoveBase();
    void layersWrongBitCount();

    void attributesLayers();
    void attributesLayersRemoveBaseLayer();
    void attributesLayersWrongBitCount();

    void emptyInput();
};

FilterTest::FilterTest() {
    addTests({&FilterTest::attributes,
              &FilterTest::attributesMultipleLayers,
              &FilterTest::attributesWrongBitCount,

              &FilterTest::layers,
              &FilterTest::layersRemoveBase,
              &FilterTest::layersWrongBitCount,

              &FilterTest::attributesLayers,
              &FilterTest::attributesLayersRemoveBaseLayer,
              &FilterTest::attributesLayersWrongBitCount,

              &FilterTest::emptyInput});
}

using namespace Math::Literals;

void FilterTest::attributes() {
    /* Supplying the attributes as external in order to make sure they're
       sorted for correct numbering */
    const Trade::MaterialAttributeData attributes[]{
        {Trade::MaterialAttribute::AlphaBlend, true},               /* 0 */
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},    /* 1 */
        {Trade::MaterialAttribute::BaseColorTexture, 7u},           /* 2 */
        {Trade::MaterialAttribute::TextureCoordinates, 11u},        /* 3 */
        {Trade::MaterialAttribute::TextureLayer, 0u},               /* 4 */
    };
    Trade::MaterialData material{Trade::MaterialType::PbrClearCoat|Trade::MaterialType::Flat|Trade::MaterialType::Phong, Trade::DataFlags{}, attributes};

    Containers::BitArray attributesToKeep{DirectInit, 5, true};
    attributesToKeep.reset(1);
    attributesToKeep.reset(3);
    attributesToKeep.reset(4);

    Trade::MaterialData filtered = filterAttributes(material, attributesToKeep, Trade::MaterialType::PbrClearCoat|Trade::MaterialType::PbrMetallicRoughness);

    /* The types are kept intact even if they don't make sense, that's a job
       for some higher-level utility that understands their relations to
       present attributes */
    CORRADE_COMPARE_AS(filtered, (Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColorTexture, 7u},
    }}), DebugTools::CompareMaterial);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MaterialAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());

    /* Removing all shouldn't do anything unexpected */
    CORRADE_COMPARE_AS(filterAttributes(material, Containers::BitArray{ValueInit, 5}, {}), (Trade::MaterialData{{}, {
    }}), DebugTools::CompareMaterial);
}

void FilterTest::attributesMultipleLayers() {
    const Trade::MaterialAttributeData attributes[]{
        {Trade::MaterialAttribute::AlphaBlend, true},               /* 0 */
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},    /* 1 */
        {Trade::MaterialAttribute::BaseColorTexture, 7u},           /* 2 */
        {Trade::MaterialLayer::ClearCoat},                          /* 3 */
        {Trade::MaterialAttribute::LayerFactor, 0.7f},              /* 4 */
        {Trade::MaterialAttribute::Roughness, 0.25f},               /* 5 */
        /* One empty layer here */
        {"textureBlendMode", "strongly!"},                          /* 6 */
        {"texturePointer", nullptr},                                /* 7 */
    };
    const UnsignedInt layers[]{3, 6, 6, 8};
    Trade::MaterialData material{Trade::MaterialType::PbrClearCoat, Trade::DataFlags{}, attributes, Trade::DataFlags{}, layers};

    Containers::BitArray attributesToKeep{DirectInit, 8, true};
    attributesToKeep.reset(0);
    attributesToKeep.reset(2);
    attributesToKeep.reset(4);
    attributesToKeep.reset(6);

    Trade::MaterialData filtered = filterAttributes(material, attributesToKeep);

    CORRADE_COMPARE_AS(filtered, (Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::Roughness, 0.25f},
        /* Empty layer stays */
        {"texturePointer", nullptr},
    }, {1, 3, 3, 4}}), DebugTools::CompareMaterial);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MaterialAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());

    /* Removing all shouldn't do anything unexpected */
    CORRADE_COMPARE_AS(filterAttributes(material, Containers::BitArray{ValueInit, 8}), (Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {
    }, {0, 0, 0, 0}}), DebugTools::CompareMaterial);
}

void FilterTest::attributesWrongBitCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MaterialData material{{}, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::BaseColorTexture, 7u},
        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::LayerFactor, 0.7f},
    }, {2, 4}};
    Containers::BitArrayView attributesToKeep{nullptr, 0, 5};

    Containers::String out;
    Error redirectError{&out};
    filterAttributes(material, attributesToKeep);
    CORRADE_COMPARE(out, "MaterialTools::filterAttributes(): expected 4 bits but got 5\n");
}

void FilterTest::layers() {
    const Trade::MaterialAttributeData attributes[]{
        {Trade::MaterialAttribute::AlphaBlend, true},               /* 0 */
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::BaseColorTexture, 7u},
        /* One empty layer here */                                  /* 1 #3 */
        {Trade::MaterialLayer::ClearCoat},                          /* 2 #3 */
        {Trade::MaterialAttribute::LayerFactor, 0.7f},
        {Trade::MaterialAttribute::Roughness, 0.25f},
        {"textureBlendMode", "strongly!"},                          /* 3 #6 */
        {"texturePointer", nullptr},
        /* Another empty layer here */                              /* 4 #8 */
        {Trade::MaterialAttribute::NormalTextureSwizzle,            /* 5 #8 */
            Trade::MaterialTextureSwizzle::RG},
        {"againSomething", false},                                  /* 6 #9 */
    };
    const UnsignedInt layers[]{3, 3, 6, 8, 8, 9, 10};
    Trade::MaterialData material{Trade::MaterialType::PbrClearCoat|Trade::MaterialType::PbrMetallicRoughness, Trade::DataFlags{}, attributes, Trade::DataFlags{}, layers};

    Containers::BitArray layersToKeep{DirectInit, 7, true};
    layersToKeep.reset(1);
    layersToKeep.reset(2);
    layersToKeep.reset(5);

    Trade::MaterialData filtered = filterLayers(material, layersToKeep, Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrSpecularGlossiness);

    CORRADE_COMPARE_AS(filtered, (Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::BaseColorTexture, 7u},
        {"textureBlendMode", "strongly!"},
        {"texturePointer", nullptr},
        /* Second empty layer stays */
        {"againSomething", false},
    }, {3, 5, 5, 6}}), DebugTools::CompareMaterial);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MaterialAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());

    /* Removing all shouldn't do anything unexpected */
    CORRADE_COMPARE_AS(filterLayers(material, Containers::BitArray{ValueInit, 7}, {}), (Trade::MaterialData{{}, {
    }}), DebugTools::CompareMaterial);
}

void FilterTest::layersRemoveBase() {
    const Trade::MaterialAttributeData attributes[]{
        {Trade::MaterialAttribute::AlphaBlend, true},               /* 0 */
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::BaseColorTexture, 7u},
        {Trade::MaterialLayer::ClearCoat},                          /* 1 #3 */
        {Trade::MaterialAttribute::LayerFactor, 0.7f},
    };
    const UnsignedInt layers[]{3, 5};
    Trade::MaterialData material{Trade::MaterialType::PbrClearCoat, Trade::DataFlags{}, attributes, Trade::DataFlags{}, layers};

    Containers::BitArray layersToKeep{DirectInit, 2, true};
    layersToKeep.reset(0);

    Trade::MaterialData filtered = filterLayers(material, layersToKeep);

    /* The base layer stays but it's empty */
    CORRADE_COMPARE_AS(filtered, (Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {
        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::LayerFactor, 0.7f},
    }, {0, 2}}), DebugTools::CompareMaterial);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MaterialAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());
}

void FilterTest::layersWrongBitCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MaterialData material{{}, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::BaseColorTexture, 7u},
        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::LayerFactor, 0.7f}
    }, {2, 4}};
    Containers::BitArrayView layersToKeep{nullptr, 0, 3};

    Containers::String out;
    Error redirectError{&out};
    filterLayers(material, layersToKeep);
    CORRADE_COMPARE(out, "MaterialTools::filterLayers(): expected 2 bits but got 3\n");
}

void FilterTest::attributesLayers() {
    const Trade::MaterialAttributeData attributes[]{
        {Trade::MaterialAttribute::AlphaBlend, true},               /* 0 #0 */
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},    /*   #1 */
        {Trade::MaterialAttribute::BaseColorTexture, 7u},           /*   #2 */
        /* One empty layer here */                                  /* 1 #3 */
        {Trade::MaterialLayer::ClearCoat},                          /* 2 #3 */
        {Trade::MaterialAttribute::LayerFactor, 0.7f},              /*   #4 */
        {Trade::MaterialAttribute::Roughness, 0.25f},               /*   #5 */
        {"textureBlendMode", "strongly!"},                          /* 3 #6 */
        {"texturePointer", nullptr},                                /*   #7 */
        /* Another empty layer here */                              /* 4 #8 */
        {Trade::MaterialAttribute::NormalTextureSwizzle,            /* 5 #8 */
            Trade::MaterialTextureSwizzle::RG},
        {"againSomething", false},                                  /* 6 #9 */
    };
    const UnsignedInt layers[]{3, 3, 6, 8, 8, 9, 10};
    Trade::MaterialData material{Trade::MaterialType::PbrClearCoat|Trade::MaterialType::PbrMetallicRoughness, Trade::DataFlags{}, attributes, Trade::DataFlags{}, layers};

    Containers::BitArray attributesToKeep{DirectInit, 10, true};
    attributesToKeep.reset(1);
    attributesToKeep.reset(4); /* in a removed layer, ignored */
    attributesToKeep.reset(6);
    attributesToKeep.reset(8); /* becomes an empty layer */

    Containers::BitArray layersToKeep{DirectInit, 7, true};
    layersToKeep.reset(1);
    layersToKeep.reset(2);

    Trade::MaterialData filtered = filterAttributesLayers(material, attributesToKeep, layersToKeep, Trade::MaterialType::PbrMetallicRoughness|Trade::MaterialType::PbrSpecularGlossiness);

    CORRADE_COMPARE_AS(filtered, (Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColorTexture, 7u},
        {"texturePointer", nullptr},
        /* Second empty layer stays */
        /* Layer 5 is now empty */
        {"againSomething", false},
    }, {2, 3, 3, 3, 4}}), DebugTools::CompareMaterial);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MaterialAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());

    /* Removing all attributes should keep all layers but make them empty */
    CORRADE_COMPARE_AS(filterAttributesLayers(material, Containers::BitArray{ValueInit, 10}, Containers::BitArray{DirectInit, 7, true}, {}), (Trade::MaterialData{{}, {
    }, {0, 0, 0, 0, 0, 0, 0}}), DebugTools::CompareMaterial);

    /* Removing all layers should make the material completely empty */
    CORRADE_COMPARE_AS(filterAttributesLayers(material, Containers::BitArray{ValueInit, 10}, Containers::BitArray{ValueInit, 7}, {}), (Trade::MaterialData{{}, {
    }}), DebugTools::CompareMaterial);
}

void FilterTest::attributesLayersRemoveBaseLayer() {
    const Trade::MaterialAttributeData attributes[]{
        {Trade::MaterialAttribute::AlphaBlend, true},               /* 0 #0 */
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},    /*   #1 */
        {Trade::MaterialAttribute::BaseColorTexture, 7u},           /*   #2 */
        {Trade::MaterialLayer::ClearCoat},                          /* 1 #3 */
        {Trade::MaterialAttribute::LayerFactor, 0.7f},              /*   #4 */
    };
    const UnsignedInt layers[]{3, 5};
    Trade::MaterialData material{Trade::MaterialType::PbrClearCoat, Trade::DataFlags{}, attributes, Trade::DataFlags{}, layers};

    Containers::BitArray attributesToKeep{DirectInit, 5, true};
    attributesToKeep.reset(1); /* in a removed base layer, ignored */
    attributesToKeep.reset(3);

    Containers::BitArray layersToKeep{DirectInit, 2, true};
    layersToKeep.reset(0);

    Trade::MaterialData filtered = filterAttributesLayers(material, attributesToKeep, layersToKeep);

    CORRADE_COMPARE_AS(filtered, (Trade::MaterialData{Trade::MaterialType::PbrClearCoat, {
        /* The base layer stays but it's empty */
        {Trade::MaterialAttribute::LayerFactor, 0.7f},
    }, {0, 1}}), DebugTools::CompareMaterial);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MaterialAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());
}

void FilterTest::attributesLayersWrongBitCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MaterialData material{{}, {
        {Trade::MaterialAttribute::BaseColor, 0xffcc66ff_rgbaf},
        {Trade::MaterialAttribute::BaseColorTexture, 7u},
        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::LayerFactor, 0.7f},
    }, {2, 4}};
    Containers::BitArrayView attributesToKeep{nullptr, 0, 5};
    Containers::BitArrayView layersToKeep{nullptr, 0, 3};

    Containers::String out;
    Error redirectError{&out};
    filterAttributesLayers(material, attributesToKeep, layersToKeep.prefix(2));
    filterAttributesLayers(material, attributesToKeep.prefix(4), layersToKeep);
    CORRADE_COMPARE(out,
        "MaterialTools::filterAttributesLayers(): expected 4 attribute bits but got 5\n"
        "MaterialTools::filterAttributesLayers(): expected 2 layer bits but got 3\n");
}

void FilterTest::emptyInput() {
    Trade::MaterialData empty{Trade::MaterialType::PbrClearCoat, {}};

    /* We have no attributes but we always have one implicit layer */
    const bool yes[1]{};
    Containers::BitArrayView layersToKeep{yes, 0, 1};

    /* It shouldn't assert or do any other crazy thing */
    CORRADE_COMPARE_AS(filterAttributes(empty, nullptr),
        empty,
        DebugTools::CompareMaterial);
    CORRADE_COMPARE_AS(filterLayers(empty, layersToKeep),
        empty,
        DebugTools::CompareMaterial);
    CORRADE_COMPARE_AS(filterAttributesLayers(empty, nullptr, layersToKeep),
        empty,
        DebugTools::CompareMaterial);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MaterialTools::Test::FilterTest)
