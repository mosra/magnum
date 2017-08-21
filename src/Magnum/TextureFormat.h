#ifndef Magnum_TextureFormat_h
#define Magnum_TextureFormat_h
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
 * @brief Enum @ref Magnum::TextureFormat
 */

#include "Magnum/OpenGL.h"

namespace Magnum {

/**
@brief Internal texture format

In most cases you may want to use @ref TextureFormat::R8 (for grayscale
textures), @ref TextureFormat::RGB8 or @ref TextureFormat::RGBA8. The matching
color format is then @ref PixelFormat::Red, @ref PixelFormat::RGB or
@ref PixelFormat::RGBA along with @ref PixelType::UnsignedByte. See
documentation of these values for possible limitations when using OpenGL ES 2.0
or WebGL.

@see @ref Texture, @ref CubeMapTexture, @ref CubeMapTextureArray
*/
enum class TextureFormat: GLenum {
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Red component, normalized unsigned, size implementation-dependent. Not
     * allowed in unemulated @ref Texture::setStorage() "*Texture::setStorage()"
     * calls, in that case use @ref TextureFormat::R8 "TextureFormat::R8"
     * instead.
     * @requires_gl30 Extension @extension{ARB,texture_rg}
     * @requires_gles30 Extension @extension{EXT,texture_rg} in OpenGL ES
     *      2.0.
     * @requires_webgl20 Not available in WebGL 1.0, see @ref TextureFormat::Luminance
     *      for an alternative.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref TextureFormat::R8.
     */
    #ifndef MAGNUM_TARGET_GLES2
    Red = GL_RED,
    #else
    Red = GL_RED_EXT,
    #endif

    /**
     * Red component, normalized unsigned byte.
     * @requires_gl30 Extension @extension{ARB,texture_rg}
     * @requires_gles30 Extension @extension{EXT,texture_rg} and
     *      @extension{EXT,texture_storage} in OpenGL ES 2.0. For texture
     *      storage only, for image specification use @ref TextureFormat::Red
     *      instead.
     * @requires_webgl20 Not available in WebGL 1.0, see @ref TextureFormat::Luminance
     *      for an alternative.
     */
    #ifndef MAGNUM_TARGET_GLES2
    R8 = GL_R8,
    #else
    R8 = GL_R8_EXT,
    #endif
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Red and green component, normalized unsigned, size
     * implementation-dependent. Not allowed in unemulated
     * @ref Texture::setStorage() "*Texture::setStorage()" calls, in that case
     * use @ref TextureFormat::RG8 "TextureFormat::RG8" instead.
     * @requires_gl30 Extension @extension{ARB,texture_rg}
     * @requires_gles30 Extension @extension{EXT,texture_rg} in OpenGL ES
     *      2.0.
     * @requires_webgl20 Not available in WebGL 1.0, see
     *      @ref TextureFormat::LuminanceAlpha for an alternative.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref TextureFormat::RG8.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RG = GL_RG,
    #else
    RG = GL_RG_EXT,
    #endif

    /**
     * Red and green component, each normalized unsigned byte.
     * @requires_gl30 Extension @extension{ARB,texture_rg}
     * @requires_gles30 Extension @extension{EXT,texture_rg} and
     *      @extension{EXT,texture_storage} in OpenGL ES 2.0. For texture
     *      storage only, for image specification use @ref TextureFormat::RG
     *      instead.
     * @requires_webgl20 Not available in WebGL 1.0, see
     *      @ref TextureFormat::LuminanceAlpha for an alternative.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RG8 = GL_RG8,
    #else
    RG8 = GL_RG8_EXT,
    #endif
    #endif

    /**
     * RGB, normalized unsigned, size implementation-dependent. Not allowed in
     * unemulated @ref Texture::setStorage() "*Texture::setStorage()" calls, in
     * that case use @ref TextureFormat::RGB8 "TextureFormat::RGB8" instead.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref TextureFormat::RGB8.
     */
    RGB = GL_RGB,

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * RGB, each component normalized unsigned byte.
     * @requires_gles30 Extension @extension{OES,required_internalformat}
     *      (for image specification) or @extension{EXT,texture_storage}
     *      (for texture storage) in OpenGL ES 2.0. Use @ref TextureFormat::RGB
     *      otherwise.
     * @requires_webgl20 Not available in WebGL 1.0, use
     *      @ref TextureFormat::RGB instead.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RGB8 = GL_RGB8,
    #else
    RGB8 = GL_RGB8_OES,
    #endif
    #endif

