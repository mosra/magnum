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

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/FileToString.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
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
    void exportFontToSingleDataNotSupported();
    void exportFontToSingleDataNotImplemented();
    void exportFontToSingleDataNotSingleFile();
    void exportFontToData();
    void exportFontToDataNotSupported();
    void exportFontToDataNotImplemented();
    void exportFontToDataThroughSingleData();
    void exportFontToDataThroughSingleDataFailed();
    void exportFontToFile();
    void exportFontToFileNotSupported();
    void exportFontToFileNotImplemented();
    void exportFontToFileThroughData();
    void exportFontToFileThroughDataFailed();
    void exportFontToFileThroughDataNotWritable();

    void exportGlyphCacheToSingleData();
    void exportGlyphCacheToSingleDataNotSupported();
    void exportGlyphCacheToSingleDataNotImplemented();
    void exportGlyphCacheToSingleDataNotSingleFile();
    void exportGlyphCacheToData();
    void exportGlyphCacheToDataNotSupported();
    void exportGlyphCacheToDataNotImplemented();
    void exportGlyphCacheToDataThroughSingleData();
    void exportGlyphCacheToDataThroughSingleDataFailed();
    void exportGlyphCacheToFile();
    void exportGlyphCacheToFileNotSupported();
    void exportGlyphCacheToFileNotImplemented();
    void exportGlyphCacheToFileThroughData();
    void exportGlyphCacheToFileThroughDataFailed();
    void exportGlyphCacheToFileThroughDataNotWritable();

    void importGlyphCacheFromSingleData();
    void importGlyphCacheFromSingleDataNotSupported();
    void importGlyphCacheFromSingleDataNotImplemented();
    void importGlyphCacheFromSingleDataNotSingleFile();
    void importGlyphCacheFromData();
    void importGlyphCacheFromDataNoData();
    void importGlyphCacheFromDataNotSupported();
    void importGlyphCacheFromDataNotImplemented();
    void importGlyphCacheFromDataAsSingleData();
    void importGlyphCacheFromFile();
    void importGlyphCacheFromFileNotSupported();
    void importGlyphCacheFromFileNotImplemented();
    void importGlyphCacheFromFileAsSingleData();
    void importGlyphCacheFromFileAsSingleDataNotFound();

    void debugFeature();
    void debugFeatures();
};

