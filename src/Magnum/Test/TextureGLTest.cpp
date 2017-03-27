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
#include "Magnum/Image.h"
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/ImageFormat.h"
#endif
#include "Magnum/OpenGLTester.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Texture.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace Test {

struct TextureGLTest: OpenGLTester {
    explicit TextureGLTest();

    #ifndef MAGNUM_TARGET_GLES
    void construct1D();
    #endif
    void construct2D();
    void construct3D();

    #ifndef MAGNUM_TARGET_GLES
    void wrap1D();
    #endif
    void wrap2D();
    void wrap3D();

    #ifndef MAGNUM_TARGET_GLES
    void bind1D();
    #endif
    void bind2D();
    void bind3D();

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    void bindImage1D();
    #endif
    void bindImage2D();
    void bindImage3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void sampling1D();
    #endif
    void sampling2D();
    void sampling3D();

    #ifndef MAGNUM_TARGET_GLES
    void samplingSRGBDecode1D();
    #endif
    void samplingSRGBDecode2D();
    void samplingSRGBDecode3D();

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    void samplingSwizzle1D();
    #endif
    void samplingSwizzle2D();
    void samplingSwizzle3D();
    #else
    void samplingMaxLevel2D();
    void samplingMaxLevel3D();
    void samplingCompare2D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void samplingBorderInteger1D();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingBorderInteger2D();
    void samplingBorderInteger3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void samplingDepthStencilMode1D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void samplingDepthStencilMode2D();
    void samplingDepthStencilMode3D();
    #endif
    #ifdef MAGNUM_TARGET_GLES
    void samplingBorder2D();
    void samplingBorder3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void storage1D();
    #endif
    void storage2D();
    void storage3D();

    #ifndef MAGNUM_TARGET_GLES
    void image1D();
    void compressedImage1D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void image1DBuffer();
    void compressedImage1DBuffer();
    #endif
    void image2D();
    void compressedImage2D();
    #ifndef MAGNUM_TARGET_GLES2
    void image2DBuffer();
    void compressedImage2DBuffer();
    #endif
    void image3D();
    void compressedImage3D();
    #ifndef MAGNUM_TARGET_GLES2
    void image3DBuffer();
    void compressedImage3DBuffer();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void subImage1D();
    void compressedSubImage1D();
    void subImage1DBuffer();
    void compressedSubImage1DBuffer();
    void subImage1DQuery();
    void compressedSubImage1DQuery();
    void subImage1DQueryBuffer();
    void compressedSubImage1DQueryBuffer();
    #endif
    void subImage2D();
    void compressedSubImage2D();
    #ifndef MAGNUM_TARGET_GLES2
    void subImage2DBuffer();
    void compressedSubImage2DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void subImage2DQuery();
    void compressedSubImage2DQuery();
    void subImage2DQueryBuffer();
    void compressedSubImage2DQueryBuffer();
    #endif
    void subImage3D();
    void compressedSubImage3D();
    #ifndef MAGNUM_TARGET_GLES2
    void subImage3DBuffer();
    void compressedSubImage3DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void subImage3DQuery();
    void compressedSubImage3DQuery();
    void subImage3DQueryBuffer();
    void compressedSubImage3DQueryBuffer();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void generateMipmap1D();
    #endif
    void generateMipmap2D();
    void generateMipmap3D();

    #ifndef MAGNUM_TARGET_GLES
    void invalidateImage1D();
    #endif
    void invalidateImage2D();
    void invalidateImage3D();

    #ifndef MAGNUM_TARGET_GLES
    void invalidateSubImage1D();
    #endif
    void invalidateSubImage2D();
    void invalidateSubImage3D();

    PixelStorage _dataStorage2D, _dataStorage3D;
    std::size_t _dataOffset2D, _dataOffset3D;

    #ifndef MAGNUM_TARGET_GLES
    CompressedPixelStorage _compressedDataStorage2D, _compressedDataStorage3D;
    #endif
    std::size_t _compressedDataOffset2D, _compressedDataOffset3D;
};

