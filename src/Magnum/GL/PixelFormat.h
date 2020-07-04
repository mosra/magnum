#ifndef Magnum_GL_PixelFormat_h
#define Magnum_GL_PixelFormat_h
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

/** @file
 * @brief Enum @ref Magnum::GL::PixelFormat, @ref Magnum::GL::PixelType, @ref Magnum::GL::CompressedPixelFormat, function @ref Magnum::GL::hasPixelFormat(), @ref Magnum::GL::pixelFormat(), @ref Magnum::GL::pixelType(), @ref Magnum::GL::pixelSize(), @ref Magnum::GL::hasCompressedPixelFormat(), @ref Magnum::GL::compressedPixelFormat()
 */

#include <Corrade/Utility/Assert.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/GL/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace GL {

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

@see @ref Magnum::PixelFormat, @ref pixelFormat(), @ref hasPixelFormat(),
    @ref Image, @ref ImageView, @ref BufferImage, @ref Trade::ImageData
@m_enum_values_as_keywords
*/
enum class PixelFormat: GLenum {
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Floating-point red channel.
     * @requires_gles30 For texture data only, extension
     *      @gl_extension{EXT,texture_rg} in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading, extension
     *      @gl_extension{EXT,texture_rg}
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
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 For texture data only, extension
     *      @gl_extension{EXT,texture_rg} in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading, extension
     *      @gl_extension{EXT,texture_rg}
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
     * Floating-point BGR. On desktop OpenGL there's no corresponding dedicated
     * texture format for BGR(A) --- simply use it with @ref TextureFormat::RGB8.
     * @requires_gl For three-component formats, only RGB component ordering is
     *      available in OpenGL ES and WebGL. See @ref PixelFormat::BGRA for an
     *      alternative.
     */
    BGR = GL_BGR,
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Floating-point BGRA. On desktop OpenGL there's no corresponding
     * dedicated texture format for BGR(A) --- simply use it with
     * @ref TextureFormat::RGBA8. On OpenGL ES however, you're required to
     * use @ref TextureFormat::BGRA8 (which is on the other hand not defined
     * for desktop OpenGL).
     * @requires_es_extension Extension @gl_extension{EXT,read_format_bgra}
     *      for framebuffer reading, extension @gl_extension{APPLE,texture_format_BGRA8888}
     *      or @gl_extension{EXT,texture_format_BGRA8888} for texture data.
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
     *      sRGB image data with the @gl_extension{EXT,sRGB} ES2 extension, use
     *      @ref PixelFormat::RGB elsewhere instead.
     */
    SRGB = GL_SRGB_EXT,

    /**
     * Floating-point sRGB + alpha.
     * @requires_gles20 Not available in ES 3.0, WebGL 2.0 or desktop OpenGL.
     *      Use @ref PixelFormat::RGBA instead.
     * @deprecated_gl Included only in order to make it possible to upload
     *      sRGB image data with the @gl_extension{EXT,sRGB} ES2 extension, use
     *      @ref PixelFormat::RGBA elsewhere instead.
     */
    SRGBAlpha = GL_SRGB_ALPHA_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Integer red channel.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only floating-point image data are available in OpenGL
     *      ES 2.0.
     * @requires_webgl20 Only floating-point image data are available in WebGL
     *      1.0.
     */
    RedInteger = GL_RED_INTEGER,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Integer green channel.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gl Only @ref PixelFormat::RedInteger is available in OpenGL ES
     *      3.0 and WebGL 2.0, only floating-point image data are available in
     *      OpenGL ES 2.0 and WebGL 1.0.
     */
    GreenInteger = GL_GREEN_INTEGER,

    /**
     * Integer blue channel.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gl Only @ref PixelFormat::RedInteger is available in OpenGL ES
     *      3.0 and WebGL 2.0, only floating-point image data are available in
     *      OpenGL ES 2.0 and WebGL 1.0.
     */
    BlueInteger = GL_BLUE_INTEGER,
    #endif

    /**
     * Integer red and green channel.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
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
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
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
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only floating-point image data are available in OpenGL
     *      ES 2.0.
     * @requires_webgl20 Only floating-point image data are available in WebGL
     *      1.0.
     */
    RGBAInteger = GL_RGBA_INTEGER,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Integer BGR.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gl Only @ref PixelFormat::RGBInteger is available in OpenGL ES
     *      3.0 and WebGL 2.0, only floating-point image data are available in
     *      OpenGL ES 2.0 and WebGL 1.0.
     */
    BGRInteger = GL_BGR_INTEGER,

    /**
     * Integer BGRA.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gl Only @ref PixelFormat::RGBAInteger is available in OpenGL
     *      ES 3.0 and WebGL 2.0, only floating-point image data are available
     *      in OpenGL ES 2.0 and WebGL 1.0.
     */
    BGRAInteger = GL_BGRA_INTEGER,
    #endif
    #endif

