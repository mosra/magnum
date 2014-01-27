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
#include "Magnum/BufferImage.h"
#include "Magnum/Color.h"
#include "Magnum/ColorFormat.h"
#include "Magnum/Image.h"
#include "Magnum/RectangleTexture.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class TextureGLTest: public AbstractOpenGLTester {
    public:
        explicit TextureGLTest();

        void construct();
        void sampling();
        void storage();

        void image();
        void imageBuffer();

        void subImage();
        void subImageBuffer();

        void invalidateImage();
        void invalidateSubImage();
};

TextureGLTest::TextureGLTest() {
    addTests({&TextureGLTest::construct,
              &TextureGLTest::sampling,
              &TextureGLTest::storage,

              &TextureGLTest::image,
              &TextureGLTest::imageBuffer,

              &TextureGLTest::subImage,
              &TextureGLTest::subImageBuffer,

              &TextureGLTest::invalidateImage,
              &TextureGLTest::invalidateSubImage});
}

void TextureGLTest::construct() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    {
        RectangleTexture texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::sampling() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setMinificationFilter(Sampler::Filter::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::storage() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setStorage(TextureFormat::RGBA8, Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(), Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::image() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    RectangleTexture texture;
    texture.setImage(TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data));

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
       std::vector<UnsignedByte>(data, data + 16), TestSuite::Compare::Container);
}

void TextureGLTest::imageBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    RectangleTexture texture;
    texture.setImage(TextureFormat::RGBA8,
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
        std::vector<UnsignedByte>(data, data+16), TestSuite::Compare::Container);
}

void TextureGLTest::subImage() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    RectangleTexture texture;
    texture.setImage(TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(Vector2i(1),
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData));

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}

void TextureGLTest::subImageBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    RectangleTexture texture;
    texture.setImage(TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(Vector2i(1),
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}

void TextureGLTest::invalidateImage() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setStorage(TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateImage();

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::invalidateSubImage() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setStorage(TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateSubImage(Vector2i(4), Vector2i(16));

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::TextureGLTest)
