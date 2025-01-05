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

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/PbrSpecularGlossinessMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct PbrSpecularGlossinessMaterialDataTest: TestSuite::Tester {
    explicit PbrSpecularGlossinessMaterialDataTest();

    void basics();
    void defaults();
    void textured();
    void texturedDefaults();
    void texturedImplicitPackedSpecularGlossiness();
    void texturedExplicitPackedSpecularGlossiness();
    void texturedSingleMatrixCoordinatesLayer();
    void invalidTextures();
    void commonTransformationCoordinatesLayerNoTextures();
    void commonTransformationCoordinatesLayerOneTexture();
    void commonTransformationCoordinatesLayerOneDifferentTexture();
    void commonCoordinatesLayerImplicit();
    void noCommonTransformationCoordinatesLayer();
};

const Containers::StringView PbrSpecularGlossinessTextureData[] {
    "DiffuseTexture",
    "SpecularTexture",
    "GlossinessTexture",
    "NormalTexture",
    "OcclusionTexture",
    "EmissiveTexture"
};

PbrSpecularGlossinessMaterialDataTest::PbrSpecularGlossinessMaterialDataTest() {
    addTests({&PbrSpecularGlossinessMaterialDataTest::basics,
              &PbrSpecularGlossinessMaterialDataTest::defaults,
              &PbrSpecularGlossinessMaterialDataTest::textured,
              &PbrSpecularGlossinessMaterialDataTest::texturedDefaults,
              &PbrSpecularGlossinessMaterialDataTest::texturedImplicitPackedSpecularGlossiness,
              &PbrSpecularGlossinessMaterialDataTest::texturedExplicitPackedSpecularGlossiness,
              &PbrSpecularGlossinessMaterialDataTest::texturedSingleMatrixCoordinatesLayer,
              &PbrSpecularGlossinessMaterialDataTest::invalidTextures,
              &PbrSpecularGlossinessMaterialDataTest::commonTransformationCoordinatesLayerNoTextures});

    addInstancedTests({
        &PbrSpecularGlossinessMaterialDataTest::commonTransformationCoordinatesLayerOneTexture,
        &PbrSpecularGlossinessMaterialDataTest::commonTransformationCoordinatesLayerOneDifferentTexture,
        &PbrSpecularGlossinessMaterialDataTest::commonCoordinatesLayerImplicit},
        Containers::arraySize(PbrSpecularGlossinessTextureData));

    addTests({&PbrSpecularGlossinessMaterialDataTest::noCommonTransformationCoordinatesLayer});
}

using namespace Math::Literals;

