/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Configuration.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Test { namespace {

struct PixelFormatTest: TestSuite::Tester {
    explicit PixelFormatTest();

    void mapping();
    void compressedMapping();

    void size();
    void sizeInvalid();
    void sizeImplementationSpecific();

    void channelFormatCount();
    void channelFormatCountInvalid();
    void channelFormatCountDepthStencilImplementationSpecific();

    void isSrgb();
    void isSrgbInvalid();
    void isSrgbImplementationSpecific();

    void isDepthOrStencil();
    void isDepthOrStencilInvalid();
    void isDepthOrStencilImplementationSpecific();

    void compressedBlockSize();
    void compressedBlockSizeInvalid();
    void compressedBlockSizeImplementationSpecific();

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
    void debugPacked();
    void debugImplementationSpecific();
    void debugImplementationSpecificPacked();

    void compressedDebug();
    void compressedDebugPacked();
    void compressedDebugImplementationSpecific();
    void compressedDebugImplementationSpecificPacked();

    void configuration();
    void compresedConfiguration();
};

PixelFormatTest::PixelFormatTest() {
    addTests({&PixelFormatTest::mapping,
              &PixelFormatTest::compressedMapping,

              &PixelFormatTest::size,
              &PixelFormatTest::sizeInvalid,
              &PixelFormatTest::sizeImplementationSpecific,

              &PixelFormatTest::channelFormatCount,
              &PixelFormatTest::channelFormatCountInvalid,
              &PixelFormatTest::channelFormatCountDepthStencilImplementationSpecific,

              &PixelFormatTest::isSrgb,
              &PixelFormatTest::isSrgbInvalid,
              &PixelFormatTest::isSrgbImplementationSpecific,

              &PixelFormatTest::isDepthOrStencil,
              &PixelFormatTest::isDepthOrStencilInvalid,
              &PixelFormatTest::isDepthOrStencilImplementationSpecific,

              &PixelFormatTest::compressedBlockSize,
              &PixelFormatTest::compressedBlockSizeInvalid,
              &PixelFormatTest::compressedBlockSizeImplementationSpecific,

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
              &PixelFormatTest::debugPacked,
              &PixelFormatTest::debugImplementationSpecific,
              &PixelFormatTest::debugImplementationSpecificPacked,

              &PixelFormatTest::compressedDebug,
              &PixelFormatTest::compressedDebugPacked,
              &PixelFormatTest::compressedDebugImplementationSpecific,
              &PixelFormatTest::compressedDebugImplementationSpecificPacked,

              &PixelFormatTest::configuration,
              &PixelFormatTest::compresedConfiguration});
}

void PixelFormatTest::mapping() {
    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid format */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto format = PixelFormat(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range)
           - that channel count times size of a channel equals to size of the
             format, unless it's a depth/stencil type */
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(format) {
            #define _c(format) \
                case PixelFormat::format: \
                    CORRADE_COMPARE(Utility::ConfigurationValue<PixelFormat>::toString(PixelFormat::format, {}), #format); \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    if(!isPixelFormatDepthOrStencil(PixelFormat::format)) \
                        CORRADE_COMPARE(pixelFormatChannelCount(PixelFormat::format)*pixelFormatSize(pixelFormatChannelFormat(PixelFormat::format)), pixelFormatSize(PixelFormat::format)); \
                    ++nextHandled; \
                    continue;
            #include "Magnum/Implementation/pixelFormatMapping.hpp"
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

void PixelFormatTest::compressedMapping() {
    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid format */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto format = CompressedPixelFormat(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range)
           - that the block size table entry matches
           - that the block data size is whole bytes and at most 16 bytes
           - that the block size is at most 16x16x16 */
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(format) {
            #define _c(format, width, height, depth, size) \
                case CompressedPixelFormat::format: \
                    CORRADE_COMPARE(Utility::ConfigurationValue<CompressedPixelFormat>::toString(CompressedPixelFormat::format, {}), #format); \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_COMPARE(compressedPixelFormatBlockSize(CompressedPixelFormat::format), (Vector3i{width, height, depth})); \
                    CORRADE_COMPARE(compressedPixelFormatBlockDataSize(CompressedPixelFormat::format), size/8); \
                    CORRADE_COMPARE(size % 8, 0); \
                    CORRADE_COMPARE_AS(width, 16, TestSuite::Compare::LessOrEqual); \
                    CORRADE_COMPARE_AS(height, 16, TestSuite::Compare::LessOrEqual); \
                    CORRADE_COMPARE_AS(depth, 16, TestSuite::Compare::LessOrEqual); \
                    CORRADE_COMPARE_AS(size/8, 16, TestSuite::Compare::LessOrEqual); \
                    ++nextHandled; \
                    continue;
            #include "Magnum/Implementation/compressedPixelFormatMapping.hpp"
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

void PixelFormatTest::size() {
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::R8I), 1);
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::R16UI), 2);
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::RGB8Unorm), 3);
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::RGBA8Snorm), 4);
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::RGB16I), 6);
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::RGBA16F), 8);
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::RGB32UI), 12);
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::RGBA32F), 16);
}

