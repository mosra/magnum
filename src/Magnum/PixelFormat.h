#ifndef Magnum_PixelFormat_h
#define Magnum_PixelFormat_h
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

/** @file
 * @brief Enum @ref Magnum::PixelFormat, @ref Magnum::PixelType, @ref Magnum::CompressedPixelFormat
 */

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief Format of pixel data

Note that some formats can be used only for framebuffer reading (using
@ref AbstractFramebuffer::read()) and some only for texture data (using
@ref Texture::setSubImage() and others), the limitations are mentioned in
documentation of each particular value.

In most cases you may want to use @ref PixelFormat::Red (for grayscale images),
@ref PixelFormat::RGB or @ref PixelFormat::RGBA along with
@ref PixelType::UnsignedByte, the matching texture format is then
@ref TextureFormat::R8, @ref TextureFormat::RGB8 or @ref TextureFormat::RGBA8.
See documentation of these values for possible limitations when using OpenGL ES
2.0 or WebGL.

@see @ref Image, @ref ImageView, @ref BufferImage, @ref Trade::ImageData
*/
enum class PixelFormat: GLenum {
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Floating-point red channel.
     * @requires_gles30 For texture data only, extension
     *      @extension{EXT,texture_rg} in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading, extension
     *      @extension{EXT,texture_rg}
     * @requires_webgl20 For texture data only. Not available in WebGL 1.0, see
     *      @ref PixelFormat::Luminance for an alternative.
     */
    #ifndef MAGNUM_TARGET_GLES2
    Red = GL_RED,
    #else
    Red = GL_RED_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Floating-point green channel.
     * @requires_gl Only @ref PixelFormat::Red is available in OpenGL ES or
     *      WebGL.
     */
    Green = GL_GREEN,

    /**
     * Floating-point blue channel.
     * @requires_gl Only @ref PixelFormat::Red is available in OpenGL ES or
     *      WebGL.
     */
    Blue = GL_BLUE,
    #endif

    #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * Floating-point luminance channel. The value is used for all RGB
     * channels.
     * @requires_gles20 Not available in ES 3.0, WebGL 2.0 or desktop OpenGL.
     *      Use @ref PixelFormat::Red instead.
     * @deprecated_gl Included for compatibility reasons only, use
     *      @ref PixelFormat::Red instead.
     */
    Luminance = GL_LUMINANCE,
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Floating-point red and green channel.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 For texture data only, extension
     *      @extension{EXT,texture_rg} in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading, extension
     *      @extension{EXT,texture_rg}
     * @requires_webgl20 For texture data only. Not available in WebGL 1.0, see
     *      @ref PixelFormat::LuminanceAlpha for and alternative.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RG = GL_RG,
    #else
    RG = GL_RG_EXT,
    #endif
    #endif

    #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * Floating-point luminance and alpha channel. First value is used for all
     * RGB channels, second value is used for alpha channel.
     * @requires_gles20 Not available in ES 3.0, WebGL 2.0 or desktop OpenGL.
     *      Use @ref PixelFormat::RG instead.
     * @deprecated_gl Included for compatibility reasons only, use
     *      @ref PixelFormat::RG instead.
     */
    LuminanceAlpha = GL_LUMINANCE_ALPHA,
    #endif

    /**
     * Floating-point RGB.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or WebGL.
     */
    RGB = GL_RGB,

    /** Floating-point RGBA. */
    RGBA = GL_RGBA,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Floating-point BGR.
     * @requires_gl Only RGB component ordering is available in OpenGL ES and
     *      WebGL.
     */
    BGR = GL_BGR,
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Floating-point BGRA.
     * @requires_es_extension Extension @extension{EXT,read_format_bgra}
     *      for framebuffer reading, extension @extension{APPLE,texture_format_BGRA8888}
     *      or @extension{EXT,texture_format_BGRA8888} for texture data.
     * @requires_gles Only RGBA component ordering is available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    BGRA = GL_BGRA,
    #else
    BGRA = GL_BGRA_EXT,
    #endif
    #endif

