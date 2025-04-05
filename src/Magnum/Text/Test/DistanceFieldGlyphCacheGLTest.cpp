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

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/PluginManager/AbstractManager.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#ifdef MAGNUM_TARGET_GLES
#include "Magnum/DebugTools/TextureImage.h"
#endif
#include "Magnum/Math/Range.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/Text/DistanceFieldGlyphCacheGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct DistanceFieldGlyphCacheGLTest: GL::OpenGLTester {
    explicit DistanceFieldGlyphCacheGLTest();

    void construct();
    void constructSizeRatioNotMultipleOfTwo();

    void constructCopy();
    void constructMove();

    void setImage();

    void setProcessedImage();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void setDistanceFieldImageUnsupportedGLFormat();
    #endif

    PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
};

const struct {
    const char* name;
    Vector2i sourceSize, size, sourceOffset;
    Range2Di flushRange;
    Containers::Size2D offset;
} SetImageData[]{
    {"",
        {256, 256}, {64, 64}, {},
        {{}, {256, 256}},
        {}},
    {"upload with offset",
        {512, 384}, {128, 96}, {256, 128},
        {{256, 128}, {512, 384}},
        {128/4, 256/4}},
    {"tight flush rectangle",
        {256, 256}, {64, 64}, {},
        /* The image is 256x256 with a black 48x48 border around. Even with the
           border excluded from the flush rectangle, the doSetImage() should be
           still called with a large enough padding to properly run the
           distance field algorithm as if the whole image was processed. */
        {{48, 48}, {208, 208}},
        {}},
    {"tight flush rectangle, ratio not a multiple of 2",
        /* Like above, but the flush range isn't satisfying the "multiple of 2"
           assertion and the code needs to round it to a larger rectangle that
           satisfies it */
        {256, 256}, {64, 64}, {},
        {{47, 48}, {208, 209}},
        {}},
};

#ifdef MAGNUM_BUILD_DEPRECATED
const struct {
    const char* name;
    bool deprecated;
    bool glPixelFormat;
} SetProcessedImageData[]{
    {"", false, false},
};
#endif

DistanceFieldGlyphCacheGLTest::DistanceFieldGlyphCacheGLTest() {
    addTests({&DistanceFieldGlyphCacheGLTest::construct,
              &DistanceFieldGlyphCacheGLTest::constructSizeRatioNotMultipleOfTwo,

              &DistanceFieldGlyphCacheGLTest::constructCopy,
              &DistanceFieldGlyphCacheGLTest::constructMove});

    addInstancedTests({&DistanceFieldGlyphCacheGLTest::setImage},
        Containers::arraySize(SetImageData));

    #ifndef MAGNUM_BUILD_DEPRECATED
    addTests({&DistanceFieldGlyphCacheGLTest::setProcessedImage});
    #else
    addInstancedTests({&DistanceFieldGlyphCacheGLTest::setProcessedImage},
        Containers::arraySize(SetProcessedImageData));

    addTests({&DistanceFieldGlyphCacheGLTest::setDistanceFieldImageUnsupportedGLFormat});
    #endif

    /* Load the plugin directly from the build tree. Otherwise it's either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void DistanceFieldGlyphCacheGLTest::construct() {
    DistanceFieldGlyphCacheGL cache{{1024, 2048}, {128, 256}, 16};
    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.features(), GlyphCacheFeature::ImageProcessing|GlyphCacheFeature::ProcessedImageDownload);
    #else
    CORRADE_COMPARE(cache.features(), GlyphCacheFeature::ImageProcessing);
    #endif
    /* The input format is always single-channel */
    CORRADE_COMPARE(cache.format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 2048, 1}));
    /* The processed format is RGBA if it'd have to be Luminance */
    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
    #endif
    {
        CORRADE_COMPARE(cache.processedFormat(), PixelFormat::RGBA8Unorm);
    }
    #ifndef MAGNUM_TARGET_WEBGL
    else
    #endif
    #endif
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    {
        CORRADE_COMPARE(cache.processedFormat(), PixelFormat::R8Unorm);
    }
    #endif
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{128, 256, 1}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{128, 256}));
    #endif
}

