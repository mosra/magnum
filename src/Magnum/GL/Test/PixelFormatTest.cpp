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
#include <Corrade/TestSuite/Compare/String.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/TextureFormat.h"

namespace Magnum { namespace GL { namespace Test { namespace {

/* Tests also TextureFormat-related utilities, since the mapping tables are
   shared between these two */

struct PixelFormatTest: TestSuite::Tester {
    explicit PixelFormatTest();

    void mapFormatTypeTextureFormat();
    void mapFormatImplementationSpecific();
    void mapFormatUnsupported();
    void mapFormatInvalid();
    void mapTypeImplementationSpecific();
    void mapTypeImplementationSpecificZero();
    void mapTypeUnsupported();
    void mapTypeInvalid();
    void mapTextureFormatImplementationSpecific();
    void mapTextureFormatUnsupported();
    void mapTextureFormatInvalid();
    void mapGenericFormatUnsupported();

    void size();
    void sizeInvalid();

    void mapCompressedFormatTextureFormat();
    void mapCompressedFormatImplementationSpecific();
    void mapCompressedFormatUnsupported();
    void mapCompressedFormatInvalid();
    void mapCompressedTextureFormatImplementationSpecific();
    void mapCompressedTextureFormatUnsupported();
    void mapCompressedTextureFormatInvalid();
    void mapGenericCompressedFormatUnsupported();

    void sizeCompressed();
    #ifndef MAGNUM_TARGET_GLES
    void sizeCompressedGeneric();
    #endif
    void sizeCompressedInvalid();

