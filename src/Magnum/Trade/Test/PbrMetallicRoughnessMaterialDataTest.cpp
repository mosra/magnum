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
#include "Magnum/Trade/PbrMetallicRoughnessMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct PbrMetallicRoughnessMaterialDataTest: TestSuite::Tester {
    explicit PbrMetallicRoughnessMaterialDataTest();

    void basics();
    void defaults();
    void textured();
    void texturedDefaults();
    void texturedImplicitPackedNoneRoughnessMetallic();
    void texturedExplicitPackedNoneRoughnessMetallic();
    void texturedImplicitPackedOcclusionRoughnessMetallic();
    void texturedExplicitPackedOcclusionRoughnessMetallic();
    void texturedExplicitPackedRoughnessMetallicOcclusion();
    void texturedExplicitPackedNormalRoughnessMetallic();
    void texturedSingleMatrixCoordinatesLayer();
    void invalidTextures();
    void commonTransformationCoordinatesLayerNoTextures();
    void commonTransformationCoordinatesLayerOneTexture();
    void commonTransformationCoordinatesLayerOneDifferentTexture();
    void commonCoordinatesLayerImplicit();
    void noCommonTransformationCoordinatesLayer();
};

const Containers::StringView PbrMetallicRoughnessTextureData[] {
    "BaseColorTexture",
    "MetalnessTexture",
    "RoughnessTexture",
    "NormalTexture",
    "OcclusionTexture",
    "EmissiveTexture"
};

PbrMetallicRoughnessMaterialDataTest::PbrMetallicRoughnessMaterialDataTest() {
    addTests({&PbrMetallicRoughnessMaterialDataTest::basics,
              &PbrMetallicRoughnessMaterialDataTest::defaults,
              &PbrMetallicRoughnessMaterialDataTest::textured,
              &PbrMetallicRoughnessMaterialDataTest::texturedDefaults,
              &PbrMetallicRoughnessMaterialDataTest::texturedImplicitPackedNoneRoughnessMetallic,
              &PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedNoneRoughnessMetallic,
              &PbrMetallicRoughnessMaterialDataTest::texturedImplicitPackedOcclusionRoughnessMetallic,
              &PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedOcclusionRoughnessMetallic,
              &PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedRoughnessMetallicOcclusion,
              &PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedNormalRoughnessMetallic,
              &PbrMetallicRoughnessMaterialDataTest::texturedSingleMatrixCoordinatesLayer,
              &PbrMetallicRoughnessMaterialDataTest::invalidTextures,
              &PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesLayerNoTextures});

    addInstancedTests({
        &PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesLayerOneTexture,
        &PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesLayerOneDifferentTexture,
        &PbrMetallicRoughnessMaterialDataTest::commonCoordinatesLayerImplicit},
        Containers::arraySize(PbrMetallicRoughnessTextureData));

    addTests({&PbrMetallicRoughnessMaterialDataTest::noCommonTransformationCoordinatesLayer});
}

using namespace Math::Literals;

