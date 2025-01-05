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

#include "Magnum/Trade/PbrClearCoatMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct PbrClearCoatMaterialDataTest: TestSuite::Tester {
    explicit PbrClearCoatMaterialDataTest();

    void basics();
    void defaults();
    void textured();
    void texturedDefaults();
    void texturedExplicitPackedLayerFactorRoughness();
    void texturedSingleMatrixCoordinatesLayer();
    void texturedBaseMaterialMatrixCoordinatesLayer();
    void invalidTextures();
    void commonTransformationCoordinatesLayerNoTextures();
    void commonTransformationCoordinatesLayerOneTexture();
    void commonTransformationCoordinatesLayerOneDifferentTexture();
    void commonCoordinatesLayerImplicit();
    void noCommonTransformationCoordinatesLayer();
};

const Containers::StringView PbrClearCoatTextureData[] {
    "LayerFactorTexture",
    "RoughnessTexture",
    "NormalTexture"
};

PbrClearCoatMaterialDataTest::PbrClearCoatMaterialDataTest() {
    addTests({&PbrClearCoatMaterialDataTest::basics,
              &PbrClearCoatMaterialDataTest::defaults,
              &PbrClearCoatMaterialDataTest::textured,
              &PbrClearCoatMaterialDataTest::texturedDefaults,
              &PbrClearCoatMaterialDataTest::texturedExplicitPackedLayerFactorRoughness,
              &PbrClearCoatMaterialDataTest::texturedSingleMatrixCoordinatesLayer,
              &PbrClearCoatMaterialDataTest::texturedBaseMaterialMatrixCoordinatesLayer,
              &PbrClearCoatMaterialDataTest::invalidTextures,
              &PbrClearCoatMaterialDataTest::commonTransformationCoordinatesLayerNoTextures});

    addInstancedTests({
        &PbrClearCoatMaterialDataTest::commonTransformationCoordinatesLayerOneTexture,
        &PbrClearCoatMaterialDataTest::commonTransformationCoordinatesLayerOneDifferentTexture,
        &PbrClearCoatMaterialDataTest::commonCoordinatesLayerImplicit},
        Containers::arraySize(PbrClearCoatTextureData));

    addTests({&PbrClearCoatMaterialDataTest::noCommonTransformationCoordinatesLayer});
}

using namespace Math::Literals;

void PbrClearCoatMaterialDataTest::basics() {
    MaterialData base{MaterialType::PbrClearCoat, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::Roughness, 0.7f}
    }, {0, 2}};

    CORRADE_COMPARE(base.types(), MaterialType::PbrClearCoat);
    const auto& data = base.as<PbrClearCoatMaterialData>();

    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_COMPARE(data.roughness(), 0.7f);
}

void PbrClearCoatMaterialDataTest::defaults() {
    MaterialData base{{}, {
        /* Needs to have at least the layer name, otherwise the queries will
           blow up */
        {MaterialLayer::ClearCoat}
    }, {0, 1}};

    CORRADE_COMPARE(base.types(), MaterialTypes{});
    const auto& data = base.as<PbrClearCoatMaterialData>();

    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_COMPARE(data.layerFactor(), 1.0f);
    CORRADE_COMPARE(data.roughness(), 0.0f);
}