void PixelFormatTest::sizeInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};

    pixelFormatSize(PixelFormat{});
    pixelFormatSize(PixelFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "pixelFormatSize(): invalid format PixelFormat(0x0)\n"
        "pixelFormatSize(): invalid format PixelFormat(0xdead)\n");
}

void PixelFormatTest::sizeImplementationSpecific() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};

    pixelFormatSize(pixelFormatWrap(0xdead));

    CORRADE_COMPARE(out.str(), "pixelFormatSize(): can't determine size of an implementation-specific format 0xdead\n");
}

void PixelFormatTest::channelFormatCount() {
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::R8Unorm), PixelFormat::R8Unorm);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RG8Snorm), PixelFormat::R8Snorm);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RGB8Srgb), PixelFormat::R8Srgb);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RGBA8UI), PixelFormat::R8UI);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RG8I), PixelFormat::R8I);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RG16Unorm), PixelFormat::R16Unorm);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RGBA16Snorm), PixelFormat::R16Snorm);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RG16UI), PixelFormat::R16UI);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RGBA16I), PixelFormat::R16I);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RGB32UI), PixelFormat::R32UI);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RG32I), PixelFormat::R32I);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RGB16F), PixelFormat::R16F);
    CORRADE_COMPARE(pixelFormatChannelFormat(PixelFormat::RGB32F), PixelFormat::R32F);

    CORRADE_COMPARE(pixelFormatChannelCount(PixelFormat::R16UI), 1);
    CORRADE_COMPARE(pixelFormatChannelCount(PixelFormat::RG8Unorm), 2);
    CORRADE_COMPARE(pixelFormatChannelCount(PixelFormat::RGB16I), 3);
    CORRADE_COMPARE(pixelFormatChannelCount(PixelFormat::RGBA16F), 4);
}

void PixelFormatTest::channelFormatCountInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    pixelFormatChannelFormat(PixelFormat{});
    pixelFormatChannelFormat(PixelFormat(0xdead));
    pixelFormatChannelCount(PixelFormat{});
    pixelFormatChannelCount(PixelFormat(0xdead));
    CORRADE_COMPARE(out.str(),
        "pixelFormatChannelFormat(): invalid format PixelFormat(0x0)\n"
        "pixelFormatChannelFormat(): invalid format PixelFormat(0xdead)\n"
        "pixelFormatChannelCount(): invalid format PixelFormat(0x0)\n"
        "pixelFormatChannelCount(): invalid format PixelFormat(0xdead)\n");
}

