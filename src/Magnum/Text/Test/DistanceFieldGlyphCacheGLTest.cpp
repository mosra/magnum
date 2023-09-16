/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/StringStl.h> /**< @todo remove once Debug is stream-free */
#include <Corrade/PluginManager/AbstractManager.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/DebugStl.h> /**< @todo remove once Debug is stream-free */
#include <Corrade/Utility/Path.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/Text/DistanceFieldGlyphCache.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct DistanceFieldGlyphCacheGLTest: GL::OpenGLTester {
    explicit DistanceFieldGlyphCacheGLTest();

    void initialize();

    void setImage();

    void setDistanceFieldImage();
    void setDistanceFieldImageOutOfRange();

    PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
};

const struct {
    const char* name;
    Vector2i sourceSize, size, sourceOffset;
    Containers::Size2D offset;
} SetImageData[]{
    {"",
        {256, 256}, {64, 64},
        {}, {}},
    {"upload with offset",
        {512, 384}, {128, 96},
        {256, 128}, {128/4, 256/4}},
};

DistanceFieldGlyphCacheGLTest::DistanceFieldGlyphCacheGLTest() {
    addTests({&DistanceFieldGlyphCacheGLTest::initialize});

    addInstancedTests({&DistanceFieldGlyphCacheGLTest::setImage},
        Containers::arraySize(SetImageData));

    addTests({&DistanceFieldGlyphCacheGLTest::setDistanceFieldImage,
              &DistanceFieldGlyphCacheGLTest::setDistanceFieldImageOutOfRange});

    /* Load the plugin directly from the build tree. Otherwise it's either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void DistanceFieldGlyphCacheGLTest::initialize() {
    DistanceFieldGlyphCache cache{{1024, 2048}, {128, 256}, 16};
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.textureSize(), (Vector2i{1024, 2048}));
    CORRADE_COMPARE(cache.distanceFieldTextureSize(), (Vector2i{128, 256}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{128, 256}));
    #endif
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

    DistanceFieldGlyphCache cache{data.sourceSize, data.size, 32};
    /* Test also uploading under an offset */
    cache.setImage(data.sourceOffset, *inputImage);
    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    Image2D actual = cache.image();
    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_EXPECT_FAIL_IF(!data.sourceOffset.isZero(),
        "The distance field tool is currently broken if a non-zero offset is used.");
    CORRADE_COMPARE_WITH(actual.pixels<UnsignedByte>().exceptPrefix(data.offset),
        Utility::Path::join(TEXTURETOOLS_DISTANCEFIELDGLTEST_DIR, "output.tga"),
        /* Same threshold as in TextureTools DistanceFieldGLTest */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.178f}));
    #else
    CORRADE_SKIP("Skipping image download test as it's not available on GLES.");
    #endif
}

void DistanceFieldGlyphCacheGLTest::setDistanceFieldImage() {
    DistanceFieldGlyphCache cache({64, 32}, {16, 8}, 16);

    /* Pick a format that matches the internal texture format. This is rather
       shitty, TBH. */
    #if !(defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_GLES2))
    const GL::PixelFormat format = GL::PixelFormat::Red;
    #else
    GL::PixelFormat format;
    #ifndef MAGNUM_TARGET_WEBGL
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>()) {
        format = GL::PixelFormat::Red;
    } else
    #endif
    {
        /* Ugh, don't want to bother implementing this */
        CORRADE_SKIP("A three-component input is expected on ES2, skipping due to developer laziness.");
    }
    #endif

    /* Clear the texture first, as it'd have random garbage otherwise */
    UnsignedByte zeros[16*8]{};
    cache.setDistanceFieldImage({}, ImageView2D{format, GL::PixelType::UnsignedByte, {16, 8}, zeros});
    MAGNUM_VERIFY_NO_GL_ERROR();

    UnsignedByte data[]{
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    };
    cache.setDistanceFieldImage({8, 4}, ImageView2D{format, GL::PixelType::UnsignedByte, {8, 4}, data});
    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    Image2D actual = cache.image();
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
    #else
    CORRADE_SKIP("Skipping image download test as it's not available on GLES.");
    #endif
}

void DistanceFieldGlyphCacheGLTest::setDistanceFieldImageOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    DistanceFieldGlyphCache cache{{200, 300}, {100, 200}, 4};

    /* This is fine. Not testing on ES2 as there it would need the complicated
       format logic from above. */
    #if !(defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_GLES2))
    cache.setDistanceFieldImage({80, 175}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    cache.setDistanceFieldImage({81, 175}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    cache.setDistanceFieldImage({80, 176}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    cache.setDistanceFieldImage({-1, 175}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    cache.setDistanceFieldImage({80, -1}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    CORRADE_COMPARE_AS(out.str(),
        "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): Range({81, 175}, {101, 200}) out of range for texture size Vector(100, 200)\n"
        "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): Range({80, 176}, {100, 201}) out of range for texture size Vector(100, 200)\n"
        "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): Range({-1, 175}, {19, 200}) out of range for texture size Vector(100, 200)\n"
        "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): Range({80, -1}, {100, 24}) out of range for texture size Vector(100, 200)\n",
        TestSuite::Compare::String);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::DistanceFieldGlyphCacheGLTest)
