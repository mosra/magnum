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

/* See Magnum/GL/PixelFormat.cpp, Magnum/GL/Test/PixelFormatTest.cpp and
   DebugTools/Screenshot.cpp. _c() is a mapping, _s() denotes a skipped value
   (so the enum numbering is preserved), _n() denotes a value where pixel
   format mapping is defined, but texture format is not */
#ifdef _c
#ifndef MAGNUM_TARGET_GLES2
_c(R8Unorm, Red, UnsignedByte, R8)
_c(RG8Unorm, RG, UnsignedByte, RG8)
#else
_n(R8Unorm, Luminance, UnsignedByte)
_n(RG8Unorm, LuminanceAlpha, UnsignedByte)
#endif
#ifndef MAGNUM_TARGET_GLES2
_c(RGB8Unorm, RGB, UnsignedByte, RGB8)
_c(RGBA8Unorm, RGBA, UnsignedByte, RGBA8)
#else
_n(RGB8Unorm, RGB, UnsignedByte)
_n(RGBA8Unorm, RGBA, UnsignedByte)
#endif
#ifndef MAGNUM_TARGET_GLES2
_c(R8Snorm, Red, Byte, R8Snorm)
_c(RG8Snorm, RG, Byte, R8Snorm)
_c(RGB8Snorm, RGB, Byte, R8Snorm)
_c(RGBA8Snorm, RGBA, Byte, R8Snorm)
#else
_s(R8Snorm)
_s(RG8Snorm)
_s(RGB8Snorm)
_s(RGBA8Snorm)
#endif
/* Yes, GL's pixel format doesn't distinguish between linear and sRGB, so
   mapping is the same as in case of the Unorm types. It's encoded in the
   texture format tho. */
