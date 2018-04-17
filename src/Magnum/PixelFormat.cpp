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

#include "Magnum/PixelFormat.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

namespace Magnum {

UnsignedInt pixelSize(const PixelFormat format) {
    CORRADE_ASSERT(!(UnsignedInt(format) & (1 << 31)),
        "pixelSize(): can't determine pixel size of an implementation-specific format", {});

    switch(format) {
        case PixelFormat::R8Unorm:
        case PixelFormat::R8Snorm:
        case PixelFormat::R8UI:
        case PixelFormat::R8I:
            return 1;
        case PixelFormat::RG8Unorm:
        case PixelFormat::RG8Snorm:
        case PixelFormat::RG8UI:
        case PixelFormat::RG8I:
        case PixelFormat::R16Unorm:
        case PixelFormat::R16Snorm:
        case PixelFormat::R16UI:
        case PixelFormat::R16I:
        case PixelFormat::R16F:
            return 2;
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGB8Snorm:
        case PixelFormat::RGB8UI:
        case PixelFormat::RGB8I:
            return 3;
        case PixelFormat::RGBA8Unorm:
        case PixelFormat::RGBA8Snorm:
        case PixelFormat::RGBA8UI:
        case PixelFormat::RGBA8I:
        case PixelFormat::RG16Unorm:
        case PixelFormat::RG16Snorm:
        case PixelFormat::RG16UI:
        case PixelFormat::RG16I:
        case PixelFormat::RG16F:
        case PixelFormat::R32UI:
        case PixelFormat::R32I:
        case PixelFormat::R32F:
            return 4;
        case PixelFormat::RGB16Unorm:
        case PixelFormat::RGB16Snorm:
        case PixelFormat::RGB16UI:
        case PixelFormat::RGB16I:
        case PixelFormat::RGB16F:
            return 6;
        case PixelFormat::RGBA16Unorm:
        case PixelFormat::RGBA16Snorm:
        case PixelFormat::RGBA16UI:
        case PixelFormat::RGBA16I:
        case PixelFormat::RGBA16F:
        case PixelFormat::RG32UI:
        case PixelFormat::RG32I:
        case PixelFormat::RG32F:
            return 8;
        case PixelFormat::RGB32UI:
        case PixelFormat::RGB32I:
        case PixelFormat::RGB32F:
            return 12;
        case PixelFormat::RGBA32UI:
        case PixelFormat::RGBA32I:
        case PixelFormat::RGBA32F:
            return 16;

        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
        CORRADE_IGNORE_DEPRECATED_PUSH
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case PixelFormat::Red:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case PixelFormat::Green:
        case PixelFormat::Blue:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case PixelFormat::Luminance:
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case PixelFormat::RG:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case PixelFormat::LuminanceAlpha:
        #endif
        case PixelFormat::RGB:
        case PixelFormat::RGBA:
        #ifndef MAGNUM_TARGET_GLES
        case PixelFormat::BGR:
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        case PixelFormat::BGRA:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case PixelFormat::SRGB:
        case PixelFormat::SRGBAlpha:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case PixelFormat::RedInteger:
        #ifndef MAGNUM_TARGET_GLES
        case PixelFormat::GreenInteger:
        case PixelFormat::BlueInteger:
        #endif
        case PixelFormat::RGInteger:
        case PixelFormat::RGBInteger:
        case PixelFormat::RGBAInteger:
        #ifndef MAGNUM_TARGET_GLES
        case PixelFormat::BGRInteger:
        case PixelFormat::BGRAInteger:
        #endif
        #endif
        case PixelFormat::DepthComponent:
        #ifndef MAGNUM_TARGET_WEBGL
        case PixelFormat::StencilIndex:
        #endif
        case PixelFormat::DepthStencil:
            /** @todo CORRADE_ASSERT_UNREACHABLE() with message here */
            CORRADE_ASSERT(false,
                "pixelSize(): called with deprecated GL-specific format, use GL::pixelSize() instead", {});
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const PixelFormat value) {
    if(isPixelFormatImplementationSpecific(value)) {
        return debug << "PixelFormat::ImplementationSpecific(" << Debug::nospace << reinterpret_cast<void*>(pixelFormatUnwrap(value)) << Debug::nospace << ")";
    }

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case PixelFormat::value: return debug << "PixelFormat::" #value;
        _c(R8Unorm)
        _c(RG8Unorm)
        _c(RGB8Unorm)
        _c(RGBA8Unorm)
        _c(R8Snorm)
        _c(RG8Snorm)
        _c(RGB8Snorm)
        _c(RGBA8Snorm)
        _c(R8UI)
        _c(RG8UI)
        _c(RGB8UI)
        _c(RGBA8UI)
        _c(R8I)
        _c(RG8I)
        _c(RGB8I)
        _c(RGBA8I)
        _c(R16Unorm)
        _c(RG16Unorm)
        _c(RGB16Unorm)
        _c(RGBA16Unorm)
        _c(R16Snorm)
        _c(RG16Snorm)
        _c(RGB16Snorm)
        _c(RGBA16Snorm)
        _c(R16UI)
        _c(RG16UI)
        _c(RGB16UI)
        _c(RGBA16UI)
        _c(R16I)
        _c(RG16I)
        _c(RGB16I)
        _c(RGBA16I)
        _c(R32UI)
        _c(RG32UI)
        _c(RGB32UI)
        _c(RGBA32UI)
        _c(R32I)
        _c(RG32I)
        _c(RGB32I)
        _c(RGBA32I)
        _c(R16F)
        _c(RG16F)
        _c(RGB16F)
        _c(RGBA16F)
        _c(R32F)
        _c(RG32F)
        _c(RGB32F)
        _c(RGBA32F)
        #undef _c

        /* Verbatim copy from GL/PixelFormat.cpp. Here mainly to suppress
           compiler warnings about unhandled cases, to check that all values
           from the original enum are present and also to check that there are
           no accidentally conflicting values. */
        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
        CORRADE_IGNORE_DEPRECATED_PUSH
        #define _c(value) case PixelFormat::value: return debug << "GL::PixelFormat::" #value;
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
        /* LCOV_EXCL_STOP */
    }

    return debug << "PixelFormat(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const CompressedPixelFormat value) {
    if(isCompressedPixelFormatImplementationSpecific(value)) {
        return debug << "CompressedPixelFormat::ImplementationSpecific(" << Debug::nospace << reinterpret_cast<void*>(compressedPixelFormatUnwrap(value)) << Debug::nospace << ")";
    }

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case CompressedPixelFormat::value: return debug << "CompressedPixelFormat::" #value;
        _c(Bc1RGBUnorm)
        _c(Bc1RGBAUnorm)
        _c(Bc2RGBAUnorm)
        _c(Bc3RGBAUnorm)
        #undef _c

        /* Verbatim copy from GL/PixelFormat.cpp. Here mainly to suppress
           compiler warnings about unhandled cases, to check that all values
           from the original enum are present and also to check that there are
           no accidentally conflicting values. */
        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
        CORRADE_IGNORE_DEPRECATED_PUSH
        #define _c(value) case CompressedPixelFormat::value: return debug << "GL::CompressedPixelFormat::" #value;
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
        #undef _c
        #endif
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
        /* LCOV_EXCL_STOP */
    }

    return debug << "CompressedPixelFormat(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}
#endif

}
