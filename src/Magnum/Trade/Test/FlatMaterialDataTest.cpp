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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Trade/FlatMaterialData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class FlatMaterialDataTest: public TestSuite::Tester {
    public:
        explicit FlatMaterialDataTest();

        void baseColor();
        void diffuseColor();
        void defaults();
        void texturedBaseColor();
        void texturedDiffuseColor();
        void texturedDefaults();
        void texturedBaseColorSingleMatrixCoordinates();
        void texturedDiffuseColorSingleMatrixCoordinates();
        void texturedMismatchedMatrixCoordinates();
        void invalidTextures();
};

FlatMaterialDataTest::FlatMaterialDataTest() {
    addTests({&FlatMaterialDataTest::baseColor,
              &FlatMaterialDataTest::diffuseColor,
              &FlatMaterialDataTest::defaults,
              &FlatMaterialDataTest::texturedBaseColor,
              &FlatMaterialDataTest::texturedDiffuseColor,
              &FlatMaterialDataTest::texturedDefaults,
              &FlatMaterialDataTest::texturedBaseColorSingleMatrixCoordinates,
              &FlatMaterialDataTest::texturedDiffuseColorSingleMatrixCoordinates,
              &FlatMaterialDataTest::texturedMismatchedMatrixCoordinates,
              &FlatMaterialDataTest::invalidTextures});
}

using namespace Math::Literals;

void FlatMaterialDataTest::baseColor() {
    MaterialData base{MaterialType::Flat, {
        {MaterialAttribute::BaseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseColor, 0x33556600_rgbaf}, /* Ignored */
    }};

    CORRADE_COMPARE(base.types(), MaterialType::Flat);
    const auto& data = base.as<FlatMaterialData>();

    CORRADE_VERIFY(!data.hasTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
}

void FlatMaterialDataTest::diffuseColor() {
    MaterialData base{MaterialType::Flat, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
    }};

    CORRADE_COMPARE(base.types(), MaterialType::Flat);
    const auto& data = base.as<FlatMaterialData>();

    CORRADE_VERIFY(!data.hasTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
}

void FlatMaterialDataTest::defaults() {
    MaterialData base{{}, {}};

    CORRADE_COMPARE(base.types(), MaterialTypes{});
    /* Casting is fine even if the type doesn't include Flat */
    const auto& data = base.as<FlatMaterialData>();

    CORRADE_VERIFY(!data.hasTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xffffff_rgbf);
}

void FlatMaterialDataTest::texturedBaseColor() {
    FlatMaterialData data{{}, {
        {MaterialAttribute::BaseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::BaseColorTexture, 5u},
        {MaterialAttribute::BaseColorTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::BaseColorTextureCoordinates, 2u},

        /* All this is ignored */
        {MaterialAttribute::DiffuseColor, 0x33556600_rgbaf},
        {MaterialAttribute::DiffuseTexture, 6u},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::translation({0.5f, 1.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(data.hasTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.texture(), 5);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.textureCoordinates(), 2);
}

void FlatMaterialDataTest::texturedDiffuseColor() {
    FlatMaterialData data{{}, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseTexture, 5u},
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 2u},

        /* Ignored, as we have a diffuse texture */
        {MaterialAttribute::BaseColor, 0x33556600_rgbaf}
    }};

    CORRADE_VERIFY(data.hasTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.texture(), 5);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.textureCoordinates(), 2);
}

void FlatMaterialDataTest::texturedDefaults() {
    FlatMaterialData data{{}, {
        {MaterialAttribute::DiffuseTexture, 5u}
    }};

    CORRADE_VERIFY(data.hasTexture());
    CORRADE_VERIFY(!data.hasTextureTransformation());
    CORRADE_VERIFY(!data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xffffff_rgbf);
    CORRADE_COMPARE(data.texture(), 5);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
    CORRADE_COMPARE(data.textureCoordinates(), 0);
}

