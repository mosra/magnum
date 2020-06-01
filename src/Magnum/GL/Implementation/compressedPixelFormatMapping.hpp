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

/* See Magnum/GL/PixelFormat.cpp and Magnum/GL/Test/PixelFormatTest.cpp */
#ifdef _c
_c(Bc1RGBUnorm, RGBS3tcDxt1)
_c(Bc1RGBSrgb, SRGBS3tcDxt1)
_c(Bc1RGBAUnorm, RGBAS3tcDxt1)
_c(Bc1RGBASrgb, SRGBAlphaS3tcDxt1)
_c(Bc2RGBAUnorm, RGBAS3tcDxt3)
_c(Bc2RGBASrgb, SRGBAlphaS3tcDxt3)
_c(Bc3RGBAUnorm, RGBAS3tcDxt5)
_c(Bc3RGBASrgb, SRGBAlphaS3tcDxt5)
#if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
_c(Bc4RUnorm, RedRgtc1)
_c(Bc4RSnorm, SignedRedRgtc1)
_c(Bc5RGUnorm, RGRgtc2)
_c(Bc5RGSnorm, SignedRGRgtc2)
_c(Bc6hRGBUfloat, RGBBptcUnsignedFloat)
_c(Bc6hRGBSfloat, RGBBptcSignedFloat)
_c(Bc7RGBAUnorm, RGBABptcUnorm)
_c(Bc7RGBASrgb, SRGBAlphaBptcUnorm)
#else
_s(Bc4RUnorm)
_s(Bc4RSnorm)
_s(Bc5RGUnorm)
_s(Bc5RGSnorm)
_s(Bc6hRGBUfloat)
_s(Bc6hRGBSfloat)
_s(Bc7RGBAUnorm)
_s(Bc7RGBASrgb)
#endif
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
_c(Astc4x4RGBAUnorm, RGBAAstc4x4)
_c(Astc4x4RGBASrgb, SRGB8Alpha8Astc4x4)
_c(Astc4x4RGBAF, RGBAAstc4x4)
_c(Astc5x4RGBAUnorm, RGBAAstc5x4)
_c(Astc5x4RGBASrgb, SRGB8Alpha8Astc5x4)
_c(Astc5x4RGBAF, RGBAAstc5x4)
_c(Astc5x5RGBAUnorm, RGBAAstc5x5)
_c(Astc5x5RGBASrgb, SRGB8Alpha8Astc5x5)
_c(Astc5x5RGBAF, RGBAAstc5x5)
_c(Astc6x5RGBAUnorm, RGBAAstc6x5)
_c(Astc6x5RGBASrgb, SRGB8Alpha8Astc6x5)
_c(Astc6x5RGBAF, RGBAAstc6x5)
_c(Astc6x6RGBAUnorm, RGBAAstc6x6)
_c(Astc6x6RGBASrgb, SRGB8Alpha8Astc6x6)
_c(Astc6x6RGBAF, RGBAAstc6x6)
_c(Astc8x5RGBAUnorm, RGBAAstc8x5)
_c(Astc8x5RGBASrgb, SRGB8Alpha8Astc8x5)
_c(Astc8x5RGBAF, RGBAAstc8x5)
_c(Astc8x6RGBAUnorm, RGBAAstc8x6)
_c(Astc8x6RGBASrgb, SRGB8Alpha8Astc8x6)
_c(Astc8x6RGBAF, RGBAAstc8x6)
_c(Astc8x8RGBAUnorm, RGBAAstc8x8)
_c(Astc8x8RGBASrgb, SRGB8Alpha8Astc8x8)
_c(Astc8x8RGBAF, RGBAAstc8x8)
_c(Astc10x5RGBAUnorm, RGBAAstc10x5)
_c(Astc10x5RGBASrgb, SRGB8Alpha8Astc10x5)
_c(Astc10x5RGBAF, RGBAAstc10x5)
_c(Astc10x6RGBAUnorm, RGBAAstc10x6)
_c(Astc10x6RGBASrgb, SRGB8Alpha8Astc10x6)
_c(Astc10x6RGBAF, RGBAAstc10x6)
_c(Astc10x8RGBAUnorm, RGBAAstc10x8)
_c(Astc10x8RGBASrgb, SRGB8Alpha8Astc10x8)
_c(Astc10x8RGBAF, RGBAAstc10x8)
_c(Astc10x10RGBAUnorm, RGBAAstc10x10)
_c(Astc10x10RGBASrgb, SRGB8Alpha8Astc10x10)
_c(Astc10x10RGBAF, RGBAAstc10x10)
_c(Astc12x10RGBAUnorm, RGBAAstc12x10)
_c(Astc12x10RGBASrgb, SRGB8Alpha8Astc12x10)
_c(Astc12x10RGBAF, RGBAAstc12x10)
_c(Astc12x12RGBAUnorm, RGBAAstc12x12)
_c(Astc12x12RGBASrgb, SRGB8Alpha8Astc12x12)
_c(Astc12x12RGBAF, RGBAAstc12x12)
#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
_c(Astc3x3x3RGBAUnorm, RGBAAstc3x3x3)
_c(Astc3x3x3RGBASrgb, SRGB8Alpha8Astc3x3x3)
_c(Astc3x3x3RGBAF, RGBAAstc3x3x3)
_c(Astc4x3x3RGBAUnorm, RGBAAstc4x3x3)
_c(Astc4x3x3RGBASrgb, SRGB8Alpha8Astc4x3x3)
_c(Astc4x3x3RGBAF, RGBAAstc4x3x3)
_c(Astc4x4x3RGBAUnorm, RGBAAstc4x4x3)
_c(Astc4x4x3RGBASrgb, SRGB8Alpha8Astc4x4x3)
_c(Astc4x4x3RGBAF, RGBAAstc4x4x3)
_c(Astc4x4x4RGBAUnorm, RGBAAstc4x4x4)
_c(Astc4x4x4RGBASrgb, SRGB8Alpha8Astc4x4x4)
_c(Astc4x4x4RGBAF, RGBAAstc4x4x4)
_c(Astc5x4x4RGBAUnorm, RGBAAstc5x4x4)
_c(Astc5x4x4RGBASrgb, SRGB8Alpha8Astc5x4x4)
_c(Astc5x4x4RGBAF, RGBAAstc5x4x4)
_c(Astc5x5x4RGBAUnorm, RGBAAstc5x5x4)
_c(Astc5x5x4RGBASrgb, SRGB8Alpha8Astc5x5x4)
_c(Astc5x5x4RGBAF, RGBAAstc5x5x4)
_c(Astc5x5x5RGBAUnorm, RGBAAstc5x5x5)
_c(Astc5x5x5RGBASrgb, SRGB8Alpha8Astc5x5x5)
_c(Astc5x5x5RGBAF, RGBAAstc5x5x5)
_c(Astc6x5x5RGBAUnorm, RGBAAstc6x5x5)
_c(Astc6x5x5RGBASrgb, SRGB8Alpha8Astc6x5x5)
_c(Astc6x5x5RGBAF, RGBAAstc6x5x5)
_c(Astc6x6x5RGBAUnorm, RGBAAstc6x6x5)
_c(Astc6x6x5RGBASrgb, SRGB8Alpha8Astc6x6x5)
_c(Astc6x6x5RGBAF, RGBAAstc6x6x5)
_c(Astc6x6x6RGBAUnorm, RGBAAstc6x6x6)
_c(Astc6x6x6RGBASrgb, SRGB8Alpha8Astc6x6x6)
_c(Astc6x6x6RGBAF, RGBAAstc6x6x6)
#else
_s(Astc3x3x3RGBAUnorm)
_s(Astc3x3x3RGBASrgb)
_s(Astc3x3x3RGBAF)
_s(Astc4x3x3RGBAUnorm)
_s(Astc4x3x3RGBASrgb)
_s(Astc4x3x3RGBAF)
_s(Astc4x4x3RGBAUnorm)
_s(Astc4x4x3RGBASrgb)
_s(Astc4x4x3RGBAF)
_s(Astc4x4x4RGBAUnorm)
_s(Astc4x4x4RGBASrgb)
_s(Astc4x4x4RGBAF)
_s(Astc5x4x4RGBAUnorm)
_s(Astc5x4x4RGBASrgb)
_s(Astc5x4x4RGBAF)
_s(Astc5x5x4RGBAUnorm)
_s(Astc5x5x4RGBASrgb)
_s(Astc5x5x4RGBAF)
_s(Astc5x5x5RGBAUnorm)
_s(Astc5x5x5RGBASrgb)
_s(Astc5x5x5RGBAF)
_s(Astc6x5x5RGBAUnorm)
_s(Astc6x5x5RGBASrgb)
_s(Astc6x5x5RGBAF)
_s(Astc6x6x5RGBAUnorm)
_s(Astc6x6x5RGBASrgb)
_s(Astc6x6x5RGBAF)
_s(Astc6x6x6RGBAUnorm)
_s(Astc6x6x6RGBASrgb)
_s(Astc6x6x6RGBAF)
#endif
#ifdef MAGNUM_TARGET_GLES
_c(PvrtcRGB2bppUnorm, RGBPvrtc2bppV1)
#ifndef MAGNUM_TARGET_WEBGL
_c(PvrtcRGB2bppSrgb, SRGBPvrtc2bppV1)
#else
_s(PvrtcRGB2bppSrgb)
#endif
_c(PvrtcRGBA2bppUnorm, RGBAPvrtc2bppV1)
#ifndef MAGNUM_TARGET_WEBGL
_c(PvrtcRGBA2bppSrgb, SRGBAlphaPvrtc2bppV1)
#else
_s(PvrtcRGBA2bppSrgb)
#endif
_c(PvrtcRGB4bppUnorm, RGBPvrtc4bppV1)
#ifndef MAGNUM_TARGET_WEBGL
_c(PvrtcRGB4bppSrgb, SRGBPvrtc4bppV1)
#else
_s(PvrtcRGB4bppSrgb)
#endif
_c(PvrtcRGBA4bppUnorm, RGBAPvrtc4bppV1)
#ifndef MAGNUM_TARGET_WEBGL
_c(PvrtcRGBA4bppSrgb, SRGBAlphaPvrtc4bppV1)
#else
_s(PvrtcRGBA4bppSrgb)
#endif
#else
_s(PvrtcRGB2bppUnorm)
_s(PvrtcRGB2bppSrgb)
_s(PvrtcRGBA2bppUnorm)
_s(PvrtcRGBA2bppSrgb)
_s(PvrtcRGB4bppUnorm)
_s(PvrtcRGB4bppSrgb)
_s(PvrtcRGBA4bppUnorm)
_s(PvrtcRGBA4bppSrgb)
#endif
#endif
