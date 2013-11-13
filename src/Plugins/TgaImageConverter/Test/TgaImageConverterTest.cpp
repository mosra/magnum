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
#include <Containers/Array.h>
#include <TestSuite/Tester.h>
#include <Utility/Directory.h>

#include "ColorFormat.h"
#include "Image.h"
#include "Trade/ImageData.h"
#include "TgaImageConverter/TgaImageConverter.h"
#include "TgaImporter/TgaImporter.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test {

class TgaImageConverterTest: public TestSuite::Tester {
    public:
        explicit TgaImageConverterTest();

        void wrongFormat();
        void wrongType();

        void data();
};

namespace {
    #ifndef MAGNUM_TARGET_GLES
    const Image2D original(ColorFormat::BGR, ColorType::UnsignedByte, {2, 3}, new char[18]
    #else
    const Image2D original(ColorFormat::RGB, ColorType::UnsignedByte, {2, 3}, new char[18]
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

              &TgaImageConverterTest::data});
}

void TgaImageConverterTest::wrongFormat() {
    ImageReference2D image(ColorFormat::RG, ColorType::UnsignedByte, {}, nullptr);

    std::ostringstream out;
    Error::setOutput(&out);

    const auto data = TgaImageConverter().exportToData(image);
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(out.str(), "Trade::TgaImageConverter::convertToData(): unsupported image format ColorFormat::RG\n");
}

void TgaImageConverterTest::wrongType() {
    ImageReference2D image(ColorFormat::Red, ColorType::Float, {}, nullptr);

    std::ostringstream out;
    Error::setOutput(&out);

    const auto data = TgaImageConverter().exportToData(image);
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(out.str(), "Trade::TgaImageConverter::convertToData(): unsupported image type ColorType::Float\n");
}

void TgaImageConverterTest::data() {
    const auto data = TgaImageConverter().exportToData(original);

    TgaImporter importer;
    CORRADE_VERIFY(importer.openData(data));
    std::optional<Trade::ImageData2D> converted = importer.image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(converted->format(), ColorFormat::BGR);
    #else
    CORRADE_COMPARE(converted->format(), ColorFormat::RGB);
    #endif
    CORRADE_COMPARE(converted->type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(std::string(reinterpret_cast<const char*>(converted->data()), 2*3*3),
                    std::string(reinterpret_cast<const char*>(original.data()), 2*3*3));
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::TgaImageConverterTest)