void FlatMaterialDataTest::texturedBaseColorSingleMatrixCoordinates() {
    FlatMaterialData data{{}, {
        {MaterialAttribute::BaseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::BaseColorTexture, 5u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::TextureCoordinates, 2u},

        /* This is ignored because it doesn't match the texture */
        {MaterialAttribute::DiffuseTextureMatrix, Matrix3::translation({0.5f, 1.0f})},
        {MaterialAttribute::DiffuseTextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(data.hasTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.texture(), 5);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.textureCoordinates(), 2);
}

void FlatMaterialDataTest::texturedDiffuseColorSingleMatrixCoordinates() {
    FlatMaterialData data{{}, {
        {MaterialAttribute::DiffuseColor, 0xccffbbff_rgbaf},
        {MaterialAttribute::DiffuseTexture, 5u},
        {MaterialAttribute::TextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
        {MaterialAttribute::TextureCoordinates, 2u},

        /* This is ignored because it doesn't match the texture */
        {MaterialAttribute::BaseColorTextureMatrix, Matrix3::translation({0.5f, 1.0f})},
        {MaterialAttribute::BaseColorTextureCoordinates, 3u}
    }};

    CORRADE_VERIFY(data.hasTexture());
    CORRADE_VERIFY(data.hasTextureTransformation());
    CORRADE_VERIFY(data.hasTextureCoordinates());
    CORRADE_COMPARE(data.color(), 0xccffbb_rgbf);
    CORRADE_COMPARE(data.texture(), 5);
    CORRADE_COMPARE(data.textureMatrix(), Matrix3::scaling({0.5f, 1.0f}));
    CORRADE_COMPARE(data.textureCoordinates(), 2);
}

void FlatMaterialDataTest::texturedMismatchedMatrixCoordinates() {
    {
        FlatMaterialData data{{}, {
            {MaterialAttribute::BaseColorTexture, 5u},

            /* This is ignored because it doesn't match the texture */
            {MaterialAttribute::DiffuseColor, 0x33556600_rgbaf},
            {MaterialAttribute::DiffuseTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::DiffuseTextureCoordinates, 2u},
        }};

        CORRADE_VERIFY(data.hasTexture());
        CORRADE_VERIFY(!data.hasTextureTransformation());
        CORRADE_VERIFY(!data.hasTextureCoordinates());
        CORRADE_COMPARE(data.color(), 0xffffff_rgbf);
        CORRADE_COMPARE(data.texture(), 5);
        CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.textureCoordinates(), 0);
    } {
        FlatMaterialData data{{}, {
            {MaterialAttribute::DiffuseTexture, 5u},

            /* This is ignored because it doesn't match the texture */
            {MaterialAttribute::BaseColor, 0x33556600_rgbaf},
            {MaterialAttribute::BaseColorTextureMatrix, Matrix3::scaling({0.5f, 1.0f})},
            {MaterialAttribute::BaseColorTextureCoordinates, 2u},
        }};

        CORRADE_VERIFY(data.hasTexture());
        CORRADE_VERIFY(!data.hasTextureTransformation());
        CORRADE_VERIFY(!data.hasTextureCoordinates());
        CORRADE_COMPARE(data.color(), 0xffffff_rgbf);
        CORRADE_COMPARE(data.texture(), 5);
        CORRADE_COMPARE(data.textureMatrix(), Matrix3{});
        CORRADE_COMPARE(data.textureCoordinates(), 0);
    }
}

void FlatMaterialDataTest::invalidTextures() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    FlatMaterialData data{{}, {}};

    std::ostringstream out;
    Error redirectError{&out};
    data.texture();
    data.textureMatrix();
    data.textureCoordinates();
    CORRADE_COMPARE(out.str(),
        "Trade::FlatMaterialData::texture(): the material doesn't have a texture\n"
        "Trade::FlatMaterialData::textureMatrix(): the material doesn't have a texture\n"
        "Trade::FlatMaterialData::textureCoordinates(): the material doesn't have a texture\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::FlatMaterialDataTest)
