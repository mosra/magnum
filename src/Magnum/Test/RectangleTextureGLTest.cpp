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

#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/BufferImage.h"
#include "Magnum/Image.h"
#include "Magnum/ImageFormat.h"
#include "Magnum/OpenGLTester.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/RectangleTexture.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"
#include "Magnum/OpenGLTester.h"

namespace Magnum { namespace Test {

struct RectangleTextureGLTest: OpenGLTester {
    explicit RectangleTextureGLTest();

    void construct();
    void wrap();

    void bind();
    void bindImage();

    void sampling();
    void samplingSRGBDecode();
    void samplingBorderInteger();
    void samplingSwizzle();
    void samplingDepthStencilMode();

    void storage();

    void image();
    void compressedImage();
    void imageBuffer();
    void compressedImageBuffer();
    void subImage();
    void compressedSubImage();
    void subImageBuffer();
    void compressedSubImageBuffer();
    void subImageQuery();
    void compressedSubImageQuery();
    void subImageQueryBuffer();
    void compressedSubImageQueryBuffer();

    void invalidateImage();
    void invalidateSubImage();
};

RectangleTextureGLTest::RectangleTextureGLTest() {
    addTests({&RectangleTextureGLTest::construct,
              &RectangleTextureGLTest::wrap,

              &RectangleTextureGLTest::bind,
              &RectangleTextureGLTest::bindImage,

              &RectangleTextureGLTest::sampling,
              &RectangleTextureGLTest::samplingSRGBDecode,
              &RectangleTextureGLTest::samplingBorderInteger,
              &RectangleTextureGLTest::samplingSwizzle,
              &RectangleTextureGLTest::samplingDepthStencilMode,

              &RectangleTextureGLTest::storage,

              &RectangleTextureGLTest::image,
              &RectangleTextureGLTest::compressedImage,
              &RectangleTextureGLTest::imageBuffer,
              &RectangleTextureGLTest::compressedImageBuffer,

              &RectangleTextureGLTest::subImage,
              &RectangleTextureGLTest::compressedSubImage,
              &RectangleTextureGLTest::subImageBuffer,
              &RectangleTextureGLTest::compressedSubImageBuffer,
              &RectangleTextureGLTest::subImageQuery,
              &RectangleTextureGLTest::compressedSubImageQuery,
              &RectangleTextureGLTest::subImageQueryBuffer,
              &RectangleTextureGLTest::compressedSubImageQueryBuffer,

              &RectangleTextureGLTest::invalidateImage,
              &RectangleTextureGLTest::invalidateSubImage});
}

namespace {
    template<std::size_t size, class T> Containers::ArrayView<const T> unsafeSuffix(const T(&data)[size], std::size_t offset) {
        static_assert(sizeof(T) == 1, "");
        return {data - offset, size + offset};
    }
}

void RectangleTextureGLTest::construct() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    {
        RectangleTexture texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void RectangleTextureGLTest::wrap() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = RectangleTexture::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    RectangleTexture::wrap(id);
    glDeleteTextures(1, &id);
}

void RectangleTextureGLTest::bind() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_ERROR();
}

void RectangleTextureGLTest::bindImage() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::GL::ARB::shader_image_load_store::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setStorage(TextureFormat::RGBA8, Vector2i{32})
        .bindImage(2, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImage(2);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_ERROR();
}

void RectangleTextureGLTest::sampling() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setMinificationFilter(Sampler::Filter::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
           .setCompareMode(Sampler::CompareMode::CompareRefToTexture)
           .setCompareFunction(Sampler::CompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_ERROR();
}

void RectangleTextureGLTest::samplingSRGBDecode() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setSRGBDecode(false);

    MAGNUM_VERIFY_NO_ERROR();
}

void RectangleTextureGLTest::samplingBorderInteger() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));

    RectangleTexture a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    RectangleTexture b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}

void RectangleTextureGLTest::samplingSwizzle() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_swizzle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_ERROR();
}

void RectangleTextureGLTest::samplingDepthStencilMode() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::GL::ARB::stencil_texturing::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setDepthStencilMode(Sampler::DepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_ERROR();
}

void RectangleTextureGLTest::storage() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setStorage(TextureFormat::RGBA8, Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(), Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();
}

namespace {
    constexpr UnsignedByte Data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };

    const auto DataStorage = PixelStorage{}.setSkip({0, 1, 0});
    const auto DataOffset = 8;
}

void RectangleTextureGLTest::image() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setImage(TextureFormat::RGBA8, ImageView2D{
        DataStorage, PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data, DataOffset)});

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image = texture.image({DataStorage, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(DataOffset),
        Containers::arrayView(Data), TestSuite::Compare::Container);
}

void RectangleTextureGLTest::compressedImage() {
    CORRADE_SKIP("No rectangle texture compression format exists.");
}

void RectangleTextureGLTest::imageBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setImage(TextureFormat::RGBA8, BufferImage2D{
        DataStorage, PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data, DataOffset), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image = texture.image({DataStorage, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(imageData.suffix(DataOffset),
        Containers::arrayView(Data), TestSuite::Compare::Container);
}

void RectangleTextureGLTest::compressedImageBuffer() {
    CORRADE_SKIP("No rectangle texture compression format exists.");
}

namespace {
    constexpr UnsignedByte Zero[4*4*4] = {};

    constexpr UnsignedByte SubDataComplete[] = {
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    };
}

void RectangleTextureGLTest::subImage() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setImage(TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setSubImage(Vector2i(1), ImageView2D{
        DataStorage, PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data, DataOffset)});

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image = texture.image({PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubDataComplete), TestSuite::Compare::Container);
}

void RectangleTextureGLTest::compressedSubImage() {
    CORRADE_SKIP("No rectangle texture compression format exists.");
}

void RectangleTextureGLTest::subImageBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setImage(TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setSubImage(Vector2i(1), BufferImage2D{
        DataStorage, PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data, DataOffset), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image = texture.image({PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(SubDataComplete),
        TestSuite::Compare::Container);
}

void RectangleTextureGLTest::compressedSubImageBuffer() {
    CORRADE_SKIP("No rectangle texture compression format exists.");
}

void RectangleTextureGLTest::subImageQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setStorage(TextureFormat::RGBA8, Vector2i{4})
           .setSubImage({}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubDataComplete});

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image = texture.subImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}),
        {DataStorage, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(DataOffset),
        Containers::arrayView(Data), TestSuite::Compare::Container);
}

void RectangleTextureGLTest::compressedSubImageQuery() {
    CORRADE_SKIP("No rectangle texture compression format exists.");
}

void RectangleTextureGLTest::subImageQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setStorage(TextureFormat::RGBA8, Vector2i{4})
           .setSubImage({}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubDataComplete});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image = texture.subImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}),
        {DataStorage, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(imageData.suffix(DataOffset),
        Containers::arrayView(Data), TestSuite::Compare::Container);
}

void RectangleTextureGLTest::compressedSubImageQueryBuffer() {
    CORRADE_SKIP("No rectangle texture compression format exists.");
}

void RectangleTextureGLTest::invalidateImage() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setStorage(TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateImage();

    MAGNUM_VERIFY_NO_ERROR();
}

void RectangleTextureGLTest::invalidateSubImage() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    RectangleTexture texture;
    texture.setStorage(TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateSubImage(Vector2i(4), Vector2i(16));

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::RectangleTextureGLTest)
