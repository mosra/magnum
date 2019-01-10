/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

/* See Magnum/GL/PixelFormat.cpp and Magnum/GL/Test/PixelFormatTest.cpp */
#ifdef _c
#ifndef MAGNUM_TARGET_GLES2
_c(R8Unorm, Red, UnsignedByte)
_c(RG8Unorm, RG, UnsignedByte)
#else
_c(R8Unorm, Luminance, UnsignedByte)
_c(RG8Unorm, LuminanceAlpha, UnsignedByte)
#endif
_c(RGB8Unorm, RGB, UnsignedByte)
_c(RGBA8Unorm, RGBA, UnsignedByte)
#ifndef MAGNUM_TARGET_GLES2
_c(R8Snorm, Red, Byte)
_c(RG8Snorm, RG, Byte)
_c(RGB8Snorm, RGB, Byte)
_c(RGBA8Snorm, RGBA, Byte)
#else
_s(R8Snorm)
_s(RG8Snorm)
_s(RGB8Snorm)
_s(RGBA8Snorm)
#endif
#ifndef MAGNUM_TARGET_GLES2
_c(R8UI, RedInteger, UnsignedByte)
_c(RG8UI, RGInteger, UnsignedByte)
_c(RGB8UI, RGBInteger, UnsignedByte)
_c(RGBA8UI, RGBAInteger, UnsignedByte)
_c(R8I, RedInteger, Byte)
_c(RG8I, RGInteger, Byte)
_c(RGB8I, RGBInteger, Byte)
_c(RGBA8I, RGBAInteger, Byte)
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
#ifndef MAGNUM_TARGET_GLES2
_c(R16Unorm, Red, UnsignedShort)
_c(RG16Unorm, RG, UnsignedShort)
#else
_c(R16Unorm, Luminance, UnsignedShort)
_c(RG16Unorm, LuminanceAlpha, UnsignedShort)
#endif
_c(RGB16Unorm, RGB, UnsignedShort)
_c(RGBA16Unorm, RGBA, UnsignedShort)
#ifndef MAGNUM_TARGET_GLES2
_c(R16Snorm, Red, Short)
_c(RG16Snorm, RG, Short)
_c(RGB16Snorm, RGB, Short)
_c(RGBA16Snorm, RGBA, Short)
#else
_s(R16Snorm)
_s(RG16Snorm)
_s(RGB16Snorm)
_s(RGBA16Snorm)
#endif
#ifndef MAGNUM_TARGET_GLES2
_c(R16UI, RedInteger, UnsignedShort)
_c(RG16UI, RGInteger, UnsignedShort)
_c(RGB16UI, RGBInteger, UnsignedShort)
_c(RGBA16UI, RGBAInteger, UnsignedShort)
_c(R16I, RedInteger, Short)
_c(RG16I, RGInteger, Short)
_c(RGB16I, RGBInteger, Short)
_c(RGBA16I, RGBAInteger, Short)
_c(R32UI, RedInteger, UnsignedInt)
_c(RG32UI, RGInteger, UnsignedInt)
_c(RGB32UI, RGBInteger, UnsignedInt)
_c(RGBA32UI, RGBAInteger, UnsignedInt)
_c(R32I, RedInteger, Int)
_c(RG32I, RGInteger, Int)
_c(RGB32I, RGBInteger, Int)
_c(RGBA32I, RGBAInteger, Int)
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
_c(R16F, Red, HalfFloat)
_c(RG16F, RG, HalfFloat)
#else
_c(R16F, Luminance, HalfFloat)
_c(RG16F, LuminanceAlpha, HalfFloat)
#endif
_c(RGB16F, RGB, HalfFloat)
_c(RGBA16F, RGBA, HalfFloat)
#ifndef MAGNUM_TARGET_GLES2
_c(R32F, Red, Float)
_c(RG32F, RG, Float)
#else
_c(R32F, Luminance, Float)
_c(RG32F, LuminanceAlpha, Float)
#endif
_c(RGB32F, RGB, Float)
_c(RGBA32F, RGBA, Float)
#endif
