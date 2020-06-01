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

/* each entry is width, height, depth, size in bytes */
#ifdef _c
/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_bc1_with_no_alpha */
_c(Bc1RGBUnorm, 4, 4, 1, 64)
_c(Bc1RGBSrgb, 4, 4, 1, 64)
/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_bc1_with_alpha */
_c(Bc1RGBAUnorm, 4, 4, 1, 64)
_c(Bc1RGBASrgb, 4, 4, 1, 64)
/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_bc2 */
_c(Bc2RGBAUnorm, 4, 4, 1, 128)
_c(Bc2RGBASrgb, 4, 4, 1, 128)
/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_bc3 */
_c(Bc3RGBAUnorm, 4, 4, 1, 128)
_c(Bc3RGBASrgb, 4, 4, 1, 128)

/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_bc4_unsigned */
_c(Bc4RUnorm, 4, 4, 1, 64)
_c(Bc4RSnorm, 4, 4, 1, 64)
/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_bc5_unsigned */
_c(Bc5RGUnorm, 4, 4, 1, 128)
_c(Bc5RGSnorm, 4, 4, 1, 128)

/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_bc6h */
_c(Bc6hRGBUfloat, 4, 4, 1, 128)
_c(Bc6hRGBSfloat, 4, 4, 1, 128)

/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_bc7 */
_c(Bc7RGBAUnorm, 4, 4, 1, 128)
_c(Bc7RGBASrgb, 4, 4, 1, 128)

/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#Section-r11eac */
_c(EacR11Unorm, 4, 4, 1, 64)
_c(EacR11Snorm, 4, 4, 1, 64)

/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_format_unsigned_rg11_eac */
_c(EacRG11Unorm, 4, 4, 1, 128)
_c(EacRG11Snorm, 4, 4, 1, 128)

/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#RGBETC2 */
_c(Etc2RGB8Unorm, 4, 4, 1, 64)
_c(Etc2RGB8Srgb, 4, 4, 1, 64)

/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_format_rgb_etc2_with_punchthrough_alpha */
_c(Etc2RGB8A1Unorm, 4, 4, 1, 64)
_c(Etc2RGB8A1Srgb, 4, 4, 1, 64)

/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_format_rgba_etc2 */
_c(Etc2RGBA8Unorm, 4, 4, 1, 128)
_c(Etc2RGBA8Srgb, 4, 4, 1, 128)

/* https://www.khronos.org/registry/DataFormat/specs/1.1/dataformat.1.1.html#_basic_concepts,
   all blocks fit into 128 bits */
