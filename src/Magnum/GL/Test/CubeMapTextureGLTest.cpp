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

#include <sstream>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/BufferImage.h"
#endif
#include "Magnum/GL/CubeMapTexture.h"
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/ImageFormat.h"
#endif
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct CubeMapTextureGLTest: OpenGLTester {
    explicit CubeMapTextureGLTest();

    void construct();
    void constructMove();
    void wrap();

    void bind();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void bindImage();
    #endif

    template<class T> void sampling();
    #ifndef MAGNUM_TARGET_WEBGL
    void samplingSrgbDecode();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingSwizzle();
    #elif !defined(MAGNUM_TARGET_WEBGL)
    void samplingMaxLevel();
    void samplingCompare();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingBorderInteger();
    void samplingDepthStencilMode();
    #endif
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingBorder();
    #endif

    void storageImageSize();

    void storage();
    void image();
    #ifndef MAGNUM_TARGET_GLES2
    void imageBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void imageQueryView();
    void imageQueryViewNullptr();
    void imageQueryViewBadSize();
    #endif
    void subImage();
    #ifndef MAGNUM_TARGET_GLES2
    void subImageBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void subImageQuery();
    void subImageQueryView();
    /* unlike all others, subImage() simply calls into AbstractTexture, so
       all assertions are already tested in AbstractTextureGLTest */
    void subImageQueryBuffer();
    #endif

    void compressedImage();
    #ifndef MAGNUM_TARGET_GLES2
    void compressedImageBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void compressedImageQueryView();
    void compressedImageQueryViewNullptr();
    void compressedImageQueryViewBadSize();
    void compressedImageQueryViewBadDataSize();
    void compressedImageQueryViewBadFormat();
    #endif
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void immutableCompressedImage();
    #endif
    void compressedSubImage();
    #ifndef MAGNUM_TARGET_GLES2
    void compressedSubImageBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void compressedSubImageQuery();
    void compressedSubImageQueryView();
    void compressedSubImageQueryViewNullptr();
    void compressedSubImageQueryViewBadSize();
    void compressedSubImageQueryViewBadDataSize();
    void compressedSubImageQueryViewBadFormat();
    void compressedSubImageQueryBuffer();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void fullImageQuery();
    void fullImageQueryView();
    void fullImageQueryViewNullptr();
    void fullImageQueryViewBadSize();
    void fullImageQueryBuffer();

    void compressedFullImageQuery();
    void compressedFullImageQueryView();
    void compressedFullImageQueryViewNullptr();
    void compressedFullImageQueryViewBadSize();
    void compressedFullImageQueryViewBadDataSize();
    void compressedFullImageQueryViewBadFormat();
    void compressedFullImageQueryBuffer();
    #endif

    void generateMipmap();

    void invalidateImage();
    void invalidateSubImage();
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

constexpr UnsignedByte Data[]{
    0, 0, 0, 0, 0, 0, 0, 0,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    PixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} PixelStorageData[]{
    {"default pixel storage",
        Containers::arrayView(Data).suffix(8), {},
        Containers::arrayView(Data).suffix(8), 0},
    #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
    {"skip Y",
        Containers::arrayView(Data).suffix(8), PixelStorage{}.setSkip({0, 1, 0}),
        Containers::arrayView(Data), 8}
    #endif
};

/* Just 4x4 0x00 - 0x3f compressed using RGBA DXT3 by the driver */
constexpr UnsignedByte CompressedData[]{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0,  17, 17,  34,  34,  51,  51,  67,
    232,  57,  0,   0, 213, 255, 170,   2
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    #ifndef MAGNUM_TARGET_GLES
    CompressedPixelStorage storage;
    #endif
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} CompressedPixelStorageData[]{
    {"default pixel storage",
        Containers::arrayView(CompressedData).suffix(16),
        #ifndef MAGNUM_TARGET_GLES
        {},
        #endif
        Containers::arrayView(CompressedData).suffix(16), 0},
    #ifndef MAGNUM_TARGET_GLES
    {"skip Y",
        Containers::arrayView(CompressedData).suffix(16),
        CompressedPixelStorage{}
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16)
            .setSkip({0, 4, 0}),
        Containers::arrayView(CompressedData), 16}
    #endif
};

#ifndef MAGNUM_TARGET_GLES
constexpr UnsignedByte FullData[]{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,

    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,

    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,

    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,

    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    PixelStorage storage;
    std::size_t offset;
} FullPixelStorageData[]{
    {"default pixel storage",
        Containers::arrayView(FullData).suffix(16), {}, 0},
    #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
    {"skip Z",
        Containers::arrayView(FullData).suffix(16), PixelStorage{}.setSkip({0, 0, 1}), 16}
    #endif
};

/* Just 4x4 0x00 - 0x3f compressed using RGBA DXT3 by the driver, repeated
   six times */