    /**
     * Depth component.
     * @requires_gles30 For texture data only, extension @gl_extension{OES,depth_texture}
     *      or @gl_extension{ANGLE,depth_texture} in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading only, extension
     *      @gl_extension2{NV,read_depth,NV_read_depth_stencil}.
     * @requires_webgl20 For texture data only, extension
     *      @webgl_extension{WEBGL,depth_texture} in WebGL 1.0.
     */
    DepthComponent = GL_DEPTH_COMPONENT,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Stencil index.
     * @requires_gl44 Extension @gl_extension{ARB,texture_stencil8} for texture
     *      data, otherwise for framebuffer reading only.
     * @requires_es_extension Extension @gl_extension2{NV,read_stencil,NV_read_depth_stencil},
     *      for framebuffer reading only.
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension{OES,texture_stencil8}, for texture data only.
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
     * @requires_gl30 Extension @gl_extension{ARB,framebuffer_object}
     * @requires_gles30 For texture data only, extension @gl_extension{OES,packed_depth_stencil}
     *      in OpenGL ES 2.0.
     * @requires_es_extension For framebuffer reading only, extension
     *      @gl_extension{NV,read_depth_stencil}
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

@see @ref Magnum::PixelFormat, @ref pixelType(), @ref hasPixelFormat(),
    @ref Image, @ref ImageView, @ref BufferImage, @ref Trade::ImageData
@m_enum_values_as_keywords
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
     * @requires_gles30 Extension @gl_extension{OES,depth_texture}
     *      or @gl_extension{ANGLE,depth_texture} in OpenGL ES 2.0 for depth
     *      texture data
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     *      for color texture data
     * @requires_webgl20 Extension @webgl_extension{WEBGL,depth_texture} in
     *      WebGL 1.0 for depth texture data
     * @requires_webgl_extension @webgl_extension{EXT,texture_norm16} for color
     *      texture data
     */
    UnsignedShort = GL_UNSIGNED_SHORT,

    /* Available everywhere except ES2 (WebGL 1 has it) */
    #if !(defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    /**
     * Each component signed short.
     * @requires_gl Can't be used for framebuffer reading in OpenGL ES or
     *      WebGL.
     * @requires_gles30 For texture data only, only @ref PixelType::UnsignedShort
     *      is available in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, @webgl_extension{EXT,texture_norm16}
     *      in WebGL 1.0.
     */
    Short = GL_SHORT,
    #endif

    /**
     * Each component unsigned int.
     * @requires_gles30 In OpenGL ES 2.0 for texture data only, using extension
     *      @gl_extension{OES,depth_texture} or @gl_extension{ANGLE,depth_texture}
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
     * @see @ref Magnum::Half "Half", @ref Math::packHalf(),
     *      @ref Math::unpackHalf()
     * @m_since{2020,06}
     * @requires_gl30 Extension @gl_extension{ARB,half_float_pixel}
     * @requires_gles30 Extension @gl_extension2{OES,texture_half_float,OES_texture_float}
     *      to use for texture reading in OpenGL ES 2.0.
     * @requires_gles30 Extension @gl_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
     *      for filtering the texture using @ref SamplerFilter::Linear.
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_half_float}
     *      to use the texture as a render target.
     * @requires_webgl20 Extension @webgl_extension{OES,texture_half_float} to
     *      use for texture reading in WebGL 1.0.
     * @requires_webgl20 Extension @webgl_extension{OES,texture_half_float_linear}
     *      for filtering the texture using @ref SamplerFilter::Linear.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_half_float}
     *      to use the texture as a render target.
     */
    #ifndef MAGNUM_TARGET_GLES2
    Half = GL_HALF_FLOAT,
    #else
    Half = GL_HALF_FLOAT_OES,
    #endif

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * Half float.
     * @m_deprecated_since{2020,06} Use @ref PixelType::Half instead.
     */
    HalfFloat CORRADE_DEPRECATED_ENUM("use Half instead") = Half,
    #endif

    /**
     * Each component float.
     * @requires_gles30 Extension @gl_extension{OES,texture_float} to use for
     *      texture reading in OpenGL ES 2.0.
     * @requires_gles32 Extension @gl_extension{EXT,color_half_float} to use the
     *      texture as a render target.
     * @requires_es_extension Extension @gl_extension{OES,texture_float_linear}
     *      for filtering the texture using @ref SamplerFilter::Linear.
     * @requires_webgl20 Extension @webgl_extension{OES,texture_float} to use
     *      for texture reading in WebGL 1.0.
     * @requires_webgl_extension Extension @webgl_extension{OES,texture_float_linear}
     *      for filtering the texture using @ref SamplerFilter::Linear.
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,color_buffer_float}
     *      in WebGL 1.0 or @webgl_extension{EXT,color_buffer_float} in WebGL
     *      2.0 to use the texture as a render target.
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
     *      @gl_extension{EXT,read_format_bgra}
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
     *      @gl_extension{EXT,read_format_bgra}
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
     *      @gl_extension{EXT,texture_type_2_10_10_10_REV} in OpenGL ES 2.0.
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
     * @requires_gl30 Extension @gl_extension{EXT,packed_float}
     * @requires_gles30 Floating-point types are not available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Floating-point types are not available in WebGL 1.0.
     */
    UnsignedInt10F11F11FRev = GL_UNSIGNED_INT_10F_11F_11F_REV,

