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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/AbstractImage.h"
#include "Magnum/Image.h"
#include "Magnum/ColorFormat.h"

namespace Magnum { namespace Test {

class AbstractImageTest: public TestSuite::Tester {
    public:
        explicit AbstractImageTest();

        void pixelSize();
        void dataSize();

        void debugFormat();
        void debugType();
};

AbstractImageTest::AbstractImageTest() {
    addTests<AbstractImageTest>({&AbstractImageTest::pixelSize,
              &AbstractImageTest::dataSize,

              &AbstractImageTest::debugFormat,
              &AbstractImageTest::debugType});
}

void AbstractImageTest::pixelSize() {
    CORRADE_COMPARE(AbstractImage::pixelSize(ColorFormat::RGBA, ColorType::UnsignedInt), 4*4);
    CORRADE_COMPARE(AbstractImage::pixelSize(ColorFormat::DepthComponent, ColorType::UnsignedShort), 2);
    CORRADE_COMPARE(AbstractImage::pixelSize(ColorFormat::StencilIndex, ColorType::UnsignedByte), 1);
    CORRADE_COMPARE(AbstractImage::pixelSize(ColorFormat::DepthStencil, ColorType::UnsignedInt248), 4);
}

void AbstractImageTest::dataSize() {
    /* Verify that row size is properly rounded */
    CORRADE_COMPARE(Image2D(ColorFormat::RGBA, ColorType::UnsignedByte).dataSize({}), 0);
    CORRADE_COMPARE(Image2D(ColorFormat::Red, ColorType::UnsignedByte).dataSize({4, 2}), 8);
    CORRADE_COMPARE(Image2D(ColorFormat::Red, ColorType::UnsignedByte).dataSize({2, 4}), 16);
    CORRADE_COMPARE(Image2D(ColorFormat::RGBA, ColorType::UnsignedByte).dataSize(Vector2i(1)), 4);

    CORRADE_COMPARE(Image2D(ColorFormat::RGBA, ColorType::UnsignedShort).dataSize({16, 8}),
                    4*2*16*8);
}

void AbstractImageTest::debugFormat() {
    std::ostringstream o;
    Debug(&o) << ColorFormat::RGBA;
    CORRADE_COMPARE(o.str(), "ColorFormat::RGBA\n");
}

void AbstractImageTest::debugType() {
    std::ostringstream o;
    Debug(&o) << ColorType::UnsignedShort5551;
    CORRADE_COMPARE(o.str(), "ColorType::UnsignedShort5551\n");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::AbstractImageTest)