void PbrMetallicRoughnessMaterialDataTest::basics() {
    MaterialData base{MaterialType::PbrMetallicRoughness, {
        {MaterialAttribute::BaseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::Metalness, 0.5f},
        {MaterialAttribute::Roughness, 0.79f},
        {MaterialAttribute::EmissiveColor, 0x111111_rgbf}
    }};

    CORRADE_COMPARE(base.types(), MaterialType::PbrMetallicRoughness);
    const auto& data = base.as<PbrMetallicRoughnessMaterialData>();

    CORRADE_VERIFY(!data.hasMetalnessTexture());
    CORRADE_VERIFY(!data.hasRoughnessTexture());
    CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_COMPARE(data.baseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(data.metalness(), 0.5f);
    CORRADE_COMPARE(data.roughness(), 0.79f);
    CORRADE_COMPARE(data.emissiveColor(), 0x111111_rgbf);
}

void PbrMetallicRoughnessMaterialDataTest::defaults() {
    MaterialData base{{}, {}};

    CORRADE_COMPARE(base.types(), MaterialTypes{});
    /* Casting is fine even if the type doesn't include PbrMetallicRoughness */
    const auto& data = base.as<PbrMetallicRoughnessMaterialData>();

    CORRADE_VERIFY(!data.hasMetalnessTexture());
    CORRADE_VERIFY(!data.hasRoughnessTexture());
    CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_COMPARE(data.baseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.metalness(), 1.0f);
    CORRADE_COMPARE(data.roughness(), 1.0f);
    CORRADE_COMPARE(data.emissiveColor(), 0x000000_rgbf);
}

void PbrMetallicRoughnessMaterialDataTest::textured() {
    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::BaseColorTexture, 0u},
        {MaterialAttribute::BaseColorTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::BaseColorTextureCoordinates, 2u},
        {MaterialAttribute::BaseColorTextureLayer, 8u},
        {MaterialAttribute::Metalness, 0.5f},
        {MaterialAttribute::MetalnessTexture, 1u},
        {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
        {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::MetalnessTextureCoordinates, 3u},
        {MaterialAttribute::MetalnessTextureLayer, 9u},
        {MaterialAttribute::Roughness, 0.79f},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::A},
        {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({1.0f, 1.0f})},
        {MaterialAttribute::RoughnessTextureCoordinates, 4u},
        {MaterialAttribute::RoughnessTextureLayer, 10u},
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
        {MaterialAttribute::EmissiveTextureLayer, 13u}
    }};

    CORRADE_VERIFY(data.hasMetalnessTexture());
    CORRADE_VERIFY(data.hasRoughnessTexture());
    CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_VERIFY(data.hasTextureLayer());
    CORRADE_COMPARE(data.baseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(data.baseColorTexture(), 0);
    CORRADE_COMPARE(data.baseColorTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.baseColorTextureCoordinates(), 2);
    CORRADE_COMPARE(data.baseColorTextureLayer(), 8);
    CORRADE_COMPARE(data.metalness(), 0.5f);
    CORRADE_COMPARE(data.metalnessTexture(), 1);
    CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::G);
    CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.metalnessTextureCoordinates(), 3);
    CORRADE_COMPARE(data.metalnessTextureLayer(), 9);
    CORRADE_COMPARE(data.roughness(), 0.79f);
    CORRADE_COMPARE(data.roughnessTexture(), 2);
    CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::A);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({1.0f, 1.0f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 4);
    CORRADE_COMPARE(data.roughnessTextureLayer(), 10);
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

void PbrMetallicRoughnessMaterialDataTest::texturedDefaults() {
    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 1u},
        {MaterialAttribute::MetalnessTexture, 2u},
        {MaterialAttribute::RoughnessTexture, 3u},
        {MaterialAttribute::NormalTexture, 4u},
        {MaterialAttribute::OcclusionTexture, 5u},
        {MaterialAttribute::EmissiveTexture, 6u}
    }};

    CORRADE_VERIFY(data.hasMetalnessTexture());
    CORRADE_VERIFY(data.hasRoughnessTexture());
    CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_VERIFY(!data.hasTextureLayer());
    CORRADE_COMPARE(data.baseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.baseColorTexture(), 1);
    CORRADE_COMPARE(data.baseColorTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.baseColorTextureCoordinates(), 0);
    CORRADE_COMPARE(data.baseColorTextureLayer(), 0);
    CORRADE_COMPARE(data.metalness(), 1.0f);
    CORRADE_COMPARE(data.metalnessTexture(), 2);
    CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.metalnessTextureCoordinates(), 0);
    CORRADE_COMPARE(data.metalnessTextureLayer(), 0);
    CORRADE_COMPARE(data.roughness(), 1.0f);
    CORRADE_COMPARE(data.roughnessTexture(), 3);
    CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 0);
    CORRADE_COMPARE(data.roughnessTextureLayer(), 0);
    CORRADE_COMPARE(data.normalTexture(), 4);
    CORRADE_COMPARE(data.normalTextureScale(), 1.0f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.normalTextureCoordinates(), 0);
    CORRADE_COMPARE(data.normalTextureLayer(), 0);
    CORRADE_COMPARE(data.occlusionTexture(), 5);
    CORRADE_COMPARE(data.occlusionTextureStrength(), 1.0f);
    CORRADE_COMPARE(data.occlusionTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 0);
    CORRADE_COMPARE(data.occlusionTextureLayer(), 0);
    CORRADE_COMPARE(data.emissiveColor(), 0x000000_rgbf);
    CORRADE_COMPARE(data.emissiveTexture(), 6);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 0);
    CORRADE_COMPARE(data.emissiveTextureLayer(), 0);
}

void PbrMetallicRoughnessMaterialDataTest::texturedSingleMatrixCoordinatesLayer() {
    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 1u},
        {MaterialAttribute::MetalnessTexture, 2u},
        {MaterialAttribute::RoughnessTexture, 3u},
        {MaterialAttribute::NormalTexture, 4u},
        {MaterialAttribute::OcclusionTexture, 5u},
        {MaterialAttribute::EmissiveTexture, 6u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u},
        {MaterialAttribute::TextureLayer, 8u},
    }};

    CORRADE_COMPARE(data.baseColorTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.baseColorTextureCoordinates(), 7);
    CORRADE_COMPARE(data.baseColorTextureLayer(), 8);
    CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.metalnessTextureCoordinates(), 7);
    CORRADE_COMPARE(data.metalnessTextureLayer(), 8);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 7);
    CORRADE_COMPARE(data.roughnessTextureLayer(), 8);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7);
    CORRADE_COMPARE(data.normalTextureLayer(), 8);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 7);
    CORRADE_COMPARE(data.occlusionTextureLayer(), 8);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 7);
    CORRADE_COMPARE(data.emissiveTextureLayer(), 8);
}

