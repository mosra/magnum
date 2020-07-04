/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "PixelFormat.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/GL/TextureFormat.h"

namespace Magnum { namespace GL {

namespace {

#ifndef DOXYGEN_GENERATING_OUTPUT /* It gets *really* confused */
constexpr struct {
    PixelFormat format;
    PixelType type;
} FormatMapping[] {
    #define _c(input, format, type, textureFormat) {PixelFormat::format, PixelType::type},
    /* GCC 4.8 doesn't like just a {} for default enum values */
    #define _n(input, format, type) {PixelFormat::format, PixelType::type},
    #define _s(input) {PixelFormat{}, PixelType{}},
    #include "Magnum/GL/Implementation/pixelFormatMapping.hpp"
    #undef _s
    #undef _n
    #undef _c
};

constexpr TextureFormat TextureFormatMapping[] {
    #define _c(input, format, type, textureFormat) TextureFormat::textureFormat,
    /* GCC 4.8 doesn't like just a {} for default enum values */
    #define _n(input, format, type) TextureFormat{},
    #define _s(input) TextureFormat{},
    #include "Magnum/GL/Implementation/pixelFormatMapping.hpp"
    #undef _s
    #undef _n
    #undef _c
};
#endif

}

bool hasPixelFormat(const Magnum::PixelFormat format) {
    if(isPixelFormatImplementationSpecific(format))
        return true;

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(FormatMapping),
        "GL::hasPixelFormat(): invalid format" << format, {});
    return UnsignedInt(FormatMapping[UnsignedInt(format) - 1].format);
}

bool hasTextureFormat(const Magnum::PixelFormat format) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "GL::hasTextureFormat(): cannot map an implementation-specific pixel format to an OpenGL texture format", {});

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(TextureFormatMapping),
        "GL::hasTextureFormat(): invalid format" << format, {});
    return UnsignedInt(TextureFormatMapping[UnsignedInt(format) - 1]);
}

PixelFormat pixelFormat(const Magnum::PixelFormat format) {
    if(isPixelFormatImplementationSpecific(format))
        return pixelFormatUnwrap<GL::PixelFormat>(format);

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(FormatMapping),
        "GL::pixelFormat(): invalid format" << format, {});
    const PixelFormat out = FormatMapping[UnsignedInt(format) - 1].format;
    CORRADE_ASSERT(UnsignedInt(out),
        "GL::pixelFormat(): format" << format << "is not supported on this target", {});
    return out;
}

PixelType pixelType(const Magnum::PixelFormat format, const UnsignedInt extra) {
    if(isPixelFormatImplementationSpecific(format)) {
        CORRADE_ASSERT(extra,
            "GL::pixelType(): format is implementation-specific, but no additional type specifier was passed", {});
        return PixelType(extra);
    }

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(FormatMapping),
        "GL::pixelType(): invalid format" << format, {});
    const PixelType out = FormatMapping[UnsignedInt(format) - 1].type;
    CORRADE_ASSERT(UnsignedInt(out),
        "GL::pixelType(): format" << format << "is not supported on this target", {});
    return out;
}

TextureFormat textureFormat(const Magnum::PixelFormat format) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "GL::textureFormat(): cannot map an implementation-specific pixel format to an OpenGL texture format", {});

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(FormatMapping),
        "GL::textureFormat(): invalid format" << format, {});
    const TextureFormat out = TextureFormatMapping[UnsignedInt(format) - 1];
    CORRADE_ASSERT(UnsignedInt(out),
        "GL::textureFormat(): format" << format << "is not supported on this target", {});
    return out;
}

