/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/CubeMapTextureArray.h"
#endif

namespace Magnum { namespace GL { namespace Test { namespace {

struct CubeMapTextureGLTest: OpenGLTester {
    explicit CubeMapTextureGLTest();

    #ifndef MAGNUM_TARGET_GLES
    void compressedBlockSize();
    #endif

    void construct();
    void constructMove();
    void wrap();
    void wrapCreateIfNotAlready();

    #ifndef MAGNUM_TARGET_WEBGL
    void label();
    #endif

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
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void samplingDepthStencilMode();
    #endif
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingBorder();
    #endif

    void storageImageSize();

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void view();
    void viewOnArray();
    #endif

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

    void compressedImage();
    #ifndef MAGNUM_TARGET_GLES2
    void compressedImageBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void compressedImageQueryView();
    void compressedImageQueryViewNullptr();
    void compressedImageQueryViewBadSize();
    void compressedImageQueryViewBadFormat();
    #endif
    void compressedSubImage();
    #ifndef MAGNUM_TARGET_GLES2
    void compressedSubImageBuffer();
    #endif

    void image3D();
    #ifndef MAGNUM_TARGET_GLES2
    void image3DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void image3DQueryView();
    void image3DQueryViewNullptr();
    void image3DQueryViewBadSize();
    void subImage3DQuery();
    void subImage3DQueryView();
    /* unlike all others, subImage() simply calls into AbstractTexture, so
       all assertions are already tested in AbstractTextureGLTest */
    void subImage3DQueryBuffer();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void compressedImage3D();
    #ifndef MAGNUM_TARGET_GLES2
    void compressedImage3DBuffer();
    #endif
    void compressedImage3DQueryView();
    void compressedImage3DQueryViewNullptr();
    void compressedImage3DQueryViewBadSize();
    void compressedImage3DQueryViewBadFormat();
    void compressedSubImage3DQuery();
    void compressedSubImage3DQueryView();
    void compressedSubImage3DQueryViewNullptr();
    void compressedSubImage3DQueryViewBadSize();
    void compressedSubImage3DQueryViewBadFormat();
    void compressedSubImage3DQueryBuffer();
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
        Containers::arrayView(Data).exceptPrefix(8), {},
        Containers::arrayView(Data).exceptPrefix(8), 0},
    #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
    {"skip Y",
        Containers::arrayView(Data).exceptPrefix(8), PixelStorage{}.setSkip({0, 1, 0}),
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
    CompressedPixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} CompressedPixelStorageData[]{
    {"default pixel storage",
        Containers::arrayView(CompressedData).exceptPrefix(16),
        {},
        Containers::arrayView(CompressedData).exceptPrefix(16), 0},
    #ifndef MAGNUM_TARGET_GLES
    {"skip Y",
        Containers::arrayView(CompressedData).exceptPrefix(16),
        CompressedPixelStorage{}
            .setSkip({0, 4, 0}),
        Containers::arrayView(CompressedData), 16}
    #endif
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    CompressedPixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
    bool immutable;
} CompressedSubImageData[]{
    {"default pixel storage",
        Containers::arrayView(CompressedData).exceptPrefix(16),
        {},
        Containers::arrayView(CompressedData).exceptPrefix(16), 0, false},
    #ifndef MAGNUM_TARGET_GLES
    {"skip Y",
        Containers::arrayView(CompressedData).exceptPrefix(16),
        CompressedPixelStorage{}
            .setSkip({0, 4, 0}),
        Containers::arrayView(CompressedData), 16, false},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    {"immutable storage, default pixel storage",
        Containers::arrayView(CompressedData).exceptPrefix(16),
        {},
        Containers::arrayView(CompressedData).exceptPrefix(16), 0, true},
    #ifndef MAGNUM_TARGET_GLES
    {"immutable storage, skip Y",
        Containers::arrayView(CompressedData).exceptPrefix(16),
        CompressedPixelStorage{}
            .setSkip({0, 4, 0}),
        Containers::arrayView(CompressedData), 16, true},
    #endif
    #endif
};

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
        Containers::arrayView(FullData).exceptPrefix(16), {}, 0},
    #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
    {"skip Z",
        Containers::arrayView(FullData).exceptPrefix(16), PixelStorage{}.setSkip({0, 0, 1}), 16}
    #endif
};

#ifndef MAGNUM_TARGET_GLES
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
        Containers::arrayView(CompressedFullData).exceptPrefix(16*4),
        #ifndef MAGNUM_TARGET_GLES
        {},
        #endif
        0},
    #ifndef MAGNUM_TARGET_GLES
    {"skip Z",
        Containers::arrayView(CompressedFullData).exceptPrefix(16*4),
        CompressedPixelStorage{}
            .setSkip({0, 0, 4}), 16*4}
    #endif
};
#endif