constexpr UnsignedByte CompressedFullData[]{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

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

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    #ifndef MAGNUM_TARGET_GLES
    CompressedPixelStorage storage;
    #endif
    std::size_t offset;
} CompressedFullPixelStorageData[]{
    {"default pixel storage",
        Containers::arrayView(CompressedFullData).suffix(16*4),
        #ifndef MAGNUM_TARGET_GLES
        {},
        #endif
        0},
    #ifndef MAGNUM_TARGET_GLES
    {"skip Z",
        Containers::arrayView(CompressedFullData).suffix(16*4),
        CompressedPixelStorage{}
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16)
            .setSkip({0, 0, 4}), 16*4}
    #endif
};
#endif

CubeMapTextureGLTest::CubeMapTextureGLTest() {
    addTests({&CubeMapTextureGLTest::construct,
              &CubeMapTextureGLTest::constructMove,
              &CubeMapTextureGLTest::wrap,

              &CubeMapTextureGLTest::bind,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &CubeMapTextureGLTest::bindImage,
              #endif

              &CubeMapTextureGLTest::sampling<GenericSampler>,
              &CubeMapTextureGLTest::sampling<GLSampler>,
              #ifndef MAGNUM_TARGET_WEBGL
              &CubeMapTextureGLTest::samplingSrgbDecode,
              #endif
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &CubeMapTextureGLTest::samplingSwizzle,
              #elif !defined(MAGNUM_TARGET_WEBGL)
              &CubeMapTextureGLTest::samplingMaxLevel,
              &CubeMapTextureGLTest::samplingCompare,
              #endif
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &CubeMapTextureGLTest::samplingBorderInteger,
              &CubeMapTextureGLTest::samplingDepthStencilMode,
              #endif
              #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
              &CubeMapTextureGLTest::samplingBorder,
              #endif

              &CubeMapTextureGLTest::storageImageSize});

    addInstancedTests({
        &CubeMapTextureGLTest::storage,
        &CubeMapTextureGLTest::image,
        #ifndef MAGNUM_TARGET_GLES2
        &CubeMapTextureGLTest::imageBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &CubeMapTextureGLTest::imageQueryView,
        #endif
        }, Containers::arraySize(PixelStorageData));

    #ifndef MAGNUM_TARGET_GLES
    addTests({&CubeMapTextureGLTest::imageQueryViewNullptr,
              &CubeMapTextureGLTest::imageQueryViewBadSize});
    #endif

    #ifndef MAGNUM_TARGET_GLES
    addInstancedTests({
        &CubeMapTextureGLTest::fullImageQuery,
        &CubeMapTextureGLTest::fullImageQueryView,
        &CubeMapTextureGLTest::fullImageQueryBuffer},
        Containers::arraySize(FullPixelStorageData));

    addTests({&CubeMapTextureGLTest::fullImageQueryViewNullptr,
              &CubeMapTextureGLTest::fullImageQueryViewBadSize});
    #endif

    addInstancedTests({
        &CubeMapTextureGLTest::subImage,
        #ifndef MAGNUM_TARGET_GLES2
        &CubeMapTextureGLTest::subImageBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &CubeMapTextureGLTest::subImageQuery,
        &CubeMapTextureGLTest::subImageQueryView,
        &CubeMapTextureGLTest::subImageQueryBuffer
        #endif
        }, Containers::arraySize(PixelStorageData));

    addInstancedTests({
        &CubeMapTextureGLTest::compressedImage,
        #ifndef MAGNUM_TARGET_GLES2
        &CubeMapTextureGLTest::compressedImageBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &CubeMapTextureGLTest::compressedImageQueryView,
        #endif
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        &CubeMapTextureGLTest::immutableCompressedImage,
        #endif
        }, Containers::arraySize(CompressedPixelStorageData));

    #ifndef MAGNUM_TARGET_GLES
    addTests({&CubeMapTextureGLTest::compressedImageQueryViewNullptr,
              &CubeMapTextureGLTest::compressedImageQueryViewBadSize,
              &CubeMapTextureGLTest::compressedImageQueryViewBadDataSize,
              &CubeMapTextureGLTest::compressedImageQueryViewBadFormat});

    addInstancedTests({
        &CubeMapTextureGLTest::compressedFullImageQuery,
        &CubeMapTextureGLTest::compressedFullImageQueryView,
        &CubeMapTextureGLTest::compressedFullImageQueryBuffer},
        Containers::arraySize(CompressedFullPixelStorageData));

    addTests({&CubeMapTextureGLTest::compressedFullImageQueryViewNullptr,
              &CubeMapTextureGLTest::compressedFullImageQueryViewBadSize,
              &CubeMapTextureGLTest::compressedFullImageQueryViewBadDataSize,
              &CubeMapTextureGLTest::compressedFullImageQueryViewBadFormat});
    #endif

    addInstancedTests({
        &CubeMapTextureGLTest::compressedSubImage,
        #ifndef MAGNUM_TARGET_GLES2
        &CubeMapTextureGLTest::compressedSubImageBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &CubeMapTextureGLTest::compressedSubImageQuery,
        &CubeMapTextureGLTest::compressedSubImageQueryView,
        &CubeMapTextureGLTest::compressedSubImageQueryBuffer
        #endif
        }, Containers::arraySize(CompressedPixelStorageData));

    #ifndef MAGNUM_TARGET_GLES
    addTests({&CubeMapTextureGLTest::compressedSubImageQueryViewNullptr,
              &CubeMapTextureGLTest::compressedSubImageQueryViewBadSize,
              &CubeMapTextureGLTest::compressedSubImageQueryViewBadDataSize,
              &CubeMapTextureGLTest::compressedSubImageQueryViewBadFormat});
    #endif

    addTests({&CubeMapTextureGLTest::generateMipmap,

              &CubeMapTextureGLTest::invalidateImage,
              &CubeMapTextureGLTest::invalidateSubImage});
}

