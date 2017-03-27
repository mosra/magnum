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

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/BufferImage.h"
#endif
#include "Magnum/CubeMapTexture.h"
#include "Magnum/Image.h"
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/ImageFormat.h"
#endif
#include "Magnum/OpenGLTester.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace Test {

struct CubeMapTextureGLTest: OpenGLTester {
    explicit CubeMapTextureGLTest();

    void construct();
    void wrap();

    void bind();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void bindImage();
    #endif

    void sampling();
    void samplingSRGBDecode();
    #ifndef MAGNUM_TARGET_GLES2
    void samplingSwizzle();
    #else
    void samplingMaxLevel();
    void samplingCompare();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingBorderInteger();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void samplingDepthStencilMode();
    #endif
    #ifdef MAGNUM_TARGET_GLES
    void samplingBorder();
    #endif

    void storage();

    void image();
    void compressedImage();
    #ifndef MAGNUM_TARGET_GLES2
    void imageBuffer();
    void compressedImageBuffer();
    #endif
    void immutableCompressedImage();
    #ifndef MAGNUM_TARGET_GLES
    void fullImageQuery();
    void compressedFullImageQuery();
    void fullImageQueryBuffer();
    void compressedFullImageQueryBuffer();
    #endif

    void subImage();
    void compressedSubImage();
    #ifndef MAGNUM_TARGET_GLES2
    void subImageBuffer();
    void compressedSubImageBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void subImageQuery();
    void compressedSubImageQuery();
    void subImageQueryBuffer();
    void compressedSubImageQueryBuffer();
    #endif

    void generateMipmap();

    void invalidateImage();
    void invalidateSubImage();

private:
    PixelStorage _dataStorage;
    std::size_t _dataOffset;

    #ifndef MAGNUM_TARGET_GLES
    CompressedPixelStorage _compressedDataStorage, _compressedFullDataStorage;
    #endif
    std::size_t _compressedDataOffset, _compressedFullDataOffset;
};

CubeMapTextureGLTest::CubeMapTextureGLTest() {
    addTests({&CubeMapTextureGLTest::construct,
              &CubeMapTextureGLTest::wrap,

              &CubeMapTextureGLTest::bind,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &CubeMapTextureGLTest::bindImage,
              #endif

              &CubeMapTextureGLTest::sampling,
              &CubeMapTextureGLTest::samplingSRGBDecode,
              #ifndef MAGNUM_TARGET_GLES2
              &CubeMapTextureGLTest::samplingSwizzle,
              #else
              &CubeMapTextureGLTest::samplingMaxLevel,
              &CubeMapTextureGLTest::samplingCompare,
              #endif
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &CubeMapTextureGLTest::samplingBorderInteger,
              #endif
              #ifndef MAGNUM_TARGET_GLES2
              &CubeMapTextureGLTest::samplingDepthStencilMode,
              #endif
              #ifdef MAGNUM_TARGET_GLES
              &CubeMapTextureGLTest::samplingBorder,
              #endif

              &CubeMapTextureGLTest::storage,

              &CubeMapTextureGLTest::image,
              &CubeMapTextureGLTest::compressedImage,
              #ifndef MAGNUM_TARGET_GLES2
              &CubeMapTextureGLTest::imageBuffer,
              &CubeMapTextureGLTest::compressedImageBuffer,
              #endif
              &CubeMapTextureGLTest::immutableCompressedImage,
              #ifndef MAGNUM_TARGET_GLES
              &CubeMapTextureGLTest::fullImageQuery,
              &CubeMapTextureGLTest::compressedFullImageQuery,
              &CubeMapTextureGLTest::fullImageQueryBuffer,
              &CubeMapTextureGLTest::compressedFullImageQueryBuffer,
              #endif

              &CubeMapTextureGLTest::subImage,
              &CubeMapTextureGLTest::compressedSubImage,
              #ifndef MAGNUM_TARGET_GLES2
              &CubeMapTextureGLTest::subImageBuffer,
              &CubeMapTextureGLTest::compressedSubImageBuffer,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &CubeMapTextureGLTest::subImageQuery,
              &CubeMapTextureGLTest::compressedSubImageQuery,
              &CubeMapTextureGLTest::subImageQueryBuffer,
              &CubeMapTextureGLTest::compressedSubImageQueryBuffer,
              #endif

              &CubeMapTextureGLTest::generateMipmap,

              &CubeMapTextureGLTest::invalidateImage,
              &CubeMapTextureGLTest::invalidateSubImage});

    _dataStorage = PixelStorage{}.setSkip({0, 1, 0});
    _dataOffset = 8;
    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::unpack_subimage>())
    #endif
    {
        _dataStorage = {};
        _dataOffset = 0;
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>())
    {
        _compressedDataStorage = CompressedPixelStorage{}
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16)
            .setSkip({0, 4, 0});
        _compressedFullDataStorage = CompressedPixelStorage{}
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16)
            .setSkip({0, 0, 4});
        _compressedDataOffset = 16;
        _compressedFullDataOffset = 16*4;
    } else
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage = {};
        #endif
        _compressedDataOffset = 0;
        _compressedFullDataOffset = 0;
    }
}

