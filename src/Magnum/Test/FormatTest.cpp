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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/ColorFormat.h"

namespace Magnum { namespace Test {

struct FormatTest: TestSuite::Tester {
    explicit FormatTest();

    void debugColorFormat();
    void debugColorType();
    void debugCompressedColorFormat();
};

FormatTest::FormatTest() {
    addTests({&FormatTest::debugColorFormat,
              &FormatTest::debugColorType,
              &FormatTest::debugCompressedColorFormat});
}

void FormatTest::debugColorFormat() {
    std::ostringstream out;

    Debug(&out) << ColorFormat::RGBA;
    CORRADE_COMPARE(out.str(), "ColorFormat::RGBA\n");
}

void FormatTest::debugColorType() {
    std::ostringstream out;

    Debug(&out) << ColorType::UnsignedByte;
    CORRADE_COMPARE(out.str(), "ColorType::UnsignedByte\n");
}

void FormatTest::debugCompressedColorFormat() {
    #ifdef MAGNUM_TARGET_GLES
    CORRADE_SKIP("No enum value available");
    #else
    std::ostringstream out;

    Debug(&out) << CompressedColorFormat::RGBBptcUnsignedFloat;
    CORRADE_COMPARE(out.str(), "CompressedColorFormat::RGBBptcUnsignedFloat\n");
    #endif
}

}}

CORRADE_TEST_MAIN(Magnum::Test::FormatTest)
