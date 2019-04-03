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
#include "Magnum/GL/PixelFormat.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct PixelFormatTest: TestSuite::Tester {
    explicit PixelFormatTest();

    void mapFormatType();
    void mapFormatImplementationSpecific();
    void mapFormatUnsupported();
    void mapFormatInvalid();
    void mapTypeImplementationSpecific();
    void mapTypeImplementationSpecificZero();
    void mapTypeUnsupported();
    void mapTypeInvalid();

    void size();
    void sizeInvalid();

    void mapCompressedFormat();
    void mapCompressedFormatImplementationSpecific();
    void mapCompressedFormatUnsupported();
    void mapCompressedFormatInvalid();

    void debugPixelFormat();
    void debugPixelType();

    void debugCompressedPixelFormat();
};

PixelFormatTest::PixelFormatTest() {
    addTests({&PixelFormatTest::mapFormatType,
              &PixelFormatTest::mapFormatImplementationSpecific,
              &PixelFormatTest::mapFormatUnsupported,
              &PixelFormatTest::mapFormatInvalid,
              &PixelFormatTest::mapTypeImplementationSpecific,
              &PixelFormatTest::mapTypeImplementationSpecificZero,
              &PixelFormatTest::mapTypeUnsupported,
              &PixelFormatTest::mapTypeInvalid,

              &PixelFormatTest::size,
              &PixelFormatTest::sizeInvalid,

              &PixelFormatTest::mapCompressedFormat,
              &PixelFormatTest::mapCompressedFormatImplementationSpecific,
              &PixelFormatTest::mapCompressedFormatUnsupported,
              &PixelFormatTest::mapCompressedFormatInvalid,

              &PixelFormatTest::debugPixelFormat,
              &PixelFormatTest::debugPixelType,
              &PixelFormatTest::debugCompressedPixelFormat});
}

void PixelFormatTest::mapFormatType() {
    /* Touchstone verification */
    CORRADE_VERIFY(hasPixelFormat(Magnum::PixelFormat::RGBA8Unorm));
    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::RGBA8Unorm), PixelFormat::RGBA);
    CORRADE_COMPARE(pixelType(Magnum::PixelFormat::RGBA8Unorm), PixelType::UnsignedByte);

    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 0;
    for(UnsignedInt i = 0; i <= 0xffff; ++i) {
        const auto format = Magnum::PixelFormat(i);
        /* Each case verifies:
           - that the cases are ordered by number (so insertion here is done in
             proper place)
           - that there was no gap (unhandled value inside the range)
           - that a particular pixel format maps to a particular GL format
           - that a particular pixel type maps to a particular GL type */
        switch(format) {
            #define _c(format, expectedFormat, expectedType) \
                case Magnum::PixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasPixelFormat(Magnum::PixelFormat::format)); \
                    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::format), Magnum::GL::PixelFormat::expectedFormat); \
                    CORRADE_COMPARE(pixelType(Magnum::PixelFormat::format), Magnum::GL::PixelType::expectedType); \
                    ++nextHandled; \
                    continue;
            #define _s(format) \
                case Magnum::PixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasPixelFormat(Magnum::PixelFormat::format)); \
                    pixelFormat(Magnum::PixelFormat::format); \
                    pixelType(Magnum::PixelFormat::format); \
                    ++nextHandled; \
                    continue;
            #include "Magnum/GL/Implementation/pixelFormatMapping.hpp"
            #undef _s
            #undef _c
        }

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xffff);
}

void PixelFormatTest::mapFormatImplementationSpecific() {
    CORRADE_VERIFY(hasPixelFormat(Magnum::pixelFormatWrap(PixelFormat::RGBA)));
    CORRADE_COMPARE(pixelFormat(Magnum::pixelFormatWrap(PixelFormat::RGBA)),
        PixelFormat::RGBA);
}

void PixelFormatTest::mapFormatUnsupported() {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("All pixel formats are supported on ES3+.");
    #else
    std::ostringstream out;
    Error redirectError{&out};

    pixelFormat(Magnum::PixelFormat::RGB16UI);
    CORRADE_COMPARE(out.str(), "GL::pixelFormat(): format PixelFormat::RGB16UI is not supported on this target\n");
    #endif
}

void PixelFormatTest::mapFormatInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    hasPixelFormat(Magnum::PixelFormat(0x123));
    pixelFormat(Magnum::PixelFormat(0x123));
    CORRADE_COMPARE(out.str(),
        "GL::hasPixelFormat(): invalid format PixelFormat(0x123)\n"
        "GL::pixelFormat(): invalid format PixelFormat(0x123)\n");
}

void PixelFormatTest::mapTypeImplementationSpecific() {
    CORRADE_COMPARE(pixelType(Magnum::pixelFormatWrap(PixelFormat::RGBA), GL_UNSIGNED_BYTE),
        PixelType::UnsignedByte);
}

void PixelFormatTest::mapTypeImplementationSpecificZero() {
    std::ostringstream out;
    Error redirectError{&out};

    pixelType(Magnum::pixelFormatWrap(PixelFormat::RGBA));
    CORRADE_COMPARE(out.str(), "GL::pixelType(): format is implementation-specific, but no additional type specifier was passed\n");
}

void PixelFormatTest::mapTypeUnsupported() {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("All pixel formats are supported on ES3+");
    #else
    CORRADE_VERIFY(!hasPixelFormat(Magnum::PixelFormat::RGBA16UI));

    std::ostringstream out;
    Error redirectError{&out};
    pixelType(Magnum::PixelFormat::RGB16UI);
    CORRADE_COMPARE(out.str(), "GL::pixelType(): format PixelFormat::RGB16UI is not supported on this target\n");
    #endif
}

