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
#include "Magnum/Trade/PhongMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class PhongMaterialDataTest: public TestSuite::Tester {
    public:
        explicit PhongMaterialDataTest();

        #ifdef MAGNUM_BUILD_DEPRECATED
        void constructDeprecated();
        void constructDeprecatedTextured();
        void constructDeprecatedTexturedTextureTransform();
        void constructDeprecatedTexturedCoordinates();
        void constructDeprecatedTextureTransformNoTextures();
        void constructDeprecatedNoTextureTransformationFlag();
        void constructDeprecatedNoTextureCoordinatesFlag();
        #endif

        void basics();
        void defaults();
        void textured();
        void texturedDefaults();
        void texturedSingleMatrixCoordinates();
        void texturedImplicitPackedSpecularGlossiness();
        void invalidTextures();
        void commonTransformationCoordinatesNoTextures();
        void commonTransformationCoordinatesOneTexture();
        void commonTransformationCoordinatesOneDifferentTexture();
        void noCommonTransformationCoordinates();

        #ifdef MAGNUM_BUILD_DEPRECATED
        void debugFlag();
        void debugFlags();
        #endif
};

const Containers::StringView PhongTextureData[] {
    "AmbientTexture",
    "DiffuseTexture",
    "SpecularTexture",
    "NormalTexture"
};

PhongMaterialDataTest::PhongMaterialDataTest() {
    addTests({
        #ifdef MAGNUM_BUILD_DEPRECATED
        &PhongMaterialDataTest::constructDeprecated,
        &PhongMaterialDataTest::constructDeprecatedTextured,
        &PhongMaterialDataTest::constructDeprecatedTexturedTextureTransform,
        &PhongMaterialDataTest::constructDeprecatedTexturedCoordinates,
        &PhongMaterialDataTest::constructDeprecatedTextureTransformNoTextures,
        &PhongMaterialDataTest::constructDeprecatedNoTextureTransformationFlag,
        &PhongMaterialDataTest::constructDeprecatedNoTextureCoordinatesFlag,
        #endif

        &PhongMaterialDataTest::basics,
        &PhongMaterialDataTest::defaults,
        &PhongMaterialDataTest::textured,
        &PhongMaterialDataTest::texturedDefaults,
        &PhongMaterialDataTest::texturedSingleMatrixCoordinates,
        &PhongMaterialDataTest::texturedImplicitPackedSpecularGlossiness,
        &PhongMaterialDataTest::invalidTextures,
        &PhongMaterialDataTest::commonTransformationCoordinatesNoTextures});

    addInstancedTests({
        &PhongMaterialDataTest::commonTransformationCoordinatesOneTexture,
        &PhongMaterialDataTest::commonTransformationCoordinatesOneDifferentTexture},
        Containers::arraySize(PhongTextureData));

    addTests({
        &PhongMaterialDataTest::noCommonTransformationCoordinates,
        #ifdef MAGNUM_BUILD_DEPRECATED
        &PhongMaterialDataTest::debugFlag,
        &PhongMaterialDataTest::debugFlags
        #endif
        });
}

using namespace Containers::Literals;
using namespace Math::Literals;

#ifdef MAGNUM_BUILD_DEPRECATED
void PhongMaterialDataTest::constructDeprecated() {
    const int a{};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData data{PhongMaterialData::Flag::DoubleSided,
        0xccffbb_rgbf, {},
        0xebefbf_rgbf, {},
        0xacabad_rgbf, {}, {}, {},
        MaterialAlphaMode::Mask, 0.3f, 80.0f, &a};
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::DoubleSided);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.ambientColor(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xebefbf_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Mask);
    CORRADE_COMPARE(data.alphaMask(), 0.3f);
    CORRADE_COMPARE(data.shininess(), 80.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void PhongMaterialDataTest::constructDeprecatedTextured() {
    const int a{};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData data{
        PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture,
        0x111111_rgbf, 42,
        0xeebbff_rgbf, {},
        0xacabad_rgbf, 17, {}, {},
        MaterialAlphaMode::Blend, 0.37f, 96.0f, &a};
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.ambientTexture(), 42);
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 0);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureCoordinates(), 0);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Blend);
    CORRADE_COMPARE(data.alphaMask(), 0.37f);
    CORRADE_COMPARE(data.shininess(), 96.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void PhongMaterialDataTest::constructDeprecatedTexturedTextureTransform() {
    const int a{};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData data{
        PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureTransformation,
        0x111111_rgbf, {},
        0xeebbff_rgbf, 42,
        0xacabad_rgbf, {}, 17,
        Matrix3::rotation(90.0_degf),
        MaterialAlphaMode::Mask, 0.5f, 96.0f, &a};
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureTransformation);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.diffuseTexture(), 42);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.normalTexture(), 17);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::rotation(90.0_degf));
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Mask);
    CORRADE_COMPARE(data.alphaMask(), 0.5f);
    CORRADE_COMPARE(data.shininess(), 96.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void PhongMaterialDataTest::constructDeprecatedTexturedCoordinates() {
    const int a{};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData data{
        PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureCoordinates,
        0x111111_rgbf, 42, 3,
        0xeebbff_rgbf, {}, 6,
        0xacabad_rgbf, 17, 1,
        0, 8, {},
        MaterialAlphaMode::Blend, 0.37f, 96.0f, &a};
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(data.types(), MaterialType::Phong);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureCoordinates);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.ambientTexture(), 42);
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 3);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 6);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureCoordinates(), 1);
    CORRADE_COMPARE(data.normalTexture(), 0);
    CORRADE_COMPARE(data.normalTextureCoordinates(), 8);
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 3);
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Blend);
    CORRADE_COMPARE(data.alphaMask(), 0.37f);
    CORRADE_COMPARE(data.shininess(), 96.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void PhongMaterialDataTest::constructDeprecatedTextureTransformNoTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData a{PhongMaterialData::Flag::TextureTransformation,
        {}, {},
        {}, {},
        {}, {}, {}, {},
        {}, 0.5f, 80.0f};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(),
        "Trade::PhongMaterialData: texture transformation enabled but the material has no textures\n");
}

