/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/ColorFormat.h"
#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace Trade { namespace Test {

class ImageDataTest: public TestSuite::Tester {
    public:
        explicit ImageDataTest();

        void construct();
        void constructCopy();
        void constructMove();

        void toReference();
        void release();
};

ImageDataTest::ImageDataTest() {
    addTests<ImageDataTest>({&ImageDataTest::construct,
              &ImageDataTest::constructCopy,
              &ImageDataTest::constructMove,

              &ImageDataTest::toReference,
              &ImageDataTest::release});
}

void ImageDataTest::construct() {
    auto data = new unsigned char[3];
    Trade::ImageData2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);

    CORRADE_COMPARE(a.format(), ColorFormat::Red);
    CORRADE_COMPARE(a.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), data);
}

void ImageDataTest::constructCopy() {
    #ifndef CORRADE_GCC44_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_constructible<Trade::ImageData2D, const Trade::ImageData2D&>::value));
    /* GCC 4.6 doesn't have std::is_assignable */
    #ifndef CORRADE_GCC46_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_assignable<Trade::ImageData2D, const Trade::ImageData2D&>{}));
    #endif
    #else
    CORRADE_SKIP("Type traits needed to test this are not available on GCC 4.4.");
    #endif
}

void ImageDataTest::constructMove() {
    auto data = new unsigned char[3];
    Trade::ImageData2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);
    Trade::ImageData2D b(std::move(a));

    CORRADE_COMPARE(a.data(), static_cast<unsigned char*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_COMPARE(b.format(), ColorFormat::Red);
    CORRADE_COMPARE(b.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);

    auto data2 = new unsigned char[3];
    Trade::ImageData2D c(ColorFormat::RGBA, ColorType::UnsignedShort, {2, 6}, data2);
    c = std::move(b);

    CORRADE_COMPARE(b.data(), data2);
    CORRADE_COMPARE(b.size(), Vector2i(2, 6));

    CORRADE_COMPARE(c.format(), ColorFormat::Red);
    CORRADE_COMPARE(c.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(c.size(), Vector2i(1, 3));
    CORRADE_COMPARE(c.data(), data);
}

void ImageDataTest::toReference() {
    auto data = new unsigned char[3];
    const Trade::ImageData2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);
    ImageReference2D b = a;

    CORRADE_COMPARE(b.format(), ColorFormat::Red);
    CORRADE_COMPARE(b.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);

    CORRADE_VERIFY((std::is_convertible<const Trade::ImageData2D&, ImageReference2D>::value));
    {
        #ifdef CORRADE_GCC47_COMPATIBILITY
        CORRADE_EXPECT_FAIL("Rvalue references for *this are not supported in GCC < 4.8.1.");
        #endif
        CORRADE_VERIFY(!(std::is_convertible<const Trade::ImageData2D, ImageReference2D>::value));
        CORRADE_VERIFY(!(std::is_convertible<const Trade::ImageData2D&&, ImageReference2D>::value));
    }
}

void ImageDataTest::release() {
    unsigned char data[] = {'b', 'e', 'e', 'r'};
    ImageData2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 4}, data);
    const unsigned char* const pointer = a.release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), static_cast<unsigned char*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i());
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ImageDataTest)
