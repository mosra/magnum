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

#include <string> /** @todo remove once AbstractImporter is <string>-free */
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once AbstractImporter is <string>-free */
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Path.h>

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
    void debugFeaturePacked();
    void debugFeatures();
    void debugFeaturesPacked();
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
              &AbstractImporterTest::debugFeaturePacked,
              &AbstractImporterTest::debugFeatures,
              &AbstractImporterTest::debugFeaturesPacked});
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
    importer.openFile(Utility::Path::join(AUDIO_TEST_DIR, "file.bin"));
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

    Containers::String out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openFile("nonexistent.bin"));
    CORRADE_VERIFY(!importer.isOpened());
    /* There's an error from Path::read() before */
    CORRADE_COMPARE_AS(out,
        "\nAudio::AbstractImporter::openFile(): cannot open file nonexistent.bin\n",
        TestSuite::Compare::StringHasSuffix);
}

void AbstractImporterTest::openFileNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Importer: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    Containers::String out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openFile("file.dat"));
    CORRADE_COMPARE(out, "Audio::AbstractImporter::openFile(): not implemented\n");
}

void AbstractImporterTest::openDataNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    Containers::String out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openData(nullptr));
    CORRADE_COMPARE(out, "Audio::AbstractImporter::openData(): feature not supported\n");
}

void AbstractImporterTest::openDataNotImplemented() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return ImporterFeature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    Containers::String out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openData(nullptr));
    CORRADE_COMPARE(out, "Audio::AbstractImporter::openData(): feature advertised but not implemented\n");
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
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    Containers::String out;
    Error redirectError{&out};

    importer.format();
    CORRADE_COMPARE(out, "Audio::AbstractImporter::format(): no file opened\n");
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
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    Containers::String out;
    Error redirectError{&out};

    importer.frequency();
    CORRADE_COMPARE(out, "Audio::AbstractImporter::frequency(): no file opened\n");
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

    CORRADE_COMPARE_AS(importer.data(), (Containers::Array<char>{InPlaceInit, {'H'}}), TestSuite::Compare::Container);
}

void AbstractImporterTest::dataNoFile() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct: AbstractImporter {
        ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        BufferFormat doFormat() const override { return {}; }
        UnsignedInt doFrequency() const override { return {}; }
        Containers::Array<char> doData() override { return nullptr; }
    } importer;

    Containers::String out;
    Error redirectError{&out};

    importer.data();
    CORRADE_COMPARE(out, "Audio::AbstractImporter::data(): no file opened\n");
}

void AbstractImporterTest::dataCustomDeleter() {
    CORRADE_SKIP_IF_NO_ASSERT();

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

    Containers::String out;
    Error redirectError{&out};

    importer.data();
    CORRADE_COMPARE(out, "Audio::AbstractImporter::data(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractImporterTest::debugFeature() {
    Containers::String out;

    Debug{&out} << ImporterFeature::OpenData << ImporterFeature(0xf0);
    CORRADE_COMPARE(out, "Audio::ImporterFeature::OpenData Audio::ImporterFeature(0xf0)\n");
}

void AbstractImporterTest::debugFeaturePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << ImporterFeature::OpenData << Debug::packed << ImporterFeature(0xf0) << ImporterFeature::OpenData;
    CORRADE_COMPARE(out, "OpenData 0xf0 Audio::ImporterFeature::OpenData\n");
}

void AbstractImporterTest::debugFeatures() {
    Containers::String out;

    Debug{&out} << (ImporterFeature::OpenData|ImporterFeature(0xf0)) << ImporterFeatures{};
    CORRADE_COMPARE(out, "Audio::ImporterFeature::OpenData|Audio::ImporterFeature(0xf0) Audio::ImporterFeatures{}\n");
}

void AbstractImporterTest::debugFeaturesPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (ImporterFeature::OpenData|ImporterFeature(0xf0)) << Debug::packed << ImporterFeatures{} << ImporterFeature::OpenData;
    CORRADE_COMPARE(out, "OpenData|0xf0 {} Audio::ImporterFeature::OpenData\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::AbstractImporterTest)
