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
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/Shaders/Flat.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct FlatGLTest: GL::OpenGLTester {
    explicit FlatGLTest();

    template<UnsignedInt dimensions> void construct();

    template<UnsignedInt dimensions> void constructMove();

    template<UnsignedInt dimensions> void bindTexture();
    template<UnsignedInt dimensions> void bindTextureNotEnabled();

    template<UnsignedInt dimensions> void setAlphaMask();
    template<UnsignedInt dimensions> void setAlphaMaskNotEnabled();
};

constexpr struct {
    const char* name;
    Flat2D::Flags flags;
} ConstructData[]{
    {"", {}},
    {"textured", Flat2D::Flag::Textured}
};

FlatGLTest::FlatGLTest() {
    addInstancedTests<FlatGLTest>({
        &FlatGLTest::construct<2>,
        &FlatGLTest::construct<3>},
        Containers::arraySize(ConstructData));

    addTests<FlatGLTest>({
        &FlatGLTest::constructMove<2>,
        &FlatGLTest::constructMove<3>,

        &FlatGLTest::bindTexture<2>,
        &FlatGLTest::bindTexture<3>,
        &FlatGLTest::bindTextureNotEnabled<2>,
        &FlatGLTest::bindTextureNotEnabled<3>,

        &FlatGLTest::setAlphaMask<2>,
        &FlatGLTest::setAlphaMask<3>,
        &FlatGLTest::setAlphaMaskNotEnabled<2>,
        &FlatGLTest::setAlphaMaskNotEnabled<3>});
}

template<UnsignedInt dimensions> void FlatGLTest::construct() {
    setTestCaseName(Utility::formatString("construct<{}>", dimensions));

    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Flat<dimensions> shader{data.flags};
    CORRADE_COMPARE(shader.flags(), data.flags);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

template<UnsignedInt dimensions> void FlatGLTest::constructMove() {
    setTestCaseName(Utility::formatString("constructMove<{}>", dimensions));

    Flat<dimensions> a{Flat<dimensions>::Flag::Textured};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Flat<dimensions> b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), Flat<dimensions>::Flag::Textured);
    CORRADE_VERIFY(!a.id());

    Flat<dimensions> c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), Flat<dimensions>::Flag::Textured);
    CORRADE_VERIFY(!b.id());
}

template<UnsignedInt dimensions> void FlatGLTest::bindTexture() {
    setTestCaseName(Utility::formatString("bindTexture<{}>", dimensions));

    char data[4];

    GL::Texture2D texture;
    texture
        .setMinificationFilter(SamplerFilter::Linear, SamplerMipmap::Linear)
        .setMagnificationFilter(SamplerFilter::Linear)
        .setWrapping(SamplerWrapping::ClampToEdge)
        .setImage(0, GL::TextureFormat::RGBA, ImageView2D{PixelFormat::RGBA8Unorm, {1, 1}, data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Test just that no assertion is fired */
    Flat<dimensions> shader{Flat<dimensions>::Flag::Textured};
    shader.bindTexture(texture);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void FlatGLTest::bindTextureNotEnabled() {
    setTestCaseName(Utility::formatString("bindTextureNotEnabled<{}>", dimensions));

    std::ostringstream out;
    Error redirectError{&out};

    GL::Texture2D texture;
    Flat<dimensions> shader;
    shader.bindTexture(texture);

    CORRADE_COMPARE(out.str(), "Shaders::Flat::bindTexture(): the shader was not created with texturing enabled\n");
}

template<UnsignedInt dimensions> void FlatGLTest::setAlphaMask() {
    /* Test just that no assertion is fired */
    Flat<dimensions> shader{Flat<dimensions>::Flag::AlphaMask};
    shader.setAlphaMask(0.25f);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void FlatGLTest::setAlphaMaskNotEnabled() {
    std::ostringstream out;
    Error redirectError{&out};

    Flat<dimensions> shader;
    shader.setAlphaMask(0.75f);

    CORRADE_COMPARE(out.str(),
        "Shaders::Flat::setAlphaMask(): the shader was not created with alpha mask enabled\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::FlatGLTest)
