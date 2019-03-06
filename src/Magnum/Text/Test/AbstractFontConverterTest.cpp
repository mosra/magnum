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

#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/FileToString.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractFontConverter.h"
#include "Magnum/Text/AbstractGlyphCache.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct AbstractFontConverterTest: TestSuite::Tester {
    explicit AbstractFontConverterTest();

    void convertGlyphs();

    void exportFontToSingleData();
    void exportFontToFile();

    void exportGlyphCacheToSingleData();
    void exportGlyphCacheToFile();

    void importGlyphCacheFromSingleData();
    void importGlyphCacheFromFile();
};

AbstractFontConverterTest::AbstractFontConverterTest() {
    addTests({&AbstractFontConverterTest::convertGlyphs,

              &AbstractFontConverterTest::exportFontToSingleData,
              &AbstractFontConverterTest::exportFontToFile,

              &AbstractFontConverterTest::exportGlyphCacheToSingleData,
              &AbstractFontConverterTest::exportGlyphCacheToFile,

              &AbstractFontConverterTest::importGlyphCacheFromSingleData,
              &AbstractFontConverterTest::importGlyphCacheFromFile});

    /* Create testing dir */
    Utility::Directory::mkpath(TEXT_TEST_OUTPUT_DIR);
}

struct DummyFont: AbstractFont {
    Features doFeatures() const override { return {}; }
    bool doIsOpened() const override { return false; }
    void doClose() override {}

    UnsignedInt doGlyphId(char32_t) override { return {}; }
    Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
    Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override {
        return nullptr;
    }
} dummyFont;

struct DummyGlyphCache: AbstractGlyphCache {
    using AbstractGlyphCache::AbstractGlyphCache;

    GlyphCacheFeatures doFeatures() const override { return {}; }
    void doSetImage(const Vector2i&, const ImageView2D&) override {}
} dummyGlyphCache{{}};

void AbstractFontConverterTest::convertGlyphs() {
    class GlyphExporter: public AbstractFontConverter {
        public:
            /* GCC 4.8 apparently can't handle {} here */
            GlyphExporter(std::u32string& characters): _characters(characters) {}

        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportFont; }

            Containers::Array<char> doExportFontToSingleData(AbstractFont&, AbstractGlyphCache&, const std::u32string& characters) const override {
                _characters = characters;
                return nullptr;
            }

            std::u32string& _characters;
    };

    std::u32string characters;
    GlyphExporter exporter(characters);
    exporter.exportFontToSingleData(dummyFont, dummyGlyphCache, "abC01a0 ");
    CORRADE_COMPARE(characters, U" 01Cab");
}

void AbstractFontConverterTest::exportFontToSingleData() {
    class SingleDataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportFont; }

            Containers::Array<char> doExportFontToSingleData(AbstractFont&, AbstractGlyphCache&, const std::u32string&) const override {
                Containers::Array<char> data(1);
                data[0] = '\xee';
                return data;
            }
    };

    /* doExportFontToData() should call doExportFontToSingleData() */
    SingleDataExporter exporter;
    auto ret = exporter.exportFontToData(dummyFont, dummyGlyphCache, "font.out", {});
    CORRADE_COMPARE(ret.size(), 1);
    CORRADE_COMPARE(ret[0].first, "font.out");
    CORRADE_COMPARE(ret[0].second.size(), 1);
    CORRADE_COMPARE(ret[0].second[0], '\xee');
}

void AbstractFontConverterTest::exportFontToFile() {
    class DataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportFont|Feature::MultiFile; }

            std::vector<std::pair<std::string, Containers::Array<char>>> doExportFontToData(AbstractFont&, AbstractGlyphCache&, const std::string& filename, const std::u32string&) const override {
                /* Why the hell GCC 4.9 fails to do proper move so I need to
                   work around that this ugly way?! */
                std::vector<std::pair<std::string, Containers::Array<char>>> ret;
                ret.emplace_back(filename, Containers::Array<char>{Containers::InPlaceInit, {'\xf0'}});
                ret.emplace_back(filename + ".data", Containers::Array<char>{Containers::InPlaceInit, {'\xfe', '\xed'}});
                return ret;
            }
    };

    /* Remove previous files */
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out"));
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out.data"));

    /* doExportToFile() should call doExportToData() */
    DataExporter exporter;
    bool exported = exporter.exportFontToFile(dummyFont, dummyGlyphCache, Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out"), {});
    CORRADE_VERIFY(exported);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out"),
                       "\xf0", TestSuite::Compare::FileToString);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out.data"),
                       "\xfe\xed", TestSuite::Compare::FileToString);
}