void PbrClearCoatMaterialDataTest::textured() {
    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::Roughness, 0.7f},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::A},
        {MaterialAttribute::RoughnessTextureMatrix, Matrix3::translation({2.0f, 1.5f})},
        {MaterialAttribute::RoughnessTextureCoordinates, 6u},
        {MaterialAttribute::RoughnessTextureLayer, 17u},
        {MaterialAttribute::NormalTexture, 3u},
        {MaterialAttribute::NormalTextureScale, 0.5f},
        {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::B},
        {MaterialAttribute::NormalTextureMatrix, Matrix3::translation({0.0f, 0.5f})},
        {MaterialAttribute::NormalTextureCoordinates, 7u},
        {MaterialAttribute::NormalTextureLayer, 66u},
    }, {0, 13}};

    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_VERIFY(data.hasTextureLayer());
    CORRADE_COMPARE(data.roughness(), 0.7f);
    CORRADE_COMPARE(data.roughnessTexture(), 2u);
    CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::A);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::translation({2.0f, 1.5f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 6u);
    CORRADE_COMPARE(data.roughnessTextureLayer(), 17u);
    CORRADE_COMPARE(data.normalTexture(), 3u);
    CORRADE_COMPARE(data.normalTextureScale(), 0.5f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::B);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7u);
    CORRADE_COMPARE(data.normalTextureLayer(), 66u);
}

void PbrClearCoatMaterialDataTest::texturedDefaults() {
    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::NormalTexture, 3u},
    }, {0, 3}};

    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_COMPARE(data.roughness(), 0.0f);
    CORRADE_COMPARE(data.roughnessTexture(), 2u);
    CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 0u);
    CORRADE_COMPARE(data.roughnessTextureLayer(), 0u);
    CORRADE_COMPARE(data.normalTexture(), 3u);
    CORRADE_COMPARE(data.normalTextureScale(), 1.0f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.normalTextureCoordinates(), 0u);
    CORRADE_COMPARE(data.normalTextureLayer(), 0u);
}

void PbrClearCoatMaterialDataTest::texturedExplicitPackedLayerFactorRoughness() {
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
        CORRADE_COMPARE(data.roughnessTextureLayer(), 0);

    /* Explicit parameters for everything, but all the same */
    } {
        PbrClearCoatMaterialData data{{}, {
            {MaterialLayer::ClearCoat},
            {MaterialAttribute::LayerFactorTexture, 2u},
            {MaterialAttribute::LayerFactorTextureSwizzle, MaterialTextureSwizzle::R},
            {MaterialAttribute::LayerFactorTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::LayerFactorTextureCoordinates, 3u},
            {MaterialAttribute::LayerFactorTextureLayer, 17u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::RoughnessTextureLayer, 17u}
        }, {0, 11}};
        CORRADE_VERIFY(data.hasLayerFactorRoughnessTexture());
        CORRADE_COMPARE(data.layerFactorTexture(), 2);
        CORRADE_COMPARE(data.layerFactorTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.layerFactorTextureCoordinates(), 3);
        CORRADE_COMPARE(data.layerFactorTextureLayer(), 17);
        CORRADE_COMPARE(data.roughnessTexture(), 2);
        CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::G);
        CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.roughnessTextureCoordinates(), 3);
        CORRADE_COMPARE(data.roughnessTextureLayer(), 17);

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

    /* Unexpected array texture layer */
    } {
        PbrClearCoatMaterialData data{{}, {
            {MaterialLayer::ClearCoat},
            {MaterialAttribute::LayerFactorTexture, 2u},
            {MaterialAttribute::LayerFactorTextureLayer, 1u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
        }, {0, 5}};
        CORRADE_VERIFY(!data.hasLayerFactorRoughnessTexture());
    }
}

void PbrClearCoatMaterialDataTest::texturedSingleMatrixCoordinatesLayer() {
    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::NormalTexture, 3u},
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.0f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},
        {MaterialAttribute::TextureLayer, 17u},
    }, {0, 6}};

    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_VERIFY(data.hasTextureLayer());
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 7u);
    CORRADE_COMPARE(data.roughnessTextureLayer(), 17u);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7u);
    CORRADE_COMPARE(data.normalTextureLayer(), 17u);
}

