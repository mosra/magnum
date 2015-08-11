/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/PixelStorage.h"

namespace Magnum { namespace Test {

struct PixelStorageTest: TestSuite::Tester {
    explicit PixelStorageTest();

    void pixelSize();
    void dataSize();
};

PixelStorageTest::PixelStorageTest() {
    addTests({&PixelStorageTest::pixelSize,
              &PixelStorageTest::dataSize});
}

void PixelStorageTest::pixelSize() {
    CORRADE_COMPARE(Implementation::imagePixelSize(PixelFormat::RGBA, PixelType::UnsignedInt), 4*4);
    CORRADE_COMPARE(Implementation::imagePixelSize(PixelFormat::DepthComponent, PixelType::UnsignedShort), 2);
    CORRADE_COMPARE(Implementation::imagePixelSize(PixelFormat::StencilIndex, PixelType::UnsignedByte), 1);
    CORRADE_COMPARE(Implementation::imagePixelSize(PixelFormat::DepthStencil, PixelType::UnsignedInt248), 4);
}

void PixelStorageTest::dataSize() {
    /* Verify that row size is properly rounded */
    CORRADE_COMPARE(Image2D(PixelFormat::RGBA, PixelType::UnsignedByte).dataSize({}), 0);
    CORRADE_COMPARE(Image2D(PixelFormat::Red, PixelType::UnsignedByte).dataSize({4, 2}), 8);
    CORRADE_COMPARE(Image2D(PixelFormat::Red, PixelType::UnsignedByte).dataSize({2, 4}), 16);
    CORRADE_COMPARE(Image2D(PixelFormat::RGBA, PixelType::UnsignedByte).dataSize(Vector2i(1)), 4);

    CORRADE_COMPARE(Image2D(PixelFormat::RGBA, PixelType::UnsignedShort).dataSize({16, 8}),
                    4*2*16*8);
}

}}

CORRADE_TEST_MAIN(Magnum::Test::PixelStorageTest)
