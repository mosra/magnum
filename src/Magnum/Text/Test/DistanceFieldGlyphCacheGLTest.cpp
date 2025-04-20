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
#include "Magnum/GL/Texture.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/TextureArray.h"
#endif
#include "Magnum/Text/DistanceFieldGlyphCacheGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct DistanceFieldGlyphCacheGLTest: GL::OpenGLTester {
    explicit DistanceFieldGlyphCacheGLTest();

    void construct();
    #ifndef MAGNUM_TARGET_GLES2
    void constructArray();
    #endif
    void constructSizeRatioNotMultipleOfTwo();
    #ifndef MAGNUM_TARGET_GLES2
    void constructSizeRatioNotMultipleOfTwoArray();
    #endif
    void constructCopy();
    #ifndef MAGNUM_TARGET_GLES2
    void constructCopyArray();
    #endif
    void constructMove();
    #ifndef MAGNUM_TARGET_GLES2
    void constructMoveArray();
    #endif

    void setImage();
    #ifndef MAGNUM_TARGET_GLES2
    void setImageArray();
    #endif
    void setImageEdgeClamp();
    #ifndef MAGNUM_TARGET_GLES2
    void setImageEdgeClampArray();
    #endif

    void setProcessedImage();
    #ifndef MAGNUM_TARGET_GLES2
    void setProcessedImageArray();
    #endif
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

/* Expands upon SetImageData with third dimension. For simplicity only a single
   layer always contains data to process, but it's changed which one it is to
   verify all uploaded layers get processed. */
const struct {
    const char* name;
    Vector3i sourceSize;
    Vector2i size;
    Vector3i sourceOffset;
    Range3Di flushRange;
    Containers::Size2D offset;
} SetImageArrayData[]{
    {"single layer",
        {256, 256, 1}, {64, 64}, {},
        {{}, {256, 256, 1}},
        {}},
    {"multiple layers, data in the first layer",
        {256, 256, 7}, {64, 64}, {},
        {{}, {256, 256, 7}},
        {}},
    {"multiple layers, data in the middle layer",
        {256, 256, 7}, {64, 64}, {0, 0, 3},
        {{}, {256, 256, 7}},
        {}},
    {"multiple layers, data in the last layer",
        {256, 256, 7}, {64, 64}, {0, 0, 6},
        {{}, {256, 256, 7}},
        {}},
    {"single layer, upload with offset",
        {512, 384, 1}, {128, 96}, {256, 128, 0},
        {{256, 128, 0}, {512, 384, 1}},
        {128/4, 256/4}},
    {"multiple layers, upload with offset, data in the first flushed layer",
        {512, 384, 7}, {128, 96}, {256, 128, 3},
        {{256, 128, 3}, {512, 384, 7}},
        {128/4, 256/4}},
    {"multiple layers, upload with offset, data in the middle flushed layer",
        {512, 384, 7}, {128, 96}, {256, 128, 3},
        {{256, 128, 1}, {512, 384, 5}},
        {128/4, 256/4}},
    {"multiple layers, upload with offset, data in the last flushed layer",
        {512, 384, 7}, {128, 96}, {256, 128, 4},
        {{256, 128, 1}, {512, 384, 5}},
        {128/4, 256/4}},
    /* These two don't have the other layer offset variants as that should be
       sufficiently tested above */
    {"tight flush rectangle",
        {256, 256, 7}, {64, 64}, {0, 0, 3},
        {{48, 48, 2}, {208, 208, 6}},
        {}},
    {"tight flush rectangle, ratio not a multiple of 2",
        {256, 256, 7}, {64, 64}, {0, 0, 3},
        {{47, 48, 2}, {208, 209, 6}},
        {}},
};

#ifdef MAGNUM_BUILD_DEPRECATED
const struct {
    const char* name;
    bool deprecated;
    bool glPixelFormat;
} SetProcessedImageData[]{
    {"", false, false},
    {"using deprecated API", true, false},
    {"using deprecated API and GL-specific pixel format", true, true},
};
#endif

