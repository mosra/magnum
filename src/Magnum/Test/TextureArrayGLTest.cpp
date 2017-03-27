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
#include "Magnum/BufferImage.h"
#include "Magnum/Image.h"
#include "Magnum/ImageFormat.h"
#include "Magnum/OpenGLTester.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/TextureArray.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace Test {

struct TextureArrayGLTest: OpenGLTester {
    explicit TextureArrayGLTest();

    #ifndef MAGNUM_TARGET_GLES
    void construct1D();
    #endif
    void construct2D();

    #ifndef MAGNUM_TARGET_GLES
    void wrap1D();
    #endif
    void wrap2D();

    #ifndef MAGNUM_TARGET_GLES
    void bind1D();
    #endif
    void bind2D();

    #ifndef MAGNUM_TARGET_WEBGL
    #ifndef MAGNUM_TARGET_GLES
    void bindImage1D();
    #endif
    void bindImage2D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void sampling1D();
    #endif
    void sampling2D();

    #ifndef MAGNUM_TARGET_GLES
    void samplingSRGBDecode1D();
    #endif
    void samplingSRGBDecode2D();

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    void samplingSwizzle1D();
    #endif
    void samplingSwizzle2D();
    #else
    void samplingMaxLevel2D();
    void samplingCompare2D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void samplingBorderInteger1D();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingBorderInteger2D();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void samplingDepthStencilMode1D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void samplingDepthStencilMode2D();
    #endif
    #ifdef MAGNUM_TARGET_GLES
    void samplingBorder2D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void storage1D();
    #endif
    void storage2D();

    #ifndef MAGNUM_TARGET_GLES
    void image1D();
    void compressedImage1D();
    void image1DBuffer();
    void compressedImage1DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void image2D();
    void compressedImage2D();
    void image2DBuffer();
    void compressedImage2DBuffer();
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
    void subImage2DBuffer();
    void compressedSubImage2DBuffer();
    #ifndef MAGNUM_TARGET_GLES
    void subImage2DQuery();
    void compressedSubImage2DQuery();
    void subImage2DQueryBuffer();
    void compressedSubImage2DQueryBuffer();
    #endif

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

    #ifndef MAGNUM_TARGET_GLES
    CompressedPixelStorage _compressedDataStorage2D;
    #endif
    std::size_t _compressedDataOffset2D;
};

TextureArrayGLTest::TextureArrayGLTest() {
    addTests({
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::construct1D,
        #endif
        &TextureArrayGLTest::construct2D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::wrap1D,
        #endif
        &TextureArrayGLTest::wrap2D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::bind1D,
        #endif
        &TextureArrayGLTest::bind2D,

        #ifndef MAGNUM_TARGET_WEBGL
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::bindImage1D,
        #endif
        &TextureArrayGLTest::bindImage2D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::sampling1D,
        #endif
        &TextureArrayGLTest::sampling2D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::samplingSRGBDecode1D,
        #endif
        &TextureArrayGLTest::samplingSRGBDecode2D,

        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::samplingSwizzle1D,
        #endif
        &TextureArrayGLTest::samplingSwizzle2D,
        #else
        &TextureArrayGLTest::samplingMaxLevel2D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::samplingBorderInteger1D,
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        &TextureArrayGLTest::samplingBorderInteger2D,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::samplingDepthStencilMode1D,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureArrayGLTest::samplingDepthStencilMode2D,
        #endif
        #ifdef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::samplingBorder2D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::storage1D,
        #endif
        &TextureArrayGLTest::storage2D,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::image1D,
        &TextureArrayGLTest::compressedImage1D,
        &TextureArrayGLTest::image1DBuffer,
        &TextureArrayGLTest::compressedImage1DBuffer,
        #endif
        &TextureArrayGLTest::image2D,
        &TextureArrayGLTest::compressedImage2D,
        &TextureArrayGLTest::image2DBuffer,
        &TextureArrayGLTest::compressedImage2DBuffer,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::subImage1D,
        &TextureArrayGLTest::compressedSubImage1D,
        &TextureArrayGLTest::subImage1DBuffer,
        &TextureArrayGLTest::compressedSubImage1DBuffer,
        &TextureArrayGLTest::subImage1DQuery,
        &TextureArrayGLTest::compressedSubImage1DQuery,
        &TextureArrayGLTest::subImage1DQueryBuffer,
        &TextureArrayGLTest::compressedSubImage1DQueryBuffer,
        #endif
        &TextureArrayGLTest::subImage2D,
        &TextureArrayGLTest::compressedSubImage2D,
        &TextureArrayGLTest::subImage2DBuffer,
        &TextureArrayGLTest::compressedSubImage2DBuffer,
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::subImage2DQuery,
        &TextureArrayGLTest::compressedSubImage2DQuery,
        &TextureArrayGLTest::subImage2DQueryBuffer,
        &TextureArrayGLTest::compressedSubImage2DQueryBuffer,
        #endif

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

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>())
    {
        _compressedDataStorage2D = CompressedPixelStorage{}
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16)
            .setSkip({0, 0, 1});
        _compressedDataOffset2D = 16;
    } else
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D = {};
        #endif
        _compressedDataOffset2D = 0;
    }
}