    #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * Floating-point sRGB.
     * @requires_gles20 Not available in ES 3.0, WebGL 2.0 or desktop OpenGL.
     *      Use @ref PixelFormat::RGB instead.
     * @deprecated_gl Included only in order to make it possible to upload
     *      sRGB image data with the @extension{EXT,sRGB} ES2 extension, use
     *      @ref PixelFormat::RGB elsewhere instead.
     */
    SRGB = GL_SRGB_EXT,

    /**
     * Floating-point sRGB + alpha.
     * @requires_gles20 Not available in ES 3.0, WebGL 2.0 or desktop OpenGL.
     *      Use @ref PixelFormat::RGBA instead.
     * @deprecated_gl Included only in order to make it possible to upload
     *      sRGB image data with the @extension{EXT,sRGB} ES2 extension, use
     *      @ref PixelFormat::RGBA elsewhere instead.
     */
    SRGBAlpha = GL_SRGB_ALPHA_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Integer red channel.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only floating-point image data are available in OpenGL
     *      ES 2.0.
     * @requires_webgl20 Only floating-point image data are available in WebGL
     *      1.0.
     */
    RedInteger = GL_RED_INTEGER,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Integer green channel.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gl Only @ref PixelFormat::RedInteger is available in OpenGL ES
     *      3.0 and WebGL 2.0, only floating-point image data are available in
     *      OpenGL ES 2.0 and WebGL 1.0.
     */
    GreenInteger = GL_GREEN_INTEGER,

    /**
     * Integer blue channel.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gl Only @ref PixelFormat::RedInteger is available in OpenGL ES
     *      3.0 and WebGL 2.0, only floating-point image data are available in
     *      OpenGL ES 2.0 and WebGL 1.0.
     */
    BlueInteger = GL_BLUE_INTEGER,
    #endif

    /**
     * Integer red and green channel.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_gles30 For texture data only, only floating-point image data
     *      are available in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, only floating-point image data
     *      are available in WebGL 1.0.
     */
    RGInteger = GL_RG_INTEGER,

    /**
     * Integer RGB.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_gles30 For texture data only, only floating-point image data
     *      are available in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, only floating-point image data
     *      are available in WebGL 1.0.
     */
    RGBInteger = GL_RGB_INTEGER,

    /**
     * Integer RGBA.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only floating-point image data are available in OpenGL
     *      ES 2.0.
     * @requires_webgl20 Only floating-point image data are available in WebGL
     *      1.0.
     */
    RGBAInteger = GL_RGBA_INTEGER,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Integer BGR.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gl Only @ref PixelFormat::RGBInteger is available in OpenGL ES
     *      3.0 and WebGL 2.0, only floating-point image data are available in
     *      OpenGL ES 2.0 and WebGL 1.0.
     */
    BGRInteger = GL_BGR_INTEGER,

    /**
     * Integer BGRA.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gl Only @ref PixelFormat::RGBAInteger is available in OpenGL
     *      ES 3.0 and WebGL 2.0, only floating-point image data are available
     *      in OpenGL ES 2.0 and WebGL 1.0.
     */
    BGRAInteger = GL_BGRA_INTEGER,
    #endif
    #endif

    /**
     * Depth component.
     * @requires_gles30 For texture data only, extension @extension{OES,depth_texture}
     *      or @extension{ANGLE,depth_texture} in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading only, extension
     *      @extension2{NV,read_depth,GL_NV_read_depth_stencil}.
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{WEBGL,depth_texture} in WebGL 1.0.
     */
    DepthComponent = GL_DEPTH_COMPONENT,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Stencil index.
     * @requires_gl44 Extension @extension{ARB,texture_stencil8} for texture
     *      data, otherwise for framebuffer reading only.
     * @requires_es_extension Extension @extension2{NV,read_stencil,GL_NV_read_depth_stencil},
     *      for framebuffer reading only.
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension{OES,texture_stencil8}, for texture data only.
     * @requires_gles Stencil index is not available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    StencilIndex = GL_STENCIL_INDEX,
    #else
    StencilIndex = GL_STENCIL_INDEX_OES,
    #endif
    #endif

