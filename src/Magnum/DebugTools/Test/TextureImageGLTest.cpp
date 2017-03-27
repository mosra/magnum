/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/CubeMapTexture.h"
#include "Magnum/Image.h"
#include "Magnum/OpenGLTester.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Texture.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/DebugTools/BufferData.h"
#include "Magnum/DebugTools/TextureImage.h"
#include "Magnum/Math/Range.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/BufferImage.h"
#endif

namespace Magnum { namespace DebugTools { namespace Test {

struct TextureImageGLTest: OpenGLTester {
    explicit TextureImageGLTest();

    void subImage2D();
    #ifndef MAGNUM_TARGET_GLES2
    void subImage2DBuffer();
    #endif

    void subImageCube();
    #ifndef MAGNUM_TARGET_GLES2
    void subImageCubeBuffer();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void subImage2DUInt();
    void subImage2DFloat();
    #endif
};

TextureImageGLTest::TextureImageGLTest() {
    addTests({&TextureImageGLTest::subImage2D,
              #ifndef MAGNUM_TARGET_GLES2
              &TextureImageGLTest::subImage2DBuffer,
              #endif

              &TextureImageGLTest::subImageCube,
              #ifndef MAGNUM_TARGET_GLES2
              &TextureImageGLTest::subImageCubeBuffer,
              #endif

              #ifndef MAGNUM_TARGET_GLES2
              &TextureImageGLTest::subImage2DUInt,
              &TextureImageGLTest::subImage2DFloat,
              #endif
              });
}

namespace {
    constexpr UnsignedByte Data2D[] = { 0x00, 0x01, 0x02, 0x03,
                                        0x04, 0x05, 0x06, 0x07,
                                        0x08, 0x09, 0x0a, 0x0b,
                                        0x0c, 0x0d, 0x0e, 0x0f };
}

void TextureImageGLTest::subImage2D() {
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, Data2D});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {PixelFormat::RGBA, PixelType::UnsignedByte});
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

#ifndef MAGNUM_TARGET_GLES2
void TextureImageGLTest::subImage2DBuffer() {
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, Data2D});

    BufferImage2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    Containers::Array<UnsignedByte> data = bufferData<UnsignedByte>(image.buffer());
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(data, Containers::arrayView(Data2D),
        TestSuite::Compare::Container);
}
#endif

void TextureImageGLTest::subImageCube() {
    ImageView2D view{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, Data2D};

    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, TextureFormat::RGBA8, view)
           .setImage(CubeMapCoordinate::NegativeX, 0, TextureFormat::RGBA8, view)
           .setImage(CubeMapCoordinate::PositiveY, 0, TextureFormat::RGBA8, view)
           .setImage(CubeMapCoordinate::NegativeY, 0, TextureFormat::RGBA8, view)
           .setImage(CubeMapCoordinate::PositiveZ, 0, TextureFormat::RGBA8, view)
           .setImage(CubeMapCoordinate::NegativeZ, 0, TextureFormat::RGBA8, view);

    Image2D image = textureSubImage(texture, CubeMapCoordinate::PositiveX, 0, {{}, Vector2i{2}}, {PixelFormat::RGBA, PixelType::UnsignedByte});
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

#ifndef MAGNUM_TARGET_GLES2
void TextureImageGLTest::subImageCubeBuffer() {
    ImageView2D view{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, Data2D};

    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, TextureFormat::RGBA8, view)
           .setImage(CubeMapCoordinate::NegativeX, 0, TextureFormat::RGBA8, view)
           .setImage(CubeMapCoordinate::PositiveY, 0, TextureFormat::RGBA8, view)
           .setImage(CubeMapCoordinate::NegativeY, 0, TextureFormat::RGBA8, view)
           .setImage(CubeMapCoordinate::PositiveZ, 0, TextureFormat::RGBA8, view)
           .setImage(CubeMapCoordinate::NegativeZ, 0, TextureFormat::RGBA8, view);

    BufferImage2D image = textureSubImage(texture, CubeMapCoordinate::PositiveX, 0, {{}, Vector2i{2}}, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    Containers::Array<UnsignedByte> data = bufferData<UnsignedByte>(image.buffer());
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(data, Containers::arrayView(Data2D),
        TestSuite::Compare::Container);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
namespace {
    constexpr UnsignedInt Data2DUInt[] = { 0xcafebabe,
                                           0xdeadbeef,
                                           0xbadf00d,
                                           0xdeadbabe };
}

void TextureImageGLTest::subImage2DUInt() {
    Texture2D texture;
    texture.setImage(0, TextureFormat::R32UI, ImageView2D{PixelFormat::RedInteger, PixelType::UnsignedInt, Vector2i{2}, Data2DUInt});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {PixelFormat::RedInteger, PixelType::UnsignedInt});
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedInt>(image.data()),
        Containers::arrayView(Data2DUInt),
        TestSuite::Compare::Container);
}

namespace {
    constexpr Float Data2DFloat[] = { 1.0f,
                                      3.14159f,
                                      2.71828f,
                                      1.41421f };
}

void TextureImageGLTest::subImage2DFloat() {
    Texture2D texture;
    texture.setStorage(1, TextureFormat::R32F, Vector2i{2})
        .setSubImage(0, {}, ImageView2D{PixelFormat::Red, PixelType::Float, Vector2i{2}, Data2DFloat});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {PixelFormat::Red, PixelType::Float});
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<Float>(image.data()),
        Containers::arrayView(Data2DFloat),
        TestSuite::Compare::Container);
}
#endif

}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::TextureImageGLTest)