void PbrSpecularGlossinessMaterialDataTest::basics() {
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
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_COMPARE(data.diffuseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(data.specularColor(), 0xff336600_rgbaf);
    CORRADE_COMPARE(data.glossiness(), 0.79f);
}

void PbrSpecularGlossinessMaterialDataTest::defaults() {
    MaterialData base{{}, {}};

    CORRADE_COMPARE(base.types(), MaterialTypes{});
    /* Casting is fine even if the type doesn't include PbrMetallicRoughness */
    const auto& data = base.as<PbrSpecularGlossinessMaterialData>();

    CORRADE_VERIFY(!data.hasSpecularTexture());
    CORRADE_VERIFY(!data.hasGlossinessTexture());
    CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_COMPARE(data.diffuseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.specularColor(), 0xffffff00_rgbaf);
    CORRADE_COMPARE(data.glossiness(), 1.0f);
}

void PbrSpecularGlossinessMaterialDataTest::textured() {
    PbrSpecularGlossinessMaterialData data{{}, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseTexture, 0u},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 2u},
        {MaterialAttribute::DiffuseTextureLayer, 8u},
        {MaterialAttribute::SpecularColor, 0x33556600_rgbaf},
        {MaterialAttribute::SpecularTexture, 1u},
        {MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGBA},
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::SpecularTextureCoordinates, 3u},
        {MaterialAttribute::SpecularTextureLayer, 9u},
        {MaterialAttribute::Glossiness, 0.79f},
        {MaterialAttribute::GlossinessTexture, 2u},
        {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A},
        {MaterialAttribute::GlossinessTextureMatrix, Matrix3::scaling({1.0f, 1.0f})},
        {MaterialAttribute::GlossinessTextureCoordinates, 4u},
        {MaterialAttribute::GlossinessTextureLayer, 10u},
        {MaterialAttribute::NormalTexture, 3u},
        {MaterialAttribute::NormalTextureScale, 0.35f},
        {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::BA},
        {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({1.0f, 0.5f})},
        {MaterialAttribute::NormalTextureCoordinates, 5u},
        {MaterialAttribute::NormalTextureLayer, 11u},
        {MaterialAttribute::OcclusionTexture, 4u},
        {MaterialAttribute::OcclusionTextureStrength, 0.66f},
        {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({1.0f, 0.75f})},
        {MaterialAttribute::OcclusionTextureCoordinates, 6u},
        {MaterialAttribute::OcclusionTextureLayer, 12u},
        {MaterialAttribute::EmissiveColor, 0x111111_rgbf},
        {MaterialAttribute::EmissiveTexture, 5u},
        {MaterialAttribute::EmissiveTextureMatrix, Matrix3::scaling({0.75f, 0.5f})},
        {MaterialAttribute::EmissiveTextureCoordinates, 7u},
        {MaterialAttribute::EmissiveTextureLayer, 13u},
    }};

    CORRADE_VERIFY(data.hasSpecularTexture());
    CORRADE_VERIFY(data.hasGlossinessTexture());
    CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_VERIFY(data.hasTextureLayer());
    CORRADE_COMPARE(data.diffuseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(data.diffuseTexture(), 0);
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 2);
    CORRADE_COMPARE(data.diffuseTextureLayer(), 8);
    CORRADE_COMPARE(data.specularColor(), 0x33556600_rgbaf);
    CORRADE_COMPARE(data.specularTexture(), 1);
    CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGBA);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 3);
    CORRADE_COMPARE(data.specularTextureLayer(), 9);
    CORRADE_COMPARE(data.glossiness(), 0.79f);
    CORRADE_COMPARE(data.glossinessTexture(), 2);
    CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::A);
    CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3::scaling({1.0f, 1.0f}));
    CORRADE_COMPARE(data.glossinessTextureCoordinates(), 4);
    CORRADE_COMPARE(data.glossinessTextureLayer(), 10);
    CORRADE_COMPARE(data.normalTexture(), 3);
    CORRADE_COMPARE(data.normalTextureScale(), 0.35f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::BA);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({1.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 5);
    CORRADE_COMPARE(data.normalTextureLayer(), 11);
    CORRADE_COMPARE(data.occlusionTexture(), 4);
    CORRADE_COMPARE(data.occlusionTextureStrength(), 0.66f);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3::scaling({1.0f, 0.75f}));
    CORRADE_COMPARE(data.occlusionTextureSwizzle(), MaterialTextureSwizzle::B);
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 6);
    CORRADE_COMPARE(data.occlusionTextureLayer(), 12);
    CORRADE_COMPARE(data.emissiveColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3::scaling({0.75f, 0.5f}));
    CORRADE_COMPARE(data.emissiveTexture(), 5);
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 7);
    CORRADE_COMPARE(data.emissiveTextureLayer(), 13);
}

void PbrSpecularGlossinessMaterialDataTest::texturedDefaults() {
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
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_COMPARE(data.diffuseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.diffuseTexture(), 1);
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 0);
    CORRADE_COMPARE(data.diffuseTextureLayer(), 0);
    CORRADE_COMPARE(data.specularColor(), 0xffffff00_rgbaf);
    CORRADE_COMPARE(data.specularTexture(), 2);
    CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.specularTextureCoordinates(), 0);
    CORRADE_COMPARE(data.specularTextureLayer(), 0);
    CORRADE_COMPARE(data.glossiness(), 1.0f);
    CORRADE_COMPARE(data.glossinessTexture(), 3);
    CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.glossinessTextureCoordinates(), 0);
    CORRADE_COMPARE(data.glossinessTextureLayer(), 0);
    CORRADE_COMPARE(data.normalTexture(), 4);
    CORRADE_COMPARE(data.normalTextureScale(), 1.0f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.normalTextureCoordinates(), 0);
    CORRADE_COMPARE(data.normalTextureLayer(), 0);
    CORRADE_COMPARE(data.occlusionTexture(), 5);
    CORRADE_COMPARE(data.occlusionTextureStrength(), 1.0f);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.occlusionTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 0);
    CORRADE_COMPARE(data.occlusionTextureLayer(), 0);
    CORRADE_COMPARE(data.emissiveColor(), 0x000000_rgbf);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.emissiveTexture(), 6);
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 0);
    CORRADE_COMPARE(data.emissiveTextureLayer(), 0);
}

