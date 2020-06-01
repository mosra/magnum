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

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/TextureImage.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/CubeMapTexture.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Range.h"

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/DebugTools/BufferData.h"
#include "Magnum/GL/BufferImage.h"
#endif

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct TextureImageGLTest: GL::OpenGLTester {
    explicit TextureImageGLTest();

    void subImage2D();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void subImage2DBuffer();
    #endif
    void subImage2DGeneric();

    void subImageCube();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void subImageCubeBuffer();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void subImage2DUInt();
    void subImage2DFloat();
    void subImage2DFloatGeneric();
    #endif
};

TextureImageGLTest::TextureImageGLTest() {
    addTests({&TextureImageGLTest::subImage2D,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &TextureImageGLTest::subImage2DBuffer,
              #endif
              &TextureImageGLTest::subImage2DGeneric,

              &TextureImageGLTest::subImageCube,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &TextureImageGLTest::subImageCubeBuffer,
              #endif

              #ifndef MAGNUM_TARGET_GLES2
              &TextureImageGLTest::subImage2DUInt,
              &TextureImageGLTest::subImage2DFloat,
              &TextureImageGLTest::subImage2DFloatGeneric,
              #endif
              });
}

constexpr UnsignedByte Data2D[] = { 0x00, 0x01, 0x02, 0x03,
                                    0x04, 0x05, 0x06, 0x07,
                                    0x08, 0x09, 0x0a, 0x0b,
                                    0x0c, 0x0d, 0x0e, 0x0f };

void TextureImageGLTest::subImage2D() {
    GL::Texture2D texture;
    texture.setImage(0,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        GL::TextureFormat::RGBA8,
        #else
        GL::TextureFormat::RGBA,
        #endif
        ImageView2D{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, Vector2i{2}, Data2D});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), pixelFormatWrap(GL::PixelFormat::RGBA));
    CORRADE_COMPARE(GL::PixelType(image.formatExtra()), GL::PixelType::UnsignedByte);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureImageGLTest::subImage2DBuffer() {
    GL::Texture2D texture;
    texture.setImage(0, GL::TextureFormat::RGBA8, ImageView2D{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, Vector2i{2}, Data2D});

    GL::BufferImage2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte}, GL::BufferUsage::StaticRead);
    Containers::Array<UnsignedByte> data = bufferData<UnsignedByte>(image.buffer());
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), GL::PixelFormat::RGBA);
    CORRADE_COMPARE(image.type(), GL::PixelType::UnsignedByte);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(data, Containers::arrayView(Data2D),
        TestSuite::Compare::Container);
}
#endif

void TextureImageGLTest::subImage2DGeneric() {
    GL::Texture2D texture;
    texture.setImage(0,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        GL::TextureFormat::RGBA8,
        #else
        GL::TextureFormat::RGBA,
        #endif
        ImageView2D{PixelFormat::RGBA8Unorm, Vector2i{2}, Data2D});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {PixelFormat::RGBA8Unorm});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(image.formatExtra(), 0);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

