/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "Magnum/configure.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/BufferImage.h"
#endif
#include "Magnum/Color.h"
#include "Magnum/ColorFormat.h"
#include "Magnum/CubeMapTexture.h"
#include "Magnum/Image.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class CubeMapTextureGLTest: public AbstractOpenGLTester {
    public:
        explicit CubeMapTextureGLTest();

        void construct();
        void bind();

        void sampling();
        #ifdef MAGNUM_TARGET_GLES2
        void samplingMaxLevel();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void samplingBorderInteger();
        #endif

        void storage();

        void image();
        #ifndef MAGNUM_TARGET_GLES2
        void imageBuffer();
        #endif

        void subImage();
        #ifndef MAGNUM_TARGET_GLES2
        void subImageBuffer();
        #endif

        void generateMipmap();

        void invalidateImage();
        void invalidateSubImage();
};

CubeMapTextureGLTest::CubeMapTextureGLTest() {
    addTests({&CubeMapTextureGLTest::construct,
              &CubeMapTextureGLTest::bind,

              &CubeMapTextureGLTest::sampling,
              #ifdef MAGNUM_TARGET_GLES2
              &CubeMapTextureGLTest::samplingMaxLevel,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &CubeMapTextureGLTest::samplingBorderInteger,
              #endif

              &CubeMapTextureGLTest::storage,

              &CubeMapTextureGLTest::image,
              #ifndef MAGNUM_TARGET_GLES2
              &CubeMapTextureGLTest::imageBuffer,
              #endif

              &CubeMapTextureGLTest::subImage,
              #ifndef MAGNUM_TARGET_GLES2
              &CubeMapTextureGLTest::subImageBuffer,
              #endif

              &CubeMapTextureGLTest::generateMipmap,

              &CubeMapTextureGLTest::invalidateImage,
              &CubeMapTextureGLTest::invalidateSubImage});
}

void CubeMapTextureGLTest::construct() {
    {
        CubeMapTexture texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureGLTest::bind() {
    CubeMapTexture texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureGLTest::sampling() {
    CubeMapTexture texture;
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           #ifndef MAGNUM_TARGET_GLES2
           .setBaseLevel(1)
           .setMaxLevel(750)
           #endif
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

   MAGNUM_VERIFY_NO_ERROR();
}

#ifdef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::samplingMaxLevel() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::GL::APPLE::texture_max_level::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::samplingBorderInteger() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));

    CubeMapTexture a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    CubeMapTexture b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void CubeMapTextureGLTest::storage() {
    CubeMapTexture texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeX, 0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveY, 0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeY, 0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveZ, 0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeZ, 0), Vector2i(32));

    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeX, 1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveY, 1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeY, 1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveZ, 1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeZ, 1), Vector2i(16));

    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 2), Vector2i(8));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeX, 2), Vector2i(8));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveY, 2), Vector2i(8));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeY, 2), Vector2i(8));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveZ, 2), Vector2i(8));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeZ, 2), Vector2i(8));

    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 3), Vector2i(4));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeX, 3), Vector2i(4));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveY, 3), Vector2i(4));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeY, 3), Vector2i(4));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveZ, 3), Vector2i(4));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeZ, 3), Vector2i(4));

    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 4), Vector2i(2));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeX, 4), Vector2i(2));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveY, 4), Vector2i(2));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeY, 4), Vector2i(2));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveZ, 4), Vector2i(2));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeZ, 4), Vector2i(2));

    /* Not available */
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 5), Vector2i(0));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeX, 5), Vector2i(0));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveY, 5), Vector2i(0));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeY, 5), Vector2i(0));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveZ, 5), Vector2i(0));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::NegativeZ, 5), Vector2i(0));

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

void CubeMapTextureGLTest::image() {
    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    CubeMapTexture texture;
    texture.setImage(CubeMapTexture::Coordinate::PositiveX, 0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(CubeMapTexture::Coordinate::PositiveX, 0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
       std::vector<UnsignedByte>(data, data+16), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::imageBuffer() {
    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    CubeMapTexture texture;
    texture.setImage(CubeMapTexture::Coordinate::PositiveX, 0, TextureFormat::RGBA8,
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(CubeMapTexture::Coordinate::PositiveX, 0, image, BufferUsage::StaticRead);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    const auto imageData = image.buffer().data<UnsignedByte>();
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
       std::vector<UnsignedByte>(data, data+16), TestSuite::Compare::Container);
    #endif
}
#endif

void CubeMapTextureGLTest::subImage() {
    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    CubeMapTexture texture;
    texture.setImage(CubeMapTexture::Coordinate::PositiveX, 0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(CubeMapTexture::Coordinate::PositiveX, 0, Vector2i(1),
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(CubeMapTexture::Coordinate::PositiveX, 0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::subImageBuffer() {
    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    CubeMapTexture texture;
    texture.setImage(CubeMapTexture::Coordinate::PositiveX, 0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(CubeMapTexture::Coordinate::PositiveX, 0, Vector2i(1),
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(CubeMapTexture::Coordinate::PositiveX, 0, image, BufferUsage::StaticRead);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    const auto imageData = image.buffer().data<UnsignedByte>();
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
    #endif
}
#endif

void CubeMapTextureGLTest::generateMipmap() {
    CubeMapTexture texture;
    texture.setImage(CubeMapTexture::Coordinate::PositiveX, 0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(32)));

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 1), Vector2i( 0));
    #endif

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 2), Vector2i( 8));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 3), Vector2i( 4));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 4), Vector2i( 2));
    CORRADE_COMPARE(texture.imageSize(CubeMapTexture::Coordinate::PositiveX, 5), Vector2i( 1));

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

void CubeMapTextureGLTest::invalidateImage() {
    CubeMapTexture texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureGLTest::invalidateSubImage() {
    CubeMapTexture texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::CubeMapTextureGLTest)