    /**
     * BGR, unsigned int, each component 9bit + 5bit exponent.
     * @requires_gl30 Extension @gl_extension{EXT,texture_shared_exponent}
     * @requires_gles30 Only 8bit and 16bit types are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only 8bit and 16bit types are available in WebGL 1.0.
     */
    UnsignedInt5999Rev = GL_UNSIGNED_INT_5_9_9_9_REV,
    #endif

    /**
     * Unsigned int, depth component 24bit, stencil index 8bit.
     * @requires_gl30 Extension @gl_extension{ARB,framebuffer_object}
     * @requires_gles30 For texture data only, extension
     *      @gl_extension{OES,packed_depth_stencil} in OpenGL ES 2.0.
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
     * @requires_gl30 Extension @gl_extension{ARB,depth_buffer_float}
     * @requires_gles30 For texture data only, only @ref PixelType::UnsignedInt248
     *      is available in OpenGL ES 2.0.
     * @requires_webgl20 For texture data only, only @ref PixelType::UnsignedInt248
     *      is available in WebGL 1.0.
     */
    Float32UnsignedInt248Rev = GL_FLOAT_32_UNSIGNED_INT_24_8_REV
    #endif
};

/**
@brief Check availability of a generic pixel format

Some OpenGL targets don't support all generic pixel formats (for example WebGL
1.0 and OpenGL ES 2.0 don't support most of single- and two-component or
integer formats). Returns @cpp false @ce if current target can't support such
format, @cpp true @ce otherwise. Moreover, returns @cpp true @ce also for all
formats that are @ref isPixelFormatImplementationSpecific(). The @p format
value is expected to be valid.

@note Support of some formats depends on presence of a particular OpenGL
    extension. Such check is outside of the scope of this function and you are
    expected to verify extension availability before using such format.

@see @ref pixelFormat(), @ref pixelType(), @ref hasCompressedPixelFormat()
*/
MAGNUM_GL_EXPORT bool hasPixelFormat(Magnum::PixelFormat format);

/**
@brief Convert a generic pixel format to OpenGL pixel format

In case @ref isPixelFormatImplementationSpecific() returns @cpp false @ce for
@p format, maps it to a corresponding OpenGL pixel format. In case
@ref isPixelFormatImplementationSpecific() returns @cpp true @ce, assumes
@p format stores OpenGL-specific pixel format and returns
@ref pixelFormatUnwrap() cast to @ref GL::PixelFormat.

Not all generic pixel formats may be available on all targets and this function
expects that given format is available on the target. Use @ref hasPixelFormat()
to query availability of given format.

@note On OpenGL ES 2.0 and WebGL 1.0, one- and two-channel texture formats are
    always translated to @ref PixelFormat::Luminance and
    @ref PixelFormat::LuminanceAlpha, independently on the
    @gl_extension{EXT,texture_rg} being present or not. If you wish to use @ref PixelFormat::Red and @ref PixelFormat::RG instead, specify the GL-specific
    pixel format directly instead of using the generic enum.

@see @ref pixelType(), @ref compressedPixelFormat()
*/
MAGNUM_GL_EXPORT PixelFormat pixelFormat(Magnum::PixelFormat format);

/**
@brief Convert a generic pixel type to OpenGL pixel type

In case @ref isPixelFormatImplementationSpecific() returns @cpp false @ce for
@p format, maps it to a corresponding OpenGL pixel type. In case
@ref isPixelFormatImplementationSpecific() returns @cpp true @ce, assumes
@p extra stores OpenGL-specific pixel type and returns it cast to
@ref GL::PixelType.

Not all generic pixel formats may be available on all targets and this function
expects that given format is available on the target. Use @ref hasPixelFormat()
to query availability of given format.
@see @ref pixelFormat(), @ref compressedPixelFormat()
*/
MAGNUM_GL_EXPORT PixelType pixelType(Magnum::PixelFormat format, UnsignedInt extra = 0);

/**
@brief Pixel size for given format/type combination (in bytes)

@see @ref Magnum::pixelSize(), @ref PixelStorage::dataProperties()
*/
MAGNUM_GL_EXPORT UnsignedInt pixelSize(PixelFormat format, PixelType type);