void AbstractFontConverterTest::exportGlyphCacheToSingleData() {
    struct: Text::AbstractFontConverter {
        Features doFeatures() const override { return Feature::ConvertData|Feature::ExportGlyphCache; }

        Containers::Array<char> doExportGlyphCacheToSingleData(AbstractGlyphCache&) const override {
            return Containers::Array<char>{Containers::InPlaceInit, {'\xee'}};
        }
    } exporter;

    /* doExportGlyphCacheToData() should call doExportGlyphCacheToSingleData() */
    auto ret = exporter.exportGlyphCacheToData(dummyGlyphCache, "font.out");
    CORRADE_COMPARE(ret.size(), 1);
    CORRADE_COMPARE(ret[0].first, "font.out");
    CORRADE_COMPARE_AS(ret[0].second,
        (Containers::Array<char>{Containers::InPlaceInit, {'\xee'}}),
        TestSuite::Compare::Container);
}

void AbstractFontConverterTest::exportGlyphCacheToFile() {
    class DataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportGlyphCache|Feature::MultiFile; }

            std::vector<std::pair<std::string, Containers::Array<char>>> doExportGlyphCacheToData(AbstractGlyphCache&, const std::string& filename) const override {
                /* Why the hell GCC 4.9 fails to do proper move so I need to
                   work around that this ugly way?! */
                std::vector<std::pair<std::string, Containers::Array<char>>> ret;
                ret.emplace_back(filename, Containers::Array<char>{Containers::InPlaceInit, {'\xf0'}});
                ret.emplace_back(filename + ".data", Containers::Array<char>{Containers::InPlaceInit, {'\xfe', '\xed'}});
                return ret;
            }
    };

    /* Remove previous files */
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out"));
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out.data"));

    /* doExportGlyphCacheToFile() should call doExportGlyphCacheToData() */
    DataExporter exporter;
    bool exported = exporter.exportGlyphCacheToFile(dummyGlyphCache, Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out"));
    CORRADE_VERIFY(exported);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out"),
                       "\xf0", TestSuite::Compare::FileToString);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out.data"),
                       "\xfe\xed", TestSuite::Compare::FileToString);
}

class SingleGlyphCacheDataImporter: public Text::AbstractFontConverter {
    private:
        Features doFeatures() const override { return Feature::ConvertData|Feature::ImportGlyphCache; }

        Containers::Pointer<AbstractGlyphCache> doImportGlyphCacheFromSingleData(const Containers::ArrayView<const char> data) const override {
            if(data.size() == 1 && data[0] == '\xa5')
                return Containers::pointer(new DummyGlyphCache{{123, 345}});
            return nullptr;
        }
};

void AbstractFontConverterTest::importGlyphCacheFromSingleData() {
    /* doImportFromData() should call doImportFromSingleData() */
    SingleGlyphCacheDataImporter importer;
    const char data[] = {'\xa5'};
    Containers::Pointer<AbstractGlyphCache> cache = importer.importGlyphCacheFromData({{{}, data}});
    CORRADE_COMPARE(cache->textureSize(), (Vector2i{123, 345}));
}

void AbstractFontConverterTest::importGlyphCacheFromFile() {
    /* doImportFromFile() should call doImportFromSingleData() */
    SingleGlyphCacheDataImporter importer;
    Containers::Pointer<AbstractGlyphCache> cache = importer.importGlyphCacheFromFile(Utility::Directory::join(TEXT_TEST_DIR, "data.bin"));
    CORRADE_COMPARE(cache->textureSize(), (Vector2i{123, 345}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractFontConverterTest)
