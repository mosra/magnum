/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/PixelFormat.h"
#include "Magnum/GL/PixelFormat.h"

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
#include "Magnum/PixelStorage.h"
#endif

namespace Magnum { namespace GL { namespace Test {

struct PixelFormatTest: TestSuite::Tester {
    explicit PixelFormatTest();

    void mapFormatType();
    void mapFormatImplementationSpecific();
    #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
    void mapFormatDeprecated();
    #endif
    void mapFormatUnsupported();
    void mapFormatInvalid();
    void mapTypeImplementationSpecific();
    void mapTypeImplementationSpecificZero();
    #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
    void mapTypeDeprecated();
    #endif
    void mapTypeUnsupported();
    void mapTypeInvalid();

    void size();
    #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
    void sizeDataPropertiesDeprecated();
    #endif
    void sizeInvalid();

    void mapCompressedFormat();
    void mapCompressedFormatImplementationSpecific();
    #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
    void mapCompressedFormatDeprecated();
    #endif
    void mapCompressedFormatUnsupported();
    void mapCompressedFormatInvalid();

    void debugPixelFormat();
    void debugPixelType();

    void debugCompressedPixelFormat();
};

PixelFormatTest::PixelFormatTest() {
    addTests({&PixelFormatTest::mapFormatType,
              &PixelFormatTest::mapFormatImplementationSpecific,
              #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
              &PixelFormatTest::mapFormatDeprecated,
              #endif
              &PixelFormatTest::mapFormatUnsupported,
              &PixelFormatTest::mapFormatInvalid,
              &PixelFormatTest::mapTypeImplementationSpecific,
              &PixelFormatTest::mapTypeImplementationSpecificZero,
              #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
              &PixelFormatTest::mapTypeDeprecated,
              #endif
              &PixelFormatTest::mapTypeUnsupported,
              &PixelFormatTest::mapTypeInvalid,

              &PixelFormatTest::size,
              #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
              &PixelFormatTest::sizeDataPropertiesDeprecated,
              #endif
              &PixelFormatTest::sizeInvalid,

              &PixelFormatTest::mapCompressedFormat,
              &PixelFormatTest::mapCompressedFormatImplementationSpecific,
              #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
              &PixelFormatTest::mapCompressedFormatDeprecated,
              #endif
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

            /* Here to silence unhandled value warnings and to verify that all
               GL-specific formats are larger than a particular value. This
               value is used in pixelFormat() and pixelType() to detect
               deprecated GL-specific values and convert them properly. */
            #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
            #define _c(value) \
                case Magnum::PixelFormat::value: \
                    CORRADE_VERIFY(UnsignedInt(Magnum::PixelFormat::value) >= 0x1000); \
                    continue;
            CORRADE_IGNORE_DEPRECATED_PUSH
            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            _c(Red)
            #endif
            #ifndef MAGNUM_TARGET_GLES
            _c(Green)
            _c(Blue)
            #endif
            #ifdef MAGNUM_TARGET_GLES2
            _c(Luminance)
            #endif
            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            _c(RG)
            #endif
            #ifdef MAGNUM_TARGET_GLES2
            _c(LuminanceAlpha)
            #endif
            _c(RGB)
            _c(RGBA)
            #ifndef MAGNUM_TARGET_GLES
            _c(BGR)
            #endif
            #ifndef MAGNUM_TARGET_WEBGL
            _c(BGRA)
            #endif
            #ifdef MAGNUM_TARGET_GLES2
            _c(SRGB)
            _c(SRGBAlpha)
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            _c(RedInteger)
            #ifndef MAGNUM_TARGET_GLES
            _c(GreenInteger)
            _c(BlueInteger)
            #endif
            _c(RGInteger)
            _c(RGBInteger)
            _c(RGBAInteger)
            #ifndef MAGNUM_TARGET_GLES
            _c(BGRInteger)
            _c(BGRAInteger)
            #endif
            #endif
            _c(DepthComponent)
            #ifndef MAGNUM_TARGET_WEBGL
            _c(StencilIndex)
            #endif
            _c(DepthStencil)
            #undef _c
            CORRADE_IGNORE_DEPRECATED_POP
            #endif
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

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
void PixelFormatTest::mapFormatDeprecated() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::RGBA),
        PixelFormat::RGBA);
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

void PixelFormatTest::mapFormatUnsupported() {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("All pixel formats are supported on ES3+");
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

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
void PixelFormatTest::mapTypeDeprecated() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(pixelType(Magnum::PixelFormat::RGBA, GL_UNSIGNED_BYTE),
        PixelType::UnsignedByte);
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

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

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
void PixelFormatTest::sizeDataPropertiesDeprecated() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(PixelStorage::pixelSize(PixelFormat::RGBA, PixelType::UnsignedShort4444), 2);
    CORRADE_IGNORE_DEPRECATED_POP

    PixelStorage storage;
    storage.setAlignment(4)
        .setRowLength(15)
        .setSkip({3, 7, 0});

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{1}),
        (std::tuple<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>, std::size_t>{{3*4, 7*15*4, 0}, {60, 1, 1}, 4}));
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

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