void PixelFormatTest::channelFormatCountDepthStencilImplementationSpecific() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    pixelFormatChannelFormat(pixelFormatWrap(0xdead));
    pixelFormatChannelFormat(PixelFormat::Depth16Unorm);
    pixelFormatChannelCount(pixelFormatWrap(0xdead));
    pixelFormatChannelCount(PixelFormat::Depth16Unorm);
    CORRADE_COMPARE(out.str(),
        "pixelFormatChannelFormat(): can't determine channel format of an implementation-specific format 0xdead\n"
        "pixelFormatChannelFormat(): can't determine channel format of PixelFormat::Depth16Unorm\n"
        "pixelFormatChannelCount(): can't determine channel count of an implementation-specific format 0xdead\n"
        "pixelFormatChannelCount(): can't determine channel count of PixelFormat::Depth16Unorm\n");
}

void PixelFormatTest::isSrgb() {
    CORRADE_VERIFY(isPixelFormatSrgb(PixelFormat::RG8Srgb));
    CORRADE_VERIFY(!isPixelFormatSrgb(PixelFormat::RGB16F));
    CORRADE_VERIFY(!isPixelFormatSrgb(PixelFormat::Stencil8UI));
}

void PixelFormatTest::isSrgbInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    isPixelFormatSrgb(PixelFormat{});
    isPixelFormatSrgb(PixelFormat(0xdead));
    CORRADE_COMPARE(out.str(),
        "isPixelFormatSrgb(): invalid format PixelFormat(0x0)\n"
        "isPixelFormatSrgb(): invalid format PixelFormat(0xdead)\n");
}

void PixelFormatTest::isSrgbImplementationSpecific() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    isPixelFormatSrgb(pixelFormatWrap(0xdead));
    CORRADE_COMPARE(out.str(),
        "isPixelFormatSrgb(): can't determine colorspace of an implementation-specific format 0xdead\n");
}

void PixelFormatTest::isDepthOrStencil() {
    CORRADE_VERIFY(!isPixelFormatDepthOrStencil(PixelFormat::RG8Srgb));
    CORRADE_VERIFY(!isPixelFormatDepthOrStencil(PixelFormat::RGB16F));
    CORRADE_VERIFY(isPixelFormatDepthOrStencil(PixelFormat::Stencil8UI));
}

void PixelFormatTest::isDepthOrStencilInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    isPixelFormatDepthOrStencil(PixelFormat{});
    isPixelFormatDepthOrStencil(PixelFormat(0xdead));
    CORRADE_COMPARE(out.str(),
        "isPixelFormatDepthOrStencil(): invalid format PixelFormat(0x0)\n"
        "isPixelFormatDepthOrStencil(): invalid format PixelFormat(0xdead)\n");
}

void PixelFormatTest::isDepthOrStencilImplementationSpecific() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    isPixelFormatDepthOrStencil(pixelFormatWrap(0xdead));
    CORRADE_COMPARE(out.str(),
        "isPixelFormatDepthOrStencil(): can't determine type of an implementation-specific format 0xdead\n");
}

void PixelFormatTest::compressedBlockSize() {
    CORRADE_COMPARE(compressedPixelFormatBlockSize(CompressedPixelFormat::Etc2RGB8A1Srgb), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE(compressedPixelFormatBlockDataSize(CompressedPixelFormat::Etc2RGB8A1Srgb), 8);
    CORRADE_COMPARE(compressedPixelFormatBlockSize(CompressedPixelFormat::Astc5x4RGBAUnorm), (Vector3i{5, 4, 1}));
    CORRADE_COMPARE(compressedPixelFormatBlockDataSize(CompressedPixelFormat::Astc5x4RGBAUnorm), 16);
    CORRADE_COMPARE(compressedPixelFormatBlockSize(CompressedPixelFormat::Astc12x10RGBAUnorm), (Vector3i{12, 10, 1}));
    CORRADE_COMPARE(compressedPixelFormatBlockDataSize(CompressedPixelFormat::Astc12x10RGBAUnorm), 16);
    CORRADE_COMPARE(compressedPixelFormatBlockSize(CompressedPixelFormat::PvrtcRGBA2bppUnorm), (Vector3i{8, 4, 1}));
    CORRADE_COMPARE(compressedPixelFormatBlockDataSize(CompressedPixelFormat::PvrtcRGBA2bppUnorm), 8);

    /* The rest tested in compressedMapping() */
}

void PixelFormatTest::compressedBlockSizeInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};

    compressedPixelFormatBlockSize(CompressedPixelFormat{});
    compressedPixelFormatBlockSize(CompressedPixelFormat(0xdead));
    compressedPixelFormatBlockDataSize(CompressedPixelFormat{});
    compressedPixelFormatBlockDataSize(CompressedPixelFormat(0xdead));

    CORRADE_COMPARE(out.str(),
        "compressedPixelFormatBlockSize(): invalid format CompressedPixelFormat(0x0)\n"
        "compressedPixelFormatBlockSize(): invalid format CompressedPixelFormat(0xdead)\n"
        "compressedPixelFormatBlockDataSize(): invalid format CompressedPixelFormat(0x0)\n"
        "compressedPixelFormatBlockDataSize(): invalid format CompressedPixelFormat(0xdead)\n");
}