    /**
     * Depth and stencil.
     * @requires_gl30 Extension @extension{ARB,framebuffer_object}
     * @requires_gles30 For texture data only, extension @extension{OES,packed_depth_stencil}
     *      in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading only, extension
     *      @extension2{NV,read_depth_stencil,GL_NV_read_depth_stencil}
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{WEBGL,depth_texture} in WebGL 1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    DepthStencil = GL_DEPTH_STENCIL
    #else
    /* Using OES version even though WebGL 1.0 *has* DEPTH_STENCIL constant,
       because there are no such headers for it */
    DepthStencil = GL_DEPTH_STENCIL_OES
    #endif
};

/**
@brief Type of pixel data

Note that some formats can be used only for framebuffer reading (using
@ref AbstractFramebuffer::read()) and some only for texture data (using
@ref Texture::setSubImage() and others), the limitations are mentioned in
documentation of each particular value.

In most cases you may want to use @ref PixelType::UnsignedByte along with
@ref PixelFormat::Red (for grayscale images), @ref PixelFormat::RGB or
@ref PixelFormat::RGBA, the matching texture format is then
@ref TextureFormat::R8, @ref TextureFormat::RGB8 or @ref TextureFormat::RGBA8.
See documentation of these values for possible limitations when using OpenGL ES
2.0 or WebGL.

@see @ref Image, @ref ImageView, @ref BufferImage, @ref Trade::ImageData
*/
enum class PixelType: GLenum {
    /** Each component unsigned byte. */
    UnsignedByte = GL_UNSIGNED_BYTE,

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Each component signed byte.
     * @requires_gles30 For texture data only, only @ref PixelType::UnsignedByte
     *      is available in OpenGL ES 2.0.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_webgl20 For texture data only, only @ref PixelType::UnsignedByte
     *      is available in WebGL 1.0.
     */
    Byte = GL_BYTE,
    #endif

    /**
     * Each component unsigned short.
     * @requires_gles30 For texture data only, extension @extension{OES,depth_texture}
     *      or @extension{ANGLE,depth_texture} in OpenGL ES 2.0.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{WEBGL,depth_texture} in WebGL 1.0.
     */
    UnsignedShort = GL_UNSIGNED_SHORT,

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Each component signed short.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_gles30 For texture data only, only @ref PixelType::UnsignedShort
     *      is available in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, only @ref PixelType::UnsignedShort
     *      is available in WebGL 1.0.
     */
    Short = GL_SHORT,
    #endif

    /**
     * Each component unsigned int.
     * @requires_gles30 In OpenGL ES 2.0 for texture data only, using extension
     *      @extension{OES,depth_texture} or @extension{ANGLE,depth_texture}
     * @requires_webgl20 In WebGL 1.0 for texture data only, using extension
     *      @webgl_extension{WEBGL,depth_texture}
     */
    UnsignedInt = GL_UNSIGNED_INT,

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Each component signed int.
     * @requires_gles30 Only @ref PixelType::UnsignedInt is available in OpenGL
     *      ES 2.0.
     * @requires_webgl20 Only @ref PixelType::UnsignedInt is available in WebGL
     *      1.0.
     */
    Int = GL_INT,
    #endif

    /**
     * Each component half float.
     * @see @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     * @requires_gl30 Extension @extension{ARB,half_float_pixel}
     * @requires_gles30 For texture data only, extension
     *      @extension2{OES,texture_half_float,OES_texture_float} in OpenGL
     *      ES 2.0.
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{OES,texture_half_float} in WebGL 1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    HalfFloat = GL_HALF_FLOAT,
    #else
    HalfFloat = GL_HALF_FLOAT_OES,
    #endif

    /**
     * Each component float.
     * @requires_gles30 For texture data only, extension @extension{OES,texture_float}
     *      in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, extension @webgl_extension{OES,texture_float}
     *      in WebGL 1.0.
     */
    Float = GL_FLOAT,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGB, unsigned byte, red and green component 3bit, blue component 2bit.
     * @requires_gl Packed 12bit types are not available in OpenGL ES or WebGL.
     */
    UnsignedByte332 = GL_UNSIGNED_BYTE_3_3_2,

    /**
     * BGR, unsigned byte, red and green component 3bit, blue component 2bit.
     * @requires_gl Packed 12bit types are not available in OpenGL ES or WebGL.
     */
    UnsignedByte233Rev = GL_UNSIGNED_BYTE_2_3_3_REV,
    #endif

