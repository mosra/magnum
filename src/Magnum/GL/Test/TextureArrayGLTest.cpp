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
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/BufferImage.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/TextureArray.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"

#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/ImageFormat.h"
#endif

namespace Magnum { namespace GL { namespace Test { namespace {

struct TextureArrayGLTest: OpenGLTester {
    explicit TextureArrayGLTest();

    #ifndef MAGNUM_TARGET_GLES
    void construct1D();
    #endif
    void construct2D();

    void constructMove();

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
    template<class T> void sampling1D();
    #endif
    template<class T> void sampling2D();

    #ifndef MAGNUM_TARGET_WEBGL
    #ifndef MAGNUM_TARGET_GLES
    void samplingSrgbDecode1D();
    #endif
    void samplingSrgbDecode2D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void samplingSwizzle1D();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingSwizzle2D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void samplingBorderInteger1D();
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    void samplingBorderInteger2D();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void samplingDepthStencilMode1D();
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    void samplingDepthStencilMode2D();
    #endif
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingBorder2D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void storage1D();
    #endif
    void storage2D();

    #ifndef MAGNUM_TARGET_GLES
    void image1D();
    void image1DBuffer();
    void image1DQueryView();
    void subImage1D();
    void subImage1DBuffer();
    void subImage1DQuery();
    void subImage1DQueryView();
    void subImage1DQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */

    void compressedImage1D();
    void compressedImage1DBuffer();
    void compressedImage1DQueryView();
    void compressedSubImage1D();
    void compressedSubImage1DBuffer();
    void compressedSubImage1DQuery();
    void compressedSubImage1DQueryView();
    void compressedSubImage1DQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */
    #endif

    void image2D();
    void image2DBuffer();
    #ifndef MAGNUM_TARGET_GLES
    void image2DQueryView();
    #endif
    void subImage2D();
    void subImage2DBuffer();
    #ifndef MAGNUM_TARGET_GLES
    void subImage2DQuery();
    void subImage2DQueryView();
    void subImage2DQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */
    #endif