DistanceFieldGlyphCacheGLTest::DistanceFieldGlyphCacheGLTest() {
    addTests({&DistanceFieldGlyphCacheGLTest::construct,
              #ifndef MAGNUM_TARGET_GLES2
              &DistanceFieldGlyphCacheGLTest::constructArray,
              #endif
              &DistanceFieldGlyphCacheGLTest::constructSizeRatioNotMultipleOfTwo,
              #ifndef MAGNUM_TARGET_GLES2
              &DistanceFieldGlyphCacheGLTest::constructSizeRatioNotMultipleOfTwoArray,
              #endif
              &DistanceFieldGlyphCacheGLTest::constructCopy,
              #ifndef MAGNUM_TARGET_GLES2
              &DistanceFieldGlyphCacheGLTest::constructCopyArray,
              #endif
              &DistanceFieldGlyphCacheGLTest::constructMove,
              #ifndef MAGNUM_TARGET_GLES2
              &DistanceFieldGlyphCacheGLTest::constructMoveArray
              #endif
              });

    addInstancedTests({&DistanceFieldGlyphCacheGLTest::setImage},
        Containers::arraySize(SetImageData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&DistanceFieldGlyphCacheGLTest::setImageArray},
        Containers::arraySize(SetImageArrayData));
    #endif

    addTests({&DistanceFieldGlyphCacheGLTest::setImageEdgeClamp,
              #ifndef MAGNUM_TARGET_GLES2
              &DistanceFieldGlyphCacheGLTest::setImageEdgeClampArray
              #endif
              });

    #ifndef MAGNUM_BUILD_DEPRECATED
    addTests({&DistanceFieldGlyphCacheGLTest::setProcessedImage});
    #else
    addInstancedTests({&DistanceFieldGlyphCacheGLTest::setProcessedImage},
        Containers::arraySize(SetProcessedImageData));
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    addTests({&DistanceFieldGlyphCacheGLTest::setProcessedImageArray});
    #endif

    #ifdef MAGNUM_BUILD_DEPRECATED
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
    DistanceFieldGlyphCacheGL cache{{256, 512}, {64, 128}, 16};
    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.features(), GlyphCacheFeature::ImageProcessing|GlyphCacheFeature::ProcessedImageDownload);
    #else
    CORRADE_COMPARE(cache.features(), GlyphCacheFeature::ImageProcessing);
    #endif
    /* The input format is always single-channel */
    CORRADE_COMPARE(cache.format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(cache.size(), (Vector3i{256, 512, 1}));
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
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{64, 128, 1}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{64, 128}));
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void DistanceFieldGlyphCacheGLTest::constructArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    DistanceFieldGlyphCacheArrayGL cache{{256, 512, 7}, {64, 128}, 16};
    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.features(), GlyphCacheFeature::ImageProcessing|GlyphCacheFeature::ProcessedImageDownload);
    #else
    CORRADE_COMPARE(cache.features(), GlyphCacheFeature::ImageProcessing);
    #endif
    /* The input format is always single-channel */
    CORRADE_COMPARE(cache.format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(cache.size(), (Vector3i{256, 512, 7}));
    CORRADE_COMPARE(cache.processedFormat(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(cache.processedSize(), (Vector3i{64, 128, 7}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector3i{64, 128, 7}));
    #endif
}
#endif

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

#ifndef MAGNUM_TARGET_GLES2
void DistanceFieldGlyphCacheGLTest::constructSizeRatioNotMultipleOfTwoArray() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    /* This should be fine. THe depth doesn't affect anything. */
    DistanceFieldGlyphCacheArrayGL{{Vector2i{23*14}, 7}, Vector2i{23}, 4};

    /* It's the same assert as in TextureTools::DistanceFieldGL */
    Containers::String out;
    Error redirectError{&out};
    DistanceFieldGlyphCacheArrayGL{{Vector2i{23*14}, 7}, Vector2i{23*2}, 4};
    /* Verify also just one axis wrong */
    DistanceFieldGlyphCacheArrayGL{{Vector2i{23*14}, 7}, {23*2, 23}, 4};
    DistanceFieldGlyphCacheArrayGL{{Vector2i{23*14}, 7}, {23, 23*2}, 4};
    /* Almost correct except that it's not an integer multiply */
    DistanceFieldGlyphCacheArrayGL{{Vector2i{23*14}, 7}, {22, 23}, 4};
    DistanceFieldGlyphCacheArrayGL{{Vector2i{23*14}, 7}, {23, 22}, 4};
    CORRADE_COMPARE_AS(out,
        "Text::DistanceFieldGlyphCacheArrayGL: expected source and processed size ratio to be a multiple of 2, got {322, 322} and {46, 46}\n"
        "Text::DistanceFieldGlyphCacheArrayGL: expected source and processed size ratio to be a multiple of 2, got {322, 322} and {46, 23}\n"
        "Text::DistanceFieldGlyphCacheArrayGL: expected source and processed size ratio to be a multiple of 2, got {322, 322} and {23, 46}\n"
        "Text::DistanceFieldGlyphCacheArrayGL: expected source and processed size ratio to be a multiple of 2, got {322, 322} and {22, 23}\n"
        "Text::DistanceFieldGlyphCacheArrayGL: expected source and processed size ratio to be a multiple of 2, got {322, 322} and {23, 22}\n",
        TestSuite::Compare::String);
}
#endif

void DistanceFieldGlyphCacheGLTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DistanceFieldGlyphCacheGL>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DistanceFieldGlyphCacheGL>{});
}