    void debugPixelFormat();
    void debugPixelType();
    void debugCompressedPixelFormat();
    void debugTextureFormat();
};

PixelFormatTest::PixelFormatTest() {
    addTests({&PixelFormatTest::mapFormatTypeTextureFormat,
              &PixelFormatTest::mapFormatImplementationSpecific,
              &PixelFormatTest::mapFormatUnsupported,
              &PixelFormatTest::mapFormatInvalid,
              &PixelFormatTest::mapTypeImplementationSpecific,
              &PixelFormatTest::mapTypeImplementationSpecificZero,
              &PixelFormatTest::mapTypeUnsupported,
              &PixelFormatTest::mapTypeInvalid,
              &PixelFormatTest::mapTextureFormatImplementationSpecific,
              &PixelFormatTest::mapTextureFormatUnsupported,
              &PixelFormatTest::mapTextureFormatInvalid,
              &PixelFormatTest::mapGenericFormatUnsupported,

              &PixelFormatTest::size,
              &PixelFormatTest::sizeInvalid,

              &PixelFormatTest::mapCompressedFormatTextureFormat,
              &PixelFormatTest::mapCompressedFormatImplementationSpecific,
              &PixelFormatTest::mapCompressedFormatUnsupported,
              &PixelFormatTest::mapCompressedFormatInvalid,
              &PixelFormatTest::mapCompressedTextureFormatImplementationSpecific,
              &PixelFormatTest::mapCompressedTextureFormatUnsupported,
              &PixelFormatTest::mapCompressedTextureFormatInvalid,
              &PixelFormatTest::mapGenericCompressedFormatUnsupported,

              &PixelFormatTest::sizeCompressed,
              #ifndef MAGNUM_TARGET_GLES
              &PixelFormatTest::sizeCompressedGeneric,
              #endif
              &PixelFormatTest::sizeCompressedInvalid,

              &PixelFormatTest::debugPixelFormat,
              &PixelFormatTest::debugPixelType,
              &PixelFormatTest::debugCompressedPixelFormat,
              &PixelFormatTest::debugTextureFormat});
}

void PixelFormatTest::mapFormatTypeTextureFormat() {
    /* Touchstone verification */
    CORRADE_VERIFY(hasPixelFormat(Magnum::PixelFormat::RGBA8Unorm));
    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::RGBA8Unorm), PixelFormat::RGBA);
    CORRADE_COMPARE(pixelType(Magnum::PixelFormat::RGBA8Unorm), PixelType::UnsignedByte);
    CORRADE_COMPARE(genericPixelFormat(PixelFormat::RGB, PixelType::UnsignedByte), Magnum::PixelFormat::RGB8Unorm);
    CORRADE_VERIFY(hasTextureFormat(Magnum::PixelFormat::RGBA8Unorm));
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(textureFormat(Magnum::PixelFormat::RGBA8Unorm), TextureFormat::RGBA8);
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::RGB8), Magnum::PixelFormat::RGB8Unorm);
    #else
    CORRADE_COMPARE(textureFormat(Magnum::PixelFormat::RGBA8Unorm), TextureFormat::RGBA);
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::RGB), Magnum::PixelFormat::RGB8Unorm);
    #endif

    /* No mapping for these */
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::RGB565), Containers::NullOpt);
    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_VERIFY(!hasTextureFormat(Magnum::PixelFormat::Depth32F));
    #endif

    /* sRGB formats have N:1 mapping, conversion back is losing the sRGB bit */
    CORRADE_COMPARE(genericPixelFormat(pixelFormat(Magnum::PixelFormat::R8Srgb), pixelType(Magnum::PixelFormat::R8Srgb)), Magnum::PixelFormat::R8Unorm);
    CORRADE_COMPARE(genericPixelFormat(pixelFormat(Magnum::PixelFormat::RGBA8Srgb), pixelType(Magnum::PixelFormat::RGBA8Srgb)), Magnum::PixelFormat::RGBA8Unorm);

    /* On ES2, forward PixelFormat mapping goes to luminance, but backwards
       mapping from R and RG works too. For TextureFormat, forward mapping goes
       to unsized formats and luminance (which aren't usable in glTexStorage()
       then, only glTexImage()), but backwards mapping from sized formats works
       too. */
    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::R8Unorm), PixelFormat::Luminance);
    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::RG8Unorm), PixelFormat::LuminanceAlpha);
    CORRADE_COMPARE(pixelType(Magnum::PixelFormat::R8Unorm), PixelType::UnsignedByte);
    CORRADE_COMPARE(pixelType(Magnum::PixelFormat::RG8Unorm), PixelType::UnsignedByte);
    CORRADE_COMPARE(genericPixelFormat(PixelFormat::Luminance, PixelType::UnsignedByte), Magnum::PixelFormat::R8Unorm);
    CORRADE_COMPARE(genericPixelFormat(PixelFormat::LuminanceAlpha, PixelType::UnsignedByte), Magnum::PixelFormat::RG8Unorm);
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE(genericPixelFormat(PixelFormat::Red, PixelType::UnsignedByte), Magnum::PixelFormat::R8Unorm);
    CORRADE_COMPARE(genericPixelFormat(PixelFormat::RG, PixelType::UnsignedByte), Magnum::PixelFormat::RG8Unorm);
    #endif

    CORRADE_COMPARE(textureFormat(Magnum::PixelFormat::R8Unorm), TextureFormat::Luminance);
    CORRADE_COMPARE(textureFormat(Magnum::PixelFormat::RG8Unorm), TextureFormat::LuminanceAlpha);
    CORRADE_COMPARE(textureFormat(Magnum::PixelFormat::RGB8Unorm), TextureFormat::RGB);
    CORRADE_COMPARE(textureFormat(Magnum::PixelFormat::RGBA8Unorm), TextureFormat::RGBA);
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::Luminance), Magnum::PixelFormat::R8Unorm);
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::LuminanceAlpha), Magnum::PixelFormat::RG8Unorm);
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::RGB), Magnum::PixelFormat::RGB8Unorm);
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::RGBA), Magnum::PixelFormat::RGBA8Unorm);
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::R8), Magnum::PixelFormat::R8Unorm);
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::RG8), Magnum::PixelFormat::RG8Unorm);
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::RGB8), Magnum::PixelFormat::RGB8Unorm);
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::RGBA8), Magnum::PixelFormat::RGBA8Unorm);
    #endif
    #endif

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
           - that a particular pixel format maps to a particular GL format and
             back
           - that a particular pixel format maps to a particular GL type and
             back
           - that a particular pixel format maps to a particular GL texture
             format and back
           - that the pixel format size matches size of the generic format */
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(format) {
            #define _c(format, expectedFormat, expectedType, expectedTextureFormat) \
                case Magnum::PixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasPixelFormat(Magnum::PixelFormat::format)); \
                    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::format), Magnum::GL::PixelFormat::expectedFormat); \
                    CORRADE_COMPARE(pixelType(Magnum::PixelFormat::format), Magnum::GL::PixelType::expectedType); \
                    CORRADE_COMPARE(genericPixelFormat(Magnum::GL::PixelFormat::expectedFormat, Magnum::GL::PixelType::expectedType), Magnum::PixelFormat::format); \
                    CORRADE_VERIFY(hasTextureFormat(Magnum::PixelFormat::format)); \
                    CORRADE_COMPARE(textureFormat(Magnum::PixelFormat::format), Magnum::GL::TextureFormat::expectedTextureFormat); \
                    CORRADE_COMPARE(genericPixelFormat(Magnum::GL::TextureFormat::expectedTextureFormat), Magnum::PixelFormat::format); \
                    CORRADE_COMPARE(pixelFormatSize(Magnum::GL::PixelFormat::expectedFormat, Magnum::GL::PixelType::expectedType), pixelFormatSize(Magnum::PixelFormat::format)); \
                    ++nextHandled; \
                    continue;
            /* For duplicate format/type mappings compared to _c() it only
               checks the forward mapping and genericPixelFormat() from a
               TextureFormat. The duplicate mapping is tested in the touchstone
               verification above. */
            #define _d(format, expectedFormat, expectedType, expectedTextureFormat) \
                case Magnum::PixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasPixelFormat(Magnum::PixelFormat::format)); \
                    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::format), Magnum::GL::PixelFormat::expectedFormat); \
                    CORRADE_COMPARE(pixelType(Magnum::PixelFormat::format), Magnum::GL::PixelType::expectedType); \
                    CORRADE_VERIFY(hasTextureFormat(Magnum::PixelFormat::format)); \
                    CORRADE_COMPARE(textureFormat(Magnum::PixelFormat::format), Magnum::GL::TextureFormat::expectedTextureFormat); \
                    CORRADE_COMPARE(genericPixelFormat(Magnum::GL::TextureFormat::expectedTextureFormat), Magnum::PixelFormat::format); \
                    CORRADE_COMPARE(pixelFormatSize(Magnum::GL::PixelFormat::expectedFormat, Magnum::GL::PixelType::expectedType), pixelFormatSize(Magnum::PixelFormat::format)); \
                    ++nextHandled; \
                    continue;
            #define _n(format, expectedFormat, expectedType) \
                case Magnum::PixelFormat::format: { \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasPixelFormat(Magnum::PixelFormat::format)); \
                    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::format), Magnum::GL::PixelFormat::expectedFormat); \
                    CORRADE_COMPARE(pixelType(Magnum::PixelFormat::format), Magnum::GL::PixelType::expectedType); \
                    CORRADE_COMPARE(genericPixelFormat(Magnum::GL::PixelFormat::expectedFormat, Magnum::GL::PixelType::expectedType), Magnum::PixelFormat::format); \
                    CORRADE_COMPARE(pixelFormatSize(Magnum::GL::PixelFormat::expectedFormat, Magnum::GL::PixelType::expectedType), pixelFormatSize(Magnum::PixelFormat::format)); \
                    CORRADE_VERIFY(!hasTextureFormat(Magnum::PixelFormat::format)); \
                    Containers::String out; \
                    { /* Redirected otherwise graceful assert would abort */ \
                        Error redirectError{&out}; \
                        textureFormat(Magnum::PixelFormat::format); \
                    } \
                    Debug{Debug::Flag::NoNewlineAtTheEnd} << out; \
                    ++nextHandled; \
                    continue; \
                }
            /* For duplicate format/type mappings compared to _n() it only
               checks the forward mapping and genericPixelFormat() from a
               TextureFormat. The duplicate mapping is tested in the touchstone
               verification above. */
            #define _dn(format, expectedFormat, expectedType) \
                case Magnum::PixelFormat::format: { \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasPixelFormat(Magnum::PixelFormat::format)); \
                    CORRADE_COMPARE(pixelFormat(Magnum::PixelFormat::format), Magnum::GL::PixelFormat::expectedFormat); \
                    CORRADE_COMPARE(pixelType(Magnum::PixelFormat::format), Magnum::GL::PixelType::expectedType); \
                    CORRADE_COMPARE(pixelFormatSize(Magnum::GL::PixelFormat::expectedFormat, Magnum::GL::PixelType::expectedType), pixelFormatSize(Magnum::PixelFormat::format)); \
                    CORRADE_VERIFY(!hasTextureFormat(Magnum::PixelFormat::format)); \
                    Containers::String out; \
                    { /* Redirected otherwise graceful assert would abort */ \
                        Error redirectError{&out}; \
                        textureFormat(Magnum::PixelFormat::format); \
                    } \
                    Debug{Debug::Flag::NoNewlineAtTheEnd} << out; \
                    ++nextHandled; \
                    continue; \
                }
            #define _s(format) \
                case Magnum::PixelFormat::format: { \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasPixelFormat(Magnum::PixelFormat::format)); \
                    CORRADE_VERIFY(!hasTextureFormat(Magnum::PixelFormat::format)); \
                    Containers::String out; \
                    { /* Redirected otherwise graceful assert would abort */ \
                        Error redirectError{&out}; \
                        pixelFormat(Magnum::PixelFormat::format); \
                        pixelType(Magnum::PixelFormat::format); \
                        textureFormat(Magnum::PixelFormat::format); \
                    } \
                    Debug{Debug::Flag::NoNewlineAtTheEnd} << out; \
                    ++nextHandled; \
                    continue; \
                }
            #include "Magnum/GL/Implementation/pixelFormatMapping.hpp"
            #undef _s
            #undef _n
            #undef _d
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

