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

#include "PixelFormat.h"

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/GL/TextureFormat.h"

namespace Magnum { namespace GL {

namespace {

constexpr struct {
    PixelFormat format;
    PixelType type;
} FormatMapping[] {
    #define _n(input, format, type) {PixelFormat::format, PixelType::type},
    #define _dn _n
    #define _c(input, format, type, textureFormat) _n(input, format, type)
    #define _d _c
    /* GCC 4.8 doesn't like just a {} for default enum values */
    #define _s(input) {PixelFormat{}, PixelType{}},
    #include "Magnum/GL/Implementation/pixelFormatMapping.hpp"
    #undef _s
    #undef _d
    #undef _c
    #undef _nd
    #undef _n
};

constexpr TextureFormat TextureFormatMapping[] {
    #define _c(input, format, type, textureFormat) TextureFormat::textureFormat,
    #define _d _c
    /* GCC 4.8 doesn't like just a {} for default enum values */
    #define _s(input) TextureFormat{},
    #define _n(input, format, type) _s(input)
    #define _dn _n
    #include "Magnum/GL/Implementation/pixelFormatMapping.hpp"
    #undef _dn
    #undef _n
    #undef _s
    #undef _d
    #undef _c
};

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
        return pixelFormatUnwrap<PixelFormat>(format);

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

UnsignedInt pixelFormatSize(const PixelFormat format, const PixelType type) {
    std::size_t size = 0;
    #ifdef CORRADE_TARGET_GCC
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
        #ifndef MAGNUM_TARGET_GLES2
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
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    #ifdef CORRADE_TARGET_GCC
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
            CORRADE_ASSERT_UNREACHABLE("GL::pixelFormatSize(): invalid" << type << "specified for" << format, 0);
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("GL::pixelFormatSize(): unknown" << format << "or" << type, {});
}

Containers::Optional<Magnum::PixelFormat> genericPixelFormat(const PixelFormat format, PixelType type) {
    /* Assuming a switch will get better optimized to some LUT than an if
       cascade */
    switch((UnsignedLong(format) << 32)|UnsignedLong(type)) {
        #define _n(input, format, type)                                     \
            case (UnsignedLong(PixelFormat::format) << 32)|                 \
                  UnsignedLong(PixelType::type):                            \
                return Magnum::PixelFormat::input;
        #define _c(input, format, type, textureFormat) _n(input, format, type)
        #define _d(input, format, type, textureFormat)
        #define _dn(input, format, type)
        #define _s(input)
        #include "Magnum/GL/Implementation/pixelFormatMapping.hpp"
        #undef _s
        #undef _dn
        #undef _n
        #undef _d
        #undef _c

        #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /* The mapping defaults to Luminance and LuminanceAlpha on ES2.
           Recognize also the R and RG formats from EXT_texture_rg. */
        /** @todo ideally there would be some variant of the above mapping for
            when EXT_texture_rg is supported, to have it consistent */
        case (UnsignedLong(PixelFormat::Red) << 32)|UnsignedLong(PixelType::UnsignedByte):
            return Magnum::PixelFormat::R8Unorm;
        case (UnsignedLong(PixelFormat::RG) << 32)|UnsignedLong(PixelType::UnsignedByte):
            return Magnum::PixelFormat::RG8Unorm;
        #endif
    }

    return {};
}

Containers::Optional<Magnum::PixelFormat> genericPixelFormat(const TextureFormat format) {
    switch(format) {
        #define _c(input, format, type, textureFormat)                      \
            case TextureFormat::textureFormat: return Magnum::PixelFormat::input;
        #define _d _c
        #define _n(input, format, type)
        #define _dn(input, format, type)
        #define _s(input)
        #include "Magnum/GL/Implementation/pixelFormatMapping.hpp"
        #undef _s
        #undef _dn
        #undef _n
        #undef _d
        #undef _c

        #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /* The mapping defaults to Luminance, LuminanceAlpha, RGB and RGBA on
           ES2. Recognize also the sized formats from EXT_texture_rg. */
        /** @todo ideally there would be some variant of the above mapping for
            when EXT_texture_rg is supported, to have it consistent */
        case TextureFormat::R8: return Magnum::PixelFormat::R8Unorm;
        case TextureFormat::RG8: return Magnum::PixelFormat::RG8Unorm;
        case TextureFormat::RGB8: return Magnum::PixelFormat::RGB8Unorm;
        case TextureFormat::RGBA8: return Magnum::PixelFormat::RGBA8Unorm;
        #endif

        /* For compressed formats it returns NullOpt too instead of asserting,
           as -- compared to the generic-to-GL translation, which is O(1) --
           the inverse mapping is potentially a linear lookup and forcing the
           user to check some isTextureFormatCompressed() first (which would do
           another linear lookup) makes no sense from a perf PoV. Plus for
           unknown formats it's unknown whether it's a compressed format or
           not, and the function suddenly starting to assert when a format
           becomes known isn't good for backwards compatibility. */
        default:
            return {};
    }
}

Debug& operator<<(Debug& debug, const PixelFormat value) {
    debug << "GL::PixelFormat" << Debug::nospace;

    #ifdef CORRADE_TARGET_GCC
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
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const PixelType value) {
    debug << "GL::PixelType" << Debug::nospace;

    #ifdef CORRADE_TARGET_GCC
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
        #ifndef MAGNUM_TARGET_GLES2
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
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

namespace {

/* Enum values are the same between CompressedPixelFormat and TextureFormat, so
   having just a single table for both */
constexpr CompressedPixelFormat CompressedFormatMapping[] {
    #define _c(input, format) CompressedPixelFormat::format,
    #define _d _c
    #define _s(input) CompressedPixelFormat{},
    #include "Magnum/GL/Implementation/compressedPixelFormatMapping.hpp"
    #undef _s
    #undef _d
    #undef _c
};

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
        return compressedPixelFormatUnwrap<CompressedPixelFormat>(format);

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(CompressedFormatMapping),
        "GL::compressedPixelFormat(): invalid format" << format, {});
    const CompressedPixelFormat out = CompressedFormatMapping[UnsignedInt(format) - 1];
    CORRADE_ASSERT(UnsignedInt(out),
        "GL::compressedPixelFormat(): format" << format << "is not supported on this target", {});
    return out;
}

Vector3i compressedPixelFormatBlockSize(const CompressedPixelFormat format) {
    switch(format) {
        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        case CompressedPixelFormat::RedRgtc1:
        case CompressedPixelFormat::RGRgtc2:
        case CompressedPixelFormat::SignedRedRgtc1:
        case CompressedPixelFormat::SignedRGRgtc2:
        case CompressedPixelFormat::RGBBptcUnsignedFloat:
        case CompressedPixelFormat::RGBBptcSignedFloat:
        case CompressedPixelFormat::RGBABptcUnorm:
        case CompressedPixelFormat::SRGBAlphaBptcUnorm:
        #endif
        case CompressedPixelFormat::RGB8Etc2:
        case CompressedPixelFormat::SRGB8Etc2:
        case CompressedPixelFormat::RGB8PunchthroughAlpha1Etc2:
        case CompressedPixelFormat::SRGB8PunchthroughAlpha1Etc2:
        case CompressedPixelFormat::RGBA8Etc2Eac:
        case CompressedPixelFormat::SRGB8Alpha8Etc2Eac:
        case CompressedPixelFormat::R11Eac:
        case CompressedPixelFormat::SignedR11Eac:
        case CompressedPixelFormat::RG11Eac:
        case CompressedPixelFormat::SignedRG11Eac:
        case CompressedPixelFormat::RGBS3tcDxt1:
        case CompressedPixelFormat::SRGBS3tcDxt1:
        case CompressedPixelFormat::RGBAS3tcDxt1:
        case CompressedPixelFormat::SRGBAlphaS3tcDxt1:
        case CompressedPixelFormat::RGBAS3tcDxt3:
        case CompressedPixelFormat::SRGBAlphaS3tcDxt3:
        case CompressedPixelFormat::RGBAS3tcDxt5:
        case CompressedPixelFormat::SRGBAlphaS3tcDxt5:
        #ifdef MAGNUM_TARGET_GLES
        case CompressedPixelFormat::RGBPvrtc4bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case CompressedPixelFormat::SRGBPvrtc4bppV1:
        #endif
        case CompressedPixelFormat::RGBAPvrtc4bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case CompressedPixelFormat::SRGBAlphaPvrtc4bppV1:
        #endif
        #endif
        case CompressedPixelFormat::RGBAAstc4x4:
        case CompressedPixelFormat::SRGB8Alpha8Astc4x4:
            return {4, 4, 1};
        case CompressedPixelFormat::RGBAAstc5x4:
        case CompressedPixelFormat::SRGB8Alpha8Astc5x4:
            return {5, 4, 1};
        case CompressedPixelFormat::RGBAAstc5x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc5x5:
            return {5, 5, 1};
        case CompressedPixelFormat::RGBAAstc6x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc6x5:
            return {6, 5, 1};
        case CompressedPixelFormat::RGBAAstc6x6:
        case CompressedPixelFormat::SRGB8Alpha8Astc6x6:
            return {6, 6, 1};
        case CompressedPixelFormat::RGBAAstc8x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc8x5:
            return {8, 5, 1};
        case CompressedPixelFormat::RGBAAstc8x6:
        case CompressedPixelFormat::SRGB8Alpha8Astc8x6:
            return {8, 6, 1};
        case CompressedPixelFormat::RGBAAstc8x8:
        case CompressedPixelFormat::SRGB8Alpha8Astc8x8:
            return {8, 8, 1};
        case CompressedPixelFormat::RGBAAstc10x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc10x5:
            return {10, 5, 1};
        case CompressedPixelFormat::RGBAAstc10x6:
        case CompressedPixelFormat::SRGB8Alpha8Astc10x6:
            return {10, 6, 1};
        case CompressedPixelFormat::RGBAAstc10x8:
        case CompressedPixelFormat::SRGB8Alpha8Astc10x8:
            return {10, 8, 1};
        case CompressedPixelFormat::RGBAAstc10x10:
        case CompressedPixelFormat::SRGB8Alpha8Astc10x10:
            return {10, 10, 1};
        case CompressedPixelFormat::RGBAAstc12x10:
        case CompressedPixelFormat::SRGB8Alpha8Astc12x10:
            return {12, 10, 1};
        case CompressedPixelFormat::RGBAAstc12x12:
        case CompressedPixelFormat::SRGB8Alpha8Astc12x12:
            return {12, 12, 1};
        #ifdef MAGNUM_TARGET_GLES
        case CompressedPixelFormat::RGBPvrtc2bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case CompressedPixelFormat::SRGBPvrtc2bppV1:
        #endif
        case CompressedPixelFormat::RGBAPvrtc2bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case CompressedPixelFormat::SRGBAlphaPvrtc2bppV1:
        #endif
            return {8, 4, 1};
        #endif
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        case CompressedPixelFormat::RGBAAstc3x3x3:
        case CompressedPixelFormat::SRGB8Alpha8Astc3x3x3:
            return {3, 3, 3};
        case CompressedPixelFormat::RGBAAstc4x3x3:
        case CompressedPixelFormat::SRGB8Alpha8Astc4x3x3:
            return {4, 3, 3};
        case CompressedPixelFormat::RGBAAstc4x4x3:
        case CompressedPixelFormat::SRGB8Alpha8Astc4x4x3:
            return {4, 4, 3};
        case CompressedPixelFormat::RGBAAstc4x4x4:
        case CompressedPixelFormat::SRGB8Alpha8Astc4x4x4:
            return {4, 4, 4};
        case CompressedPixelFormat::RGBAAstc5x4x4:
        case CompressedPixelFormat::SRGB8Alpha8Astc5x4x4:
            return {5, 4, 4};
        case CompressedPixelFormat::RGBAAstc5x5x4:
        case CompressedPixelFormat::SRGB8Alpha8Astc5x5x4:
            return {5, 5, 4};
        case CompressedPixelFormat::RGBAAstc5x5x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc5x5x5:
            return {5, 5, 5};
        case CompressedPixelFormat::RGBAAstc6x5x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc6x5x5:
            return {6, 5, 5};
        case CompressedPixelFormat::RGBAAstc6x6x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc6x6x5:
            return {6, 6, 5};
        case CompressedPixelFormat::RGBAAstc6x6x6:
        case CompressedPixelFormat::SRGB8Alpha8Astc6x6x6:
            return {6, 6, 6};
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case CompressedPixelFormat::Red:
        case CompressedPixelFormat::RG:
        case CompressedPixelFormat::RGB:
        case CompressedPixelFormat::RGBA:
            CORRADE_ASSERT_UNREACHABLE("GL::compressedPixelFormatBlockSize(): cannot determine block size of generic" << format, {});
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE("GL::compressedPixelFormatBlockSize(): unknown format" << format, {});
}

UnsignedInt compressedPixelFormatBlockDataSize(const CompressedPixelFormat format) {
    switch(format) {
        /* Values from Magnum/Implementation/compressedPixelFormatMapping.hpp,
           names matched w/ GL/Implementation/compressedPixelFormatMapping.hpp.
           Using the bit sizes to avoid accidental errors. */
        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        case CompressedPixelFormat::RedRgtc1:
        case CompressedPixelFormat::SignedRedRgtc1:
        #endif
        case CompressedPixelFormat::RGB8Etc2:
        case CompressedPixelFormat::SRGB8Etc2:
        case CompressedPixelFormat::R11Eac:
        case CompressedPixelFormat::SignedR11Eac:
        case CompressedPixelFormat::RGB8PunchthroughAlpha1Etc2:
        case CompressedPixelFormat::SRGB8PunchthroughAlpha1Etc2:
        case CompressedPixelFormat::RGBS3tcDxt1:
        case CompressedPixelFormat::SRGBS3tcDxt1:
        case CompressedPixelFormat::RGBAS3tcDxt1:
        case CompressedPixelFormat::SRGBAlphaS3tcDxt1:
        #ifdef MAGNUM_TARGET_GLES
        case CompressedPixelFormat::RGBPvrtc2bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case CompressedPixelFormat::SRGBPvrtc2bppV1:
        #endif
        case CompressedPixelFormat::RGBAPvrtc2bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case CompressedPixelFormat::SRGBAlphaPvrtc2bppV1:
        #endif
        case CompressedPixelFormat::RGBPvrtc4bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case CompressedPixelFormat::SRGBPvrtc4bppV1:
        #endif
        case CompressedPixelFormat::RGBAPvrtc4bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case CompressedPixelFormat::SRGBAlphaPvrtc4bppV1:
        #endif
        #endif
            return 64/8;
        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        case CompressedPixelFormat::RGRgtc2:
        case CompressedPixelFormat::SignedRGRgtc2:
        case CompressedPixelFormat::RGBBptcUnsignedFloat:
        case CompressedPixelFormat::RGBBptcSignedFloat:
        case CompressedPixelFormat::RGBABptcUnorm:
        case CompressedPixelFormat::SRGBAlphaBptcUnorm:
        #endif
        case CompressedPixelFormat::RG11Eac:
        case CompressedPixelFormat::SignedRG11Eac:
        case CompressedPixelFormat::RGBA8Etc2Eac:
        case CompressedPixelFormat::SRGB8Alpha8Etc2Eac:
        case CompressedPixelFormat::RGBAS3tcDxt3:
        case CompressedPixelFormat::SRGBAlphaS3tcDxt3:
        case CompressedPixelFormat::RGBAS3tcDxt5:
        case CompressedPixelFormat::SRGBAlphaS3tcDxt5:
        case CompressedPixelFormat::RGBAAstc4x4:
        case CompressedPixelFormat::SRGB8Alpha8Astc4x4:
        case CompressedPixelFormat::RGBAAstc5x4:
        case CompressedPixelFormat::SRGB8Alpha8Astc5x4:
        case CompressedPixelFormat::RGBAAstc5x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc5x5:
        case CompressedPixelFormat::RGBAAstc6x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc6x5:
        case CompressedPixelFormat::RGBAAstc6x6:
        case CompressedPixelFormat::SRGB8Alpha8Astc6x6:
        case CompressedPixelFormat::RGBAAstc8x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc8x5:
        case CompressedPixelFormat::RGBAAstc8x6:
        case CompressedPixelFormat::SRGB8Alpha8Astc8x6:
        case CompressedPixelFormat::RGBAAstc8x8:
        case CompressedPixelFormat::SRGB8Alpha8Astc8x8:
        case CompressedPixelFormat::RGBAAstc10x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc10x5:
        case CompressedPixelFormat::RGBAAstc10x6:
        case CompressedPixelFormat::SRGB8Alpha8Astc10x6:
        case CompressedPixelFormat::RGBAAstc10x8:
        case CompressedPixelFormat::SRGB8Alpha8Astc10x8:
        case CompressedPixelFormat::RGBAAstc10x10:
        case CompressedPixelFormat::SRGB8Alpha8Astc10x10:
        case CompressedPixelFormat::RGBAAstc12x10:
        case CompressedPixelFormat::SRGB8Alpha8Astc12x10:
        case CompressedPixelFormat::RGBAAstc12x12:
        case CompressedPixelFormat::SRGB8Alpha8Astc12x12:
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        case CompressedPixelFormat::RGBAAstc3x3x3:
        case CompressedPixelFormat::SRGB8Alpha8Astc3x3x3:
        case CompressedPixelFormat::RGBAAstc4x3x3:
        case CompressedPixelFormat::SRGB8Alpha8Astc4x3x3:
        case CompressedPixelFormat::RGBAAstc4x4x3:
        case CompressedPixelFormat::SRGB8Alpha8Astc4x4x3:
        case CompressedPixelFormat::RGBAAstc4x4x4:
        case CompressedPixelFormat::SRGB8Alpha8Astc4x4x4:
        case CompressedPixelFormat::RGBAAstc5x4x4:
        case CompressedPixelFormat::SRGB8Alpha8Astc5x4x4:
        case CompressedPixelFormat::RGBAAstc5x5x4:
        case CompressedPixelFormat::SRGB8Alpha8Astc5x5x4:
        case CompressedPixelFormat::RGBAAstc5x5x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc5x5x5:
        case CompressedPixelFormat::RGBAAstc6x5x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc6x5x5:
        case CompressedPixelFormat::RGBAAstc6x6x5:
        case CompressedPixelFormat::SRGB8Alpha8Astc6x6x5:
        case CompressedPixelFormat::RGBAAstc6x6x6:
        case CompressedPixelFormat::SRGB8Alpha8Astc6x6x6:
        #endif
            return 128/8;
        #ifndef MAGNUM_TARGET_GLES
        case CompressedPixelFormat::Red:
        case CompressedPixelFormat::RG:
        case CompressedPixelFormat::RGB:
        case CompressedPixelFormat::RGBA:
            CORRADE_ASSERT_UNREACHABLE("GL::compressedPixelFormatBlockDataSize(): cannot determine block size of generic" << format, {});
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE("GL::compressedPixelFormatBlockDataSize(): unknown format" << format, {});
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

Containers::Optional<Magnum::CompressedPixelFormat> genericCompressedPixelFormat(const CompressedPixelFormat format) {
    switch(format) {
        #define _c(input, format)                                           \
            case CompressedPixelFormat::format:                             \
                return Magnum::CompressedPixelFormat::input;
        #define _d(input, format)
        #define _s(input)
        #include "Magnum/GL/Implementation/compressedPixelFormatMapping.hpp"
        #undef _s
        #undef _d
        #undef _c

        default:
            return {};
    }
}

Containers::Optional<Magnum::CompressedPixelFormat> genericCompressedPixelFormat(const TextureFormat format) {
    /* Enum values are the same between CompressedPixelFormat and
       TextureFormat, so just casting and delegating.

       For uncompressed formats it returns NullOpt too instead of asserting, as
       -- compared to the generic-to-GL translation, which is O(1) -- the
       inverse mapping is potentially a linear lookup and forcing the user to
       check some isTextureFormatCompressed() first (which would do another
       linear lookup) makes no sense from a perf PoV. Plus for unknown formats
       it's unknown whether it's a compressed format or not, and the function
       suddenly starting to assert when a format becomes known isn't good for
       backwards compatibility. */
    return genericCompressedPixelFormat(CompressedPixelFormat(UnsignedInt(format)));
}

Debug& operator<<(Debug& debug, const CompressedPixelFormat value) {
    debug << "GL::CompressedPixelFormat" << Debug::nospace;

    #ifdef CORRADE_TARGET_GCC
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
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

}}