void PbrMetallicRoughnessMaterialDataTest::texturedImplicitPackedNoneRoughnessMetallic() {
    /* Just the texture ID, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());
        CORRADE_COMPARE(data.roughnessTexture(), 2);
        CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::G);
        CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.roughnessTextureCoordinates(), 0);
        CORRADE_COMPARE(data.roughnessTextureLayer(), 0);
        CORRADE_COMPARE(data.metalnessTexture(), 2);
        CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::B);
        CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.metalnessTextureCoordinates(), 0);
        CORRADE_COMPARE(data.metalnessTextureLayer(), 0);

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::RoughnessTextureLayer, 17u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTextureLayer, 17u},
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());
        CORRADE_COMPARE(data.roughnessTexture(), 2);
        CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::G);
        CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.roughnessTextureCoordinates(), 3);
        CORRADE_COMPARE(data.roughnessTextureLayer(), 17);
        CORRADE_COMPARE(data.metalnessTexture(), 2);
        CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::B);
        CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.metalnessTextureCoordinates(), 3);
        CORRADE_COMPARE(data.metalnessTextureLayer(), 17);

    /* Swizzle is ignored when the combined texture is specified, so this is
       fine */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::RoughnessTextureCoordinates, 1u},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Unexpected array texture layer */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::MetalnessTextureLayer, 1u},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    }
}

void PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedNoneRoughnessMetallic() {
    /* Just the texture IDs and swizzles, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B}
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::RoughnessTextureLayer, 7u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTextureLayer, 7u}
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Different texture ID */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 3u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B}
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* One texture missing */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Unexpected swizzle */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::R},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureCoordinates, 1u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Unexpected array texture layer */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureLayer, 1u},
        }};
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    }
}

void PbrMetallicRoughnessMaterialDataTest::texturedImplicitPackedOcclusionRoughnessMetallic() {
    /* Just the texture IDs, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
        }};
        CORRADE_VERIFY(data.hasOcclusionRoughnessMetallicTexture());
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
        /* This is a superset */
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::R},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::OcclusionTextureCoordinates, 3u},
            {MaterialAttribute::OcclusionTextureLayer, 17u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::RoughnessTextureLayer, 17u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTextureLayer, 17u}
        }};
        CORRADE_VERIFY(data.hasOcclusionRoughnessMetallicTexture());
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
        /* This is a superset */
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Different texture ID */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::NoneRoughnessMetallicTexture, 3u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* One texture missing */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected swizzle */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 1u},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected array texture layer */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::OcclusionTextureLayer, 1u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    }
}

void PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedOcclusionRoughnessMetallic() {
    /* Just the texture IDs and swizzles, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(data.hasOcclusionRoughnessMetallicTexture());
        /* This is a superset */
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::R},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::OcclusionTextureCoordinates, 3u},
            {MaterialAttribute::OcclusionTextureLayer, 17u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::RoughnessTextureLayer, 17u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTextureLayer, 17u}
        }};
        CORRADE_VERIFY(data.hasOcclusionRoughnessMetallicTexture());
        /* This is a superset */
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());

    /* Different texture ID */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::RoughnessTexture, 3u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* One texture missing */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected swizzle */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 1u},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());

    /* Unexpected array texture layer */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureLayer, 1u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    }
}

void PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedRoughnessMetallicOcclusion() {
    /* Just the texture IDs and swizzles, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
        CORRADE_VERIFY(data.hasRoughnessMetallicOcclusionTexture());
        /* This isn't a superset */
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::R},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::RoughnessTextureLayer, 7u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTextureLayer, 7u},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::OcclusionTextureCoordinates, 3u},
            {MaterialAttribute::OcclusionTextureLayer, 7u},
        }};
        CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
        CORRADE_VERIFY(data.hasRoughnessMetallicOcclusionTexture());
        /* This isn't a superset */
        CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());

    /* Different texture ID */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 3u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());

    /* One texture missing */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());

    /* Unexpected swizzle */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureCoordinates, 1u},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());

    /* Unexpected array texture layer */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureLayer, 1u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    }
}

void PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedNormalRoughnessMetallic() {
    /* Just the texture IDs and swizzles, the rest is implicit */
    {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A}
        }};
        CORRADE_VERIFY(data.hasNormalRoughnessMetallicTexture());

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::NormalTextureCoordinates, 3u},
            {MaterialAttribute::NormalTextureLayer, 17u},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::RoughnessTextureLayer, 17u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTextureLayer, 17u}
        }};
        CORRADE_VERIFY(data.hasNormalRoughnessMetallicTexture());

    /* Different texture ID */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 3u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A}
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());

    /* One texture missing */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());

    /* Unexpected swizzle */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RGB},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());

    /* Unexpected texture matrix */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());

    /* Unexpected texture coordinates */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::MetalnessTextureCoordinates, 1u},
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());

    /* Unexpected array texture layer */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NormalTexture, 2u},
            {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RG},
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::RoughnessTextureLayer, 1u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
        }};
        CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    }
}

