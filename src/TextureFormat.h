#ifndef Magnum_TextureFormat_h
#define Magnum_TextureFormat_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

/** @file
 * @brief Enum Magnum::TextureFormat
 */

#include "OpenGL.h"

namespace Magnum {

/**
@brief Internal texture format

@see Texture, CubeMapTexture, CubeMapTextureArray
*/
enum class TextureFormat: GLenum {
    /**
     * Red component, normalized unsigned, size implementation-dependent.
     * @deprecated Prefer to use the exactly specified version of this format,
     *      e.g. @ref Magnum::TextureFormat "TextureFormat::R8".
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gles30 %Extension @es_extension{EXT,texture_rg}
     */
    #ifndef MAGNUM_TARGET_GLES2
    Red = GL_RED,
    #else
    Red = GL_RED_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red component, normalized unsigned byte.
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gles30 Use @ref Magnum::TextureFormat "TextureFormat::Red" in
     *      OpenGL ES 2.0 instead.
     */
    R8 = GL_R8,
    #endif

    /**
     * Red and green component, normalized unsigned, size
     * implementation-dependent.
     * @deprecated Prefer to use the exactly specified version of this format,
     *      e.g. @ref Magnum::TextureFormat "TextureFormat::RG8".
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gles30 %Extension @es_extension{EXT,texture_rg}
     */
    #ifndef MAGNUM_TARGET_GLES2
    RG = GL_RG,
    #else
    RG = GL_RG_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red and green component, each normalized unsigned byte.
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gles30 Use @ref Magnum::TextureFormat "TextureFormat::RG" in
     *      OpenGL ES 2.0 instead.
     */
    RG8 = GL_RG8,
    #endif

    /**
     * RGB, normalized unsigned, size implementation-dependent.
     * @deprecated Prefer to use the exactly specified version of this format,
     *      e.g. @ref Magnum::TextureFormat "TextureFormat::RGB8".
     */
    RGB = GL_RGB,

    /**
     * RGB, each component normalized unsigned byte.
     * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
     */
    #ifndef MAGNUM_TARGET_GLES2
    RGB8 = GL_RGB8,
    #else
    RGB8 = GL_RGB8_OES,
    #endif

    /**
     * RGBA, normalized unsigned, size implementation-dependent.
     * @deprecated Prefer to use the exactly specified version of this format,
     *      e.g. @ref Magnum::TextureFormat "TextureFormat::RGBA8".
     */
    RGBA = GL_RGBA,

    /**
     * RGBA, each component normalized unsigned byte.
     * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
     */
    #ifndef MAGNUM_TARGET_GLES2
    RGBA8 = GL_RGBA8,
    #else
    RGBA8 = GL_RGBA8_OES,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red component, normalized signed byte.
     * @requires_gl31 %Extension @extension{EXT,texture_snorm}
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     */
    R8Snorm = GL_R8_SNORM,

    /**
     * Red and green component, each normalized signed byte.
     * @requires_gl31 %Extension @extension{EXT,texture_snorm}
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     */
    RG8Snorm = GL_RG8_SNORM,

    /**
     * RGB, each component normalized signed byte.
     * @requires_gl31 %Extension @extension{EXT,texture_snorm}
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     */
    RGB8Snorm = GL_RGB8_SNORM,

    /**
     * RGBA, each component normalized signed byte.
     * @requires_gl31 %Extension @extension{EXT,texture_snorm}
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     */
    RGBA8Snorm = GL_RGBA8_SNORM,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, normalized unsigned short.
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    R16 = GL_R16,