    void compressedImage2D();
    void compressedImage2DBuffer();
    #ifndef MAGNUM_TARGET_GLES
    void compressedImage2DQueryView();
    #endif
    void compressedSubImage2D();
    void compressedSubImage2DBuffer();
    #ifndef MAGNUM_TARGET_GLES
    void compressedSubImage2DQuery();
    void compressedSubImage2DQueryView();
    void compressedSubImage2DQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */
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
};

struct GenericSampler {
    typedef Magnum::SamplerFilter Filter;
    typedef Magnum::SamplerMipmap Mipmap;
    typedef Magnum::SamplerWrapping Wrapping;
};
struct GLSampler {
    typedef GL::SamplerFilter Filter;
    typedef GL::SamplerMipmap Mipmap;
    typedef GL::SamplerWrapping Wrapping;
};

#ifndef MAGNUM_TARGET_GLES
constexpr UnsignedByte Data1D[]{
    0, 0, 0, 0, 0, 0, 0, 0,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    PixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} PixelStorage1DData[]{
    {"default pixel storage",
        Containers::arrayView(Data1D).suffix(8), {},
        Containers::arrayView(Data1D).suffix(8), 0},
    {"skip Y",
        Containers::arrayView(Data1D).suffix(8), PixelStorage{}.setSkip({0, 1, 0}),
        Containers::arrayView(Data1D), 8}};
#endif

constexpr UnsignedByte Data2D[]{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    PixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} PixelStorage2DData[]{
    {"default pixel storage",
        Containers::arrayView(Data2D).suffix(16), {},
        Containers::arrayView(Data2D).suffix(16), 0},
    {"skip Z",
        Containers::arrayView(Data2D).suffix(16), PixelStorage{}.setSkip({0, 0, 1}),
        Containers::arrayView(Data2D), 16}};

/* Just 4x4x3 0x00 - 0x7f compressed using RGBA DXT3 by the driver */
constexpr UnsignedByte CompressedData2D[]{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
      0,  17,  17,  34,  34,  51,  51,  67,
    232,  57,   0,   0, 213, 255, 170,   2,
     68,  84,  85, 101, 102, 118, 119, 119,
    239, 123,   8,  66, 213, 255, 170,   2
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    #ifndef MAGNUM_TARGET_GLES
    CompressedPixelStorage storage;
    #endif
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} CompressedPixelStorage2DData[]{
    {"default pixel storage",
        Containers::arrayView(CompressedData2D).suffix(16),
        #ifndef MAGNUM_TARGET_GLES
        {},
        #endif
        Containers::arrayView(CompressedData2D).suffix(16), 0},
    #ifndef MAGNUM_TARGET_GLES
    {"skip Y",
        Containers::arrayView(CompressedData2D).suffix(16),
        CompressedPixelStorage{}
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16)
            .setSkip({0, 0, 1}),
        Containers::arrayView(CompressedData2D), 16}
    #endif
};

TextureArrayGLTest::TextureArrayGLTest() {
    addTests({
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::construct1D,
        #endif
        &TextureArrayGLTest::construct2D,

        &TextureArrayGLTest::constructMove,

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
        &TextureArrayGLTest::sampling1D<GenericSampler>,
        &TextureArrayGLTest::sampling1D<GLSampler>,
        #endif
        &TextureArrayGLTest::sampling2D<GenericSampler>,
        &TextureArrayGLTest::sampling2D<GLSampler>,

        #ifndef MAGNUM_TARGET_WEBGL
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::samplingSrgbDecode1D,
        #endif
        &TextureArrayGLTest::samplingSrgbDecode2D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::samplingSwizzle1D,
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        &TextureArrayGLTest::samplingSwizzle2D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::samplingBorderInteger1D,
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        &TextureArrayGLTest::samplingBorderInteger2D,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::samplingDepthStencilMode1D,
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        &TextureArrayGLTest::samplingDepthStencilMode2D,
        #endif
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        &TextureArrayGLTest::samplingBorder2D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::storage1D,
        #endif
        &TextureArrayGLTest::storage2D});

    #ifndef MAGNUM_TARGET_GLES
    addInstancedTests({
        &TextureArrayGLTest::image1D,
        &TextureArrayGLTest::image1DBuffer,
        &TextureArrayGLTest::image1DQueryView,
        &TextureArrayGLTest::subImage1D,
        &TextureArrayGLTest::subImage1DBuffer,
        &TextureArrayGLTest::subImage1DQuery,
        &TextureArrayGLTest::subImage1DQueryView,
        &TextureArrayGLTest::subImage1DQueryBuffer},
        Containers::arraySize(PixelStorage1DData));

    addTests({&TextureArrayGLTest::compressedImage1D,
              &TextureArrayGLTest::compressedImage1DBuffer,
              &TextureArrayGLTest::compressedImage1DQueryView,
              &TextureArrayGLTest::compressedSubImage1D,
              &TextureArrayGLTest::compressedSubImage1DBuffer,
              &TextureArrayGLTest::compressedSubImage1DQuery,
              &TextureArrayGLTest::compressedSubImage1DQueryView,
              &TextureArrayGLTest::compressedSubImage1DQueryBuffer});
    #endif

    addInstancedTests({
        &TextureArrayGLTest::image2D,
        &TextureArrayGLTest::image2DBuffer,
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::image2DQueryView,
        #endif
        &TextureArrayGLTest::subImage2D,
        &TextureArrayGLTest::subImage2DBuffer,
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::subImage2DQuery,
        &TextureArrayGLTest::subImage2DQueryView,
        &TextureArrayGLTest::subImage2DQueryBuffer
        #endif
        }, Containers::arraySize(PixelStorage2DData));

    addInstancedTests({
        &TextureArrayGLTest::compressedImage2D,
        &TextureArrayGLTest::compressedImage2DBuffer,
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::compressedImage2DQueryView,
        #endif
        &TextureArrayGLTest::compressedSubImage2D,
        &TextureArrayGLTest::compressedSubImage2DBuffer,
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayGLTest::compressedSubImage2DQuery,
        &TextureArrayGLTest::compressedSubImage2DQueryView,
        &TextureArrayGLTest::compressedSubImage2DQueryBuffer
        #endif
        }, Containers::arraySize(CompressedPixelStorage2DData));

    addTests({
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
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    {
        Texture1DArray texture;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureArrayGLTest::construct2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    {
        Texture2DArray texture;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureArrayGLTest::constructMove() {
    /* Move constructor tested in AbstractTexture, here we just verify there
       are no extra members that would need to be taken care of */
    CORRADE_COMPARE(sizeof(Texture2DArray), sizeof(AbstractTexture));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Texture2DArray>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Texture2DArray>::value);
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
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
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
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureArrayGLTest::bind2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_WEBGL
#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::bindImage1D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::ARB::shader_image_load_store::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {32, 4})
        .bindImage(2, 0, 1, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_GL_ERROR();

    texture.bindImageLayered(3, 0, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    AbstractTexture::unbindImage(2);
    AbstractTexture::unbindImage(3);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureArrayGLTest::bindImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {32, 32, 4})
        .bindImage(2, 0, 1, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_GL_ERROR();

    texture.bindImageLayered(3, 0, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    AbstractTexture::unbindImage(2);
    AbstractTexture::unbindImage(3);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
template<class T> void TextureArrayGLTest::sampling1D() {
    setTestCaseTemplateName(std::is_same<T, GenericSampler>::value ?
        "GenericSampler" : "GLSampler");

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setMinificationFilter(T::Filter::Linear, T::Mipmap::Linear)
           .setMagnificationFilter(T::Filter::Linear)
           .setMinLod(-750.0f)
           .setMaxLod(750.0f)
           .setLodBias(0.5f)
           .setBaseLevel(1)
           .setMaxLevel(750)
           .setWrapping(T::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
           .setCompareMode(SamplerCompareMode::CompareRefToTexture)
           .setCompareFunction(SamplerCompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureArrayGLTest::samplingSrgbDecode1D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setSrgbDecode(false);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureArrayGLTest::samplingSwizzle1D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::ARB::texture_swizzle::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureArrayGLTest::samplingBorderInteger1D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::EXT::texture_integer::string() + std::string(" is not supported."));

    Texture1DArray a;
    a.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture1DArray b;
    b.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureArrayGLTest::samplingDepthStencilMode1D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::ARB::stencil_texturing::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setDepthStencilMode(SamplerDepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

template<class T> void TextureArrayGLTest::sampling2D() {
    setTestCaseTemplateName(std::is_same<T, GenericSampler>::value ?
        "GenericSampler" : "GLSampler");

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setMinificationFilter(T::Filter::Linear, T::Mipmap::Linear)
           .setMagnificationFilter(T::Filter::Linear)
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
           .setWrapping(T::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           #else
           .setWrapping(T::Wrapping::ClampToEdge)
           #endif
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
           #ifndef MAGNUM_TARGET_GLES
           .setCompareMode(SamplerCompareMode::CompareRefToTexture)
           .setCompareFunction(SamplerCompareFunction::GreaterOrEqual)
           #endif
           ;

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_WEBGL
void TextureArrayGLTest::samplingSrgbDecode2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setSrgbDecode(false);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureArrayGLTest::samplingSwizzle2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::ARB::texture_swizzle::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureArrayGLTest::samplingMaxLevel2D() {
    if(!Context::current().isExtensionSupported<Extensions::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::APPLE::texture_max_level::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureArrayGLTest::samplingCompare2D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::shadow_samplers>() ||
       !Context::current().isExtensionSupported<Extensions::NV::shadow_samplers_array>())
        CORRADE_SKIP(Extensions::NV::shadow_samplers_array::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setCompareMode(SamplerCompareMode::CompareRefToTexture)
           .setCompareFunction(SamplerCompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureArrayGLTest::samplingBorderInteger2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::EXT::texture_integer::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::EXT::texture_border_clamp::string() + std::string(" is not supported."));
    #endif

    Texture2DArray a;
    a.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture2DArray b;
    b.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_WEBGL
void TextureArrayGLTest::samplingDepthStencilMode2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::ARB::stencil_texturing::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Texture2DArray texture;
    texture.setDepthStencilMode(SamplerDepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
void TextureArrayGLTest::samplingBorder2D() {
    if(!Context::current().isExtensionSupported<Extensions::NV::texture_border_clamp>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP("No required extension is supported.");

    Texture2DArray texture;
    texture.setWrapping(SamplerWrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::storage1D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector2i(32));

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 0,  0)); /* not available */

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureArrayGLTest::storage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector3i(32));

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
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

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::image1D() {
    setTestCaseDescription(PixelStorage1DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView2D{
        PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), PixelStorage1DData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image2D image = texture.image(0, {PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorage1DData[testCaseInstanceId()].offset),
        PixelStorage1DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::image1DBuffer() {
    setTestCaseDescription(PixelStorage1DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage2D{
        PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), PixelStorage1DData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage2D image = texture.image(0,
        {PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(PixelStorage1DData[testCaseInstanceId()].offset),
        PixelStorage1DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::image1DQueryView() {
    setTestCaseDescription(PixelStorage1DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView2D{
        PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), PixelStorage1DData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{PixelStorage1DData[testCaseInstanceId()].offset + 2*2*4};
    MutableImageView2D image{PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, data};
    texture.image(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorage1DData[testCaseInstanceId()].offset),
        PixelStorage1DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

constexpr UnsignedByte Zero1D[4*4*4] = {};
constexpr UnsignedByte SubData1DComplete[] = {
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
    0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
};

void TextureArrayGLTest::subImage1D() {
    setTestCaseDescription(PixelStorage1DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero1D));
    texture.setSubImage(0, Vector2i(1), ImageView2D{
        PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), PixelStorage1DData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubData1DComplete),
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::subImage1DBuffer() {
    setTestCaseDescription(PixelStorage1DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero1D));
    texture.setSubImage(0, Vector2i(1), BufferImage2D{
        PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorage1DData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData),
        Containers::arrayView(SubData1DComplete),
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::subImage1DQuery() {
    setTestCaseDescription(PixelStorage1DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubData1DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image2D image = texture.subImage(0, Range2Di::fromSize(Vector2i{1}, Vector2i{2}),
        {PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorage1DData[testCaseInstanceId()].offset),
        PixelStorage1DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::subImage1DQueryView() {
    setTestCaseDescription(PixelStorage1DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubData1DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{PixelStorage1DData[testCaseInstanceId()].offset + 2*2*4};
    MutableImageView2D image{PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, data};
    texture.subImage(0, Range2Di::fromSize(Vector2i{1}, Vector2i{2}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorage1DData[testCaseInstanceId()].offset),
        PixelStorage1DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::subImage1DQueryBuffer() {
    setTestCaseDescription(PixelStorage1DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubData1DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage2D image = texture.subImage(0, Range2Di::fromSize(Vector2i{1}, Vector2i{2}),
        {PixelStorage1DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{2});

    /* Was broken on NV since 370.xx (May 2017), fixed in 390.25 (Mar 2018) */
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(PixelStorage1DData[testCaseInstanceId()].offset),
        PixelStorage1DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedImage1D() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::compressedImage1DBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::compressedImage1DQueryView() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::compressedSubImage1D() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::compressedSubImage1DBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::compressedSubImage1DQuery() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::compressedSubImage1DQueryView() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureArrayGLTest::compressedSubImage1DQueryBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}
#endif

void TextureArrayGLTest::image2D() {
    setTestCaseDescription(PixelStorage2DData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView3D{
        PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2),
        PixelStorage2DData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorage2DData[testCaseInstanceId()].offset),
        PixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}

void TextureArrayGLTest::image2DBuffer() {
    setTestCaseDescription(PixelStorage2DData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage3D{
        PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2),
        PixelStorage2DData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0, {PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(PixelStorage2DData[testCaseInstanceId()].offset),
        PixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::image2DQueryView() {
    setTestCaseDescription(PixelStorage2DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView3D{
        PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2),
        PixelStorage2DData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{PixelStorage2DData[testCaseInstanceId()].offset + 2*2*2*4};
    MutableImageView3D image{PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{2}, data};
    texture.image(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorage2DData[testCaseInstanceId()].offset),
        PixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}
#endif

constexpr UnsignedByte Zero2D[4*4*4*4]{};

#ifndef MAGNUM_TARGET_GLES
constexpr UnsignedByte SubData2DComplete[]{
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
#endif

void TextureArrayGLTest::subImage2D() {
    setTestCaseDescription(PixelStorage2DData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(4), Zero2D));
    texture.setSubImage(0, Vector3i(1), ImageView3D{
        PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2),
        PixelStorage2DData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}

void TextureArrayGLTest::subImage2DBuffer() {
    setTestCaseDescription(PixelStorage2DData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(4), Zero2D));
    texture.setSubImage(0, Vector3i(1), BufferImage3D{
        PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2),
        PixelStorage2DData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData),
        Containers::arrayView(SubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::subImage2DQuery() {
    setTestCaseDescription(PixelStorage2DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector3i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{4}, SubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, Vector3i{2}),
        {PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorage2DData[testCaseInstanceId()].offset),
        PixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::subImage2DQueryView() {
    setTestCaseDescription(PixelStorage2DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector3i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{4}, SubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{PixelStorage2DData[testCaseInstanceId()].offset + 2*2*2*4};
    MutableImageView3D image{PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{2}, data};
    texture.subImage(0, Range3Di::fromSize(Vector3i{1}, Vector3i{2}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorage2DData[testCaseInstanceId()].offset),
        PixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::subImage2DQueryBuffer() {
    setTestCaseDescription(PixelStorage2DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector3i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{4}, SubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, Vector3i{2}),
        {PixelStorage2DData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(PixelStorage2DData[testCaseInstanceId()].offset),
        PixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}
#endif

void TextureArrayGLTest::compressedImage2D() {
    setTestCaseDescription(CompressedPixelStorage2DData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() + std::string(" is not supported."));
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage2DData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2},
        CompressedPixelStorage2DData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage3D image = texture.compressedImage(0, {CompressedPixelStorage2DData[testCaseInstanceId()].storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorage2DData[testCaseInstanceId()].offset),
        CompressedPixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}

void TextureArrayGLTest::compressedImage2DBuffer() {
    setTestCaseDescription(CompressedPixelStorage2DData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() + std::string(" is not supported."));
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setCompressedImage(0, CompressedBufferImage3D{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage2DData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2},
        CompressedPixelStorage2DData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage3D image = texture.compressedImage(0, {CompressedPixelStorage2DData[testCaseInstanceId()].storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(CompressedPixelStorage2DData[testCaseInstanceId()].offset),
        CompressedPixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::compressedImage2DQueryView() {
    setTestCaseDescription(CompressedPixelStorage2DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{
        CompressedPixelStorage2DData[testCaseInstanceId()].storage,
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2},
        CompressedPixelStorage2DData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{CompressedPixelStorage2DData[testCaseInstanceId()].offset + 2*16};
    MutableCompressedImageView3D image{CompressedPixelStorage2DData[testCaseInstanceId()].storage, CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2}, data};
    texture.compressedImage(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorage2DData[testCaseInstanceId()].offset),
        CompressedPixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}
#endif

/* Just 12x4x4 zeros compressed using RGBA DXT3 by the driver */
constexpr UnsignedByte CompressedZero2D[3*4*16]{};

#ifndef MAGNUM_TARGET_GLES
/* Combination of CompressedZero2D and CompressedData2D */
constexpr UnsignedByte CompressedSubData2DComplete[]{
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
#endif

void TextureArrayGLTest::compressedSubImage2D() {
    setTestCaseDescription(CompressedPixelStorage2DData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() + std::string(" is not supported."));
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
        Vector3i{12, 4, 4}, CompressedZero2D});
    texture.setCompressedSubImage(0, {4, 0, 1}, CompressedImageView3D{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage2DData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2},
        CompressedPixelStorage2DData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage3D image = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 4, 4}));

    {
        CORRADE_EXPECT_FAIL_IF(CompressedPixelStorage2DData[testCaseInstanceId()].storage != CompressedPixelStorage{} && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Non-default compressed pixel storage for array textures behaves weirdly on NVidia");

        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedSubData2DComplete),
            TestSuite::Compare::Container);
    }
    #endif
}

void TextureArrayGLTest::compressedSubImage2DBuffer() {
    setTestCaseDescription(CompressedPixelStorage2DData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() + std::string(" is not supported."));
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
        Vector3i{12, 4, 4}, CompressedZero2D});
    texture.setCompressedSubImage(0, {4, 0, 1}, CompressedBufferImage3D{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage2DData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2},
        CompressedPixelStorage2DData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 4, 4}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData),
        Containers::arrayView(CompressedSubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::compressedSubImage2DQuery() {
    setTestCaseDescription(CompressedPixelStorage2DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage == CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 4, 4})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
               {12, 4, 4}, CompressedSubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 1}, {4, 4, 2}), {CompressedPixelStorage2DData[testCaseInstanceId()].storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorage2DData[testCaseInstanceId()].offset),
        CompressedPixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedSubImage2DQueryView() {
    setTestCaseDescription(CompressedPixelStorage2DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage == CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 4, 4})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
               {12, 4, 4}, CompressedSubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{CompressedPixelStorage2DData[testCaseInstanceId()].offset + 2*16};
    MutableCompressedImageView3D image{CompressedPixelStorage2DData[testCaseInstanceId()].storage, CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 2}, data};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 1}, {4, 4, 2}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorage2DData[testCaseInstanceId()].offset),
        CompressedPixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::compressedSubImage2DQueryBuffer() {
    setTestCaseDescription(CompressedPixelStorage2DData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    if(CompressedPixelStorage2DData[testCaseInstanceId()].storage == CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() + std::string(" is not supported."));

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 4, 4})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
               {12, 4, 4}, CompressedSubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 1}, {4, 4, 2}), {
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage2DData[testCaseInstanceId()].storage
        #endif
    }, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 2}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(CompressedPixelStorage2DData[testCaseInstanceId()].offset),
        CompressedPixelStorage2DData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void TextureArrayGLTest::generateMipmap1D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i( 0));

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 1, 32));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureArrayGLTest::generateMipmap2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
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

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 1,  1, 32));

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::invalidateImage1D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureArrayGLTest::invalidateImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayGLTest::invalidateSubImage1D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));

    Texture1DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateSubImage(1, Vector2i(2), Vector2i(8));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureArrayGLTest::invalidateSubImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        CORRADE_SKIP(Extensions::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture2DArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::TextureArrayGLTest)