    /**
     * RGB, unsigned byte, red and blue component 5bit, green 6bit.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or WebGL.
     */
    UnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * BGR, unsigned short, red and blue 5bit, green 6bit.
     * @requires_gl Only @ref PixelType::UnsignedShort565 is available in
     *      OpenGL ES or WebGL.
     */
    UnsignedShort565Rev = GL_UNSIGNED_SHORT_5_6_5_REV,
    #endif

    /**
     * RGBA, unsigned short, each component 4bit.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or WebGL.
     */
    UnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * ABGR, unsigned short, each component 4bit.
     * @requires_es_extension For framebuffer reading only, extension
     *      @extension{EXT,read_format_bgra}
     * @requires_gles Only RGBA component ordering is available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    UnsignedShort4444Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
    #else
    UnsignedShort4444Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT,
    #endif
    #endif

    /**
     * RGBA, unsigned short, each RGB component 5bit, alpha component 1bit.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or WebGL.
     */
    UnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * ABGR, unsigned short, each RGB component 5bit, alpha component 1bit.
     * @requires_es_extension For framebuffer reading only, extension
     *      @extension{EXT,read_format_bgra}
     * @requires_gles Not available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    UnsignedShort1555Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
    #else
    UnsignedShort1555Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGBA, unsigned int, each component 8bit.
     * @requires_gl Use @ref PixelType::UnsignedByte in OpenGL ES and WebGL
     *      instead.
     */
    UnsignedInt8888 = GL_UNSIGNED_INT_8_8_8_8,

    /**
     * ABGR, unsigned int, each component 8bit.
     * @requires_gl Only RGBA component ordering is available in OpenGL ES and
     *      WebGL, see @ref PixelType::UnsignedInt8888 for more information.
     */
    UnsignedInt8888Rev = GL_UNSIGNED_INT_8_8_8_8_REV,

    /**
     * RGBA, unsigned int, each RGB component 10bit, alpha component 2bit.
     * @requires_gl Only @ref PixelType::UnsignedInt2101010Rev is available in
     *      OpenGL ES and WebGL.
     */
    UnsignedInt1010102 = GL_UNSIGNED_INT_10_10_10_2,
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * ABGR, unsigned int, each RGB component 10bit, alpha component 2bit.
     * @requires_gles30 Can't be used for framebuffer reading in OpenGL ES 2.0.
     * @requires_gles30 For texture data only, extension
     *      @extension{EXT,texture_type_2_10_10_10_REV} in OpenGL ES 2.0.
     *      Not available in WebGL 1.0.
     * @requires_webgl20 Only RGBA component ordering is available in WebGL
     *      1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,
    #else
    UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * BGR, unsigned int, red and green 11bit float, blue 10bit float.
     * @requires_gl30 Extension @extension{EXT,packed_float}
     * @requires_gles30 Floating-point types are not available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Floating-point types are not available in WebGL 1.0.
     */
    UnsignedInt10F11F11FRev = GL_UNSIGNED_INT_10F_11F_11F_REV,

    /**
     * BGR, unsigned int, each component 9bit + 5bit exponent.
     * @requires_gl30 Extension @extension{EXT,texture_shared_exponent}
     * @requires_gles30 Only 8bit and 16bit types are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only 8bit and 16bit types are available in WebGL 1.0.
     */
    UnsignedInt5999Rev = GL_UNSIGNED_INT_5_9_9_9_REV,
    #endif

    /**
     * Unsigned int, depth component 24bit, stencil index 8bit.
     * @requires_gl30 Extension @extension{ARB,framebuffer_object}
     * @requires_gles30 For texture data only, extension
     *      @extension{OES,packed_depth_stencil} in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{WEBGL,depth_texture} in WebGL 1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    UnsignedInt248 = GL_UNSIGNED_INT_24_8,
    #else
    UnsignedInt248 = GL_UNSIGNED_INT_24_8_OES,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Float + unsigned int, depth component 32bit float, 24bit gap, stencil
     * index 8bit.
     * @requires_gl30 Extension @extension{ARB,depth_buffer_float}
     * @requires_gles30 For texture data only, only @ref PixelType::UnsignedInt248
     *      is available in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, only @ref PixelType::UnsignedInt248
     *      is available in WebGL 1.0.
     */
    Float32UnsignedInt248Rev = GL_FLOAT_32_UNSIGNED_INT_24_8_REV
    #endif
};