CubeMapTextureGLTest::CubeMapTextureGLTest() {
    addTests({
        #ifndef MAGNUM_TARGET_GLES
        &CubeMapTextureGLTest::compressedBlockSize,
        #endif

        &CubeMapTextureGLTest::construct,
        &CubeMapTextureGLTest::constructMove,
        &CubeMapTextureGLTest::wrap,
        &CubeMapTextureGLTest::wrapCreateIfNotAlready,

        #ifndef MAGNUM_TARGET_WEBGL
        &CubeMapTextureGLTest::label,
        #endif

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
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &CubeMapTextureGLTest::samplingDepthStencilMode,
        #endif
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        &CubeMapTextureGLTest::samplingBorder,
        #endif

        &CubeMapTextureGLTest::storageImageSize,

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        &CubeMapTextureGLTest::view,
        &CubeMapTextureGLTest::viewOnArray
        #endif
    });

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

    addInstancedTests({
        &CubeMapTextureGLTest::subImage,
        #ifndef MAGNUM_TARGET_GLES2
        &CubeMapTextureGLTest::subImageBuffer,
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
        }, Containers::arraySize(CompressedPixelStorageData));

    #ifndef MAGNUM_TARGET_GLES
    addTests({&CubeMapTextureGLTest::compressedImageQueryViewNullptr,
              &CubeMapTextureGLTest::compressedImageQueryViewBadSize,
              &CubeMapTextureGLTest::compressedImageQueryViewBadFormat});
    #endif

    addInstancedTests({
        &CubeMapTextureGLTest::compressedSubImage,
        #ifndef MAGNUM_TARGET_GLES2
        &CubeMapTextureGLTest::compressedSubImageBuffer,
        #endif
        }, Containers::arraySize(CompressedSubImageData));

    addInstancedTests({
        &CubeMapTextureGLTest::image3D,
        #ifndef MAGNUM_TARGET_GLES2
        &CubeMapTextureGLTest::image3DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &CubeMapTextureGLTest::image3DQueryView
        #endif
        }, Containers::arraySize(FullPixelStorageData));

    #ifndef MAGNUM_TARGET_GLES
    addTests({&CubeMapTextureGLTest::image3DQueryViewNullptr,
              &CubeMapTextureGLTest::image3DQueryViewBadSize});

    addInstancedTests({
        &CubeMapTextureGLTest::subImage3DQuery,
        &CubeMapTextureGLTest::subImage3DQueryView,
        &CubeMapTextureGLTest::subImage3DQueryBuffer
        }, Containers::arraySize(PixelStorageData));

    addInstancedTests({
        &CubeMapTextureGLTest::compressedImage3D,
        &CubeMapTextureGLTest::compressedImage3DBuffer,
        &CubeMapTextureGLTest::compressedImage3DQueryView},
        Containers::arraySize(CompressedFullPixelStorageData));

    addTests({&CubeMapTextureGLTest::compressedImage3DQueryViewNullptr,
              &CubeMapTextureGLTest::compressedImage3DQueryViewBadSize,
              &CubeMapTextureGLTest::compressedImage3DQueryViewBadFormat});

    addInstancedTests({
        &CubeMapTextureGLTest::compressedSubImage3DQuery,
        &CubeMapTextureGLTest::compressedSubImage3DQueryView,
        &CubeMapTextureGLTest::compressedSubImage3DQueryBuffer
        }, Containers::arraySize(CompressedPixelStorageData));

    addTests({&CubeMapTextureGLTest::compressedSubImage3DQueryViewNullptr,
              &CubeMapTextureGLTest::compressedSubImage3DQueryViewBadSize,
              &CubeMapTextureGLTest::compressedSubImage3DQueryViewBadFormat});
    #endif

    addTests({&CubeMapTextureGLTest::generateMipmap,

              &CubeMapTextureGLTest::invalidateImage,
              &CubeMapTextureGLTest::invalidateSubImage});
}