void PbrSpecularGlossinessMaterialDataTest::texturedSingleMatrixCoordinatesLayer() {
    PbrSpecularGlossinessMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 1u},
        {MaterialAttribute::SpecularTexture, 2u},
        {MaterialAttribute::GlossinessTexture, 3u},
        {MaterialAttribute::NormalTexture, 4u},
        {MaterialAttribute::OcclusionTexture, 5u},
        {MaterialAttribute::EmissiveTexture, 6u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},
        {MaterialAttribute::TextureLayer, 22u},
    }};

    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_VERIFY(data.hasTextureLayer());
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 7);
    CORRADE_COMPARE(data.diffuseTextureLayer(), 22);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 7);
    CORRADE_COMPARE(data.specularTextureLayer(), 22);
    CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.glossinessTextureCoordinates(), 7);
    CORRADE_COMPARE(data.glossinessTextureLayer(), 22);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7);
    CORRADE_COMPARE(data.normalTextureLayer(), 22);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 7);
    CORRADE_COMPARE(data.occlusionTextureLayer(), 22);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 7);
    CORRADE_COMPARE(data.emissiveTextureLayer(), 22);
}

void PbrSpecularGlossinessMaterialDataTest::texturedImplicitPackedSpecularGlossiness() {
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
        CORRADE_COMPARE(data.specularTextureLayer(), 0);
        CORRADE_COMPARE(data.glossinessTexture(), 2);
        CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::A);
        CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.glossinessTextureCoordinates(), 0);
        CORRADE_COMPARE(data.glossinessTextureLayer(), 0);

    /* Explicit parameters for everything, but all the same */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularGlossinessTexture, 2u},
            {MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGB},
            {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::SpecularTextureCoordinates, 3u},
            {MaterialAttribute::SpecularTextureLayer, 17u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::GlossinessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::GlossinessTextureCoordinates, 3u},
            {MaterialAttribute::GlossinessTextureLayer, 17u}
        }};
        CORRADE_VERIFY(data.hasSpecularGlossinessTexture());
        CORRADE_COMPARE(data.specularTexture(), 2);
        CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
        CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.specularTextureCoordinates(), 3);
        CORRADE_COMPARE(data.specularTextureLayer(), 17);
        CORRADE_COMPARE(data.glossinessTexture(), 2);
        CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::A);
        CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.glossinessTextureCoordinates(), 3);
        CORRADE_COMPARE(data.glossinessTextureLayer(), 17);

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

    /* Unexpected array texture layer */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularGlossinessTexture, 2u},
            {MaterialAttribute::SpecularTextureLayer, 1u},
        }};
        CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());
    }
}

void PbrSpecularGlossinessMaterialDataTest::texturedExplicitPackedSpecularGlossiness() {
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
        CORRADE_COMPARE(data.specularTextureLayer(), 0);
        CORRADE_COMPARE(data.glossinessTexture(), 2);
        CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::A);
        CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.glossinessTextureCoordinates(), 0);
        CORRADE_COMPARE(data.glossinessTextureLayer(), 0);

    /* Explicit parameters for everything, but all the same */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularTexture, 2u},
            {MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGB},
            {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::SpecularTextureCoordinates, 3u},
            {MaterialAttribute::SpecularTextureLayer, 17u},
            {MaterialAttribute::GlossinessTexture, 2u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::GlossinessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::GlossinessTextureCoordinates, 3u},
            {MaterialAttribute::GlossinessTextureLayer, 17u}
        }};
        CORRADE_VERIFY(data.hasSpecularGlossinessTexture());
        CORRADE_COMPARE(data.specularTexture(), 2);
        CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
        CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.specularTextureCoordinates(), 3);
        CORRADE_COMPARE(data.specularTextureLayer(), 17);
        CORRADE_COMPARE(data.glossinessTexture(), 2);
        CORRADE_COMPARE(data.glossinessTextureSwizzle(), MaterialTextureSwizzle::A);
        CORRADE_COMPARE(data.glossinessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.glossinessTextureCoordinates(), 3);
        CORRADE_COMPARE(data.glossinessTextureLayer(), 17);

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

    /* Unexpected array texture layer */
    } {
        PbrSpecularGlossinessMaterialData data{{}, {
            {MaterialAttribute::SpecularTexture, 2u},
            {MaterialAttribute::SpecularTextureLayer, 1u},
            {MaterialAttribute::GlossinessTexture, 2u},
            {MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::A},
        }};
        CORRADE_VERIFY(!data.hasSpecularGlossinessTexture());
    }
}