void PhongMaterialDataTest::constructDeprecatedNoTextureTransformationFlag() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData a{{},
        {}, {},
        {}, {},
        {}, {}, {}, Matrix3::rotation(90.0_degf),
        {}, 0.5f, 80.0f};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(),
        "PhongMaterialData::PhongMaterialData: non-default texture matrix requires Flag::TextureTransformation to be enabled\n");
}

void PhongMaterialDataTest::constructDeprecatedNoTextureCoordinatesFlag() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    PhongMaterialData a{{},
        {}, {}, 1,
        {}, {}, 2,
        {}, {}, 3, {}, 4, {},
        {}, 0.5f, 80.0f};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(),
        "PhongMaterialData::PhongMaterialData: non-zero texture coordinate sets require Flag::TextureCoordinates to be enabled\n");
}
#endif

void PhongMaterialDataTest::basics() {
    MaterialData base{MaterialType::Phong, {
        {MaterialAttribute::AmbientColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseColor, 0xebefbfff_rgbaf},
        {MaterialAttribute::SpecularColor, 0xacabadff_rgbaf},
        {MaterialAttribute::Shininess, 96.0f}
    }};

    CORRADE_COMPARE(base.types(), MaterialType::Phong);
    const auto& data = base.as<PhongMaterialData>();

    CORRADE_VERIFY(!data.hasSpecularTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientColor(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xebefbf_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.shininess(), 96.0f);
}

void PhongMaterialDataTest::defaults() {
    MaterialData base{{}, {}};

    CORRADE_COMPARE(base.types(), MaterialTypes{});
    /* Casting is fine even if the type doesn't include Phong */
    const auto& data = base.as<PhongMaterialData>();

    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientColor(), 0x000000_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xffffff_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xffffff00_rgbaf);
    CORRADE_COMPARE(data.shininess(), 80.0f);
}