TextureGLTest::TextureGLTest() {
    addTests({
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::construct1D,
        #endif
        &TextureGLTest::construct2D,
        &TextureGLTest::construct3D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::wrap1D,
        #endif
        &TextureGLTest::wrap2D,
        &TextureGLTest::wrap3D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::bind1D,
        #endif
        &TextureGLTest::bind2D,
        &TextureGLTest::bind3D,

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::bindImage1D,
        #endif
        &TextureGLTest::bindImage2D,
        &TextureGLTest::bindImage3D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::sampling1D,
        #endif
        &TextureGLTest::sampling2D,
        &TextureGLTest::sampling3D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingSRGBDecode1D,
        #endif
        &TextureGLTest::samplingSRGBDecode2D,
        &TextureGLTest::samplingSRGBDecode3D,

        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingSwizzle1D,
        #endif
        &TextureGLTest::samplingSwizzle2D,
        &TextureGLTest::samplingSwizzle3D,
        #else
        &TextureGLTest::samplingMaxLevel2D,
        &TextureGLTest::samplingMaxLevel3D,
        &TextureGLTest::samplingCompare2D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingBorderInteger1D,
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        &TextureGLTest::samplingBorderInteger2D,
        &TextureGLTest::samplingBorderInteger3D,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingDepthStencilMode1D,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::samplingDepthStencilMode2D,
        &TextureGLTest::samplingDepthStencilMode3D,
        #endif
        #ifdef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingBorder2D,
        &TextureGLTest::samplingBorder3D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::storage1D,
        #endif
        &TextureGLTest::storage2D,
        &TextureGLTest::storage3D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::image1D,
        &TextureGLTest::compressedImage1D,
        &TextureGLTest::image1DBuffer,
        &TextureGLTest::compressedImage1DBuffer,
        #endif
        &TextureGLTest::image2D,
        &TextureGLTest::compressedImage2D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::image2DBuffer,
        &TextureGLTest::compressedImage2DBuffer,
        #endif
        &TextureGLTest::image3D,
        &TextureGLTest::compressedImage3D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::image3DBuffer,
        &TextureGLTest::compressedImage3DBuffer,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::subImage1D,
        &TextureGLTest::compressedSubImage1D,
        &TextureGLTest::subImage1DBuffer,
        &TextureGLTest::compressedSubImage1DBuffer,
        &TextureGLTest::subImage1DQuery,
        &TextureGLTest::compressedSubImage1DQuery,
        &TextureGLTest::subImage1DQueryBuffer,
        &TextureGLTest::compressedSubImage1DQueryBuffer,
        #endif
        &TextureGLTest::subImage2D,
        &TextureGLTest::compressedSubImage2D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::subImage2DBuffer,
        &TextureGLTest::compressedSubImage2DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::subImage2DQuery,
        &TextureGLTest::compressedSubImage2DQuery,
        &TextureGLTest::subImage2DQueryBuffer,
        &TextureGLTest::compressedSubImage2DQueryBuffer,
        #endif
        &TextureGLTest::subImage3D,
        &TextureGLTest::compressedSubImage3D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::subImage3DBuffer,
        &TextureGLTest::compressedSubImage3DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::subImage3DQuery,
        &TextureGLTest::compressedSubImage3DQuery,
        &TextureGLTest::subImage3DQueryBuffer,
        &TextureGLTest::compressedSubImage3DQueryBuffer,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::generateMipmap1D,
        #endif
        &TextureGLTest::generateMipmap2D,
        &TextureGLTest::generateMipmap3D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::invalidateImage1D,
        #endif
        &TextureGLTest::invalidateImage2D,
        &TextureGLTest::invalidateImage3D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::invalidateSubImage1D,
        #endif
        &TextureGLTest::invalidateSubImage2D,
        &TextureGLTest::invalidateSubImage3D,
    });

    _dataStorage2D = PixelStorage{}.setSkip({0, 1, 0});
    _dataStorage3D = PixelStorage{}.setSkip({0, 0, 1});
    _dataOffset2D = 8;
    _dataOffset3D = 16;
    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::unpack_subimage>())
    #endif
    {
        _dataStorage2D = _dataStorage3D = {};
        _dataOffset2D = _dataOffset3D = 0;
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>())
    {
        _compressedDataStorage2D = CompressedPixelStorage{}
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16)
            .setSkip({0, 4, 0});
        _compressedDataStorage3D = CompressedPixelStorage{}
            .setCompressedBlockSize({4, 4, 4})
            .setCompressedBlockDataSize(16*4)
            .setSkip({0, 0, 4});
        _compressedDataOffset2D = 16;
        _compressedDataOffset3D = 16*4;
    } else
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D = _compressedDataStorage3D = {};
        #endif
        _compressedDataOffset2D = _compressedDataOffset3D = 0;
    }
}