void PbrClearCoatMaterialDataTest::texturedBaseMaterialMatrixCoordinatesLayer() {
    PbrClearCoatMaterialData data{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.0f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},
        {MaterialAttribute::TextureLayer, 17u},

        {MaterialLayer::ClearCoat},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::NormalTexture, 3u},
    }, {3, 6}};

    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_VERIFY(data.hasTextureLayer());
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 7u);
    CORRADE_COMPARE(data.roughnessTextureLayer(), 17u);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7u);
    CORRADE_COMPARE(data.normalTextureLayer(), 17u);

    CORRADE_VERIFY(data.hasCommonTextureTransformation());
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_VERIFY(data.hasCommonTextureLayer());
    CORRADE_COMPARE(data.commonTextureMatrix(), Matrix3::translation({0.0f, 0.5f}));
    CORRADE_COMPARE(data.commonTextureCoordinates(), 7);
    CORRADE_COMPARE(data.commonTextureLayer(), 17);
}

void PbrClearCoatMaterialDataTest::invalidTextures() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
    }, {0, 1}};

    Containers::String out;
    Error redirectError{&out};
    data.roughnessTexture();
    data.roughnessTextureSwizzle();
    data.roughnessTextureMatrix();
    data.roughnessTextureCoordinates();
    data.roughnessTextureLayer();
    data.normalTexture();
    data.normalTextureScale();
    data.normalTextureSwizzle();
    data.normalTextureMatrix();
    data.normalTextureCoordinates();
    data.normalTextureLayer();
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attribute(): attribute RoughnessTexture not found in layer ClearCoat\n"
        "Trade::PbrClearCoatMaterialData::roughnessTextureSwizzle(): the layer doesn't have a roughness texture\n"
        "Trade::PbrClearCoatMaterialData::roughnessTextureMatrix(): the layer doesn't have a roughness texture\n"
        "Trade::PbrClearCoatMaterialData::roughnessTextureCoordinates(): the layer doesn't have a roughness texture\n"
        "Trade::PbrClearCoatMaterialData::roughnessTextureLayer(): the layer doesn't have a roughness texture\n"
        "Trade::MaterialData::attribute(): attribute NormalTexture not found in layer ClearCoat\n"
        "Trade::PbrClearCoatMaterialData::normalTextureScale(): the layer doesn't have a normal texture\n"
        "Trade::PbrClearCoatMaterialData::normalTextureSwizzle(): the layer doesn't have a normal texture\n"
        "Trade::PbrClearCoatMaterialData::normalTextureMatrix(): the layer doesn't have a normal texture\n"
        "Trade::PbrClearCoatMaterialData::normalTextureCoordinates(): the layer doesn't have a normal texture\n"
        "Trade::PbrClearCoatMaterialData::normalTextureLayer(): the layer doesn't have a normal texture\n");
}

void PbrClearCoatMaterialDataTest::commonTransformationCoordinatesLayerNoTextures() {
    PbrClearCoatMaterialData a{{}, {
        {MaterialLayer::ClearCoat},
    }, {0, 1}};
    CORRADE_VERIFY(a.hasCommonTextureTransformation());
    CORRADE_VERIFY(a.hasCommonTextureCoordinates());
    CORRADE_VERIFY(a.hasCommonTextureLayer());
    CORRADE_COMPARE(a.commonTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(a.commonTextureCoordinates(), 0);
    CORRADE_COMPARE(a.commonTextureLayer(), 0);

    PbrClearCoatMaterialData b{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},
        {MaterialAttribute::TextureLayer, 17u},

        {MaterialLayer::ClearCoat}
    }, {3, 4}};
    CORRADE_VERIFY(b.hasCommonTextureTransformation());
    CORRADE_VERIFY(b.hasCommonTextureCoordinates());
    CORRADE_VERIFY(b.hasCommonTextureLayer());
    CORRADE_COMPARE(b.commonTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(b.commonTextureCoordinates(), 7);
    CORRADE_COMPARE(b.commonTextureLayer(), 17);

    PbrClearCoatMaterialData c{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},
        {MaterialAttribute::TextureLayer, 17u},
    }, {0, 4}};
    CORRADE_VERIFY(c.hasCommonTextureTransformation());
    CORRADE_VERIFY(c.hasCommonTextureCoordinates());
    CORRADE_VERIFY(c.hasCommonTextureLayer());
    CORRADE_COMPARE(c.commonTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(c.commonTextureCoordinates(), 7);
    CORRADE_COMPARE(c.commonTextureLayer(), 17);
}

