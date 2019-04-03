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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/ImageView.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Shaders/Phong.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct PhongGLTest: GL::OpenGLTester {
    explicit PhongGLTest();

    void construct();

    void constructMove();

    void bindTextures();
    void bindTexturesNotEnabled();

    void setAlphaMask();
    void setAlphaMaskNotEnabled();

    void setWrongLightCount();
    void setWrongLightId();
};

constexpr struct {
    const char* name;
    Phong::Flags flags;
    UnsignedInt lightCount;
} ConstructData[]{
    {"", {}, 1},
    {"ambient texture", Phong::Flag::AmbientTexture, 1},
    {"diffuse texture", Phong::Flag::DiffuseTexture, 1},
    {"specular texture", Phong::Flag::SpecularTexture, 1},
    {"ambient + diffuse texture", Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture, 1},
    {"ambient + specular texture", Phong::Flag::AmbientTexture|Phong::Flag::SpecularTexture, 1},
    {"diffuse + specular texture", Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture, 1},
    {"ambient + diffuse + specular texture", Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture, 1},
    {"alpha mask", Phong::Flag::AlphaMask, 1},
    {"alpha mask + diffuse texture", Phong::Flag::AlphaMask|Phong::Flag::DiffuseTexture, 1},
    {"five lights", {}, 5}
};

PhongGLTest::PhongGLTest() {
    addInstancedTests({&PhongGLTest::construct}, Containers::arraySize(ConstructData));

    addTests({&PhongGLTest::constructMove,

              &PhongGLTest::bindTextures,
              &PhongGLTest::bindTexturesNotEnabled,

              &PhongGLTest::setAlphaMask,
              &PhongGLTest::setAlphaMaskNotEnabled,

              &PhongGLTest::setWrongLightCount,
              &PhongGLTest::setWrongLightId});
}

void PhongGLTest::construct() {
    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Phong shader{data.flags, data.lightCount};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_COMPARE(shader.lightCount(), data.lightCount);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void PhongGLTest::constructMove() {
    Phong a{Phong::Flag::AlphaMask, 3};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Phong b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), Phong::Flag::AlphaMask);
    CORRADE_COMPARE(b.lightCount(), 3);
    CORRADE_VERIFY(!a.id());

    Phong c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), Phong::Flag::AlphaMask);
    CORRADE_COMPARE(c.lightCount(), 3);
    CORRADE_VERIFY(!b.id());
}

void PhongGLTest::bindTextures() {
    char data[4];

    GL::Texture2D texture;
    texture
        .setMinificationFilter(SamplerFilter::Linear, SamplerMipmap::Linear)
        .setMagnificationFilter(SamplerFilter::Linear)
        .setWrapping(SamplerWrapping::ClampToEdge)
        .setImage(0, GL::TextureFormat::RGBA, ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Test just that no assertion is fired */
    Phong shader{Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture};
    shader.bindAmbientTexture(texture)
          .bindDiffuseTexture(texture)
          .bindSpecularTexture(texture)
          .bindTextures(&texture, &texture, &texture);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void PhongGLTest::bindTexturesNotEnabled() {
    std::ostringstream out;
    Error redirectError{&out};

    GL::Texture2D texture;
    Phong shader;
    shader.bindAmbientTexture(texture)
          .bindDiffuseTexture(texture)
          .bindSpecularTexture(texture)
          .bindTextures(&texture, &texture, &texture);

    CORRADE_COMPARE(out.str(),
        "Shaders::Phong::bindAmbientTexture(): the shader was not created with ambient texture enabled\n"
        "Shaders::Phong::bindDiffuseTexture(): the shader was not created with diffuse texture enabled\n"
        "Shaders::Phong::bindSpecularTexture(): the shader was not created with specular texture enabled\n"
        "Shaders::Phong::bindTextures(): the shader was not created with any textures enabled\n");
}

void PhongGLTest::setAlphaMask() {
    /* Test just that no assertion is fired */
    Phong shader{Phong::Flag::AlphaMask};
    shader.setAlphaMask(0.25f);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void PhongGLTest::setAlphaMaskNotEnabled() {
    std::ostringstream out;
    Error redirectError{&out};

    Phong shader;
    shader.setAlphaMask(0.75f);

    CORRADE_COMPARE(out.str(),
        "Shaders::Phong::setAlphaMask(): the shader was not created with alpha mask enabled\n");
}

void PhongGLTest::setWrongLightCount() {
    std::ostringstream out;
    Error redirectError{&out};

    Phong shader{{}, 5};

    /* This is okay */
    shader.setLightColors({{}, {}, {}, {}, {}})
        .setLightPositions({{}, {}, {}, {}, {}});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* This is not */
    shader.setLightColor({})
        .setLightPosition({});

    CORRADE_COMPARE(out.str(),
        "Shaders::Phong::setLightColors(): expected 5 items but got 1\n"
        "Shaders::Phong::setLightPositions(): expected 5 items but got 1\n");
}

void PhongGLTest::setWrongLightId() {
    std::ostringstream out;
    Error redirectError{&out};

    Phong shader{{}, 3};

    /* This is okay */
    shader.setLightColor(2, {})
        .setLightPosition(2, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* This is not */
    shader.setLightColor(3, {})
        .setLightPosition(3, {});

    CORRADE_COMPARE(out.str(),
        "Shaders::Phong::setLightColor(): light ID 3 is out of bounds for 3 lights\n"
        "Shaders::Phong::setLightPosition(): light ID 3 is out of bounds for 3 lights\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::PhongGLTest)
