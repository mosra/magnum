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

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/PbrMetallicRoughnessMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class PbrMetallicRoughnessMaterialDataTest: public TestSuite::Tester {
    public:
        explicit PbrMetallicRoughnessMaterialDataTest();

        void basics();
        void defaults();
        void textured();
        void texturedDefaults();
        void texturedImplicitPackedMetallicRoughness();
        void texturedExplicitPackedMetallicRoughness();
        void texturedExplicitPackedRoughnessMetallicOcclusion();
        void texturedExplicitPackedOcclusionRoughnessMetallic();
        void texturedExplicitPackedNormalRoughnessMetallic();
        void texturedSingleMatrixCoordinates();
        void invalidTextures();
        void commonTransformationCoordinatesNoTextures();
        void commonTransformationCoordinatesOneTexture();
        void commonTransformationCoordinatesOneDifferentTexture();
        void noCommonTransformationCoordinates();
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
              &PbrMetallicRoughnessMaterialDataTest::texturedImplicitPackedMetallicRoughness,
              &PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedMetallicRoughness,
              &PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedRoughnessMetallicOcclusion,
              &PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedOcclusionRoughnessMetallic,
              &PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedNormalRoughnessMetallic,
              &PbrMetallicRoughnessMaterialDataTest::texturedSingleMatrixCoordinates,
              &PbrMetallicRoughnessMaterialDataTest::invalidTextures,
              &PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesNoTextures});

    addInstancedTests({
        &PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesOneTexture,
        &PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesOneDifferentTexture},
        Containers::arraySize(PbrMetallicRoughnessTextureData));

    addTests({&PbrMetallicRoughnessMaterialDataTest::noCommonTransformationCoordinates});
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
        {MaterialAttribute::Metalness, 0.5f},
        {MaterialAttribute::MetalnessTexture, 1u},
        {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
        {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::MetalnessTextureCoordinates, 3u},
        {MaterialAttribute::Roughness, 0.79f},
        {MaterialAttribute::RoughnessTexture, 2u},
        {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::A},
        {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({1.0f, 1.0f})},
        {MaterialAttribute::RoughnessTextureCoordinates, 4u},
        {MaterialAttribute::NormalTexture, 3u},
        {MaterialAttribute::NormalTextureScale, 0.35f},
        {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::BA},
        {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({1.0f, 0.5f})},
        {MaterialAttribute::NormalTextureCoordinates, 5u},
        {MaterialAttribute::OcclusionTexture, 4u},
        {MaterialAttribute::OcclusionTextureStrength, 0.66f},
        {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
        {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({1.0f, 0.75f})},
        {MaterialAttribute::OcclusionTextureCoordinates, 6u},
        {MaterialAttribute::EmissiveColor, 0x111111_rgbf},
        {MaterialAttribute::EmissiveTexture, 5u},
        {MaterialAttribute::EmissiveTextureMatrix, Matrix3::scaling({0.75f, 0.5f})},
        {MaterialAttribute::EmissiveTextureCoordinates, 7u}
    }};

    CORRADE_VERIFY(data.hasMetalnessTexture());
    CORRADE_VERIFY(data.hasRoughnessTexture());
    CORRADE_VERIFY(!data.hasNoneRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
    CORRADE_VERIFY(!data.hasOcclusionRoughnessMetallicTexture());
    CORRADE_VERIFY(!data.hasNormalRoughnessMetallicTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.baseColor(), 0xccffbbff_rgbaf);
    CORRADE_COMPARE(data.baseColorTexture(), 0);
    CORRADE_COMPARE(data.baseColorTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.baseColorTextureCoordinates(), 2);
    CORRADE_COMPARE(data.metalness(), 0.5f);
    CORRADE_COMPARE(data.metalnessTexture(), 1);
    CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::G);
    CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.metalnessTextureCoordinates(), 3);
    CORRADE_COMPARE(data.roughness(), 0.79f);
    CORRADE_COMPARE(data.roughnessTexture(), 2);
    CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::A);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({1.0f, 1.0f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 4);
    CORRADE_COMPARE(data.normalTexture(), 3);
    CORRADE_COMPARE(data.normalTextureScale(), 0.35f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::BA);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({1.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 5);
    CORRADE_COMPARE(data.occlusionTexture(), 4);
    CORRADE_COMPARE(data.occlusionTextureStrength(), 0.66f);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3::scaling({1.0f, 0.75f}));
    CORRADE_COMPARE(data.occlusionTextureSwizzle(), MaterialTextureSwizzle::B);
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 6);
    CORRADE_COMPARE(data.emissiveColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3::scaling({0.75f, 0.5f}));
    CORRADE_COMPARE(data.emissiveTexture(), 5);
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 7);
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
    CORRADE_COMPARE(data.baseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.baseColorTexture(), 1);
    CORRADE_COMPARE(data.baseColorTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.baseColorTextureCoordinates(), 0);
    CORRADE_COMPARE(data.metalness(), 1.0f);
    CORRADE_COMPARE(data.metalnessTexture(), 2);
    CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.metalnessTextureCoordinates(), 0);
    CORRADE_COMPARE(data.roughness(), 1.0f);
    CORRADE_COMPARE(data.roughnessTexture(), 3);
    CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 0);
    CORRADE_COMPARE(data.normalTexture(), 4);
    CORRADE_COMPARE(data.normalTextureScale(), 1.0f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.normalTextureCoordinates(), 0);
    CORRADE_COMPARE(data.occlusionTexture(), 5);
    CORRADE_COMPARE(data.occlusionTextureStrength(), 1.0f);
    CORRADE_COMPARE(data.occlusionTextureSwizzle(), MaterialTextureSwizzle::R);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 0);
    CORRADE_COMPARE(data.emissiveColor(), 0x000000_rgbf);
    CORRADE_COMPARE(data.emissiveTexture(), 6);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 0);
}