namespace {
    template<std::size_t size, class T> Containers::ArrayView<const T> unsafeSuffix(const T(&data)[size], std::size_t offset) {
        static_assert(sizeof(T) == 1, "");
        return {data - offset, size + offset};
    }
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::construct1D() {
    {
        Texture1D texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::construct2D() {
    {
        Texture2D texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::construct3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    {
        Texture3D texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::wrap1D() {
    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = Texture1D::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    Texture1D::wrap(id);
    glDeleteTextures(1, &id);
}
#endif

void TextureGLTest::wrap2D() {
    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = Texture2D::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    Texture2D::wrap(id);
    glDeleteTextures(1, &id);
}

void TextureGLTest::wrap3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = Texture3D::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    Texture3D::wrap(id);
    glDeleteTextures(1, &id);
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::bind1D() {
    Texture1D texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::bind2D() {
    Texture2D texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::bind3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
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
#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::bindImage1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::GL::ARB::shader_image_load_store::string() + std::string(" is not supported."));

    Texture1D texture;
    texture.setStorage(1, TextureFormat::RGBA8, 32)
        .bindImage(2, 0, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImage(2);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::bindImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::GL::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{32})
        .bindImage(2, 0, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImage(2);

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

void TextureGLTest::bindImage3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::GL::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Texture3D texture;
    texture.setStorage(1, TextureFormat::RGBA8, {32, 32, 4})
        .bindImage(2, 0, 1, ImageAccess::ReadWrite, ImageFormat::RGBA8);

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

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::sampling1D() {
    Texture1D texture;
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           .setMinLod(-750.0f)
           .setMaxLod(750.0f)
           .setLodBias(0.5f)
           .setBaseLevel(1)
           .setMaxLevel(750)
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
           .setCompareMode(Sampler::CompareMode::CompareRefToTexture)
           .setCompareFunction(Sampler::CompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::samplingSRGBDecode1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    Texture1D texture;
    texture.setSRGBDecode(false);

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::samplingSwizzle1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_swizzle::string() + std::string(" is not supported."));

    Texture1D texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::samplingBorderInteger1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));

    Texture1D a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture1D b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::samplingDepthStencilMode1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::GL::ARB::stencil_texturing::string() + std::string(" is not supported."));

    Texture1D texture;
    texture.setDepthStencilMode(Sampler::DepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::sampling2D() {
    Texture2D texture;
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
           .setCompareMode(Sampler::CompareMode::CompareRefToTexture)
           .setCompareFunction(Sampler::CompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::samplingSRGBDecode2D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::sRGB>())
        CORRADE_SKIP(Extensions::GL::EXT::sRGB::string() + std::string(" is not supported."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    Texture2D texture;
    texture.setSRGBDecode(false);

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::samplingSwizzle2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_swizzle::string() + std::string(" is not supported."));
    #endif

    Texture2D texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_ERROR();
}
#else
void TextureGLTest::samplingMaxLevel2D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::GL::APPLE::texture_max_level::string() + std::string(" is not supported."));

    Texture2D texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::samplingCompare2D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::shadow_samplers>())
        CORRADE_SKIP(Extensions::GL::EXT::shadow_samplers::string() + std::string(" is not supported."));

    Texture2D texture;
    texture.setCompareMode(Sampler::CompareMode::CompareRefToTexture)
           .setCompareFunction(Sampler::CompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureGLTest::samplingBorderInteger2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_border_clamp::string() + std::string(" is not supported."));
    #endif

    Texture2D a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture2D b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::samplingDepthStencilMode2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::GL::ARB::stencil_texturing::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Texture2D texture;
    texture.setDepthStencilMode(Sampler::DepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifdef MAGNUM_TARGET_GLES
void TextureGLTest::samplingBorder2D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::NV::texture_border_clamp>() &&
       !Context::current().isExtensionSupported<Extensions::GL::EXT::texture_border_clamp>())
        CORRADE_SKIP("No required extension is supported.");

    Texture2D texture;
    texture.setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::sampling3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
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

void TextureGLTest::samplingSRGBDecode3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::sRGB>())
        CORRADE_SKIP(Extensions::GL::EXT::sRGB::string() + std::string(" is not supported."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setSRGBDecode(false);

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::samplingSwizzle3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_swizzle::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_ERROR();
}
#else
void TextureGLTest::samplingMaxLevel3D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::GL::APPLE::texture_max_level::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureGLTest::samplingBorderInteger3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_border_clamp::string() + std::string(" is not supported."));
    #endif

    Texture3D a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture3D b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::samplingDepthStencilMode3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::GL::ARB::stencil_texturing::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Texture3D texture;
    texture.setDepthStencilMode(Sampler::DepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifdef MAGNUM_TARGET_GLES
void TextureGLTest::samplingBorder3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    if(!Context::current().isExtensionSupported<Extensions::GL::NV::texture_border_clamp>() &&
       !Context::current().isExtensionSupported<Extensions::GL::EXT::texture_border_clamp>())
        CORRADE_SKIP("No required extension is supported.");

    Texture3D texture;
    texture.setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::storage1D() {
    Texture1D texture;
    texture.setStorage(5, TextureFormat::RGBA8, 32);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), 32);
    CORRADE_COMPARE(texture.imageSize(1), 16);
    CORRADE_COMPARE(texture.imageSize(2),  8);
    CORRADE_COMPARE(texture.imageSize(3),  4);
    CORRADE_COMPARE(texture.imageSize(4),  2);
    CORRADE_COMPARE(texture.imageSize(5),  0); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::storage2D() {
    Texture2D texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES2
    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported, skipping image size testing.");
    #endif

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

void TextureGLTest::storage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector3i(32));

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES2
    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported, skipping image size testing.");
    #endif

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

namespace {
    constexpr UnsignedByte Data1D[] = { 0x00, 0x01, 0x02, 0x03,
                                        0x04, 0x05, 0x06, 0x07 };

    const auto DataStorage1D = PixelStorage{}.setSkip({1, 0, 0});
    const std::size_t DataOffset1D = 4;
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::image1D() {
    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView1D{DataStorage1D,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2, unsafeSuffix(Data1D, DataOffset1D)});

    MAGNUM_VERIFY_NO_ERROR();

    Image1D image = texture.image(0, {DataStorage1D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(DataOffset1D),
        Containers::arrayView(Data1D), TestSuite::Compare::Container);
}

void TextureGLTest::compressedImage1D() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::image1DBuffer() {
    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage1D{DataStorage1D,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2, unsafeSuffix(Data1D, DataOffset1D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage1D image = texture.image(0,
        {DataStorage1D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticDraw);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(imageData.suffix(DataOffset1D),
        Containers::arrayView(Data1D), TestSuite::Compare::Container);
}

void TextureGLTest::compressedImage1DBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}
#endif

namespace {
    constexpr UnsignedByte Data2D[] = { 0x00, 0x01, 0x02, 0x03,
                                        0x04, 0x05, 0x06, 0x07,
                                        0x08, 0x09, 0x0a, 0x0b,
                                        0x0c, 0x0d, 0x0e, 0x0f };

    /* Just 4x4 0x00 - 0x3f compressed using RGBA DXT3 by the driver */
    constexpr UnsignedByte CompressedData2D[] = {
          0,  17, 17,  34,  34,  51,  51,  67,
        232,  57,  0,   0, 213, 255, 170,   2
    };
}

void TextureGLTest::image2D() {
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView2D{_dataStorage2D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data2D, _dataOffset2D)});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = texture.image(0, {_dataStorage2D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_dataOffset2D),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
    #endif
}

void TextureGLTest::compressedImage2D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::GL::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #endif

    Texture2D texture;
    texture.setCompressedImage(0, CompressedImageView2D{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, unsafeSuffix(CompressedData2D, _compressedDataOffset2D)});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(0, {_compressedDataStorage2D});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});

    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_compressedDataOffset2D),
        Containers::arrayView(CompressedData2D),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::image2DBuffer() {
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage2D{_dataStorage2D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data2D, _dataOffset2D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image = texture.image(0,
        {_dataStorage2D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(imageData.suffix(_dataOffset2D),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
    #endif
}

void TextureGLTest::compressedImage2DBuffer() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::GL::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #endif

    Texture2D texture;
    texture.setCompressedImage(0, CompressedBufferImage2D{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, unsafeSuffix(CompressedData2D, _compressedDataOffset2D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage2D image = texture.compressedImage(0, {_compressedDataStorage2D}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(imageData.suffix(_compressedDataOffset2D),
        Containers::arrayView(CompressedData2D),
        TestSuite::Compare::Container);
    #endif
}
#endif

namespace {
    constexpr UnsignedByte Data3D[] = { 0x00, 0x01, 0x02, 0x03,
                                        0x04, 0x05, 0x06, 0x07,
                                        0x08, 0x09, 0x0a, 0x0b,
                                        0x0c, 0x0d, 0x0e, 0x0f,
                                        0x10, 0x11, 0x12, 0x13,
                                        0x14, 0x15, 0x16, 0x17,
                                        0x18, 0x19, 0x1a, 0x1b,
                                        0x1c, 0x1d, 0x1e, 0x1f };

    #ifndef MAGNUM_TARGET_GLES
    /* Just 4x4x4 0x00 - 0xff compressed using RGBA BPTC Unorm by the driver */
    constexpr UnsignedByte CompressedData3D[] = {
        144, 224, 128,   3,  80,   0, 129, 170,
         84, 253,  73,  36, 109, 100, 107, 255,
        144, 232, 161, 135,  94, 244, 129, 170,
         84, 253,  65,  34, 109, 100, 107, 255,
        144, 240, 194,  11,  47, 248, 130, 170,
         84, 253,  65,  34, 109, 100, 107, 251,
        144, 247, 223, 143,  63, 252, 131, 170,
         84, 253,  73,  34, 109, 100,  91, 251
    };
    #endif
}

void TextureGLTest::image3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView3D{_dataStorage3D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2), unsafeSuffix(Data3D, _dataOffset3D)});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {_dataStorage3D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_dataOffset3D),
        Containers::arrayView(Data3D), TestSuite::Compare::Container);
    #endif
}

void TextureGLTest::compressedImage3D() {
    #ifdef MAGNUM_TARGET_GLES
    /** @todo ASTC HDR, when available on any ES driver */
    CORRADE_SKIP("No 3D texture compression format available on OpenGL ES.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_compression_bptc::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setCompressedImage(0, CompressedImageView3D{_compressedDataStorage3D,
        CompressedPixelFormat::RGBABptcUnorm, Vector3i{4}, unsafeSuffix(CompressedData3D, _compressedDataOffset3D)});

    MAGNUM_VERIFY_NO_ERROR();

    CompressedImage3D image = texture.compressedImage(0, {_compressedDataStorage3D});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_compressedDataOffset3D),
        Containers::arrayView(CompressedData3D),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::image3DBuffer() {
    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage3D{_dataStorage3D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2), unsafeSuffix(Data3D, _dataOffset3D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0,
        {_dataStorage3D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(imageData.suffix(_dataOffset3D),
        Containers::arrayView(Data3D), TestSuite::Compare::Container);
    #endif
}

void TextureGLTest::compressedImage3DBuffer() {
    #ifdef MAGNUM_TARGET_GLES
    /** @todo ASTC HDR, when available on any ES driver */
    CORRADE_SKIP("No 3D texture compression format available on OpenGL ES.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_compression_bptc::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setCompressedImage(0, CompressedBufferImage3D{_compressedDataStorage3D,
        CompressedPixelFormat::RGBABptcUnorm, Vector3i{4}, unsafeSuffix(CompressedData3D, _compressedDataOffset3D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    CompressedBufferImage3D image = texture.compressedImage(0, {_compressedDataStorage3D}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{4});
    CORRADE_COMPARE_AS(imageData.suffix(_compressedDataOffset3D),
        Containers::arrayView(CompressedData3D),
        TestSuite::Compare::Container);
    #endif
}
#endif

namespace {
    constexpr UnsignedByte Zero1D[4*4] = {};
    constexpr UnsignedByte SubData1DComplete[] = {
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0
    };
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::subImage1D() {
    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView1D(PixelFormat::RGBA, PixelType::UnsignedByte, 4, Zero1D));
    texture.setSubImage(0, 1, ImageView1D{DataStorage1D,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2, unsafeSuffix(Data1D, DataOffset1D)});

    MAGNUM_VERIFY_NO_ERROR();

    Image1D image = texture.image(0, {DataStorage1D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(DataOffset1D),
        Containers::arrayView(SubData1DComplete),
        TestSuite::Compare::Container);
}

void TextureGLTest::compressedSubImage1D() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::subImage1DBuffer() {
    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView1D(PixelFormat::RGBA, PixelType::UnsignedByte, 4, Zero1D));
    texture.setSubImage(0, 1, BufferImage1D{DataStorage1D,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2, unsafeSuffix(Data1D, DataOffset1D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage1D image = texture.image(0, {DataStorage1D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), 4);
    CORRADE_COMPARE_AS(imageData.suffix(DataOffset1D),
        Containers::arrayView(SubData1DComplete),
        TestSuite::Compare::Container);
}

void TextureGLTest::compressedSubImage1DBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::subImage1DQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture1D texture;
    texture.setStorage(1, TextureFormat::RGBA8, 4)
           .setSubImage(0, {}, ImageView1D{PixelFormat::RGBA, PixelType::UnsignedByte, 4, SubData1DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    Image1D image = texture.subImage(0, Range1Di::fromSize(1, 2),
        {DataStorage1D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(DataOffset1D),
        Containers::arrayView(Data1D), TestSuite::Compare::Container);
}

void TextureGLTest::compressedSubImage1DQuery() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::subImage1DQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture1D texture;
    texture.setStorage(1, TextureFormat::RGBA8, 4)
           .setSubImage(0, {}, ImageView1D{PixelFormat::RGBA, PixelType::UnsignedByte, 4, SubData1DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage1D image = texture.subImage(0, Range1Di::fromSize(1, 2),
        {DataStorage1D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(imageData.suffix(DataOffset1D),
        Containers::arrayView(Data1D), TestSuite::Compare::Container);
}

void TextureGLTest::compressedSubImage1DQueryBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}
#endif

namespace {
    constexpr UnsignedByte Zero2D[4*4*4] = {};

    /* Just 12x4 zeros compressed using RGBA DXT3 by the driver */
    constexpr UnsignedByte CompressedZero2D[3*16] = {};

    constexpr UnsignedByte SubData2DComplete[] = {
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    };

    /* Combination of CompressedZero2D and CompressedData2D */
    constexpr UnsignedByte CompressedSubData2DComplete[] = {
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,  17,  17,  34,  34,  51,  51,  67,
        232,  57,   0,   0, 213, 255, 170,   2,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0
    };
}

void TextureGLTest::subImage2D() {
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero2D));
    texture.setSubImage(0, Vector2i(1), ImageView2D{_dataStorage2D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data2D, _dataOffset2D)});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}

void TextureGLTest::compressedSubImage2D() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::GL::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #endif

    Texture2D texture;
    texture.setCompressedImage(0, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3,
        {12, 4}, CompressedZero2D});
    texture.setCompressedSubImage(0, {4, 0}, CompressedImageView2D{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, unsafeSuffix(CompressedData2D, _compressedDataOffset2D)});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector2i{12, 4}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(CompressedSubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::subImage2DBuffer() {
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero2D));
    texture.setSubImage(0, Vector2i(1), BufferImage2D{_dataStorage2D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data2D, _dataOffset2D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(SubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}

void TextureGLTest::compressedSubImage2DBuffer() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::GL::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #endif

    Texture2D texture;
    texture.setCompressedImage(0, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3,
        {12, 4}, CompressedZero2D});
    texture.setCompressedSubImage(0, {4, 0}, CompressedBufferImage2D{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, unsafeSuffix(CompressedData2D, _compressedDataOffset2D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage2D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector2i{12, 4}));
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(CompressedSubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::subImage2DQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubData2DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image = texture.subImage(0, Range2Di::fromSize(Vector2i{1}, Vector2i{2}),
        {_dataStorage2D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_dataOffset2D),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

void TextureGLTest::compressedSubImage2DQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 4})
           .setCompressedSubImage(0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 4}, CompressedSubData2DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    /* Test also without compressed pixel storage to ensure that both size
       computations work */
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>()) {
        CompressedImage2D image = texture.compressedSubImage(0, Range2Di::fromSize({4, 0}, Vector2i{4}), {});

        MAGNUM_VERIFY_NO_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i{4});
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedData2D),
            TestSuite::Compare::Container);
    }

    CompressedImage2D image = texture.compressedSubImage(0, Range2Di::fromSize({4, 0}, Vector2i{4}), {_compressedDataStorage2D});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_compressedDataOffset2D),
        Containers::arrayView(CompressedData2D),
        TestSuite::Compare::Container);
}

void TextureGLTest::subImage2DQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubData2DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image = texture.subImage(0, Range2Di::fromSize(Vector2i{1}, Vector2i{2}),
        {_dataStorage2D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(imageData.suffix(_dataOffset2D),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

void TextureGLTest::compressedSubImage2DQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 4})
           .setCompressedSubImage(0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 4}, CompressedSubData2DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    /* Test also without compressed pixel storage to ensure that both size
       computations work */
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>()) {
        CompressedBufferImage2D image = texture.compressedSubImage(0, Range2Di::fromSize({4, 0}, Vector2i{4}), {}, BufferUsage::StaticRead);
        const auto imageData = image.buffer().data<UnsignedByte>();

        MAGNUM_VERIFY_NO_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i{4});
        CORRADE_COMPARE_AS(imageData, Containers::arrayView(CompressedData2D),
            TestSuite::Compare::Container);
    }

    CompressedBufferImage2D image = texture.compressedSubImage(0, Range2Di::fromSize({4, 0}, Vector2i{4}), {_compressedDataStorage2D}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(imageData.suffix(_compressedDataOffset2D),
        Containers::arrayView(CompressedData2D),
        TestSuite::Compare::Container);
}
#endif

namespace {
    constexpr UnsignedByte Zero3D[4*4*4*4] = {};

    /* Just 12x4x4 zeros compressed using RGBA BPTC Unorm by the driver */
    constexpr UnsignedByte CompressedZero3D[3*4*16] = {
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    constexpr UnsignedByte SubData3DComplete[] = {
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
    };

    #ifndef MAGNUM_TARGET_GLES
    /* Combination of CompressedZero3D and CompressedData3D. Note that, in
       contrast to array textures, the data are ordered in "cubes" instead of
       slices. */
    constexpr UnsignedByte CompressedSubData3DComplete[] = {
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        144, 224, 128,   3,  80,   0, 129, 170,
         84, 253,  73,  36, 109, 100, 107, 255,
        144, 232, 161, 135,  94, 244, 129, 170,
         84, 253,  65,  34, 109, 100, 107, 255,
        144, 240, 194,  11,  47, 248, 130, 170,
         84, 253,  65,  34, 109, 100, 107, 251,
        144, 247, 223, 143,  63, 252, 131, 170,
         84, 253,  73,  34, 109, 100,  91, 251,

        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    #endif
}

void TextureGLTest::subImage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(4), Zero3D));
    texture.setSubImage(0, Vector3i(1), ImageView3D{_dataStorage3D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2), unsafeSuffix(Data3D, _dataOffset3D)});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubData3DComplete),
        TestSuite::Compare::Container);
    #endif
}

void TextureGLTest::compressedSubImage3D() {
    #ifdef MAGNUM_TARGET_GLES
    /** @todo ASTC HDR, when available on any ES driver */
    CORRADE_SKIP("No 3D texture compression format available on OpenGL ES.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_compression_bptc::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBABptcUnorm,
        {12, 4, 4}, CompressedZero3D});
    texture.setCompressedSubImage(0, {4, 0, 0}, CompressedImageView3D{_compressedDataStorage3D,
        CompressedPixelFormat::RGBABptcUnorm, Vector3i{4}, unsafeSuffix(CompressedData3D, _compressedDataOffset3D)});

    MAGNUM_VERIFY_NO_ERROR();

    CompressedImage3D image = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 4, 4}));