    /**
     * RGBA, normalized unsigned, size implementation-dependent. Not allowed in
     * unemulated @ref Texture::setStorage() "*Texture::setStorage()" calls, in
     * that case use @ref TextureFormat::RGBA8 "TextureFormat::RGBA8" instead.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref TextureFormat::RGBA8.
     */
    RGBA = GL_RGBA,

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * RGBA, each component normalized unsigned byte.
     * @requires_gles30 Extension @extension{OES,required_internalformat}
     *      (for image specification) or @extension{EXT,texture_storage}
     *      (for texture storage) in OpenGL ES 2.0. Use @ref TextureFormat::RGBA
     *      otherwise.
     * @requires_webgl20 Not available in WebGL 1.0, use
     *      @ref TextureFormat::RGBA instead.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RGBA8 = GL_RGBA8,
    #else
    RGBA8 = GL_RGBA8_OES,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red component, normalized signed byte.
     * @requires_gl31 Extension @extension{EXT,texture_snorm}
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     * @requires_webgl20 Only unsigned formats are available in WebGL 1.0.
     */
    R8Snorm = GL_R8_SNORM,

    /**
     * Red and green component, each normalized signed byte.
     * @requires_gl31 Extension @extension{EXT,texture_snorm}
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     * @requires_webgl20 Only unsigned formats are available in WebGL 1.0.
     */
    RG8Snorm = GL_RG8_SNORM,

    /**
     * RGB, each component normalized signed byte.
     * @requires_gl31 Extension @extension{EXT,texture_snorm}
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     * @requires_webgl20 Only unsigned formats are available in WebGL 1.0.
     */
    RGB8Snorm = GL_RGB8_SNORM,

    /**
     * RGBA, each component normalized signed byte.
     * @requires_gl31 Extension @extension{EXT,texture_snorm}
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     * @requires_webgl20 Only unsigned formats are available in WebGL 1.0.
     */
    RGBA8Snorm = GL_RGBA8_SNORM,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, normalized unsigned short.
     * @requires_gl30 Extension @extension{ARB,texture_rg}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES and WebGL.
     */
    R16 = GL_R16,

    /**
     * Red and green component, each normalized unsigned short.
     * @requires_gl30 Extension @extension{ARB,texture_rg}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES and WebGL.
     */
    RG16 = GL_RG16,

    /**
     * RGB, each component normalized unsigned short.
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES and WebGL.
     */
    RGB16 = GL_RGB16,

    /**
     * RGBA, each component normalized unsigned short.
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES and WebGL.
     */
    RGBA16 = GL_RGBA16,

    /**
     * Red component, normalized signed short.
     * @requires_gl31 Extension @extension{EXT,texture_snorm}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES and WebGL.
     */
    R16Snorm = GL_R16_SNORM,

    /**
     * Red and green component, each normalized signed short.
     * @requires_gl31 Extension @extension{EXT,texture_snorm}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES and WebGL.
     */
    RG16Snorm = GL_RG16_SNORM,

    /**
     * RGB, each component normalized signed short.
     * @requires_gl31 Extension @extension{EXT,texture_snorm}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES and WebGL.
     */
    RGB16Snorm = GL_RGB16_SNORM,

    /**
     * RGBA, each component normalized signed short.
     * @requires_gl31 Extension @extension{EXT,texture_snorm}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES and WebGL.
     */
    RGBA16Snorm = GL_RGBA16_SNORM,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red component, non-normalized unsigned byte.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R8UI = GL_R8UI,