void PbrMetallicRoughnessMaterialDataTest::texturedSingleMatrixCoordinates() {
    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 1u},
        {MaterialAttribute::MetalnessTexture, 2u},
        {MaterialAttribute::RoughnessTexture, 3u},
        {MaterialAttribute::NormalTexture, 4u},
        {MaterialAttribute::OcclusionTexture, 5u},
        {MaterialAttribute::EmissiveTexture, 6u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u}
    }};

    CORRADE_COMPARE(data.baseColorTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.baseColorTextureCoordinates(), 7);
    CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.metalnessTextureCoordinates(), 7);
    CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.roughnessTextureCoordinates(), 7);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 7);
    CORRADE_COMPARE(data.occlusionTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.occlusionTextureCoordinates(), 7);
    CORRADE_COMPARE(data.emissiveTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.emissiveTextureCoordinates(), 7);
}

void PbrMetallicRoughnessMaterialDataTest::texturedImplicitPackedMetallicRoughness() {
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
        CORRADE_COMPARE(data.metalnessTexture(), 2);
        CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::B);
        CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.metalnessTextureCoordinates(), 0);

    /* Explicit parameters for everything, but all the same */
    } {
        PbrMetallicRoughnessMaterialData data{{}, {
            {MaterialAttribute::NoneRoughnessMetallicTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u}
        }};
        CORRADE_VERIFY(data.hasNoneRoughnessMetallicTexture());
        CORRADE_COMPARE(data.roughnessTexture(), 2);
        CORRADE_COMPARE(data.roughnessTextureSwizzle(), MaterialTextureSwizzle::G);
        CORRADE_COMPARE(data.roughnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.roughnessTextureCoordinates(), 3);
        CORRADE_COMPARE(data.metalnessTexture(), 2);
        CORRADE_COMPARE(data.metalnessTextureSwizzle(), MaterialTextureSwizzle::B);
        CORRADE_COMPARE(data.metalnessTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
        CORRADE_COMPARE(data.metalnessTextureCoordinates(), 3);

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
    }
}

void PbrMetallicRoughnessMaterialDataTest::texturedExplicitPackedMetallicRoughness() {
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
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u}
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
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u},
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::OcclusionTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::OcclusionTextureCoordinates, 3u}
        }};
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
            {MaterialAttribute::OcclusionTexture, 2u},
            {MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 1u},
        }};
        CORRADE_VERIFY(!data.hasRoughnessMetallicOcclusionTexture());
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
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::G},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u}
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
            {MaterialAttribute::RoughnessTexture, 2u},
            {MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::B},
            {MaterialAttribute::RoughnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::RoughnessTextureCoordinates, 3u},
            {MaterialAttribute::MetalnessTexture, 2u},
            {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
            {MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::A},
            {MaterialAttribute::MetalnessTextureCoordinates, 3u}
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
    }
}