    /**
     * Red and green component, each normalized unsigned short.
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    RG16 = GL_RG16,

    /**
     * RGB, each component normalized unsigned short.
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    RGB16 = GL_RGB16,

    /**
     * RGBA, each component normalized unsigned short.
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    RGBA16 = GL_RGBA16,

    /**
     * Red component, normalized signed short.
     * @requires_gl31 %Extension @extension{EXT,texture_snorm}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    R16Snorm = GL_R16_SNORM,

    /**
     * Red and green component, each normalized signed short.
     * @requires_gl31 %Extension @extension{EXT,texture_snorm}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    RG16Snorm = GL_RG16_SNORM,

    /**
     * RGB, each component normalized signed short.
     * @requires_gl31 %Extension @extension{EXT,texture_snorm}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    RGB16Snorm = GL_RGB16_SNORM,

    /**
     * RGBA, each component normalized signed short.
     * @requires_gl31 %Extension @extension{EXT,texture_snorm}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    RGBA16Snorm = GL_RGBA16_SNORM,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red component, non-normalized unsigned byte.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R8UI = GL_R8UI,

    /**
     * Red and green component, each non-normalized unsigned byte.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG8UI = GL_RG8UI,

    /**
     * RGB, each component non-normalized unsigned byte.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGB8UI = GL_RGB8UI,

    /**
     * RGBA, each component non-normalized unsigned byte.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA8UI = GL_RGBA8UI,

    /**
     * Red component, non-normalized signed byte.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R8I = GL_R8I,

    /**
     * Red and green component, each non-normalized signed byte.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG8I = GL_RG8I,

    /**
     * RGB, each component non-normalized signed byte.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGB8I = GL_RGB8I,

    /**
     * RGBA, each component non-normalized signed byte.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA8I = GL_RGBA8I,

    /**
     * Red component, non-normalized unsigned short.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R16UI = GL_R16UI,

    /**
     * Red and green component, each non-normalized unsigned short.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG16UI = GL_RG16UI,

    /**
     * RGB, each component non-normalized unsigned short.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGB16UI = GL_RGB16UI,

    /**
     * RGBA, each component non-normalized unsigned short.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA16UI = GL_RGBA16UI,

    /**
     * Red component, non-normalized signed short.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R16I = GL_R16I,

    /**
     * Red and green component, each non-normalized signed short.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG16I = GL_RG16I,

    /**
     * RGB, each component non-normalized signed short.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGB16I = GL_RGB16I,

    /**
     * RGBA, each component non-normalized signed short.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA16I = GL_RGBA16I,

    /**
     * Red component, non-normalized unsigned int.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R32UI = GL_R32UI,

    /**
     * Red and green component, each non-normalized unsigned int.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG32UI = GL_RG32UI,

    /**
     * RGB, each component non-normalized unsigned int.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGB32UI = GL_RGB32UI,

    /**
     * RGBA, each component non-normalized unsigned int.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA32UI = GL_RGBA32UI,

    /**
     * Red component, non-normalized signed int.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R32I = GL_R32I,

    /**
     * Red and green component, each non-normalized signed int.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG32I = GL_RG32I,

    /**
     * RGB, each component non-normalized signed int.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGB32I = GL_RGB32I,

    /**
     * RGBA, each component non-normalized signed int.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA32I = GL_RGBA32I,

    /**
     * Red component, half float.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R16F = GL_R16F,

    /**
     * Red and green component, each half float.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG16F = GL_RG16F,

    /**
     * RGB, each component half float.
     * @requires_gl30 %Extension @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGB16F = GL_RGB16F,

    /**
     * RGBA, each component half float.
     * @requires_gl30 %Extension @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA16F = GL_RGBA16F,

    /**
     * Red component, float.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R32F = GL_R32F,

    /**
     * Red and green component, each float.
     * @requires_gl30 %Extension @extension{ARB,texture_rg} and @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG32F = GL_RG32F,

    /**
     * RGB, each component float.
     * @requires_gl30 %Extension @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGB32F = GL_RGB32F,

    /**
     * RGBA, each component float.
     * @requires_gl30 %Extension @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA32F = GL_RGBA32F,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGB, normalized unsigned, red and green component 3bit, blue 2bit.
     * @requires_gl Packed 8bit types are not available in OpenGL ES.
     */
    R3B3G2 = GL_R3_G3_B2,
    #endif