    /**
     * Red and green component, each non-normalized unsigned byte.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG8UI = GL_RG8UI,

    /**
     * RGB, each component non-normalized unsigned byte.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB8UI = GL_RGB8UI,

    /**
     * RGBA, each component non-normalized unsigned byte.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA8UI = GL_RGBA8UI,

    /**
     * Red component, non-normalized signed byte.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R8I = GL_R8I,

    /**
     * Red and green component, each non-normalized signed byte.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG8I = GL_RG8I,

    /**
     * RGB, each component non-normalized signed byte.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB8I = GL_RGB8I,

    /**
     * RGBA, each component non-normalized signed byte.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA8I = GL_RGBA8I,

    /**
     * Red component, non-normalized unsigned short.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R16UI = GL_R16UI,

    /**
     * Red and green component, each non-normalized unsigned short.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG16UI = GL_RG16UI,

    /**
     * RGB, each component non-normalized unsigned short.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB16UI = GL_RGB16UI,

    /**
     * RGBA, each component non-normalized unsigned short.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA16UI = GL_RGBA16UI,

    /**
     * Red component, non-normalized signed short.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R16I = GL_R16I,

    /**
     * Red and green component, each non-normalized signed short.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG16I = GL_RG16I,

    /**
     * RGB, each component non-normalized signed short.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB16I = GL_RGB16I,

    /**
     * RGBA, each component non-normalized signed short.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA16I = GL_RGBA16I,

    /**
     * Red component, non-normalized unsigned int.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R32UI = GL_R32UI,

    /**
     * Red and green component, each non-normalized unsigned int.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG32UI = GL_RG32UI,

    /**
     * RGB, each component non-normalized unsigned int.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB32UI = GL_RGB32UI,

    /**
     * RGBA, each component non-normalized unsigned int.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA32UI = GL_RGBA32UI,

    /**
     * Red component, non-normalized signed int.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R32I = GL_R32I,

    /**
     * Red and green component, each non-normalized signed int.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG32I = GL_RG32I,

    /**
     * RGB, each component non-normalized signed int.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB32I = GL_RGB32I,

    /**
     * RGBA, each component non-normalized signed int.
     * @requires_gl30 Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA32I = GL_RGBA32I,

    /**
     * Red component, half float.
     * @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R16F = GL_R16F,

    /**
     * Red and green component, each half float.
     * @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG16F = GL_RG16F,

    /**
     * RGB, each component half float.
     * @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     * @requires_gl30 Extension @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB16F = GL_RGB16F,

    /**
     * RGBA, each component half float.
     * @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     * @requires_gl30 Extension @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA16F = GL_RGBA16F,

    /**
     * Red component, float.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R32F = GL_R32F,

    /**
     * Red and green component, each float.
     * @requires_gl30 Extension @extension{ARB,texture_rg} and @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG32F = GL_RG32F,

    /**
     * RGB, each component float.
     * @requires_gl30 Extension @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB32F = GL_RGB32F,

    /**
     * RGBA, each component float.
     * @requires_gl30 Extension @extension{ARB,texture_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA32F = GL_RGBA32F,
    #endif

    #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * Luminance, normalized unsigned, single value used for all RGB channels.
     * Size implementation-dependent. Not allowed in unemulated
     * @ref Texture::setStorage() "*Texture::setStorage()" calls.
     * @requires_gles20 Not available in OpenGL ES 3.0, WebGL 2.0 or desktop
     *      OpenGL. Use @ref TextureFormat::R8 instead.
     * @deprecated_gl Included for compatibility reasons only, use
     *      @ref TextureFormat::R8 instead.
     */
    Luminance = GL_LUMINANCE,

    /**
     * Floating-point luminance and alpha channel. First value is used for all
     * RGB channels, second value is used for alpha channel. Size
     * implementation-dependent. Not allowed in unemulated
     * @ref Texture::setStorage() "*Texture::setStorage()" calls.
     * @requires_gles20 Not available in OpenGL ES 3.0, WebGL 2.0 or desktop
     *      OpenGL. Use @ref TextureFormat::RG8 instead.
     * @deprecated_gl Included for compatibility reasons only, use
     *      @ref TextureFormat::RG8 instead.
     */
    LuminanceAlpha = GL_LUMINANCE_ALPHA,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGB, normalized unsigned, red and green component 3bit, blue 2bit.
     * @requires_gl Packed 8bit types are not available in OpenGL ES or WebGL.
     */
    R3B3G2 = GL_R3_G3_B2,