/** @debugoperatorenum{PixelFormat} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, PixelFormat value);

/** @debugoperatorenum{PixelType} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, PixelType value);

/**
@brief Format of compressed pixel data

Equivalent to `Compressed*` values of @ref TextureFormat enum.

@see @ref Magnum::CompressedPixelFormat, @ref compressedPixelFormat(),
    @ref hasCompressedPixelFormat(), @ref CompressedImage,
    @ref CompressedImageView, @ref CompressedBufferImage, @ref Trade::ImageData
@m_enum_values_as_keywords
*/
enum class CompressedPixelFormat: GLenum {
    #ifndef MAGNUM_TARGET_GLES
    /**
     * Compressed red channel, normalized unsigned.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    Red = GL_COMPRESSED_RED,

    /**
     * Compressed red and green channel, normalized unsigned.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
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
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    /**
     * RGTC compressed red channel, normalized unsigned. Equivalent to the old
     * @def_gl{COMPRESSED_LUMINANCE_LATC1_EXT} from
     * @gl_extension{EXT,texture_compression_latc}, only using the red channel
     * instead of all three. **Available only on 2D, 2D array, cube map and
     * cube map array textures.**
     * @requires_gl30 Extension @gl_extension{EXT,texture_compression_rgtc}
     * @requires_es_extension OpenGL ES 3.0 and extension
     *      @gl_extension{EXT,texture_compression_rgtc}
     * @requires_webgl_extension Extension
     *      @webgl_extension{EXT,texture_compression_rgtc}. Unlike the OpenGL
     *      ES variant, this extension doesn't require WebGL 2.
     */
    #ifndef MAGNUM_TARGET_GLES
    RedRgtc1 = GL_COMPRESSED_RED_RGTC1,
    #else
    RedRgtc1 = GL_COMPRESSED_RED_RGTC1_EXT,
    #endif

    /**
     * RGTC compressed red and green channel, normalized unsigned. Equivalent
     * to the old @def_gl{COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT} from
     * @gl_extension{EXT,texture_compression_latc}, only using the red and
     * green channel instead of all four. **Available only on 2D, 2D array,
     * cube map and cube map array textures.**
     * @requires_gl30 Extension @gl_extension{EXT,texture_compression_rgtc}
     * @requires_es_extension OpenGL ES 3.0 and extension
     *      @gl_extension{EXT,texture_compression_rgtc}
     * @requires_webgl_extension Extension
     *      @webgl_extension{EXT,texture_compression_rgtc}. Unlike the OpenGL
     *      ES variant, this extension doesn't require WebGL 2.
     */
    #ifndef MAGNUM_TARGET_GLES
    RGRgtc2 = GL_COMPRESSED_RG_RGTC2,
    #else
    RGRgtc2 = GL_COMPRESSED_RED_GREEN_RGTC2_EXT, /*?!*/
    #endif

    /**
     * RGTC compressed red channel, normalized signed. Equivalent to the old
     * @def_gl{COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT} from
     * @gl_extension{EXT,texture_compression_latc}, only using the red channel
     * instead of all three. **Available only on 2D, 2D array, cube map and
     * cube map array textures.**
     * @requires_gl30 Extension @gl_extension{EXT,texture_compression_rgtc}
     * @requires_es_extension OpenGL ES 3.0 and extension
     *      @gl_extension{EXT,texture_compression_rgtc}
     * @requires_webgl_extension Extension
     *      @webgl_extension{EXT,texture_compression_rgtc}. Unlike the OpenGL
     *      ES variant, this extension doesn't require WebGL 2.
     */
    #ifndef MAGNUM_TARGET_GLES
    SignedRedRgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
    #else
    SignedRedRgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,
    #endif

    /**
     * RGTC compressed red and green channel, normalized signed. Equivalent
     * to the old @def_gl{COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT} from
     * @gl_extension{EXT,texture_compression_latc}, only using the red and
     * green channel instead of all four. **Available only on 2D, 2D array,
     * cube map and cube map array textures.**
     * @requires_gl30 Extension @gl_extension{EXT,texture_compression_rgtc}
     * @requires_es_extension OpenGL ES 3.0 and extension
     *      @gl_extension{EXT,texture_compression_rgtc}
     * @requires_webgl_extension Extension
     *      @webgl_extension{EXT,texture_compression_rgtc}. Unlike the OpenGL
     *      ES variant, this extension doesn't require WebGL 2.
     */
    #ifndef MAGNUM_TARGET_GLES
    SignedRGRgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
    #else
    SignedRGRgtc2 = GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT, /*?!*/
    #endif

