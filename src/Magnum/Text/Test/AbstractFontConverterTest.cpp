/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Utility/Directory.h>

#include "Magnum/Text/AbstractFontConverter.h"
#include "Magnum/Text/GlyphCache.h"

#include "testConfigure.h"

namespace Magnum { namespace Text { namespace Test {

class AbstractFontConverterTest: public TestSuite::Tester {
    public:
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
}

void AbstractFontConverterTest::convertGlyphs() {
    class GlyphExporter: public AbstractFontConverter {
        public:
            #ifndef __MINGW32__
            GlyphExporter(std::u32string& characters): characters(characters) {}
            #else
            GlyphExporter(std::vector<char32_t>& characters): characters(characters) {}
            #endif

        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportFont; }

            #ifndef __MINGW32__
            Containers::Array<unsigned char> doExportFontToSingleData(AbstractFont&, GlyphCache&, const std::u32string& characters) const override
            #else
            Containers::Array<unsigned char> doExportFontToSingleData(AbstractFont&, GlyphCache&, const std::vector<char32_t>& characters) const override
            #endif
            {
                this->characters = characters;
                return nullptr;
            }

            #ifndef __MINGW32__
            std::u32string& characters;
            #else
            std::vector<char32_t>& characters;
            #endif
    };

    #ifndef __MINGW32__
    std::u32string characters;
    #else
    std::vector<char32_t> characters;
    #endif
    GlyphExporter exporter(characters);
    exporter.exportFontToSingleData(*static_cast<AbstractFont*>(nullptr), *static_cast<GlyphCache*>(nullptr), "abC01a0 ");
    #ifndef __MINGW32__
    CORRADE_COMPARE(characters, U" 01Cab");
    #else
    CORRADE_COMPARE(characters, (std::vector<char32_t>{
            U' ', U'0', U'1', U'C', U'a', U'b'}));
    #endif
}

void AbstractFontConverterTest::exportFontToSingleData() {
    class SingleDataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportFont; }

            #ifndef __MINGW32__
            Containers::Array<unsigned char> doExportFontToSingleData(AbstractFont&, GlyphCache&, const std::u32string&) const override
            #else
            Containers::Array<unsigned char> doExportFontToSingleData(AbstractFont&, GlyphCache&, const std::vector<char32_t>&) const override
            #endif
            {
                Containers::Array<unsigned char> data(1);
                data[0] = 0xee;
                return std::move(data);
            }
    };

    /* doExportFontToData() should call doExportFontToSingleData() */
    SingleDataExporter exporter;
    auto ret = exporter.exportFontToData(*static_cast<AbstractFont*>(nullptr), *static_cast<GlyphCache*>(nullptr), "font.out", {});
    CORRADE_COMPARE(ret.size(), 1);
    CORRADE_COMPARE(ret[0].first, "font.out");
    CORRADE_COMPARE(ret[0].second.size(), 1);
    CORRADE_COMPARE(ret[0].second[0], 0xee);
}

void AbstractFontConverterTest::exportFontToFile() {
    class DataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportFont|Feature::MultiFile; }

            #ifndef __MINGW32__
            std::vector<std::pair<std::string, Containers::Array<unsigned char>>> doExportFontToData(AbstractFont&, GlyphCache&, const std::string& filename, const std::u32string&) const override
            #else
            std::vector<std::pair<std::string, Containers::Array<unsigned char>>> doExportFontToData(AbstractFont&, GlyphCache&, const std::string& filename, const std::vector<char32_t>&) const override
            #endif
            {
                Containers::Array<unsigned char> file(1);
                file[0] = 0xf0;

                Containers::Array<unsigned char> data(2);
                data[0] = 0xfe;
                data[1] = 0xed;

                std::vector<std::pair<std::string, Containers::Array<unsigned char>>> out;
                out.emplace_back(filename, std::move(file));
                out.emplace_back(filename + ".data", std::move(data));
                return std::move(out);
            }
    };

    /* Remove previous files */
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out"));
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out.data"));

    /* doExportToFile() should call doExportToData() */
    DataExporter exporter;
    bool exported = exporter.exportFontToFile(*static_cast<AbstractFont*>(nullptr), *static_cast<GlyphCache*>(nullptr), Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out"), {});
    CORRADE_VERIFY(exported);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out"),
                       "\xf0", TestSuite::Compare::FileToString);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out.data"),
                       "\xfe\xed", TestSuite::Compare::FileToString);
}

