/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Vk/PixelFormat.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct PixelFormatTest: TestSuite::Tester {
    explicit PixelFormatTest();

    void map();
    void mapImplementationSpecific();
    void mapUnsupported();
    void mapInvalid();
    void mapGenericUnsupported();

    void mapCompressed();
    void mapCompressedImplementationSpecific();
    void mapCompressedUnsupported();
    void mapCompressedInvalid();
    void mapGenericCompressedUnsupported();

    void debug();
};

PixelFormatTest::PixelFormatTest() {
    addTests({&PixelFormatTest::map,
              &PixelFormatTest::mapImplementationSpecific,
              &PixelFormatTest::mapUnsupported,
              &PixelFormatTest::mapInvalid,
              &PixelFormatTest::mapGenericUnsupported,

              &PixelFormatTest::mapCompressed,
              &PixelFormatTest::mapCompressedImplementationSpecific,
              &PixelFormatTest::mapCompressedUnsupported,
              &PixelFormatTest::mapCompressedInvalid,
              &PixelFormatTest::mapGenericCompressedUnsupported,

              &PixelFormatTest::debug});
}

void PixelFormatTest::map() {
    /* Touchstone verification. Using Vulkan enums directly to sidestep
       potential problems in enum mapping as well. */
    CORRADE_VERIFY(hasPixelFormat(Magnum::PixelFormat::RGBA8Unorm));
    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::RGBA8Unorm), PixelFormat(VK_FORMAT_R8G8B8A8_UNORM));

    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid format */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto format = Magnum::PixelFormat(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range)
           - that a particular generic format maps to a particular format
           - that the debug output matches what was converted */
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(format) {
            #define _c(format) \
                case Magnum::PixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasPixelFormat(Magnum::PixelFormat::format)); \
                    CORRADE_COMPARE(genericPixelFormat(PixelFormat::format), Magnum::PixelFormat::format); \
                    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::format), PixelFormat::format); \
                    { \
                        Containers::String out; \
                        Debug{&out} << pixelFormat(Magnum::PixelFormat::format); \
                        CORRADE_COMPARE(out, "Vk::PixelFormat::" #format "\n"); \
                    } \
                    ++nextHandled; \
                    continue;
            #define _s(format) \
                case Magnum::PixelFormat::format: { \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasPixelFormat(Magnum::PixelFormat::format)); \
                    Containers::String out; \
                    { /* Redirected otherwise graceful assert would abort */ \
                        Error redirectError{&out}; \
                        pixelFormat(Magnum::PixelFormat::format); \
                    } \
                    Debug{Debug::Flag::NoNewlineAtTheEnd} << out; \
                    ++nextHandled; \
                    continue; \
                }
            #include "Magnum/Vk/Implementation/pixelFormatMapping.hpp"
            #undef _s
            #undef _c
        }
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic pop
        #endif

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xffff);
}

void PixelFormatTest::mapImplementationSpecific() {
    CORRADE_VERIFY(hasPixelFormat(Magnum::pixelFormatWrap(VK_FORMAT_A8B8G8R8_SINT_PACK32)));
    CORRADE_COMPARE(pixelFormat(Magnum::pixelFormatWrap(VK_FORMAT_A8B8G8R8_SINT_PACK32)),
        PixelFormat(VK_FORMAT_A8B8G8R8_SINT_PACK32));
}

void PixelFormatTest::mapUnsupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #if 1
    CORRADE_SKIP("All pixel formats are supported.");
    #else
    Containers::String out;
    Error redirectError{&out};

    pixelFormat(Magnum::PixelFormat::RGB16UI);
    CORRADE_COMPARE(out, "Vk::pixelFormat(): unsupported format PixelFormat::RGB16UI\n");
    #endif
}

void PixelFormatTest::mapInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    hasPixelFormat(Magnum::PixelFormat{});
    hasPixelFormat(Magnum::PixelFormat(0x123));
    pixelFormat(Magnum::PixelFormat{});
    pixelFormat(Magnum::PixelFormat(0x123));
    CORRADE_COMPARE(out,
        "Vk::hasPixelFormat(): invalid format PixelFormat(0x0)\n"
        "Vk::hasPixelFormat(): invalid format PixelFormat(0x123)\n"
        "Vk::pixelFormat(): invalid format PixelFormat(0x0)\n"
        "Vk::pixelFormat(): invalid format PixelFormat(0x123)\n");
}

void PixelFormatTest::mapGenericUnsupported() {
    /* This one doesn't have any generic equivalent yet, and isn't in the
       enum either */
    CORRADE_COMPARE(genericPixelFormat(PixelFormat(VK_FORMAT_R5G6B5_UNORM_PACK16)), Containers::NullOpt);
    /* For compressed texture formats it returns NullOpt too, instead of
       asserting. See comment in the source for reasons. */
    CORRADE_COMPARE(genericPixelFormat(PixelFormat::CompressedAstc4x4RGBAF), Containers::NullOpt);
}