#ifndef MAGNUM_TARGET_GLES2
#ifndef MAGNUM_TARGET_WEBGL
_c(R8Srgb, Red, UnsignedByte, SR8)
#ifdef MAGNUM_TARGET_GLES
_c(RG8Srgb, RG, UnsignedByte, SRG8)
#else
_n(RG8Srgb, RG, UnsignedByte)
#endif
#else
_n(R8Srgb, Red, UnsignedByte)
_n(RG8Srgb, RG, UnsignedByte)
#endif
#else
/* SLUMINANCE / SLUMINANCE_ALPHA texture formats not exposed */
_n(R8Srgb, Luminance, UnsignedByte)
_n(RG8Srgb, LuminanceAlpha, UnsignedByte)
#endif
#ifndef MAGNUM_TARGET_GLES2
_c(RGB8Srgb, RGB, UnsignedByte, SRGB8)
_c(RGBA8Srgb, RGBA, UnsignedByte, SRGB8Alpha8)
#else
_n(RGB8Srgb, RGB, UnsignedByte)
_n(RGBA8Srgb, RGBA, UnsignedByte)
#endif
#ifndef MAGNUM_TARGET_GLES2
_c(R8UI, RedInteger, UnsignedByte, R8UI)
_c(RG8UI, RGInteger, UnsignedByte, RG8UI)
_c(RGB8UI, RGBInteger, UnsignedByte, RGB8UI)
_c(RGBA8UI, RGBAInteger, UnsignedByte, RGBA8UI)
_c(R8I, RedInteger, Byte, R8I)
_c(RG8I, RGInteger, Byte, RG8I)
_c(RGB8I, RGBInteger, Byte, RGB8I)
_c(RGBA8I, RGBAInteger, Byte, RGBA8I)
#else
_s(R8UI)
_s(RG8UI)
_s(RGB8UI)
_s(RGBA8UI)
_s(R8I)
_s(RG8I)
_s(RGB8I)
_s(RGBA8I)
#endif
#ifndef MAGNUM_TARGET_GLES
_c(R16Unorm, Red, UnsignedShort, R16)
_c(RG16Unorm, RG, UnsignedShort, RG16)
_c(RGB16Unorm, RGB, UnsignedShort, RGB16)
_c(RGBA16Unorm, RGBA, UnsignedShort, RGBA16)
#elif !defined(MAGNUM_TARGET_GLES2)
_n(R16Unorm, Red, UnsignedShort)
_n(RG16Unorm, RG, UnsignedShort)
_n(RGB16Unorm, RGB, UnsignedShort)
_n(RGBA16Unorm, RGBA, UnsignedShort)
#else
_n(R16Unorm, Luminance, UnsignedShort)
_n(RG16Unorm, LuminanceAlpha, UnsignedShort)
_n(RGB16Unorm, RGB, UnsignedShort)
_n(RGBA16Unorm, RGBA, UnsignedShort)
#endif
/* Available everywhere except ES2 (WebGL 1 has it) */
#ifndef MAGNUM_TARGET_GLES2
_c(R16Snorm, Red, Short, R16Snorm)
_c(RG16Snorm, RG, Short, RG16Snorm)
_c(RGB16Snorm, RGB, Short, RGB16Snorm)
_c(RGBA16Snorm, RGBA, Short, RGBA16Snorm)
#elif defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL)
_c(R16Snorm, Luminance, Short, R16Snorm)
_c(RG16Snorm, LuminanceAlpha, Short, RG16Snorm)
_c(RGB16Snorm, RGB, Short, RGB16Snorm)
_c(RGBA16Snorm, RGBA, Short, RGBA16Snorm)
#else
_s(R16Snorm)
_s(RG16Snorm)
_s(RGB16Snorm)
_s(RGBA16Snorm)
#endif
#ifndef MAGNUM_TARGET_GLES2
_c(R16UI, RedInteger, UnsignedShort, R16UI)
_c(RG16UI, RGInteger, UnsignedShort, RG16UI)
_c(RGB16UI, RGBInteger, UnsignedShort, RGB16UI)
_c(RGBA16UI, RGBAInteger, UnsignedShort, RGBA16UI)
_c(R16I, RedInteger, Short, R16I)
_c(RG16I, RGInteger, Short, RG16I)
_c(RGB16I, RGBInteger, Short, RGB16I)
_c(RGBA16I, RGBAInteger, Short, RGBA16I)
_c(R32UI, RedInteger, UnsignedInt, R32UI)
_c(RG32UI, RGInteger, UnsignedInt, RG32UI)
_c(RGB32UI, RGBInteger, UnsignedInt, RGBA32UI)
_c(RGBA32UI, RGBAInteger, UnsignedInt, RGBA32UI)
_c(R32I, RedInteger, Int, R32I)
_c(RG32I, RGInteger, Int, RG32I)
_c(RGB32I, RGBInteger, Int, RGB32I)
_c(RGBA32I, RGBAInteger, Int, RGBA32I)
#else
_s(R16UI)
_s(RG16UI)
_s(RGB16UI)
_s(RGBA16UI)
_s(R16I)
_s(RG16I)
_s(RGB16I)
_s(RGBA16I)
_s(R32UI)
_s(RG32UI)
_s(RGB32UI)
_s(RGBA32UI)
_s(R32I)
_s(RG32I)
_s(RGB32I)
_s(RGBA32I)
#endif
#ifndef MAGNUM_TARGET_GLES2
_c(R16F, Red, Half, R16F)
_c(RG16F, RG, Half, RG16F)
_c(RGB16F, RGB, Half, RGB16F)
_c(RGBA16F, RGBA, Half, RGBA16F)
#else
_n(R16F, Luminance, Half)
_n(RG16F, LuminanceAlpha, Half)
_n(RGB16F, RGB, Half)
_n(RGBA16F, RGBA, Half)
#endif
#ifndef MAGNUM_TARGET_GLES2
_c(R32F, Red, Float, R32F)
_c(RG32F, RG, Float, RG32F)
_c(RGB32F, RGB, Float, RGB32F)
_c(RGBA32F, RGBA, Float, RGBA32F)
#else
_n(R32F, Luminance, Float)
_n(RG32F, LuminanceAlpha, Float)
_n(RGB32F, RGB, Float)
_n(RGBA32F, RGBA, Float)
#endif
#endif
