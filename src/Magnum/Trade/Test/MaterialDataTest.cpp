/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Trade/PhongMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class MaterialDataTest: public TestSuite::Tester {
    public:
        explicit MaterialDataTest();

        void constructPhong();
        void constructPhongAmbientTexture();
        void constructPhongDiffuseTexture();
        void constructPhongSpecularTexture();
        void constructCopy();
        void constructMovePhongNoAmbientTexture();
        void constructMovePhongNoDiffuseTexture();
        void constructMovePhongNoSpecularTexture();

        void accessInvalidColors();
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
              &MaterialDataTest::constructPhongAmbientTexture,
              &MaterialDataTest::constructPhongDiffuseTexture,
              &MaterialDataTest::constructPhongSpecularTexture,
              &MaterialDataTest::constructCopy,
              &MaterialDataTest::constructMovePhongNoAmbientTexture,
              &MaterialDataTest::constructMovePhongNoDiffuseTexture,
              &MaterialDataTest::constructMovePhongNoSpecularTexture,

              &MaterialDataTest::accessInvalidColors,
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
    PhongMaterialData data{{}, MaterialAlphaMode::Mask, 0.3f, 80.0f, &a};
    data.ambientColor() = 0xccffbb_rgbf;
    data.diffuseColor() = 0xeebbff_rgbf;
    data.specularColor() = 0xacabad_rgbf;

    const PhongMaterialData& cdata = data;

    CORRADE_COMPARE(cdata.type(), MaterialType::Phong);
    CORRADE_COMPARE(cdata.flags(), PhongMaterialData::Flags{});
    CORRADE_COMPARE(cdata.alphaMode(), MaterialAlphaMode::Mask);
    CORRADE_COMPARE(cdata.alphaMask(), 0.3f);
    CORRADE_COMPARE(cdata.ambientColor(), 0xccffbb_rgbf);
    CORRADE_COMPARE(cdata.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(cdata.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(cdata.shininess(), 80.0f);
    CORRADE_COMPARE(cdata.importerState(), &a);
}

void MaterialDataTest::constructPhongAmbientTexture() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{PhongMaterialData::Flag::AmbientTexture, MaterialAlphaMode::Mask, 0.3f, 80.0f, &a};
    data.ambientTexture() = 42;
    data.diffuseColor() = 0xeebbff_rgbf;
    data.specularColor() = 0xacabad_rgbf;

    const PhongMaterialData& cdata = data;

    CORRADE_COMPARE(cdata.type(), MaterialType::Phong);
    CORRADE_COMPARE(cdata.flags(), PhongMaterialData::Flag::AmbientTexture);
    CORRADE_COMPARE(cdata.alphaMode(), MaterialAlphaMode::Mask);
    CORRADE_COMPARE(cdata.alphaMask(), 0.3f);
    CORRADE_COMPARE(cdata.ambientTexture(), 42);
    CORRADE_COMPARE(cdata.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(cdata.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(cdata.shininess(), 80.0f);
    CORRADE_COMPARE(cdata.importerState(), &a);
}

void MaterialDataTest::constructPhongDiffuseTexture() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::DoubleSided, MaterialAlphaMode::Opaque, 0.0f, 80.0f, &a};
    data.ambientColor() = 0xccffbb_rgbf;
    data.diffuseTexture() = 42;
    data.specularColor() = 0xacabad_rgbf;

    const PhongMaterialData& cdata = data;

    CORRADE_COMPARE(cdata.type(), MaterialType::Phong);
    CORRADE_COMPARE(cdata.flags(), PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::DoubleSided);
    CORRADE_COMPARE(cdata.alphaMode(), MaterialAlphaMode::Opaque);
    CORRADE_COMPARE(cdata.alphaMask(), 0.0f);
    CORRADE_COMPARE(cdata.ambientColor(), 0xccffbb_rgbf);
    CORRADE_COMPARE(cdata.diffuseTexture(), 42);
    CORRADE_COMPARE(cdata.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(cdata.shininess(), 80.0f);
    CORRADE_COMPARE(cdata.importerState(), &a);
}

void MaterialDataTest::constructPhongSpecularTexture() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{PhongMaterialData::Flag::SpecularTexture, MaterialAlphaMode::Blend, 1.0f, 30.0f, &a};
    data.ambientColor() = 0xccffbb_rgbf;
    data.diffuseColor() = 0xeebbff_rgbf;
    data.specularTexture() = 42;

    const PhongMaterialData& cdata = data;

    CORRADE_COMPARE(cdata.type(), MaterialType::Phong);
    CORRADE_COMPARE(cdata.flags(), PhongMaterialData::Flag::SpecularTexture);
    CORRADE_COMPARE(cdata.alphaMode(), MaterialAlphaMode::Blend);
    CORRADE_COMPARE(cdata.alphaMask(), 1.0f);
    CORRADE_COMPARE(cdata.ambientColor(), 0xccffbb_rgbf);
    CORRADE_COMPARE(cdata.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(cdata.specularTexture(), 42);
    CORRADE_COMPARE(cdata.shininess(), 30.0f);
    CORRADE_COMPARE(cdata.importerState(), &a);
}

void MaterialDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<AbstractMaterialData, const AbstractMaterialData&>{}));
    CORRADE_VERIFY(!(std::is_constructible<PhongMaterialData, const PhongMaterialData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<AbstractMaterialData, const AbstractMaterialData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<PhongMaterialData, const PhongMaterialData&>{}));
}