void PhongMaterialDataTest::textured() {
    PhongMaterialData data{{}, {
        {MaterialAttribute::AmbientColor, 0x111111ff_rgbaf},
        {MaterialAttribute::AmbientTexture, 42u},
        {MaterialAttribute::AmbientTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::AmbientTextureCoordinates, 2u},
        {MaterialAttribute::DiffuseTexture, 33u},
        {MaterialAttribute::DiffuseColor, 0xeebbffff_rgbaf},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 3u},
        {MaterialAttribute::SpecularColor, 0xacabadff_rgbaf},
        {MaterialAttribute::SpecularTexture, 17u},
        {MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGBA},
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({1.0f, 1.0f})},
        {MaterialAttribute::SpecularTextureCoordinates, 4u},
        {MaterialAttribute::NormalTexture, 0u},
        {MaterialAttribute::NormalTextureScale, 0.5f},
        {MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::GB},
        {MaterialAttribute::NormalTextureMatrix, Matrix3::scaling({1.0f, 0.5f})},
        {MaterialAttribute::NormalTextureCoordinates, 5u}
    }};

    CORRADE_VERIFY(data.hasSpecularTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.ambientTexture(), 42);
    CORRADE_COMPARE(data.ambientTextureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 2);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.diffuseTexture(), 33);
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 3);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGBA);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({1.0f, 1.0f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 4);
    CORRADE_COMPARE(data.normalTexture(), 0);
    CORRADE_COMPARE(data.normalTextureScale(), 0.5f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::GB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::scaling({1.0f, 0.5f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 5);
}

void PhongMaterialDataTest::texturedDefaults() {
    PhongMaterialData data{{}, {
        {MaterialAttribute::AmbientTexture, 42u},
        {MaterialAttribute::DiffuseTexture, 33u},
        {MaterialAttribute::SpecularTexture, 17u},
        {MaterialAttribute::NormalTexture, 1u}
    }};

    CORRADE_VERIFY(data.hasSpecularTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.ambientTexture(), 42);
    CORRADE_COMPARE(data.ambientTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 0);
    CORRADE_COMPARE(data.diffuseColor(), 0xffffffff_rgbaf);
    CORRADE_COMPARE(data.diffuseTexture(), 33);
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 0);
    CORRADE_COMPARE(data.specularColor(), 0xffffff00_rgbaf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.specularTextureCoordinates(), 0);
    CORRADE_COMPARE(data.normalTexture(), 1);
    CORRADE_COMPARE(data.normalTextureScale(), 1.0f);
    CORRADE_COMPARE(data.normalTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.normalTextureCoordinates(), 0);
}

void PhongMaterialDataTest::texturedSingleMatrixCoordinates() {
    PhongMaterialData data{{}, {
        {MaterialAttribute::AmbientTexture, 42u},
        {MaterialAttribute::DiffuseTexture, 33u},
        {MaterialAttribute::SpecularTexture, 17u},
        {MaterialAttribute::NormalTexture, 0u},
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 1.0f})},
        {MaterialAttribute::TextureCoordinates, 2u}
    }};

    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.ambientTextureMatrix(), Matrix3::translation({0.5f, 1.0f}));
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 2);
    CORRADE_COMPARE(data.diffuseTextureMatrix(), Matrix3::translation({0.5f, 1.0f}));
    CORRADE_COMPARE(data.diffuseTextureCoordinates(), 2);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::translation({0.5f, 1.0f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 2);
    CORRADE_COMPARE(data.normalTextureMatrix(), Matrix3::translation({0.5f, 1.0f}));
    CORRADE_COMPARE(data.normalTextureCoordinates(), 2);
}

void PhongMaterialDataTest::texturedImplicitPackedSpecularGlossiness() {
    PhongMaterialData data{{}, {
        {MaterialAttribute::SpecularColor, 0xacabadff_rgbaf},
        {MaterialAttribute::SpecularGlossinessTexture, 17u},
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({1.0f, 1.0f})},
        {MaterialAttribute::SpecularTextureCoordinates, 4u},
    }};

    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::TextureCoordinates|PhongMaterialData::Flag::TextureTransformation);
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
    CORRADE_VERIFY(data.hasSpecularTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureSwizzle(), MaterialTextureSwizzle::RGB);
    CORRADE_COMPARE(data.specularTextureMatrix(), Matrix3::scaling({1.0f, 1.0f}));
    CORRADE_COMPARE(data.specularTextureCoordinates(), 4);
}

void PhongMaterialDataTest::invalidTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PhongMaterialData data{{}, {}};

    std::ostringstream out;
    Error redirectError{&out};
    data.ambientTexture();
    data.ambientTextureMatrix();
    data.ambientTextureCoordinates();
    data.diffuseTexture();
    data.diffuseTextureMatrix();
    data.diffuseTextureCoordinates();
    data.specularTexture();
    data.specularTextureSwizzle();
    data.specularTextureMatrix();
    data.specularTextureCoordinates();
    data.normalTexture();
    data.normalTextureScale();
    data.normalTextureSwizzle();
    data.normalTextureMatrix();
    data.normalTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::MaterialData::attribute(): attribute AmbientTexture not found in layer 0\n"
        "Trade::PhongMaterialData::ambientTextureMatrix(): the material doesn't have an ambient texture\n"
        "Trade::PhongMaterialData::ambientTextureCoordinates(): the material doesn't have an ambient texture\n"
        "Trade::MaterialData::attribute(): attribute DiffuseTexture not found in layer 0\n"
        "Trade::PhongMaterialData::diffuseTextureMatrix(): the material doesn't have a diffuse texture\n"
        "Trade::PhongMaterialData::diffuseTextureCoordinates(): the material doesn't have a diffuse texture\n"
        "Trade::PhongMaterialData::specularTexture(): the material doesn't have a specular texture\n"
        "Trade::PhongMaterialData::specularTextureSwizzle(): the material doesn't have a specular texture\n"
        "Trade::PhongMaterialData::specularTextureMatrix(): the material doesn't have a specular texture\n"
        "Trade::PhongMaterialData::specularTextureCoordinates(): the material doesn't have a specular texture\n"
        "Trade::MaterialData::attribute(): attribute NormalTexture not found in layer 0\n"
        "Trade::PhongMaterialData::normalTextureScale(): the material doesn't have a normal texture\n"
        "Trade::PhongMaterialData::normalTextureSwizzle(): the material doesn't have a normal texture\n"
        "Trade::PhongMaterialData::normalTextureMatrix(): the material doesn't have a normal texture\n"
        "Trade::PhongMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture\n");
}

