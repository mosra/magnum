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

#include "TextureFormat.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/GL/PixelFormat.h"

namespace Magnum { namespace GL {

/* the textureFormat() and hasTextureFormat() utilities share mapping tables
   with pixelFormat() / hasPixelFormat() so are defined in PixelFormat.cpp
   instead (and tested there too) */

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const TextureFormat value) {
    debug << "GL::TextureFormat" << Debug::nospace;

    #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case TextureFormat::value: return debug << "::" #value;
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(Red)
        _c(R8)
        _c(RG)
        _c(RG8)
        #endif
        _c(RGB)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(RGB8)
        #endif
        _c(RGBA)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(RGBA8)
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        _c(SR8)
        #ifdef MAGNUM_TARGET_GLES
        _c(SRG8)
        #endif
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        _c(SRGB)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(SRGB8)
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        _c(SRGBAlpha)
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(SRGB8Alpha8)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(R8Snorm)
        _c(RG8Snorm)
        _c(RGB8Snorm)
        _c(RGBA8Snorm)
        #endif
        #if !(defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        _c(R16)
        _c(RG16)
        _c(RGB16)
        _c(RGBA16)
        _c(R16Snorm)
        _c(RG16Snorm)
        _c(RGB16Snorm)
        _c(RGBA16Snorm)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(R8UI)
        _c(RG8UI)
        _c(RGB8UI)
        _c(RGBA8UI)
        _c(R8I)
        _c(RG8I)
        _c(RGB8I)
        _c(RGBA8I)
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
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        _c(Luminance)
        _c(LuminanceAlpha)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(R3G3B2)
        _c(RGB4)
        _c(RGB5)
        #endif
        _c(RGB565)
        #if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        _c(RGB10)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(RGB12)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(R11FG11FB10F)
        _c(RGB9E5)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(RGBA2)
        #endif
        _c(RGBA4)
        _c(RGB5A1)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(RGB10A2)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(RGB10A2UI)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(RGBA12)
        #endif
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        _c(BGRA)
        _c(BGRA8)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(CompressedRed)
        _c(CompressedRG)
        _c(CompressedRGB)
        _c(CompressedRGBA)
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        _c(CompressedRedRgtc1)
        _c(CompressedRGRgtc2)
        _c(CompressedSignedRedRgtc1)
        _c(CompressedSignedRGRgtc2)
        _c(CompressedRGBBptcUnsignedFloat)
        _c(CompressedRGBBptcSignedFloat)
        _c(CompressedRGBABptcUnorm)
        _c(CompressedSRGBAlphaBptcUnorm)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(CompressedRGB8Etc2)
        _c(CompressedSRGB8Etc2)
        _c(CompressedRGB8PunchthroughAlpha1Etc2)
        _c(CompressedSRGB8PunchthroughAlpha1Etc2)
        _c(CompressedRGBA8Etc2Eac)
        _c(CompressedSRGB8Alpha8Etc2Eac)
        _c(CompressedR11Eac)
        _c(CompressedSignedR11Eac)
        _c(CompressedRG11Eac)
        _c(CompressedSignedRG11Eac)
        #endif
        _c(CompressedRGBS3tcDxt1)
        _c(CompressedSRGBS3tcDxt1)
        _c(CompressedRGBAS3tcDxt1)
        _c(CompressedSRGBAlphaS3tcDxt1)
        _c(CompressedRGBAS3tcDxt3)
        _c(CompressedSRGBAlphaS3tcDxt3)
        _c(CompressedRGBAS3tcDxt5)
        _c(CompressedSRGBAlphaS3tcDxt5)
        _c(CompressedRGBAAstc4x4)
        _c(CompressedSRGB8Alpha8Astc4x4)
        _c(CompressedRGBAAstc5x4)
        _c(CompressedSRGB8Alpha8Astc5x4)
        _c(CompressedRGBAAstc5x5)
        _c(CompressedSRGB8Alpha8Astc5x5)
        _c(CompressedRGBAAstc6x5)
        _c(CompressedSRGB8Alpha8Astc6x5)
        _c(CompressedRGBAAstc6x6)
        _c(CompressedSRGB8Alpha8Astc6x6)
        _c(CompressedRGBAAstc8x5)
        _c(CompressedSRGB8Alpha8Astc8x5)
        _c(CompressedRGBAAstc8x6)
        _c(CompressedSRGB8Alpha8Astc8x6)
        _c(CompressedRGBAAstc8x8)
        _c(CompressedSRGB8Alpha8Astc8x8)
        _c(CompressedRGBAAstc10x5)
        _c(CompressedSRGB8Alpha8Astc10x5)
        _c(CompressedRGBAAstc10x6)
        _c(CompressedSRGB8Alpha8Astc10x6)
        _c(CompressedRGBAAstc10x8)
        _c(CompressedSRGB8Alpha8Astc10x8)
        _c(CompressedRGBAAstc10x10)
        _c(CompressedSRGB8Alpha8Astc10x10)
        _c(CompressedRGBAAstc12x10)
        _c(CompressedSRGB8Alpha8Astc12x10)
        _c(CompressedRGBAAstc12x12)
        _c(CompressedSRGB8Alpha8Astc12x12)
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        _c(CompressedRGBAAstc3x3x3)
        _c(CompressedSRGB8Alpha8Astc3x3x3)
        _c(CompressedRGBAAstc4x3x3)
        _c(CompressedSRGB8Alpha8Astc4x3x3)
        _c(CompressedRGBAAstc4x4x3)
        _c(CompressedSRGB8Alpha8Astc4x4x3)
        _c(CompressedRGBAAstc4x4x4)
        _c(CompressedSRGB8Alpha8Astc4x4x4)
        _c(CompressedRGBAAstc5x4x4)
        _c(CompressedSRGB8Alpha8Astc5x4x4)
        _c(CompressedRGBAAstc5x5x4)
        _c(CompressedSRGB8Alpha8Astc5x5x4)
        _c(CompressedRGBAAstc5x5x5)
        _c(CompressedSRGB8Alpha8Astc5x5x5)
        _c(CompressedRGBAAstc6x5x5)
        _c(CompressedSRGB8Alpha8Astc6x5x5)
        _c(CompressedRGBAAstc6x6x5)
        _c(CompressedSRGB8Alpha8Astc6x6x5)
        _c(CompressedRGBAAstc6x6x6)
        _c(CompressedSRGB8Alpha8Astc6x6x6)
        #endif
        #ifdef MAGNUM_TARGET_GLES
        _c(CompressedRGBPvrtc2bppV1)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(CompressedSRGBPvrtc2bppV1)
        #endif
        _c(CompressedRGBAPvrtc2bppV1)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(CompressedSRGBAlphaPvrtc2bppV1)
        #endif
        _c(CompressedRGBPvrtc4bppV1)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(CompressedSRGBPvrtc4bppV1)
        #endif
        _c(CompressedRGBAPvrtc4bppV1)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(CompressedSRGBAlphaPvrtc4bppV1)
        #endif
        #endif
        _c(DepthComponent)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(DepthComponent16)
        _c(DepthComponent24)
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        _c(DepthComponent32)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(DepthComponent32F)
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        _c(StencilIndex8)
        #endif
        _c(DepthStencil)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(Depth24Stencil8)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(Depth32FStencil8)
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