    /**
     * BPTC compressed RGB, unsigned float. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl42 Extension @gl_extension{ARB,texture_compression_bptc}
     * @requires_es_extension OpenGL ES 3.0 and extension
     *      @gl_extension{EXT,texture_compression_bptc}
     * @requires_webgl_extension Extension
     *      @webgl_extension{EXT,texture_compression_bptc}. Unlike the OpenGL
     *      ES variant, this extension doesn't require WebGL 2.
     */
    #ifndef MAGNUM_TARGET_GLES
    RGBBptcUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
    #else
    RGBBptcUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT,
    #endif

    /**
     * BPTC compressed RGB, signed float. **Available only on 2D, 3D, 2D array,
     * cube map and cube map array textures.**
     * @requires_gl42 Extension @gl_extension{ARB,texture_compression_bptc}
     * @requires_es_extension OpenGL ES 3.0 and extension
     *      @gl_extension{EXT,texture_compression_bptc}
     * @requires_webgl_extension Extension
     *      @webgl_extension{EXT,texture_compression_bptc}. Unlike the OpenGL
     *      ES variant, this extension doesn't require WebGL 2.
     */
    #ifndef MAGNUM_TARGET_GLES
    RGBBptcSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
    #else
    RGBBptcSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT,
    #endif

    /**
     * BPTC compressed RGBA, normalized unsigned. **Available only on 2D, 3D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl42 Extension @gl_extension{ARB,texture_compression_bptc}
     * @requires_es_extension OpenGL ES 3.0 and extension
     *      @gl_extension{EXT,texture_compression_bptc}
     * @requires_webgl_extension Extension
     *      @webgl_extension{EXT,texture_compression_bptc}. Unlike the OpenGL
     *      ES variant, this extension doesn't require WebGL 2.
     */
    #ifndef MAGNUM_TARGET_GLES
    RGBABptcUnorm = GL_COMPRESSED_RGBA_BPTC_UNORM,
    #else
    RGBABptcUnorm = GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,
    #endif

    /**
     * BPTC compressed sRGBA, normalized unsigned. **Available only on 2D, 3D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl42 Extension @gl_extension{ARB,texture_compression_bptc}
     * @requires_es_extension OpenGL ES 3.0 and extension
     *      @gl_extension{EXT,texture_compression_bptc}
     * @requires_webgl_extension Extension
     *      @webgl_extension{EXT,texture_compression_bptc}. Unlike the OpenGL
     *      ES variant, this extension doesn't require WebGL 2.
     */
    #ifndef MAGNUM_TARGET_GLES
    SRGBAlphaBptcUnorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
    #else
    SRGBAlphaBptcUnorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * ETC2 compressed RGB, normalized unsigned. **Available only on 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    RGB8Etc2 = GL_COMPRESSED_RGB8_ETC2,

    /**
     * ETC2 compressed sRGB, normalized unsigned. **Available only on 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    SRGB8Etc2 = GL_COMPRESSED_SRGB8_ETC2,

    /**
     * ETC2 compressed RGB with punchthrough (single-bit) alpha, normalized
     * unsigned. **Available only on 2D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    RGB8PunchthroughAlpha1Etc2 = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,

    /**
     * ETC2 compressed sRGB with punchthrough (single-bit) alpha, normalized
     * unsigned. **Available only on 2D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    SRGB8PunchthroughAlpha1Etc2 = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,

    /**
     * ETC2/EAC compressed RGBA, normalized unsigned. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    RGBA8Etc2Eac = GL_COMPRESSED_RGBA8_ETC2_EAC,

    /**
     * ETC2/EAC compressed sRGB with alpha, normalized unsigned. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    SRGB8Alpha8Etc2Eac = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,

    /**
     * EAC compressed red channel, normalized unsigned. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    R11Eac = GL_COMPRESSED_R11_EAC,

    /**
     * EAC compressed red channel, normalized signed. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    SignedR11Eac = GL_COMPRESSED_SIGNED_R11_EAC,

    /**
     * EAC compressed red and green channel, normalized unsigned. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    RG11Eac = GL_COMPRESSED_RG11_EAC,

    /**
     * EAC compressed red and green channel, normalized signed. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    SignedRG11Eac = GL_COMPRESSED_SIGNED_RG11_EAC,
    #endif

    /**
     * S3TC DXT1 compressed RGB. **Available only for 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     *      or @gl_extension{EXT,texture_compression_dxt1}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc},
     *      @gl_extension{EXT,texture_compression_dxt1} or
     *      @gl_extension2{ANGLE,texture_compression_dxt1,ANGLE_texture_compression_dxt}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    RGBS3tcDxt1 = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,

    /**
     * S3TC DXT1 compressed sRGB. **Available only for 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc_srgb}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc_srgb}
     * @m_since{2019,10}
     */
    SRGBS3tcDxt1 = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,