void PhongMaterialDataTest::commonTransformationCoordinatesNoTextures() {
    PhongMaterialData a{{}, {}};
    CORRADE_VERIFY(a.hasCommonTextureTransformation());
    CORRADE_VERIFY(a.hasCommonTextureCoordinates());
    CORRADE_COMPARE(a.commonTextureMatrix(), Matrix3{});
    CORRADE_COMPARE(a.commonTextureCoordinates(), 0);

    #ifdef MAGNUM_BUILD_DEPRECATED
    /* textureMatrix() should return the common matrix, if possible, and
       fall back to the global one if not */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(a.textureMatrix(), Matrix3{});
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    PhongMaterialData b{{}, {
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 0.5f})},
        {MaterialAttribute::TextureCoordinates, 7u}
    }};
    CORRADE_VERIFY(b.hasCommonTextureTransformation());
    CORRADE_VERIFY(b.hasCommonTextureCoordinates());
    CORRADE_COMPARE(b.commonTextureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_COMPARE(b.commonTextureCoordinates(), 7);

    #ifdef MAGNUM_BUILD_DEPRECATED
    /* textureMatrix() should return the common matrix, if possible, and
       fall back to the global one if not */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(b.textureMatrix(), Matrix3::scaling({0.5f, 0.5f}));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
}

void PhongMaterialDataTest::commonTransformationCoordinatesOneTexture() {
    Containers::StringView textureName = PhongTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PhongMaterialData data{{}, {
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

    #ifdef MAGNUM_BUILD_DEPRECATED
    /* textureMatrix() should return the common matrix, if possible, and
       fall back to the global one if not */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
}

void PhongMaterialDataTest::commonTransformationCoordinatesOneDifferentTexture() {
    Containers::StringView textureName = PhongTextureData[testCaseInstanceId()];
    setTestCaseDescription(textureName);

    PhongMaterialData data{{}, {
        {MaterialAttribute::AmbientTexture, 2u},
        {MaterialAttribute::DiffuseTexture, 3u},
        {MaterialAttribute::SpecularTexture, 4u},
        {MaterialAttribute::NormalTexture, 5u},
        {std::string{textureName} + "Matrix", Matrix3::scaling({0.5f, 1.0f})},
        {std::string{textureName} + "Coordinates", 17u},

        /* These are used by all textures except the one above, failing the
           check */
        {MaterialAttribute::TextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::TextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());

    #ifdef MAGNUM_BUILD_DEPRECATED
    /* textureMatrix() should return the common matrix, if possible, and
       fall back to the global one if not */
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::translation({0.5f, 0.0f}));
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
}

void PhongMaterialDataTest::noCommonTransformationCoordinates() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PhongMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 3u},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::translation({0.5f, 0.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 3u},
        {MaterialAttribute::SpecularTexture, 4u},
        {MaterialAttribute::SpecularTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::NormalTexture, 5u},
        {MaterialAttribute::NormalTextureCoordinates, 17u}
    }};

    CORRADE_VERIFY(!data.hasCommonTextureTransformation());
    CORRADE_VERIFY(!data.hasCommonTextureCoordinates());

    std::ostringstream out;
    Error redirectError{&out};
    data.commonTextureMatrix();
    data.commonTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::PhongMaterialData::commonTextureMatrix(): the material doesn't have a common texture coordinate transformation\n"
        "Trade::PhongMaterialData::commonTextureCoordinates(): the material doesn't have a common texture coordinate set\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void PhongMaterialDataTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << PhongMaterialData::Flag::AmbientTexture << PhongMaterialData::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::PhongMaterialData::Flag::AmbientTexture Trade::PhongMaterialData::Flag(0xf0)\n");
}

void PhongMaterialDataTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture) << PhongMaterialData::Flags{};
    CORRADE_COMPARE(out.str(), "Trade::PhongMaterialData::Flag::DiffuseTexture|Trade::PhongMaterialData::Flag::SpecularTexture Trade::PhongMaterialData::Flags{}\n");
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::PhongMaterialDataTest)
