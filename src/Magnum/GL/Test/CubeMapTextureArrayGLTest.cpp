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
#include "Magnum/GL/BufferImage.h"
#include "Magnum/GL/CubeMapTextureArray.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/ImageFormat.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct CubeMapTextureArrayGLTest: OpenGLTester {
    explicit CubeMapTextureArrayGLTest();

    void construct();
    void constructMove();
    void wrap();

    void bind();
    void bindImage();

    template<class T> void sampling();
    void samplingSrgbDecode();
    void samplingBorderInteger();
    void samplingSwizzle();
    void samplingDepthStencilMode();
    #ifdef MAGNUM_TARGET_GLES
    void samplingBorder();
    #endif

    void storage();

    void image();
    void imageBuffer();
    #ifndef MAGNUM_TARGET_GLES
    void imageQueryView();
    #endif
    void subImage();
    void subImageBuffer();
    #ifndef MAGNUM_TARGET_GLES
    void subImageQuery();
    void subImageQueryView();
    void subImageQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */
    #endif

    void compressedImage();
    void compressedImageBuffer();
    #ifndef MAGNUM_TARGET_GLES
    void compressedImageQueryView();
    #endif
    void compressedSubImage();
    void compressedSubImageBuffer();
    #ifndef MAGNUM_TARGET_GLES
    void compressedSubImageQuery();
    void compressedSubImageQueryView();
    void compressedSubImageQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */
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
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,

    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,

    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,

    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x16, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x1e, 0x4f,

    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    PixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} PixelStorageData[]{
    {"default pixel storage",
        Containers::arrayView(Data).suffix(16), {},
        Containers::arrayView(Data).suffix(16), 0},
    {"skip Z",
        Containers::arrayView(Data).suffix(16), PixelStorage{}.setSkip({0, 0, 1}),
        Containers::arrayView(Data), 16}
};

/* Just 4x4 0x00 - 0x3f compressed using RGBA DXT3 by the driver, repeated
   six times */
constexpr UnsignedByte CompressedData[]{
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
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} CompressedPixelStorageData[]{
    {"default pixel storage",
        Containers::arrayView(CompressedData).suffix(16*4),
        #ifndef MAGNUM_TARGET_GLES
        {},
        #endif
        Containers::arrayView(CompressedData).suffix(16*4), 0},
    #ifndef MAGNUM_TARGET_GLES
    {"skip Z",
        Containers::arrayView(CompressedData).suffix(16*4),
        CompressedPixelStorage{}
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16)
            .setSkip({0, 0, 4}),
        Containers::arrayView(CompressedData), 16*4}
    #endif
};

constexpr UnsignedByte SubData[]{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,

    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,

    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    PixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} SubPixelStorageData[]{
    {"default pixel storage",
        Containers::arrayView(SubData).suffix(16), {},
        Containers::arrayView(SubData).suffix(16), 0},
    {"skip Z",
        Containers::arrayView(SubData).suffix(16), PixelStorage{}.setSkip({0, 0, 1}),
        Containers::arrayView(SubData), 16}
};

/* Just 4x4x4 0x00 - 0xff compressed using RGBA DXT3 by the driver */
constexpr UnsignedByte CompressedSubData[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

      0,  17,  17,  34,  34,  51,  51,  67,
    232,  57,   0,   0, 213, 255, 170,   2,
     68,  84,  85, 101, 102, 118, 119, 119,
    239, 123,   8,  66, 213, 255, 170,   2,
    136, 136, 153, 153, 170, 170, 187, 187,
    247, 189,  16, 132, 213, 255, 170,   2,
    203, 204, 220, 221, 237, 238, 254, 255,
    255, 255,  24, 190, 213, 255, 170,   2
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    #ifndef MAGNUM_TARGET_GLES
    CompressedPixelStorage storage;
    #endif
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} CompressedSubPixelStorageData[]{
    {"default pixel storage",
        Containers::arrayView(CompressedSubData).suffix(16*4),
        #ifndef MAGNUM_TARGET_GLES
        {},
        #endif
        Containers::arrayView(CompressedSubData).suffix(16*4), 0},
    #ifndef MAGNUM_TARGET_GLES
    {"skip Z",
        Containers::arrayView(CompressedSubData).suffix(16*4),
        CompressedPixelStorage{}
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16)
            .setSkip({0, 0, 4}),
        Containers::arrayView(CompressedSubData), 16*4}
    #endif
};