AbstractFontConverterTest::AbstractFontConverterTest() {
    addTests({&AbstractFontConverterTest::convertGlyphs,

              &AbstractFontConverterTest::exportFontToSingleData,
              &AbstractFontConverterTest::exportFontToSingleDataNotSupported,
              &AbstractFontConverterTest::exportFontToSingleDataNotImplemented,
              &AbstractFontConverterTest::exportFontToSingleDataNotSingleFile,
              &AbstractFontConverterTest::exportFontToData,
              &AbstractFontConverterTest::exportFontToDataNotSupported,
              &AbstractFontConverterTest::exportFontToDataNotImplemented,
              &AbstractFontConverterTest::exportFontToDataThroughSingleData,
              &AbstractFontConverterTest::exportFontToDataThroughSingleDataFailed,
              &AbstractFontConverterTest::exportFontToFile,
              &AbstractFontConverterTest::exportFontToFileNotSupported,
              &AbstractFontConverterTest::exportFontToFileNotImplemented,
              &AbstractFontConverterTest::exportFontToFileThroughData,
              &AbstractFontConverterTest::exportFontToFileThroughDataFailed,
              &AbstractFontConverterTest::exportFontToFileThroughDataNotWritable,

              &AbstractFontConverterTest::exportGlyphCacheToSingleData,
              &AbstractFontConverterTest::exportGlyphCacheToSingleDataNotSupported,
              &AbstractFontConverterTest::exportGlyphCacheToSingleDataNotImplemented,
              &AbstractFontConverterTest::exportGlyphCacheToSingleDataNotSingleFile,
              &AbstractFontConverterTest::exportGlyphCacheToData,
              &AbstractFontConverterTest::exportGlyphCacheToDataNotSupported,
              &AbstractFontConverterTest::exportGlyphCacheToDataNotImplemented,
              &AbstractFontConverterTest::exportGlyphCacheToDataThroughSingleData,
              &AbstractFontConverterTest::exportGlyphCacheToDataThroughSingleDataFailed,
              &AbstractFontConverterTest::exportGlyphCacheToFile,
              &AbstractFontConverterTest::exportGlyphCacheToFileNotSupported,
              &AbstractFontConverterTest::exportGlyphCacheToFileNotImplemented,
              &AbstractFontConverterTest::exportGlyphCacheToFileThroughData,
              &AbstractFontConverterTest::exportGlyphCacheToFileThroughDataFailed,
              &AbstractFontConverterTest::exportGlyphCacheToFileThroughDataNotWritable,

              &AbstractFontConverterTest::importGlyphCacheFromSingleData,
              &AbstractFontConverterTest::importGlyphCacheFromSingleDataNotSupported,
              &AbstractFontConverterTest::importGlyphCacheFromSingleDataNotImplemented,
              &AbstractFontConverterTest::importGlyphCacheFromSingleDataNotSingleFile,
              &AbstractFontConverterTest::importGlyphCacheFromData,
              &AbstractFontConverterTest::importGlyphCacheFromDataNoData,
              &AbstractFontConverterTest::importGlyphCacheFromDataNotSupported,
              &AbstractFontConverterTest::importGlyphCacheFromDataNotImplemented,
              &AbstractFontConverterTest::importGlyphCacheFromDataAsSingleData,
              &AbstractFontConverterTest::importGlyphCacheFromFile,
              &AbstractFontConverterTest::importGlyphCacheFromFileNotSupported,
              &AbstractFontConverterTest::importGlyphCacheFromFileNotImplemented,
              &AbstractFontConverterTest::importGlyphCacheFromFileAsSingleData,
              &AbstractFontConverterTest::importGlyphCacheFromFileAsSingleDataNotFound,

              &AbstractFontConverterTest::debugFeature,
              &AbstractFontConverterTest::debugFeatures});

    /* Create testing dir */
    Utility::Directory::mkpath(TEXT_TEST_OUTPUT_DIR);
}

struct DummyFont: AbstractFont {
    FontFeatures doFeatures() const override { return {}; }
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
    std::u32string characters;
    struct GlyphConverter: AbstractFontConverter {
        /* GCC 4.8 apparently can't handle {} here */
        GlyphConverter(std::u32string& characters): _characters(characters) {}

        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont;
        }

        Containers::Array<char> doExportFontToSingleData(AbstractFont&, AbstractGlyphCache&, const std::u32string& characters) const override {
            _characters = characters;
            return nullptr;
        }

        private: std::u32string& _characters;
    } converter{characters};

    converter.exportFontToSingleData(dummyFont, dummyGlyphCache, "abC01a0 ");
    CORRADE_COMPARE(characters, U" 01Cab");
}

void AbstractFontConverterTest::exportFontToSingleData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont;
        }

        Containers::Array<char> doExportFontToSingleData(AbstractFont&, AbstractGlyphCache&, const std::u32string& characters) const override {
            return Containers::array({'\xee', char(characters.size())});
        }
    } converter;

    Containers::Array<char> out = converter.exportFontToSingleData(dummyFont, dummyGlyphCache, "euhh");
    CORRADE_COMPARE_AS(out, Containers::arrayView({'\xee', '\x03'}),
        TestSuite::Compare::Container);
}

void AbstractFontConverterTest::exportFontToSingleDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ExportFont;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportFontToSingleData(dummyFont, dummyGlyphCache, {});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportFontToSingleData(): feature not supported\n");
}

void AbstractFontConverterTest::exportFontToSingleDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportFontToSingleData(dummyFont, dummyGlyphCache, {});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportFontToSingleData(): feature advertised but not implemented\n");
}

void AbstractFontConverterTest::exportFontToSingleDataNotSingleFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont|FontConverterFeature::MultiFile;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportFontToSingleData(dummyFont, dummyGlyphCache, {});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportFontToSingleData(): the format is not single-file\n");
}