template<std::size_t size, class T> Containers::ArrayView<const T> unsafeSuffix(const T(&data)[size], std::size_t offset) {
    static_assert(sizeof(T) == 1, "");
    return {data - offset, size + offset};
}

void CubeMapTextureGLTest::construct() {
    {
        CubeMapTexture texture;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureGLTest::constructMove() {
    /* Move constructor tested in AbstractTexture, here we just verify there
       are no extra members that would need to be taken care of */
    CORRADE_COMPARE(sizeof(CubeMapTexture), sizeof(AbstractTexture));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<CubeMapTexture>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<CubeMapTexture>::value);
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

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTextureGLTest::bindImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{32})
        .bindImage(2, 0, CubeMapCoordinate::NegativeX, ImageAccess::ReadWrite, ImageFormat::RGBA8);

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

template<class T> void CubeMapTextureGLTest::sampling() {
    setTestCaseTemplateName(std::is_same<T, GenericSampler>::value ?
        "GenericSampler" : "Sampler");

    CubeMapTexture texture;
    texture.setMinificationFilter(T::Filter::Linear, T::Mipmap::Linear)
           .setMagnificationFilter(T::Filter::Linear)
           #ifndef MAGNUM_TARGET_GLES2
           .setMinLod(-750.0f)
           .setMaxLod(750.0f)
           #ifndef MAGNUM_TARGET_GLES
           .setLodBias(0.5f) /* todo both types */
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
            #ifndef MAGNUM_TARGET_GLES2
           .setCompareMode(SamplerCompareMode::CompareRefToTexture)
           .setCompareFunction(SamplerCompareFunction::GreaterOrEqual)
            #endif
           ;

   MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_WEBGL
void CubeMapTextureGLTest::samplingSrgbDecode() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::EXT::sRGB>())
        CORRADE_SKIP(Extensions::EXT::sRGB::string() + std::string(" is not supported."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setSrgbDecode(false);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTextureGLTest::samplingSwizzle() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::ARB::texture_swizzle::string() + std::string(" is not supported."));
    #endif

    CubeMapTexture texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTextureGLTest::samplingMaxLevel() {
    if(!Context::current().isExtensionSupported<Extensions::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::APPLE::texture_max_level::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureGLTest::samplingCompare() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::shadow_samplers>() ||
       !Context::current().isExtensionSupported<Extensions::NV::shadow_samplers_cube>())
        CORRADE_SKIP(Extensions::NV::shadow_samplers_cube::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setCompareMode(SamplerCompareMode::CompareRefToTexture)
           .setCompareFunction(SamplerCompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTextureGLTest::samplingBorderInteger() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::EXT::texture_integer::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::EXT::texture_border_clamp::string() + std::string(" is not supported."));
    #endif

    CubeMapTexture a;
    a.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    CubeMapTexture b;
    b.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureGLTest::samplingDepthStencilMode() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::ARB::stencil_texturing::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported.");
    #endif

    CubeMapTexture texture;
    texture.setDepthStencilMode(SamplerDepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTextureGLTest::samplingBorder() {
    if(!Context::current().isExtensionSupported<Extensions::NV::texture_border_clamp>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP("No required extension is supported.");

    CubeMapTexture texture;
    texture.setWrapping(SamplerWrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

constexpr UnsignedByte Zero[4*4*4]{};

void CubeMapTextureGLTest::storageImageSize() {
    CubeMapTexture texture;
    texture.setStorage(5,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::RGBA8,
        #else
        TextureFormat::RGBA,
        #endif
        Vector2i(32));

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
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

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}

void CubeMapTextureGLTest::storage() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(PixelStorageData[testCaseInstanceId()].storage != PixelStorage{} && !Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() + std::string(" is not supported."));
    #else
    if(PixelStorageData[testCaseInstanceId()].storage != PixelStorage{})
        CORRADE_SKIP("Image unpack is not supported in WebGL 1.");
    #endif
    #endif

    CubeMapTexture texture;
    texture.setStorage(1,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::RGBA8,
        #else
        TextureFormat::RGBA,
        #endif
        Vector2i(2));
    texture.setSubImage(CubeMapCoordinate::PositiveX, 0, {}, ImageView2D{PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorageData[testCaseInstanceId()].dataSparse});
    texture.setSubImage(CubeMapCoordinate::NegativeX, 0, {},
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setSubImage(CubeMapCoordinate::PositiveY, 0, {}, ImageView2D{PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorageData[testCaseInstanceId()].dataSparse});
    texture.setSubImage(CubeMapCoordinate::NegativeY, 0, {},
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setSubImage(CubeMapCoordinate::PositiveZ, 0, {}, ImageView2D{PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorageData[testCaseInstanceId()].dataSparse});
    texture.setSubImage(CubeMapCoordinate::NegativeZ, 0, {},
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    {
        Image2D image = texture.image(CubeMapCoordinate::PositiveX, 0,
            {PixelStorageData[testCaseInstanceId()].storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
            PixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    } {
        Image2D image = texture.image(CubeMapCoordinate::PositiveY, 0,
            {PixelStorageData[testCaseInstanceId()].storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
            PixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    } {
        Image2D image = texture.image(CubeMapCoordinate::PositiveZ, 0,
            {PixelStorageData[testCaseInstanceId()].storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
            PixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    }
    #endif
}

void CubeMapTextureGLTest::image() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(PixelStorageData[testCaseInstanceId()].storage != PixelStorage{} && !Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() + std::string(" is not supported."));
    #else
    if(PixelStorageData[testCaseInstanceId()].storage != PixelStorage{})
        CORRADE_SKIP("Image unpack is not supported in WebGL 1.");
    #endif
    #endif

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    constexpr TextureFormat format = TextureFormat::RGBA8;
    #else
    constexpr TextureFormat format = TextureFormat::RGBA;
    #endif

    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, format, ImageView2D{PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorageData[testCaseInstanceId()].dataSparse});
    texture.setImage(CubeMapCoordinate::NegativeX, 0, format,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setImage(CubeMapCoordinate::PositiveY, 0, format, ImageView2D{PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorageData[testCaseInstanceId()].dataSparse});
    texture.setImage(CubeMapCoordinate::NegativeY, 0, format,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setImage(CubeMapCoordinate::PositiveZ, 0, format, ImageView2D{PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorageData[testCaseInstanceId()].dataSparse});
    texture.setImage(CubeMapCoordinate::NegativeZ, 0, format,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    {
        Image2D image = texture.image(CubeMapCoordinate::PositiveX, 0,
            {PixelStorageData[testCaseInstanceId()].storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
            PixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    } {
        Image2D image = texture.image(CubeMapCoordinate::PositiveY, 0,
            {PixelStorageData[testCaseInstanceId()].storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
            PixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    } {
        Image2D image = texture.image(CubeMapCoordinate::PositiveZ, 0,
            {PixelStorageData[testCaseInstanceId()].storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
            PixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::imageBuffer() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, TextureFormat::RGBA8,
        BufferImage2D{PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorageData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});
    texture.setImage(CubeMapCoordinate::NegativeX, 0, TextureFormat::RGBA8,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setImage(CubeMapCoordinate::PositiveY, 0, TextureFormat::RGBA8,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setImage(CubeMapCoordinate::NegativeY, 0, TextureFormat::RGBA8,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setImage(CubeMapCoordinate::PositiveZ, 0, TextureFormat::RGBA8,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setImage(CubeMapCoordinate::NegativeZ, 0, TextureFormat::RGBA8,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image = texture.image(CubeMapCoordinate::PositiveX, 0,
        {PixelStorageData[testCaseInstanceId()].storage, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(PixelStorageData[testCaseInstanceId()].offset),
        PixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::imageQueryView() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2})
        .setSubImage(CubeMapCoordinate::PositiveY, 0, {},
        ImageView2D{PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorageData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{PixelStorageData[testCaseInstanceId()].offset + 2*2*4};
    MutableImageView2D image{PixelStorageData[testCaseInstanceId()].storage,
            PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, data};
    texture.image(CubeMapCoordinate::PositiveY, 0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
        PixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::imageQueryViewNullptr() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableImageView2D image{PixelFormat::RGBA, PixelType::UnsignedByte,
        Vector2i{2}, {nullptr, 2*2*4}};

    std::ostringstream out;
    Error redirectError{&out};
    texture.image(CubeMapCoordinate::PositiveY, 0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::image(): image view is nullptr\n");
}

void CubeMapTextureGLTest::imageQueryViewBadSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*4];
    MutableImageView2D image{PixelFormat::RGBA, PixelType::UnsignedByte,
        {2, 1}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.image(CubeMapCoordinate::PositiveY, 0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::image(): expected image view size Vector(2, 2) but got Vector(2, 1)\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES
constexpr UnsignedByte SubDataComplete[]{
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
    0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
};
#endif

void CubeMapTextureGLTest::subImage() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(PixelStorageData[testCaseInstanceId()].storage != PixelStorage{} && !Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() + std::string(" is not supported."));
    #else
    if(PixelStorageData[testCaseInstanceId()].storage != PixelStorage{})
        CORRADE_SKIP("Image unpack is not supported in WebGL 1.");
    #endif
    #endif

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    constexpr TextureFormat format = TextureFormat::RGBA8;
    #else
    constexpr TextureFormat format = TextureFormat::RGBA;
    #endif

    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::NegativeX, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::PositiveY, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::NegativeY, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::PositiveZ, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setImage(CubeMapCoordinate::NegativeZ, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero));
    texture.setSubImage(CubeMapCoordinate::PositiveX, 0, Vector2i(1), ImageView2D{
        PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorageData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = texture.image(CubeMapCoordinate::PositiveX, 0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubDataComplete), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::subImageBuffer() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

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
        PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        PixelStorageData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image = texture.image(CubeMapCoordinate::PositiveX, 0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData),
        Containers::arrayView(SubDataComplete),
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::subImageQuery() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(CubeMapCoordinate::PositiveX, 0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4}, SubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image3D image = texture.subImage(0, Range3Di::fromSize({1, 1, 0}, {2, 2, 1}),
        {PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 1));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
        PixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::subImageQueryView() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(CubeMapCoordinate::PositiveX, 0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4}, SubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{PixelStorageData[testCaseInstanceId()].offset + 2*2*4};
    MutableImageView3D image{PixelStorageData[testCaseInstanceId()].storage,
            PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 1}, data};
    texture.subImage(0, Range3Di::fromSize({1, 1, 0}, {2, 2, 1}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 1));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
        PixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::subImageQueryBuffer() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(CubeMapCoordinate::PositiveX, 0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4}, SubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage3D image = texture.subImage(0, Range3Di::fromSize({1, 1, 0}, {2, 2, 1}),
        {PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 1));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(PixelStorageData[testCaseInstanceId()].offset),
        PixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}
#endif

void CubeMapTextureGLTest::compressedImage() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

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
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    const CompressedImageView2D view{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorageData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        CompressedPixelStorageData[testCaseInstanceId()].dataSparse};

    CubeMapTexture texture;
    texture.setCompressedImage(CubeMapCoordinate::PositiveX, 0, view)
           .setCompressedImage(CubeMapCoordinate::NegativeX, 0, view)
           .setCompressedImage(CubeMapCoordinate::PositiveY, 0, view)
           .setCompressedImage(CubeMapCoordinate::NegativeY, 0, view)
           .setCompressedImage(CubeMapCoordinate::PositiveZ, 0, view)
           .setCompressedImage(CubeMapCoordinate::NegativeZ, 0, view);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0, {CompressedPixelStorageData[testCaseInstanceId()].storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorageData[testCaseInstanceId()].offset),
        CompressedPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::compressedImageBuffer() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

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
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    CompressedBufferImage2D buffer{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorageData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        CompressedPixelStorageData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw};

    CubeMapTexture texture;
    texture.setCompressedImage(CubeMapCoordinate::PositiveX, 0, buffer)
           .setCompressedImage(CubeMapCoordinate::NegativeX, 0, buffer)
           .setCompressedImage(CubeMapCoordinate::PositiveY, 0, buffer)
           .setCompressedImage(CubeMapCoordinate::NegativeY, 0, buffer)
           .setCompressedImage(CubeMapCoordinate::PositiveZ, 0, buffer)
           .setCompressedImage(CubeMapCoordinate::NegativeZ, 0, buffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0,
        {CompressedPixelStorageData[testCaseInstanceId()].storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(CompressedPixelStorageData[testCaseInstanceId()].offset),
        CompressedPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::compressedImageQueryView() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));

    const CompressedImageView2D view{
        CompressedPixelStorageData[testCaseInstanceId()].storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        CompressedPixelStorageData[testCaseInstanceId()].dataSparse};

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
        .setCompressedSubImage(CubeMapCoordinate::PositiveZ, 0, {}, view);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{CompressedPixelStorageData[testCaseInstanceId()].offset + 16};
    MutableCompressedImageView2D image{CompressedPixelStorageData[testCaseInstanceId()].storage, CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, data};
    texture.compressedImage(CubeMapCoordinate::PositiveZ, 0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorageData[testCaseInstanceId()].offset),
        CompressedPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedImageQueryViewNullptr() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, {nullptr, 16}};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedImage(CubeMapCoordinate::PositiveX, 0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedImage(): image view is nullptr\n");
}

void CubeMapTextureGLTest::compressedImageQueryViewBadSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*16];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4, 8}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedImage(CubeMapCoordinate::PositiveX, 0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedImage(): expected image view size Vector(4, 4) but got Vector(4, 8)\n");
}

void CubeMapTextureGLTest::compressedImageQueryViewBadDataSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16 - 1];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedImage(CubeMapCoordinate::PositiveX, 0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedImage(): expected image view data size 16 bytes but got 15\n");
}

void CubeMapTextureGLTest::compressedImageQueryViewBadFormat() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt1, Vector2i{4}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedImage(CubeMapCoordinate::PositiveX, 0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedImage(): expected image view format GL::CompressedPixelFormat::RGBAS3tcDxt3 but got GL::CompressedPixelFormat::RGBAS3tcDxt1\n");
}
#endif

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void CubeMapTextureGLTest::immutableCompressedImage() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_storage>())
        CORRADE_SKIP(Extensions::ARB::texture_storage::string() + std::string(" is not supported."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_storage>())
        CORRADE_SKIP(Extensions::EXT::texture_storage::string() + std::string(" is not supported."));
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
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    /* Testing that GL_TEXTURE_COMPRESSED_IMAGE_SIZE is consistent and returns
       the same value regardless whether the texture is immutable or not. (Not
       the case, at least on NVidia 358.16, compare with compressedImage() test
       case that just calls setImage() six times instead of setStorage() and
       gets value that's six times smaller. I couldn't find anything in the
       specs so I suspect it's a bug). */

    const CompressedImageView2D view{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorageData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        CompressedPixelStorageData[testCaseInstanceId()].dataSparse};

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
           .setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, {}, view)
           .setCompressedSubImage(CubeMapCoordinate::NegativeX, 0, {}, view)
           .setCompressedSubImage(CubeMapCoordinate::PositiveY, 0, {}, view)
           .setCompressedSubImage(CubeMapCoordinate::NegativeY, 0, {}, view)
           .setCompressedSubImage(CubeMapCoordinate::PositiveZ, 0, {}, view)
           .setCompressedSubImage(CubeMapCoordinate::NegativeZ, 0, {}, view);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::NegativeY, 0, {CompressedPixelStorageData[testCaseInstanceId()].storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorageData[testCaseInstanceId()].offset),
        CompressedPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}
#endif

/* Just 12x12 zeros compressed using RGBA DXT3 by the driver */
constexpr UnsignedByte CompressedZero[9*16]{};

#ifndef MAGNUM_TARGET_GLES
/* Combination of CompressedZero and CompressedData */
constexpr UnsignedByte CompressedSubDataComplete[]{
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
#endif

void CubeMapTextureGLTest::compressedSubImage() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

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
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
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
        CompressedPixelStorageData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        CompressedPixelStorageData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{12});

    {
        CORRADE_EXPECT_FAIL_IF(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>() && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Non-default compressed pixel storage for cube map textures behaves weirdly on NVidia for client-memory images when using ARB_direct_state_access");

        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedSubDataComplete),
            TestSuite::Compare::Container);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::compressedSubImageBuffer() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

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
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
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
        CompressedPixelStorageData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        CompressedPixelStorageData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0,
        {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{12});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData),
        Containers::arrayView(CompressedSubDataComplete),
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::compressedSubImageQuery() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    if(CompressedPixelStorageData[testCaseInstanceId()].storage == CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12})
        .setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), {CompressedPixelStorageData[testCaseInstanceId()].storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorageData[testCaseInstanceId()].offset),
        CompressedPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedSubImageQueryView() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    if(CompressedPixelStorageData[testCaseInstanceId()].storage == CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12})
        .setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{CompressedPixelStorageData[testCaseInstanceId()].offset + 16};
    MutableCompressedImageView3D image{CompressedPixelStorageData[testCaseInstanceId()].storage, CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 1}, data};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorageData[testCaseInstanceId()].offset),
        CompressedPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedSubImageQueryViewNullptr() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4, 4, 1}, {nullptr, 16}};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedSubImage(): image view is nullptr\n");
}

void CubeMapTextureGLTest::compressedSubImageQueryViewBadSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*16];
    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4, 4, 2}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedSubImage(): expected image view size Vector(4, 4, 1) but got Vector(4, 4, 2)\n");
}

void CubeMapTextureGLTest::compressedSubImageQueryViewBadDataSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16 - 1];
    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4, 4, 1}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedSubImage(): expected image view data size 16 bytes but got 15\n");
}

void CubeMapTextureGLTest::compressedSubImageQueryViewBadFormat() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16];
    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt1, Vector3i{4, 4, 1}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedSubImage(): expected image view format GL::CompressedPixelFormat::RGBAS3tcDxt3 but got GL::CompressedPixelFormat::RGBAS3tcDxt1\n");
}

void CubeMapTextureGLTest::compressedSubImageQueryBuffer() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    if(CompressedPixelStorageData[testCaseInstanceId()].storage == CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12})
           .setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), {CompressedPixelStorageData[testCaseInstanceId()].storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE_AS(
        Containers::arrayCast<UnsignedByte>(imageData).suffix(CompressedPixelStorageData[testCaseInstanceId()].offset),
        CompressedPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::fullImageQuery() {
    setTestCaseDescription(FullPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2, 2})
        .setSubImage(0, {}, ImageView3D{
            PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6},
            FullPixelStorageData[testCaseInstanceId()].data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image3D image = texture.image(0,
        {FullPixelStorageData[testCaseInstanceId()].storage, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && FullPixelStorageData[testCaseInstanceId()].storage != PixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(FullPixelStorageData[testCaseInstanceId()].offset),
            FullPixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    }
}

void CubeMapTextureGLTest::fullImageQueryView() {
    setTestCaseDescription(FullPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2, 2})
        .setSubImage(0, {}, ImageView3D{
            PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6},
            FullPixelStorageData[testCaseInstanceId()].data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{FullPixelStorageData[testCaseInstanceId()].offset + 2*2*6*4};
    MutableImageView3D image{FullPixelStorageData[testCaseInstanceId()].storage, PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6}, data};
    texture.image(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && FullPixelStorageData[testCaseInstanceId()].storage != PixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(FullPixelStorageData[testCaseInstanceId()].offset),
            FullPixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    }
}

void CubeMapTextureGLTest::fullImageQueryViewNullptr() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableImageView3D image{PixelFormat::RGBA, PixelType::UnsignedByte,
        {2, 2, 6}, {nullptr, 2*2*6*4}};

    std::ostringstream out;
    Error redirectError{&out};
    texture.image(0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::image(): image view is nullptr\n");
}

void CubeMapTextureGLTest::fullImageQueryViewBadSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*4*6];
    MutableImageView3D image{PixelFormat::RGBA, PixelType::UnsignedByte,
        {2, 1, 6}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.image(0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::image(): expected image view size Vector(2, 2, 6) but got Vector(2, 1, 6)\n");
}

void CubeMapTextureGLTest::fullImageQueryBuffer() {
    setTestCaseDescription(FullPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2})
        .setSubImage(0, {}, BufferImage3D{
            PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6},
            FullPixelStorageData[testCaseInstanceId()].data,
            BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage3D image = texture.image(0,
        {FullPixelStorageData[testCaseInstanceId()].storage, PixelFormat::RGBA,
            PixelType::UnsignedByte}, BufferUsage::StaticRead);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    const auto imageData = image.buffer().data();
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && FullPixelStorageData[testCaseInstanceId()].storage != PixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(FullPixelStorageData[testCaseInstanceId()].offset),
            FullPixelStorageData[testCaseInstanceId()].data, TestSuite::Compare::Container);
    }
}

void CubeMapTextureGLTest::compressedFullImageQuery() {
    setTestCaseDescription(CompressedFullPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
        .setCompressedSubImage(0, {}, CompressedImageView3D{
            CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6},
            CompressedFullPixelStorageData[testCaseInstanceId()].data});

    {
        bool fails(Context::current().detectedDriver() & Context::DetectedDriver::Amd);
        CORRADE_EXPECT_FAIL_IF(fails,
            "ARB_DSA compressed cubemap APIs are broken on AMD drivers.");

        MAGNUM_VERIFY_NO_GL_ERROR();
        if(fails) CORRADE_SKIP("Skipping the rest of the test");
    }

    CompressedImage3D image = texture.compressedImage(0, {CompressedFullPixelStorageData[testCaseInstanceId()].storage});

    {
        #ifdef CORRADE_TARGET_WINDOWS
        bool fails(Context::current().detectedDriver() & Context::DetectedDriver::IntelWindows);
        CORRADE_EXPECT_FAIL_IF(fails,
            "ARB_DSA compressed cubemap APIs are broken on Intel Windows drivers.");
        #endif

        MAGNUM_VERIFY_NO_GL_ERROR();
        #ifdef CORRADE_TARGET_WINDOWS
        if(fails) CORRADE_SKIP("Skipping the rest of the test");
        #endif
    }

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && CompressedFullPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedFullPixelStorageData[testCaseInstanceId()].offset),
            CompressedFullPixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    }
}

void CubeMapTextureGLTest::compressedFullImageQueryView() {
    setTestCaseDescription(CompressedFullPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
        .setCompressedSubImage(0, {}, CompressedImageView3D{
            CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6},
            CompressedFullPixelStorageData[testCaseInstanceId()].data});

    {
        bool fails(Context::current().detectedDriver() & Context::DetectedDriver::Amd);
        CORRADE_EXPECT_FAIL_IF(fails,
            "ARB_DSA compressed cubemap APIs are broken on AMD drivers.");

        MAGNUM_VERIFY_NO_GL_ERROR();
        if(fails) CORRADE_SKIP("Skipping the rest of the test");
    }

    Containers::Array<char> data{CompressedFullPixelStorageData[testCaseInstanceId()].offset + 16*6};
    MutableCompressedImageView3D image{CompressedFullPixelStorageData[testCaseInstanceId()].storage, CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6}, data};
    texture.compressedImage(0, image);

    {
        #ifdef CORRADE_TARGET_WINDOWS
        bool fails(Context::current().detectedDriver() & Context::DetectedDriver::IntelWindows);
        CORRADE_EXPECT_FAIL_IF(fails,
            "ARB_DSA compressed cubemap APIs are broken on Intel Windows drivers.");
        #endif

        MAGNUM_VERIFY_NO_GL_ERROR();
        #ifdef CORRADE_TARGET_WINDOWS
        if(fails) CORRADE_SKIP("Skipping the rest of the test");
        #endif
    }

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && CompressedFullPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedFullPixelStorageData[testCaseInstanceId()].offset),
            CompressedFullPixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    }
}

void CubeMapTextureGLTest::compressedFullImageQueryViewNullptr() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4, 4, 6}, {nullptr, 16*6}};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedImage(): image view is nullptr\n");
}