CubeMapTextureArrayGLTest::CubeMapTextureArrayGLTest() {
    addTests({&CubeMapTextureArrayGLTest::construct,
              &CubeMapTextureArrayGLTest::constructMove,
              &CubeMapTextureArrayGLTest::wrap,

              &CubeMapTextureArrayGLTest::bind,
              &CubeMapTextureArrayGLTest::bindImage,

              &CubeMapTextureArrayGLTest::sampling<GenericSampler>,
              &CubeMapTextureArrayGLTest::sampling<GLSampler>,
              &CubeMapTextureArrayGLTest::samplingSrgbDecode,
              &CubeMapTextureArrayGLTest::samplingBorderInteger,
              &CubeMapTextureArrayGLTest::samplingSwizzle,
              &CubeMapTextureArrayGLTest::samplingDepthStencilMode,
              #ifdef MAGNUM_TARGET_GLES
              &CubeMapTextureArrayGLTest::samplingBorder,
              #endif

              &CubeMapTextureArrayGLTest::storage});

    addInstancedTests({
        &CubeMapTextureArrayGLTest::image,
        &CubeMapTextureArrayGLTest::imageBuffer,
        #ifndef MAGNUM_TARGET_GLES
        &CubeMapTextureArrayGLTest::imageQueryView
        #endif
        }, Containers::arraySize(PixelStorageData));

    addInstancedTests({
        &CubeMapTextureArrayGLTest::subImage,
        &CubeMapTextureArrayGLTest::subImageBuffer,
        #ifndef MAGNUM_TARGET_GLES
        &CubeMapTextureArrayGLTest::subImageQuery,
        &CubeMapTextureArrayGLTest::subImageQueryView,
        &CubeMapTextureArrayGLTest::subImageQueryBuffer
        #endif
        }, Containers::arraySize(SubPixelStorageData));

    addInstancedTests({
        &CubeMapTextureArrayGLTest::compressedImage,
        &CubeMapTextureArrayGLTest::compressedImageBuffer,
        #ifndef MAGNUM_TARGET_GLES
        &CubeMapTextureArrayGLTest::compressedImageQueryView
        #endif
        }, Containers::arraySize(CompressedPixelStorageData));

    addInstancedTests({
        &CubeMapTextureArrayGLTest::compressedSubImage,
        &CubeMapTextureArrayGLTest::compressedSubImageBuffer,
        #ifndef MAGNUM_TARGET_GLES
        &CubeMapTextureArrayGLTest::compressedSubImageQuery,
        &CubeMapTextureArrayGLTest::compressedSubImageQueryView,
        &CubeMapTextureArrayGLTest::compressedSubImageQueryBuffer,
        #endif
        }, Containers::arraySize(CompressedSubPixelStorageData));

    addTests({&CubeMapTextureArrayGLTest::generateMipmap,

              &CubeMapTextureArrayGLTest::invalidateImage,
              &CubeMapTextureArrayGLTest::invalidateSubImage});
}

void CubeMapTextureArrayGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    {
        CubeMapTextureArray texture;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureArrayGLTest::constructMove() {
    /* Move constructor tested in AbstractTexture, here we just verify there
       are no extra members that would need to be taken care of */
    CORRADE_COMPARE(sizeof(CubeMapTextureArray), sizeof(AbstractTexture));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<CubeMapTextureArray>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<CubeMapTextureArray>::value);
}

void CubeMapTextureArrayGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = CubeMapTextureArray::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    CubeMapTextureArray::wrap(id);
    glDeleteTextures(1, &id);
}

void CubeMapTextureArrayGLTest::bind() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureArrayGLTest::bindImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {32, 32, 12})
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