    {
        CORRADE_EXPECT_FAIL_IF(!Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>() && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Default compressed pixel storage behaves weirdly with BPTC compression on NVidia");

        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedSubData3DComplete),
            TestSuite::Compare::Container);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::subImage3DBuffer() {
    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(4), Zero3D));
    texture.setSubImage(0, Vector3i(1), BufferImage3D{_dataStorage3D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2), unsafeSuffix(Data3D, _dataOffset3D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(SubData3DComplete),
        TestSuite::Compare::Container);
    #endif
}

void TextureGLTest::compressedSubImage3DBuffer() {
    #ifdef MAGNUM_TARGET_GLES
    /** @todo ASTC HDR, when available on any ES driver */
    CORRADE_SKIP("No 3D texture compression format available on OpenGL ES.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_compression_bptc::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBABptcUnorm,
        {12, 4, 4}, CompressedZero3D});
    texture.setCompressedSubImage(0, {4, 0, 0}, CompressedImageView3D{_compressedDataStorage3D,
        CompressedPixelFormat::RGBABptcUnorm, Vector3i{4}, unsafeSuffix(CompressedData3D, _compressedDataOffset3D)});

    MAGNUM_VERIFY_NO_ERROR();

    CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 4, 4}));

    {
        CORRADE_EXPECT_FAIL_IF(!Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>() && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Default compressed pixel storage behaves weirdly with BPTC compression on NVidia");

        CORRADE_COMPARE_AS(imageData,
            Containers::arrayView(CompressedSubData3DComplete),
            TestSuite::Compare::Container);
    }
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::subImage3DQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector3i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{4}, SubData3DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    Image3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, Vector3i{2}),
        {_dataStorage3D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_dataOffset3D),
        Containers::arrayView(Data3D), TestSuite::Compare::Container);
}