#ifndef MAGNUM_TARGET_WEBGL
using namespace Containers::Literals;
#endif

template<std::size_t size, class T> Containers::ArrayView<const T> unsafeSuffix(const T(&data)[size], std::size_t offset) {
    static_assert(sizeof(T) == 1, "");
    return {data - offset, size + offset};
}

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::compressedBlockSize() {
    /* For uncompressed formats returns zero */
    CORRADE_COMPARE(CubeMapTexture::compressedBlockSize(TextureFormat::RGBA8), Vector2i{});
    CORRADE_COMPARE(CubeMapTexture::compressedBlockDataSize(TextureFormat::RGBA8), 0);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    {
        /* Same happens with e.g. ASTC 10x10, where it reports 1 (?!) */
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::Mesa,
            "Mesa misreports compressed block size for certain formats.");
        CORRADE_COMPARE(CubeMapTexture::compressedBlockSize(TextureFormat::CompressedRGBAS3tcDxt1), Vector2i{4});
    }
    CORRADE_COMPARE(CubeMapTexture::compressedBlockSize(TextureFormat::CompressedRGBAS3tcDxt3), Vector2i{4});
    CORRADE_COMPARE(CubeMapTexture::compressedBlockDataSize(TextureFormat::CompressedRGBAS3tcDxt1), 8);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

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

void CubeMapTextureGLTest::wrapCreateIfNotAlready() {
    /* Make an object and ensure it's created */
    CubeMapTexture texture;
    texture.bind(0);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(texture.flags(), ObjectFlag::Created|ObjectFlag::DeleteOnDestruction);

    /* Wrap into another object without ObjectFlag::Created being set, which is
       a common usage pattern to make non-owning references. Then calling an
       API that internally does createIfNotAlready() shouldn't assert just
       because Created isn't set but the object is bound, instead it should
       just mark it as such when it discovers it. */
    CubeMapTexture wrapped = CubeMapTexture::wrap(texture.id());
    CORRADE_COMPARE(wrapped.flags(), ObjectFlags{});

    #ifndef MAGNUM_TARGET_WEBGL
    wrapped.label();
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(wrapped.flags(), ObjectFlag::Created);
    #else
    CORRADE_SKIP("No API that would call createIfNotAlready() on WebGL, can't test.");
    #endif
}

#ifndef MAGNUM_TARGET_WEBGL
void CubeMapTextureGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    CubeMapTexture texture;
    CORRADE_COMPARE(texture.label(), "");
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Test the string size gets correctly used, instead of relying on null
       termination */
    texture.setLabel("MyTexture!"_s.exceptSuffix(1));
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(texture.label(), "MyTexture");
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

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
        CORRADE_SKIP(Extensions::ARB::shader_image_load_store::string() << "is not supported.");
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
        CORRADE_SKIP(Extensions::EXT::sRGB::string() << "is not supported.");
    #endif
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::EXT::texture_sRGB_decode::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setSrgbDecode(false);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTextureGLTest::samplingSwizzle() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::ARB::texture_swizzle::string() << "is not supported.");
    #endif

    CubeMapTexture texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTextureGLTest::samplingMaxLevel() {
    if(!Context::current().isExtensionSupported<Extensions::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::APPLE::texture_max_level::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureGLTest::samplingCompare() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::shadow_samplers>() ||
       !Context::current().isExtensionSupported<Extensions::NV::shadow_samplers_cube>())
        CORRADE_SKIP(Extensions::NV::shadow_samplers_cube::string() << "is not supported.");

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
        CORRADE_SKIP(Extensions::EXT::texture_integer::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::EXT::texture_border_clamp::string() << "is not supported.");
    #endif

    CubeMapTexture a;
    a.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    CubeMapTexture b;
    b.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::samplingDepthStencilMode() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::ARB::stencil_texturing::string() << "is not supported.");
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isVersionSupported(Version::GLES310) && !Context::current().isExtensionSupported<Extensions::ANGLE::stencil_texturing>())
        CORRADE_SKIP("Neither OpenGL ES 3.1 nor" << Extensions::ANGLE::stencil_texturing::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::stencil_texturing>())
        CORRADE_SKIP(Extensions::WEBGL::stencil_texturing::string() << "is not supported.");
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

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTextureGLTest::view() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_storage>())
        CORRADE_SKIP(Extensions::ARB::texture_storage::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_view>())
        CORRADE_SKIP(Extensions::ARB::texture_view::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_view>() &&
       !Context::current().isExtensionSupported<Extensions::OES::texture_view>())
        CORRADE_SKIP("Neither" << Extensions::EXT::texture_view::string() << "nor" << Extensions::OES::texture_view::string() << "is supported.");
    #endif

    CubeMapTexture texture;
    texture.setStorage(5, TextureFormat::RGBA8, {32, 32});

    auto view = CubeMapTexture::view(texture, TextureFormat::RGBA8, 2, 3);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(view.imageSize(0), (Vector2i{8, 8}));
    CORRADE_COMPARE(view.imageSize(1), (Vector2i{4, 4}));
    CORRADE_COMPARE(view.imageSize(2), (Vector2i{2, 2}));
}