#ifndef MAGNUM_TARGET_GLES2
void DistanceFieldGlyphCacheGLTest::constructCopyArray() {
    CORRADE_VERIFY(!std::is_copy_constructible<DistanceFieldGlyphCacheArrayGL>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DistanceFieldGlyphCacheArrayGL>{});
}
#endif

void DistanceFieldGlyphCacheGLTest::constructMove() {
    DistanceFieldGlyphCacheGL a{{256, 512}, {64, 64}, 3};

    DistanceFieldGlyphCacheGL b = Utility::move(a);
    CORRADE_COMPARE(b.size(), (Vector3i{256, 512, 1}));

    DistanceFieldGlyphCacheGL c{{2, 4}, {1, 2}, 1};
    c = Utility::move(b);
    CORRADE_COMPARE(c.size(), (Vector3i{256, 512, 1}));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DistanceFieldGlyphCacheGL>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DistanceFieldGlyphCacheGL>::value);
}

#ifndef MAGNUM_TARGET_GLES2
void DistanceFieldGlyphCacheGLTest::constructMoveArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    DistanceFieldGlyphCacheArrayGL a{{256, 512, 7}, {64, 64}, 3};

    DistanceFieldGlyphCacheArrayGL b = Utility::move(a);
    CORRADE_COMPARE(b.size(), (Vector3i{256, 512, 7}));

    DistanceFieldGlyphCacheArrayGL c{{2, 4, 3}, {1, 2}, 1};
    c = Utility::move(b);
    CORRADE_COMPARE(c.size(), (Vector3i{256, 512, 7}));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DistanceFieldGlyphCacheGL>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DistanceFieldGlyphCacheGL>::value);
}
#endif

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

    Containers::StridedArrayView2D<const UnsignedByte> src = inputImage->pixels<UnsignedByte>();
    /* Test also uploading under an offset */
    Utility::copy(src, cache.image().pixels<UnsignedByte>()[0].sliceSize({
        std::size_t(data.sourceOffset.y()),
        std::size_t(data.sourceOffset.x())}, src.size()));
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

