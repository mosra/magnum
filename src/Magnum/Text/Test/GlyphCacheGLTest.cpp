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

#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#ifdef MAGNUM_TARGET_GLES
#include "Magnum/DebugTools/TextureImage.h"
#endif
#if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/PixelFormat.h"
#endif
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Text/GlyphCacheGL.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct GlyphCacheGLTest: GL::OpenGLTester {
    explicit GlyphCacheGLTest();

    void construct();
    void constructNoPadding();
    void constructProcessed();
    void constructProcessedNoPadding();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void constructDeprecated();
    void constructDeprecatedProcessed();
    void constructDeprecatedTextureFormat();
    void constructDeprecatedTextureFormatProcessed();
    #endif

    void constructCopy();
    void constructMove();

    void setImage();
    void setImageFourChannel();

    void flushImageSubclassProcessedFormatSize();
};

GlyphCacheGLTest::GlyphCacheGLTest() {
    addTests({&GlyphCacheGLTest::construct,
              &GlyphCacheGLTest::constructNoPadding,
              &GlyphCacheGLTest::constructProcessed,
              &GlyphCacheGLTest::constructProcessedNoPadding,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &GlyphCacheGLTest::constructDeprecated,
              &GlyphCacheGLTest::constructDeprecatedProcessed,
              &GlyphCacheGLTest::constructDeprecatedTextureFormat,
              &GlyphCacheGLTest::constructDeprecatedTextureFormatProcessed,
              #endif

              &GlyphCacheGLTest::constructCopy,
              &GlyphCacheGLTest::constructMove,

              &GlyphCacheGLTest::setImage,
              &GlyphCacheGLTest::setImageFourChannel,

              &GlyphCacheGLTest::flushImageSubclassProcessedFormatSize});
}

void GlyphCacheGLTest::construct() {
    GlyphCacheGL cache{PixelFormat::R8Unorm, {1024, 2048}, {3, 2}};
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 2048, 1}));
    CORRADE_COMPARE(cache.padding(), (Vector2i{3, 2}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{1024, 2048}));
    #endif
}

void GlyphCacheGLTest::constructNoPadding() {
    GlyphCacheGL cache{PixelFormat::RGBA8Unorm, {1024, 2048}};
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 2048, 1}));
    CORRADE_COMPARE(cache.padding(), Vector2i{1});
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{1024, 2048}));
    #endif
}

void GlyphCacheGLTest::constructProcessed() {
    struct Cache: GlyphCacheGL {
        explicit Cache(PixelFormat format, const Vector2i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding): GlyphCacheGL{format, size, processedFormat, processedSize, padding} {}

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
    } cache{PixelFormat::R8Unorm, {1024, 2048}, PixelFormat::RGBA8Unorm, {128, 256}, {3, 2}};
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 2048, 1}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{128, 256, 1}));
    CORRADE_COMPARE(cache.padding(), (Vector2i{3, 2}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{128, 256}));
    #endif
}

void GlyphCacheGLTest::constructProcessedNoPadding() {
    struct Cache: GlyphCacheGL {
        explicit Cache(PixelFormat format, const Vector2i& size, PixelFormat processedFormat, const Vector2i& processedSize): GlyphCacheGL{format, size, processedFormat, processedSize} {}

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
    } cache{PixelFormat::R8Unorm, {1024, 2048}, PixelFormat::RGBA8Unorm, {128, 256}};
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 2048, 1}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{128, 256, 1}));
    CORRADE_COMPARE(cache.padding(), Vector2i{1});
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{128, 256}));
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
void GlyphCacheGLTest::constructDeprecated() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    GlyphCacheGL cache{{1024, 2048}};
    CORRADE_IGNORE_DEPRECATED_POP
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 2048, 1}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{1024, 2048}));
    #endif
}

void GlyphCacheGLTest::constructDeprecatedProcessed() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    GlyphCacheGL cache{{1024, 2048}, {128, 256}, {}};
    CORRADE_IGNORE_DEPRECATED_POP
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 2048, 1}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{128, 256}));
    #endif
}

