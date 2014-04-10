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
#include "Magnum/TextureArray.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class TextureArrayGLTest: public AbstractOpenGLTester {
    public:
        explicit TextureArrayGLTest();

        #ifndef MAGNUM_TARGET_GLES
        void construct1D();
        #endif
        void construct2D();

        #ifndef MAGNUM_TARGET_GLES
        void bind1D();
        #endif
        void bind2D();

        #ifndef MAGNUM_TARGET_GLES
        void sampling1D();
        #endif
        void sampling2D();

        #ifdef MAGNUM_TARGET_GLES2
        void samplingMaxLevel2D();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void samplingBorderInteger1D();
        void samplingBorderInteger2D();
        void samplingDepthStencilMode1D();
        void samplingDepthStencilMode2D();
        #else
        void samplingBorder2D();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void storage1D();
        #endif
        void storage2D();

        #ifndef MAGNUM_TARGET_GLES
        void image1D();
        void image1DBuffer();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void image2D();
        void image2DBuffer();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void subImage1D();
        void subImage1DBuffer();
        #endif
        void subImage2D();
        void subImage2DBuffer();

        #ifndef MAGNUM_TARGET_GLES
        void generateMipmap1D();
        #endif
        void generateMipmap2D();

        #ifndef MAGNUM_TARGET_GLES
        void invalidateImage1D();
        #endif
        void invalidateImage2D();

        #ifndef MAGNUM_TARGET_GLES
        void invalidateSubImage1D();
        #endif
        void invalidateSubImage2D();
};

TextureArrayGLTest::TextureArrayGLTest() {
    addTests({
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::construct1D,
        #endif
        &TextureArrayGLTest::construct2D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::bind1D,
        #endif
        &TextureArrayGLTest::bind2D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::sampling1D,
        #endif
        &TextureArrayGLTest::sampling2D,

        #ifdef MAGNUM_TARGET_GLES2
        &TextureArrayGLTest::samplingMaxLevel2D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::samplingBorderInteger1D,
        &TextureArrayGLTest::samplingBorderInteger2D,
        &TextureArrayGLTest::samplingDepthStencilMode1D,
        &TextureArrayGLTest::samplingDepthStencilMode2D,
        #else
        &TextureArrayGLTest::samplingBorder2D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::storage1D,
        #endif
        &TextureArrayGLTest::storage2D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::image1D,
        &TextureArrayGLTest::image1DBuffer,
        #endif
        &TextureArrayGLTest::image2D,
        &TextureArrayGLTest::image2DBuffer,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::subImage1D,
        &TextureArrayGLTest::subImage1DBuffer,
        #endif
        &TextureArrayGLTest::subImage2D,
        &TextureArrayGLTest::subImage2DBuffer,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::generateMipmap1D,
        #endif
        &TextureArrayGLTest::generateMipmap2D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::invalidateImage1D,
        #endif
        &TextureArrayGLTest::invalidateImage2D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::invalidateSubImage1D,
        #endif
        &TextureArrayGLTest::invalidateSubImage2D
    });
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::construct1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    {
        Texture1DArray texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::construct2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    {
        Texture2DArray texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::bind1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;

    if(Context::current()->isExtensionSupported<Extensions::GL::ARB::multi_bind>()) {
        CORRADE_EXPECT_FAIL("With ARB_multi_bind the texture must be associated with given target at least once before binding it.");
        texture.setBaseLevel(0);
        CORRADE_VERIFY(false);
    }

    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::bind2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current()->isExtensionSupported<Extensions::GL::ARB::multi_bind>()) {
        CORRADE_EXPECT_FAIL("With ARB_multi_bind the texture must be associated with given target at least once before binding it.");
        texture.setBaseLevel(0);
        CORRADE_VERIFY(false);
    }
    #endif

    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::sampling1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           .setMinLod(-750.0f)
           .setMaxLod(750.0f)
           .setLodBias(0.5f)
           .setBaseLevel(1)
           .setMaxLevel(750)
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureArrayGLTest::samplingBorderInteger1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));

    Texture1DArray a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture1DArray b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureArrayGLTest::samplingDepthStencilMode1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::GL::ARB::stencil_texturing::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setDepthStencilMode(Sampler::DepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::sampling2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           #ifndef MAGNUM_TARGET_GLES2
           .setMinLod(-750.0f)
           .setMaxLod(750.0f)
           #ifndef MAGNUM_TARGET_GLES
           .setLodBias(0.5f)
           #endif
           .setBaseLevel(1)
           .setMaxLevel(750)
           #endif
           #ifndef MAGNUM_TARGET_GLES
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           #else
           .setWrapping(Sampler::Wrapping::ClampToEdge)
           #endif
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

    MAGNUM_VERIFY_NO_ERROR();
}

