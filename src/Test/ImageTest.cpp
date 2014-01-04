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

#include <TestSuite/Tester.h>

#include "ColorFormat.h"
#include "Image.h"

namespace Magnum { namespace Test {

class ImageTest: public TestSuite::Tester {
    public:
        explicit ImageTest();

        void construct();
        void constructCopy();
        void constructMove();

        void setData();
        void toReference();
        void release();
};

ImageTest::ImageTest() {
    addTests<ImageTest>({&ImageTest::construct,
              &ImageTest::constructCopy,
              &ImageTest::constructMove,

              &ImageTest::setData,
              &ImageTest::toReference,
              &ImageTest::release});
}

void ImageTest::construct() {
    auto data = new unsigned char[3];
    Image2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);

    CORRADE_COMPARE(a.format(), ColorFormat::Red);
    CORRADE_COMPARE(a.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), data);
}

void ImageTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Image2D, const Image2D&>::value));
    /* GCC 4.6 doesn't have std::is_assignable */
    #ifndef CORRADE_GCC46_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_assignable<Image2D, const Image2D&>{}));
    #endif
}

void ImageTest::constructMove() {
    auto data = new unsigned char[3];
    Image2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);
    Image2D b(std::move(a));

    CORRADE_COMPARE(a.data(), static_cast<unsigned char*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_COMPARE(b.format(), ColorFormat::Red);
    CORRADE_COMPARE(b.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);

    auto data2 = new unsigned char[3];
    Image2D c(ColorFormat::RGBA, ColorType::UnsignedShort, {2, 6}, data2);
    c = std::move(b);

    CORRADE_COMPARE(b.data(), data2);
    CORRADE_COMPARE(b.size(), Vector2i(2, 6));

    CORRADE_COMPARE(c.format(), ColorFormat::Red);
    CORRADE_COMPARE(c.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(c.size(), Vector2i(1, 3));
    CORRADE_COMPARE(c.data(), data);
}

void ImageTest::setData() {
    auto data = new unsigned char[3];
    Image2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);
    auto data2 = new unsigned short[2*4];
    a.setData(ColorFormat::RGBA, ColorType::UnsignedShort, {2, 1}, data2);

    CORRADE_COMPARE(a.format(), ColorFormat::RGBA);
    CORRADE_COMPARE(a.type(), ColorType::UnsignedShort);
    CORRADE_COMPARE(a.size(), Vector2i(2, 1));
    CORRADE_COMPARE(a.data(), reinterpret_cast<unsigned char*>(data2));
}

void ImageTest::toReference() {
    auto data = new unsigned char[3];
    const Image2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);
    ImageReference2D b = a;

    CORRADE_COMPARE(b.format(), ColorFormat::Red);
    CORRADE_COMPARE(b.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);

    CORRADE_VERIFY((std::is_convertible<const Image2D&, ImageReference2D>::value));
    {
        #ifdef CORRADE_GCC47_COMPATIBILITY
        CORRADE_EXPECT_FAIL("Rvalue references for *this are not supported in GCC < 4.8.1.");
        #endif
        CORRADE_VERIFY(!(std::is_convertible<const Image2D, ImageReference2D>::value));
        CORRADE_VERIFY(!(std::is_convertible<const Image2D&&, ImageReference2D>::value));
    }
}

void ImageTest::release() {
    unsigned char data[] = {'c', 'a', 'f', 'e'};
    Image2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 4}, data);
    const unsigned char* const pointer = a.release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), static_cast<unsigned char*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i());
}

}}

CORRADE_TEST_MAIN(Magnum::Test::ImageTest)