    /**
     * S3TC DXT1 compressed RGBA. **Available only for 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     *      or @gl_extension{EXT,texture_compression_dxt1}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc},
     *      @gl_extension{EXT,texture_compression_dxt1} or
     *      @gl_extension2{ANGLE,texture_compression_dxt1,ANGLE_texture_compression_dxt}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    RGBAS3tcDxt1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,

    /**
     * S3TC DXT1 compressed sRGB + linear alpha. **Available only for 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc_srgb}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc_srgb}
     * @m_since{2019,10}
     */
    SRGBAlphaS3tcDxt1 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,

    /**
     * S3TC DXT3 compressed RGBA. **Available only for 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     *      or @gl_extension2{ANGLE,texture_compression_dxt3,ANGLE_texture_compression_dxt}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    RGBAS3tcDxt3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,

    /**
     * S3TC DXT3 compressed sRGB + linear alpha. **Available only for 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc_srgb}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc_srgb}
     * @m_since{2019,10}
     */
    SRGBAlphaS3tcDxt3 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,

    /**
     * S3TC DXT5 compressed RGBA. **Available only for 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     *      or @gl_extension2{ANGLE,texture_compression_dxt5,ANGLE_texture_compression_dxt}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    RGBAS3tcDxt5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,

    /**
     * S3TC DXT5 compressed sRGB + linear alpha. **Available only for 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc_srgb}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc_srgb}
     * @m_since{2019,10}
     */
    SRGBAlphaS3tcDxt5 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,

    /**
     * 2D ASTC compressed RGBA with 4x4 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc4x4 = GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
    #else
    RGBAAstc4x4 = GL_COMPRESSED_RGBA_ASTC_4x4,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 4x4 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
    #else
    SRGB8Alpha8Astc4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4,
    #endif

    /**
     * 2D ASTC compressed RGBA with 5x4 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc5x4 = GL_COMPRESSED_RGBA_ASTC_5x4_KHR,
    #else
    RGBAAstc5x4 = GL_COMPRESSED_RGBA_ASTC_5x4,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 5x4 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
    #else
    SRGB8Alpha8Astc5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4,
    #endif

    /**
     * 2D ASTC compressed RGBA with 5x5 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc5x5 = GL_COMPRESSED_RGBA_ASTC_5x5_KHR,
    #else
    RGBAAstc5x5 = GL_COMPRESSED_RGBA_ASTC_5x5,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 5x5 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
    #else
    SRGB8Alpha8Astc5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5,
    #endif

    /**
     * 2D ASTC compressed RGBA with 6x5 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc6x5 = GL_COMPRESSED_RGBA_ASTC_6x5_KHR,
    #else
    RGBAAstc6x5 = GL_COMPRESSED_RGBA_ASTC_6x5,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 6x5 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
    #else
    SRGB8Alpha8Astc6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5,
    #endif

    /**
     * 2D ASTC compressed RGBA with 6x6 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc6x6 = GL_COMPRESSED_RGBA_ASTC_6x6_KHR,
    #else
    RGBAAstc6x6 = GL_COMPRESSED_RGBA_ASTC_6x6,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 6x6 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
    #else
    SRGB8Alpha8Astc6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6,
    #endif

    /**
     * 2D ASTC compressed RGBA with 8x5 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc8x5 = GL_COMPRESSED_RGBA_ASTC_8x5_KHR,
    #else
    RGBAAstc8x5 = GL_COMPRESSED_RGBA_ASTC_8x5,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 8x5 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc8x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
    #else
    SRGB8Alpha8Astc8x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5,
    #endif

    /**
     * 2D ASTC compressed RGBA with 8x6 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc8x6 = GL_COMPRESSED_RGBA_ASTC_8x6_KHR,
    #else
    RGBAAstc8x6 = GL_COMPRESSED_RGBA_ASTC_8x6,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 8x6 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc8x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,
    #else
    SRGB8Alpha8Astc8x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6,
    #endif

    /**
     * 2D ASTC compressed RGBA with 8x8 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc8x8 = GL_COMPRESSED_RGBA_ASTC_8x8_KHR,
    #else
    RGBAAstc8x8 = GL_COMPRESSED_RGBA_ASTC_8x8,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 8x8 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc8x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
    #else
    SRGB8Alpha8Astc8x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8,
    #endif

    /**
     * 2D ASTC compressed RGBA with 10x5 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc10x5 = GL_COMPRESSED_RGBA_ASTC_10x5_KHR,
    #else
    RGBAAstc10x5 = GL_COMPRESSED_RGBA_ASTC_10x5,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 10x5 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc10x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
    #else
    SRGB8Alpha8Astc10x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5,
    #endif

    /**
     * 2D ASTC compressed RGBA with 10x6 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc10x6 = GL_COMPRESSED_RGBA_ASTC_10x6_KHR,
    #else
    RGBAAstc10x6 = GL_COMPRESSED_RGBA_ASTC_10x6,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 10x6 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc10x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,
    #else
    SRGB8Alpha8Astc10x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6,
    #endif

    /**
     * 2D ASTC compressed RGBA with 10x8 blocks. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc10x8 = GL_COMPRESSED_RGBA_ASTC_10x8_KHR,
    #else
    RGBAAstc10x8 = GL_COMPRESSED_RGBA_ASTC_10x8,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 10x8 blocks. **Available only on
     * 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc10x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,
    #else
    SRGB8Alpha8Astc10x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8,
    #endif

    /**
     * 2D ASTC compressed RGBA with 10x10 blocks. **Available only on 2D, 3D,
     * 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc10x10 = GL_COMPRESSED_RGBA_ASTC_10x10_KHR,
    #else
    RGBAAstc10x10 = GL_COMPRESSED_RGBA_ASTC_10x10,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 10x10 blocks. **Available only
     * on 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc10x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,
    #else
    SRGB8Alpha8Astc10x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10,
    #endif

    /**
     * 2D ASTC compressed RGBA with 12x10 blocks. **Available only on 2D, 3D,
     * 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc12x10 = GL_COMPRESSED_RGBA_ASTC_12x10_KHR,
    #else
    RGBAAstc12x10 = GL_COMPRESSED_RGBA_ASTC_12x10,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 12x10 blocks. **Available only
     * on 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc12x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,
    #else
    SRGB8Alpha8Astc12x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10,
    #endif

    /**
     * 2D ASTC compressed RGBA with 12x12 blocks. **Available only on 2D, 3D,
     * 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    RGBAAstc12x12 = GL_COMPRESSED_RGBA_ASTC_12x12_KHR,
    #else
    RGBAAstc12x12 = GL_COMPRESSED_RGBA_ASTC_12x12,
    #endif

    /**
     * 2D ASTC compressed sRGB with alpha with 12x12 blocks. **Available only
     * on 2D, 3D, 2D array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension2{KHR,texture_compression_astc_ldr,KHR_texture_compression_astc_hdr}
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_sliced_3d}
     *      for 3D textures
     * @requires_es_extension Extension @gl_extension{KHR,texture_compression_astc_hdr}
     *      for 3D textures and HDR profile
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_astc},
     *      2D and cube map textures only
     */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    SRGB8Alpha8Astc12x12 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
    #else
    SRGB8Alpha8Astc12x12 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12,
    #endif