void PixelFormatTest::mapFormatImplementationSpecific() {
    CORRADE_VERIFY(hasPixelFormat(Magnum::pixelFormatWrap(PixelFormat::RGBA)));
    CORRADE_COMPARE(pixelFormat(Magnum::pixelFormatWrap(PixelFormat::RGBA)),
        PixelFormat::RGBA);
}

void PixelFormatTest::mapFormatUnsupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("All pixel formats are supported on ES3+.");
    #else
    Containers::String out;
    Error redirectError{&out};

    CORRADE_VERIFY(!hasPixelFormat(Magnum::PixelFormat::RGB16UI));
    pixelFormat(Magnum::PixelFormat::RGB16UI);
    CORRADE_COMPARE(out, "GL::pixelFormat(): format PixelFormat::RGB16UI is not supported on this target\n");
    #endif
}

void PixelFormatTest::mapFormatInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    hasPixelFormat(Magnum::PixelFormat{});
    hasPixelFormat(Magnum::PixelFormat(0x123));
    pixelFormat(Magnum::PixelFormat{});
    pixelFormat(Magnum::PixelFormat(0x123));
    CORRADE_COMPARE(out,
        "GL::hasPixelFormat(): invalid format PixelFormat(0x0)\n"
        "GL::hasPixelFormat(): invalid format PixelFormat(0x123)\n"
        "GL::pixelFormat(): invalid format PixelFormat(0x0)\n"
        "GL::pixelFormat(): invalid format PixelFormat(0x123)\n");
}