#ifndef MAGNUM_TARGET_GLES2
void DistanceFieldGlyphCacheGLTest::setImageArray() {
    auto&& data = SetImageArrayData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    Containers::Pointer<Trade::AbstractImporter> importer;
    if(!(importer = _manager.loadAndInstantiate("TgaImporter")))
        CORRADE_SKIP("TgaImporter plugin not found.");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(TEXTURETOOLS_DISTANCEFIELDGLTEST_DIR, "input.tga")));
    CORRADE_COMPARE(importer->image2DCount(), 1);
    Containers::Optional<Trade::ImageData2D> inputImage = importer->image2D(0);
    CORRADE_VERIFY(inputImage);
    CORRADE_COMPARE(inputImage->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(inputImage->size(), (Vector2i{256, 256}));

    DistanceFieldGlyphCacheArrayGL cache{data.sourceSize, data.size, 32};

    /* Clear the target texture to avoid random garbage getting in when the
       data.flushRange isn't covering the whole output */
    Containers::Array<char> zeros{ValueInit, data.size.product()*data.sourceSize.z()*pixelFormatSize(cache.processedFormat())};
    cache.texture().setSubImage(0, {}, ImageView3D{cache.processedFormat(), {data.size, data.sourceSize.z()}, zeros});

    Containers::StridedArrayView3D<const UnsignedByte> src = inputImage->pixels<UnsignedByte>();
    /* Test also uploading under an offset */
    Utility::copy(src, cache.image().pixels<UnsignedByte>().sliceSize({
        std::size_t(data.sourceOffset.z()),
        std::size_t(data.sourceOffset.y()),
        std::size_t(data.sourceOffset.x())}, src.size()));
    cache.flushImage(data.flushRange);
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* On GLES processedImage() isn't implemented as it'd mean creating a
       temporary framebuffer. Do it via DebugTools here instead, we cannot
       really verify that the size matches, but at least something.

       Only one layer always contains the processed data, get just that one. */
    #ifndef MAGNUM_TARGET_GLES
    Image3D actual3 = cache.processedImage();
    /** @todo ugh have slicing on images directly already */
    MutableImageView2D actual{actual3.format(), actual3.size().xy(), actual3.data().exceptPrefix(actual3.size().xy().product()*actual3.pixelSize()*data.sourceOffset.z())};
    #else
    Image2D actual = DebugTools::textureSubImage(cache.texture(), 0, data.sourceOffset.z(), {{}, data.size}, cache.processedFormat());
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The format may be three-component, consider just the first channel */
    Containers::StridedArrayView3D<const char> pixels = actual.pixels();
    {
        #ifdef MAGNUM_TARGET_GLES
        CORRADE_EXPECT_FAIL_IF(data.sourceOffset.z() != 0 && GL::Context::current().detectedDriver() >= GL::Context::DetectedDriver::SwiftShader,
            "SwiftShader is trash and doesn't implement reading from non-zero array layers.");
        #endif
        CORRADE_COMPARE_WITH((Containers::arrayCast<2, const UnsignedByte>(pixels.prefix({pixels.size()[0], pixels.size()[1], 1})).exceptPrefix(data.offset)),
            Utility::Path::join(TEXTURETOOLS_DISTANCEFIELDGLTEST_DIR, "output.tga"),
            /* Same threshold as in TextureTools DistanceFieldGLTest */
            (DebugTools::CompareImageToFile{_manager, 1.0f, 0.178f}));
    }
}
#endif

void DistanceFieldGlyphCacheGLTest::setImageEdgeClamp() {
    /* Verifies that the input texture filtering clamp is set to edge to not
       have content from one side leak to another when the data don't have
       enough padding. Affects only the non-texelFetch() codepath, texel
       fetches return zero for out-of-bounds reads. Well, assuming robustness
       enabled, at least. */

    DistanceFieldGlyphCacheGL cache{{8, 4}, {4, 2}, 4};

    /* Make the right edge all white */
    Containers::StridedArrayView2D<UnsignedByte> src = cache.image().pixels<UnsignedByte>()[0];
    src[0][7] = '\xff';
    src[1][7] = '\xff';
    src[2][7] = '\xff';
    src[3][7] = '\xff';
    cache.flushImage({{}, {8, 4}});
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* On GLES processedImage() isn't implemented as it'd mean creating a
       temporary framebuffer. Do it via DebugTools here instead, we cannot
       really verify that the size matches, but at least something. */
    #ifndef MAGNUM_TARGET_GLES
    Image3D actual3 = cache.processedImage();
    /** @todo ugh have slicing on images directly already */
    MutableImageView2D actual{actual3.format(), actual3.size().xy(), actual3.data()};
    #elif !defined(MAGNUM_TARGET_GLES2)
    Image2D actual = DebugTools::textureSubImage(cache.texture(), 0, {{}, {4, 2}}, cache.processedFormat());
    #else
    /* On ES2, R8Unorm maps to Luminance, but here it's actually Red if
       EXT_texture_rg is supported */
    Image2D actual = DebugTools::textureSubImage(cache.texture(), 0, {{}, {4, 2}},
        #ifndef MAGNUM_TARGET_WEBGL
        cache.processedFormat() == PixelFormat::R8Unorm ?
            Image2D{GL::PixelFormat::Red, GL::PixelType::UnsignedByte} :
        #endif
            Image2D{cache.processedFormat()}
    );
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The format may be three-component, consider just the first channel */
    Containers::StridedArrayView3D<const char> dst3 = actual.pixels();
    Containers::StridedArrayView2D<const UnsignedByte> dst = Containers::arrayCast<2, const UnsignedByte>(dst3.prefix({dst3.size()[0], dst3.size()[1], 1}));

    /* On the right side the pixels should be non-zero to verify processing got
       done at all */
    CORRADE_VERIFY(dst[0][3] > 0);
    CORRADE_VERIFY(dst[1][3] > 0);

    /* On the left side the pixels should be completely zero, without the right
       side leaking for example due to accidental repeat clamp */
    {
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
        CORRADE_EXPECT_FAIL_IF(GL::Context::current().detectedDriver() >= GL::Context::DetectedDriver::SwiftShader,
            "SwiftShader is trash and wraps around out-of-bounds texelFetch().");
        #endif
        CORRADE_COMPARE(dst[0][0], '\x00');
        CORRADE_COMPARE(dst[1][0], '\x00');
    }
}