    #if defined(DOXYGEN_GENERATING_OUTPUT) || (defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    /**
     * 3D ASTC compressed RGBA with 3x3x3 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    RGBAAstc3x3x3 = GL_COMPRESSED_RGBA_ASTC_3x3x3_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 3x3x3 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    SRGB8Alpha8Astc3x3x3 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES,

    /**
     * 3D ASTC compressed RGBA with 4x3x3 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    RGBAAstc4x3x3 = GL_COMPRESSED_RGBA_ASTC_4x3x3_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 4x3x3 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    SRGB8Alpha8Astc4x3x3 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES,

    /**
     * 3D ASTC compressed RGBA with 4x4x3 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    RGBAAstc4x4x3 = GL_COMPRESSED_RGBA_ASTC_4x4x3_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 4x4x3 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    SRGB8Alpha8Astc4x4x3 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES,

    /**
     * 3D ASTC compressed RGBA with 4x4x4 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    RGBAAstc4x4x4 = GL_COMPRESSED_RGBA_ASTC_4x4x4_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 4x4x4 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    SRGB8Alpha8Astc4x4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES,

    /**
     * 3D ASTC compressed RGBA with 5x4x4 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    RGBAAstc5x4x4 = GL_COMPRESSED_RGBA_ASTC_5x4x4_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 5x4x4 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    SRGB8Alpha8Astc5x4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES,

    /**
     * 3D ASTC compressed RGBA with 5x5x4 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    RGBAAstc5x5x4 = GL_COMPRESSED_RGBA_ASTC_5x5x4_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 5x5x4 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    SRGB8Alpha8Astc5x5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES,

    /**
     * 3D ASTC compressed RGBA with 5x5x5 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    RGBAAstc5x5x5 = GL_COMPRESSED_RGBA_ASTC_5x5x5_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 5x5x5 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    SRGB8Alpha8Astc5x5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES,

    /**
     * 3D ASTC compressed RGBA with 6x5x5 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    RGBAAstc6x5x5 = GL_COMPRESSED_RGBA_ASTC_6x5x5_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 6x5x5 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    SRGB8Alpha8Astc6x5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES,

    /**
     * 3D ASTC compressed RGBA with 6x6x5 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    RGBAAstc6x6x5 = GL_COMPRESSED_RGBA_ASTC_6x6x5_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 6x6x5 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    SRGB8Alpha8Astc6x6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES,

    /**
     * 3D ASTC compressed RGBA with 6x6x6 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    RGBAAstc6x6x6 = GL_COMPRESSED_RGBA_ASTC_6x6x6_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 6x6x6 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     * @m_since{2019,10}
     */
    SRGB8Alpha8Astc6x6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES,
    #endif