template<class T> void CubeMapTextureArrayGLTest::sampling() {
    setTestCaseTemplateName(std::is_same<T, GenericSampler>::value ?
        "GenericSampler" : "GLSampler");

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setMinificationFilter(T::Filter::Linear, T::Mipmap::Linear)
           .setMagnificationFilter(T::Filter::Linear)
           .setMinLod(-750.0f)
           .setMaxLod(750.0f)
            #ifndef MAGNUM_TARGET_GLES
           .setLodBias(0.5f)
           #endif
           .setBaseLevel(1)
           .setMaxLevel(750)
           #ifndef MAGNUM_TARGET_GLES
           .setWrapping(T::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           #else
           .setWrapping(T::Wrapping::ClampToEdge)
           #endif
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
           .setCompareMode(SamplerCompareMode::CompareRefToTexture)
           .setCompareFunction(SamplerCompareFunction::GreaterOrEqual);

   MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureArrayGLTest::samplingSrgbDecode() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::EXT::texture_sRGB_decode::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setSrgbDecode(false);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureArrayGLTest::samplingBorderInteger() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::EXT::texture_integer::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::EXT::texture_border_clamp::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray a;
    a.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    CubeMapTextureArray b;
    b.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureArrayGLTest::samplingSwizzle() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::ARB::texture_swizzle::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureArrayGLTest::samplingDepthStencilMode() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::ARB::stencil_texturing::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setDepthStencilMode(SamplerDepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifdef MAGNUM_TARGET_GLES
void CubeMapTextureArrayGLTest::samplingBorder() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::EXT::texture_border_clamp::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setWrapping(SamplerWrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void CubeMapTextureArrayGLTest::storage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, {32, 32, 24});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 24));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 24));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 24));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 24));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 24));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i(0)); /* not available */

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureArrayGLTest::image() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView3D{
        PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6},
        PixelStorageData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {PixelStorageData[testCaseInstanceId()].storage, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
        PixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureArrayGLTest::imageBuffer() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage3D{
        PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6},
        PixelStorageData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0, {PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(PixelStorageData[testCaseInstanceId()].offset),
        PixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureArrayGLTest::imageQueryView() {
    setTestCaseDescription(PixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView3D{
        PixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6},
        PixelStorageData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{PixelStorageData[testCaseInstanceId()].offset + 2*2*6*4};
    MutableImageView3D image{PixelStorageData[testCaseInstanceId()].storage, PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 6}, data};
    texture.image(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(PixelStorageData[testCaseInstanceId()].offset),
        PixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}
#endif

void CubeMapTextureArrayGLTest::compressedImage() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() + std::string(" is not supported."));
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorageData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6},
        CompressedPixelStorageData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage3D image = texture.compressedImage(0, {CompressedPixelStorageData[testCaseInstanceId()].storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorageData[testCaseInstanceId()].offset),
        CompressedPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureArrayGLTest::compressedImageBuffer() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() + std::string(" is not supported."));
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setCompressedImage(0, CompressedBufferImage3D{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorageData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6},
        CompressedPixelStorageData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage3D image = texture.compressedImage(0, {CompressedPixelStorageData[testCaseInstanceId()].storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(CompressedPixelStorageData[testCaseInstanceId()].offset),
        CompressedPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureArrayGLTest::compressedImageQueryView() {
    setTestCaseDescription(CompressedPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));

    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorageData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6},
        CompressedPixelStorageData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{CompressedPixelStorageData[testCaseInstanceId()].offset + 6*16};
    MutableCompressedImageView3D image{CompressedPixelStorageData[testCaseInstanceId()].storage, CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 6}, data};
    texture.compressedImage(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{4, 4, 6}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedPixelStorageData[testCaseInstanceId()].offset),
        CompressedPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}
#endif

constexpr UnsignedByte Zero[4*4*4*6]{};
#ifndef MAGNUM_TARGET_GLES
constexpr UnsignedByte SubDataComplete[]{
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
    0, 0, 0, 0, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0, 0, 0, 0,
    0, 0, 0, 0, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0, 0, 0, 0,
    0, 0, 0, 0, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
};
#endif

void CubeMapTextureArrayGLTest::subImage() {
    setTestCaseDescription(SubPixelStorageData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4, 6}, Zero));
    texture.setSubImage(0, Vector3i(1), ImageView3D{
        SubPixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 4},
        SubPixelStorageData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4, 4, 6));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubDataComplete), TestSuite::Compare::Container);
    #endif
}

void CubeMapTextureArrayGLTest::subImageBuffer() {
    setTestCaseDescription(SubPixelStorageData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4, 6}, Zero));
    texture.setSubImage(0, Vector3i(1), BufferImage3D{
        SubPixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 4},
        SubPixelStorageData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4, 4, 6));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData),
        Containers::arrayView(SubDataComplete),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureArrayGLTest::subImageQuery() {
    setTestCaseDescription(SubPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {4, 4, 6})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4, 6}, SubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, {2, 2, 4}),
        {SubPixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(SubPixelStorageData[testCaseInstanceId()].offset),
        SubPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::subImageQueryView() {
    setTestCaseDescription(SubPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {4, 4, 6})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4, 6}, SubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{SubPixelStorageData[testCaseInstanceId()].offset + 2*2*4*4};
    MutableImageView3D image{PixelStorageData[testCaseInstanceId()].storage, PixelFormat::RGBA, PixelType::UnsignedByte, {2, 2, 4}, data};
    texture.subImage(0, Range3Di::fromSize(Vector3i{1}, {2, 2, 4}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(SubPixelStorageData[testCaseInstanceId()].offset),
        SubPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::subImageQueryBuffer() {
    setTestCaseDescription(SubPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {4, 4, 6})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4, 6}, SubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, {2, 2, 4}),
        {SubPixelStorageData[testCaseInstanceId()].storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(SubPixelStorageData[testCaseInstanceId()].offset),
        SubPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}
#endif

/* Just 12x12x6 zeros compressed using RGBA DXT3 by the driver */
constexpr UnsignedByte CompressedZero[9*16*6]{};

#ifndef MAGNUM_TARGET_GLES
/* Combination of CompressedZero and CompressedSubData */
constexpr UnsignedByte CompressedSubDataComplete[] = {
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
                      0,  17,  17,  34,  34,  51,  51,  67,
                    232,  57,   0,   0, 213, 255, 170,   2,
                                                      0, 0, 0, 0, 0, 0, 0, 0,
                                                      0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
                     68,  84,  85, 101, 102, 118, 119, 119,
                    239, 123,   8,  66, 213, 255, 170,   2,
                                                      0, 0, 0, 0, 0, 0, 0, 0,
                                                      0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
                    136, 136, 153, 153, 170, 170, 187, 187,
                    247, 189,  16, 132, 213, 255, 170,   2,
                                                      0, 0, 0, 0, 0, 0, 0, 0,
                                                      0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
                    203, 204, 220, 221, 237, 238, 254, 255,
                    255, 255,  24, 190, 213, 255, 170,   2,
                                                      0, 0, 0, 0, 0, 0, 0, 0,
                                                      0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0
};
#endif

void CubeMapTextureArrayGLTest::compressedSubImage() {
    setTestCaseDescription(CompressedSubPixelStorageData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() + std::string(" is not supported."));
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
        {12, 12, 6}, CompressedZero});
    texture.setCompressedSubImage(0, {4, 4, 1}, CompressedImageView3D{
        #ifndef MAGNUM_TARGET_GLES
        CompressedSubPixelStorageData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4},
        CompressedSubPixelStorageData[testCaseInstanceId()].dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage3D image = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 12, 6}));

    {
        CORRADE_EXPECT_FAIL_IF(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Non-default compressed pixel storage for cube map textures behaves weirdly on NVidia for client-memory images");

        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedSubDataComplete),
            TestSuite::Compare::Container);
    }
    #endif
}

