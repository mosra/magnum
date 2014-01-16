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

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/ColorFormat.h"
#include "Magnum/Trade/ImageData.h"
#include "MagnumPlugins/TgaImporter/TgaImporter.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test {

class TgaImporterTest: public TestSuite::Tester {
    public:
        TgaImporterTest();

        void openNonexistent();
        void openShort();
        void paletted();
        void compressed();

        void colorBits16();
        void colorBits24();
        void colorBits32();

        void grayscaleBits8();
        void grayscaleBits16();

        void file();
};

TgaImporterTest::TgaImporterTest() {
    addTests<TgaImporterTest>({&TgaImporterTest::openNonexistent,
              &TgaImporterTest::openShort,
              &TgaImporterTest::paletted,
              &TgaImporterTest::compressed,

              &TgaImporterTest::colorBits16,
              &TgaImporterTest::colorBits24,
              &TgaImporterTest::colorBits32,

              &TgaImporterTest::grayscaleBits8,
              &TgaImporterTest::grayscaleBits16,

              &TgaImporterTest::file});
}

void TgaImporterTest::openNonexistent() {
    std::ostringstream debug;
    Error::setOutput(&debug);

    TgaImporter importer;
    CORRADE_VERIFY(!importer.openFile("nonexistent.file"));
    CORRADE_COMPARE(debug.str(), "Trade::TgaImporter::openFile(): cannot open file nonexistent.file\n");
}

void TgaImporterTest::openShort() {
    TgaImporter importer;
    const unsigned char data[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CORRADE_VERIFY(importer.openData(data));

    std::ostringstream debug;
    Error::setOutput(&debug);
    CORRADE_VERIFY(!importer.image2D(0));
    CORRADE_COMPARE(debug.str(), "Trade::TgaImporter::image2D(): the file is too short: 17 bytes\n");
}

void TgaImporterTest::paletted() {
    TgaImporter importer;
    const unsigned char data[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CORRADE_VERIFY(importer.openData(data));

    std::ostringstream debug;
    Error::setOutput(&debug);
    CORRADE_VERIFY(!importer.image2D(0));
    CORRADE_COMPARE(debug.str(), "Trade::TgaImporter::image2D(): paletted files are not supported\n");
}

void TgaImporterTest::compressed() {
    TgaImporter importer;
    const unsigned char data[] = { 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CORRADE_VERIFY(importer.openData(data));

    std::ostringstream debug;
    Error::setOutput(&debug);
    CORRADE_VERIFY(!importer.image2D(0));
    CORRADE_COMPARE(debug.str(), "Trade::TgaImporter::image2D(): unsupported (compressed?) image type: 9\n");
}

void TgaImporterTest::colorBits16() {
    TgaImporter importer;
    const unsigned char data[] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0 };
    CORRADE_VERIFY(importer.openData(data));

    std::ostringstream debug;
    Error::setOutput(&debug);
    CORRADE_VERIFY(!importer.image2D(0));
    CORRADE_COMPARE(debug.str(), "Trade::TgaImporter::image2D(): unsupported color bits-per-pixel: 16\n");
}

void TgaImporterTest::colorBits24() {
    TgaImporter importer;
    const unsigned char data[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        1, 2, 3, 2, 3, 4,
        3, 4, 5, 4, 5, 6,
        5, 6, 7, 6, 7, 8
    };
    #ifndef MAGNUM_TARGET_GLES
    const char* pixels = reinterpret_cast<const char*>(data) + 18;
    #else
    const char pixels[] = {
        3, 2, 1, 4, 3, 2,
        5, 4, 3, 6, 5, 4,
        7, 6, 5, 8, 7, 6
    };
    #endif
    CORRADE_VERIFY(importer.openData(data));

    std::optional<Trade::ImageData2D> image = importer.image2D(0);
    CORRADE_VERIFY(image);
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(image->format(), ColorFormat::BGR);
    #else
    CORRADE_COMPARE(image->format(), ColorFormat::RGB);
    #endif
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE(image->type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(std::string(reinterpret_cast<const char*>(image->data()), 2*3*3), std::string(pixels, 2*3*3));
}

void TgaImporterTest::colorBits32() {
    TgaImporter importer;
    const unsigned char data[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 32, 0,
        1, 2, 3, 1, 2, 3, 4, 1,
        3, 4, 5, 1, 4, 5, 6, 1,
        5, 6, 7, 1, 6, 7, 8, 1
    };
    #ifndef MAGNUM_TARGET_GLES
    const char* pixels = reinterpret_cast<const char*>(data) + 18;
    #else
    const char pixels[] = {
        3, 2, 1, 1, 4, 3, 2, 1,
        5, 4, 3, 1, 6, 5, 4, 1,
        7, 6, 5, 1, 8, 7, 6, 1
    };
    #endif
    CORRADE_VERIFY(importer.openData(data));

    std::optional<Trade::ImageData2D> image = importer.image2D(0);
    CORRADE_VERIFY(image);
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(image->format(), ColorFormat::BGRA);
    #else
    CORRADE_COMPARE(image->format(), ColorFormat::RGBA);
    #endif
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE(image->type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(std::string(reinterpret_cast<const char*>(image->data()), 2*3*3), std::string(pixels, 2*3*3));
}

void TgaImporterTest::grayscaleBits8() {
    TgaImporter importer;
    const unsigned char data[] = {
        0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 8, 0,
        1, 2,
        3, 4,
        5, 6
    };
    CORRADE_VERIFY(importer.openData(data));

    std::optional<Trade::ImageData2D> image = importer.image2D(0);
    CORRADE_VERIFY(image);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(image->format(), ColorFormat::Red);
    #else
    CORRADE_COMPARE(image->format(), ColorFormat::Luminance);
    #endif
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE(image->type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(std::string(reinterpret_cast<const char*>(image->data()), 2*3),
                    std::string(reinterpret_cast<const char*>(data) + 18, 2*3));
}

void TgaImporterTest::grayscaleBits16() {
    TgaImporter importer;
    const unsigned char data[] = { 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0 };
    CORRADE_VERIFY(importer.openData(data));

    std::ostringstream debug;
    Error::setOutput(&debug);
    CORRADE_VERIFY(!importer.image2D(0));
    CORRADE_COMPARE(debug.str(), "Trade::TgaImporter::image2D(): unsupported grayscale bits-per-pixel: 16\n");
}

void TgaImporterTest::file() {
    TgaImporter importer;
    const unsigned char data[] = {
        0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 8, 0,
        1, 2,
        3, 4,
        5, 6
    };
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(TGAIMPORTER_TEST_DIR, "file.tga")));

    std::optional<Trade::ImageData2D> image = importer.image2D(0);
    CORRADE_VERIFY(image);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(image->format(), ColorFormat::Red);
    #else
    CORRADE_COMPARE(image->format(), ColorFormat::Luminance);
    #endif
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE(image->type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(std::string(reinterpret_cast<const char*>(image->data()), 2*3),
                    std::string(reinterpret_cast<const char*>(data) + 18, 2*3));
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::TgaImporterTest)
