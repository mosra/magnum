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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/MaterialTools/Copy.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools { namespace Test { namespace {

struct CopyTest: TestSuite::Tester {
    explicit CopyTest();

    void singleLayer();
    void singleLayerNoLayerData();

    void multipleLayers();

    void rvalueNotOwned();
    void rvalueOwnedAttributesLayers();
    /* There's currently no constructor (and no use case) that would need owned
       attributes but not layers or vice versa, it's either all or none */
    void rvalueOwnedAttributesNoLayerData();
};

CopyTest::CopyTest() {
    addTests({&CopyTest::singleLayer,
              &CopyTest::singleLayerNoLayerData,

              &CopyTest::multipleLayers,

              &CopyTest::rvalueNotOwned,
              &CopyTest::rvalueOwnedAttributesLayers,
              &CopyTest::rvalueOwnedAttributesNoLayerData});
}

using namespace Math::Literals;

void CopyTest::singleLayer() {
    Trade::MaterialData material{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColor, 0xff3366ff_rgbaf},
        {"baseColorTextureName", "yesss.png"},
    }, {3}, reinterpret_cast<void*>(std::size_t{0xdeadbeef})};

    Trade::MaterialData copy = MaterialTools::copy(material);
    CORRADE_COMPARE(copy.types(), Trade::MaterialType::PbrMetallicRoughness);
    CORRADE_COMPARE(copy.importerState(), reinterpret_cast<void*>(std::size_t{0xdeadbeef}));

    CORRADE_COMPARE(copy.layerDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.layerCount(), 1);
    CORRADE_COMPARE(copy.layerData().size(), 1);
    CORRADE_COMPARE(copy.layerData()[0], 3);

    CORRADE_COMPARE(copy.attributeDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.attributeCount(), 3);
    CORRADE_COMPARE(copy.attribute<bool>(Trade::MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(copy.attribute<Color4>(Trade::MaterialAttribute::BaseColor), 0xff3366ff_rgbaf);
    CORRADE_COMPARE(copy.attribute<Containers::StringView>("baseColorTextureName"), "yesss.png");

    /* The data should have a default deleter to make this usable in plugins */
    Containers::Array<Trade::MaterialAttributeData> attributeData = copy.releaseAttributeData();
    Containers::Array<UnsignedInt> layerData = copy.releaseLayerData();
    CORRADE_VERIFY(!attributeData.deleter());
    CORRADE_VERIFY(!layerData.deleter());
}

void CopyTest::singleLayerNoLayerData() {
    Trade::MaterialData material{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColor, 0xff3366ff_rgbaf},
        {"baseColorTextureName", "yesss.png"},
    }, reinterpret_cast<void*>(std::size_t{0xdeadbeef})};

    Trade::MaterialData copy = MaterialTools::copy(material);
    CORRADE_COMPARE(copy.types(), Trade::MaterialType::PbrMetallicRoughness);
    CORRADE_COMPARE(copy.importerState(), reinterpret_cast<void*>(std::size_t{0xdeadbeef}));

    CORRADE_COMPARE(copy.layerDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.layerCount(), 1);
    CORRADE_COMPARE(copy.layerData().size(), 0);
    CORRADE_COMPARE(copy.layerData(), nullptr);

    CORRADE_COMPARE(copy.attributeDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.attributeCount(), 3);
    CORRADE_COMPARE(copy.attribute<bool>(Trade::MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(copy.attribute<Color4>(Trade::MaterialAttribute::BaseColor), 0xff3366ff_rgbaf);
    CORRADE_COMPARE(copy.attribute<Containers::StringView>("baseColorTextureName"), "yesss.png");
}

void CopyTest::multipleLayers() {
    Trade::MaterialData material{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColor, 0xff3366ff_rgbaf},
        {"baseColorTextureName", "yesss.png"},
    }, {2, 3}, reinterpret_cast<void*>(std::size_t{0xdeadbeef})};

    Trade::MaterialData copy = MaterialTools::copy(material);
    CORRADE_COMPARE(copy.types(), Trade::MaterialType::PbrMetallicRoughness);
    CORRADE_COMPARE(copy.importerState(), reinterpret_cast<void*>(std::size_t{0xdeadbeef}));

    CORRADE_COMPARE(copy.layerDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.layerCount(), 2);

    CORRADE_COMPARE(copy.attributeDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.attributeCount(0), 2);
    CORRADE_COMPARE(copy.attributeCount(1), 1);
    CORRADE_COMPARE(copy.attribute<bool>(0, Trade::MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(copy.attribute<Color4>(0, Trade::MaterialAttribute::BaseColor), 0xff3366ff_rgbaf);
    CORRADE_COMPARE(copy.attribute<Containers::StringView>(1, "baseColorTextureName"), "yesss.png");

    /* The data should have a default deleter to make this usable in plugins */
    Containers::Array<Trade::MaterialAttributeData> attributeData = copy.releaseAttributeData();
    Containers::Array<UnsignedInt> layerData = copy.releaseLayerData();
    CORRADE_VERIFY(!attributeData.deleter());
    CORRADE_VERIFY(!layerData.deleter());
}

void CopyTest::rvalueNotOwned() {
    Trade::MaterialAttributeData attributes[]{
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColor, 0xff3366ff_rgbaf},
        {"baseColorTextureName", "yesss.png"},
    };
    UnsignedInt layers[]{2, 3};

    Trade::MaterialData copy = MaterialTools::copy(Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness,
        Trade::DataFlag::Mutable, attributes,
        Trade::DataFlag::Mutable|Trade::DataFlag::ExternallyOwned, layers,
        reinterpret_cast<void*>(std::size_t{0xdeadbeef})});
    CORRADE_COMPARE(copy.types(), Trade::MaterialType::PbrMetallicRoughness);
    CORRADE_COMPARE(copy.importerState(), reinterpret_cast<void*>(std::size_t{0xdeadbeef}));

    CORRADE_COMPARE(copy.layerDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.layerCount(), 2);

    CORRADE_COMPARE(copy.attributeDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.attributeCount(0), 2);
    CORRADE_COMPARE(copy.attributeCount(1), 1);
    CORRADE_COMPARE(copy.attribute<bool>(0, Trade::MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(copy.attribute<Color4>(0, Trade::MaterialAttribute::BaseColor), 0xff3366ff_rgbaf);
    CORRADE_COMPARE(copy.attribute<Containers::StringView>(1, "baseColorTextureName"), "yesss.png");
}

void CopyTest::rvalueOwnedAttributesLayers() {
    Containers::Array<Trade::MaterialAttributeData> attributes{InPlaceInit, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColor, 0xff3366ff_rgbaf},
        {"baseColorTextureName", "yesss.png"},
    }};
    Containers::Array<UnsignedInt> layers{InPlaceInit, {2, 3}};
    const void* originalAttributes = attributes.data();
    const void* originalLayers = layers.data();

    Trade::MaterialData copy = MaterialTools::copy(Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, Utility::move(attributes), Utility::move(layers), reinterpret_cast<void*>(std::size_t{0xdeadbeef})});
    CORRADE_COMPARE(copy.types(), Trade::MaterialType::PbrMetallicRoughness);
    CORRADE_COMPARE(copy.importerState(), reinterpret_cast<void*>(std::size_t{0xdeadbeef}));

    CORRADE_COMPARE(copy.layerDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.layerData().data(), originalLayers);
    CORRADE_COMPARE(copy.layerCount(), 2);

    CORRADE_COMPARE(copy.attributeDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.attributeData().data(), originalAttributes);
    CORRADE_COMPARE(copy.attributeCount(0), 2);
    CORRADE_COMPARE(copy.attributeCount(1), 1);
    CORRADE_COMPARE(copy.attribute<bool>(0, Trade::MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(copy.attribute<Color4>(0, Trade::MaterialAttribute::BaseColor), 0xff3366ff_rgbaf);
    CORRADE_COMPARE(copy.attribute<Containers::StringView>(1, "baseColorTextureName"), "yesss.png");
}

void CopyTest::rvalueOwnedAttributesNoLayerData() {
    Containers::Array<Trade::MaterialAttributeData> attributes{InPlaceInit, {
        {Trade::MaterialAttribute::AlphaBlend, true},
        {Trade::MaterialAttribute::BaseColor, 0xff3366ff_rgbaf},
        {"baseColorTextureName", "yesss.png"},
    }};
    Containers::Array<UnsignedInt> layers{InPlaceInit, {2, 3}};
    const void* originalAttributes = attributes.data();

    Trade::MaterialData copy = MaterialTools::copy(Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, Utility::move(attributes), reinterpret_cast<void*>(std::size_t{0xdeadbeef})});
    CORRADE_COMPARE(copy.types(), Trade::MaterialType::PbrMetallicRoughness);
    CORRADE_COMPARE(copy.importerState(), reinterpret_cast<void*>(std::size_t{0xdeadbeef}));

    CORRADE_COMPARE(copy.layerDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.layerCount(), 1);
    CORRADE_COMPARE(copy.layerData().size(), 0);
    CORRADE_COMPARE(copy.layerData(), nullptr);

    CORRADE_COMPARE(copy.attributeDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.attributeData().data(), originalAttributes);
    CORRADE_COMPARE(copy.attributeCount(), 3);
    CORRADE_COMPARE(copy.attribute<bool>(Trade::MaterialAttribute::AlphaBlend), true);
    CORRADE_COMPARE(copy.attribute<Color4>(Trade::MaterialAttribute::BaseColor), 0xff3366ff_rgbaf);
    CORRADE_COMPARE(copy.attribute<Containers::StringView>("baseColorTextureName"), "yesss.png");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MaterialTools::Test::CopyTest)