UnsignedInt pixelSize(const PixelFormat format, const PixelType type) {
    std::size_t size = 0;
    #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(type) {
        case PixelType::UnsignedByte:
        #ifndef MAGNUM_TARGET_GLES2
        case PixelType::Byte:
        #endif
            size = 1; break;
        case PixelType::UnsignedShort:
        /* Available everywhere except ES2 (WebGL 1 has it) */
        #if !(defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        case PixelType::Short:
        #endif
        case PixelType::Half:
            size = 2; break;
        case PixelType::UnsignedInt:
        #ifndef MAGNUM_TARGET_GLES2
        case PixelType::Int:
        #endif
        case PixelType::Float:
            size = 4; break;

        #ifndef MAGNUM_TARGET_GLES
        case PixelType::UnsignedByte332:
        case PixelType::UnsignedByte233Rev:
            return 1;
        #endif
        case PixelType::UnsignedShort565:
        #ifndef MAGNUM_TARGET_GLES
        case PixelType::UnsignedShort565Rev:
        #endif
        case PixelType::UnsignedShort4444:
        #ifndef MAGNUM_TARGET_WEBGL
        case PixelType::UnsignedShort4444Rev:
        #endif
        case PixelType::UnsignedShort5551:
        #ifndef MAGNUM_TARGET_WEBGL
        case PixelType::UnsignedShort1555Rev:
        #endif
            return 2;
        #ifndef MAGNUM_TARGET_GLES
        case PixelType::UnsignedInt8888:
        case PixelType::UnsignedInt8888Rev:
        case PixelType::UnsignedInt1010102:
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case PixelType::UnsignedInt2101010Rev:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case PixelType::UnsignedInt10F11F11FRev:
        case PixelType::UnsignedInt5999Rev:
        #endif
        case PixelType::UnsignedInt248:
            return 4;
        #ifndef MAGNUM_TARGET_GLES2
        case PixelType::Float32UnsignedInt248Rev:
            return 8;
        #endif
    }
    #ifdef __GNUC__
    #pragma GCC diagnostic pop
    #endif

    #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case PixelFormat::Red:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case PixelFormat::RedInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case PixelFormat::Green:
        case PixelFormat::Blue:
        case PixelFormat::GreenInteger:
        case PixelFormat::BlueInteger:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case PixelFormat::Luminance:
        #endif
        case PixelFormat::DepthComponent:
        #ifndef MAGNUM_TARGET_WEBGL
        case PixelFormat::StencilIndex:
        #endif
            return 1*size;
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case PixelFormat::RG:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case PixelFormat::RGInteger:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case PixelFormat::LuminanceAlpha:
        #endif
            return 2*size;
        case PixelFormat::RGB:
        #ifndef MAGNUM_TARGET_GLES2
        case PixelFormat::RGBInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case PixelFormat::BGR:
        case PixelFormat::BGRInteger:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case PixelFormat::SRGB:
        #endif
            return 3*size;
        case PixelFormat::RGBA:
        #ifndef MAGNUM_TARGET_GLES2
        case PixelFormat::RGBAInteger:
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        case PixelFormat::BGRA:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case PixelFormat::SRGBAlpha:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case PixelFormat::BGRAInteger:
        #endif
            return 4*size;

        /* Handled above */
        case PixelFormat::DepthStencil:
            CORRADE_ASSERT_UNREACHABLE("GL::pixelSize(): invalid" << type << "specified for" << format, 0);
    }
    #ifdef __GNUC__
    #pragma GCC diagnostic pop
    #endif

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const PixelFormat value) {
    debug << "GL::PixelFormat" << Debug::nospace;

    #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case PixelFormat::value: return debug << "::" #value;
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
        /* LCOV_EXCL_STOP */
    }
    #ifdef __GNUC__
    #pragma GCC diagnostic pop
    #endif

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const PixelType value) {
    debug << "GL::PixelType" << Debug::nospace;

    #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case PixelType::value: return debug << "::" #value;
        _c(UnsignedByte)
        #ifndef MAGNUM_TARGET_GLES2
        _c(Byte)
        #endif
        _c(UnsignedShort)
        /* Available everywhere except ES2 (WebGL 1 has it) */
        #if !(defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        _c(Short)
        #endif
        _c(UnsignedInt)
        #ifndef MAGNUM_TARGET_GLES2
        _c(Int)
        #endif
        _c(Half)
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(UnsignedByte332)
        _c(UnsignedByte233Rev)
        #endif
        _c(UnsignedShort565)
        #ifndef MAGNUM_TARGET_GLES
        _c(UnsignedShort565Rev)
        #endif
        _c(UnsignedShort4444)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(UnsignedShort4444Rev)
        #endif
        _c(UnsignedShort5551)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(UnsignedShort1555Rev)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(UnsignedInt8888)
        _c(UnsignedInt8888Rev)
        _c(UnsignedInt1010102)
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(UnsignedInt2101010Rev)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(UnsignedInt10F11F11FRev)
        _c(UnsignedInt5999Rev)
        #endif
        _c(UnsignedInt248)
        #ifndef MAGNUM_TARGET_GLES2
        _c(Float32UnsignedInt248Rev)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }
    #ifdef __GNUC__
    #pragma GCC diagnostic pop
    #endif

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

namespace {

#ifndef DOXYGEN_GENERATING_OUTPUT /* It gets *really* confused */
/* Enum values are the same between CompressedPixelFormat and TextureFormat, so
   having just a single table for both */
constexpr CompressedPixelFormat CompressedFormatMapping[] {
    #define _c(input, format) CompressedPixelFormat::format,
    #define _s(input) CompressedPixelFormat{},
    #include "Magnum/GL/Implementation/compressedPixelFormatMapping.hpp"
    #undef _s
    #undef _c
};
#endif

}

