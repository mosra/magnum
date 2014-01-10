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

#include <Corrade/TestSuite/Compare/Container.h>

#include "BufferImage.h"
#include "ColorFormat.h"
#include "Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class BufferImageTest: public AbstractOpenGLTester {
    public:
        explicit BufferImageTest();

        void construct();
        void constructCopy();
        void constructMove();

        void setData();
};

BufferImageTest::BufferImageTest() {
    addTests({&BufferImageTest::construct,
              &BufferImageTest::constructCopy,
              &BufferImageTest::constructMove,

              &BufferImageTest::setData});
}

void BufferImageTest::construct() {
    const unsigned char data[] = { 'a', 'b', 'c' };
    BufferImage2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data<UnsignedByte>();
    #endif

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(a.format(), ColorFormat::Red);
    CORRADE_COMPARE(a.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
                       std::vector<UnsignedByte>(data, data + 3),
                       TestSuite::Compare::Container);
    #endif
}

void BufferImageTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<BufferImage2D, const BufferImage2D&>{}));
    /* GCC 4.6 doesn't have std::is_assignable */
    #ifndef CORRADE_GCC46_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_assignable<BufferImage2D, const BufferImage2D&>{}));
    #endif
}

void BufferImageTest::constructMove() {
    const unsigned char data[3] = { 'a', 'b', 'c' };
    BufferImage2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data, BufferUsage::StaticDraw);
    const Int id = a.buffer().id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    BufferImage2D b(std::move(a));

    CORRADE_COMPARE(a.buffer().id(), 0);
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_COMPARE(b.format(), ColorFormat::Red);
    CORRADE_COMPARE(b.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.buffer().id(), id);

    const unsigned short data2[2*4] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    BufferImage2D c(ColorFormat::RGBA, ColorType::UnsignedShort, {2, 1}, data2, BufferUsage::StaticDraw);
    const Int cId = c.buffer().id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.buffer().id(), cId);
    CORRADE_COMPARE(b.size(), Vector2i(2, 1));

    CORRADE_COMPARE(c.format(), ColorFormat::Red);
    CORRADE_COMPARE(c.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(c.size(), Vector2i(1, 3));
    CORRADE_COMPARE(c.buffer().id(), id);
}

void BufferImageTest::setData() {
    const unsigned char data[3] = { 'a', 'b', 'c' };
    BufferImage2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data, BufferUsage::StaticDraw);

    const unsigned short data2[2*4] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    a.setData(ColorFormat::RGBA, ColorType::UnsignedShort, {2, 1}, data2, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data<UnsignedShort>();
    #endif

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(a.format(), ColorFormat::RGBA);
    CORRADE_COMPARE(a.type(), ColorType::UnsignedShort);
    CORRADE_COMPARE(a.size(), Vector2i(2, 1));

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(std::vector<UnsignedShort>(imageData.begin(), imageData.end()),
                       std::vector<UnsignedShort>(data2, data2 + 8),
                       TestSuite::Compare::Container);
    #endif
}

}}

CORRADE_TEST_MAIN(Magnum::Test::BufferImageTest)