    #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * Luminance, normalized unsigned, single value used for all RGB channels.
     * Size implementation-dependent.
     * @deprecated Included for compatibility reasons only, use
     *      @ref Magnum::TextureFormat "TextureFormat::R8" instead.
     * @requires_gles20 Not available in ES 3.0 or desktop OpenGL. Use
     *      @ref Magnum::TextureFormat "TextureFormat::R8" instead.
     */
    Luminance = GL_LUMINANCE,

    /**
     * Floating-point luminance and alpha channel. First value is used for all
     * RGB channels, second value is used for alpha channel. Size
     * implementation-dependent.
     * @deprecated Included for compatibility reasons only, use
     *      @ref Magnum::TextureFormat "TextureFormat::RG8" instead.
     * @requires_gles20 Not available in ES 3.0 or desktop OpenGL. Use
     *      @ref Magnum::TextureFormat "TextureFormat::RG8" instead.
     */
    LuminanceAlpha = GL_LUMINANCE_ALPHA,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGB, each component normalized unsigned 4bit.
     * @requires_gl Packed 12bit types are not available in OpenGL ES.
     */
    RGB4 = GL_RGB4,

    /**
     * RGB, each component normalized unsigned 5bit.
     * @requires_gl Use @ref Magnum::TextureFormat "TextureFormat::RGB5A1" or
     *      @ref Magnum::TextureFormat "TextureFormat::RGB565" in OpenGL ES.
     */
    RGB5 = GL_RGB5,
    #endif

    /**
     * RGB, normalized unsigned, red and blue component 5bit, green 6bit.
     * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
     */
    /* 1.5.6 <= GLEW < 1.8.0 doesn't have this, even if there is
       GL_ARB_ES2_compatibility */
    #ifdef GL_RGB565
    RGB565 = GL_RGB565,
    #else
    RGB565 = 0x8D62,
    #endif

    #ifndef MAGNUM_TARGET_GLES3
    /**
     * RGB, each component normalized unsigned 10bit.
     * @requires_es_extension %Extension @es_extension{OES,required_internalformat}
     *      and @es_extension{EXT,texture_type_2_10_10_10_REV}
     */
    #ifndef MAGNUM_TARGET_GLES
    RGB10 = GL_RGB10,
    #else
    RGB10 = GL_RGB10_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGB, each component normalized unsigned 12bit.
     * @requires_gl Packed 36bit types are not available in OpenGL ES.
     */
    RGB12 = GL_RGB12,

    /**
     * RGBA, normalized unsigned, each component 2bit.
     * @requires_gl Packed 8bit types are not available in OpenGL ES.
     */
    RGBA2 = GL_RGBA2,
    #endif

    /**
     * RGBA, normalized unsigned, each component 4bit.
     * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
     */
    RGBA4 = GL_RGBA4,

    /**
     * RGBA, normalized unsigned, each RGB component 5bit, alpha 1bit.
     * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
     */
    RGB5A1 = GL_RGB5_A1,