#ifndef MAGNUM_TARGET_GLES2
void DistanceFieldGlyphCacheGLTest::setImageEdgeClampArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    /* Like setImageEdgeClamp(), but for texture arrays. As texelFetch() is
       always used in this case, a leak should never happen, nevertheless it's
       good to have it verified. */

    DistanceFieldGlyphCacheArrayGL cache{{8, 4, 1}, {4, 2}, 4};

    /* Make the right edge all white */
    Containers::StridedArrayView2D<UnsignedByte> src = cache.image().pixels<UnsignedByte>()[0];
    src[0][7] = '\xff';
    src[1][7] = '\xff';
    src[2][7] = '\xff';
    src[3][7] = '\xff';
    cache.flushImage({{}, {8, 4}});
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* On GLES processedImage() isn't implemented as it'd mean creating a
       temporary framebuffer. Do it via DebugTools here instead, we cannot
       really verify that the size matches, but at least something. */
    #ifndef MAGNUM_TARGET_GLES
    Image3D actual3 = cache.processedImage();
    /** @todo ugh have slicing on images directly already */
    MutableImageView2D actual{actual3.format(), actual3.size().xy(), actual3.data()};
    #else
    Image2D actual = DebugTools::textureSubImage(cache.texture(), 0, 0, {{}, {4, 2}}, cache.processedFormat());
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The format may be three-component, consider just the first channel */
    Containers::StridedArrayView3D<const char> dst3 = actual.pixels();
    Containers::StridedArrayView2D<const UnsignedByte> dst = Containers::arrayCast<2, const UnsignedByte>(dst3.prefix({dst3.size()[0], dst3.size()[1], 1}));

    /* On the right side the pixels should be non-zero to verify processing got
       done at all */
    CORRADE_VERIFY(dst[0][3] > 0);
    CORRADE_VERIFY(dst[1][3] > 0);

    /* On the left side the pixels should be completely zero, without the right
       side leaking for example due to accidental repeat clamp */
    {
        #ifdef MAGNUM_TARGET_GLES
        CORRADE_EXPECT_FAIL_IF(GL::Context::current().detectedDriver() >= GL::Context::DetectedDriver::SwiftShader,
            "SwiftShader is trash and wraps around out-of-bounds texelFetch().");
        #endif
        CORRADE_COMPARE(dst[0][0], '\x00');
        CORRADE_COMPARE(dst[1][0], '\x00');
    }
}
#endif

const UnsignedByte InputData[]{
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0x00, 0xff, 0x11, 0xee, 0x22, 0xdd, 0x33, 0xcc,
    0x44, 0xbb, 0x55, 0xaa, 0x66, 0x99, 0x77, 0x88,
};

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
                GL::PixelType::UnsignedByte, {8, 4}, InputData});
        else
            cache.setDistanceFieldImage({8, 4}, ImageView2D{PixelFormat::R8Unorm, {8, 4}, InputData});
        CORRADE_IGNORE_DEPRECATED_POP
    } else
    #endif
    {
        cache.setProcessedImage({8, 4}, ImageView2D{PixelFormat::R8Unorm, {8, 4}, InputData});
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
        0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0xff, 0x11, 0xee, 0x22, 0xdd, 0x33, 0xcc,
        0, 0, 0, 0, 0, 0, 0, 0, 0x44, 0xbb, 0x55, 0xaa, 0x66, 0x99, 0x77, 0x88,
    };
    CORRADE_COMPARE_AS(actual,
        (ImageView2D{PixelFormat::R8Unorm, {16, 8}, expected}),
        DebugTools::CompareImage);
}