void CubeMapTextureGLTest::compressedFullImageQueryViewBadSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*6*16];
    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4, 8, 6}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedImage(): expected image view size Vector(4, 4, 6) but got Vector(4, 8, 6)\n");
}

void CubeMapTextureGLTest::compressedFullImageQueryViewBadDataSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16*6 - 1];
    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4, 4, 6}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedImage(): expected image view data size 96 bytes but got 95\n");
}

void CubeMapTextureGLTest::compressedFullImageQueryViewBadFormat() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[6*16];
    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt1, Vector3i{4, 4, 6}, data};

    std::ostringstream out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out.str(), "GL::CubeMapTexture::compressedImage(): expected image view format GL::CompressedPixelFormat::RGBAS3tcDxt3 but got GL::CompressedPixelFormat::RGBAS3tcDxt1\n");
}

void CubeMapTextureGLTest::compressedFullImageQueryBuffer() {
    setTestCaseDescription(CompressedFullPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
        .setCompressedSubImage(0, {}, CompressedBufferImage3D{
            CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6},
            CompressedFullPixelStorageData[testCaseInstanceId()].data,
            BufferUsage::StaticDraw});

    {
        bool fails(Context::current().detectedDriver() & Context::DetectedDriver::Amd);
        CORRADE_EXPECT_FAIL_IF(fails,
            "ARB_DSA compressed cubemap APIs are broken on AMD drivers.");

        MAGNUM_VERIFY_NO_GL_ERROR();
        if(fails) CORRADE_SKIP("Skipping the rest of the test");
    }

    CompressedBufferImage3D image = texture.compressedImage(0, {CompressedFullPixelStorageData[testCaseInstanceId()].storage}, BufferUsage::StaticRead);

    {
        #ifdef CORRADE_TARGET_WINDOWS
        bool fails(Context::current().detectedDriver() & Context::DetectedDriver::IntelWindows);
        CORRADE_EXPECT_FAIL_IF(fails,
            "ARB_DSA compressed cubemap APIs are broken on Intel Windows drivers.");
        #endif

        MAGNUM_VERIFY_NO_GL_ERROR();
        #ifdef CORRADE_TARGET_WINDOWS
        if(fails) CORRADE_SKIP("Skipping the rest of the test");
        #endif
    }

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    const auto imageData = image.buffer().data();
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && CompressedFullPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(CompressedFullPixelStorageData[testCaseInstanceId()].offset),
            CompressedFullPixelStorageData[testCaseInstanceId()].data,
            TestSuite::Compare::Container);
    }
}
#endif