    #if defined(DOXYGEN_GENERATING_OUTPUT) || defined(MAGNUM_TARGET_GLES)
    /**
     * PVRTC compressed RGB, normalized unsigned byte with 2 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL.
     * @requires_es_extension Extension @gl_extension{IMG,texture_compression_pvrtc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_pvrtc}
     * @m_since{2019,10}
     */
    RGBPvrtc2bppV1 = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * PVRTC compressed sRGB, normalized unsigned byte with 2 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL or WebGL.
     * @requires_es_extension Extension @gl_extension{EXT,pvrtc_sRGB}
     * @m_since{2019,10}
     */
    SRGBPvrtc2bppV1 = GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT,
    #endif

    /**
     * PVRTC compressed RGBA, normalized unsigned byte with 2 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL.
     * @requires_es_extension Extension @gl_extension{IMG,texture_compression_pvrtc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_pvrtc}
     * @m_since{2019,10}
     */
    RGBAPvrtc2bppV1 = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * PVRTC compressed sRGB + linear alpha, normalized unsigned byte with 2
     * bits per pixel. **Available only on 2D, 3D, 2D array, cube map and cube
     * map array textures.**
     * @requires_gles Not available on desktop OpenGL or WebGL.
     * @requires_es_extension Extension @gl_extension{EXT,pvrtc_sRGB}
     * @m_since{2019,10}
     */
    SRGBAlphaPvrtc2bppV1 = GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT,
    #endif

    /**
     * PVRTC compressed RGB, normalized unsigned byte with 4 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL.
     * @requires_es_extension Extension @gl_extension{IMG,texture_compression_pvrtc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_pvrtc}
     * @m_since{2019,10}
     */
    RGBPvrtc4bppV1 = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * PVRTC compressed sRGB, normalized unsigned byte with 4 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL or WebGL.
     * @requires_es_extension Extension @gl_extension{EXT,pvrtc_sRGB}
     * @m_since{2019,10}
     */
    SRGBPvrtc4bppV1 = GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT,
    #endif

    /**
     * PVRTC compressed RGBA, normalized unsigned byte with 4 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL.
     * @requires_es_extension Extension @gl_extension{IMG,texture_compression_pvrtc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_pvrtc}
     * @m_since{2019,10}
     */
    RGBAPvrtc4bppV1 = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * PVRTC compressed sRGB + linear alpha, normalized unsigned byte with 4
     * bits per pixel. **Available only on 2D, 3D, 2D array, cube map and cube
     * map array textures.**
     * @requires_gles Not available on desktop OpenGL or WebGL.
     * @requires_es_extension Extension @gl_extension{EXT,pvrtc_sRGB}
     * @m_since{2019,10}
     */
    SRGBAlphaPvrtc4bppV1 = GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT
    #endif

    /* PVRTC2 variants not listed as PVRTC is mainly on Apple hardware but
       Metal doesn't support it and it doesn't have a WebGL equiv either. */
    #endif
};

/**
@brief Check availability of generic compressed pixel format

Some OpenGL targets don't support all generic pixel formats (for example PVRTC
compression might not be available on desktop OpenGL). Returns @cpp false @ce
if current target can't support such format, @cpp true @ce otherwise. Moreover,
returns @cpp true @ce also for all formats that are
@ref isCompressedPixelFormatImplementationSpecific(). The @p format value is
expected to be valid.

@note Support of some formats depends on presence of a particular OpenGL
    extension. Such check is outside of the scope of this function and you are
    expected to verify extension availability before using such format.

@see @ref compressedPixelFormat(), @ref hasPixelFormat(),
    @ref hasTextureFormat()
*/
MAGNUM_GL_EXPORT bool hasCompressedPixelFormat(Magnum::CompressedPixelFormat format);

/**
@brief Convert generic compressed pixel format to OpenGL compressed pixel format

In case @ref isCompressedPixelFormatImplementationSpecific() returns
@cpp false @ce for @p format, maps it to a corresponding OpenGL pixel format.
In case @ref isCompressedPixelFormatImplementationSpecific() returns
@cpp true @ce, assumes @p format stores OpenGL-specific pixel format and
returns @ref compressedPixelFormatUnwrap() cast to @ref GL::CompressedPixelFormat.

Not all generic pixel formats may be available on all targets and this function
expects that given format is available on the target. Use
@ref hasCompressedPixelFormat() to query availability of given format.
@see @ref pixelFormat(), @ref textureFormat()
*/
MAGNUM_GL_EXPORT CompressedPixelFormat compressedPixelFormat(Magnum::CompressedPixelFormat format);

/** @debugoperatorenum{CompressedPixelFormat} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, CompressedPixelFormat value);

}}

#endif