void GlyphCacheGLTest::constructDeprecatedTextureFormat() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    GlyphCacheGL cache{
        #ifndef MAGNUM_TARGET_GLES2
        GL::TextureFormat::RGBA8,
        #else
        GL::TextureFormat::RGBA,
        #endif
        {256, 512}};
    CORRADE_IGNORE_DEPRECATED_POP
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.size(), (Vector3i{256, 512, 1}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{256, 512}));
    #endif
}

void GlyphCacheGLTest::constructDeprecatedTextureFormatProcessed() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    GlyphCacheGL cache{
        #ifndef MAGNUM_TARGET_GLES2
        GL::TextureFormat::RGBA8,
        #else
        GL::TextureFormat::RGBA,
        #endif
        {256, 512}, {32, 64}, {}};
    CORRADE_IGNORE_DEPRECATED_POP
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.size(), (Vector3i{256, 512, 1}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{32, 64}));
    #endif
}
#endif

void GlyphCacheGLTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<GlyphCacheGL>{});
    CORRADE_VERIFY(!std::is_copy_assignable<GlyphCacheGL>{});
}

void GlyphCacheGLTest::constructMove() {
    GlyphCacheGL a{PixelFormat::R8Unorm, {1024, 512}};

    GlyphCacheGL b = Utility::move(a);
    CORRADE_COMPARE(b.format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(b.size(), (Vector3i{1024, 512, 1}));

    GlyphCacheGL c{PixelFormat::RGBA8Unorm, {2, 3}};
    c = Utility::move(b);
    CORRADE_COMPARE(c.format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(c.size(), (Vector3i{1024, 512, 1}));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<GlyphCacheGL>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<GlyphCacheGL>::value);
}

const UnsignedByte InputData[]{
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0x00, 0xff, 0x11, 0xee, 0x22, 0xdd, 0x33, 0xcc,
    0x44, 0xbb, 0x55, 0xaa, 0x66, 0x99, 0x77, 0x88
};

const UnsignedByte ExpectedData[]{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0xff, 0x11, 0xee, 0x22, 0xdd, 0x33, 0xcc,
    0, 0, 0, 0, 0, 0, 0, 0, 0x44, 0xbb, 0x55, 0xaa, 0x66, 0x99, 0x77, 0x88
};

void GlyphCacheGLTest::setImage() {
    GlyphCacheGL cache{PixelFormat::R8Unorm, {16, 8}};

    /* Fill the texture with non-zero data to verify the padding gets uploaded
       as well. On ES2 with EXT_texture_rg the internal format isn't Luminance
       but Red. */
    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        cache.texture().setSubImage(0, {}, Image2D{GL::PixelFormat::Red, GL::PixelType::UnsignedByte, {16, 8}, Containers::Array<char>{DirectInit, 16*8, '\xcd'}});
    else
    #endif
    {
        cache.texture().setSubImage(0, {}, Image2D{PixelFormat::R8Unorm, {16, 8}, Containers::Array<char>{DirectInit, 16*8, '\xcd'}});
    }
    MAGNUM_VERIFY_NO_GL_ERROR();

    Utility::copy(
        Containers::StridedArrayView2D<const UnsignedByte>{InputData, {4, 8}},
        cache.image().pixels<UnsignedByte>()[0].sliceSize({4, 8}, {4, 8}));
    cache.flushImage(Range2Di::fromSize({8, 4}, {8, 4}));
    MAGNUM_VERIFY_NO_GL_ERROR();

    ImageView3D actual = cache.image();
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The CPU-side image is zero-initialized, what was set above in the
       texture isn't present there */
    /** @todo ugh have slicing on images directly already, and 3D image
        comparison */
    CORRADE_COMPARE_AS((ImageView2D{actual.format(), actual.size().xy(), actual.data()}),
        (ImageView2D{PixelFormat::R8Unorm, {16, 8}, ExpectedData}),
        DebugTools::CompareImage);

    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("Luminance format used on GLES2 isn't usable for framebuffer reading, can't verify texture contents.");
    #else
    /* The actual texture has just the slice updated, the rest stays. On GLES
       we cannot really verify that the size matches, but at least
       something. */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = cache.texture().image(0, {PixelFormat::R8Unorm});
    #else
    Image2D image = DebugTools::textureSubImage(cache.texture(), 0, {{}, {16, 8}}, {PixelFormat::R8Unorm});
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();

    const UnsignedByte expectedTextureData[]{
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
            0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
            0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
            0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0,
            0x00, 0xff, 0x11, 0xee, 0x22, 0xdd, 0x33, 0xcc,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0,
            0x44, 0xbb, 0x55, 0xaa, 0x66, 0x99, 0x77, 0x88
    };
    CORRADE_COMPARE_AS(image,
        (ImageView2D{PixelFormat::R8Unorm, {16, 8}, expectedTextureData}),
        DebugTools::CompareImage);
    #endif
}

void GlyphCacheGLTest::setImageFourChannel() {
    /* Same as setImage(), but with a four-channel format (so quarter of
       width). Needed to be able to read the texture on ES2 to verify the
       upload works, as there's a special case for when the EXT_unpack_subimage
       extension isn't present. */

    GlyphCacheGL cache{PixelFormat::RGBA8Unorm, {4, 8}};

    /* Zero the texture to avoid comparing against garbage */
    cache.texture().setSubImage(0, {}, Image2D{PixelFormat::RGBA8Unorm, {4, 8}, Containers::Array<char>{ValueInit, 4*4*8}});

    Utility::copy(
        Containers::StridedArrayView2D<const Color4ub>{Containers::arrayCast<const Color4ub>(Containers::arrayView(InputData)), {4, 2}},
        cache.image().pixels<Color4ub>()[0].sliceSize({4, 2}, {4, 2}));
    cache.flushImage(Range2Di::fromSize({2, 4}, {2, 4}));
    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableImageView3D actual3 = cache.image();
    /** @todo ugh have slicing on images directly already */
    MutableImageView2D actual{actual3.format(), actual3.size().xy(), actual3.data()};
    MAGNUM_VERIFY_NO_GL_ERROR();

    ImageView2D expected{PixelFormat::RGBA8Unorm, {4, 8}, ExpectedData};
    CORRADE_COMPARE_AS(actual,
        expected,
        DebugTools::CompareImage);

    /* Verify the actual texture. It should be the same as above. On GLES we
       cannot really verify that the size matches, but at least something. */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = cache.texture().image(0, {PixelFormat::RGBA8Unorm});
    #else
    Image2D image = DebugTools::textureSubImage(cache.texture(), 0, {{}, {4, 8}}, {PixelFormat::RGBA8Unorm});
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(image,
        expected,
        DebugTools::CompareImage);
}

void GlyphCacheGLTest::flushImageSubclassProcessedFormatSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Cache: GlyphCacheGL {
        explicit Cache(PixelFormat format, const Vector2i& size, PixelFormat processedFormat, const Vector2i& processedSize): GlyphCacheGL{format, size, processedFormat, processedSize} {}

        GlyphCacheFeatures doFeatures() const override {
            return GlyphCacheFeature::ImageProcessing;
        }
    };
    Cache differentFormat{PixelFormat::R8Unorm, {32, 32}, PixelFormat::RGBA8Unorm, {32, 32}};
    Cache differentSize{PixelFormat::R8Unorm, {32, 32}, PixelFormat::R8Unorm, {16, 32}};

    Containers::String out;
    Error redirectError{&out};
    differentFormat.flushImage({{}, {32, 32}});
    differentSize.flushImage({{}, {32, 32}});
    CORRADE_COMPARE_AS(out,
        "Text::GlyphCacheGL::flushImage(): subclass expected to provide a doSetImage() implementation to handle different processed format or size\n"
        "Text::GlyphCacheGL::flushImage(): subclass expected to provide a doSetImage() implementation to handle different processed format or size\n",
        TestSuite::Compare::String);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::GlyphCacheGLTest)