void CubeMapTextureGLTest::viewOnArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_storage>())
        CORRADE_SKIP(Extensions::ARB::texture_storage::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_view>())
        CORRADE_SKIP(Extensions::ARB::texture_view::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_view>() &&
       !Context::current().isExtensionSupported<Extensions::OES::texture_view>())
        CORRADE_SKIP("Neither" << Extensions::EXT::texture_view::string() << "nor" << Extensions::OES::texture_view::string() << "is supported.");
    #endif

    CubeMapTextureArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, {32, 32, 12});

    auto view = CubeMapTexture::view(texture, TextureFormat::RGBA8, 2, 3, 6);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(view.imageSize(0), (Vector2i{8, 8}));
    CORRADE_COMPARE(view.imageSize(1), (Vector2i{4, 4}));
    CORRADE_COMPARE(view.imageSize(2), (Vector2i{2, 2}));
}
#endif

void CubeMapTextureGLTest::storage() {
    auto&& data = PixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(data.storage != PixelStorage{} && !Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() << "is not supported.");
    #else
    if(data.storage != PixelStorage{})
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
    texture.setSubImage(CubeMapCoordinate::PositiveX, 0, {}, ImageView2D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse});
    texture.setSubImage(CubeMapCoordinate::NegativeX, 0, {},
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setSubImage(CubeMapCoordinate::PositiveY, 0, {}, ImageView2D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse});
    texture.setSubImage(CubeMapCoordinate::NegativeY, 0, {},
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setSubImage(CubeMapCoordinate::PositiveZ, 0, {}, ImageView2D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse});
    texture.setSubImage(CubeMapCoordinate::NegativeZ, 0, {},
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    {
        Image2D image = texture.image(CubeMapCoordinate::PositiveX, 0,
            {data.storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    } {
        Image2D image = texture.image(CubeMapCoordinate::PositiveY, 0,
            {data.storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    } {
        Image2D image = texture.image(CubeMapCoordinate::PositiveZ, 0,
            {data.storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
    #endif
}

void CubeMapTextureGLTest::image() {
    auto&& data = PixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(data.storage != PixelStorage{} && !Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() << "is not supported.");
    #else
    if(data.storage != PixelStorage{})
        CORRADE_SKIP("Image unpack is not supported in WebGL 1.");
    #endif
    #endif

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    constexpr TextureFormat format = TextureFormat::RGBA8;
    #else
    constexpr TextureFormat format = TextureFormat::RGBA;
    #endif

    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, format, ImageView2D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse});
    texture.setImage(CubeMapCoordinate::NegativeX, 0, format,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setImage(CubeMapCoordinate::PositiveY, 0, format, ImageView2D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse});
    texture.setImage(CubeMapCoordinate::NegativeY, 0, format,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});
    texture.setImage(CubeMapCoordinate::PositiveZ, 0, format, ImageView2D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse});
    texture.setImage(CubeMapCoordinate::NegativeZ, 0, format,
        ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2), Zero});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    {
        Image2D image = texture.image(CubeMapCoordinate::PositiveX, 0,
            {data.storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.flags(), ImageFlags2D{});
        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    } {
        Image2D image = texture.image(CubeMapCoordinate::PositiveY, 0,
            {data.storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.flags(), ImageFlags2D{});
        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    } {
        Image2D image = texture.image(CubeMapCoordinate::PositiveZ, 0,
            {data.storage,
            PixelFormat::RGBA, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(image.flags(), ImageFlags2D{});
        CORRADE_COMPARE(image.size(), Vector2i(2));
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::imageBuffer() {
    auto&& data = PixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CubeMapTexture texture;
    texture.setImage(CubeMapCoordinate::PositiveX, 0, TextureFormat::RGBA8,
        BufferImage2D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse,
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
        {data.storage, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::imageQueryView() {
    auto&& data = PixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2})
        .setSubImage(CubeMapCoordinate::PositiveY, 0, {},
        ImageView2D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 2*2*4};
    MutableImageView2D image{data.storage,
            PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, imageData, ImageFlag2D::Array};
    texture.image(CubeMapCoordinate::PositiveY, 0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::imageQueryViewNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableImageView2D image{PixelFormat::RGBA, PixelType::UnsignedByte,
        Vector2i{2}, {nullptr, 2*2*4}};

    Containers::String out;
    Error redirectError{&out};
    texture.image(CubeMapCoordinate::PositiveY, 0, image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::image(): image view is nullptr\n");
}

void CubeMapTextureGLTest::imageQueryViewBadSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*4];
    MutableImageView2D image{PixelFormat::RGBA, PixelType::UnsignedByte,
        {2, 1}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.image(CubeMapCoordinate::PositiveY, 0, image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::image(): expected image view size Vector(2, 2) but got Vector(2, 1)\n");
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
    auto&& data = PixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(data.storage != PixelStorage{} && !Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() << "is not supported.");
    #else
    if(data.storage != PixelStorage{})
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
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse});

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
    auto&& data = PixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

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
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse,
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

void CubeMapTextureGLTest::compressedImage() {
    auto&& data = CompressedPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #endif

    const CompressedImageView2D view{
        data.storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        data.dataSparse};

    CubeMapTexture texture;
    texture.setCompressedImage(CubeMapCoordinate::PositiveX, 0, view)
           .setCompressedImage(CubeMapCoordinate::NegativeX, 0, view)
           .setCompressedImage(CubeMapCoordinate::PositiveY, 0, view)
           .setCompressedImage(CubeMapCoordinate::NegativeY, 0, view)
           .setCompressedImage(CubeMapCoordinate::PositiveZ, 0, view)
           .setCompressedImage(CubeMapCoordinate::NegativeZ, 0, view);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0, {data.storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags2D{});
    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::compressedImageBuffer() {
    auto&& data = CompressedPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #endif

    CompressedBufferImage2D buffer{
        data.storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        data.dataSparse,
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
        {data.storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::compressedImageQueryView() {
    auto&& data = CompressedPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    const CompressedImageView2D view{
        data.storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        data.dataSparse};

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
        .setCompressedSubImage(CubeMapCoordinate::PositiveZ, 0, {}, view);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 16};
    MutableCompressedImageView2D image{data.storage, CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, imageData, ImageFlag2D::Array};
    texture.compressedImage(CubeMapCoordinate::PositiveZ, 0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedImageQueryViewNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, {nullptr, 16}};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedImage(CubeMapCoordinate::PositiveX, 0, image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::compressedImage(): image view is nullptr\n");
}

void CubeMapTextureGLTest::compressedImageQueryViewBadSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*16];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4, 8}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedImage(CubeMapCoordinate::PositiveX, 0, image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::compressedImage(): expected image view size Vector(4, 4) but got Vector(4, 8)\n");
}

void CubeMapTextureGLTest::compressedImageQueryViewBadFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt1, Vector2i{4}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedImage(CubeMapCoordinate::PositiveX, 0, image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::compressedImage(): expected image view format GL::CompressedPixelFormat::RGBAS3tcDxt3 but got GL::CompressedPixelFormat::RGBAS3tcDxt1\n");
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
    auto&& data = CompressedSubImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #endif

    CubeMapTexture texture;
    #ifndef MAGNUM_TARGET_GLES2
    if(data.immutable) {
        texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12});
        texture.setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::NegativeX, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::PositiveY, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::NegativeY, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::PositiveZ, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::NegativeZ, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    } else
    #endif
    {
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
    }
    texture.setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, Vector2i{4}, CompressedImageView2D{
        data.storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{12});
    /* This fails if the "nv-cubemap-broken-dsa-compressed-subimage-upload" is
       disabled, but only if pixel storage is non-default and setStorage()
       isn't used. Thus, the "skip Y" case will fail, and "default pixel
       storage" case will fail if run after any other test that sets pixel
       storage compressed block properties. Running it as a first test
       works. */
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(CompressedSubDataComplete),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::compressedSubImageBuffer() {
    auto&& data = CompressedSubImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #endif

    CubeMapTexture texture;
    if(data.immutable) {
        texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12});
        texture.setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::NegativeX, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::PositiveY, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::NegativeY, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::PositiveZ, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
        texture.setCompressedSubImage(CubeMapCoordinate::NegativeZ, 0, {},
            CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{12}, CompressedZero});
    } else {
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
    }
    texture.setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, Vector2i{4}, CompressedBufferImage2D{
        data.storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        data.dataSparse,
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

void CubeMapTextureGLTest::image3D() {
    auto&& data = FullPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    constexpr TextureFormat format = TextureFormat::RGBA8;
    #else
    constexpr TextureFormat format = TextureFormat::RGBA;
    #endif

    CubeMapTexture texture;
    texture.setStorage(1, format, Vector2i{2, 2})
        .setSubImage(0, {}, ImageView3D{
            PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6},
            data.data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0,
        {data.storage, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlag3D::CubeMap);
    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && data.storage != PixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void CubeMapTextureGLTest::image3DBuffer() {
    auto&& data = FullPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2})
        .setSubImage(0, {}, BufferImage3D{
            PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6},
            data.data,
            BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0,
        {data.storage, PixelFormat::RGBA,
            PixelType::UnsignedByte}, BufferUsage::StaticRead);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    const auto imageData = image.buffer().data();
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && data.storage != PixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
            data.data, TestSuite::Compare::Container);
    }
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureGLTest::image3DQueryView() {
    auto&& data = FullPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2, 2})
        .setSubImage(0, {}, ImageView3D{
            PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6},
            data.data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 2*2*6*4};
    MutableImageView3D image{data.storage, PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6}, imageData, ImageFlag3D::Array};
    texture.image(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && data.storage != PixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
}

void CubeMapTextureGLTest::image3DQueryViewNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableImageView3D image{PixelFormat::RGBA, PixelType::UnsignedByte,
        {2, 2, 6}, {nullptr, 2*2*6*4}};

    Containers::String out;
    Error redirectError{&out};
    texture.image(0, image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::image(): image view is nullptr\n");
}

void CubeMapTextureGLTest::image3DQueryViewBadSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*4*6];
    MutableImageView3D image{PixelFormat::RGBA, PixelType::UnsignedByte,
        {2, 1, 6}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.image(0, image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::image(): expected image view size Vector(2, 2, 6) but got Vector(2, 1, 6)\n");
}

void CubeMapTextureGLTest::subImage3DQuery() {
    auto&& data = PixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(CubeMapCoordinate::PositiveX, 0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4}, SubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image3D image = texture.subImage(0, Range3Di::fromSize({1, 1, 0}, {2, 2, 1}),
        {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 1));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::subImage3DQueryView() {
    auto&& data = PixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(CubeMapCoordinate::PositiveX, 0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4}, SubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 2*2*4};
    MutableImageView3D image{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 1}, imageData, ImageFlag3D::Array};
    texture.subImage(0, Range3Di::fromSize({1, 1, 0}, {2, 2, 1}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 1));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::subImage3DQueryBuffer() {
    auto&& data = PixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(CubeMapCoordinate::PositiveX, 0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4}, SubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage3D image = texture.subImage(0, Range3Di::fromSize({1, 1, 0}, {2, 2, 1}),
        {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 1));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedImage3D() {
    auto&& data = CompressedFullPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
        .setCompressedSubImage(0, {}, CompressedImageView3D{
            CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6},
            data.data});

    {
        bool fails(Context::current().detectedDriver() & Context::DetectedDriver::Amd);
        CORRADE_EXPECT_FAIL_IF(fails,
            "ARB_DSA compressed cubemap APIs are broken on AMD drivers.");

        MAGNUM_VERIFY_NO_GL_ERROR();
        if(fails) CORRADE_SKIP("Skipping the rest of the test");
    }

    CompressedImage3D image = texture.compressedImage(0, {data.storage});

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

    CORRADE_COMPARE(image.flags(), ImageFlag3D::CubeMap);
    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && data.storage != CompressedPixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
}

void CubeMapTextureGLTest::compressedImage3DBuffer() {
    auto&& data = CompressedFullPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
        .setCompressedSubImage(0, {}, CompressedBufferImage3D{
            CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6},
            data.data,
            BufferUsage::StaticDraw});

    {
        bool fails(Context::current().detectedDriver() & Context::DetectedDriver::Amd);
        CORRADE_EXPECT_FAIL_IF(fails,
            "ARB_DSA compressed cubemap APIs are broken on AMD drivers.");

        MAGNUM_VERIFY_NO_GL_ERROR();
        if(fails) CORRADE_SKIP("Skipping the rest of the test");
    }

    CompressedBufferImage3D image = texture.compressedImage(0, {data.storage}, BufferUsage::StaticRead);

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
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && data.storage != CompressedPixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
}

void CubeMapTextureGLTest::compressedImage3DQueryView() {
    auto&& data = CompressedFullPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4})
        .setCompressedSubImage(0, {}, CompressedImageView3D{
            CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6},
            data.data});

    {
        bool fails(Context::current().detectedDriver() & Context::DetectedDriver::Amd);
        CORRADE_EXPECT_FAIL_IF(fails,
            "ARB_DSA compressed cubemap APIs are broken on AMD drivers.");

        MAGNUM_VERIFY_NO_GL_ERROR();
        if(fails) CORRADE_SKIP("Skipping the rest of the test");
    }

    Containers::Array<char> imageData{data.offset + 16*6};
    MutableCompressedImageView3D image{data.storage, CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6}, imageData, ImageFlag3D::Array};
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

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    {
        CORRADE_EXPECT_FAIL_IF((Context::current().detectedDriver() & Context::DetectedDriver::Mesa) && data.storage != CompressedPixelStorage{},
            "Mesa drivers can't handle non-default pixel storage for full cubemap image queries.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
}

void CubeMapTextureGLTest::compressedImage3DQueryViewNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4, 4, 6}, {nullptr, 16*6}};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::compressedImage(): image view is nullptr\n");
}