void TextureGLTest::compressedSubImage3DQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_compression_bptc::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBABptcUnorm, {12, 4, 4})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBABptcUnorm, {12, 4, 4}, CompressedSubData3DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    /* Test also without compressed pixel storage to ensure that both size
       computations work */
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>()) {
        CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 0}, Vector3i{4}), {});

        MAGNUM_VERIFY_NO_ERROR();

        CORRADE_COMPARE(image.size(), (Vector3i{4}));

        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::NVidia,
            "Default compressed pixel storage behaves weirdly with BPTC compression on NVidia");

        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedData3D),
            TestSuite::Compare::Container);
    }

    CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 0}, Vector3i{4}), {_compressedDataStorage3D});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4}));

    {
        CORRADE_EXPECT_FAIL_IF(!Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>() && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Default compressed pixel storage behaves weirdly with BPTC compression on NVidia");

        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_compressedDataOffset3D),
            Containers::arrayView(CompressedData3D), TestSuite::Compare::Container);
    }
}

void TextureGLTest::subImage3DQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector3i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{4}, SubData3DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, Vector3i{2}),
        {_dataStorage3D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{2});
    CORRADE_COMPARE_AS(imageData.suffix(_dataOffset3D),
        Containers::arrayView(Data3D), TestSuite::Compare::Container);
}