namespace {
    template<std::size_t size, class T> Containers::ArrayView<const T> unsafeSuffix(const T(&data)[size], std::size_t offset) {
        static_assert(sizeof(T) == 1, "");
        return {data - offset, size + offset};
    }
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::construct1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
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
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
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
void TextureArrayGLTest::wrap1D() {
    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = Texture1DArray::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    Texture1DArray::wrap(id);
    glDeleteTextures(1, &id);
}
#endif

void TextureArrayGLTest::wrap2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = Texture2DArray::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    Texture2DArray::wrap(id);
    glDeleteTextures(1, &id);
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::bind1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
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

void TextureArrayGLTest::bind2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
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
void TextureArrayGLTest::bindImage1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::GL::ARB::shader_image_load_store::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {32, 4})
        .bindImage(2, 0, 1, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    texture.bindImageLayered(3, 0, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    AbstractTexture::unbindImage(2);
    AbstractTexture::unbindImage(3);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::bindImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::GL::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Texture2DArray texture;
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
void TextureArrayGLTest::sampling1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
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
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
           .setCompareMode(Sampler::CompareMode::CompareRefToTexture)
           .setCompareFunction(Sampler::CompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureArrayGLTest::samplingSRGBDecode1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setSRGBDecode(false);

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureArrayGLTest::samplingSwizzle1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_swizzle::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureArrayGLTest::samplingBorderInteger1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_integer>())
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
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::GL::ARB::stencil_texturing::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setDepthStencilMode(Sampler::DepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::sampling2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
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
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
           #ifndef MAGNUM_TARGET_GLES
           .setCompareMode(Sampler::CompareMode::CompareRefToTexture)
           .setCompareFunction(Sampler::CompareFunction::GreaterOrEqual)
           #endif
           ;

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureArrayGLTest::samplingSRGBDecode2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setSRGBDecode(false);

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES2
void TextureArrayGLTest::samplingSwizzle2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_swizzle::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_ERROR();
}
#else
void TextureArrayGLTest::samplingMaxLevel2D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::GL::APPLE::texture_max_level::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureArrayGLTest::samplingCompare2D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::shadow_samplers>() ||
       !Context::current().isExtensionSupported<Extensions::GL::NV::shadow_samplers_array>())
        CORRADE_SKIP(Extensions::GL::NV::shadow_samplers_array::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setCompareMode(Sampler::CompareMode::CompareRefToTexture)
           .setCompareFunction(Sampler::CompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureArrayGLTest::samplingBorderInteger2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_border_clamp::string() + std::string(" is not supported."));
    #endif

    Texture2DArray a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture2DArray b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureArrayGLTest::samplingDepthStencilMode2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::GL::ARB::stencil_texturing::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Texture2DArray texture;
    texture.setDepthStencilMode(Sampler::DepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifdef MAGNUM_TARGET_GLES
void TextureArrayGLTest::samplingBorder2D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::NV::texture_border_clamp>() &&
       !Context::current().isExtensionSupported<Extensions::GL::EXT::texture_border_clamp>())
        CORRADE_SKIP("No required extension is supported.");

    Texture2DArray texture;
    texture.setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::storage1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
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
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector3i(32));

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES2
    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported, skipping image size testing");
    #endif

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 0,  0,  0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

namespace {
    constexpr UnsignedByte Data1D[] = { 0x00, 0x01, 0x02, 0x03,
                                        0x04, 0x05, 0x06, 0x07,
                                        0x08, 0x09, 0x0a, 0x0b,
                                        0x0c, 0x0d, 0x0e, 0x0f };

    const auto DataStorage1D = PixelStorage{}.setSkip({0, 1, 0});
    const std::size_t DataOffset1D = 8;
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::image1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView2D{DataStorage1D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data1D, DataOffset1D)});

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image = texture.image(0, {DataStorage1D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(DataOffset1D),
        Containers::arrayView(Data1D), TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedImage1D() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::image1DBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage2D{DataStorage1D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data1D, DataOffset1D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image = texture.image(0,
        {DataStorage1D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(imageData.suffix(DataOffset1D),
        Containers::arrayView(Data1D), TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedImage1DBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}
#endif

namespace {
    constexpr UnsignedByte Data2D[] = { 0x00, 0x01, 0x02, 0x03,
                                        0x04, 0x05, 0x06, 0x07,
                                        0x08, 0x09, 0x0a, 0x0b,
                                        0x0c, 0x0d, 0x0e, 0x0f,
                                        0x10, 0x11, 0x12, 0x13,
                                        0x14, 0x15, 0x16, 0x17,
                                        0x18, 0x19, 0x1a, 0x1b,
                                        0x1c, 0x1d, 0x1e, 0x1f };

    const auto DataStorage2D = PixelStorage{}.setSkip({0, 0, 1});
    const std::size_t DataOffset2D = 16;

    /* Just 4x4x2 0x00 - 0x7f compressed using RGBA DXT3 by the driver */
    constexpr UnsignedByte CompressedData2D[] = {
          0,  17,  17,  34,  34,  51,  51,  67,
        232,  57,   0,   0, 213, 255, 170,   2,
         68,  84,  85, 101, 102, 118, 119, 119,
        239, 123,   8,  66, 213, 255, 170,   2
    };
}

void TextureArrayGLTest::image2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView3D{DataStorage2D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2), unsafeSuffix(Data2D, DataOffset2D)});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {DataStorage2D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(DataOffset2D),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
    #endif
}

void TextureArrayGLTest::compressedImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::GL::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2}, unsafeSuffix(CompressedData2D, _compressedDataOffset2D)});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage3D image = texture.compressedImage(0, {_compressedDataStorage2D});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_compressedDataOffset2D),
        Containers::arrayView(CompressedData2D),
        TestSuite::Compare::Container);
    #endif
}