void CubeMapTextureGLTest::compressedImage3DQueryViewBadSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*6*16];
    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4, 8, 6}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::compressedImage(): expected image view size Vector(4, 4, 6) but got Vector(4, 8, 6)\n");
}

void CubeMapTextureGLTest::compressedImage3DQueryViewBadFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[6*16];
    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt1, Vector3i{4, 4, 6}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::compressedImage(): expected image view format GL::CompressedPixelFormat::RGBAS3tcDxt3 but got GL::CompressedPixelFormat::RGBAS3tcDxt1\n");
}

void CubeMapTextureGLTest::compressedSubImage3DQuery() {
    auto&& data = CompressedPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12})
        .setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), {data.storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedSubImage3DQueryView() {
    auto&& data = CompressedPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12})
        .setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 16};
    MutableCompressedImageView3D image{data.storage, CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 1}, imageData, ImageFlag3D::Array};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void CubeMapTextureGLTest::compressedSubImage3DQueryViewNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4, 4, 1}, {nullptr, 16}};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::compressedSubImage(): image view is nullptr\n");
}

void CubeMapTextureGLTest::compressedSubImage3DQueryViewBadSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*16];
    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4, 4, 2}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::compressedSubImage(): expected image view size Vector(4, 4, 1) but got Vector(4, 4, 2)\n");
}

void CubeMapTextureGLTest::compressedSubImage3DQueryViewBadFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16];
    MutableCompressedImageView3D image{CompressedPixelFormat::RGBAS3tcDxt1, Vector3i{4, 4, 1}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), image);
    CORRADE_COMPARE(out, "GL::CubeMapTexture::compressedSubImage(): expected image view format GL::CompressedPixelFormat::RGBAS3tcDxt3 but got GL::CompressedPixelFormat::RGBAS3tcDxt1\n");
}

void CubeMapTextureGLTest::compressedSubImage3DQueryBuffer() {
    auto&& data = CompressedPixelStorageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    CubeMapTexture texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{12})
           .setCompressedSubImage(CubeMapCoordinate::PositiveX, 0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 0}, {4, 4, 1}), {data.storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE_AS(
        Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
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
