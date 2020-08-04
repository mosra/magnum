/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Trade/PhongMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class MaterialDataTest: public TestSuite::Tester {
    public:
        explicit MaterialDataTest();

        void constructPhong();
        void constructPhongTextured();
        void constructPhongTexturedTextureTransform();
        void constructPhongTexturedCoordinates();
        void constructPhongTextureTransformNoTextures();
        void constructPhongNoTextureTransformationFlag();
        void constructPhongNoTextureCoordinatesFlag();
        void constructCopy();
        void constructMovePhong();

        void accessInvalidTextures();

        void debugType();
        void debugFlag();
        void debugFlags();
        void debugAlphaMode();

        void debugPhongFlag();
        void debugPhongFlags();
};

MaterialDataTest::MaterialDataTest() {
    addTests({&MaterialDataTest::constructPhong,
              &MaterialDataTest::constructPhongTextured,
              &MaterialDataTest::constructPhongTexturedTextureTransform,
              &MaterialDataTest::constructPhongTexturedCoordinates,
              &MaterialDataTest::constructPhongTextureTransformNoTextures,
              &MaterialDataTest::constructPhongNoTextureTransformationFlag,
              &MaterialDataTest::constructPhongNoTextureCoordinatesFlag,
              &MaterialDataTest::constructCopy,
              &MaterialDataTest::constructMovePhong,

              &MaterialDataTest::accessInvalidTextures,

              &MaterialDataTest::debugType,
              &MaterialDataTest::debugFlag,
              &MaterialDataTest::debugFlags,
              &MaterialDataTest::debugAlphaMode,

              &MaterialDataTest::debugPhongFlag,
              &MaterialDataTest::debugPhongFlags});
}

void MaterialDataTest::constructPhong() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{{},
        0xccffbb_rgbf, {},
        0xebefbf_rgbf, {},
        0xacabad_rgbf, {}, {}, {},
        MaterialAlphaMode::Mask, 0.3f, 80.0f, &a};

    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flags{});
    CORRADE_COMPARE(data.ambientColor(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xebefbf_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Mask);
    CORRADE_COMPARE(data.alphaMask(), 0.3f);
    CORRADE_COMPARE(data.shininess(), 80.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void MaterialDataTest::constructPhongTextured() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{
        PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture,
        0x111111_rgbf, 42,
        0xeebbff_rgbf, {},
        0xacabad_rgbf, 17, {}, {},
        MaterialAlphaMode::Blend, 0.37f, 96.0f, &a};

    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture);
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.ambientTexture(), 42);
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 0);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureCoordinates(), 0);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Blend);
    CORRADE_COMPARE(data.alphaMask(), 0.37f);
    CORRADE_COMPARE(data.shininess(), 96.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void MaterialDataTest::constructPhongTexturedTextureTransform() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{
        PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureTransformation,
        0x111111_rgbf, {},
        0xeebbff_rgbf, 42,
        0xacabad_rgbf, {}, 17,
        Matrix3::rotation(90.0_degf),
        MaterialAlphaMode::Opaque, 0.5f, 96.0f, &a};

    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureTransformation);
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.diffuseTexture(), 42);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.normalTexture(), 17);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::rotation(90.0_degf));
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Opaque);
    CORRADE_COMPARE(data.alphaMask(), 0.5f);
    CORRADE_COMPARE(data.shininess(), 96.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void MaterialDataTest::constructPhongTexturedCoordinates() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{
        PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::TextureCoordinates,
        0x111111_rgbf, 42, 3,
        0xeebbff_rgbf, {}, 0,
        0xacabad_rgbf, 17, 1,
        {}, 0, {},
        MaterialAlphaMode::Blend, 0.37f, 96.0f, &a};

    CORRADE_COMPARE(data.type(), MaterialType::Phong);
    CORRADE_COMPARE(data.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::TextureCoordinates);
    CORRADE_COMPARE(data.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(data.ambientTexture(), 42);
    CORRADE_COMPARE(data.ambientTextureCoordinates(), 3);
    CORRADE_COMPARE(data.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(data.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(data.specularTexture(), 17);
    CORRADE_COMPARE(data.specularTextureCoordinates(), 1);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.alphaMode(), MaterialAlphaMode::Blend);
    CORRADE_COMPARE(data.alphaMask(), 0.37f);
    CORRADE_COMPARE(data.shininess(), 96.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void MaterialDataTest::constructPhongTextureTransformNoTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    PhongMaterialData a{PhongMaterialData::Flag::TextureTransformation,
        {}, {},
        {}, {},
        {}, {}, {}, {},
        {}, 0.5f, 80.0f};
    CORRADE_COMPARE(out.str(),
        "Trade::PhongMaterialData: texture transformation enabled but the material has no textures\n");
}

void MaterialDataTest::constructPhongNoTextureTransformationFlag() {
    using namespace Math::Literals;

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    PhongMaterialData a{{},
        {}, {},
        {}, {},
        {}, {}, {}, Matrix3::rotation(90.0_degf),
        {}, 0.5f, 80.0f};
    CORRADE_COMPARE(out.str(),
        "PhongMaterialData::PhongMaterialData: non-default texture matrix requires Flag::TextureTransformation to be enabled\n");
}

void MaterialDataTest::constructPhongNoTextureCoordinatesFlag() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    PhongMaterialData a{{},
        {}, {}, 1,
        {}, {}, 2,
        {}, {}, 3, {}, 4, {},
        {}, 0.5f, 80.0f};
    CORRADE_COMPARE(out.str(),
        "PhongMaterialData::PhongMaterialData: non-zero texture coordinate sets require Flag::TextureCoordinates to be enabled\n");
}

void MaterialDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<AbstractMaterialData, const AbstractMaterialData&>{}));
    CORRADE_VERIFY(!(std::is_constructible<PhongMaterialData, const PhongMaterialData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<AbstractMaterialData, const AbstractMaterialData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<PhongMaterialData, const PhongMaterialData&>{}));
}