void AbstractFontConverterTest::exportFontToData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont;
        }

        std::vector<std::pair<std::string, Containers::Array<char>>> doExportFontToData(AbstractFont&, AbstractGlyphCache&, const std::string& filename, const std::u32string& characters) const override {
            /* Wow what a shit API */
            std::vector<std::pair<std::string, Containers::Array<char>>> out;
            out.emplace_back(filename, Containers::array({char(characters.size())}));
            out.emplace_back(filename + ".dat", Containers::array({'\xee'}));
            return out;
        }
    } converter;

    auto ret = converter.exportFontToData(dummyFont, dummyGlyphCache, "font.out", "eH");
    CORRADE_COMPARE(ret.size(), 2);

    CORRADE_COMPARE(ret[0].first, "font.out");
    CORRADE_COMPARE_AS(ret[0].second, Containers::arrayView({'\x02'}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(ret[1].first, "font.out.dat");
    CORRADE_COMPARE_AS(ret[1].second, Containers::arrayView({'\xee'}),
        TestSuite::Compare::Container);
}

void AbstractFontConverterTest::exportFontToDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ExportFont;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportFontToData(dummyFont, dummyGlyphCache, "font.out", {});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportFontToData(): feature not supported\n");
}

void AbstractFontConverterTest::exportFontToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            /* MultiFile, otherwise it'd proxy through SingleData where the
               assertion is already tested */
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont|FontConverterFeature::MultiFile;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportFontToData(dummyFont, dummyGlyphCache, "font.out", {});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportFontToData(): feature advertised but not implemented\n");
}

void AbstractFontConverterTest::exportFontToDataThroughSingleData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont;
        }

        Containers::Array<char> doExportFontToSingleData(AbstractFont&, AbstractGlyphCache&, const std::u32string& characters) const override {
            return Containers::array({'\xee', char(characters.size())});
        }
    } converter;

    /* doExportFontToData() should call doExportFontToSingleData() */
    auto ret = converter.exportFontToData(dummyFont, dummyGlyphCache, "font.out", "ehh");
    CORRADE_COMPARE(ret.size(), 1);
    CORRADE_COMPARE(ret[0].first, "font.out");
    CORRADE_COMPARE_AS(ret[0].second, Containers::arrayView({'\xee', '\x02'}),
        TestSuite::Compare::Container);
}

void AbstractFontConverterTest::exportFontToDataThroughSingleDataFailed() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont;
        }

        Containers::Array<char> doExportFontToSingleData(AbstractFont&, AbstractGlyphCache&, const std::u32string&) const override {
            return {};
        }
    } converter;

    auto ret = converter.exportFontToData(dummyFont, dummyGlyphCache, "font.out", "ehh");
    CORRADE_VERIFY(ret.empty());
}

void AbstractFontConverterTest::exportFontToFile() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont|FontConverterFeature::MultiFile;
        }

        bool doExportFontToFile(AbstractFont&, AbstractGlyphCache&, const std::string& filename, const std::u32string& characters) const override {
            return
                Utility::Directory::write(filename, Containers::arrayView({'\xf0'})) &&
                Utility::Directory::write(filename + ".dat", Containers::arrayView({'\xfe', char(characters.size())}));
        }
    } converter;

    const std::string filename = Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out");
    const std::string filename2 = Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out.dat");

    /* Remove previous files, if any */
    Utility::Directory::rm(filename);
    Utility::Directory::rm(filename2);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename2));

    CORRADE_VERIFY(converter.exportFontToFile(dummyFont, dummyGlyphCache, filename, "eh"));
    CORRADE_COMPARE_AS(filename, "\xf0",
        TestSuite::Compare::FileToString);
    CORRADE_COMPARE_AS(filename2,
        "\xfe\x02", TestSuite::Compare::FileToString);
}

void AbstractFontConverterTest::exportFontToFileNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ExportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportFontToFile(dummyFont, dummyGlyphCache, "file.out", {});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportFontToFile(): feature not supported\n");
}

void AbstractFontConverterTest::exportFontToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ExportFont;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportFontToFile(dummyFont, dummyGlyphCache, "file.out", {});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportFontToFile(): feature advertised but not implemented\n");
}