void CubeMapTextureGLTest::generateMipmap() {
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    constexpr TextureFormat format = TextureFormat::RGBA8;
    #else
    constexpr TextureFormat format = TextureFormat::RGBA;
    #endif

    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));
    texture.setImage(CubeMapCoordinate::PositiveY, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));
    texture.setImage(CubeMapCoordinate::PositiveZ, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));
    texture.setImage(CubeMapCoordinate::NegativeX, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));
    texture.setImage(CubeMapCoordinate::NegativeY, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));
    texture.setImage(CubeMapCoordinate::NegativeZ, 0, format,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i( 0));
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 1));

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}

void CubeMapTextureGLTest::invalidateImage() {
    CubeMapTexture texture;
    texture.setStorage(2,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::RGBA8,
        #else
        TextureFormat::RGBA,
        #endif
        Vector2i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureGLTest::invalidateSubImage() {
    CubeMapTexture texture;
    texture.setStorage(2,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::RGBA8,
        #else
        TextureFormat::RGBA,
        #endif
        Vector2i(32));
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(Vector2i(8), 4));

    {
        #ifndef MAGNUM_TARGET_GLES
        /* Mesa (last checked version 18.0.0) treats cube map images as having
           only single layer instead of 6, so the above invalidation call
           fails. Relevant source code (scroll up to see imageDepth = 1):
           https://github.com/mesa3d/mesa/blob/051fddb4a9e6abb6f2cf9c892e34c8739983c794/src/mesa/main/texobj.c#L2293 */
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && Context::current().isExtensionSupported<Extensions::ARB::invalidate_subdata>(),
            "Broken on Mesa.");
        #endif

        MAGNUM_VERIFY_NO_GL_ERROR();
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::CubeMapTextureGLTest)