void TextureImageGLTest::subImageCube() {
    ImageView2D view{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, Vector2i{2}, Data2D};

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    constexpr GL::TextureFormat format = GL::TextureFormat::RGBA8;
    #else
    constexpr GL::TextureFormat format = GL::TextureFormat::RGBA;
    #endif

    GL::CubeMapTexture texture;
    texture.setImage(GL::CubeMapCoordinate::PositiveX, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeX, 0, format, view)
           .setImage(GL::CubeMapCoordinate::PositiveY, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeY, 0, format, view)
           .setImage(GL::CubeMapCoordinate::PositiveZ, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeZ, 0, format, view);

    Image2D image = textureSubImage(texture, GL::CubeMapCoordinate::PositiveX, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), pixelFormatWrap(GL::PixelFormat::RGBA));
    CORRADE_COMPARE(GL::PixelType(image.formatExtra()), GL::PixelType::UnsignedByte);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureImageGLTest::subImageCubeBuffer() {
    ImageView2D view{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, Vector2i{2}, Data2D};

    GL::CubeMapTexture texture;
    texture.setImage(GL::CubeMapCoordinate::PositiveX, 0, GL::TextureFormat::RGBA8, view)
           .setImage(GL::CubeMapCoordinate::NegativeX, 0, GL::TextureFormat::RGBA8, view)
           .setImage(GL::CubeMapCoordinate::PositiveY, 0, GL::TextureFormat::RGBA8, view)
           .setImage(GL::CubeMapCoordinate::NegativeY, 0, GL::TextureFormat::RGBA8, view)
           .setImage(GL::CubeMapCoordinate::PositiveZ, 0, GL::TextureFormat::RGBA8, view)
           .setImage(GL::CubeMapCoordinate::NegativeZ, 0, GL::TextureFormat::RGBA8, view);

    GL::BufferImage2D image = textureSubImage(texture, GL::CubeMapCoordinate::PositiveX, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte}, GL::BufferUsage::StaticRead);
    Containers::Array<UnsignedByte> data = bufferData<UnsignedByte>(image.buffer());
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), GL::PixelFormat::RGBA);
    CORRADE_COMPARE(image.type(), GL::PixelType::UnsignedByte);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(data, Containers::arrayView(Data2D),
        TestSuite::Compare::Container);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
constexpr UnsignedInt Data2DUInt[] = { 0xcafebabe,
                                       0xdeadbeef,
                                       0xbadf00d,
                                       0xdeadbabe };

void TextureImageGLTest::subImage2DUInt() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_integer>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_integer::string() + std::string(" is not supported"));
    #endif

    GL::Texture2D texture;
    texture.setImage(0, GL::TextureFormat::R32UI, ImageView2D{GL::PixelFormat::RedInteger, GL::PixelType::UnsignedInt, Vector2i{2}, Data2DUInt});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RedInteger, GL::PixelType::UnsignedInt});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), pixelFormatWrap(GL::PixelFormat::RedInteger));
    CORRADE_COMPARE(GL::PixelType(image.formatExtra()), GL::PixelType::UnsignedInt);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedInt>(image.data()),
        Containers::arrayView(Data2DUInt),
        TestSuite::Compare::Container);
}

constexpr Float Data2DFloat[] = { 1.0f,
                                  3.14159f,
                                  2.71828f,
                                  1.41421f };

void TextureImageGLTest::subImage2DFloat() {
    GL::Texture2D texture;
    texture
        /* If I don't set min filter, SwiftShader will return all zeros. ARM
           Mali G71 (on Huawei P10) needs the mag filter as well. */
        .setMinificationFilter(GL::SamplerFilter::Nearest)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, GL::TextureFormat::R32F, Vector2i{2})
        .setSubImage(0, {}, ImageView2D{GL::PixelFormat::Red, GL::PixelType::Float, Vector2i{2}, Data2DFloat});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::Red, GL::PixelType::Float});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), pixelFormatWrap(GL::PixelFormat::Red));
    CORRADE_COMPARE(GL::PixelType(image.formatExtra()), GL::PixelType::Float);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<Float>(image.data()),
        Containers::arrayView(Data2DFloat),
        TestSuite::Compare::Container);
}

void TextureImageGLTest::subImage2DFloatGeneric() {
    GL::Texture2D texture;
    texture
        /* If I don't set min filter, SwiftShader will return all zeros. ARM
           Mali G71 (on Huawei P10) needs the mag filter as well. */
        .setMinificationFilter(GL::SamplerFilter::Nearest)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, GL::TextureFormat::R32F, Vector2i{2})
        .setSubImage(0, {}, ImageView2D{GL::PixelFormat::Red, GL::PixelType::Float, Vector2i{2}, Data2DFloat});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {PixelFormat::R32F});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), PixelFormat::R32F);
    CORRADE_COMPARE(image.formatExtra(), 0);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<Float>(image.data()),
        Containers::arrayView(Data2DFloat),
        TestSuite::Compare::Container);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::TextureImageGLTest)