void MaterialDataTest::constructMovePhongNoAmbientTexture() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture, {}, {}, 80.0f, &a};
    data.ambientColor() = 0xccffbb_rgbf;
    data.diffuseTexture() = 42;
    data.specularTexture() = 13;

    PhongMaterialData b{std::move(data)};

    CORRADE_COMPARE(b.flags(), (PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture));
    CORRADE_COMPARE(b.ambientColor(), 0xccffbb_rgbf);
    CORRADE_COMPARE(b.diffuseTexture(), 42);
    CORRADE_COMPARE(b.specularTexture(), 13);
    CORRADE_COMPARE(b.shininess(), 80.0f);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    PhongMaterialData d{PhongMaterialData::Flag::AmbientTexture, {}, {}, 100.0f, &c};
    d.ambientTexture() = 42;
    d.diffuseColor() = 0xeebbff_rgbf;
    d.specularColor() = 0xacabad_rgbf;
    d = std::move(b);

    CORRADE_COMPARE(d.flags(), PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture);
    CORRADE_COMPARE(d.ambientColor(), 0xccffbb_rgbf);
    CORRADE_COMPARE(d.diffuseTexture(), 42);
    CORRADE_COMPARE(d.specularTexture(), 13);
    CORRADE_COMPARE(d.shininess(), 80.0f);
    CORRADE_COMPARE(d.importerState(), &a);
}

void MaterialDataTest::constructMovePhongNoDiffuseTexture() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture, {}, {}, 80.0f, &a};
    data.ambientTexture() = 42;
    data.diffuseColor() = 0xeebbff_rgbf;
    data.specularTexture() = 13;

    PhongMaterialData b{std::move(data)};

    CORRADE_COMPARE(b.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture);
    CORRADE_COMPARE(b.ambientTexture(), 42);
    CORRADE_COMPARE(b.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(b.specularTexture(), 13);
    CORRADE_COMPARE(b.shininess(), 80.0f);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    PhongMaterialData d{PhongMaterialData::Flag::DiffuseTexture, {}, {}, 100.0f, &c};
    d.ambientColor() = 0xccffbb_rgbf;
    d.diffuseTexture() = 42;
    d.specularColor() = 0xacabad_rgbf;
    d = std::move(b);

    CORRADE_COMPARE(d.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::SpecularTexture);
    CORRADE_COMPARE(d.ambientTexture(), 42);
    CORRADE_COMPARE(d.diffuseColor(), 0xeebbff_rgbf);
    CORRADE_COMPARE(d.specularTexture(), 13);
    CORRADE_COMPARE(d.shininess(), 80.0f);
    CORRADE_COMPARE(d.importerState(), &a);
}

void MaterialDataTest::constructMovePhongNoSpecularTexture() {
    using namespace Math::Literals;

    const int a{};
    PhongMaterialData data{PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture, {}, {}, 80.0f, &a};
    data.ambientTexture() = 13;
    data.diffuseTexture() = 42;
    data.specularColor() = 0xacabad_rgbf;

    PhongMaterialData b{std::move(data)};

    CORRADE_COMPARE(b.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture);
    CORRADE_COMPARE(b.ambientTexture(), 13);
    CORRADE_COMPARE(b.diffuseTexture(), 42);
    CORRADE_COMPARE(b.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(b.shininess(), 80.0f);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    PhongMaterialData d{PhongMaterialData::Flag::SpecularTexture, {}, {}, 30.0f, &c};
    d.ambientColor() = 0xccffbb_rgbf;
    d.diffuseColor() = 0xeebbff_rgbf;
    d.specularTexture() = 42;
    d = std::move(b);

    CORRADE_COMPARE(d.flags(), PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture);
    CORRADE_COMPARE(d.ambientTexture(), 13);
    CORRADE_COMPARE(d.diffuseTexture(), 42);
    CORRADE_COMPARE(d.specularColor(), 0xacabad_rgbf);
    CORRADE_COMPARE(d.shininess(), 80.0f);
    CORRADE_COMPARE(d.importerState(), &a);
}

void MaterialDataTest::accessInvalidColors() {
    std::ostringstream out;
    Error redirectError{&out};

    PhongMaterialData a{PhongMaterialData::Flag::AmbientTexture|PhongMaterialData::Flag::DiffuseTexture|PhongMaterialData::Flag::SpecularTexture, {}, {}, 80.0f};

    a.ambientColor();
    a.diffuseColor();
    a.specularColor();

    CORRADE_COMPARE(out.str(),
        "Trade::PhongMaterialData::ambientColor(): the material has ambient texture\n"
        "Trade::PhongMaterialData::diffuseColor(): the material has diffuse texture\n"
        "Trade::PhongMaterialData::specularColor(): the material has specular texture\n");
}

void MaterialDataTest::accessInvalidTextures() {
    std::ostringstream out;
    Error redirectError{&out};

    PhongMaterialData a{PhongMaterialData::Flags{}, {}, {}, 80.0f};

    a.ambientTexture();
    a.diffuseTexture();
    a.specularTexture();

    CORRADE_COMPARE(out.str(),
        "Trade::PhongMaterialData::ambientTexture(): the material doesn't have ambient texture\n"
        "Trade::PhongMaterialData::diffuseTexture(): the material doesn't have diffuse texture\n"
        "Trade::PhongMaterialData::specularTexture(): the material doesn't have specular texture\n");
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