void PixelFormatTest::mapTypeImplementationSpecific() {
    CORRADE_COMPARE(pixelType(Magnum::pixelFormatWrap(PixelFormat::RGBA), GL_UNSIGNED_BYTE),
        PixelType::UnsignedByte);
}

void PixelFormatTest::mapTypeImplementationSpecificZero() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    pixelType(Magnum::pixelFormatWrap(PixelFormat::RGBA));
    CORRADE_COMPARE(out, "GL::pixelType(): format is implementation-specific, but no additional type specifier was passed\n");
}

void PixelFormatTest::mapTypeUnsupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("All pixel formats are supported on ES3+.");
    #else
    CORRADE_VERIFY(!hasPixelFormat(Magnum::PixelFormat::RGBA16UI));

    Containers::String out;
    Error redirectError{&out};
    pixelType(Magnum::PixelFormat::RGB16UI);
    CORRADE_COMPARE(out, "GL::pixelType(): format PixelFormat::RGB16UI is not supported on this target\n");
    #endif
}

void PixelFormatTest::mapTypeInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    pixelType(Magnum::PixelFormat{});
    pixelType(Magnum::PixelFormat(0x123));
    CORRADE_COMPARE(out,
        "GL::pixelType(): invalid format PixelFormat(0x0)\n"
        "GL::pixelType(): invalid format PixelFormat(0x123)\n");
}

