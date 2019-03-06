/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <sstream>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/File.h>

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractFontConverter.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct MagnumFontConverterTest: TestSuite::Tester {
    explicit MagnumFontConverterTest();

    void exportFont();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<Trade::AbstractImageConverter> _imageConverterManager{"nonexistent"};
    PluginManager::Manager<AbstractFontConverter> _fontConverterManager{"nonexistent"};
    PluginManager::Manager<Trade::AbstractImporter> _importerManager{"nonexistent"};
};

MagnumFontConverterTest::MagnumFontConverterTest() {
    addTests({&MagnumFontConverterTest::exportFont});

    /* Load the plugins directly from the build tree. Otherwise they are static
       and already loaded. */
    #if defined(TGAIMAGECONVERTER_PLUGIN_FILENAME) && defined(MAGNUMFONTCONVERTER_PLUGIN_FILENAME)
    CORRADE_INTERNAL_ASSERT(_imageConverterManager.load(TGAIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    CORRADE_INTERNAL_ASSERT(_fontConverterManager.load(MAGNUMFONTCONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_importerManager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void MagnumFontConverterTest::exportFont() {
    /* Remove previously created files */
    Utility::Directory::rm(Utility::Directory::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.conf"));
    Utility::Directory::rm(Utility::Directory::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.tga"));

    /* Fake font with fake cache */
    class FakeFont: public Text::AbstractFont {
        public:
            explicit FakeFont(): _opened(false) {}

        private:
            void doClose() { _opened = false; }
            bool doIsOpened() const { return _opened; }
            Metrics doOpenFile(const std::string&, Float) {
                _opened = true;
                return {16.0f, 25.0f, -10.0f, 39.7333f};
            }
            Features doFeatures() const { return {}; }
            Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) { return nullptr; }

            UnsignedInt doGlyphId(const char32_t character) {
                switch(character) {
                    case 'W': return 2;
                    case 'e': return 1;
                }

                return 0;
            }

            Vector2 doGlyphAdvance(const UnsignedInt glyph) {
                switch(glyph) {
                    case 0: return {8, 0};
                    case 1: return {12, 0};
                    case 2: return {23, 0};
                }

                CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
            }

            bool _opened;
    } font;
    font.openFile({}, {});

    /* Create fake cache */
    struct MyCache: AbstractGlyphCache {
        explicit MyCache(): AbstractGlyphCache{Vector2i{1536}, Vector2i{24}} {}

        GlyphCacheFeatures doFeatures() const override { return GlyphCacheFeature::ImageDownload; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
        Image2D doImage() override {
            return Image2D{PixelFormat::R8Unorm, Vector2i{256}, Containers::Array<char>{Containers::ValueInit, 256*256}};
        }
    } cache;
    cache.insert(font.glyphId(U'W'), {25, 34}, {{0, 8}, {16, 128}});
    cache.insert(font.glyphId(U'e'), {25, 12}, {{16, 4}, {64, 32}});

    /* Convert the file */
    Containers::Pointer<AbstractFontConverter> converter = _fontConverterManager.instantiate("MagnumFontConverter");
    converter->exportFontToFile(font, cache, Utility::Directory::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font"), "Wave");

    /* Verify font parameters */
    /** @todo This might behave differently elsewhere due to unspecified order of glyphs in cache */
    CORRADE_COMPARE_AS(Utility::Directory::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.conf"),
                       Utility::Directory::join(MAGNUMFONT_TEST_DIR, "font.conf"),
                       TestSuite::Compare::File);

    if(!(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, not testing glyph cache contents");

    /* Verify font image, no need to test image contents, as the image is
       garbage anyway */
    Containers::Pointer<Trade::AbstractImporter> importer = _importerManager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.tga")));
    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), Vector2i(256));
    CORRADE_COMPARE(image->format(), PixelFormat::R8Unorm);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::MagnumFontConverterTest)