void AbstractFontConverterTest::exportFontToFileThroughData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont|FontConverterFeature::MultiFile;
        }

        std::vector<std::pair<std::string, Containers::Array<char>>> doExportFontToData(AbstractFont&, AbstractGlyphCache&, const std::string& filename, const std::u32string& characters) const override {
            std::vector<std::pair<std::string, Containers::Array<char>>> ret;
            ret.emplace_back(filename, Containers::array({'\xf0'}));
            ret.emplace_back(filename + ".dat", Containers::array({'\xfe', char(characters.size())}));
            return ret;
        }
    } converter;

    const std::string filename = Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out");
    const std::string filename2 = Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out.dat");

    /* Remove previous files, if any */
    Utility::Directory::rm(filename);
    Utility::Directory::rm(filename2);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename2));

    /* doExportToFile() should call doExportToData() */
    CORRADE_VERIFY(converter.exportFontToFile(dummyFont, dummyGlyphCache, filename, "awoo"));
    CORRADE_COMPARE_AS(filename, "\xf0",
        TestSuite::Compare::FileToString);
    CORRADE_COMPARE_AS(filename2, "\xfe\x03",
        TestSuite::Compare::FileToString);
}

void AbstractFontConverterTest::exportFontToFileThroughDataFailed() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont|FontConverterFeature::MultiFile;
        }

        std::vector<std::pair<std::string, Containers::Array<char>>> doExportFontToData(AbstractFont&, AbstractGlyphCache&, const std::string&, const std::u32string&) const override { return {}; }
    } converter;

    const std::string filename = Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "font.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.exportFontToFile(dummyFont, dummyGlyphCache, filename, {}));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractFontConverterTest::exportFontToFileThroughDataNotWritable() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportFont|FontConverterFeature::MultiFile;
        }

        std::vector<std::pair<std::string, Containers::Array<char>>> doExportFontToData(AbstractFont&, AbstractGlyphCache&, const std::string& filename, const std::u32string&) const override {
            std::vector<std::pair<std::string, Containers::Array<char>>> ret;
            ret.emplace_back(filename, Containers::array({'\xf0'}));
            return ret;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.exportFontToFile(dummyFont, dummyGlyphCache, "/some/path/that/does/not/exist", {}));
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "Text::AbstractFontConverter::exportFontToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractFontConverterTest::exportGlyphCacheToSingleData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override { return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache; }

        Containers::Array<char> doExportGlyphCacheToSingleData(AbstractGlyphCache&) const override {
            return Containers::array({'\xee'});
        }
    } converter;

    Containers::Array<char> out = converter.exportGlyphCacheToSingleData(dummyGlyphCache);
    CORRADE_COMPARE_AS(out,
        (Containers::Array<char>{Containers::InPlaceInit, {'\xee'}}),
        TestSuite::Compare::Container);
}

void AbstractFontConverterTest::exportGlyphCacheToSingleDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ExportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportGlyphCacheToSingleData(dummyGlyphCache);
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportGlyphCacheToSingleData(): feature not supported\n");
}

void AbstractFontConverterTest::exportGlyphCacheToSingleDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportGlyphCacheToSingleData(dummyGlyphCache);
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportGlyphCacheToSingleData(): feature advertised but not implemented\n");
}

void AbstractFontConverterTest::exportGlyphCacheToSingleDataNotSingleFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache|FontConverterFeature::MultiFile;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportGlyphCacheToSingleData(dummyGlyphCache);
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportGlyphCacheToSingleData(): the format is not single-file\n");
}

void AbstractFontConverterTest::exportGlyphCacheToData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache;
        }

        std::vector<std::pair<std::string, Containers::Array<char>>> doExportGlyphCacheToData(AbstractGlyphCache&, const std::string& filename) const override {
            std::vector<std::pair<std::string, Containers::Array<char>>> ret;
            ret.emplace_back(filename, Containers::array({'\xf0'}));
            ret.emplace_back(filename + ".dat", Containers::array({'\xfe', '\xed'}));
            return ret;
        }
    } converter;

    auto ret = converter.exportGlyphCacheToData(dummyGlyphCache, "cache.out");
    CORRADE_COMPARE(ret.size(), 2);

    CORRADE_COMPARE(ret[0].first, "cache.out");
    CORRADE_COMPARE_AS(ret[0].second, Containers::arrayView({'\xf0'}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(ret[1].first, "cache.out.dat");
    CORRADE_COMPARE_AS(ret[1].second, Containers::arrayView({'\xfe', '\xed'}),
        TestSuite::Compare::Container);
}

void AbstractFontConverterTest::exportGlyphCacheToDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ExportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportGlyphCacheToData(dummyGlyphCache, "cache.out");
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportGlyphCacheToData(): feature not supported\n");
}