void PixelFormatTest::mapTextureFormatImplementationSpecific() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    hasTextureFormat(Magnum::pixelFormatWrap(PixelFormat::RGBA));
    textureFormat(Magnum::pixelFormatWrap(PixelFormat::RGBA));
    CORRADE_COMPARE(out,
        "GL::hasTextureFormat(): cannot map an implementation-specific pixel format to an OpenGL texture format\n"
        "GL::textureFormat(): cannot map an implementation-specific pixel format to an OpenGL texture format\n");
}

void PixelFormatTest::mapTextureFormatUnsupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
    CORRADE_SKIP("All pixel formats are supported on ES3+.");
    #elif defined(MAGNUM_TARGET_GLES2)
    Containers::String out;
    Error redirectError{&out};

    textureFormat(Magnum::PixelFormat::Depth32F);
    CORRADE_COMPARE(out, "GL::textureFormat(): format PixelFormat::Depth32F is not supported on this target\n");
    #else
    Containers::String out;
    Error redirectError{&out};

    textureFormat(Magnum::PixelFormat::Depth16UnormStencil8UI);
    CORRADE_COMPARE(out, "GL::textureFormat(): format PixelFormat::Depth16UnormStencil8UI is not supported on this target\n");
    #endif
}

void PixelFormatTest::mapTextureFormatInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    hasTextureFormat(Magnum::PixelFormat{});
    hasTextureFormat(Magnum::PixelFormat(0x123));
    textureFormat(Magnum::PixelFormat{});
    textureFormat(Magnum::PixelFormat(0x123));
    CORRADE_COMPARE(out,
        "GL::hasTextureFormat(): invalid format PixelFormat(0x0)\n"
        "GL::hasTextureFormat(): invalid format PixelFormat(0x123)\n"
        "GL::textureFormat(): invalid format PixelFormat(0x0)\n"
        "GL::textureFormat(): invalid format PixelFormat(0x123)\n");
}

void PixelFormatTest::mapGenericFormatUnsupported() {
    /* These don't have any generic equivalent yet */
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE(genericPixelFormat(PixelFormat::BGRA, PixelType::UnsignedByte), Containers::NullOpt);
    #endif
    CORRADE_COMPARE(genericPixelFormat(PixelFormat::RGBA, PixelType::UnsignedShort565), Containers::NullOpt);
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::RGB565), Containers::NullOpt);
    /* For compressed texture formats it returns NullOpt too, instead of
       asserting. See comment in the source for reasons. */
    CORRADE_COMPARE(genericPixelFormat(TextureFormat::CompressedR11Eac), Containers::NullOpt);
}

void PixelFormatTest::size() {
    /* Just basic sanity verification. Formats that have a matching generic
       format are checked against the generic pixelFormatSize() in
       mapFormatTypeTextureFormat() above. */

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::RGB, PixelType::UnsignedByte332), 1);
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::StencilIndex, PixelType::UnsignedByte), 1);
    #endif
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::DepthComponent, PixelType::UnsignedShort), 2);
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::RGBA, PixelType::UnsignedShort4444), 2);
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::DepthStencil, PixelType::UnsignedInt248), 4);
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::RGBA, PixelType::UnsignedInt), 4*4);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(pixelFormatSize(PixelFormat::DepthStencil, PixelType::Float32UnsignedInt248Rev), 8);
    #endif
}

void PixelFormatTest::sizeInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    pixelFormatSize(PixelFormat(0xdeadbeef), PixelType(0xbadcafe));
    pixelFormatSize(PixelFormat::DepthStencil, PixelType::Float);
    CORRADE_COMPARE_AS(out,
        "GL::pixelFormatSize(): unknown GL::PixelFormat(0xdeadbeef) or GL::PixelType(0xbadcafe)\n"
        "GL::pixelFormatSize(): invalid GL::PixelType::Float specified for GL::PixelFormat::DepthStencil\n",
        TestSuite::Compare::String);
}