void DistanceFieldGlyphCacheGLTest::constructSizeRatioNotMultipleOfTwo() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* This should be fine */
    DistanceFieldGlyphCacheGL{Vector2i{23*14}, Vector2i{23}, 4};

    /* It's the same assert as in TextureTools::DistanceFieldGL */
    Containers::String out;
    Error redirectError{&out};
    DistanceFieldGlyphCacheGL{Vector2i{23*14}, Vector2i{23*2}, 4};
    /* Verify also just one axis wrong */
    DistanceFieldGlyphCacheGL{Vector2i{23*14}, {23*2, 23}, 4};
    DistanceFieldGlyphCacheGL{Vector2i{23*14}, {23, 23*2}, 4};
    /* Almost correct except that it's not an integer multiply */
    DistanceFieldGlyphCacheGL{Vector2i{23*14}, {22, 23}, 4};
    DistanceFieldGlyphCacheGL{Vector2i{23*14}, {23, 22}, 4};
    CORRADE_COMPARE_AS(out,
        "Text::DistanceFieldGlyphCacheGL: expected source and processed size ratio to be a multiple of 2, got {322, 322} and {46, 46}\n"
        "Text::DistanceFieldGlyphCacheGL: expected source and processed size ratio to be a multiple of 2, got {322, 322} and {46, 23}\n"
        "Text::DistanceFieldGlyphCacheGL: expected source and processed size ratio to be a multiple of 2, got {322, 322} and {23, 46}\n"
        "Text::DistanceFieldGlyphCacheGL: expected source and processed size ratio to be a multiple of 2, got {322, 322} and {22, 23}\n"
        "Text::DistanceFieldGlyphCacheGL: expected source and processed size ratio to be a multiple of 2, got {322, 322} and {23, 22}\n",
        TestSuite::Compare::String);
}

void DistanceFieldGlyphCacheGLTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DistanceFieldGlyphCacheGL>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DistanceFieldGlyphCacheGL>{});
}

void DistanceFieldGlyphCacheGLTest::constructMove() {
    DistanceFieldGlyphCacheGL a{{1024, 512}, {128, 64}, 3};

    DistanceFieldGlyphCacheGL b = Utility::move(a);
    CORRADE_COMPARE(b.size(), (Vector3i{1024, 512, 1}));

    DistanceFieldGlyphCacheGL c{{2, 4}, {1, 2}, 1};
    c = Utility::move(b);
    CORRADE_COMPARE(c.size(), (Vector3i{1024, 512, 1}));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DistanceFieldGlyphCacheGL>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DistanceFieldGlyphCacheGL>::value);
}

void DistanceFieldGlyphCacheGLTest::setImage() {
    auto&& data = SetImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<Trade::AbstractImporter> importer;
    if(!(importer = _manager.loadAndInstantiate("TgaImporter")))
        CORRADE_SKIP("TgaImporter plugin not found.");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(TEXTURETOOLS_DISTANCEFIELDGLTEST_DIR, "input.tga")));
    CORRADE_COMPARE(importer->image2DCount(), 1);
    Containers::Optional<Trade::ImageData2D> inputImage = importer->image2D(0);
    CORRADE_VERIFY(inputImage);
    CORRADE_COMPARE(inputImage->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(inputImage->size(), (Vector2i{256, 256}));

    DistanceFieldGlyphCacheGL cache{data.sourceSize, data.size, 32};

    /* Clear the target texture to avoid random garbage getting in when the
       data.flushRange isn't covering the whole output */
    Containers::Array<char> zeros{ValueInit, data.size.product()*pixelFormatSize(cache.processedFormat())};
    cache.texture().setSubImage(0, {},
        /* On ES2, R8Unorm maps to Luminance, but here it's actually Red if
           EXT_texture_rg is supported */
        #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        cache.processedFormat() == PixelFormat::R8Unorm ?
            ImageView2D{GL::PixelFormat::Red, GL::PixelType::UnsignedByte, data.size, zeros} :
        #endif
            ImageView2D{cache.processedFormat(), data.size, zeros}
    );

    Containers::StridedArrayView3D<const char> src = inputImage->pixels();
    /* Test also uploading under an offset. The cache might be three-component
       in some cases, slice the destination view to just the first component */
    /** @todo actually the input can be just luminance, only the destination
        cannot -- fix by dropping the dependency on GlyphCache and creating the
        texture directly */
    Utility::copy(src, cache.image().pixels()[0].sliceSize({
        std::size_t(data.sourceOffset.y()),
        std::size_t(data.sourceOffset.x()),
        0}, src.size()));
    cache.flushImage(data.flushRange);
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* On GLES processedImage() isn't implemented as it'd mean creating a
       temporary framebuffer. Do it via DebugTools here instead, we cannot
       really verify that the size matches, but at least something. */
    #ifndef MAGNUM_TARGET_GLES
    Image3D actual3 = cache.processedImage();
    /** @todo ugh have slicing on images directly already */
    MutableImageView2D actual{actual3.format(), actual3.size().xy(), actual3.data()};
    #elif !defined(MAGNUM_TARGET_GLES2)
    Image2D actual = DebugTools::textureSubImage(cache.texture(), 0, {{}, data.size}, cache.processedFormat());
    #else
    /* On ES2, R8Unorm maps to Luminance, but here it's actually Red if
       EXT_texture_rg is supported */
    Image2D actual = DebugTools::textureSubImage(cache.texture(), 0, {{}, data.size},
        #ifndef MAGNUM_TARGET_WEBGL
        cache.processedFormat() == PixelFormat::R8Unorm ?
            Image2D{GL::PixelFormat::Red, GL::PixelType::UnsignedByte} :
        #endif
            Image2D{cache.processedFormat()}
    );
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The format may be three-component, consider just the first channel */
    Containers::StridedArrayView3D<const char> pixels = actual.pixels();
    CORRADE_COMPARE_WITH((Containers::arrayCast<2, const UnsignedByte>(pixels.prefix({pixels.size()[0], pixels.size()[1], 1})).exceptPrefix(data.offset)),
        Utility::Path::join(TEXTURETOOLS_DISTANCEFIELDGLTEST_DIR, "output.tga"),
        /* Same threshold as in TextureTools DistanceFieldGLTest */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.178f}));
}