void PixelFormatTest::mapTypeInvalid() {
    std::ostringstream out;
    Error redirectError{&out};
    pixelType(Magnum::PixelFormat(0x123));
    CORRADE_COMPARE(out.str(), "GL::pixelType(): invalid format PixelFormat(0x123)\n");
}

void PixelFormatTest::size() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(pixelSize(PixelFormat::RGB, PixelType::UnsignedByte332), 1);
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE(pixelSize(PixelFormat::StencilIndex, PixelType::UnsignedByte), 1);
    #endif
    CORRADE_COMPARE(pixelSize(PixelFormat::DepthComponent, PixelType::UnsignedShort), 2);
    CORRADE_COMPARE(pixelSize(PixelFormat::RGBA, PixelType::UnsignedShort4444), 2);
    CORRADE_COMPARE(pixelSize(PixelFormat::DepthStencil, PixelType::UnsignedInt248), 4);
    CORRADE_COMPARE(pixelSize(PixelFormat::RGBA, PixelType::UnsignedInt), 4*4);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(pixelSize(PixelFormat::DepthStencil, PixelType::Float32UnsignedInt248Rev), 8);
    #endif
}

void PixelFormatTest::sizeInvalid() {
    std::ostringstream out;
    Error redirectError{&out};
    pixelSize(PixelFormat::DepthStencil, PixelType::Float);
    CORRADE_COMPARE(out.str(), "GL::pixelSize(): invalid GL::PixelType::Float specified for GL::PixelFormat::DepthStencil\n");
}

void PixelFormatTest::mapCompressedFormat() {
    /* Touchstone verification */
    CORRADE_VERIFY(hasCompressedPixelFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm));
    CORRADE_COMPARE(compressedPixelFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm), CompressedPixelFormat::RGBAS3tcDxt1);

    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 0;
    for(UnsignedInt i = 0; i <= 0xffff; ++i) {
        const auto format = Magnum::CompressedPixelFormat(i);
        /* Each case verifies:
           - that the cases are ordered by number (so insertion here is done in
             proper place)
           - that there was no gap (unhandled value inside the range)
           - that a particular pixel format maps to a particular GL format
           - that a particular pixel type maps to a particular GL type */
        switch(format) {
            #define _c(format, expectedFormat) \
                case Magnum::CompressedPixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasCompressedPixelFormat(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_COMPARE(compressedPixelFormat(Magnum::CompressedPixelFormat::format), Magnum::GL::CompressedPixelFormat::expectedFormat); \
                    ++nextHandled; \
                    continue;
            #define _s(format) \
                case Magnum::CompressedPixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasCompressedPixelFormat(Magnum::CompressedPixelFormat::format)); \
                    compressedPixelFormat(Magnum::CompressedPixelFormat::format); \
                    ++nextHandled; \
                    continue;
            #include "Magnum/GL/Implementation/compressedPixelFormatMapping.hpp"
            #undef _s
            #undef _c
        }

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xffff);
}

void PixelFormatTest::mapCompressedFormatImplementationSpecific() {
    CORRADE_VERIFY(hasCompressedPixelFormat(Magnum::compressedPixelFormatWrap(CompressedPixelFormat::RGBAS3tcDxt1)));
    CORRADE_COMPARE(compressedPixelFormat(Magnum::compressedPixelFormatWrap(CompressedPixelFormat::RGBAS3tcDxt1)),
        CompressedPixelFormat::RGBAS3tcDxt1);
}
void PixelFormatTest::mapCompressedFormatUnsupported() {
    #if 1
    CORRADE_SKIP("All compressed pixel formats are currently supported everywhere.");
    #else
    CORRADE_VERIFY(!hasCompressedPixelFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm));

    std::ostringstream out;
    Error redirectError{&out};
    compressedPixelFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm);
    CORRADE_COMPARE(out.str(), "GL::compressedPixelFormat(): format CompressedPixelFormat::Bc1RGBAUnorm is not supported on this target\n");
    #endif
}

void PixelFormatTest::mapCompressedFormatInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    hasCompressedPixelFormat(Magnum::CompressedPixelFormat(0x123));
    compressedPixelFormat(Magnum::CompressedPixelFormat(0x123));
    CORRADE_COMPARE(out.str(),
        "GL::hasCompressedPixelFormat(): invalid format CompressedPixelFormat(0x123)\n"
        "GL::compressedPixelFormat(): invalid format CompressedPixelFormat(0x123)\n");
}

void PixelFormatTest::debugPixelFormat() {
    std::ostringstream out;

    Debug(&out) << PixelFormat::RGBA << PixelFormat(0xdead);
    CORRADE_COMPARE(out.str(), "GL::PixelFormat::RGBA GL::PixelFormat(0xdead)\n");
}

void PixelFormatTest::debugPixelType() {
    std::ostringstream out;

    Debug(&out) << PixelType::UnsignedByte << PixelType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::PixelType::UnsignedByte GL::PixelType(0xdead)\n");
}

void PixelFormatTest::debugCompressedPixelFormat() {
    std::ostringstream out;

    Debug{&out} << CompressedPixelFormat::RGBS3tcDxt1 << CompressedPixelFormat(0xdead);
    CORRADE_COMPARE(out.str(), "GL::CompressedPixelFormat::RGBS3tcDxt1 GL::CompressedPixelFormat(0xdead)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::PixelFormatTest)
