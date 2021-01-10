/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

/* See Magnum/Vk/Enums.cpp and Magnum/Vk/Test/EnumsTest.cpp */
#ifdef _c
#define _c2(input) _c(input, input)
_c2(Bc1RGBUnorm)
_c2(Bc1RGBSrgb)
_c2(Bc1RGBAUnorm)
_c2(Bc1RGBASrgb)
_c2(Bc2RGBAUnorm)
_c2(Bc2RGBASrgb)
_c2(Bc3RGBAUnorm)
_c2(Bc3RGBASrgb)
_c2(Bc4RUnorm)
_c2(Bc4RSnorm)
_c2(Bc5RGUnorm)
_c2(Bc5RGSnorm)
_c2(Bc6hRGBUfloat)
_c2(Bc6hRGBSfloat)
_c2(Bc7RGBAUnorm)
_c2(Bc7RGBASrgb)
_c2(EacR11Unorm)
_c2(EacR11Snorm)
_c2(EacRG11Unorm)
_c2(EacRG11Snorm)
_c2(Etc2RGB8Unorm)
_c2(Etc2RGB8Srgb)
_c2(Etc2RGB8A1Unorm)
_c2(Etc2RGB8A1Srgb)
_c2(Etc2RGBA8Unorm)
_c2(Etc2RGBA8Srgb)
_c2(Astc4x4RGBAUnorm)
_c2(Astc4x4RGBASrgb)
_c2(Astc4x4RGBAF)
_c2(Astc5x4RGBAUnorm)
_c2(Astc5x4RGBASrgb)
_c2(Astc5x4RGBAF)
_c2(Astc5x5RGBAUnorm)
_c2(Astc5x5RGBASrgb)
_c2(Astc5x5RGBAF)
_c2(Astc6x5RGBAUnorm)
_c2(Astc6x5RGBASrgb)
_c2(Astc6x5RGBAF)
_c2(Astc6x6RGBAUnorm)
_c2(Astc6x6RGBASrgb)
_c2(Astc6x6RGBAF)
_c2(Astc8x5RGBAUnorm)
_c2(Astc8x5RGBASrgb)
_c2(Astc8x5RGBAF)
_c2(Astc8x6RGBAUnorm)
_c2(Astc8x6RGBASrgb)
_c2(Astc8x6RGBAF)
_c2(Astc8x8RGBAUnorm)
_c2(Astc8x8RGBASrgb)
_c2(Astc8x8RGBAF)
_c2(Astc10x5RGBAUnorm)
_c2(Astc10x5RGBASrgb)
_c2(Astc10x5RGBAF)
_c2(Astc10x6RGBAUnorm)
_c2(Astc10x6RGBASrgb)
_c2(Astc10x6RGBAF)
_c2(Astc10x8RGBAUnorm)
_c2(Astc10x8RGBASrgb)
_c2(Astc10x8RGBAF)
_c2(Astc10x10RGBAUnorm)
_c2(Astc10x10RGBASrgb)
_c2(Astc10x10RGBAF)
_c2(Astc12x10RGBAUnorm)
_c2(Astc12x10RGBASrgb)
_c2(Astc12x10RGBAF)
_c2(Astc12x12RGBAUnorm)
_c2(Astc12x12RGBASrgb)
_c2(Astc12x12RGBAF)

/* https://github.com/KhronosGroup/KTX-Specification/pull/97 */
_s(Astc3x3x3RGBAUnorm) //, ASTC_3x3x3_UNORM_BLOCK)
_s(Astc3x3x3RGBASrgb) //, ASTC_3x3x3_SRGB_BLOCK)
_s(Astc3x3x3RGBAF) //, ASTC_3x3x3_SFLOAT_BLOCK_EXT)
_s(Astc4x3x3RGBAUnorm) //, ASTC_4x3x3_UNORM_BLOCK)
_s(Astc4x3x3RGBASrgb) //, ASTC_4x3x3_SRGB_BLOCK)
_s(Astc4x3x3RGBAF) //, ASTC_4x3x3_SFLOAT_BLOCK_EXT)
_s(Astc4x4x3RGBAUnorm) //, ASTC_4x4x3_UNORM_BLOCK)
_s(Astc4x4x3RGBASrgb) //, ASTC_4x4x3_SRGB_BLOCK)
_s(Astc4x4x3RGBAF) //, ASTC_4x4x3_SFLOAT_BLOCK_EXT)
_s(Astc4x4x4RGBAUnorm) //, ASTC_4x4x4_UNORM_BLOCK)
_s(Astc4x4x4RGBASrgb) //, ASTC_4x4x4_SRGB_BLOCK)
_s(Astc4x4x4RGBAF) //, ASTC_4x4x4_SFLOAT_BLOCK_EXT)
_s(Astc5x4x4RGBAUnorm) //, ASTC_5x4x4_UNORM_BLOCK)
_s(Astc5x4x4RGBASrgb) //, ASTC_5x4x4_SRGB_BLOCK)
_s(Astc5x4x4RGBAF) //, ASTC_5x4x4_SFLOAT_BLOCK_EXT)
_s(Astc5x5x4RGBAUnorm) //, ASTC_5x5x4_UNORM_BLOCK)
_s(Astc5x5x4RGBASrgb) //, ASTC_5x5x4_SRGB_BLOCK)
_s(Astc5x5x4RGBAF) //, ASTC_5x5x4_SFLOAT_BLOCK_EXT)
_s(Astc5x5x5RGBAUnorm) //, ASTC_5x5x5_UNORM_BLOCK)
_s(Astc5x5x5RGBASrgb) //, ASTC_5x5x5_SRGB_BLOCK)
_s(Astc5x5x5RGBAF) //, ASTC_5x5x5_SFLOAT_BLOCK_EXT)
_s(Astc6x5x5RGBAUnorm) //, ASTC_6x5x5_UNORM_BLOCK)
_s(Astc6x5x5RGBASrgb) //, ASTC_6x5x5_SRGB_BLOCK)
_s(Astc6x5x5RGBAF) //, ASTC_6x5x5_SFLOAT_BLOCK_EXT)
_s(Astc6x6x5RGBAUnorm) //, ASTC_6x6x5_UNORM_BLOCK)
_s(Astc6x6x5RGBASrgb) //, ASTC_6x6x5_SRGB_BLOCK)
_s(Astc6x6x5RGBAF) //, ASTC_6x6x5_SFLOAT_BLOCK_EXT)
_s(Astc6x6x6RGBAUnorm) //, ASTC_6x6x6_UNORM_BLOCK)
_s(Astc6x6x6RGBASrgb) //, ASTC_6x6x6_SRGB_BLOCK)
_s(Astc6x6x6RGBAF) //, ASTC_6x6x6_SFLOAT_BLOCK_EXT)

/* https://github.com/KhronosGroup/Vulkan-Docs/issues/512 */
_c(PvrtcRGB2bppUnorm, PvrtcRGBA2bppUnorm)
_c(PvrtcRGB2bppSrgb, PvrtcRGBA2bppSrgb)
_c2(PvrtcRGBA2bppUnorm)
_c2(PvrtcRGBA2bppSrgb)
_c(PvrtcRGB4bppUnorm, PvrtcRGBA4bppUnorm)
_c(PvrtcRGB4bppSrgb, PvrtcRGBA4bppSrgb)
_c2(PvrtcRGBA4bppUnorm)
_c2(PvrtcRGBA4bppSrgb)
#undef _c2
#endif