void PixelFormatTest::mapCompressedFormatTextureFormat() {
    /* Touchstone verification */
    CORRADE_VERIFY(hasCompressedPixelFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm));
    CORRADE_COMPARE(compressedPixelFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_VERIFY(hasTextureFormat(Magnum::CompressedPixelFormat::Astc8x8RGBASrgb));
    CORRADE_COMPARE(textureFormat(Magnum::CompressedPixelFormat::Astc8x8RGBASrgb), TextureFormat::CompressedSRGB8Alpha8Astc8x8);
    /* ASTC Unorm formats have N:1 mapping, conversion back is losing the
       Unorm/F distinction */
    CORRADE_COMPARE(genericCompressedPixelFormat(compressedPixelFormat(Magnum::CompressedPixelFormat::Astc4x4RGBAUnorm)), Magnum::CompressedPixelFormat::Astc4x4RGBAF);

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
           - that a particular pixel format maps to a particular GL format and
             back
           - that a particular pixel format maps to a particular GL type and
             back
           - that a particular pixel format maps to a particular GL texture
             format and back
           - that the pixel format block size matches block size of the generic
             format */
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(format) {
            #define _c(format, expectedFormat) \
                case Magnum::CompressedPixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasCompressedPixelFormat(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_COMPARE(genericCompressedPixelFormat(Magnum::GL::CompressedPixelFormat::expectedFormat), Magnum::CompressedPixelFormat::format); \
                    CORRADE_VERIFY(hasTextureFormat(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_COMPARE(compressedPixelFormat(Magnum::CompressedPixelFormat::format), Magnum::GL::CompressedPixelFormat::expectedFormat); \
                    CORRADE_COMPARE(textureFormat(Magnum::CompressedPixelFormat::format), Magnum::GL::TextureFormat::Compressed ## expectedFormat); \
                    CORRADE_COMPARE(genericCompressedPixelFormat(Magnum::GL::TextureFormat::Compressed ## expectedFormat), Magnum::CompressedPixelFormat::format); \
                    CORRADE_COMPARE(compressedPixelFormatBlockSize(Magnum::GL::CompressedPixelFormat::expectedFormat), compressedPixelFormatBlockSize(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_COMPARE(compressedPixelFormatBlockDataSize(Magnum::GL::CompressedPixelFormat::expectedFormat), compressedPixelFormatBlockDataSize(Magnum::CompressedPixelFormat::format)); \
                    ++nextHandled; \
                    continue;
            /* For duplicate mappings compared to _c() it only checks the
               forward mapping. The duplicate mapping is tested in the
               touchstone verification above */
            #define _d(format, expectedFormat) \
                case Magnum::CompressedPixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasCompressedPixelFormat(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_VERIFY(hasTextureFormat(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_COMPARE(compressedPixelFormat(Magnum::CompressedPixelFormat::format), Magnum::GL::CompressedPixelFormat::expectedFormat); \
                    CORRADE_COMPARE(textureFormat(Magnum::CompressedPixelFormat::format), Magnum::GL::TextureFormat::Compressed ## expectedFormat); \
                    CORRADE_COMPARE(compressedPixelFormatBlockSize(Magnum::GL::CompressedPixelFormat::expectedFormat), compressedPixelFormatBlockSize(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_COMPARE(compressedPixelFormatBlockDataSize(Magnum::GL::CompressedPixelFormat::expectedFormat), compressedPixelFormatBlockDataSize(Magnum::CompressedPixelFormat::format)); \
                    ++nextHandled; \
                    continue;
            #define _s(format) \
                case Magnum::CompressedPixelFormat::format: { \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasCompressedPixelFormat(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_VERIFY(!hasTextureFormat(Magnum::CompressedPixelFormat::format)); \
                    Containers::String out; \
                    { /* Redirected otherwise graceful assert would abort */ \
                        Error redirectError{&out}; \
                        compressedPixelFormat(Magnum::CompressedPixelFormat::format); \
                        textureFormat(Magnum::CompressedPixelFormat::format); \
                    } \
                    Debug{Debug::Flag::NoNewlineAtTheEnd} << out; \
                    ++nextHandled; \
                    continue; \
                }
            #include "Magnum/GL/Implementation/compressedPixelFormatMapping.hpp"
            #undef _s
            #undef _d
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

void PixelFormatTest::mapCompressedFormatImplementationSpecific() {
    CORRADE_VERIFY(hasCompressedPixelFormat(Magnum::compressedPixelFormatWrap(CompressedPixelFormat::RGBAS3tcDxt1)));
    CORRADE_COMPARE(compressedPixelFormat(Magnum::compressedPixelFormatWrap(CompressedPixelFormat::RGBAS3tcDxt1)),
        CompressedPixelFormat::RGBAS3tcDxt1);
}

void PixelFormatTest::mapCompressedFormatUnsupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_VERIFY(!hasCompressedPixelFormat(Magnum::CompressedPixelFormat::Astc3x3x3RGBAUnorm));

    Containers::String out;
    Error redirectError{&out};
    compressedPixelFormat(Magnum::CompressedPixelFormat::Astc3x3x3RGBAUnorm);
    CORRADE_COMPARE(out, "GL::compressedPixelFormat(): format CompressedPixelFormat::Astc3x3x3RGBAUnorm is not supported on this target\n");
    #elif !defined(MAGNUM_TARGET_GLES)
    CORRADE_VERIFY(!hasCompressedPixelFormat(Magnum::CompressedPixelFormat::PvrtcRGB2bppUnorm));

    Containers::String out;
    Error redirectError{&out};
    compressedPixelFormat(Magnum::CompressedPixelFormat::PvrtcRGB2bppUnorm);
    CORRADE_COMPARE(out, "GL::compressedPixelFormat(): format CompressedPixelFormat::PvrtcRGB2bppUnorm is not supported on this target\n");
    #else
    CORRADE_SKIP("All compressed pixel formats are supported on ES3.");
    #endif
}

void PixelFormatTest::mapCompressedFormatInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    hasCompressedPixelFormat(Magnum::CompressedPixelFormat{});
    hasCompressedPixelFormat(Magnum::CompressedPixelFormat(0x123));
    compressedPixelFormat(Magnum::CompressedPixelFormat{});
    compressedPixelFormat(Magnum::CompressedPixelFormat(0x123));
    CORRADE_COMPARE(out,
        "GL::hasCompressedPixelFormat(): invalid format CompressedPixelFormat(0x0)\n"
        "GL::hasCompressedPixelFormat(): invalid format CompressedPixelFormat(0x123)\n"
        "GL::compressedPixelFormat(): invalid format CompressedPixelFormat(0x0)\n"
        "GL::compressedPixelFormat(): invalid format CompressedPixelFormat(0x123)\n");
}

void PixelFormatTest::mapCompressedTextureFormatImplementationSpecific() {
    CORRADE_VERIFY(hasTextureFormat(Magnum::compressedPixelFormatWrap(CompressedPixelFormat::RGBAS3tcDxt1)));
    CORRADE_COMPARE(textureFormat(Magnum::compressedPixelFormatWrap(CompressedPixelFormat::RGBAS3tcDxt1)),
        TextureFormat::CompressedRGBAS3tcDxt1);
}

void PixelFormatTest::mapCompressedTextureFormatUnsupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_VERIFY(!hasTextureFormat(Magnum::CompressedPixelFormat::Astc3x3x3RGBAUnorm));

    Containers::String out;
    Error redirectError{&out};
    textureFormat(Magnum::CompressedPixelFormat::Astc3x3x3RGBAUnorm);
    CORRADE_COMPARE(out, "GL::textureFormat(): format CompressedPixelFormat::Astc3x3x3RGBAUnorm is not supported on this target\n");
    #elif !defined(MAGNUM_TARGET_GLES)
    CORRADE_VERIFY(!hasTextureFormat(Magnum::CompressedPixelFormat::PvrtcRGB2bppUnorm));

    Containers::String out;
    Error redirectError{&out};
    textureFormat(Magnum::CompressedPixelFormat::PvrtcRGB2bppUnorm);
    CORRADE_COMPARE(out, "GL::textureFormat(): format CompressedPixelFormat::PvrtcRGB2bppUnorm is not supported on this target\n");
    #else
    CORRADE_SKIP("All compressed pixel formats are supported on ES3.");
    #endif
}

void PixelFormatTest::mapCompressedTextureFormatInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    hasTextureFormat(Magnum::CompressedPixelFormat{});
    hasTextureFormat(Magnum::CompressedPixelFormat(0x123));
    textureFormat(Magnum::CompressedPixelFormat{});
    textureFormat(Magnum::CompressedPixelFormat(0x123));
    CORRADE_COMPARE(out,
        "GL::hasTextureFormat(): invalid format CompressedPixelFormat(0x0)\n"
        "GL::hasTextureFormat(): invalid format CompressedPixelFormat(0x123)\n"
        "GL::textureFormat(): invalid format CompressedPixelFormat(0x0)\n"
        "GL::textureFormat(): invalid format CompressedPixelFormat(0x123)\n");
}

void PixelFormatTest::mapGenericCompressedFormatUnsupported() {
    /* Generic formats don't have any generic equivalent yet (heh) */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(genericCompressedPixelFormat(CompressedPixelFormat::Red), Containers::NullOpt);
    CORRADE_COMPARE(genericCompressedPixelFormat(TextureFormat::CompressedRed), Containers::NullOpt);
    #endif
    /* For uncompressed texture formats it returns NullOpt too, instead of
       asserting. See comment in the source for reasons. */
    CORRADE_COMPARE(genericCompressedPixelFormat(TextureFormat::RGB), Containers::NullOpt);
}

void PixelFormatTest::sizeCompressed() {
    /* Just basic sanity verification. Formats that have a matching generic
       format are checked against the generic compressedPixelFormatBlock*Size()
       in mapCompressedFormatTextureFormat() above. */

    CORRADE_COMPARE(compressedPixelFormatBlockSize(CompressedPixelFormat::SRGBAlphaS3tcDxt1), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE(compressedPixelFormatBlockDataSize(CompressedPixelFormat::SRGBAlphaS3tcDxt1), 8);

    CORRADE_COMPARE(compressedPixelFormatBlockSize(CompressedPixelFormat::SRGB8Alpha8Astc10x5), (Vector3i{10, 5, 1}));
    CORRADE_COMPARE(compressedPixelFormatBlockDataSize(CompressedPixelFormat::SRGB8Alpha8Astc10x5), 16);
}

#ifndef MAGNUM_TARGET_GLES
void PixelFormatTest::sizeCompressedGeneric() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    compressedPixelFormatBlockSize(CompressedPixelFormat::RG);
    compressedPixelFormatBlockDataSize(CompressedPixelFormat::RG);
    CORRADE_COMPARE_AS(out,
        "GL::compressedPixelFormatBlockSize(): cannot determine block size of generic GL::CompressedPixelFormat::RG\n"
        "GL::compressedPixelFormatBlockDataSize(): cannot determine block size of generic GL::CompressedPixelFormat::RG\n",
        TestSuite::Compare::String);
}
#endif

void PixelFormatTest::sizeCompressedInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    compressedPixelFormatBlockSize(CompressedPixelFormat(0xdeadbeef));
    compressedPixelFormatBlockDataSize(CompressedPixelFormat(0xdeadbeef));
    CORRADE_COMPARE_AS(out,
        "GL::compressedPixelFormatBlockSize(): unknown format GL::CompressedPixelFormat(0xdeadbeef)\n"
        "GL::compressedPixelFormatBlockDataSize(): unknown format GL::CompressedPixelFormat(0xdeadbeef)\n",
        TestSuite::Compare::String);
}

void PixelFormatTest::debugPixelFormat() {
    Containers::String out;

    Debug(&out) << PixelFormat::RGBA << PixelFormat(0xdead);
    CORRADE_COMPARE(out, "GL::PixelFormat::RGBA GL::PixelFormat(0xdead)\n");
}

void PixelFormatTest::debugPixelType() {
    Containers::String out;

    Debug(&out) << PixelType::UnsignedByte << PixelType(0xdead);
    CORRADE_COMPARE(out, "GL::PixelType::UnsignedByte GL::PixelType(0xdead)\n");
}

void PixelFormatTest::debugCompressedPixelFormat() {
    Containers::String out;

    Debug{&out} << CompressedPixelFormat::RGBS3tcDxt1 << CompressedPixelFormat(0xdead);
    CORRADE_COMPARE(out, "GL::CompressedPixelFormat::RGBS3tcDxt1 GL::CompressedPixelFormat(0xdead)\n");
}

void PixelFormatTest::debugTextureFormat() {
    Containers::String out;

    Debug(&out) << TextureFormat::DepthComponent << TextureFormat(0xdead);
    CORRADE_COMPARE(out, "GL::TextureFormat::DepthComponent GL::TextureFormat(0xdead)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::PixelFormatTest)
