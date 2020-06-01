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
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Audio/AbstractImporter.h"
#include "Magnum/Audio/BufferFormat.h"

#include "configure.h"

namespace Magnum { namespace Audio { namespace Test { namespace {

struct AbstractImporterTest: TestSuite::Tester {
    explicit AbstractImporterTest();

    void construct();

    void openData();
    void openFileAsData();
    void openFileAsDataNotFound();

    void openFileNotImplemented();
    void openDataNotSupported();
    void openDataNotImplemented();

    /* file callbacks not supported -- those will be once this gets merged with
       Trade::AbstractImporter */

    void format();
    void formatNoFile();

    void frequency();
    void frequencyNoFile();

    void data();
    void dataNoFile();
    void dataCustomDeleter();

    void debugFeature();
    void debugFeatures();
};

AbstractImporterTest::AbstractImporterTest() {
    addTests({&AbstractImporterTest::construct,

              &AbstractImporterTest::openData,
              &AbstractImporterTest::openFileAsData,
              &AbstractImporterTest::openFileAsDataNotFound,

              &AbstractImporterTest::openFileNotImplemented,
              &AbstractImporterTest::openDataNotSupported,
              &AbstractImporterTest::openDataNotImplemented,

              &AbstractImporterTest::format,
              &AbstractImporterTest::formatNoFile,

              &AbstractImporterTest::frequency,
              &AbstractImporterTest::frequencyNoFile,

              &AbstractImporterTest::data,
              &AbstractImporterTest::dataNoFile,
              &AbstractImporterTest::dataCustomDeleter,

              &AbstractImporterTest::debugFeature,
              &AbstractImporterTest::debugFeatures});
}

void AbstractImporterTest::construct() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    CORRADE_COMPARE(importer.features(), ImporterFeatures{});
    CORRADE_VERIFY(!importer.isOpened());

    importer.close();
    CORRADE_VERIFY(!importer.isOpened());
}

void AbstractImporterTest::openData() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::ArrayView<const char> data) override {
            _opened = (data.size() == 1 && data[0] == '\xa5');
        }

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }

        bool _opened = false;
    } importer;

    CORRADE_VERIFY(!importer.isOpened());
    const char a5 = '\xa5';
    CORRADE_VERIFY(importer.openData({&a5, 1}));
    CORRADE_VERIFY(importer.isOpened());

    importer.close();
    CORRADE_VERIFY(!importer.isOpened());
}

void AbstractImporterTest::openFileAsData() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::ArrayView<const char> data) override {
            _opened = (data.size() == 1 && data[0] == '\xa5');
        }

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }

        bool _opened = false;
    } importer;

    /* doOpenFile() should call doOpenData() */
    CORRADE_VERIFY(!importer.isOpened());
    importer.openFile(Utility::Directory::join(AUDIO_TEST_DIR, "file.bin"));
    CORRADE_VERIFY(importer.isOpened());

    importer.close();
    CORRADE_VERIFY(!importer.isOpened());
}

void AbstractImporterTest::openFileAsDataNotFound() {
    struct Importer: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::ArrayView<const char>) override {
            _opened = true;
        }

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }

        bool _opened = false;
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openFile("nonexistent.bin"));
    CORRADE_VERIFY(!importer.isOpened());
    CORRADE_COMPARE(out.str(), "Audio::AbstractImporter::openFile(): cannot open file nonexistent.bin\n");
}

void AbstractImporterTest::openFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Importer: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openFile("file.dat"));
    CORRADE_COMPARE(out.str(), "Audio::AbstractImporter::openFile(): not implemented\n");
}

void AbstractImporterTest::openDataNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openData(nullptr));
    CORRADE_COMPARE(out.str(), "Audio::AbstractImporter::openData(): feature not supported\n");
}

void AbstractImporterTest::openDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openData(nullptr));
    CORRADE_COMPARE(out.str(), "Audio::AbstractImporter::openData(): feature advertised but not implemented\n");
}

void AbstractImporterTest::format() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        BufferFormat doFormat() const override { return BufferFormat::Mono8; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    CORRADE_COMPARE(importer.format(), BufferFormat::Mono8);
}

void AbstractImporterTest::formatNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.format();
    CORRADE_COMPARE(out.str(), "Audio::AbstractImporter::format(): no file opened\n");
}

void AbstractImporterTest::frequency() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return 44000; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    CORRADE_COMPARE(importer.frequency(), 44000);
}

void AbstractImporterTest::frequencyNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.frequency();
    CORRADE_COMPARE(out.str(), "Audio::AbstractImporter::frequency(): no file opened\n");
}

void AbstractImporterTest::data() {
    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override {
            Containers::Array<char> out{1};
            out[0] = 'H';
            return out;
        }
    } importer;

    CORRADE_COMPARE_AS(importer.data(), (Containers::Array<char>{Containers::InPlaceInit, {'H'}}), TestSuite::Compare::Container);
}

void AbstractImporterTest::dataNoFile() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.data();
    CORRADE_COMPARE(out.str(), "Audio::AbstractImporter::data(): no file opened\n");
}

void AbstractImporterTest::dataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override {
            return Containers::Array<char>{nullptr, 0, [](char*, std::size_t) {}};
        }
    } importer;

    std::ostringstream out;
    Error redirectError{&out};

    importer.data();
    CORRADE_COMPARE(out.str(), "Audio::AbstractImporter::data(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::debugFeature() {
    std::ostringstream out;

    Debug{&out} << ImporterFeature::OpenData << ImporterFeature(0xf0);
    CORRADE_COMPARE(out.str(), "Audio::ImporterFeature::OpenData Audio::ImporterFeature(0xf0)\n");
}

void AbstractImporterTest::debugFeatures() {
    std::ostringstream out;

    Debug{&out} << ImporterFeature::OpenData << ImporterFeatures{};
    CORRADE_COMPARE(out.str(), "Audio::ImporterFeature::OpenData Audio::ImporterFeatures{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::AbstractImporterTest)
