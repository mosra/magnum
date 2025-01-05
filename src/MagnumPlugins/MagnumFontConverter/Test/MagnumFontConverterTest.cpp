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

#include <string> /** @todo remove once AbstractFontConverter is STL-free */
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once AbstractFontConverter is STL-free */
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/File.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractFontConverter.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct MagnumFontConverterTest: TestSuite::Tester {
    explicit MagnumFontConverterTest();

    void exportFont();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void exportFontOldStyleCache();
    #endif
    void exportFontEmptyCache();
    void exportFontImageProcessingGlyphCache();
    void exportFontImageProcessingGlyphCacheNoDownload();

    void exportFontArrayCache();
    void exportFontNotFoundInCache();
    void exportFontImageConversionFailed();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<Trade::AbstractImageConverter> _imageConverterManager{"nonexistent"};
    PluginManager::Manager<AbstractFontConverter> _fontConverterManager{"nonexistent"};
    PluginManager::Manager<Trade::AbstractImporter> _importerManager{"nonexistent"};
};

MagnumFontConverterTest::MagnumFontConverterTest() {
    addTests({&MagnumFontConverterTest::exportFont,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &MagnumFontConverterTest::exportFontOldStyleCache,
              #endif
              &MagnumFontConverterTest::exportFontEmptyCache,
              &MagnumFontConverterTest::exportFontImageProcessingGlyphCache,
              &MagnumFontConverterTest::exportFontImageProcessingGlyphCacheNoDownload,

              &MagnumFontConverterTest::exportFontArrayCache,
              &MagnumFontConverterTest::exportFontNotFoundInCache,
              &MagnumFontConverterTest::exportFontImageConversionFailed});

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree. */
    _fontConverterManager.registerExternalManager(_imageConverterManager);
    #if defined(TGAIMAGECONVERTER_PLUGIN_FILENAME) && defined(MAGNUMFONTCONVERTER_PLUGIN_FILENAME)
    CORRADE_INTERNAL_ASSERT_OUTPUT(_imageConverterManager.load(TGAIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    CORRADE_INTERNAL_ASSERT_OUTPUT(_fontConverterManager.load(MAGNUMFONTCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    /* Create the output directory if it doesn't exist yet */
    CORRADE_INTERNAL_ASSERT_OUTPUT(Utility::Path::make(MAGNUMFONTCONVERTER_TEST_WRITE_DIR));
}

class MyFont: public Text::AbstractFont {
    private:
        void doClose() override { _opened = false; }
        bool doIsOpened() const override { return _opened; }
        Properties doOpenFile(Containers::StringView, Float) override {
            _opened = true;
            return {16.0f, 25.0f, -10.0f, 39.7333f, 4};
        }
        FontFeatures doFeatures() const override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return nullptr; }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>& characters, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            for(std::size_t i = 0; i != characters.size(); ++i) {
                switch(characters[i]) {
                    case U'W':
                        glyphs[i] = 2;
                        break;
                    case U'e':
                        glyphs[i] = 1;
                        break;
                    /* MSVC (but not clang-cl) doesn't support UTF-8 in
                       char32_t literals but it does it regular strings. Still
                       a problem in MSVC 2022, what a trash fire, can't you
                       just give up on those codepage insanities already,
                       ffs?! */
                    #if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG)
                    case U'\u011B':
                    #else
                    case U'ě':
                    #endif
                        glyphs[i] = 3;
                        break;
                    default:
                        glyphs[i] = 0;
                }
            }
        }

        Vector2 doGlyphSize(UnsignedInt) override { return {}; }

        Vector2 doGlyphAdvance(const UnsignedInt glyph) override {
            switch(glyph) {
                case 0: return {8, 0};
                case 1:
                case 3: /* e and ě have the same advance */
                    return {12, 0};
                case 2: return {23, 0};
            }

            CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        bool _opened = false;
};

void MagnumFontConverterTest::exportFont() {
    Containers::String confFilename = Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.conf");
    Containers::String tgaFilename = Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.tga");
    /* Remove previously created files */
    if(Utility::Path::exists(confFilename))
        CORRADE_VERIFY(Utility::Path::remove(confFilename));
    if(Utility::Path::exists(tgaFilename))
        CORRADE_VERIFY(Utility::Path::remove(tgaFilename));

    MyFont font;
    font.openFile({}, {});

    /* Create a cache. Two fonts, only the second one should be added. */
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{PixelFormat::R8Unorm, {128, 64}, {16, 8}};

    /* Override the not found glyph to be in bounds as well */
    cache.setInvalidGlyph({}, {{16, 8}, {16, 8}});

    /* This font and all its glyphs should be skipped */
    UnsignedInt unusedFontId = cache.addFont(56);
    cache.addGlyph(unusedFontId, 33, {16, 20}, {{60, 40}, {80, 50}});

    UnsignedInt fontId = cache.addFont(25, &font);
    cache.addGlyph(fontId, font.glyphId(U'W'), {25, 34}, {{16, 12}, {24, 56}});
    cache.addGlyph(fontId, font.glyphId(U'e'), {25, 12}, {{36, 8}, {112, 40}});
    /* ě has deliberately the same glyph data as e */
    cache.addGlyph(fontId, font.glyphId(
        /* MSVC (but not clang-cl) doesn't support UTF-8 in char32_t literals
           but it does it regular strings. Still a problem in MSVC 2022, what a
           trash fire, can't you just give up on those codepage insanities
           already, ffs?! */
        #if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG)
        U'\u011B'
        #else
        U'ě'
        #endif
    ), {25, 12}, {{36, 8}, {112, 40}});

    /* Set the cache image to some non-trivial contents. Compared to the
       exportFontImageProcessingGlyphCache() test the image is 16x bigger, so
       do some fancy expansion there. */
    const char pixels[]{
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v'
    };
    for(std::size_t y = 0; y != 16; ++y)
        for(std::size_t x = 0; x != 16; ++x)
            Utility::copy(
                Containers::StridedArrayView2D<const char>{pixels, {4, 8}},
                cache.image().pixels<char>()[0].exceptPrefix({y, x}).every({16, 16}));

    /* Convert the file */
    Containers::Pointer<AbstractFontConverter> converter = _fontConverterManager.instantiate("MagnumFontConverter");
    CORRADE_VERIFY(converter->exportFontToFile(font, cache, Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font"), "Waveě"));

    /* Verify font parameters */
    CORRADE_COMPARE_AS(confFilename,
        Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.conf"),
        TestSuite::Compare::File);

    if(!(_importerManager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found, not testing glyph cache contents");

    /* Verify font image */
    CORRADE_COMPARE_WITH(tgaFilename,
        Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.tga"),
        DebugTools::CompareImageFile{_importerManager});
}

#ifdef MAGNUM_BUILD_DEPRECATED
void MagnumFontConverterTest::exportFontOldStyleCache() {
    /* Like exportFont(), but using the deprecated cache APIs to verify that
       the cache contents are still copied the same */

    Containers::String confFilename = Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.conf");
    Containers::String tgaFilename = Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.tga");
    /* Remove previously created files */
    if(Utility::Path::exists(confFilename))
        CORRADE_VERIFY(Utility::Path::remove(confFilename));
    if(Utility::Path::exists(tgaFilename))
        CORRADE_VERIFY(Utility::Path::remove(tgaFilename));

    MyFont font;
    font.openFile({}, {});

    /* Create a cache the old way, i.e. insert() which results in exactly one
       font added and no association with a pointer */
    CORRADE_IGNORE_DEPRECATED_PUSH
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{{128, 64}, {16, 8}};
    /* Override the not found glyph to be in bounds as well */
    cache.insert(0, {}, {{16, 8}, {16, 8}});
    cache.insert(font.glyphId(U'W'), {25, 34}, {{16, 12}, {24, 56}});
    cache.insert(font.glyphId(U'e'), {25, 12}, {{36, 8}, {112, 40}});
    /* ě has deliberately the same glyph data as e */
    cache.insert(font.glyphId(
        /* MSVC (but not clang-cl) doesn't support UTF-8 in char32_t literals
           but it does it regular strings. Still a problem in MSVC 2022, what a
           trash fire, can't you just give up on those codepage insanities
           already, ffs?! */
        #if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG)
        U'\u011B'
        #else
        U'ě'
        #endif
    ), {25, 12}, {{36, 8}, {112, 40}});
    CORRADE_IGNORE_DEPRECATED_POP

    /* Set the cache image to some non-trivial contents. There's no "old way"
       to do this, also compared to the exportFontImageProcessingGlyphCache()
       test the image is 16x bigger, so do some fancy expansion there. */
    const char pixels[]{
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v'
    };
    for(std::size_t y = 0; y != 16; ++y)
        for(std::size_t x = 0; x != 16; ++x)
            Utility::copy(
                Containers::StridedArrayView2D<const char>{pixels, {4, 8}},
                cache.image().pixels<char>()[0].exceptPrefix({y, x}).every({16, 16}));

    /* Convert the file */
    Containers::Pointer<AbstractFontConverter> converter = _fontConverterManager.instantiate("MagnumFontConverter");
    CORRADE_VERIFY(converter->exportFontToFile(font, cache, Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font"), "Waveě"));

    /* Verify font parameters */
    CORRADE_COMPARE_AS(confFilename,
        Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.conf"),
        TestSuite::Compare::File);

    if(!(_importerManager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found, not testing glyph cache contents");

    /* Verify font image */
    CORRADE_COMPARE_WITH(tgaFilename,
        Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.tga"),
        DebugTools::CompareImageFile{_importerManager});
}
#endif

void MagnumFontConverterTest::exportFontEmptyCache() {
    Containers::String confFilename = Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font-empty-cache.conf");
    Containers::String tgaFilename = Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font-empty-cache.tga");
    /* Remove previously created files */
    if(Utility::Path::exists(confFilename))
        CORRADE_VERIFY(Utility::Path::remove(confFilename));
    if(Utility::Path::exists(tgaFilename))
        CORRADE_VERIFY(Utility::Path::remove(tgaFilename));

    MyFont font;
    font.openFile({}, {});

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    /* Default padding is 1 to avoid artifacts, set that to 0 to simplify */
    } cache{PixelFormat::R8Unorm, {8, 4}, {}};

    /* Associate the font with the cache. The case where it's not even that is
       tested in exportFontNotFoundInCache() below. */
    cache.addFont(0, &font);

    /* Convert the file */
    Containers::Pointer<AbstractFontConverter> converter = _fontConverterManager.instantiate("MagnumFontConverter");
    CORRADE_VERIFY(converter->exportFontToFile(font, cache, Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font-empty-cache"), "Wave"));

    /* Verify font parameters */
    CORRADE_COMPARE_AS(confFilename,
        Utility::Path::join(MAGNUMFONTCONVERTER_TEST_DIR, "font-empty-cache.conf"),
        TestSuite::Compare::File);

    if(!(_importerManager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found, not testing glyph cache contents");

    /* Verify font image */
    CORRADE_COMPARE_WITH(tgaFilename,
        Utility::Path::join(MAGNUMFONTCONVERTER_TEST_DIR, "font-empty-cache.tga"),
        DebugTools::CompareImageFile{_importerManager});
}

void MagnumFontConverterTest::exportFontImageProcessingGlyphCache() {
    /* Like exportFont(), but the image is processed to a 16x smaller one. The
       rest stays the same, i.e. the offsets and sizes are still relative to
       the original 128x64 image. */

    Containers::String confFilename = Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font-processed.conf");
    Containers::String tgaFilename = Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font-processed.tga");
    /* Remove previously created files */
    if(Utility::Path::exists(confFilename))
        CORRADE_VERIFY(Utility::Path::remove(confFilename));
    if(Utility::Path::exists(tgaFilename))
        CORRADE_VERIFY(Utility::Path::remove(tgaFilename));

    MyFont font;
    font.openFile({}, {});

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return GlyphCacheFeature::ProcessedImageDownload; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
        Image3D doProcessedImage() override {
            return Image3D{PixelFormat::R8Unorm, {8, 4, 1}, Containers::Array<char>{InPlaceInit, {
                '0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                'o', 'p', 'q', 'r', 's', 't', 'u', 'v'
            }}};
        }
    } cache{PixelFormat::R8Unorm, {128, 64}, {16, 8}};
    /* Override the not found glyph to be in bounds as well */
    cache.setInvalidGlyph({}, {{16, 8}, {16, 8}});
    UnsignedInt fontId = cache.addFont(25, &font);
    cache.addGlyph(fontId, font.glyphId(U'W'), {25, 34}, {{16, 12}, {24, 56}});
    cache.addGlyph(fontId, font.glyphId(U'e'), {25, 12}, {{36, 8}, {112, 40}});
    /* ě has deliberately the same glyph data as e */
    cache.addGlyph(fontId, font.glyphId(
        /* MSVC (but not clang-cl) doesn't support UTF-8 in char32_t literals
           but it does it regular strings. Still a problem in MSVC 2022, what a
           trash fire, can't you just give up on those codepage insanities
           already, ffs?! */
        #if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG)
        U'\u011B'
        #else
        U'ě'
        #endif
    ), {25, 12}, {{36, 8}, {112, 40}});

    /* Convert the file */
    Containers::Pointer<AbstractFontConverter> converter = _fontConverterManager.instantiate("MagnumFontConverter");
    CORRADE_VERIFY(converter->exportFontToFile(font, cache, Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font-processed"), "Waveě"));

    /* Verify font parameters */
    CORRADE_COMPARE_AS(confFilename,
        Utility::Path::join(MAGNUMFONT_TEST_DIR, "font-processed.conf"),
        TestSuite::Compare::File);

    if(!(_importerManager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found, not testing glyph cache contents");

    /* Verify font image */
    CORRADE_COMPARE_WITH(tgaFilename,
        Utility::Path::join(MAGNUMFONT_TEST_DIR, "font-processed.tga"),
        DebugTools::CompareImageFile{_importerManager});
}

void MagnumFontConverterTest::exportFontImageProcessingGlyphCacheNoDownload() {
    struct: AbstractFont {
        /* Supports neither file nor data opening */
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return nullptr; }
    } font;

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return GlyphCacheFeature::ImageProcessing; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{PixelFormat::R8Unorm, {100, 100}};

    Containers::Pointer<AbstractFontConverter> converter = _fontConverterManager.instantiate("MagnumFontConverter");

    Containers::String out;
    Error redirectError{&out};
    converter->exportFontToFile(font, cache, Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font"), "Wave");
    CORRADE_COMPARE(out, "Text::MagnumFontConverter::exportFontToData(): glyph cache has image processing but doesn't support image download\n");
}

void MagnumFontConverterTest::exportFontArrayCache() {
    struct: AbstractFont {
        /* Supports neither file nor data opening */
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return nullptr; }
    } font;

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{PixelFormat::R8Unorm, {100, 100, 2}};

    cache.addFont(15, &font);

    Containers::Pointer<AbstractFontConverter> converter = _fontConverterManager.instantiate("MagnumFontConverter");

    Containers::String out;
    Error redirectError{&out};
    converter->exportFontToFile(font, cache, Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font"), "Wave");
    CORRADE_COMPARE(out, "Text::MagnumFontConverter::exportFontToData(): exporting array glyph caches is not supported\n");
}

void MagnumFontConverterTest::exportFontNotFoundInCache() {
    struct: AbstractFont {
        /* Supports neither file nor data opening */
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>&) override {}
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return nullptr; }
    } font1, font2;

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{PixelFormat::R8Unorm, {100, 100}};

    cache.addFont(15, &font2);
    cache.addFont(33);

    Containers::Pointer<AbstractFontConverter> converter = _fontConverterManager.instantiate("MagnumFontConverter");

    Containers::String out;
    Error redirectError{&out};
    converter->exportFontToFile(font1, cache, Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font"), "Wave");
    CORRADE_COMPARE(out, "Text::MagnumFontConverter::exportFontToData(): font not found among 2 fonts in passed glyph cache\n");
}

void MagnumFontConverterTest::exportFontImageConversionFailed() {
    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        void doClose() override { _opened = false; }
        bool doIsOpened() const override { return _opened; }
        Properties doOpenFile(Containers::StringView, Float) override {
            _opened = true;
            return {16.0f, 25.0f, -10.0f, 39.7333f, 3};
        }

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            for(UnsignedInt& i: glyphs)
                i = 0;
        }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractShaper> doCreateShaper() override { return nullptr; }

        private:
            bool _opened;
    } font;

    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{PixelFormat::R32F, {100, 100}};

    font.openFile({}, 0.0f);

    cache.addFont(15, &font);

    Containers::Pointer<AbstractFontConverter> converter = _fontConverterManager.instantiate("MagnumFontConverter");

    Containers::String out;
    Error redirectError{&out};
    converter->exportFontToFile(font, cache, Utility::Path::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font"), "Wave");
    CORRADE_COMPARE(out,
        "Trade::TgaImageConverter::convertToData(): unsupported pixel format PixelFormat::R32F\n"
        "Text::MagnumFontConverter::exportFontToData(): cannot create a TGA image\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::MagnumFontConverterTest)