    /**
     * RGB, each component normalized unsigned 4bit.
     * @requires_gl Packed 12bit types are not available in OpenGL ES or WebGL.
     */
    RGB4 = GL_RGB4,

    /**
     * RGB, each component normalized unsigned 5bit.
     * @requires_gl Use @ref TextureFormat::RGB5A1 or @ref TextureFormat::RGB565
     *      in OpenGL ES or WebGL.
     */
    RGB5 = GL_RGB5,
    #endif

    /**
     * RGB, normalized unsigned, red and blue component 5bit, green 6bit.
     * @requires_gles30 Extension @extension{OES,required_internalformat}
     *      (for image specification) or @extension{EXT,texture_storage}
     *      (for texture storage) in OpenGL ES 2.0.
     * @requires_webgl20 Not available in WebGL 1.0, see @ref TextureFormat::RGB
     *      for an alternative.
     */
    RGB565 = GL_RGB565,

    #if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    /**
     * RGB, each component normalized unsigned 10bit.
     * @requires_es_extension Extension @extension{EXT,texture_type_2_10_10_10_REV}
     *      and either @extension{OES,required_internalformat} (for image
     *      specification) or @extension{EXT,texture_storage} (for texture
     *      storage) in OpenGL ES 2.0. Included for compatibility reasons only,
     *      use @ref TextureFormat::RGB10A2 in OpenGL ES 3.0 instead.
     * @requires_gles Not available in WebGL, use @ref TextureFormat::RGB10A2
     *      instead.
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
     * @requires_gl Packed 36bit types are not available in OpenGL ES or WebGL.
     */
    RGB12 = GL_RGB12,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * RGB, float, red and green component 11bit, blue 10bit.
     * @requires_gl30 Extension @extension{EXT,packed_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R11FG11FB10F = GL_R11F_G11F_B10F,

    /**
     * RGB, unsigned with exponent, each RGB component 9bit, exponent 5bit.
     * @requires_gl30 Extension @extension{EXT,texture_shared_exponent}
     * @requires_gles30 Use @ref TextureFormat::RGB in OpenGL ES 2.0 instead.
     * @requires_webgl20 Use @ref TextureFormat::RGB in WebGL 1.0 instead.
     */
    RGB9E5 = GL_RGB9_E5,
    #endif

    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    /**
     * sRGB, normalized unsigned, size implementation-dependent. Not allowed in
     * unemulated @ref Texture::setStorage() "*Texture::setStorage()" calls, in
     * that case use @ref TextureFormat::SRGB8 "TextureFormat::SRGB8" instead.
     * @requires_es_extension Extension @extension{EXT,sRGB} in OpenGL ES 2.0.
     *      Use @ref TextureFormat::SRGB8 in OpenGL ES 3.0 instead.
     * @requires_webgl_extension Extension @webgl_extension{EXT,sRGB} in WebGL
     *      1.0. Use @ref TextureFormat::SRGB8 in WebGL 2.0 instead.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, i.e. @ref TextureFormat::SRGB8.
     */
    #ifndef MAGNUM_TARGET_GLES
    SRGB = GL_SRGB,
    #else
    SRGB = GL_SRGB_EXT,
    #endif
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * sRGB, each component normalized unsigned byte.
     * @requires_gles30 Extension @extension{EXT,sRGB} and
     *      @extension{EXT,texture_storage}, only for
     *      @ref Texture::setStorage() "*Texture::setStorage()" calls,
     *      otherwise use @ref TextureFormat::SRGB in OpenGL ES 2.0 instead.
     * @requires_webgl20 Use @ref TextureFormat::SRGB in WebGL 1.0 instead.
     */
    #ifndef MAGNUM_TARGET_GLES2
    SRGB8 = GL_SRGB8,
    #else
    SRGB8 = 0x8C41, /* Not in any spec, but seems to work at least on NV */
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGBA, normalized unsigned, each component 2bit.
     * @requires_gl Packed 8bit types are not available in OpenGL ES or WebGL.
     */
    RGBA2 = GL_RGBA2,
    #endif

    /**
     * RGBA, normalized unsigned, each component 4bit.
     * @requires_gles30 Extension @extension{OES,required_internalformat}
     *      (for image specification) or @extension{EXT,texture_storage}
     *      (for texture storage) in OpenGL ES 2.0. Use @ref TextureFormat::RGBA
     *      otherwise.
     * @requires_webgl20 Not available in WebGL 1.0, use @ref TextureFormat::RGBA
     *      instead.
     */
    RGBA4 = GL_RGBA4,

    /**
     * RGBA, normalized unsigned, each RGB component 5bit, alpha 1bit.
     * @requires_gles30 Extension @extension{OES,required_internalformat}
     *      (for image specification) or @extension{EXT,texture_storage}
     *      (for texture storage) in OpenGL ES 2.0. Use @ref TextureFormat::RGBA
     *      otherwise.
     * @requires_webgl20 Not available in WebGL 1.0, use @ref TextureFormat::RGBA
     *      instead.
     */
    RGB5A1 = GL_RGB5_A1,

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * RGBA, normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gles30 Extension @extension{EXT,texture_type_2_10_10_10_REV}
     *      and either @extension{OES,required_internalformat} (for image
     *      specification) or @extension{EXT,texture_storage} (for texture
     *      storage) in OpenGL ES 2.0.
     * @requires_webgl20 Not available in WebGL 1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RGB10A2 = GL_RGB10_A2,
    #else
    RGB10A2 = GL_RGB10_A2_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * RGBA, non-normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gl33 Extension @extension{ARB,texture_rgb10_a2ui}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB10A2UI = GL_RGB10_A2UI,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGBA, each component normalized unsigned 12bit.
     * @requires_gl Packed 48bit types are not available in OpenGL ES or WebGL.
     */
    RGBA12 = GL_RGBA12,
    #endif

    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    /**
     * sRGB + linear alpha, normalized unsigned, size implementation-dependent.
     * Not allowed in unemulated @ref Texture::setStorage() "*Texture::setStorage()"
     * calls, in that case use @ref TextureFormat::SRGB8Alpha8 "TextureFormat::SRGB8Alpha8"
     * instead.
     * @requires_es_extension Extension @extension{EXT,sRGB} in OpenGL ES
     *      2.0. Use @ref TextureFormat::SRGB8Alpha8 in OpenGL ES 3.0 instead.
     * @requires_webgl_extension Extension @webgl_extension{EXT,sRGB} in WebGL
     *      1.0. Use @ref TextureFormat::SRGB8Alpha8 in WebGL 2.0 instead.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, i.e. @ref TextureFormat::SRGB8Alpha8.
     */
    #ifndef MAGNUM_TARGET_GLES
    SRGBAlpha = GL_SRGB_ALPHA,
    #else
    SRGBAlpha = GL_SRGB_ALPHA_EXT,
    #endif
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * sRGB + linear alpha, each component normalized unsigned byte.
     * @requires_gles30 Extension @extension{EXT,sRGB} and
     *      @extension{EXT,texture_storage}, only for
     *      @ref Texture::setStorage() "*Texture::setStorage()" calls,
     *      otherwise use @ref TextureFormat::SRGBAlpha in OpenGL ES 2.0
     *      instead.
     * @requires_webgl20 Use @ref TextureFormat::SRGBAlpha in WebGL 1.0 instead.
     */
    #ifndef MAGNUM_TARGET_GLES2
    SRGB8Alpha8 = GL_SRGB8_ALPHA8,
    #else
    SRGB8Alpha8 = GL_SRGB8_ALPHA8_EXT, /* Not in spec, works at least on NV */
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Compressed red channel, normalized unsigned. **Not available on
     * multisample textures.**
     * @requires_gl30 Extension @extension{ARB,texture_rg}
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    CompressedRed = GL_COMPRESSED_RED,

    /**
     * Compressed red and green channel, normalized unsigned. **Not available
     * on multisample textures.**
     * @requires_gl30 Extension @extension{ARB,texture_rg}
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    CompressedRG = GL_COMPRESSED_RG,

    /**
     * Compressed RGB, normalized unsigned. **Not available on multisample
     * textures.**
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    CompressedRGB = GL_COMPRESSED_RGB,

    /**
     * Compressed RGBA, normalized unsigned. **Not available on multisample
     * textures.**
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    CompressedRGBA = GL_COMPRESSED_RGBA,

    /**
     * RGTC compressed red channel, normalized unsigned. **Available only on
     * 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    CompressedRedRgtc1 = GL_COMPRESSED_RED_RGTC1,

    /**
     * RGTC compressed red and green channel, normalized unsigned. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    CompressedRGRgtc2 = GL_COMPRESSED_RG_RGTC2,

    /**
     * RGTC compressed red channel, normalized signed. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    CompressedSignedRedRgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,

    /**
     * RGTC compressed red and green channel, normalized signed. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
     * @requires_gl RGTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    CompressedSignedRGRgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,

    /**
     * BPTC compressed RGB, unsigned float. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    CompressedRGBBptcUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,

    /**
     * BPTC compressed RGB, signed float. **Available only on 2D, 3D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    CompressedRGBBptcSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,

    /**
     * BPTC compressed RGBA, normalized unsigned. **Available only on 2D, 3D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    CompressedRGBABptcUnorm = GL_COMPRESSED_RGBA_BPTC_UNORM,

    /**
     * BPTC compressed sRGBA, normalized unsigned. **Available only on 2D, 3D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
     * @requires_gl BPTC texture compression is not available in OpenGL ES or
     *      WebGL.
     */
    CompressedSRGBAlphaBptcUnorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * ETC2 compressed RGB, normalized unsigned. **Available only on 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedRGB8Etc2 = GL_COMPRESSED_RGB8_ETC2,

    /**
     * ETC2 compressed sRGB, normalized unsigned. **Available only on 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedSRGB8Etc2 = GL_COMPRESSED_SRGB8_ETC2,

    /**
     * ETC2 compressed RGB with punchthrough (single-bit) alpha, normalized
     * unsigned. **Available only on 2D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedRGB8PunchthroughAlpha1Etc2 = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,

    /**
     * ETC2 compressed sRGB with punchthrough (single-bit) alpha, normalized
     * unsigned. **Available only on 2D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedSRGB8PunchthroughAlpha1Etc2 = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,

    /**
     * ETC2/EAC compressed RGBA, normalized unsigned. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedRGBA8Etc2Eac = GL_COMPRESSED_RGBA8_ETC2_EAC,

    /**
     * ETC2/EAC compressed sRGB with alpha, normalized unsigned. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedSRGB8Alpha8Etc2Eac = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,

    /**
     * EAC compressed red channel, normalized unsigned. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedR11Eac = GL_COMPRESSED_R11_EAC,

    /**
     * EAC compressed red channel, normalized signed. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedSignedR11Eac = GL_COMPRESSED_SIGNED_R11_EAC,

    /**
     * EAC compressed red and green channel, normalized unsigned. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedRG11Eac = GL_COMPRESSED_RG11_EAC,

    /**
     * EAC compressed red and green channel, normalized signed. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedSignedRG11Eac = GL_COMPRESSED_SIGNED_RG11_EAC,
    #endif

    /**
     * S3TC DXT1 compressed RGB. **Available only on 2D, 2D array, cube map and
     * cube map array textures.**
     * @requires_extension Extension @extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @extension2{EXT,texture_compression_s3tc,texture_compression_s3tc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    CompressedRGBS3tcDxt1 = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,

    /**
     * S3TC DXT1 compressed RGBA. **Available only on 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @extension2{EXT,texture_compression_s3tc,texture_compression_s3tc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    CompressedRGBAS3tcDxt1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,

    /**
     * S3TC DXT3 compressed RGBA. **Available only on 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @extension2{EXT,texture_compression_s3tc,texture_compression_s3tc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    CompressedRGBAS3tcDxt3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,

    /**
     * S3TC DXT5 compressed RGBA. **Available only on 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @extension2{EXT,texture_compression_s3tc,texture_compression_s3tc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    CompressedRGBAS3tcDxt5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,

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
    CompressedRGBAAstc4x4 = GL_COMPRESSED_RGBA_ASTC_4x4_KHR,

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
    CompressedSRGB8Alpha8Astc4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,

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
    CompressedRGBAAstc5x4 = GL_COMPRESSED_RGBA_ASTC_5x4_KHR,

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
    CompressedSRGB8Alpha8Astc5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,

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
    CompressedRGBAAstc5x5 = GL_COMPRESSED_RGBA_ASTC_5x5_KHR,

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
    CompressedSRGB8Alpha8Astc5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,

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
    CompressedRGBAAstc6x5 = GL_COMPRESSED_RGBA_ASTC_6x5_KHR,

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
    CompressedSRGB8Alpha8Astc6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,

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
    CompressedRGBAAstc6x6 = GL_COMPRESSED_RGBA_ASTC_6x6_KHR,

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
    CompressedSRGB8Alpha8Astc6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,

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
    CompressedRGBAAstc8x5 = GL_COMPRESSED_RGBA_ASTC_8x5_KHR,

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
    CompressedSRGB8Alpha8Astc8x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,

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
    CompressedRGBAAstc8x6 = GL_COMPRESSED_RGBA_ASTC_8x6_KHR,

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
    CompressedSRGB8Alpha8Astc8x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,

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
    CompressedRGBAAstc8x8 = GL_COMPRESSED_RGBA_ASTC_8x8_KHR,

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
    CompressedSRGB8Alpha8Astc8x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,

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
    CompressedRGBAAstc10x5 = GL_COMPRESSED_RGBA_ASTC_10x5_KHR,

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
    CompressedSRGB8Alpha8Astc10x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,

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
    CompressedRGBAAstc10x6 = GL_COMPRESSED_RGBA_ASTC_10x6_KHR,

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
    CompressedSRGB8Alpha8Astc10x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,

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
    CompressedRGBAAstc10x8 = GL_COMPRESSED_RGBA_ASTC_10x8_KHR,

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
    CompressedSRGB8Alpha8Astc10x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,

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
    CompressedRGBAAstc10x10 = GL_COMPRESSED_RGBA_ASTC_10x10_KHR,

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
    CompressedSRGB8Alpha8Astc10x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,

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
    CompressedRGBAAstc12x10 = GL_COMPRESSED_RGBA_ASTC_12x10_KHR,

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
    CompressedSRGB8Alpha8Astc12x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,

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
    CompressedRGBAAstc12x12 = GL_COMPRESSED_RGBA_ASTC_12x12_KHR,

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
    CompressedSRGB8Alpha8Astc12x12 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
    #endif

    /**
     * Depth component, size implementation-dependent. Not supported in 3D
     * textures. Not allowed in unemulated @ref Texture::setStorage()
     * "*Texture::setStorage()" calls, in that case use e.g.
     * @ref TextureFormat::DepthComponent24 "TextureFormat::DepthComponent24" instead.
     * @requires_gles30 Extension @extension{OES,depth_texture} or
     *      @extension{ANGLE,depth_texture} in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{WEBGL,depth_texture} in
     *      WebGL 1.0.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref TextureFormat::DepthComponent24.
     */
    DepthComponent = GL_DEPTH_COMPONENT,

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Depth component, 16bit. Not supported in 3D textures.
     * @requires_gles30 Extension @extension{OES,depth_texture} or
     *      @extension{ANGLE,depth_texture} and either
     *      @extension{OES,required_internalformat} (for image
     *      specification) or @extension{EXT,texture_storage} (for texture
     *      storage) in OpenGL ES 2.0. Use @ref TextureFormat::DepthComponent
     *      otherwise.
     * @requires_webgl20 Not available in WebGL 1.0, use
     *      @ref TextureFormat::DepthComponent instead.
     */
    DepthComponent16 = GL_DEPTH_COMPONENT16,

    /**
     * Depth component, 24bit. Not supported in 3D textures.
     * @requires_gles30 Extension @extension{OES,required_internalformat},
     *      @extension{OES,depth_texture} and @extension{OES,depth24} in
     *      OpenGL ES 2.0. Use @ref TextureFormat::DepthComponent otherwise.
     * @requires_webgl20 Not available in WebGL 1.0, use
     *      @ref TextureFormat::DepthComponent instead.
     */
    #ifndef MAGNUM_TARGET_GLES2
    DepthComponent24 = GL_DEPTH_COMPONENT24,
    #else
    DepthComponent24 = GL_DEPTH_COMPONENT24_OES,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Depth component, 32bit. Not supported in 3D textures.
     * @requires_es_extension Extension @extension{OES,depth_texture} or
     *      @extension{ANGLE,depth_texture} and @extension{OES,depth32}
     *      and @extension{OES,required_internalformat} (for image
     *      specification) or @extension{EXT,texture_storage} (for texture
     *      storage) in OpenGL ES 2.0.
     * @requires_gles Not available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    DepthComponent32 = GL_DEPTH_COMPONENT32,
    #else
    DepthComponent32 = GL_DEPTH_COMPONENT32_OES,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Depth component, 32bit float. Not supported in 3D textures.
     * @requires_gl30 Extension @extension{ARB,depth_buffer_float}
     * @requires_gles30 Only integral depth textures are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only integral depth textures are available in WebGL
     *      1.0.
     */
    DepthComponent32F = GL_DEPTH_COMPONENT32F,
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Stencil index, 8bit. Not supported in 3D textures.
     * @requires_gl44 Extension @extension{ARB,texture_stencil8}
     * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
     *      @extension{OES,texture_stencil8}
     * @requires_gles Only available as renderbuffer format in WebGL.
     */
    StencilIndex8 = GL_STENCIL_INDEX8,
    #endif

    /**
     * Depth and stencil component, size implementation-dependent. Not
     * supported in 3D textures. Not allowed in unemulated
     * @ref Texture::setStorage() "*Texture::setStorage()" calls, in that case
     * use e.g. @ref TextureFormat::Depth24Stencil8 "TextureFormat::Depth24Stencil8"
     * instead.
     * @see @ref Texture::setDepthStencilMode() "*Texture::setDepthStencilMode()"
     * @requires_gles30 Extension @extension{OES,packed_depth_stencil} or
     *      @extension{ANGLE,depth_texture} in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{WEBGL,depth_texture} in
     *      WebGL 1.0.
     * @deprecated_gl Prefer to use exactly specified version of this format,
     *      e.g. @ref TextureFormat::Depth24Stencil8.
     */
    #ifndef MAGNUM_TARGET_GLES2
    DepthStencil = GL_DEPTH_STENCIL,
    #else
    /* Using OES version even though WebGL 1.0 *has* DEPTH_STENCIL constant,
       because there are no such headers for it */
    DepthStencil = GL_DEPTH_STENCIL_OES,
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * 24bit depth and 8bit stencil component. Not supported in 3D textures.
     * @see @ref Texture::setDepthStencilMode() "*Texture::setDepthStencilMode()"
     * @requires_gl30 Extension @extension{ARB,framebuffer_object}
     * @requires_gles30 Extension @extension{OES,packed_depth_stencil} or
     *      @extension{ANGLE,depth_texture} and either
     *      @extension{OES,required_internalformat} (for image
     *      specification) or @extension{EXT,texture_storage} (for texture
     *      storage) in OpenGL ES 2.0.
     * @requires_webgl20 Use @ref TextureFormat::DepthStencil in WebGL 1.0
     *      instead.
     */
    #ifdef MAGNUM_TARGET_GLES2
    Depth24Stencil8 = GL_DEPTH24_STENCIL8_OES
    #else
    Depth24Stencil8 = GL_DEPTH24_STENCIL8,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * 32bit float depth component and 8bit stencil component. Not supported in
     * 3D textures.
     * @see @ref Texture::setDepthStencilMode() "*Texture::setDepthStencilMode()"
     * @requires_gl30 Extension @extension{ARB,depth_buffer_float}
     * @requires_gles30 Only integral depth textures are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only integral depth textures are available in WebGL
     *      1.0.
     */
    Depth32FStencil8 = GL_DEPTH32F_STENCIL8
    #endif
};

}

#endif
