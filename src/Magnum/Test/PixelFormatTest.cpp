/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/PixelFormat.h"

namespace Magnum { namespace Test { namespace {

struct PixelFormatTest: TestSuite::Tester {
    explicit PixelFormatTest();

    void size();
    void sizeImplementationSpecific();

    void isImplementationSpecific();
    void wrap();
    void wrapInvalid();
    void unwrap();
    void unwrapInvalid();

    void compressedIsImplementationSpecific();
    void compressedWrap();
    void compressedWrapInvalid();
    void compressedUnwrap();
    void compressedUnwrapInvalid();

    void debug();
    void debugImplementationSpecific();

    void compressedDebug();
    void compressedDebugImplementationSpecific();
};

PixelFormatTest::PixelFormatTest() {
    addTests({&PixelFormatTest::size,
              &PixelFormatTest::sizeImplementationSpecific,

              &PixelFormatTest::isImplementationSpecific,
              &PixelFormatTest::wrap,
              &PixelFormatTest::wrapInvalid,
              &PixelFormatTest::unwrap,
              &PixelFormatTest::unwrapInvalid,

              &PixelFormatTest::compressedIsImplementationSpecific,
              &PixelFormatTest::compressedWrap,
              &PixelFormatTest::compressedWrapInvalid,
              &PixelFormatTest::compressedUnwrap,
              &PixelFormatTest::compressedUnwrapInvalid,

              &PixelFormatTest::debug,
              &PixelFormatTest::debugImplementationSpecific,

              &PixelFormatTest::compressedDebug,
              &PixelFormatTest::compressedDebugImplementationSpecific});
}

void PixelFormatTest::size() {
    CORRADE_COMPARE(pixelSize(PixelFormat::R8I), 1);
    CORRADE_COMPARE(pixelSize(PixelFormat::R16UI), 2);
    CORRADE_COMPARE(pixelSize(PixelFormat::RGB8Unorm), 3);
    CORRADE_COMPARE(pixelSize(PixelFormat::RGBA8Snorm), 4);
    CORRADE_COMPARE(pixelSize(PixelFormat::RGB16I), 6);
    CORRADE_COMPARE(pixelSize(PixelFormat::RGBA16F), 8);
    CORRADE_COMPARE(pixelSize(PixelFormat::RGB32UI), 12);
    CORRADE_COMPARE(pixelSize(PixelFormat::RGBA32F), 16);
}

void PixelFormatTest::sizeImplementationSpecific() {
    std::ostringstream out;
    Error redirectError{&out};

    pixelSize(pixelFormatWrap(0xdead));

    CORRADE_COMPARE(out.str(), "pixelSize(): can't determine pixel size of an implementation-specific format\n");
}

void PixelFormatTest::isImplementationSpecific() {
    constexpr bool a = isPixelFormatImplementationSpecific(PixelFormat::RGBA8Unorm);
    constexpr bool b = isPixelFormatImplementationSpecific(PixelFormat(0x8000dead));
    CORRADE_VERIFY(!a);
    CORRADE_VERIFY(b);
}

void PixelFormatTest::wrap() {
    constexpr PixelFormat a = pixelFormatWrap(0xdead);
    CORRADE_COMPARE(UnsignedInt(a), 0x8000dead);
}

void PixelFormatTest::wrapInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    pixelFormatWrap(0xdeadbeef);

    CORRADE_COMPARE(out.str(), "pixelFormatWrap(): implementation-specific value already wrapped or too large\n");
}

void PixelFormatTest::unwrap() {
    constexpr UnsignedInt a = pixelFormatUnwrap(PixelFormat(0x8000dead));
    CORRADE_COMPARE(a, 0xdead);
}

void PixelFormatTest::unwrapInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    pixelFormatUnwrap(PixelFormat(0xdead));

    CORRADE_COMPARE(out.str(), "pixelFormatUnwrap(): format doesn't contain a wrapped implementation-specific value\n");
}

void PixelFormatTest::compressedIsImplementationSpecific() {
    CORRADE_VERIFY(!isPixelFormatImplementationSpecific(PixelFormat::RGBA8Unorm));
    CORRADE_VERIFY(isPixelFormatImplementationSpecific(pixelFormatWrap(0xdead)));
}

void PixelFormatTest::compressedWrap() {
    CORRADE_COMPARE(UnsignedInt(pixelFormatWrap(0xdead)), 0x8000dead);
}

void PixelFormatTest::compressedWrapInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    compressedPixelFormatWrap(0xdeadbeef);

    CORRADE_COMPARE(out.str(), "compressedPixelFormatWrap(): implementation-specific value already wrapped or too large\n");
}

void PixelFormatTest::compressedUnwrap() {
    CORRADE_COMPARE(UnsignedInt(compressedPixelFormatUnwrap(CompressedPixelFormat(0x8000dead))), 0xdead);
}

void PixelFormatTest::compressedUnwrapInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    compressedPixelFormatUnwrap(CompressedPixelFormat(0xdead));

    CORRADE_COMPARE(out.str(), "compressedPixelFormatUnwrap(): format doesn't contain a wrapped implementation-specific value\n");
}

void PixelFormatTest::debug() {
    std::ostringstream out;
    Debug{&out} << PixelFormat::RG16Snorm << PixelFormat(0xdead);

    CORRADE_COMPARE(out.str(), "PixelFormat::RG16Snorm PixelFormat(0xdead)\n");
}

void PixelFormatTest::debugImplementationSpecific() {
    std::ostringstream out;
    Debug{&out} << pixelFormatWrap(0xdead);

    CORRADE_COMPARE(out.str(), "PixelFormat::ImplementationSpecific(0xdead)\n");
}

void PixelFormatTest::compressedDebug() {
    std::ostringstream out;
    Debug{&out} << CompressedPixelFormat::Bc3RGBAUnorm << CompressedPixelFormat(0xdead);

    CORRADE_COMPARE(out.str(), "CompressedPixelFormat::Bc3RGBAUnorm CompressedPixelFormat(0xdead)\n");
}

void PixelFormatTest::compressedDebugImplementationSpecific() {
    std::ostringstream out;
    Debug{&out} << compressedPixelFormatWrap(0xdead);

    CORRADE_COMPARE(out.str(), "CompressedPixelFormat::ImplementationSpecific(0xdead)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::PixelFormatTest)