void PixelFormatTest::compressedBlockSizeImplementationSpecific() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};

    compressedPixelFormatBlockSize(compressedPixelFormatWrap(0xdead));
    compressedPixelFormatBlockDataSize(compressedPixelFormatWrap(0xdead));

    CORRADE_COMPARE(out.str(),
        "compressedPixelFormatBlockSize(): can't determine size of an implementation-specific format 0xdead\n"
        "compressedPixelFormatBlockDataSize(): can't determine size of an implementation-specific format 0xdead\n");
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
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};

    pixelFormatWrap(0xdeadbeef);

    CORRADE_COMPARE(out.str(), "pixelFormatWrap(): implementation-specific value 0xdeadbeef already wrapped or too large\n");
}

void PixelFormatTest::unwrap() {
    constexpr UnsignedInt a = pixelFormatUnwrap(PixelFormat(0x8000dead));
    CORRADE_COMPARE(a, 0xdead);
}

void PixelFormatTest::unwrapInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};

    pixelFormatUnwrap(PixelFormat::R8Snorm);

    CORRADE_COMPARE(out.str(), "pixelFormatUnwrap(): PixelFormat::R8Snorm isn't a wrapped implementation-specific value\n");
}

void PixelFormatTest::compressedIsImplementationSpecific() {
    CORRADE_VERIFY(!isPixelFormatImplementationSpecific(PixelFormat::RGBA8Unorm));
    CORRADE_VERIFY(isPixelFormatImplementationSpecific(pixelFormatWrap(0xdead)));
}

void PixelFormatTest::compressedWrap() {
    CORRADE_COMPARE(UnsignedInt(pixelFormatWrap(0xdead)), 0x8000dead);
}

void PixelFormatTest::compressedWrapInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};

    compressedPixelFormatWrap(0xdeadbeef);

    CORRADE_COMPARE(out.str(), "compressedPixelFormatWrap(): implementation-specific value 0xdeadbeef already wrapped or too large\n");
}

void PixelFormatTest::compressedUnwrap() {
    CORRADE_COMPARE(UnsignedInt(compressedPixelFormatUnwrap(CompressedPixelFormat(0x8000dead))), 0xdead);
}

void PixelFormatTest::compressedUnwrapInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};

    compressedPixelFormatUnwrap(CompressedPixelFormat::EacR11Snorm);

    CORRADE_COMPARE(out.str(), "compressedPixelFormatUnwrap(): CompressedPixelFormat::EacR11Snorm isn't a wrapped implementation-specific value\n");
}

void PixelFormatTest::debug() {
    std::ostringstream out;
    Debug{&out} << PixelFormat::RG16Snorm << PixelFormat(0xdead);

    CORRADE_COMPARE(out.str(), "PixelFormat::RG16Snorm PixelFormat(0xdead)\n");
}

void PixelFormatTest::debugPacked() {
    std::ostringstream out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << PixelFormat::RG16Snorm << Debug::packed << PixelFormat(0xdead) << PixelFormat::RGBA8Unorm;

    CORRADE_COMPARE(out.str(), "RG16Snorm 0xdead PixelFormat::RGBA8Unorm\n");
}