void MaterialDataTest::constructMovePhong() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{
        PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureTransformation|PhongMaterialData::Flag::TextureCoordinates,
        0x111111_rgbf, 1, 0,
        0xeebbff_rgbf, 42, 1,
        0xacabad_rgbf, 24, 2, 17, 3,
        Matrix3::rotation(90.0_degf),
        MaterialAlphaMode::Blend, 0.55f, 13.0f, &a};

    PhongMaterialData b{std::move(data)};
    CORRADE_COMPARE(b.type(), MaterialType::Phong);
    CORRADE_COMPARE(b.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureTransformation|PhongMaterialData::Flag::TextureCoordinates);
    CORRADE_COMPARE(b.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(b.ambientTexture(), 1);
    CORRADE_COMPARE(b.ambientTextureCoordinates(), 0);
    CORRADE_COMPARE(b.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(b.diffuseTexture(), 42);
    CORRADE_COMPARE(b.diffuseTextureCoordinates(), 1);
    CORRADE_COMPARE(b.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(b.specularTexture(), 24);
    CORRADE_COMPARE(b.specularTextureCoordinates(), 2);
    CORRADE_COMPARE(b.normalTexture(), 17);
    CORRADE_COMPARE(b.normalTextureCoordinates(), 3);
    CORRADE_COMPARE(b.textureMatrix(), Matrix3::rotation(90.0_degf));
    CORRADE_COMPARE(b.alphaMode(), MaterialAlphaMode::Blend);
    CORRADE_COMPARE(b.alphaMask(), 0.55f);
    CORRADE_COMPARE(b.shininess(), 13.0f);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    PhongMaterialData d{{},
        0xccffbb_rgbf, {},
        0xebefbf_rgbf, {},
        0xacabad_rgbf, {}, {}, {},
        MaterialAlphaMode::Mask, 0.3f, 80.0f, &c};
    d = std::move(b);

    CORRADE_COMPARE(d.type(), MaterialType::Phong);
    CORRADE_COMPARE(d.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture|PhongMaterialData::Flag::NormalTexture|PhongMaterialData::Flag::TextureTransformation|PhongMaterialData::Flag::TextureCoordinates);
    CORRADE_COMPARE(d.ambientColor(), 0x111111_rgbf);
    CORRADE_COMPARE(d.ambientTexture(), 1);
    CORRADE_COMPARE(d.ambientTextureCoordinates(), 0);
    CORRADE_COMPARE(d.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(d.diffuseTexture(), 42);
    CORRADE_COMPARE(d.diffuseTextureCoordinates(), 1);
    CORRADE_COMPARE(d.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(d.specularTexture(), 24);
    CORRADE_COMPARE(d.specularTextureCoordinates(), 2);
    CORRADE_COMPARE(d.normalTexture(), 17);
    CORRADE_COMPARE(d.normalTextureCoordinates(), 3);
    CORRADE_COMPARE(d.textureMatrix(), Matrix3::rotation(90.0_degf));
    CORRADE_COMPARE(d.alphaMode(), MaterialAlphaMode::Blend);
    CORRADE_COMPARE(d.alphaMask(), 0.55f);
    CORRADE_COMPARE(d.shininess(), 13.0f);
    CORRADE_COMPARE(d.importerState(), &a);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<PhongMaterialData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<PhongMaterialData>::value);
}

void MaterialDataTest::accessInvalidTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PhongMaterialData a{{},
        {}, {},
        {}, {},
        {}, {}, {}, {},
        {}, 0.5f, 80.0f};

    std::ostringstream out;
    Error redirectError{&out};
    a.ambientTexture();
    a.ambientTextureCoordinates();
    a.diffuseTexture();
    a.diffuseTextureCoordinates();
    a.specularTexture();
    a.specularTextureCoordinates();
    a.normalTexture();
    a.normalTextureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::PhongMaterialData::ambientTexture(): the material doesn't have an ambient texture\n"
        "Trade::PhongMaterialData::ambientTextureCoordinates(): the material doesn't have an ambient texture\n"
        "Trade::PhongMaterialData::diffuseTexture(): the material doesn't have a diffuse texture\n"
        "Trade::PhongMaterialData::diffuseTextureCoordinates(): the material doesn't have a diffuse texture\n"
        "Trade::PhongMaterialData::specularTexture(): the material doesn't have a specular texture\n"
        "Trade::PhongMaterialData::specularTextureCoordinates(): the material doesn't have a specular texture\n"
        "Trade::PhongMaterialData::normalTexture(): the material doesn't have a normal texture\n"
        "Trade::PhongMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture\n");
}

void MaterialDataTest::debugType() {
    std::ostringstream out;

    Debug(&out) << MaterialType::Phong << MaterialType(0xbe);
    CORRADE_COMPARE(out.str(), "Trade::MaterialType::Phong Trade::MaterialType(0xbe)\n");
}

void MaterialDataTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << AbstractMaterialData::Flag::DoubleSided << AbstractMaterialData::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractMaterialData::Flag::DoubleSided Trade::AbstractMaterialData::Flag(0xf0)\n");
}

void MaterialDataTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << AbstractMaterialData::Flag::DoubleSided << AbstractMaterialData::Flags{};
    CORRADE_COMPARE(out.str(), "Trade::AbstractMaterialData::Flag::DoubleSided Trade::AbstractMaterialData::Flags{}\n");
}

void MaterialDataTest::debugAlphaMode() {
    std::ostringstream out;

    Debug{&out} << MaterialAlphaMode::Opaque << MaterialAlphaMode(0xee);
    CORRADE_COMPARE(out.str(), "Trade::MaterialAlphaMode::Opaque Trade::MaterialAlphaMode(0xee)\n");
}

void MaterialDataTest::debugPhongFlag() {
    std::ostringstream out;

    Debug{&out} << PhongMaterialData::Flag::AmbientTexture << PhongMaterialData::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::PhongMaterialData::Flag::AmbientTexture Trade::PhongMaterialData::Flag(0xf0)\n");
}

void MaterialDataTest::debugPhongFlags() {
    std::ostringstream out;

    Debug{&out} << (PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture) << PhongMaterialData::Flags{};
    CORRADE_COMPARE(out.str(), "Trade::PhongMaterialData::Flag::DiffuseTexture|Trade::PhongMaterialData::Flag::SpecularTexture Trade::PhongMaterialData::Flags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::MaterialDataTest)