void PbrSpecularGlossinessMaterialDataTest::invalidTextures() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PbrSpecularGlossinessMaterialData data{{}, {}};

    Containers::String out;
    Error redirectError{&out};
    data.diffuseTexture();
    data.diffuseTextureMatrix();
    data.diffuseTextureCoordinates();
    data.diffuseTextureLayer();
    data.specularTexture();
    data.specularTextureSwizzle();
    data.specularTextureMatrix();
    data.specularTextureCoordinates();
    data.specularTextureLayer();
    data.glossinessTexture();
    data.glossinessTextureSwizzle();
    data.glossinessTextureMatrix();
    data.glossinessTextureCoordinates();
    data.glossinessTextureLayer();
    data.normalTexture();
    data.normalTextureScale();
    data.normalTextureSwizzle();
    data.normalTextureMatrix();
    data.normalTextureCoordinates();
    data.normalTextureLayer();
    data.occlusionTexture();
    data.occlusionTextureStrength();
    data.occlusionTextureSwizzle();
    data.occlusionTextureMatrix();
    data.occlusionTextureCoordinates();
    data.occlusionTextureLayer();
    data.emissiveTexture();
    data.emissiveTextureMatrix();
    data.emissiveTextureCoordinates();
    data.emissiveTextureLayer();
    CORRADE_COMPARE(out,
        "Trade::MaterialData::attribute(): attribute DiffuseTexture not found in layer 0\n"
        "Trade::PbrSpecularGlossinessMaterialData::diffuseTextureMatrix(): the material doesn't have a diffuse texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::diffuseTextureCoordinates(): the material doesn't have a diffuse texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::diffuseTextureLayer(): the material doesn't have a diffuse texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::specularTexture(): the material doesn't have a specular texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::specularTextureSwizzle(): the material doesn't have a specular texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::specularTextureMatrix(): the material doesn't have a specular texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::specularTextureCoordinates(): the material doesn't have a specular texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::specularTextureLayer(): the material doesn't have a specular texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTexture(): the material doesn't have a glossiness texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTextureSwizzle(): the material doesn't have a glossiness texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTextureMatrix(): the material doesn't have a glossiness texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTextureCoordinates(): the material doesn't have a glossiness texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTextureLayer(): the material doesn't have a glossiness texture\n"
        "Trade::MaterialData::attribute(): attribute NormalTexture not found in layer 0\n"
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureScale(): the material doesn't have a normal texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureSwizzle(): the material doesn't have a normal texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureMatrix(): the material doesn't have a normal texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureLayer(): the material doesn't have a normal texture\n"
        "Trade::MaterialData::attribute(): attribute OcclusionTexture not found in layer 0\n"
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureStrength(): the material doesn't have an occlusion texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureSwizzle(): the material doesn't have an occlusion texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureMatrix(): the material doesn't have an occlusion texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureCoordinates(): the material doesn't have an occlusion texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureLayer(): the material doesn't have an occlusion texture\n"
        "Trade::MaterialData::attribute(): attribute EmissiveTexture not found in layer 0\n"
        "Trade::PbrSpecularGlossinessMaterialData::emissiveTextureMatrix(): the material doesn't have an emissive texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::emissiveTextureCoordinates(): the material doesn't have an emissive texture\n"
        "Trade::PbrSpecularGlossinessMaterialData::emissiveTextureLayer(): the material doesn't have an emissive texture\n");
}

