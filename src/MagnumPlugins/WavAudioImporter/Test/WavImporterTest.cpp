/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Directory.h>

#include "MagnumPlugins/WavAudioImporter/WavImporter.h"
#include "MagnumPlugins/WavAudioImporter/WavHeader.h"

#include "configure.h"

namespace Magnum { namespace Audio { namespace Test {

class WavImporterTest: public TestSuite::Tester {
    public:
        explicit WavImporterTest();

        void wrongSize();
        void wrongSignature();
        void unsupportedFormat();
        void unsupportedChannelCount();

        void mono8();
        void mono8junk();
        void mono8ALaw();
        void mono8MuLaw();
        void mono16();

        void stereo8();
        void stereo8ALaw();
        void stereo8MuLaw();
        void stereo16();

        void mono32f();
        void stereo32f();
        void stereo64f();

        void surround616();

        void debugAudioFormat();
};

WavImporterTest::WavImporterTest() {
    addTests({&WavImporterTest::wrongSize,
              &WavImporterTest::wrongSignature,
              &WavImporterTest::unsupportedFormat,
              &WavImporterTest::unsupportedChannelCount,

              &WavImporterTest::mono8,
              &WavImporterTest::mono8junk,
              &WavImporterTest::mono8ALaw,
              &WavImporterTest::mono8MuLaw,
              &WavImporterTest::mono16,

              &WavImporterTest::stereo8,
              &WavImporterTest::stereo8ALaw,
              &WavImporterTest::stereo8MuLaw,
              &WavImporterTest::stereo16,

              &WavImporterTest::mono32f,
              &WavImporterTest::stereo32f,
              &WavImporterTest::stereo64f,
              &WavImporterTest::surround616,

              &WavImporterTest::debugAudioFormat});
}

void WavImporterTest::wrongSize() {
    std::ostringstream out;
    Error redirectError{&out};

    WavImporter importer;
    CORRADE_VERIFY(!importer.openData(Containers::Array<char>(43)));
    CORRADE_COMPARE(out.str(), "Audio::WavImporter::openData(): the file is too short: 43 bytes\n");
}

void WavImporterTest::wrongSignature() {
    std::ostringstream out;
    Error redirectError{&out};

    WavImporter importer;
    CORRADE_VERIFY(!importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "wrongSignature.wav")));
    CORRADE_COMPARE(out.str(), "Audio::WavImporter::openData(): the file signature is invalid\n");
}

void WavImporterTest::unsupportedFormat() {
    std::ostringstream out;
    Error redirectError{&out};

    WavImporter importer;
    CORRADE_VERIFY(!importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "unsupportedFormat.wav")));
    CORRADE_COMPARE(out.str(), "Audio::WavImporter::openData(): unsupported format Audio::WavAudioFormat(0x2)\n");
}

void WavImporterTest::unsupportedChannelCount() {
    std::ostringstream out;
    Error redirectError{&out};

    WavImporter importer;
    CORRADE_VERIFY(!importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "unsupportedChannelCount.wav")));
    CORRADE_COMPARE(out.str(), "Audio::WavImporter::openData(): unsupported channel count 6 with 8 bits per sample\n");
}

void WavImporterTest::mono8() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "mono8.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::Mono8);
    CORRADE_COMPARE(importer.frequency(), 22050);
}

void WavImporterTest::mono8junk() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "mono8junk.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::Mono8);
    CORRADE_COMPARE(importer.frequency(), 22050);
}

void WavImporterTest::mono8ALaw() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "mono8ALaw.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::MonoALaw);
    CORRADE_COMPARE(importer.frequency(), 8000);
}

void WavImporterTest::mono8MuLaw() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "mono8MuLaw.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::MonoMuLaw);
    CORRADE_COMPARE(importer.frequency(), 8000);
}

void WavImporterTest::mono16() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "mono16.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::Mono16);
    CORRADE_COMPARE(importer.frequency(), 44000);
    CORRADE_COMPARE_AS(importer.data(),
        Containers::Array<char>::from('\x1d', '\x10', '\x71', '\xc5'),
        TestSuite::Compare::Container);
}

void WavImporterTest::stereo8() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "stereo8.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::Stereo8);
    CORRADE_COMPARE(importer.frequency(), 96000);
    CORRADE_COMPARE_AS(importer.data(),
        Containers::Array<char>::from('\xde', '\xfe', '\xca', '\x7e'),
        TestSuite::Compare::Container);
}

void WavImporterTest::stereo8ALaw() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "stereo8ALaw.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::StereoALaw);
    CORRADE_COMPARE(importer.frequency(), 8000);
}

void WavImporterTest::stereo8MuLaw() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "stereo8MuLaw.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::StereoMuLaw);
    CORRADE_COMPARE(importer.frequency(), 8000);
}


void WavImporterTest::stereo16() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "stereo16.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::Stereo16);
    CORRADE_COMPARE(importer.frequency(), 44100);
}

void WavImporterTest::mono32f() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "mono32f.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::MonoFloat);
    CORRADE_COMPARE(importer.frequency(), 48000);
}

void WavImporterTest::stereo32f() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "stereo32f.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::StereoFloat);
    CORRADE_COMPARE(importer.frequency(), 44100);
}

void WavImporterTest::stereo64f() {
    WavImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "stereo64f.wav")));

    CORRADE_COMPARE(importer.format(), Buffer::Format::StereoDouble);
    CORRADE_COMPARE(importer.frequency(), 8000);
}

void WavImporterTest::surround616() {
    std::ostringstream out;
    Error redirectError{&out};

    WavImporter importer;

    CORRADE_VERIFY(!importer.openFile(Utility::Directory::join(WAVAUDIOIMPORTER_TEST_DIR, "surround616.wav")));
    CORRADE_COMPARE(out.str(), "Audio::WavImporter::openData(): unsupported format Audio::WavAudioFormat::Extensible\n");
}

void WavImporterTest::debugAudioFormat() {
    std::ostringstream out;

    Debug{&out} << WavAudioFormat::IeeeFloat << WavAudioFormat(0xdead);
    CORRADE_COMPARE(out.str(), "Audio::WavAudioFormat::IeeeFloat Audio::WavAudioFormat(0xdead)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::WavImporterTest)
