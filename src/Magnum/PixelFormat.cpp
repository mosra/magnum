/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <Corrade/Utility/Debug.h>

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const PixelFormat value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case PixelFormat::value: return debug << "PixelFormat::" #value;
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

    return debug << "PixelFormat(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const PixelType value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case PixelType::value: return debug << "PixelType::" #value;
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
        _c(HalfFloat)
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

    return debug << "PixelType(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const CompressedPixelFormat value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case CompressedPixelFormat::value: return debug << "CompressedPixelFormat::" #value;
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
        /* LCOV_EXCL_STOP */
    }

    return debug << "CompressedPixelFormat(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

}
