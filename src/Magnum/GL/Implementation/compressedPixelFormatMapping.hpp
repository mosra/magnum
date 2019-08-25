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
_c(Bc1RGBUnorm, RGBS3tcDxt1)
_c(Bc1RGBAUnorm, RGBAS3tcDxt1)
_c(Bc2RGBAUnorm, RGBAS3tcDxt3)
_c(Bc3RGBAUnorm, RGBAS3tcDxt5)
#ifndef MAGNUM_TARGET_GLES2
_c(EacR11Unorm, R11Eac)
_c(EacR11Snorm, SignedR11Eac)
_c(EacRG11Unorm, RG11Eac)
_c(EacRG11Snorm, SignedRG11Eac)
_c(Etc2RGB8Unorm, RGB8Etc2)
_c(Etc2RGB8Srgb, SRGB8Etc2)
_c(Etc2RGB8A1Unorm, RGB8PunchthroughAlpha1Etc2)
_c(Etc2RGB8A1Srgb, SRGB8PunchthroughAlpha1Etc2)
_c(Etc2RGBA8Unorm, RGBA8Etc2Eac)
_c(Etc2RGBA8Srgb, SRGB8Alpha8Etc2Eac)
#else
_s(EacR11Unorm)
_s(EacR11Snorm)
_s(EacRG11Unorm)
_s(EacRG11Snorm)
_s(Etc2RGB8Unorm)
_s(Etc2RGB8Srgb)
_s(Etc2RGB8A1Unorm)
_s(Etc2RGB8A1Srgb)
_s(Etc2RGBA8Unorm)
_s(Etc2RGBA8Srgb)
#endif
#endif