void PbrMetallicRoughnessMaterialDataTest::invalidTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PbrMetallicRoughnessMaterialData data{{}, {}};

    std::ostringstream out;
    Error redirectError{&out};
    data.baseColorTexture();
    data.baseColorTextureMatrix();
    data.baseColorTextureCoordinates();
    data.metalnessTexture();
    data.metalnessTextureSwizzle();
    data.metalnessTextureMatrix();
    data.metalnessTextureCoordinates();
    data.roughnessTexture();
    data.roughnessTextureSwizzle();
    data.roughnessTextureMatrix();
    data.roughnessTextureCoordinates();
    data.normalTexture();
    data.normalTextureScale();
    data.normalTextureSwizzle();
    data.normalTextureMatrix();
    data.normalTextureCoordinates();
    data.occlusionTexture();
    data.occlusionTextureStrength();
    data.occlusionTextureSwizzle();
    data.occlusionTextureMatrix();
    data.occlusionTextureCoordinates();
    data.emissiveTexture();
    data.emissiveTextureMatrix();
    data.emissiveTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attribute(): attribute BaseColorTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::baseColorTextureMatrix(): the material doesn't have a base color texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::baseColorTextureCoordinates(): the material doesn't have a base color texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTexture(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureSwizzle(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureMatrix(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureCoordinates(): the material doesn't have a metalness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTexture(): the material doesn't have a roughness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureSwizzle(): the material doesn't have a roughness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureMatrix(): the material doesn't have a roughness texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureCoordinates(): the material doesn't have a roughness texture\n"
        "Trade::MaterialData::attribute(): attribute NormalTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureScale(): the material doesn't have a normal texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureSwizzle(): the material doesn't have a normal texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureMatrix(): the material doesn't have a normal texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture\n"
        "Trade::MaterialData::attribute(): attribute OcclusionTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureStrength(): the material doesn't have an occlusion texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureSwizzle(): the material doesn't have an occlusion texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureMatrix(): the material doesn't have an occlusion texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureCoordinates(): the material doesn't have an occlusion texture\n"
        "Trade::MaterialData::attribute(): attribute EmissiveTexture not found in layer 0\n"
        "Trade::PbrMetallicRoughnessMaterialData::emissiveTextureMatrix(): the material doesn't have an emissive texture\n"
        "Trade::PbrMetallicRoughnessMaterialData::emissiveTextureCoordinates(): the material doesn't have an emissive texture\n");
}

void PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesNoTextures() {
    PbrMetallicRoughnessMaterialData a{{}, {}};
    CORRADE_VERIFY(a.hasCommonTextureTransformation());
    CORRADE_VERIFY(a.hasCommonTextureCoordinates());
    CORRADE_COMPARE(a.commonTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(a.commonTextureCoordinates(), 0);

    PbrMetallicRoughnessMaterialData b{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u}
    }};
    CORRADE_VERIFY(b.hasCommonTextureTransformation());
    CORRADE_VERIFY(b.hasCommonTextureCoordinates());
    CORRADE_COMPARE(b.commonTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(b.commonTextureCoordinates(), 7);
}

void PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesOneTexture() {
    Containers::StringView textureName = PbrMetallicRoughnessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrMetallicRoughnessMaterialData data{{}, {
        {textureName, 5u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u},

        /* These shouldn't affect the above */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(data.hasCommonTextureTransformation());
    CORRADE_COMPARE(data.commonTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_VERIFY(data.hasCommonTextureCoordinates());
    CORRADE_COMPARE(data.commonTextureCoordinates(), 17u);
}

void PbrMetallicRoughnessMaterialDataTest::commonTransformationCoordinatesOneDifferentTexture() {
    Containers::StringView textureName = PbrMetallicRoughnessTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 2u},
        {MaterialAttribute::MetalnessTexture, 3u},
        {MaterialAttribute::RoughnessTexture, 4u},
        {MaterialAttribute::NormalTexture, 5u},
        {MaterialAttribute::OcclusionTexture, 6u},
        {MaterialAttribute::EmissiveTexture, 7u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u},

        /* These are used by all textures except the one above, failing the
           check */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());
}

void PbrMetallicRoughnessMaterialDataTest::noCommonTransformationCoordinates() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PbrMetallicRoughnessMaterialData data{{}, {
        {MaterialAttribute::BaseColorTexture, 3u},
        {MaterialAttribute::BaseColorTextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::BaseColorTextureCoordinates, 3u},
        {MaterialAttribute::MetalnessTexture, 4u},
        {MaterialAttribute::MetalnessTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::RoughnessTexture, 5u},
        {MaterialAttribute::RoughnessTextureCoordinates, 17u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());

    std::ostringstream out;
    Error redirectError{&out};
    data.commonTextureMatrix();
    data.commonTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::PbrMetallicRoughnessMaterialData::commonTextureMatrix(): the material doesn't have a common texture coordinate transformation\n"
        "Trade::PbrMetallicRoughnessMaterialData::commonTextureCoordinates(): the material doesn't have a common texture coordinate set\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::PbrMetallicRoughnessMaterialDataTest)