namespace {
    template<std::size_t size, class T> Containers::ArrayView<const T> unsafeSuffix(const T(&data)[size], std::size_t offset) {
        static_assert(sizeof(T) == 1, "");
        return {data - offset, size + offset};
    }
}

void CubeMapTextureGLTest::construct() {
    {
        CubeMapTexture texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureGLTest::wrap() {
    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = CubeMapTexture::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    CubeMapTexture::wrap(id);
    glDeleteTextures(1, &id);
}

void CubeMapTextureGLTest::bind() {
    CubeMapTexture texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_ERROR();
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTextureGLTest::bindImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::GL::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{32})
        .bindImage(2, 0, CubeMapCoordinate::NegativeX, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    texture.bindImageLayered(3, 0, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    AbstractTexture::unbindImage(2);
    AbstractTexture::unbindImage(3);

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}
#endif

void CubeMapTextureGLTest::sampling() {
    CubeMapTexture texture;
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
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
            #ifndef MAGNUM_TARGET_GLES2
           .setCompareMode(Sampler::CompareMode::CompareRefToTexture)
           .setCompareFunction(Sampler::CompareFunction::GreaterOrEqual)
            #endif
           ;

   MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureGLTest::samplingSRGBDecode() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::sRGB>())
        CORRADE_SKIP(Extensions::GL::EXT::sRGB::string() + std::string(" is not supported."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setSRGBDecode(false);

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::samplingSwizzle() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_swizzle::string() + std::string(" is not supported."));
    #endif

    CubeMapTexture texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_ERROR();
}
#else
void CubeMapTextureGLTest::samplingMaxLevel() {
    if(!Context::current().isExtensionSupported<Extensions::GL::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::GL::APPLE::texture_max_level::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureGLTest::samplingCompare() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::shadow_samplers>() ||
       !Context::current().isExtensionSupported<Extensions::GL::NV::shadow_samplers_cube>())
        CORRADE_SKIP(Extensions::GL::NV::shadow_samplers_cube::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setCompareMode(Sampler::CompareMode::CompareRefToTexture)
           .setCompareFunction(Sampler::CompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTextureGLTest::samplingBorderInteger() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_border_clamp::string() + std::string(" is not supported."));
    #endif

    CubeMapTexture a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    CubeMapTexture b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::samplingDepthStencilMode() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::GL::ARB::stencil_texturing::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported.");
    #endif

    CubeMapTexture texture;
    texture.setDepthStencilMode(Sampler::DepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifdef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::samplingBorder() {
    if(!Context::current().isExtensionSupported<Extensions::GL::NV::texture_border_clamp>() &&
       !Context::current().isExtensionSupported<Extensions::GL::EXT::texture_border_clamp>())
        CORRADE_SKIP("No required extension is supported.");

    CubeMapTexture texture;
    texture.setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void CubeMapTextureGLTest::storage() {
    CubeMapTexture texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES2
    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported, skipping image size testing");
    #endif

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i(8));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i(4));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i(2));
    /* Not available */
    CORRADE_COMPARE(texture.imageSize(5), Vector2i(0));

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

namespace {
    constexpr UnsignedByte Data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };

    /* Just 4x4 0x00 - 0x3f compressed using RGBA DXT3 by the driver */
    constexpr UnsignedByte CompressedData[] = {
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2
    };
}

void CubeMapTextureGLTest::image() {
    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, TextureFormat::RGBA8,
        ImageView2D{_dataStorage, PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data, _dataOffset)});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = texture.image(CubeMapCoordinate::PositiveX, 0,
        {_dataStorage, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_dataOffset),
        Containers::arrayView(Data), TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureGLTest::compressedImage() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::GL::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #endif

    const CompressedImageView2D view{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, unsafeSuffix(CompressedData, _compressedDataOffset)};

    CubeMapTexture texture;
    texture.setCompressedImage(CubeMapCoordinate::PositiveX, 0, view)
           .setCompressedImage(CubeMapCoordinate::NegativeX, 0, view)
           .setCompressedImage(CubeMapCoordinate::PositiveY, 0, view)
           .setCompressedImage(CubeMapCoordinate::NegativeY, 0, view)
           .setCompressedImage(CubeMapCoordinate::PositiveZ, 0, view)
           .setCompressedImage(CubeMapCoordinate::NegativeZ, 0, view);

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0, {_compressedDataStorage});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_compressedDataOffset),
        Containers::arrayView(CompressedData), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::imageBuffer() {
    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, TextureFormat::RGBA8, BufferImage2D{
        _dataStorage, PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data, _dataOffset), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image = texture.image(CubeMapCoordinate::PositiveX, 0,
        {_dataStorage, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(imageData.suffix(_dataOffset),
        Containers::arrayView(Data), TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureGLTest::compressedImageBuffer() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::GL::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #endif

    CompressedBufferImage2D buffer{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, unsafeSuffix(CompressedData, _compressedDataOffset), BufferUsage::StaticDraw};

    CubeMapTexture texture;
    texture.setCompressedImage(CubeMapCoordinate::PositiveX, 0, buffer)
           .setCompressedImage(CubeMapCoordinate::NegativeX, 0, buffer)
           .setCompressedImage(CubeMapCoordinate::PositiveY, 0, buffer)
           .setCompressedImage(CubeMapCoordinate::NegativeY, 0, buffer)
           .setCompressedImage(CubeMapCoordinate::PositiveZ, 0, buffer)
           .setCompressedImage(CubeMapCoordinate::NegativeZ, 0, buffer);

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0,
        {_compressedDataStorage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(imageData.suffix(_compressedDataOffset),
        Containers::arrayView(CompressedData), TestSuite::Compare::Container);
    #endif
}
#endif

void CubeMapTextureGLTest::immutableCompressedImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_storage>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_storage::string() + std::string(" is not supported."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_storage>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_storage::string() + std::string(" is not supported."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    /* Testing that GL_TEXTURE_COMPRESSED_IMAGE_SIZE is consistent and returns
       the same value regardless whether the texture is immutable or not. (Not
       the case, at least on NVidia 358.16, compare with compressedImage() test
       case that just calls setImage() six times instead of setStorage() and
       gets value that's six times smaller. I couldn't find anything in the
       specs so I suspect it's a bug). */

    const CompressedImageView2D view{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, unsafeSuffix(CompressedData, _compressedDataOffset)};

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
           .setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, {}, view)
           .setCompressedSubImage(CubeMapCoordinate::NegativeX, 0, {}, view)
           .setCompressedSubImage(CubeMapCoordinate::PositiveY, 0, {}, view)
           .setCompressedSubImage(CubeMapCoordinate::NegativeY, 0, {}, view)
           .setCompressedSubImage(CubeMapCoordinate::PositiveZ, 0, {}, view)
           .setCompressedSubImage(CubeMapCoordinate::NegativeZ, 0, {}, view);

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::NegativeY, 0, {});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(CompressedData), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
namespace {
    constexpr UnsignedByte FullData[] = { 0x00, 0x01, 0x02, 0x03,
                                          0x04, 0x05, 0x06, 0x07,
                                          0x08, 0x09, 0x0a, 0x0b,
                                          0x0c, 0x0d, 0x0e, 0x0f,

                                          0x10, 0x11, 0x12, 0x13,
                                          0x14, 0x15, 0x16, 0x17,
                                          0x18, 0x19, 0x1a, 0x1b,
                                          0x1c, 0x1d, 0x1e, 0x1f,

                                          0x20, 0x21, 0x22, 0x23,
                                          0x24, 0x25, 0x26, 0x27,
                                          0x28, 0x29, 0x2a, 0x2b,
                                          0x2c, 0x2d, 0x2e, 0x2f,

                                          0x30, 0x31, 0x32, 0x33,
                                          0x34, 0x35, 0x36, 0x37,
                                          0x38, 0x39, 0x3a, 0x3b,
                                          0x3c, 0x3d, 0x3e, 0x3f,

                                          0x40, 0x41, 0x42, 0x43,
                                          0x44, 0x45, 0x46, 0x47,
                                          0x48, 0x49, 0x4a, 0x4b,
                                          0x4c, 0x4d, 0x4e, 0x4f,

                                          0x50, 0x51, 0x52, 0x53,
                                          0x54, 0x55, 0x56, 0x57,
                                          0x58, 0x59, 0x5a, 0x5b,
                                          0x5c, 0x5d, 0x5e, 0x5f };

    const auto FullDataStorage = PixelStorage{}.setSkip({0, 0, 1});
    const auto FullDataOffset = 16;

    /* Just 4x4 0x00 - 0x3f compressed using RGBA DXT3 by the driver, repeated
       six times */
    constexpr UnsignedByte CompressedFullData[] = {
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2,
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2,
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2,

          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2,
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2,
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2
    };
}

void CubeMapTextureGLTest::fullImageQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::GL::ARB::direct_state_access::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2, 2})
        .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6}, FullData});

    MAGNUM_VERIFY_NO_ERROR();

    Image3D image = texture.image(0,
        {FullDataStorage, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(FullDataOffset),
        Containers::arrayView(FullData), TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedFullImageQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::GL::ARB::direct_state_access::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
        .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6}, CompressedFullData});

    MAGNUM_VERIFY_NO_ERROR();

    CompressedImage3D image = texture.compressedImage(0, {_compressedFullDataStorage});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_compressedFullDataOffset),
        Containers::arrayView(CompressedFullData), TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::fullImageQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::GL::ARB::direct_state_access::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2})
        .setSubImage(0, {}, BufferImage3D{PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6}, FullData, BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage3D image = texture.image(0,
        {FullDataStorage, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    const auto imageData = image.buffer().data<UnsignedByte>();
    CORRADE_COMPARE_AS(imageData.suffix(FullDataOffset),
        Containers::arrayView(FullData), TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedFullImageQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::GL::ARB::direct_state_access::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
        .setCompressedSubImage(0, {}, CompressedBufferImage3D{CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6}, CompressedFullData, BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    CompressedBufferImage3D image = texture.compressedImage(0, {_compressedFullDataStorage}, BufferUsage::StaticRead);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    const auto imageData = image.buffer().data<UnsignedByte>();
    CORRADE_COMPARE_AS(imageData.suffix(_compressedFullDataOffset),
        Containers::arrayView(CompressedFullData), TestSuite::Compare::Container);
}
#endif

namespace {
    constexpr UnsignedByte Zero[4*4*4] = {};

    /* Just 12x12 zeros compressed using RGBA DXT3 by the driver */
    constexpr UnsignedByte CompressedZero[9*16] = {};

    constexpr UnsignedByte SubDataComplete[] = {
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    };

    /* Combination of CompressedZero and CompressedData */
    constexpr UnsignedByte CompressedSubDataComplete[] = {
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,

          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,  17,  17,  34,  34,  51,  51,  67,
        232,  57,   0,   0, 213, 255, 170,   2,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,

          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0
    };
}

void CubeMapTextureGLTest::subImage() {
    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::NegativeX, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::PositiveY, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::NegativeY, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::PositiveZ, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::NegativeZ, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setSubImage(CubeMapCoordinate::PositiveX, 0, Vector2i(1), ImageView2D{
        _dataStorage, PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data, _dataOffset)});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = texture.image(CubeMapCoordinate::PositiveX, 0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubDataComplete), TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureGLTest::compressedSubImage() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::GL::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /* Compressed pixel storage for array textures is underspecified. If the
       extension is supported, first test with default values to ensure we are
       not that far off, then continue as usual */
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>()) {
        CubeMapTexture texture;
        texture.setCompressedImage(CubeMapCoordinate::PositiveX, 0,
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedImage(CubeMapCoordinate::NegativeX, 0,
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedImage(CubeMapCoordinate::PositiveY, 0,
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedImage(CubeMapCoordinate::NegativeY, 0,
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedImage(CubeMapCoordinate::PositiveZ, 0,
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedImage(CubeMapCoordinate::NegativeZ, 0,
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, Vector2i{4}, CompressedImageView2D{
            CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, CompressedData});

        MAGNUM_VERIFY_NO_ERROR();

        CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0, {});

        MAGNUM_VERIFY_NO_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i{12});
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedSubDataComplete),
            TestSuite::Compare::Container);
    }
    #endif

    CubeMapTexture texture;
    texture.setCompressedImage(CubeMapCoordinate::PositiveX, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedImage(CubeMapCoordinate::NegativeX, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedImage(CubeMapCoordinate::PositiveY, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedImage(CubeMapCoordinate::NegativeY, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedImage(CubeMapCoordinate::PositiveZ, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedImage(CubeMapCoordinate::NegativeZ, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, Vector2i{4}, CompressedImageView2D{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, unsafeSuffix(CompressedData, _compressedDataOffset)});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0, {});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{12});

    {
        CORRADE_EXPECT_FAIL_IF(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>() && Context::current().isExtensionSupported<Extensions::GL::ARB::direct_state_access>() && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Non-default compressed pixel storage for cube map textures behaves weirdly on NVidia for client-memory images when using ARB_direct_state_access");

        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedSubDataComplete),
            TestSuite::Compare::Container);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::subImageBuffer() {
    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::NegativeX, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::PositiveY, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::NegativeY, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::PositiveZ, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::NegativeZ, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setSubImage(CubeMapCoordinate::PositiveX, 0, Vector2i(1), BufferImage2D{
        _dataStorage, PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data, _dataOffset), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image = texture.image(CubeMapCoordinate::PositiveX, 0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(SubDataComplete),
        TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureGLTest::compressedSubImageBuffer() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::GL::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #endif

    CubeMapTexture texture;
    texture.setCompressedImage(CubeMapCoordinate::PositiveX, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedImage(CubeMapCoordinate::NegativeX, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedImage(CubeMapCoordinate::PositiveY, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedImage(CubeMapCoordinate::NegativeY, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedImage(CubeMapCoordinate::PositiveZ, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedImage(CubeMapCoordinate::NegativeZ, 0,
        CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    texture.setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, Vector2i{4}, CompressedBufferImage2D{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, unsafeSuffix(CompressedData, _compressedDataOffset), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0,
        {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{12});
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(CompressedSubDataComplete),
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::subImageQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    /* I'm too lazy to call setSubImage() six times */
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::GL::ARB::direct_state_access::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4, 1}, SubDataComplete});

    MAGNUM_VERIFY_NO_ERROR();

    Image3D image = texture.subImage(0, Range3Di::fromSize({1, 1, 0}, {2, 2, 1}),
        {_dataStorage, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 1));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(image.data()).suffix(_dataOffset),
        Containers::arrayView(Data), TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedSubImageQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    /* I'm too lazy to call setSubImage() six times */
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::GL::ARB::direct_state_access::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12})
        .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12, 1}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_ERROR();

    /* Test also without compressed pixel storage to ensure that both size
       computations work */
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>()) {
        CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), {});

        MAGNUM_VERIFY_NO_ERROR();

        CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 1}));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedData),
            TestSuite::Compare::Container);
    }

    CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), {_compressedDataStorage});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_compressedDataOffset),
        Containers::arrayView(CompressedData), TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::subImageQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    /* I'm too lazy to call setSubImage() six times */
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::GL::ARB::direct_state_access::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4, 1}, SubDataComplete});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage3D image = texture.subImage(0, Range3Di::fromSize({1, 1, 0}, {2, 2, 1}),
        {_dataStorage, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 1));
    CORRADE_COMPARE_AS(imageData.suffix(_dataOffset),
        Containers::arrayView(Data), TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedSubImageQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    /* I'm too lazy to call setSubImage() six times */
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::GL::ARB::direct_state_access::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12, 1}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_ERROR();

    /* Test also without compressed pixel storage to ensure that both size
       computations work */
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>()) {
        CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), {}, BufferUsage::StaticRead);
        const auto imageData = image.buffer().data<UnsignedByte>();

        MAGNUM_VERIFY_NO_ERROR();

        CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 1}));
        CORRADE_COMPARE_AS(imageData, Containers::arrayView(CompressedData),
            TestSuite::Compare::Container);
    }

    CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), {_compressedDataStorage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE_AS(imageData.suffix(_compressedDataOffset),
        Containers::arrayView(CompressedData), TestSuite::Compare::Container);
}
#endif

void CubeMapTextureGLTest::generateMipmap() {
    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));
    texture.setImage(CubeMapCoordinate::PositiveY, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));
    texture.setImage(CubeMapCoordinate::PositiveZ, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));
    texture.setImage(CubeMapCoordinate::NegativeX, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));
    texture.setImage(CubeMapCoordinate::NegativeY, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));
    texture.setImage(CubeMapCoordinate::NegativeZ, 0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i( 0));
    #endif

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 1));

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
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(Vector2i(8), 4));

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::CubeMapTextureGLTest)