void PbrSpecularGlossinessMaterialDataTest::commonTransformationCoordinatesLayerNoTextures() {
    PbrSpecularGlossinessMaterialData a{{}, {}};
    CORRADE_VERIFY(a.hasCommonTextureTransformation());
    CORRADE_VERIFY(a.hasCommonTextureCoordinates());
    CORRADE_VERIFY(a.hasCommonTextureLayer());
    CORRADE_COMPARE(a.commonTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(a.commonTextureCoordinates(), 0);
    CORRADE_COMPARE(a.commonTextureLayer(), 0);

    PbrSpecularGlossinessMaterialData b{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},
        {MaterialAttribute::TextureLayer, 22u}
    }};
    CORRADE_VERIFY(b.hasCommonTextureTransformation());
    CORRADE_VERIFY(b.hasCommonTextureCoordinates());
    CORRADE_VERIFY(b.hasCommonTextureLayer());
    CORRADE_COMPARE(b.commonTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(b.commonTextureCoordinates(), 7);
    CORRADE_COMPARE(b.commonTextureLayer(), 22);
}

void PbrSpecularGlossinessMaterialDataTest::commonTransformationCoordinatesLayerOneTexture() {
    Containers::StringView textureName = PbrSpecularGlossinessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrSpecularGlossinessMaterialData data{{}, {
        {textureName, 5u},
        {textureName + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {textureName + "Coordinates", 17u},
        {textureName + "Layer", 22u},

        /* These shouldn't affect the above */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u},
        {MaterialAttribute::TextureLayer, 66u},
    }};

    CORRADE_VERIFY(data.hasCommonTextureTransformation());
    CORRADE_COMPARE(data.commonTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_COMPARE(data.commonTextureCoordinates(), 17u);
    CORRADE_VERIFY(data.hasCommonTextureLayer());
    CORRADE_COMPARE(data.commonTextureLayer(), 22u);
}

void PbrSpecularGlossinessMaterialDataTest::commonTransformationCoordinatesLayerOneDifferentTexture() {
    Containers::StringView textureName = PbrSpecularGlossinessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrSpecularGlossinessMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 2u},
        {MaterialAttribute::SpecularTexture, 3u},
        {MaterialAttribute::GlossinessTexture, 4u},
        {MaterialAttribute::NormalTexture, 5u},
        {MaterialAttribute::OcclusionTexture, 6u},
        {MaterialAttribute::EmissiveTexture, 7u},
        {textureName + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {textureName + "Coordinates", 17u},
        {textureName + "Layer", 22u},

        /* These are used by all textures except the one above, failing the
           check */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u},
        {MaterialAttribute::TextureLayer, 66u},
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());
    CORRADE_VERIFY(!data.hasCommonTextureLayer());
}

void PbrSpecularGlossinessMaterialDataTest::commonCoordinatesLayerImplicit() {
    Containers::StringView textureName = PbrSpecularGlossinessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    /* The transformation doesn't have this behavior, because there checking an
       identity is rather expensive */

    PbrSpecularGlossinessMaterialData data{{}, {
        {textureName, 5u},
        {textureName + "Coordinates", 0u},
        {textureName + "Layer", 0u},
    }};

    /* Zero is treated same as if there would be no attribute at all */
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_VERIFY(data.hasCommonTextureLayer());
    CORRADE_COMPARE(data.commonTextureCoordinates(), 0u);
    CORRADE_COMPARE(data.commonTextureLayer(), 0u);
}

void PbrSpecularGlossinessMaterialDataTest::noCommonTransformationCoordinatesLayer() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PbrSpecularGlossinessMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 3u},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 3u},
        {MaterialAttribute::SpecularTexture, 4u},
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::SpecularTextureLayer, 22u},
        {MaterialAttribute::OcclusionTexture, 5u},
        {MaterialAttribute::OcclusionTextureCoordinates, 17u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());
    CORRADE_VERIFY(!data.hasCommonTextureLayer());

    Containers::String out;
    Error redirectError{&out};
    data.commonTextureMatrix();
    data.commonTextureCoordinates();
    data.commonTextureLayer();
    CORRADE_COMPARE(out,
        "Trade::PbrSpecularGlossinessMaterialData::commonTextureMatrix(): the material doesn't have a common texture coordinate transformation\n"
        "Trade::PbrSpecularGlossinessMaterialData::commonTextureCoordinates(): the material doesn't have a common texture coordinate set\n"
        "Trade::PbrSpecularGlossinessMaterialData::commonTextureLayer(): the material doesn't have a common array texture layer\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::PbrSpecularGlossinessMaterialDataTest)