void TextureArrayGLTest::image2DBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage3D{DataStorage2D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2), unsafeSuffix(Data2D, DataOffset2D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0, {DataStorage2D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(imageData.suffix(DataOffset2D),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
    #endif
}

void TextureArrayGLTest::compressedImage2DBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::GL::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2}, unsafeSuffix(CompressedData2D, _compressedDataOffset2D)});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage3D image = texture.compressedImage(0, {_compressedDataStorage2D}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
    CORRADE_COMPARE_AS(imageData.suffix(_compressedDataOffset2D),
        Containers::arrayView(CompressedData2D),
        TestSuite::Compare::Container);
    #endif
}

namespace {
    constexpr UnsignedByte Zero1D[4*4*4] = {};
    constexpr UnsignedByte SubData1DComplete[] = {
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    };
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::subImage1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero1D));
    texture.setSubImage(0, Vector2i(1), ImageView2D{DataStorage1D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data1D, DataOffset1D)});

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubData1DComplete), TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedSubImage1D() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::subImage1DBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero1D));
    texture.setSubImage(0, Vector2i(1), BufferImage2D{DataStorage1D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), unsafeSuffix(Data1D, DataOffset1D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(SubData1DComplete),
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedSubImage1DBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::subImage1DQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubData1DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image = texture.subImage(0, Range2Di::fromSize(Vector2i{1}, Vector2i{2}),
        {DataStorage1D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(DataOffset1D),
        Containers::arrayView(Data1D), TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedSubImage1DQuery() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::subImage1DQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubData1DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image = texture.subImage(0, Range2Di::fromSize(Vector2i{1}, Vector2i{2}),
        {DataStorage1D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(imageData.suffix(DataOffset1D),
        Containers::arrayView(Data1D), TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedSubImage1DQueryBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}
#endif

namespace {
    constexpr UnsignedByte Zero2D[4*4*4*4] = {};

    /* Just 12x4x4 zeros compressed using RGBA DXT3 by the driver */
    constexpr UnsignedByte CompressedZero2D[3*4*16] = {};

    constexpr UnsignedByte SubData2DComplete[] = {
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

    /* Combination of CompressedZero2D and CompressedData2D */
    constexpr UnsignedByte CompressedSubData2DComplete[] = {
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
         68,  84,  85, 101, 102, 118, 119, 119,
        239, 123,   8,  66, 213, 255, 170,   2,
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

void TextureArrayGLTest::subImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(4), Zero2D));
    texture.setSubImage(0, Vector3i(1), ImageView3D{DataStorage2D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2), unsafeSuffix(Data2D, DataOffset2D)});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}

void TextureArrayGLTest::compressedSubImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif
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
        Texture2DArray texture;
        texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
            Vector3i{12, 4, 4}, CompressedZero2D});
        texture.setCompressedSubImage(0, {4, 0, 1}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2}, CompressedData2D});

        MAGNUM_VERIFY_NO_ERROR();

        CompressedImage3D image = texture.compressedImage(0, {});

        MAGNUM_VERIFY_NO_ERROR();

        CORRADE_COMPARE(image.size(), (Vector3i{12, 4, 4}));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedSubData2DComplete),
            TestSuite::Compare::Container);
    }
    #endif

    Texture2DArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
        Vector3i{12, 4, 4}, CompressedZero2D});
    texture.setCompressedSubImage(0, {4, 0, 1}, CompressedImageView3D{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2}, unsafeSuffix(CompressedData2D, _compressedDataOffset2D)});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage3D image = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 4, 4}));

    {
        CORRADE_EXPECT_FAIL_IF(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>() && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Non-default compressed pixel storage for array textures behaves weirdly on NVidia");

        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedSubData2DComplete),
            TestSuite::Compare::Container);
    }
    #endif
}

