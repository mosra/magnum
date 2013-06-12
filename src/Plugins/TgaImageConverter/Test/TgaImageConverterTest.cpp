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
#include <tuple>
#include <TestSuite/Tester.h>
#include <Utility/Directory.h>
#include <Image.h>
#include <ImageFormat.h>
#include <Trade/ImageData.h>
#include <TgaImageConverter/TgaImageConverter.h>
#include <TgaImporter/TgaImporter.h>

#include "configure.h"

namespace Magnum { namespace Trade { namespace TgaImageConverter { namespace Test {

class TgaImageConverterTest: public TestSuite::Tester {
    public:
        explicit TgaImageConverterTest();

        void wrongFormat();
        void wrongType();

        void data();
        void file();
};

namespace {
    #ifndef MAGNUM_TARGET_GLES
    const Image2D original({2, 3}, ImageFormat::BGR, ImageType::UnsignedByte, new char[18]
    #else
    const Image2D original({2, 3}, ImageFormat::RGB, ImageType::UnsignedByte, new char[18]
    #endif
    {
        1, 2, 3, 2, 3, 4,
        3, 4, 5, 4, 5, 6,
        5, 6, 7, 6, 7, 8
    });
}

TgaImageConverterTest::TgaImageConverterTest() {
    addTests({&TgaImageConverterTest::wrongFormat,
              &TgaImageConverterTest::wrongType,

              &TgaImageConverterTest::data,
              &TgaImageConverterTest::file});
}

void TgaImageConverterTest::wrongFormat() {
    Image2D image({}, ImageFormat::RG, ImageType::UnsignedByte, nullptr);

    std::ostringstream out;
    Error::setOutput(&out);

    const unsigned char* data;
    std::tie(data, std::ignore) = TgaImageConverter().convertToData(&image);
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(out.str(), "Trade::TgaImageConverter::TgaImageConverter::convertToData(): unsupported image format ImageFormat::RG\n");
}

void TgaImageConverterTest::wrongType() {
    Image2D image({}, ImageFormat::Red, ImageType::Float, nullptr);

    std::ostringstream out;
    Error::setOutput(&out);

    const unsigned char* data;
    std::tie(data, std::ignore) = TgaImageConverter().convertToData(&image);
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(out.str(), "Trade::TgaImageConverter::TgaImageConverter::convertToData(): unsupported image type ImageType::Float\n");
}

void TgaImageConverterTest::data() {
    const unsigned char* data;
    std::size_t size;
    std::tie(data, size) = TgaImageConverter().convertToData(&original);

    TgaImporter::TgaImporter importer;
    CORRADE_VERIFY(importer.openData(data, size));
    Trade::ImageData2D* converted = importer.image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(converted->format(), ImageFormat::BGR);
    #else
    CORRADE_COMPARE(converted->format(), ImageFormat::RGB);
    #endif
    CORRADE_COMPARE(converted->type(), ImageType::UnsignedByte);
    CORRADE_COMPARE(std::string(reinterpret_cast<const char*>(converted->data()), 2*3*3),
                    std::string(reinterpret_cast<const char*>(original.data()), 2*3*3));
}

void TgaImageConverterTest::file() {
    const std::string filename = Utility::Directory::join(TGAIMAGECONVERTER_TEST_DIR, "file.tga");
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(TgaImageConverter().convertToFile(&original, filename));

    TgaImporter::TgaImporter importer;
    CORRADE_VERIFY(importer.openFile(filename));
    Trade::ImageData2D* converted = importer.image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(converted->format(), ImageFormat::BGR);
    #else
    CORRADE_COMPARE(converted->format(), ImageFormat::RGB);
    #endif
    CORRADE_COMPARE(converted->type(), ImageType::UnsignedByte);
    CORRADE_COMPARE(std::string(reinterpret_cast<const char*>(converted->data()), 2*3*3),
                    std::string(reinterpret_cast<const char*>(original.data()), 2*3*3));

    Utility::Directory::rm(filename);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::TgaImageConverter::Test::TgaImageConverterTest)