void AbstractFontConverterTest::exportGlyphCacheToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            /* MultiFile, otherwise it'd proxy through SingleData where the
               assertion is already tested */
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache|FontConverterFeature::MultiFile;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportGlyphCacheToData(dummyGlyphCache, "cache.out");
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportGlyphCacheToData(): feature advertised but not implemented\n");
}

void AbstractFontConverterTest::exportGlyphCacheToDataThroughSingleData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache;
        }

        Containers::Array<char> doExportGlyphCacheToSingleData(AbstractGlyphCache&) const override {
            return Containers::array({'\xee'});
        }
    } converter;

    /* doExportGlyphCacheToData() should call doExportGlyphCacheToSingleData() */
    auto ret = converter.exportGlyphCacheToData(dummyGlyphCache, "font.out");
    CORRADE_COMPARE(ret.size(), 1);
    CORRADE_COMPARE(ret[0].first, "font.out");
    CORRADE_COMPARE_AS(ret[0].second,
        (Containers::Array<char>{Containers::InPlaceInit, {'\xee'}}),
        TestSuite::Compare::Container);
}

void AbstractFontConverterTest::exportGlyphCacheToDataThroughSingleDataFailed() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache;
        }

        Containers::Array<char> doExportGlyphCacheToSingleData(AbstractGlyphCache&) const override {
            return {};
        }
    } converter;

    auto ret = converter.exportGlyphCacheToData(dummyGlyphCache, "font.out");
    CORRADE_VERIFY(ret.empty());
}

void AbstractFontConverterTest::exportGlyphCacheToFile() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache|FontConverterFeature::MultiFile;
        }

        bool doExportGlyphCacheToFile(AbstractGlyphCache&, const std::string& filename) const override {
            return
                Utility::Directory::write(filename, Containers::arrayView({'\xf0'})) &&
                Utility::Directory::write(filename + ".dat", Containers::arrayView({'\xfe', '\xed'}));
        }
    } converter;

    const std::string filename = Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "cache.out");
    const std::string filename2 = Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "cache.out.dat");

    /* Remove previous files, if any */
    Utility::Directory::rm(filename);
    Utility::Directory::rm(filename2);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename2));

    CORRADE_VERIFY(converter.exportGlyphCacheToFile(dummyGlyphCache, filename));
    CORRADE_COMPARE_AS(filename, "\xf0",
        TestSuite::Compare::FileToString);
    CORRADE_COMPARE_AS(filename2,
        "\xfe\xed", TestSuite::Compare::FileToString);
}

void AbstractFontConverterTest::exportGlyphCacheToFileNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ExportFont;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportGlyphCacheToFile(dummyGlyphCache, "cache.out");
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportGlyphCacheToFile(): feature not supported\n");
}

void AbstractFontConverterTest::exportGlyphCacheToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ExportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.exportGlyphCacheToFile(dummyGlyphCache, "cache.out");
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::exportGlyphCacheToFile(): feature advertised but not implemented\n");
}

void AbstractFontConverterTest::exportGlyphCacheToFileThroughData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache|FontConverterFeature::MultiFile;
        }

        std::vector<std::pair<std::string, Containers::Array<char>>> doExportGlyphCacheToData(AbstractGlyphCache&, const std::string& filename) const override {
            std::vector<std::pair<std::string, Containers::Array<char>>> ret;
            ret.emplace_back(filename, Containers::Array<char>{Containers::InPlaceInit, {'\xf0'}});
            ret.emplace_back(filename + ".dat", Containers::Array<char>{Containers::InPlaceInit, {'\xfe', '\xed'}});
            return ret;
        }
    } converter;

    const std::string filename = Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "cache.out");
    const std::string filename2 = Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "cache.out.dat");

    /* Remove previous files, if any */
    Utility::Directory::rm(filename);
    Utility::Directory::rm(filename2);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename2));

    /* doExportGlyphCacheToFile() should call doExportGlyphCacheToData() */
    CORRADE_VERIFY(converter.exportGlyphCacheToFile(dummyGlyphCache, filename));
    CORRADE_COMPARE_AS(filename, "\xf0",
        TestSuite::Compare::FileToString);
    CORRADE_COMPARE_AS(filename2,
        "\xfe\xed", TestSuite::Compare::FileToString);
}