void TextureGLTest::compressedSubImage3DQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_compression_bptc::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBABptcUnorm, {12, 4, 4})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBABptcUnorm, {12, 4, 4}, CompressedSubData3DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    /* Test also without compressed pixel storage to ensure that both size
       computations work */
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>()) {
        CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 0}, Vector3i{4}), {}, BufferUsage::StaticRead);
        const auto imageData = image.buffer().data<UnsignedByte>();

        MAGNUM_VERIFY_NO_ERROR();

        CORRADE_COMPARE(image.size(), Vector3i{4});

        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::NVidia,
            "Default compressed pixel storage behaves weirdly with BPTC compression on NVidia");

        CORRADE_COMPARE_AS(imageData, Containers::arrayView(CompressedData3D),
            TestSuite::Compare::Container);
    }

    CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 0}, Vector3i{4}), {_compressedDataStorage3D}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{4});

    {
        CORRADE_EXPECT_FAIL_IF(!Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>() && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Default compressed pixel storage behaves weirdly with BPTC compression on NVidia");

        CORRADE_COMPARE_AS(imageData.suffix(_compressedDataOffset3D),
            Containers::arrayView(CompressedData3D),
            TestSuite::Compare::Container);
    }
}

void TextureGLTest::generateMipmap1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));

    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView1D(PixelFormat::RGBA, PixelType::UnsignedByte, 32));

    CORRADE_COMPARE(texture.imageSize(0), 32);
    CORRADE_COMPARE(texture.imageSize(1),  0);

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), 32);
    CORRADE_COMPARE(texture.imageSize(1), 16);
    CORRADE_COMPARE(texture.imageSize(2),  8);
    CORRADE_COMPARE(texture.imageSize(3),  4);
    CORRADE_COMPARE(texture.imageSize(4),  2);
    CORRADE_COMPARE(texture.imageSize(5),  1);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::generateMipmap2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    #endif

    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8,
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

void TextureGLTest::generateMipmap3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(32)));

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i( 0));
    #endif

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 1));

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::invalidateImage1D() {
    Texture1D texture;
    texture.setStorage(2, TextureFormat::RGBA8, 32);
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::invalidateImage2D() {
    Texture2D texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::invalidateImage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::invalidateSubImage1D() {
    Texture1D texture;
    texture.setStorage(2, TextureFormat::RGBA8, 32);
    texture.invalidateSubImage(1, 2, 8);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::invalidateSubImage2D() {
    Texture2D texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateSubImage(1, Vector2i(2), Vector2i(8));

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::invalidateSubImage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::TextureGLTest)