void PbrClearCoatMaterialDataTest::commonTransformationCoordinatesLayerOneTexture() {
    Containers::StringView textureName = PbrClearCoatTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrClearCoatMaterialData data{{}, {
        /* These shouldn't affect the below */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u},
        {MaterialAttribute::TextureLayer, 22u},

        {MaterialLayer::ClearCoat},
        {textureName, 5u},
        {textureName + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {textureName + "Coordinates", 17u},
        {textureName + "Layer", 66u},
    }, {3, 8}};

    CORRADE_VERIFY(data.hasCommonTextureTransformation());
    CORRADE_COMPARE(data.commonTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_COMPARE(data.commonTextureCoordinates(), 17u);
    CORRADE_VERIFY(data.hasCommonTextureLayer());
    CORRADE_COMPARE(data.commonTextureLayer(), 66u);
}

void PbrClearCoatMaterialDataTest::commonTransformationCoordinatesLayerOneDifferentTexture() {
    Containers::StringView textureName = PbrClearCoatTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrClearCoatMaterialData data{{}, {
        /* These are used by all textures except the one below, failing the
           check */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u},
        {MaterialAttribute::TextureLayer, 22u},

        {MaterialLayer::ClearCoat},
        {MaterialAttribute::LayerFactorTexture, 2u},
        {MaterialAttribute::RoughnessTexture, 3u},
        {MaterialAttribute::NormalTexture, 5u},
        {textureName + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {textureName + "Coordinates", 17u},
        {textureName + "Layer", 66u},
    }, {3, 10}};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());
    CORRADE_VERIFY(!data.hasCommonTextureLayer());
}

void PbrClearCoatMaterialDataTest::commonCoordinatesLayerImplicit() {
    Containers::StringView textureName = PbrClearCoatTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    /* The transformation doesn't have this behavior, because there checking an
       identity is rather expensive */

    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {textureName, 5u},
        {textureName + "Coordinates", 0u},
        {textureName + "Layer", 0u},
    }, {0, 4}};

    /* Zero is treated same as if there would be no attribute at all */
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_VERIFY(data.hasCommonTextureLayer());
    CORRADE_COMPARE(data.commonTextureCoordinates(), 0u);
    CORRADE_COMPARE(data.commonTextureLayer(), 0u);
}

void PbrClearCoatMaterialDataTest::noCommonTransformationCoordinatesLayer() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PbrClearCoatMaterialData data{{}, {
        {MaterialLayer::ClearCoat},
        {MaterialAttribute::LayerFactorTexture, 3u},
        {MaterialAttribute::LayerFactorTextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::LayerFactorTextureCoordinates, 3u},
        {MaterialAttribute::RoughnessTexture, 4u},
        {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::RoughnessTextureLayer, 22u},
        {MaterialAttribute::NormalTexture, 5u},
        {MaterialAttribute::NormalTextureCoordinates, 17u}
    }, {0, 9}};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());

    Containers::String out;
    Error redirectError{&out};
    data.commonTextureMatrix();
    data.commonTextureCoordinates();
    data.commonTextureLayer();
    CORRADE_COMPARE(out,
        "Trade::PbrClearCoatMaterialData::commonTextureMatrix(): the layer doesn't have a common texture coordinate transformation\n"
        "Trade::PbrClearCoatMaterialData::commonTextureCoordinates(): the layer doesn't have a common texture coordinate set\n"
        "Trade::PbrClearCoatMaterialData::commonTextureLayer(): the layer doesn't have a common array texture layer\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::PbrClearCoatMaterialDataTest)
