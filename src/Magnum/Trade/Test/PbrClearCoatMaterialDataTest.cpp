/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include <sstream>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Trade/PbrClearCoatMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class PbrClearCoatMaterialDataTest: public TestSuite::Tester {
    public:
        explicit PbrClearCoatMaterialDataTest();

        void basics();
        void defaults();
        void textured();
        void texturedDefaults();
        void texturedExplicitPackedLayerFactorRoughness();
        void texturedSingleMatrixCoordinates();
        void texturedBaseMaterialMatrixCoordinates();
        void invalidTextures();
        void commonTransformationCoordinatesNoTextures();
        void commonTransformationCoordinatesOneTexture();
        void commonTransformationCoordinatesOneDifferentTexture();
        void noCommonTransformationCoordinates();
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
              &PbrClearCoatMaterialDataTest::texturedSingleMatrixCoordinates,
              &PbrClearCoatMaterialDataTest::texturedBaseMaterialMatrixCoordinates,
              &PbrClearCoatMaterialDataTest::invalidTextures,
              &PbrClearCoatMaterialDataTest::commonTransformationCoordinatesNoTextures});

    addInstancedTests({
        &PbrClearCoatMaterialDataTest::commonTransformationCoordinatesOneTexture,
        &PbrClearCoatMaterialDataTest::commonTransformationCoordinatesOneDifferentTexture},
        Containers::arraySize(PbrClearCoatTextureData));

    addTests({&PbrClearCoatMaterialDataTest::noCommonTransformationCoordinates});
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

void PbrClearCoatMaterialDataTest::texturedDefaults() {
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

void PbrClearCoatMaterialDataTest::texturedSingleMatrixCoordinates() {
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

void PbrClearCoatMaterialDataTest::texturedBaseMaterialMatrixCoordinates() {
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

void PbrClearCoatMaterialDataTest::invalidTextures() {
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

void PbrClearCoatMaterialDataTest::commonTransformationCoordinatesNoTextures() {
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

void PbrClearCoatMaterialDataTest::commonTransformationCoordinatesOneTexture() {
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

void PbrClearCoatMaterialDataTest::commonTransformationCoordinatesOneDifferentTexture() {
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

void PbrClearCoatMaterialDataTest::noCommonTransformationCoordinates() {
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

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::PbrClearCoatMaterialDataTest)