void TextureArrayGLTest::subImage2DBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(4), Zero2D));
    texture.setSubImage(0, Vector3i(1), BufferImage3D{DataStorage2D,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2), unsafeSuffix(Data2D, DataOffset2D), BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(SubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}

void TextureArrayGLTest::compressedSubImage2DBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif
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
        Texture2DArray texture;
        texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
            Vector3i{12, 4, 4}, CompressedZero2D});
        texture.setCompressedSubImage(0, {4, 0, 1}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2}, CompressedData2D});

        MAGNUM_VERIFY_NO_ERROR();

        CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
        const auto imageData = image.buffer().data<UnsignedByte>();

        MAGNUM_VERIFY_NO_ERROR();

        CORRADE_COMPARE(image.size(), (Vector3i{12, 4, 4}));
        CORRADE_COMPARE_AS(imageData, Containers::arrayView(CompressedSubData2DComplete),
            TestSuite::Compare::Container);
    }
    #endif

    Texture2DArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
        Vector3i{12, 4, 4}, CompressedZero2D});
    texture.setCompressedSubImage(0, {4, 0, 1}, CompressedImageView3D{
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2}, unsafeSuffix(CompressedData2D, _compressedDataOffset2D)});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 4, 4}));

    {
        CORRADE_EXPECT_FAIL_IF(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>() && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Non-default compressed pixel storage for array textures behaves weirdly on NVidia");

        CORRADE_COMPARE_AS(imageData, Containers::arrayView(CompressedSubData2DComplete),
            TestSuite::Compare::Container);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::subImage2DQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector3i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{4}, SubData2DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    Image3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, Vector3i{2}),
        {DataStorage2D, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(DataOffset2D),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedSubImage2DQuery() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 4, 4})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
               {12, 4, 4}, CompressedSubData2DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    /* Test also without compressed pixel storage to ensure that both size
       computations work */
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::compressed_texture_pixel_storage>()) {
        CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 1}, {4, 4, 2}), {});

        MAGNUM_VERIFY_NO_ERROR();

        CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedData2D),
            TestSuite::Compare::Container);
    }

    CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 1}, {4, 4, 2}), {_compressedDataStorage2D});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(_compressedDataOffset2D),
        Containers::arrayView(CompressedData2D),
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::subImage2DQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector3i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{4}, SubData2DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, Vector3i{2}),
        {DataStorage2D, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{2});
    CORRADE_COMPARE_AS(imageData.suffix(DataOffset2D),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedSubImage2DQueryBuffer() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::GL::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 4, 4})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
               {12, 4, 4}, CompressedSubData2DComplete});

    MAGNUM_VERIFY_NO_ERROR();

    CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 1}, {4, 4, 2}), {
        #ifndef MAGNUM_TARGET_GLES
        _compressedDataStorage2D
        #endif
    }, BufferUsage::StaticRead);

    MAGNUM_VERIFY_NO_ERROR();
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
    CORRADE_COMPARE_AS(imageData.suffix(_compressedDataOffset2D),
        Containers::arrayView(CompressedData2D),
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::generateMipmap1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));

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
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(32)));

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
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::invalidateImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::invalidateSubImage1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateSubImage(1, Vector2i(2), Vector2i(8));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureArrayGLTest::invalidateSubImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::TextureArrayGLTest)
