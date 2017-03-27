/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
#include <tuple>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/ImageData.h"
#include "MagnumPlugins/TgaImageConverter/TgaImageConverter.h"
#include "MagnumPlugins/TgaImporter/TgaImporter.h"

namespace Magnum { namespace Trade { namespace Test {

struct TgaImageConverterTest: TestSuite::Tester {
    explicit TgaImageConverterTest();

    void wrongFormat();
    void wrongType();

    void rgb();
    void rgba();
};

namespace {
    /* Padded to four byte alignment (the resulting file is *not* padded) */
    constexpr char OriginalDataRGB[] = {
        /* Skip */
        0, 0, 0, 0, 0, 0, 0, 0,

        1, 2, 3, 2, 3, 4, 0, 0,
        3, 4, 5, 4, 5, 6, 0, 0,
        5, 6, 7, 6, 7, 8, 0, 0
    };
    constexpr char ConvertedDataRGB[] = {
        1, 2, 3, 2, 3, 4,
        3, 4, 5, 4, 5, 6,
        5, 6, 7, 6, 7, 8
    };

    const ImageView2D OriginalRGB{PixelStorage{}.setSkip({0, 1, 0}),
        PixelFormat::RGB, PixelType::UnsignedByte, {2, 3}, OriginalDataRGB};

    constexpr char OriginalDataRGBA[] = {
        1, 2, 3, 4, 2, 3, 4, 5,
        3, 4, 5, 6, 4, 5, 6, 7,
        5, 6, 7, 8, 6, 7, 8, 9
    };
    const ImageView2D OriginalRGBA{PixelFormat::RGBA, PixelType::UnsignedByte, {2, 3}, OriginalDataRGBA};
}

TgaImageConverterTest::TgaImageConverterTest() {
    addTests({&TgaImageConverterTest::wrongFormat,
              &TgaImageConverterTest::wrongType,

              &TgaImageConverterTest::rgb,
              &TgaImageConverterTest::rgba});
}

void TgaImageConverterTest::wrongFormat() {
    ImageView2D image{
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        PixelFormat::RG,
        #else
        PixelFormat::LuminanceAlpha,
        #endif
        PixelType::UnsignedByte, {}, nullptr};

    std::ostringstream out;
    Error redirectError{&out};

    const auto data = TgaImageConverter().exportToData(image);
    CORRADE_VERIFY(!data);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    CORRADE_COMPARE(out.str(), "Trade::TgaImageConverter::exportToData(): unsupported color format PixelFormat::RG\n");
    #else
    CORRADE_COMPARE(out.str(), "Trade::TgaImageConverter::exportToData(): unsupported color format PixelFormat::LuminanceAlpha\n");
    #endif
}

void TgaImageConverterTest::wrongType() {
    ImageView2D image{
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        PixelFormat::Red,
        #else
        PixelFormat::Luminance,
        #endif
        PixelType::Float, {}, nullptr};

    std::ostringstream out;
    Error redirectError{&out};

    const auto data = TgaImageConverter().exportToData(image);
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(out.str(), "Trade::TgaImageConverter::exportToData(): unsupported color type PixelType::Float\n");
}

void TgaImageConverterTest::rgb() {
    const auto data = TgaImageConverter().exportToData(OriginalRGB);

    TgaImporter importer;
    CORRADE_VERIFY(importer.openData(data));
    std::optional<Trade::ImageData2D> converted = importer.image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->storage().alignment(), 1);
    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGB);
    CORRADE_COMPARE(converted->type(), PixelType::UnsignedByte);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(ConvertedDataRGB),
        TestSuite::Compare::Container);
}

void TgaImageConverterTest::rgba() {
    const auto data = TgaImageConverter().exportToData(OriginalRGBA);

    TgaImporter importer;
    CORRADE_VERIFY(importer.openData(data));
    std::optional<Trade::ImageData2D> converted = importer.image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->storage().alignment(), 4);
    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGBA);
    CORRADE_COMPARE(converted->type(), PixelType::UnsignedByte);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(OriginalDataRGBA),
        TestSuite::Compare::Container);
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::TgaImageConverterTest)