    /**
     * RGBA, normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
     *      and @es_extension{EXT,texture_type_2_10_10_10_REV}
     */
    #ifndef MAGNUM_TARGET_GLES2
    RGB10A2 = GL_RGB10_A2,
    #else
    RGB10A2 = GL_RGB10_A2_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * RGBA, non-normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gl33 %Extension @extension{ARB,texture_rgb10_a2ui}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGB10A2UI = GL_RGB10_A2UI,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGBA, each component normalized unsigned 12bit.
     * @requires_gl Packed 48bit types are not available in OpenGL ES.
     */
    RGBA12 = GL_RGBA12,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * RGB, float, red and green component 11bit, blue 10bit.
     * @requires_gl30 %Extension @extension{EXT,packed_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R11FG11FB10F = GL_R11F_G11F_B10F,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * RGB, unsigned with exponent, each RGB component 9bit, exponent 5bit.
     * @requires_gl30 %Extension @extension{EXT,texture_shared_exponent}
     * @requires_gles30 Use @ref Magnum::TextureFormat "TextureFormat::RGB" in
     *      OpenGL ES 2.0 instead.
     */
    RGB9E5 = GL_RGB9_E5,
    #endif

    #ifndef MAGNUM_TARGET_GLES3
    /**
     * sRGB, normalized unsigned, size implementation-dependent.
     * @todo is this allowed in core?
     * @deprecated Prefer to use the exactly specified version of this format,
     *      i.e. @ref Magnum::TextureFormat "TextureFormat::SRGB8".
     * @requires_es_extension %Extension @es_extension{EXT,sRGB} in OpenGL ES
     *      2.0, use @ref Magnum::TextureFormat "TextureFormat::SRGB8" in
     *      OpenGL ES 3.0 instead.
     */
    #ifndef MAGNUM_TARGET_GLES
    SRGB = GL_SRGB,
    #else
    SRGB = GL_SRGB_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * sRGB, each component normalized unsigned byte.
     * @requires_gles30 Use @ref Magnum::TextureFormat "TextureFormat::SRGB" in
     *      OpenGL ES 2.0 instead.
     */
    SRGB8 = GL_SRGB8,
    #endif

    #ifndef MAGNUM_TARGET_GLES3
    /**
     * sRGBA, normalized unsigned, size implementation-dependent.
     * @todo is this allowed in core?
     * @deprecated Prefer to use the exactly specified version of this format,
     *      i.e. @ref Magnum::TextureFormat "TextureFormat::SRGB8Alpha8".
     * @requires_es_extension %Extension @es_extension{EXT,sRGB} in OpenGL ES
     *      2.0, use @ref Magnum::TextureFormat "TextureFormat::SRGB8Alpha8" in
     *      OpenGL ES 3.0 instead.
     */
    #ifndef MAGNUM_TARGET_GLES
    SRGBAlpha = GL_SRGB_ALPHA,
    #else
    SRGBAlpha = GL_SRGB_ALPHA_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * sRGBA, each component normalized unsigned byte.
     * @requires_gles30 Use @ref Magnum::TextureFormat "TextureFormat::SRGBAlpha"
     *      in OpenGL ES 2.0 instead.
     */
    SRGB8Alpha8 = GL_SRGB8_ALPHA8,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Compressed red channel, normalized unsigned.
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gl Generic texture compression is not available in OpenGL ES.
     */
    CompressedRed = GL_COMPRESSED_RED,

    /**
     * Compressed red and green channel, normalized unsigned.
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gl Generic texture compression is not available in OpenGL ES.
     */
    CompressedRG = GL_COMPRESSED_RG,

    /**
     * Compressed RGB, normalized unsigned.
     * @requires_gl Generic texture compression is not available in OpenGL ES.
     */
    CompressedRGB = GL_COMPRESSED_RGB,

    /**
     * Compressed RGBA, normalized unsigned.
     * @requires_gl Generic texture compression is not available in OpenGL ES.
     */
    CompressedRGBA = GL_COMPRESSED_RGBA,

    /**
     * RTGC compressed red channel, normalized unsigned.
     * @requires_gl30 %Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES.
     */
    CompressedRedRtgc1 = GL_COMPRESSED_RED_RGTC1,

    /**
     * RTGC compressed red and green channel, normalized unsigned.
     * @requires_gl30 %Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES.
     */
    CompressedRGRgtc2 = GL_COMPRESSED_RG_RGTC2,

    /**
     * RTGC compressed red channel, normalized signed.
     * @requires_gl30 %Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES.
     */
    CompressedSignedRedRgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,

    /**
     * RTGC compressed red and green channel, normalized signed.
     * @requires_gl30 %Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES.
     */
    CompressedSignedRGRgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,

    /* These are named with _ARB suffix, because glcorearb.h doesn't
       have suffixless version (?!) and GLEW has it without suffix as
       late as of 1.8.0 { */

    /**
     * BPTC compressed RGB, unsigned float.
     * @requires_gl42 %Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES.
     */
    CompressedRGBBptcUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,

    /**
     * BPTC compressed RGB, signed float.
     * @requires_gl42 %Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES.
     */
    CompressedRGBBptcSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,

    /**
     * BPTC compressed RGBA, normalized unsigned.
     * @requires_gl42 %Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES.
     */
    CompressedRGBABptcUnorm = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,

    /**
     * BPTC compressed sRGBA, normalized unsigned.
     * @requires_gl42 %Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES.
     */
    CompressedSRGBAlphaBptcUnorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,

    /*}*/
    #endif

    /**
     * Depth component, size implementation-dependent.
     * @deprecated Prefer to use exactly specified version of this format, e.g.
     *      @ref Magnum::TextureFormat "TextureFormat::DepthComponent16".
     * @requires_gles30 %Extension @es_extension{OES,depth_texture} or
     *      @es_extension{ANGLE,depth_texture}
     */
    DepthComponent = GL_DEPTH_COMPONENT,

    /**
     * Depth and stencil component, size implementation-dependent.
     * @deprecated Prefer to use exactly specified version of this format, e.g.
     *      @ref Magnum::TextureFormat "TextureFormat::Depth24Stencil8".
     * @requires_gles30 %Extension @es_extension{OES,packed_depth_stencil}
     */
    #ifndef MAGNUM_TARGET_GLES2
    DepthStencil = GL_DEPTH_STENCIL,
    #else
    DepthStencil = GL_DEPTH_STENCIL_OES,
    #endif

    /**
     * Depth component, 16bit.
     * @requires_gles30 %Extension (@es_extension{OES,required_internalformat}
     *      and @es_extension{OES,depth_texture}) or (@es_extension{EXT,texture_storage}
     *      and @es_extension{ANGLE,depth_texture})
     */
    DepthComponent16 = GL_DEPTH_COMPONENT16,

    /**
     * Depth component, 24bit.
     * @requires_gles30 %Extension @es_extension{OES,required_internalformat},
     *      @es_extension{OES,depth_texture} and @es_extension{OES,depth24}
     */
    #ifndef MAGNUM_TARGET_GLES2
    DepthComponent24 = GL_DEPTH_COMPONENT24,
    #else
    DepthComponent24 = GL_DEPTH_COMPONENT24_OES,
    #endif

    #ifndef MAGNUM_TARGET_GLES3
    /**
     * Depth component, 32bit.
     * @requires_es_extension %Extension (@es_extension{OES,required_internalformat},
     *      @es_extension{OES,depth_texture} and @es_extension{OES,depth32}) or
     *      (@es_extension{EXT,texture_storage} and @es_extension{ANGLE,depth_texture})
     */
    #ifndef MAGNUM_TARGET_GLES2
    DepthComponent32 = GL_DEPTH_COMPONENT32,
    #else
    DepthComponent32 = GL_DEPTH_COMPONENT32_OES,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Depth component, 32bit float.
     * @requires_gl30 %Extension @extension{ARB,depth_buffer_float}
     * @requires_gles30 Only integral depth textures are available in OpenGL ES
     *      2.0.
     */
    DepthComponent32F = GL_DEPTH_COMPONENT32F,
    #endif

    /**
     * 24bit depth and 8bit stencil component.
     * @requires_gl30 %Extension @extension{EXT,packed_depth_stencil}
     * @requires_gles30 %Extension @es_extension{OES,packed_depth_stencil} and
     *      (@es_extension{OES,required_internalformat} or
     *      (@es_extension{EXT,texture_storage} and @es_extension{ANGLE,depth_texture}))
     */
    #ifdef MAGNUM_TARGET_GLES2
    Depth24Stencil8 = GL_DEPTH24_STENCIL8_OES
    #else
    Depth24Stencil8 = GL_DEPTH24_STENCIL8,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * 32bit float depth component and 8bit stencil component.
     * @requires_gl30 %Extension @extension{ARB,depth_buffer_float}
     * @requires_gles30 Only integral depth textures are available in OpenGL ES
     *      2.0.
     */
    Depth32FStencil8 = GL_DEPTH32F_STENCIL8
    #endif
};

}

#endif