#ifdef MAGNUM_TARGET_GLES2
void TextureArrayGLTest::samplingMaxLevel2D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::GL::APPLE::texture_max_level::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::samplingBorderInteger2D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));

    Texture2DArray a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture2DArray b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureArrayGLTest::samplingDepthStencilMode2D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::GL::ARB::stencil_texturing::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setDepthStencilMode(Sampler::DepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_ERROR();
}
#else
void TextureArrayGLTest::samplingBorder2D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::NV::texture_border_clamp>())
        CORRADE_SKIP(Extensions::GL::NV::texture_border_clamp::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::storage1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 0,  0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::storage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector3i(32));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 0,  0,  0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::image1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data));

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
       std::vector<UnsignedByte>(data, data + 16), TestSuite::Compare::Container);
}

void TextureArrayGLTest::image1DBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
        std::vector<UnsignedByte>(data, data + 16), TestSuite::Compare::Container);
}
#endif

void TextureArrayGLTest::image2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f,
                                      0x10, 0x11, 0x12, 0x13,
                                      0x14, 0x15, 0x16, 0x17,
                                      0x18, 0x19, 0x1a, 0x1b,
                                      0x1c, 0x1d, 0x1e, 0x1f };
    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), data));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
       std::vector<UnsignedByte>(data, data + 32), TestSuite::Compare::Container);
    #endif
}

void TextureArrayGLTest::image2DBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f,
                                      0x10, 0x11, 0x12, 0x13,
                                      0x14, 0x15, 0x16, 0x17,
                                      0x18, 0x19, 0x1a, 0x1b,
                                      0x1c, 0x1d, 0x1e, 0x1f };
    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        BufferImage3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
        std::vector<UnsignedByte>(data, data + 32), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::subImage1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(0, Vector2i(1),
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData));

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}

void TextureArrayGLTest::subImage1DBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(0, Vector2i(1),
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
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
#endif

void TextureArrayGLTest::subImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    constexpr UnsignedByte zero[4*4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f,
                                         0x10, 0x11, 0x12, 0x13,
                                         0x14, 0x15, 0x16, 0x17,
                                         0x18, 0x19, 0x1a, 0x1b,
                                         0x1c, 0x1d, 0x1e, 0x1f };
    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(4), zero));
    texture.setSubImage(0, Vector3i(1),
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), subData));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0, 0, 0, 0,
        0, 0, 0, 0, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
    #endif
}

void TextureArrayGLTest::subImage2DBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    constexpr UnsignedByte zero[4*4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f,
                                         0x10, 0x11, 0x12, 0x13,
                                         0x14, 0x15, 0x16, 0x17,
                                         0x18, 0x19, 0x1a, 0x1b,
                                         0x1c, 0x1d, 0x1e, 0x1f };
    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(4), zero));
    texture.setSubImage(0, Vector3i(1),
        BufferImage3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0, 0, 0, 0,
        0, 0, 0, 0, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::generateMipmap1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(32)));

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i( 0));

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 1, 32));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::generateMipmap2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(32)));

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i( 0));
    #endif

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 1,  1, 32));

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::invalidateImage1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::invalidateImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::invalidateSubImage1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateSubImage(1, Vector2i(2), Vector2i(8));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::invalidateSubImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::TextureArrayGLTest)