void AbstractFontConverterTest::exportGlyphCacheToSingleData() {
    class SingleDataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportGlyphCache; }

            Containers::Array<unsigned char> doExportGlyphCacheToSingleData(GlyphCache&) const override {
                Containers::Array<unsigned char> data(1);
                data[0] = 0xee;
                return std::move(data);
            }
    };

    /* doExportGlyphCacheToData() should call doExportGlyphCacheToSingleData() */
    SingleDataExporter exporter;
    auto ret = exporter.exportGlyphCacheToData(*static_cast<GlyphCache*>(nullptr), "font.out");
    CORRADE_COMPARE(ret.size(), 1);
    CORRADE_COMPARE(ret[0].first, "font.out");
    CORRADE_COMPARE(ret[0].second.size(), 1);
    CORRADE_COMPARE(ret[0].second[0], 0xee);
}

void AbstractFontConverterTest::exportGlyphCacheToFile() {
    class DataExporter: public Text::AbstractFontConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData|Feature::ExportGlyphCache|Feature::MultiFile; }

            std::vector<std::pair<std::string, Containers::Array<unsigned char>>> doExportGlyphCacheToData(GlyphCache&, const std::string& filename) const override {
                Containers::Array<unsigned char> file(1);
                file[0] = 0xf0;

                Containers::Array<unsigned char> data(2);
                data[0] = 0xfe;
                data[1] = 0xed;

                std::vector<std::pair<std::string, Containers::Array<unsigned char>>> out;
                out.emplace_back(filename, std::move(file));
                out.emplace_back(filename + ".data", std::move(data));
                return std::move(out);
            }
    };

    /* Remove previous files */
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out"));
    Utility::Directory::rm(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out.data"));

    /* doExportGlyphCacheToFile() should call doExportGlyphCacheToData() */
    DataExporter exporter;
    bool exported = exporter.exportGlyphCacheToFile(*static_cast<GlyphCache*>(nullptr), Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out"));
    CORRADE_VERIFY(exported);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out"),
                       "\xf0", TestSuite::Compare::FileToString);
    CORRADE_COMPARE_AS(Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "glyphcache.out.data"),
                       "\xfe\xed", TestSuite::Compare::FileToString);
}

namespace {

class SingleGlyphCacheDataImporter: public Text::AbstractFontConverter {
    private:
        Features doFeatures() const override { return Feature::ConvertData|Feature::ImportGlyphCache; }

        std::unique_ptr<GlyphCache> doImportGlyphCacheFromSingleData(const Containers::ArrayReference<const unsigned char> data) const override {
            if(data.size() == 1 && data[0] == 0xa5)
                return std::unique_ptr<GlyphCache>(reinterpret_cast<GlyphCache*>(0xdeadbeef));
            return nullptr;
        }
};

}

void AbstractFontConverterTest::importGlyphCacheFromSingleData() {
    /* doImportFromData() should call doImportFromSingleData() */
    SingleGlyphCacheDataImporter importer;
    const unsigned char data[] = {0xa5};
    std::unique_ptr<GlyphCache> cache = importer.importGlyphCacheFromData({{{}, data}});
    CORRADE_COMPARE(cache.get(), reinterpret_cast<GlyphCache*>(0xdeadbeef));

    /* The pointer is invalid, avoid deletion */
    cache.release();
}

void AbstractFontConverterTest::importGlyphCacheFromFile() {
    /* doImportFromFile() should call doImportFromSingleData() */
    SingleGlyphCacheDataImporter importer;
    std::unique_ptr<GlyphCache> cache = importer.importGlyphCacheFromFile(Utility::Directory::join(TEXT_TEST_DIR, "data.bin"));
    CORRADE_COMPARE(cache.get(), reinterpret_cast<GlyphCache*>(0xdeadbeef));

    /* The pointer is invalid, avoid deletion */
    cache.release();
}

}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractFontConverterTest)