_c(Astc4x4RGBAUnorm, 4, 4, 1, 128)
_c(Astc4x4RGBASrgb, 4, 4, 1, 128)
_c(Astc4x4RGBAF, 4, 4, 1, 128)
_c(Astc5x4RGBAUnorm, 5, 4, 1, 128)
_c(Astc5x4RGBASrgb, 5, 4, 1, 128)
_c(Astc5x4RGBAF, 5, 4, 1, 128)
_c(Astc5x5RGBAUnorm, 5, 5, 1, 128)
_c(Astc5x5RGBASrgb, 5, 5, 1, 128)
_c(Astc5x5RGBAF, 5, 5, 1, 128)
_c(Astc6x5RGBAUnorm, 6, 5, 1, 128)
_c(Astc6x5RGBASrgb, 6, 5, 1, 128)
_c(Astc6x5RGBAF, 6, 5, 1, 128)
_c(Astc6x6RGBAUnorm, 6, 6, 1, 128)
_c(Astc6x6RGBASrgb, 6, 6, 1, 128)
_c(Astc6x6RGBAF, 6, 6, 1, 128)
_c(Astc8x5RGBAUnorm, 8, 5, 1, 128)
_c(Astc8x5RGBASrgb, 8, 5, 1, 128)
_c(Astc8x5RGBAF, 8, 5, 1, 128)
_c(Astc8x6RGBAUnorm, 8, 6, 1, 128)
_c(Astc8x6RGBASrgb, 8, 6, 1, 128)
_c(Astc8x6RGBAF, 8, 6, 1, 128)
_c(Astc8x8RGBAUnorm, 8, 8, 1, 128)
_c(Astc8x8RGBASrgb, 8, 8, 1, 128)
_c(Astc8x8RGBAF, 8, 8, 1, 128)
_c(Astc10x5RGBAUnorm, 10, 5, 1, 128)
_c(Astc10x5RGBASrgb, 10, 5, 1, 128)
_c(Astc10x5RGBAF, 10, 5, 1, 128)
_c(Astc10x6RGBAUnorm, 10, 6, 1, 128)
_c(Astc10x6RGBASrgb, 10, 6, 1, 128)
_c(Astc10x6RGBAF, 10, 6, 1, 128)
_c(Astc10x8RGBAUnorm, 10, 8, 1, 128)
_c(Astc10x8RGBASrgb, 10, 8, 1, 128)
_c(Astc10x8RGBAF, 10, 8, 1, 128)
_c(Astc10x10RGBAUnorm, 10, 10, 1, 128)
_c(Astc10x10RGBASrgb, 10, 10, 1, 128)
_c(Astc10x10RGBAF, 10, 10, 1, 128)
_c(Astc12x10RGBAUnorm, 12, 10, 1, 128)
_c(Astc12x10RGBASrgb, 12, 10, 1, 128)
_c(Astc12x10RGBAF, 12, 10, 1, 128)
_c(Astc12x12RGBAUnorm, 12, 12, 1, 128)
_c(Astc12x12RGBASrgb, 12, 12, 1, 128)
_c(Astc12x12RGBAF, 12, 12, 1, 128)
_c(Astc3x3x3RGBAUnorm, 3, 3, 3, 128)
_c(Astc3x3x3RGBASrgb, 3, 3, 3, 128)
_c(Astc3x3x3RGBAF, 3, 3, 3, 128)
_c(Astc4x3x3RGBAUnorm, 4, 3, 3, 128)
_c(Astc4x3x3RGBASrgb, 4, 3, 3, 128)
_c(Astc4x3x3RGBAF, 4, 3, 3, 128)
_c(Astc4x4x3RGBAUnorm, 4, 4, 3, 128)
_c(Astc4x4x3RGBASrgb, 4, 4, 3, 128)
_c(Astc4x4x3RGBAF, 4, 4, 3, 128)
_c(Astc4x4x4RGBAUnorm, 4, 4, 4, 128)
_c(Astc4x4x4RGBASrgb, 4, 4, 4, 128)
_c(Astc4x4x4RGBAF, 4, 4, 4, 128)
_c(Astc5x4x4RGBAUnorm, 5, 4, 4, 128)
_c(Astc5x4x4RGBASrgb, 5, 4, 4, 128)
_c(Astc5x4x4RGBAF, 5, 4, 4, 128)
_c(Astc5x5x4RGBAUnorm, 5, 5, 4, 128)
_c(Astc5x5x4RGBASrgb, 5, 5, 4, 128)
_c(Astc5x5x4RGBAF, 5, 5, 4, 128)
_c(Astc5x5x5RGBAUnorm, 5, 5, 5, 128)
_c(Astc5x5x5RGBASrgb, 5, 5, 5, 128)
_c(Astc5x5x5RGBAF, 5, 5, 5, 128)
_c(Astc6x5x5RGBAUnorm, 6, 5, 5, 128)
_c(Astc6x5x5RGBASrgb, 6, 5, 5, 128)
_c(Astc6x5x5RGBAF, 6, 5, 5, 128)
_c(Astc6x6x5RGBAUnorm, 6, 6, 5, 128)
_c(Astc6x6x5RGBASrgb, 6, 6, 5, 128)
_c(Astc6x6x5RGBAF, 6, 6, 5, 128)
_c(Astc6x6x6RGBAUnorm, 6, 6, 6, 128)
_c(Astc6x6x6RGBASrgb, 6, 6, 6, 128)
_c(Astc6x6x6RGBAF, 6, 6, 6, 128)

/* https://en.wikipedia.org/wiki/PVRTC */
_c(PvrtcRGB2bppUnorm, 8, 4, 1, 64)
_c(PvrtcRGB2bppSrgb, 8, 4, 1, 64)
_c(PvrtcRGBA2bppUnorm, 8, 4, 1, 64)
_c(PvrtcRGBA2bppSrgb, 8, 4, 1, 64)
_c(PvrtcRGB4bppUnorm, 4, 4, 1, 64)
_c(PvrtcRGB4bppSrgb, 4, 4, 1, 64)
_c(PvrtcRGBA4bppUnorm, 4, 4, 1, 64)
_c(PvrtcRGBA4bppSrgb, 4, 4, 1, 64)
#endif