bool hasCompressedPixelFormat(const Magnum::CompressedPixelFormat format) {
    if(isCompressedPixelFormatImplementationSpecific(format))
        return true;

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(CompressedFormatMapping),
        "GL::hasCompressedPixelFormat(): invalid format" << format, {});
    return UnsignedInt(CompressedFormatMapping[UnsignedInt(format) - 1]);
}

bool hasTextureFormat(const Magnum::CompressedPixelFormat format) {
    if(isCompressedPixelFormatImplementationSpecific(format))
        return true;

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(CompressedFormatMapping),
        "GL::hasTextureFormat(): invalid format" << format, {});
    return UnsignedInt(CompressedFormatMapping[UnsignedInt(format) - 1]);
}

CompressedPixelFormat compressedPixelFormat(const Magnum::CompressedPixelFormat format) {
    if(isCompressedPixelFormatImplementationSpecific(format))
        return compressedPixelFormatUnwrap<GL::CompressedPixelFormat>(format);

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(CompressedFormatMapping),
        "GL::compressedPixelFormat(): invalid format" << format, {});
    const CompressedPixelFormat out = CompressedFormatMapping[UnsignedInt(format) - 1];
    CORRADE_ASSERT(UnsignedInt(out),
        "GL::compressedPixelFormat(): format" << format << "is not supported on this target", {});
    return out;
}

TextureFormat textureFormat(const Magnum::CompressedPixelFormat format) {
    if(isCompressedPixelFormatImplementationSpecific(format))
        return compressedPixelFormatUnwrap<GL::TextureFormat>(format);

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(CompressedFormatMapping),
        "GL::textureFormat(): invalid format" << format, {});
    /* Enum values are the same between CompressedPixelFormat and
       TextureFormat, so having just a single table for both and casting */
    const auto out = TextureFormat(GLenum(CompressedFormatMapping[UnsignedInt(format) - 1]));
    CORRADE_ASSERT(UnsignedInt(out),
        "GL::textureFormat(): format" << format << "is not supported on this target", {});
    return out;
}

Debug& operator<<(Debug& debug, const CompressedPixelFormat value) {
    debug << "GL::CompressedPixelFormat" << Debug::nospace;

    #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case CompressedPixelFormat::value: return debug << "::" #value;
        #ifndef MAGNUM_TARGET_GLES
        _c(Red)
        _c(RG)
        _c(RGB)
        _c(RGBA)
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
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
        _c(SRGBS3tcDxt1)
        _c(RGBAS3tcDxt1)
        _c(SRGBAlphaS3tcDxt1)
        _c(RGBAS3tcDxt3)
        _c(SRGBAlphaS3tcDxt3)
        _c(RGBAS3tcDxt5)
        _c(SRGBAlphaS3tcDxt5)
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
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        _c(RGBAAstc3x3x3)
        _c(SRGB8Alpha8Astc3x3x3)
        _c(RGBAAstc4x3x3)
        _c(SRGB8Alpha8Astc4x3x3)
        _c(RGBAAstc4x4x3)
        _c(SRGB8Alpha8Astc4x4x3)
        _c(RGBAAstc4x4x4)
        _c(SRGB8Alpha8Astc4x4x4)
        _c(RGBAAstc5x4x4)
        _c(SRGB8Alpha8Astc5x4x4)
        _c(RGBAAstc5x5x4)
        _c(SRGB8Alpha8Astc5x5x4)
        _c(RGBAAstc5x5x5)
        _c(SRGB8Alpha8Astc5x5x5)
        _c(RGBAAstc6x5x5)
        _c(SRGB8Alpha8Astc6x5x5)
        _c(RGBAAstc6x6x5)
        _c(SRGB8Alpha8Astc6x6x5)
        _c(RGBAAstc6x6x6)
        _c(SRGB8Alpha8Astc6x6x6)
        #endif
        #ifdef MAGNUM_TARGET_GLES
        _c(RGBPvrtc2bppV1)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(SRGBPvrtc2bppV1)
        #endif
        _c(RGBAPvrtc2bppV1)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(SRGBAlphaPvrtc2bppV1)
        #endif
        _c(RGBPvrtc4bppV1)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(SRGBPvrtc4bppV1)
        #endif
        _c(RGBAPvrtc4bppV1)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(SRGBAlphaPvrtc4bppV1)
        #endif
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }
    #ifdef __GNUC__
    #pragma GCC diagnostic pop
    #endif

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

}}