void PbrMetallicRoughnessMaterialDataTest::invalidTextures() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PbrMetallicRoughnessMaterialData data{{}, {}};

    Containers::String out;
    Error redirectError{&out};
    data.baseColorTexture();
    data.baseColorTextureMatrix();
    data.baseColorTextureCoordinates();
    data.baseColorTextureLayer();
    data.metalnessTexture();
    data.metalnessTextureSwizzle();
    data.metalnessTextureMatrix();
    data.metalnessTextureCoordinates();
    data.metalnessTextureLayer();
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
        "Trade::MaterialData::attribute(): attribute BaseColorTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::baseColorTextureMatrix(): the material doesn't have a base color texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::baseColorTextureCoordinates(): the material doesn't have a base color texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::baseColorTextureLayer(): the material doesn't have a base color texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTexture(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureSwizzle(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureMatrix(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureCoordinates(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureLayer(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTexture(): the material doesn't have a roughness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureSwizzle(): the material doesn't have a roughness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureMatrix(): the material doesn't have a roughness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureCoordinates(): the material doesn't have a roughness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureLayer(): the material doesn't have a roughness texture\n"
        "Trade::MaterialData::attribute(): attribute NormalTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureScale(): the material doesn't have a normal texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureSwizzle(): the material doesn't have a normal texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureMatrix(): the material doesn't have a normal texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureLayer(): the material doesn't have a normal texture\n"
        "Trade::MaterialData::attribute(): attribute OcclusionTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureStrength(): the material doesn't have an occlusion texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureSwizzle(): the material doesn't have an occlusion texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureMatrix(): the material doesn't have an occlusion texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureCoordinates(): the material doesn't have an occlusion texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureLayer(): the material doesn't have an occlusion texture\n"
        "Trade::MaterialData::attribute(): attribute EmissiveTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::emissiveTextureMatrix(): the material doesn't have an emissive texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::emissiveTextureCoordinates(): the material doesn't have an emissive texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::emissiveTextureLayer(): the material doesn't have an emissive texture\n");
}

void PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesLayerNoTextures() {
    PbrMetallicRoughnessMaterialData a{{}, {}};
    CORRADE_VERIFY(a.hasCommonTextureTransformation());
    CORRADE_VERIFY(a.hasCommonTextureCoordinates());
    CORRADE_VERIFY(a.hasCommonTextureLayer());
    CORRADE_COMPARE(a.commonTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(a.commonTextureCoordinates(), 0);
    CORRADE_COMPARE(a.commonTextureLayer(), 0);

    PbrMetallicRoughnessMaterialData b{{}, {
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

void PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesLayerOneTexture() {
    Containers::StringView textureName = PbrMetallicRoughnessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrMetallicRoughnessMaterialData data{{}, {
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

void PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesLayerOneDifferentTexture() {
    Containers::StringView textureName = PbrMetallicRoughnessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 2u},
        {MaterialAttribute::MetalnessTexture, 3u},
        {MaterialAttribute::RoughnessTexture, 4u},
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

void PbrMetallicRoughnessMaterialDataTest::commonCoordinatesLayerImplicit() {
    Containers::StringView textureName = PbrMetallicRoughnessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    /* The transformation doesn't have this behavior, because there checking an
       identity is rather expensive */

    PbrMetallicRoughnessMaterialData data{{}, {
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

void PbrMetallicRoughnessMaterialDataTest::noCommonTransformationCoordinatesLayer() {
    CORRADE_SKIP_IF_NO_ASSERT();

    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 3u},
        {MaterialAttribute::BaseColorTextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::BaseColorTextureCoordinates, 3u},
        {MaterialAttribute::MetalnessTexture, 4u},
        {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::MetalnessTextureLayer, 22u},
        {MaterialAttribute::RoughnessTexture, 5u},
        {MaterialAttribute::RoughnessTextureCoordinates, 17u}
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
        "Trade::PbrMetallicRoughnessMaterialData::commonTextureMatrix(): the material doesn't have a common texture coordinate transformation\n"
        "Trade::PbrMetallicRoughnessMaterialData::commonTextureCoordinates(): the material doesn't have a common texture coordinate set\n"
        "Trade::PbrMetallicRoughnessMaterialData::commonTextureLayer(): the material doesn't have a common array texture layer\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::PbrMetallicRoughnessMaterialDataTest)