void CubeMapTextureArrayGLTest::compressedSubImageBuffer() {
    setTestCaseDescription(CompressedSubPixelStorageData[testCaseInstanceId()].name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() + std::string(" is not supported."));
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(CompressedPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3,
        {12, 12, 6}, CompressedZero});
    texture.setCompressedSubImage(0, {4, 4, 1}, CompressedBufferImage3D{
        #ifndef MAGNUM_TARGET_GLES
        CompressedSubPixelStorageData[testCaseInstanceId()].storage,
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, Vector3i{4},
        CompressedSubPixelStorageData[testCaseInstanceId()].dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 12, 6}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData), Containers::arrayView(CompressedSubDataComplete),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void CubeMapTextureArrayGLTest::compressedSubImageQuery() {
    setTestCaseDescription(CompressedSubPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedSubPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    if(CompressedSubPixelStorageData[testCaseInstanceId()].storage == CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 12, 6})
        .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12, 6}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 1}, Vector3i{4}), {CompressedSubPixelStorageData[testCaseInstanceId()].storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedSubPixelStorageData[testCaseInstanceId()].offset),
        CompressedSubPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::compressedSubImageQueryView() {
    setTestCaseDescription(CompressedSubPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedSubPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    if(CompressedSubPixelStorageData[testCaseInstanceId()].storage == CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 12, 6})
        .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12, 6}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> data{CompressedSubPixelStorageData[testCaseInstanceId()].offset + 4*16};
    MutableCompressedImageView3D image{CompressedPixelStorageData[testCaseInstanceId()].storage, CompressedPixelFormat::RGBAS3tcDxt3, {4, 4, 4}, data};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 1}, Vector3i{4}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).suffix(CompressedSubPixelStorageData[testCaseInstanceId()].offset),
        CompressedSubPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::compressedSubImageQueryBuffer() {
    setTestCaseDescription(CompressedSubPixelStorageData[testCaseInstanceId()].name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() + std::string(" is not supported."));
    if(CompressedSubPixelStorageData[testCaseInstanceId()].storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() + std::string(" is not supported."));
    if(CompressedSubPixelStorageData[testCaseInstanceId()].storage == CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 12, 6})
        .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 12, 6}, CompressedSubDataComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 4, 1}, Vector3i{4}), {CompressedSubPixelStorageData[testCaseInstanceId()].storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).suffix(CompressedSubPixelStorageData[testCaseInstanceId()].offset),
        CompressedSubPixelStorageData[testCaseInstanceId()].data,
        TestSuite::Compare::Container);
}
#endif

void CubeMapTextureArrayGLTest::generateMipmap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, {32, 32, 24}));

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 24));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(0));

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 24));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 24));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 24));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 24));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 24));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 1,  1, 24));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureArrayGLTest::invalidateImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, {32, 32, 24});
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void CubeMapTextureArrayGLTest::invalidateSubImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() + std::string(" is not supported."));
    #endif

    CubeMapTextureArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, {32, 32, 24});
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::CubeMapTextureArrayGLTest)