void PixelFormatTest::mapCompressed() {
    /* Touchstone verification. Using Vulkan enums directly to sidestep
       potential problems in enum mapping as well. */
    CORRADE_VERIFY(hasPixelFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm));
    CORRADE_COMPARE(pixelFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm), PixelFormat(VK_FORMAT_BC1_RGBA_UNORM_BLOCK));
    /* PVRTC RGB and RGBA formats have N:1 mapping, conversion back makes them
       always RGBA */
    CORRADE_COMPARE(genericCompressedPixelFormat(pixelFormat(Magnum::CompressedPixelFormat::PvrtcRGB4bppSrgb)), Magnum::CompressedPixelFormat::PvrtcRGBA4bppSrgb);

    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid format */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto format = Magnum::CompressedPixelFormat(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range)
           - that a particular generic format maps to a particular format
           - that the debug output matches what was converted */
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(format) {
            #define _c(format, expectedFormat) \
                case Magnum::CompressedPixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasPixelFormat(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_COMPARE(genericCompressedPixelFormat(PixelFormat::Compressed ## expectedFormat), Magnum::CompressedPixelFormat::format); \
                    CORRADE_COMPARE(pixelFormat(Magnum::CompressedPixelFormat::format), PixelFormat::Compressed ## expectedFormat); \
                    { \
                        Containers::String out; \
                        Debug{&out} << pixelFormat(Magnum::CompressedPixelFormat::format); \
                        CORRADE_COMPARE(out, "Vk::PixelFormat::Compressed" #expectedFormat "\n"); \
                    } \
                    ++nextHandled; \
                    continue;
            /* For duplicate mappings compared to _c() it only checks the
               forward mapping. The duplicate mapping is tested in the
               touchstone verification above */
            #define _d(format, expectedFormat) \
                case Magnum::CompressedPixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasPixelFormat(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_COMPARE(pixelFormat(Magnum::CompressedPixelFormat::format), PixelFormat::Compressed ## expectedFormat); \
                    { \
                        Containers::String out; \
                        Debug{&out} << pixelFormat(Magnum::CompressedPixelFormat::format); \
                        CORRADE_COMPARE(out, "Vk::PixelFormat::Compressed" #expectedFormat "\n"); \
                    } \
                    ++nextHandled; \
                    continue;
            #define _s(format) \
                case Magnum::CompressedPixelFormat::format: { \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasPixelFormat(Magnum::CompressedPixelFormat::format)); \
                    Containers::String out; \
                    { /* Redirected otherwise graceful assert would abort */ \
                        Error redirectError{&out}; \
                        pixelFormat(Magnum::CompressedPixelFormat::format); \
                    } \
                    Debug{Debug::Flag::NoNewlineAtTheEnd} << out; \
                    ++nextHandled; \
                    continue; \
                }
            #include "Magnum/Vk/Implementation/compressedPixelFormatMapping.hpp"
            #undef _s
            #undef _c
        }
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic pop
        #endif

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xffff);
}

void PixelFormatTest::mapCompressedImplementationSpecific() {
    CORRADE_VERIFY(hasPixelFormat(Magnum::compressedPixelFormatWrap(VK_FORMAT_ASTC_10x6_UNORM_BLOCK)));
    CORRADE_COMPARE(pixelFormat(Magnum::compressedPixelFormatWrap(VK_FORMAT_ASTC_10x6_UNORM_BLOCK)),
        PixelFormat(VK_FORMAT_ASTC_10x6_UNORM_BLOCK));
}

void PixelFormatTest::mapCompressedUnsupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    CORRADE_VERIFY(!hasPixelFormat(Magnum::CompressedPixelFormat::Astc3x3x3RGBAUnorm));

    Containers::String out;
    Error redirectError{&out};
    pixelFormat(Magnum::CompressedPixelFormat::Astc3x3x3RGBAUnorm);
    CORRADE_COMPARE(out, "Vk::pixelFormat(): unsupported format CompressedPixelFormat::Astc3x3x3RGBAUnorm\n");
}

void PixelFormatTest::mapCompressedInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    hasPixelFormat(Magnum::CompressedPixelFormat{});
    hasPixelFormat(Magnum::CompressedPixelFormat(0x123));
    pixelFormat(Magnum::CompressedPixelFormat{});
    pixelFormat(Magnum::CompressedPixelFormat(0x123));
    CORRADE_COMPARE(out,
        "Vk::hasPixelFormat(): invalid format CompressedPixelFormat(0x0)\n"
        "Vk::hasPixelFormat(): invalid format CompressedPixelFormat(0x123)\n"
        "Vk::pixelFormat(): invalid format CompressedPixelFormat(0x0)\n"
        "Vk::pixelFormat(): invalid format CompressedPixelFormat(0x123)\n");
}

void PixelFormatTest::mapGenericCompressedUnsupported() {
    /* PVRTC2 doesn't have any generic equivalent yet */
    CORRADE_COMPARE(genericPixelFormat(PixelFormat::CompressedPvrtc2RGBA2bppUnorm), Containers::NullOpt);
    /* For uncompressed texture formats it returns NullOpt too, instead of
       asserting. See comment in the source for reasons. */
    CORRADE_COMPARE(genericCompressedPixelFormat(PixelFormat::RGB8Unorm), Containers::NullOpt);
}

void PixelFormatTest::debug() {
    Containers::String out;
    Debug{&out} << PixelFormat::RGB16UI << PixelFormat(-10007655);
    CORRADE_COMPARE(out, "Vk::PixelFormat::RGB16UI Vk::PixelFormat(-10007655)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::PixelFormatTest)