void AbstractFontConverterTest::exportGlyphCacheToFileThroughDataFailed() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override { return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache|FontConverterFeature::MultiFile; }

        std::vector<std::pair<std::string, Containers::Array<char>>> doExportGlyphCacheToData(AbstractGlyphCache&, const std::string&) const override { return {}; }
    } converter;

    const std::string filename = Utility::Directory::join(TEXT_TEST_OUTPUT_DIR, "cache.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.exportGlyphCacheToFile(dummyGlyphCache, filename));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractFontConverterTest::exportGlyphCacheToFileThroughDataNotWritable() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override { return FontConverterFeature::ConvertData|FontConverterFeature::ExportGlyphCache|FontConverterFeature::MultiFile; }

        std::vector<std::pair<std::string, Containers::Array<char>>> doExportGlyphCacheToData(AbstractGlyphCache&, const std::string& filename) const override {
            std::vector<std::pair<std::string, Containers::Array<char>>> ret;
            ret.emplace_back(filename, Containers::array({'\xf0'}));
            return ret;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.exportGlyphCacheToFile(dummyGlyphCache, "/some/path/that/does/not/exist"));
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "Text::AbstractFontConverter::exportGlyphCacheToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractFontConverterTest::importGlyphCacheFromSingleData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ImportGlyphCache;
        }

        Containers::Pointer<AbstractGlyphCache> doImportGlyphCacheFromSingleData(const Containers::ArrayView<const char> data) const override {
            if(data.size() == 1 && data[0] == '\xa5')
                return Containers::pointer(new DummyGlyphCache{{123, 345}});
            return nullptr;
        }
    } converter;

    const char data[] = {'\xa5'};
    Containers::Pointer<AbstractGlyphCache> cache = converter.importGlyphCacheFromSingleData(data);
    CORRADE_VERIFY(cache);
    CORRADE_COMPARE(cache->textureSize(), (Vector2i{123, 345}));
}

void AbstractFontConverterTest::importGlyphCacheFromSingleDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ImportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.importGlyphCacheFromSingleData({});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::importGlyphCacheFromSingleData(): feature not supported\n");
}

void AbstractFontConverterTest::importGlyphCacheFromSingleDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ImportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.importGlyphCacheFromSingleData({});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::importGlyphCacheFromSingleData(): feature advertised but not implemented\n");
}

void AbstractFontConverterTest::importGlyphCacheFromSingleDataNotSingleFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ImportGlyphCache|FontConverterFeature::MultiFile;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.importGlyphCacheFromSingleData({});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::importGlyphCacheFromSingleData(): the format is not single-file\n");
}

void AbstractFontConverterTest::importGlyphCacheFromData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ImportGlyphCache;
        }

        Containers::Pointer<AbstractGlyphCache> doImportGlyphCacheFromData(const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data) const override {
            if(data.size() == 2 && data[1].second.size() == 1 && data[1].second[0] == '\xa5')
                return Containers::pointer(new DummyGlyphCache{{123, 345}});
            return nullptr;
        }
    } converter;

    const char data[] = {'\xa5'};
    Containers::Pointer<AbstractGlyphCache> cache = converter.importGlyphCacheFromData({{}, {{}, data}});
    CORRADE_VERIFY(cache);
    CORRADE_COMPARE(cache->textureSize(), (Vector2i{123, 345}));
}

void AbstractFontConverterTest::importGlyphCacheFromDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ImportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.importGlyphCacheFromData({{}});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::importGlyphCacheFromData(): feature not supported\n");
}

void AbstractFontConverterTest::importGlyphCacheFromDataNoData() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ImportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.importGlyphCacheFromData({});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::importGlyphCacheFromData(): no data passed\n");
}

void AbstractFontConverterTest::importGlyphCacheFromDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            /* MultiFile, otherwise it'd proxy through SingleData where the
               assertion is already tested */
            return FontConverterFeature::ConvertData|FontConverterFeature::ImportGlyphCache|FontConverterFeature::MultiFile;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.importGlyphCacheFromData({{}});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::importGlyphCacheFromData(): feature advertised but not implemented\n");
}