void PixelFormatTest::debugImplementationSpecific() {
    std::ostringstream out;
    Debug{&out} << pixelFormatWrap(0xdead);

    CORRADE_COMPARE(out.str(), "PixelFormat::ImplementationSpecific(0xdead)\n");
}

void PixelFormatTest::debugImplementationSpecificPacked() {
    std::ostringstream out;
    /* Second is not packed, the first should not make any flags persistent */
    Debug{&out} << Debug::packed << pixelFormatWrap(0xdead) << PixelFormat::RGBA8Unorm;

    CORRADE_COMPARE(out.str(), "ImplementationSpecific(0xdead) PixelFormat::RGBA8Unorm\n");
}

void PixelFormatTest::compressedDebug() {
    std::ostringstream out;
    Debug{&out} << CompressedPixelFormat::Bc3RGBAUnorm << CompressedPixelFormat(0xdead);

    CORRADE_COMPARE(out.str(), "CompressedPixelFormat::Bc3RGBAUnorm CompressedPixelFormat(0xdead)\n");
}

void PixelFormatTest::compressedDebugPacked() {
    std::ostringstream out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << CompressedPixelFormat::Bc3RGBAUnorm << Debug::packed << CompressedPixelFormat(0xdead) << CompressedPixelFormat::Astc10x10RGBAF;

    CORRADE_COMPARE(out.str(), "Bc3RGBAUnorm 0xdead CompressedPixelFormat::Astc10x10RGBAF\n");
}

void PixelFormatTest::compressedDebugImplementationSpecific() {
    std::ostringstream out;
    Debug{&out} << compressedPixelFormatWrap(0xdead);

    CORRADE_COMPARE(out.str(), "CompressedPixelFormat::ImplementationSpecific(0xdead)\n");
}

void PixelFormatTest::compressedDebugImplementationSpecificPacked() {
    std::ostringstream out;
    /* Second is not packed, the first should not make any flags persistent */
    Debug{&out} << compressedPixelFormatWrap(0xdead) << CompressedPixelFormat::Astc10x10RGBAF;

    CORRADE_COMPARE(out.str(), "CompressedPixelFormat::ImplementationSpecific(0xdead) CompressedPixelFormat::Astc10x10RGBAF\n");
}

void PixelFormatTest::configuration() {
    Utility::Configuration c;

    c.setValue("format", PixelFormat::RGB8Srgb);
    CORRADE_COMPARE(c.value("format"), "RGB8Srgb");
    CORRADE_COMPARE(c.value<PixelFormat>("format"), PixelFormat::RGB8Srgb);

    c.setValue("zero", PixelFormat(0));
    CORRADE_COMPARE(c.value("zero"), "");
    CORRADE_COMPARE(c.value<PixelFormat>("zero"), PixelFormat{});

    c.setValue("invalid", PixelFormat(0xdead));
    CORRADE_COMPARE(c.value("invalid"), "");
    CORRADE_COMPARE(c.value<PixelFormat>("invalid"), PixelFormat{});
}

void PixelFormatTest::compresedConfiguration() {
    Utility::Configuration c;

    c.setValue("format", CompressedPixelFormat::Astc3x3x3RGBASrgb);
    CORRADE_COMPARE(c.value("format"), "Astc3x3x3RGBASrgb");
    CORRADE_COMPARE(c.value<CompressedPixelFormat>("format"), CompressedPixelFormat::Astc3x3x3RGBASrgb);

    c.setValue("zero", CompressedPixelFormat(0));
    CORRADE_COMPARE(c.value("zero"), "");
    CORRADE_COMPARE(c.value<CompressedPixelFormat>("zero"), CompressedPixelFormat{});

    c.setValue("invalid", CompressedPixelFormat(0xdead));
    CORRADE_COMPARE(c.value("invalid"), "");
    CORRADE_COMPARE(c.value<CompressedPixelFormat>("invalid"), CompressedPixelFormat{});
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::PixelFormatTest)