/**
@brief Format of compressed pixel data

Equivalent to `Compressed*` values of @ref TextureFormat enum.

@see @ref CompressedImage, @ref CompressedImageView, @ref CompressedBufferImage,
    @ref Trade::ImageData
*/
enum class CompressedPixelFormat: GLenum {
    #ifndef MAGNUM_TARGET_GLES
    /**
     * Compressed red channel, normalized unsigned.
     * @requires_gl30 Extension @extension{ARB,texture_rg}
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    Red = GL_COMPRESSED_RED,

    /**
     * Compressed red and green channel, normalized unsigned.
     * @requires_gl30 Extension @extension{ARB,texture_rg}
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    RG = GL_COMPRESSED_RG,

    /**
     * Compressed RGB, normalized unsigned.
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    RGB = GL_COMPRESSED_RGB,

    /**
     * Compressed RGBA, normalized unsigned.
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    RGBA = GL_COMPRESSED_RGBA,

    /**
     * RGTC compressed red channel, normalized unsigned. **Available only for
     * 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    RedRgtc1 = GL_COMPRESSED_RED_RGTC1,

    /**
     * RGTC compressed red and green channel, normalized unsigned. **Available
     * only for 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    RGRgtc2 = GL_COMPRESSED_RG_RGTC2,

    /**
     * RGTC compressed red channel, normalized signed. **Available only for 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    SignedRedRgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,

    /**
     * RGTC compressed red and green channel, normalized signed. **Available
     * only for 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    SignedRGRgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,

    /**
     * BPTC compressed RGB, unsigned float. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    RGBBptcUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,

    /**
     * BPTC compressed RGB, signed float. **Available only on 2D, 3D, 2D array,
     * cube map and cube map array textures.**
     * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    RGBBptcSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,

    /**
     * BPTC compressed RGBA, normalized unsigned. **Available only on 2D, 3D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    RGBABptcUnorm = GL_COMPRESSED_RGBA_BPTC_UNORM,

    /**
     * BPTC compressed sRGBA, normalized unsigned. **Available only on 2D, 3D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    SRGBAlphaBptcUnorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * ETC2 compressed RGB, normalized unsigned. **Available only on 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    RGB8Etc2 = GL_COMPRESSED_RGB8_ETC2,

    /**
     * ETC2 compressed sRGB, normalized unsigned. **Available only on 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    SRGB8Etc2 = GL_COMPRESSED_SRGB8_ETC2,

    /**
     * ETC2 compressed RGB with punchthrough (single-bit) alpha, normalized
     * unsigned. **Available only on 2D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    RGB8PunchthroughAlpha1Etc2 = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,

    /**
     * ETC2 compressed sRGB with punchthrough (single-bit) alpha, normalized
     * unsigned. **Available only on 2D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    SRGB8PunchthroughAlpha1Etc2 = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,

    /**
     * ETC2/EAC compressed RGBA, normalized unsigned. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    RGBA8Etc2Eac = GL_COMPRESSED_RGBA8_ETC2_EAC,

    /**
     * ETC2/EAC compressed sRGB with alpha, normalized unsigned. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    SRGB8Alpha8Etc2Eac = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,

    /**
     * EAC compressed red channel, normalized unsigned. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    R11Eac = GL_COMPRESSED_R11_EAC,

    /**
     * EAC compressed red channel, normalized signed. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    SignedR11Eac = GL_COMPRESSED_SIGNED_R11_EAC,

    /**
     * EAC compressed red and green channel, normalized unsigned. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    RG11Eac = GL_COMPRESSED_RG11_EAC,

    /**
     * EAC compressed red and green channel, normalized signed. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    SignedRG11Eac = GL_COMPRESSED_SIGNED_RG11_EAC,
    #endif

    /**
     * S3TC DXT1 compressed RGB. **Available only for 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @extension2{EXT,texture_compression_s3tc,texture_compression_s3tc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    RGBS3tcDxt1 = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,

    /**
     * S3TC DXT1 compressed RGBA. **Available only for 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @extension2{EXT,texture_compression_s3tc,texture_compression_s3tc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    RGBAS3tcDxt1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,

    /**
     * S3TC DXT3 compressed RGBA. **Available only for 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @extension2{EXT,texture_compression_s3tc,texture_compression_s3tc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    RGBAS3tcDxt3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,

    /**
     * S3TC DXT5 compressed RGBA. **Available only for 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @extension2{EXT,texture_compression_s3tc,texture_compression_s3tc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    RGBAS3tcDxt5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * ASTC compressed RGBA with 4x4 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc4x4 = GL_COMPRESSED_RGBA_ASTC_4x4_KHR,

    /**
     * ASTC compressed sRGB with alpha with 4x4 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,

    /**
     * ASTC compressed RGBA with 5x4 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc5x4 = GL_COMPRESSED_RGBA_ASTC_5x4_KHR,

    /**
     * ASTC compressed sRGB with alpha with 5x4 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,

    /**
     * ASTC compressed RGBA with 5x5 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc5x5 = GL_COMPRESSED_RGBA_ASTC_5x5_KHR,

    /**
     * ASTC compressed sRGB with alpha with 5x5 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,

    /**
     * ASTC compressed RGBA with 6x5 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc6x5 = GL_COMPRESSED_RGBA_ASTC_6x5_KHR,

    /**
     * ASTC compressed sRGB with alpha with 6x5 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,

    /**
     * ASTC compressed RGBA with 6x6 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc6x6 = GL_COMPRESSED_RGBA_ASTC_6x6_KHR,

    /**
     * ASTC compressed sRGB with alpha with 6x6 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,

    /**
     * ASTC compressed RGBA with 8x5 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc8x5 = GL_COMPRESSED_RGBA_ASTC_8x5_KHR,

    /**
     * ASTC compressed sRGB with alpha with 8x5 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc8x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,

    /**
     * ASTC compressed RGBA with 8x6 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc8x6 = GL_COMPRESSED_RGBA_ASTC_8x6_KHR,

    /**
     * ASTC compressed sRGB with alpha with 8x6 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc8x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,

    /**
     * ASTC compressed RGBA with 8x8 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc8x8 = GL_COMPRESSED_RGBA_ASTC_8x8_KHR,

    /**
     * ASTC compressed sRGB with alpha with 8x8 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc8x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,

    /**
     * ASTC compressed RGBA with 10x5 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc10x5 = GL_COMPRESSED_RGBA_ASTC_10x5_KHR,

    /**
     * ASTC compressed sRGB with alpha with 10x5 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc10x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,

    /**
     * ASTC compressed RGBA with 10x6 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc10x6 = GL_COMPRESSED_RGBA_ASTC_10x6_KHR,

    /**
     * ASTC compressed sRGB with alpha with 10x6 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc10x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,

    /**
     * ASTC compressed RGBA with 10x8 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc10x8 = GL_COMPRESSED_RGBA_ASTC_10x8_KHR,

    /**
     * ASTC compressed sRGB with alpha with 10x8 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc10x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,

    /**
     * ASTC compressed RGBA with 10x10 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc10x10 = GL_COMPRESSED_RGBA_ASTC_10x10_KHR,

    /**
     * ASTC compressed sRGB with alpha with 10x10 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc10x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,

    /**
     * ASTC compressed RGBA with 12x10 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc12x10 = GL_COMPRESSED_RGBA_ASTC_12x10_KHR,

    /**
     * ASTC compressed sRGB with alpha with 12x10 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc12x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,

    /**
     * ASTC compressed RGBA with 12x12 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    RGBAAstc12x12 = GL_COMPRESSED_RGBA_ASTC_12x12_KHR,

    /**
     * ASTC compressed sRGB with alpha with 12x12 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_extension Extension @extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension2{KHR,texture_compression_astc_ldr,texture_compression_astc_hdr}
     * @requires_es_extension Extension @extension2{KHR,texture_compression_astc_hdr,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles ASTC texture compression is not available in WebGL.
     */
    SRGB8Alpha8Astc12x12 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR
    #endif
};

/** @debugoperatorenum{Magnum::PixelFormat} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, PixelFormat value);

/** @debugoperatorenum{Magnum::PixelType} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, PixelType value);

/** @debugoperatorenum{Magnum::CompressedPixelFormat} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, CompressedPixelFormat value);

}

#endif