void AbstractFontConverterTest::importGlyphCacheFromDataAsSingleData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ImportGlyphCache;
        }

        Containers::Pointer<AbstractGlyphCache> doImportGlyphCacheFromSingleData(const Containers::ArrayView<const char> data) const override {
            if(data.size() == 1 && data[0] == '\xa5')
                return Containers::pointer(new DummyGlyphCache{{123, 345}});
            return nullptr;
        }
    } converter;

    const char data[] = {'\xa5'};
    Containers::Pointer<AbstractGlyphCache> cache = converter.importGlyphCacheFromData({{{}, data}});
    CORRADE_VERIFY(cache);
    CORRADE_COMPARE(cache->textureSize(), (Vector2i{123, 345}));
}

void AbstractFontConverterTest::importGlyphCacheFromFile() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ImportGlyphCache;
        }

        Containers::Pointer<AbstractGlyphCache> doImportGlyphCacheFromFile(const std::string& filename) const override {
            Containers::Array<char> data = Utility::Directory::read(filename);
            if(data.size() == 1 && data[0] == '\xa5')
                return Containers::pointer(new DummyGlyphCache{{123, 345}});
            return nullptr;
        }
    } converter;

    Containers::Pointer<AbstractGlyphCache> cache = converter.importGlyphCacheFromFile(Utility::Directory::join(TEXT_TEST_DIR, "data.bin"));
    CORRADE_VERIFY(cache);
    CORRADE_COMPARE(cache->textureSize(), (Vector2i{123, 345}));
}

void AbstractFontConverterTest::importGlyphCacheFromFileNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ExportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.importGlyphCacheFromFile({});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::importGlyphCacheFromFile(): feature not supported\n");
}

void AbstractFontConverterTest::importGlyphCacheFromFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ImportGlyphCache;
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.importGlyphCacheFromFile({});
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::importGlyphCacheFromFile(): feature advertised but not implemented\n");
}

void AbstractFontConverterTest::importGlyphCacheFromFileAsSingleData() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ImportGlyphCache;
        }

        Containers::Pointer<AbstractGlyphCache> doImportGlyphCacheFromSingleData(const Containers::ArrayView<const char> data) const override {
            if(data.size() == 1 && data[0] == '\xa5')
                return Containers::pointer(new DummyGlyphCache{{123, 345}});
            return nullptr;
        }
    } converter;

    /* doImportFromFile() should call doImportFromSingleData() */
    Containers::Pointer<AbstractGlyphCache> cache = converter.importGlyphCacheFromFile(Utility::Directory::join(TEXT_TEST_DIR, "data.bin"));
    CORRADE_VERIFY(cache);
    CORRADE_COMPARE(cache->textureSize(), (Vector2i{123, 345}));
}

void AbstractFontConverterTest::importGlyphCacheFromFileAsSingleDataNotFound() {
    struct: AbstractFontConverter {
        FontConverterFeatures doFeatures() const override {
            return FontConverterFeature::ConvertData|FontConverterFeature::ImportGlyphCache;
        }

        Containers::Pointer<AbstractGlyphCache> doImportGlyphCacheFromSingleData(const Containers::ArrayView<const char>) const override {
            CORRADE_VERIFY(!"this shouldn't get reached");
            return {};
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.importGlyphCacheFromFile("nonexistent.bin"));
    CORRADE_COMPARE(out.str(), "Text::AbstractFontConverter::importGlyphCacheFromFile(): cannot open file nonexistent.bin\n");
}

void AbstractFontConverterTest::debugFeature() {
    std::ostringstream out;

    Debug{&out} << FontConverterFeature::ExportFont << FontConverterFeature(0xf0);
    CORRADE_COMPARE(out.str(), "Text::FontConverterFeature::ExportFont Text::FontConverterFeature(0xf0)\n");
}

void AbstractFontConverterTest::debugFeatures() {
    std::ostringstream out;

    Debug{&out} << (FontConverterFeature::ExportFont|FontConverterFeature::ImportGlyphCache) << FontConverterFeatures{};
    CORRADE_COMPARE(out.str(), "Text::FontConverterFeature::ExportFont|Text::FontConverterFeature::ImportGlyphCache Text::FontConverterFeatures{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractFontConverterTest)