#ifndef MAGNUM_TARGET_GLES2
void DistanceFieldGlyphCacheGLTest::setProcessedImageArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_array>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_array::string() << "is not supported.");
    #endif

    /* Adapted from GlyphCacheGLTest::setImageArray(), with a difference that a
       single-component is used so the X size is 16 instead of 8 */

    DistanceFieldGlyphCacheArrayGL cache{{64, 32, 4}, {16, 8}, 16};

    /* Clear the texture first, as it'd have random garbage otherwise */
    UnsignedByte zeros[16*8*4]{};
    cache.setProcessedImage({}, ImageView3D{PixelFormat::R8Unorm, {16, 8, 4}, zeros});
    MAGNUM_VERIFY_NO_GL_ERROR();

    cache.setProcessedImage({6, 4, 1}, ImageView3D{PixelFormat::R8Unorm, {8, 2, 2}, InputData});
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* On GLES processedImage() isn't implemented as it'd mean creating a
       temporary framebuffer. Do it via DebugTools here instead, we cannot
       really verify that the size matches, but at least something. */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = cache.processedImage();
    /** @todo ugh have slicing on images directly already, and 3D image
        comparison */
    const std::size_t sliceSize = image.size().xy().product();
    ImageView2D image0{image.format(), image.size().xy(), image.data()};
    ImageView2D image1{image.format(), image.size().xy(), image.data().exceptPrefix(1*sliceSize)};
    ImageView2D image2{image.format(), image.size().xy(), image.data().exceptPrefix(2*sliceSize)};
    ImageView2D image3{image.format(), image.size().xy(), image.data().exceptPrefix(3*sliceSize)};
    #else
    Image2D image0 = DebugTools::textureSubImage(cache.texture(), 0, 0, {{}, {16, 8}}, {PixelFormat::R8Unorm});
    Image2D image1 = DebugTools::textureSubImage(cache.texture(), 0, 1, {{}, {16, 8}}, {PixelFormat::R8Unorm});
    Image2D image2 = DebugTools::textureSubImage(cache.texture(), 0, 2, {{}, {16, 8}}, {PixelFormat::R8Unorm});
    Image2D image3 = DebugTools::textureSubImage(cache.texture(), 0, 3, {{}, {16, 8}}, {PixelFormat::R8Unorm});
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();

    const UnsignedByte expected03[16*8]{};
    const UnsignedByte expected1[]{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0, 0,
        0, 0, 0, 0, 0, 0, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    const UnsignedByte expected2[]{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0x00, 0xff, 0x11, 0xee, 0x22, 0xdd, 0x33, 0xcc, 0, 0,
        0, 0, 0, 0, 0, 0, 0x44, 0xbb, 0x55, 0xaa, 0x66, 0x99, 0x77, 0x88, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    CORRADE_COMPARE_AS(image0,
        (ImageView2D{PixelFormat::R8Unorm, {16, 8}, expected03}),
        DebugTools::CompareImage);
    {
        #ifdef MAGNUM_TARGET_GLES
        CORRADE_EXPECT_FAIL_IF(GL::Context::current().detectedDriver() >= GL::Context::DetectedDriver::SwiftShader,
            "SwiftShader is trash and doesn't implement reading from non-zero array layers.");
        #endif
        CORRADE_COMPARE_AS(image1,
            (ImageView2D{PixelFormat::R8Unorm, {16, 8}, expected1}),
            DebugTools::CompareImage);
        CORRADE_COMPARE_AS(image2,
            (ImageView2D{PixelFormat::R8Unorm, {16, 8}, expected2}),
            DebugTools::CompareImage);
    }
    /* This is broken on SwiftShader too, returning the first layer (or all
       zeros) but since we expect the same as first layer (which is all zeros),
       it passes */
    CORRADE_COMPARE_AS(image3,
        (ImageView2D{PixelFormat::R8Unorm, {16, 8}, expected03}),
        DebugTools::CompareImage);
}
#endif

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