            /* Here to silence unhandled value warnings and to verify that all
               GL-specific formats are larger than a particular value. This
               value is used in compressedPixelFormat() to detect deprecated
               GL-specific values and convert them properly. */
            #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
            #define _c(value) \
                case Magnum::CompressedPixelFormat::value: \
                    CORRADE_VERIFY(UnsignedInt(Magnum::CompressedPixelFormat::value) >= 0x1000); \
                    continue;
            CORRADE_IGNORE_DEPRECATED_PUSH
            #ifndef MAGNUM_TARGET_GLES
            _c(Red)
            _c(RG)
            _c(RGB)
            _c(RGBA)
            _c(RedRgtc1)
            _c(RGRgtc2)
            _c(SignedRedRgtc1)
            _c(SignedRGRgtc2)
            _c(RGBBptcUnsignedFloat)
            _c(RGBBptcSignedFloat)
            _c(RGBABptcUnorm)
            _c(SRGBAlphaBptcUnorm)
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            _c(RGB8Etc2)
            _c(SRGB8Etc2)
            _c(RGB8PunchthroughAlpha1Etc2)
            _c(SRGB8PunchthroughAlpha1Etc2)
            _c(RGBA8Etc2Eac)
            _c(SRGB8Alpha8Etc2Eac)
            _c(R11Eac)
            _c(SignedR11Eac)
            _c(RG11Eac)
            _c(SignedRG11Eac)
            #endif
            _c(RGBS3tcDxt1)
            _c(RGBAS3tcDxt1)
            _c(RGBAS3tcDxt3)
            _c(RGBAS3tcDxt5)
            #ifndef MAGNUM_TARGET_WEBGL
            _c(RGBAAstc4x4)
            _c(SRGB8Alpha8Astc4x4)
            _c(RGBAAstc5x4)
            _c(SRGB8Alpha8Astc5x4)
            _c(RGBAAstc5x5)
            _c(SRGB8Alpha8Astc5x5)
            _c(RGBAAstc6x5)
            _c(SRGB8Alpha8Astc6x5)
            _c(RGBAAstc6x6)
            _c(SRGB8Alpha8Astc6x6)
            _c(RGBAAstc8x5)
            _c(SRGB8Alpha8Astc8x5)
            _c(RGBAAstc8x6)
            _c(SRGB8Alpha8Astc8x6)
            _c(RGBAAstc8x8)
            _c(SRGB8Alpha8Astc8x8)
            _c(RGBAAstc10x5)
            _c(SRGB8Alpha8Astc10x5)
            _c(RGBAAstc10x6)
            _c(SRGB8Alpha8Astc10x6)
            _c(RGBAAstc10x8)
            _c(SRGB8Alpha8Astc10x8)
            _c(RGBAAstc10x10)
            _c(SRGB8Alpha8Astc10x10)
            _c(RGBAAstc12x10)
            _c(SRGB8Alpha8Astc12x10)
            _c(RGBAAstc12x12)
            _c(SRGB8Alpha8Astc12x12)
            #endif
            #undef _c
            CORRADE_IGNORE_DEPRECATED_POP
            #endif
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

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
void PixelFormatTest::mapCompressedFormatDeprecated() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE(compressedPixelFormat(Magnum::CompressedPixelFormat::RGBAS3tcDxt1),
        CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

void PixelFormatTest::mapCompressedFormatUnsupported() {
    #if 1
    CORRADE_SKIP("All compressed pixel formats are currently supported everywhere");
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

}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::PixelFormatTest)