void DistanceFieldGlyphCacheGLTest::setProcessedImage() {
    #ifdef MAGNUM_BUILD_DEPRECATED
    auto&& data = SetProcessedImageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    #endif

    DistanceFieldGlyphCacheGL cache({64, 32}, {16, 8}, 16);

    #ifdef MAGNUM_TARGET_GLES2
    /* Ugh, don't want to bother implementing this */
    if(cache.processedFormat() == PixelFormat::RGBA8Unorm)
        CORRADE_SKIP("A four-component input is expected on ES2, skipping due to developer laziness.");
    #endif

    /* Clear the texture first, as it'd have random garbage otherwise */
    UnsignedByte zeros[16*8]{};
    cache.setProcessedImage({}, ImageView2D{PixelFormat::R8Unorm, {16, 8}, zeros});
    MAGNUM_VERIFY_NO_GL_ERROR();

    UnsignedByte imageData[]{
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    };

    #ifdef MAGNUM_BUILD_DEPRECATED
    if(data.deprecated) {
        CORRADE_IGNORE_DEPRECATED_PUSH
        if(data.glPixelFormat)
            cache.setDistanceFieldImage({8, 4}, ImageView2D{
                #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
                GL::PixelFormat::Red,
                #else
                GL::PixelFormat::Luminance,
                #endif
                GL::PixelType::UnsignedByte, {8, 4}, imageData});
        else
            cache.setDistanceFieldImage({8, 4}, ImageView2D{PixelFormat::R8Unorm, {8, 4}, imageData});
        CORRADE_IGNORE_DEPRECATED_POP
    } else
    #endif
    {
        cache.setProcessedImage({8, 4}, ImageView2D{PixelFormat::R8Unorm, {8, 4}, imageData});
    }
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* On GLES processedImage() isn't implemented as it'd mean creating a
       temporary framebuffer. Do it via DebugTools here instead, we cannot
       really verify that the size matches, but at least something. */
    #ifndef MAGNUM_TARGET_GLES
    Image3D actual3 = cache.processedImage();
    /** @todo ugh have slicing on images directly already */
    MutableImageView2D actual{actual3.format(), actual3.size().xy(), actual3.data()};
    #elif !defined(MAGNUM_TARGET_GLES2)
    Image2D actual = DebugTools::textureSubImage(cache.texture(), 0, {{}, {16, 8}}, cache.processedFormat());
    #else
    /* On ES2, R8Unorm maps to Luminance, but here it's actually Red if
       EXT_texture_rg is supported. We however need the generic format again
       below for comparison, so reinterpret it back. */
    Image2D actualGLFormat = DebugTools::textureSubImage(cache.texture(), 0, {{}, {16, 8}},
        #ifndef MAGNUM_TARGET_WEBGL
        cache.processedFormat() == PixelFormat::R8Unorm ?
            Image2D{GL::PixelFormat::Red, GL::PixelType::UnsignedByte} :
        #endif
            Image2D{cache.processedFormat()}
    );
    ImageView2D actual{actualGLFormat.storage(), PixelFormat::R8Unorm, actualGLFormat.size(), actualGLFormat.data()};
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();

    UnsignedByte expected[]{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
        0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    };
    CORRADE_COMPARE_AS(actual,
        (ImageView2D{PixelFormat::R8Unorm, {16, 8}, expected}),
        DebugTools::CompareImage);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void DistanceFieldGlyphCacheGLTest::setDistanceFieldImageUnsupportedGLFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DistanceFieldGlyphCacheGL cache{{4, 4}, {1, 1}, 4};

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    /* Format that is convertible back to the generic format but isn't
       supported */
    cache.setDistanceFieldImage({}, ImageView2D{GL::PixelFormat::RGBA, GL::PixelType::Float, {1, 1}, "hellohellohello"});
    /* Format that doesn't have a generic equivalent gets passed as-is */
    cache.setDistanceFieldImage({}, ImageView2D{GL::PixelFormat::RGBA, GL::PixelType::UnsignedShort5551, {1, 1}, "hello!!"});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE_AS(out, Utility::format(
        "Text::AbstractGlyphCache::setProcessedImage(): expected PixelFormat::{0} but got PixelFormat::RGBA32F\n"
        "Text::AbstractGlyphCache::setProcessedImage(): expected PixelFormat::{0} but got PixelFormat::ImplementationSpecific(0x1908)\n",
        cache.processedFormat() == PixelFormat::RGBA8Unorm ? "RGBA8Unorm" : "R8Unorm"),
        TestSuite::Compare::String);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::DistanceFieldGlyphCacheGLTest)
