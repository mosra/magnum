#ifndef Magnum_GL_TextureFormat_h
#define Magnum_GL_TextureFormat_h
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
 * @brief Enum @ref Magnum::GL::TextureFormat
 */

#include "Magnum/Magnum.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/GL/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace GL {

/**
@brief Internal texture format

In most cases you may want to use @ref TextureFormat::R8 (for grayscale
textures), @ref TextureFormat::RGB8 or @ref TextureFormat::RGBA8. The matching
color format is then @ref PixelFormat::Red, @ref PixelFormat::RGB or
@ref PixelFormat::RGBA along with @ref PixelType::UnsignedByte. See
documentation of these values for possible limitations when using OpenGL ES 2.0
or WebGL.

@see @ref textureFormat(), @ref hasTextureFormat(), @ref Texture,
    @ref CubeMapTexture, @ref CubeMapTextureArray
@m_enum_values_as_keywords
*/
enum class TextureFormat: GLenum {
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Red component, normalized unsigned, size implementation-dependent. Not
     * allowed in unemulated @ref Texture::setStorage() "*Texture::setStorage()"
     * calls, in that case use @ref TextureFormat::R8 "TextureFormat::R8"
     * instead.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gles30 Extension @gl_extension{EXT,texture_rg} in OpenGL ES
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
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gles30 Extension @gl_extension{EXT,texture_rg} and
     *      @gl_extension{EXT,texture_storage} in OpenGL ES 2.0. For texture
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

    /**
     * Red and green component, normalized unsigned, size
     * implementation-dependent. Not allowed in unemulated
     * @ref Texture::setStorage() "*Texture::setStorage()" calls, in that case
     * use @ref TextureFormat::RG8 "TextureFormat::RG8" instead.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gles30 Extension @gl_extension{EXT,texture_rg} in OpenGL ES
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
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gles30 Extension @gl_extension{EXT,texture_rg} and
     *      @gl_extension{EXT,texture_storage} in OpenGL ES 2.0. For texture
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
     * RGB, type/size implementation-dependent. Not allowed in unemulated
     * @ref Texture::setStorage() "*Texture::setStorage()" calls, in
     * that case use @ref TextureFormat::RGB8 "TextureFormat::RGB8" instead. On
     * desktop OpenGL this format is meant to be used for
     * @ref PixelFormat::BGR as well.
     * @requires_gl Can't be used as a render target in OpenGL ES or WebGL. See
     *      @ref TextureFormat::RGBA for an alternative.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref TextureFormat::RGB8.
     * @requires_webgl20 Extension @webgl_extension{OES,texture_half_float_linear}
     *      for filtering @ref PixelType::Half textures using
     *      @ref SamplerFilter::Linear in WebGL 1.0.
     */
    RGB = GL_RGB,

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * RGB, each component normalized unsigned byte. On desktop OpenGL this
     * format is meant to be used for @ref PixelFormat::BGRA as well.
     * @requires_gles30 Extension @gl_extension{OES,required_internalformat}
     *      (for image specification) or @gl_extension{EXT,texture_storage}
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
     * RGBA, type/size implementation-dependent. Not allowed in unemulated
     * @ref Texture::setStorage() "*Texture::setStorage()" calls, in that case
     * use @ref TextureFormat::RGBA8 "TextureFormat::RGBA8" instead. On desktop
     * OpenGL this format is meant to be used for @ref PixelFormat::BGRA as
     * well, however on OpenGL ES you're required to use
     * @ref TextureFormat::BGRA for BGRA pixel formats.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref TextureFormat::RGBA8.
     * @requires_webgl20 Extension @webgl_extension{OES,texture_half_float_linear}
     *      for filtering @ref PixelType::Half textures using
     *      @ref SamplerFilter::Linear in WebGL 1.0.
     */
    RGBA = GL_RGBA,

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * RGBA, each component normalized unsigned byte. On desktop OpenGL this
     * format is meant to be used with @ref PixelFormat::BGRA as well, however
     * on OpenGL ES you're required to use @ref TextureFormat::BGRA8 for BGRA
     * pixel formats.
     * @requires_gles30 Extension @gl_extension{OES,required_internalformat}
     *      (for image specification) or @gl_extension{EXT,texture_storage}
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

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * sRGB-encoded red component, normalized unsigned byte.
     * @requires_extension Extension @gl_extension{EXT,texture_sRGB_R8}
     * @requires_es_extension Extension @gl_extension{EXT,texture_sRGB_R8}
     * @requires_gles One- and two-component sRGB texture formats are not
     *      available in WebGL, use @ref TextureFormat::SRGB8 or
     *      @ref TextureFormat::SRGB8Alpha8
     * @m_since{2019,10}
     */
    SR8 = GL_SR8_EXT,

    #if defined(MAGNUM_TARGET_GLES) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * sRGB-encoded red and green component, normalized unsigned byte.
     * @requires_es_extension Extension @gl_extension{EXT,texture_sRGB_RG8}
     * @requires_gles One- and two-component sRGB texture formats are not
     *      available in WebGL, use @ref TextureFormat::SRGB8 or
     *      @ref TextureFormat::SRGB8Alpha8 instead. Only
     *      @ref TextureFormat::SR8, @ref TextureFormat::SRGB8 or
     *      @ref TextureFormat::SRGB8Alpha8 is available in desktop OpenGL.
     * @m_since{2019,10}
     */
    SRG8 = GL_SRG8_EXT,
    #endif
    #endif

    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    /**
     * sRGB, normalized unsigned, size implementation-dependent. Not allowed in
     * unemulated @ref Texture::setStorage() "*Texture::setStorage()" calls, in
     * that case use @ref TextureFormat::SRGB8 "TextureFormat::SRGB8" instead.
     * @requires_es_extension Extension @gl_extension{EXT,sRGB} in OpenGL ES 2.0.
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

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * sRGB, each component normalized unsigned byte.
     * @requires_gles30 Use @ref TextureFormat::SRGB in OpenGL ES 2.0 instead.
     * @requires_gl Can't be used as render target in OpenGL ES. Use
     *      @ref TextureFormat::SRGB8Alpha8 instead.
     * @requires_webgl20 Use @ref TextureFormat::SRGB in WebGL 1.0 instead.
     */
    SRGB8 = GL_SRGB8, /* NV_sRGB_formats has this on ES2, but meh */
    #endif

    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    /**
     * sRGB + linear alpha, normalized unsigned, size implementation-dependent.
     * Not allowed in unemulated @ref Texture::setStorage() "*Texture::setStorage()"
     * calls, in that case use @ref TextureFormat::SRGB8Alpha8 "TextureFormat::SRGB8Alpha8"
     * instead.
     * @requires_es_extension Extension @gl_extension{EXT,sRGB} in OpenGL ES
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
     * @requires_gles30 Extension @gl_extension{EXT,sRGB} and
     *      @gl_extension{EXT,texture_storage}, only for
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

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red component, normalized signed byte.
     * @requires_gl31 Extension @gl_extension{EXT,texture_snorm}
     * @requires_gl Can't be used as render target in OpenGL ES. Use
     *      @ref TextureFormat::RGB8I instead.
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     * @requires_webgl20 Only unsigned formats are available in WebGL 1.0.
     */
    R8Snorm = GL_R8_SNORM,

    /**
     * Red and green component, each normalized signed byte.
     * @requires_gl31 Extension @gl_extension{EXT,texture_snorm}
     * @requires_gl Can't be used as render target in OpenGL ES. Use
     *      @ref TextureFormat::RGB8I instead.
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     * @requires_webgl20 Only unsigned formats are available in WebGL 1.0.
     */
    RG8Snorm = GL_RG8_SNORM,

    /**
     * RGB, each component normalized signed byte.
     * @requires_gl31 Extension @gl_extension{EXT,texture_snorm}
     * @requires_gl Can't be used as render target in OpenGL ES. Use
     *      @ref TextureFormat::RGB8I instead.
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     * @requires_webgl20 Only unsigned formats are available in WebGL 1.0.
     */
    RGB8Snorm = GL_RGB8_SNORM,

    /**
     * RGBA, each component normalized signed byte.
     * @requires_gl31 Extension @gl_extension{EXT,texture_snorm}
     * @requires_gl Can't be used as render target in OpenGL ES. Use
     *      @ref TextureFormat::RGB8I instead.
     * @requires_gles30 Only unsigned formats are available in OpenGL ES 2.0.
     * @requires_webgl20 Only unsigned formats are available in WebGL 1.0.
     */
    RGBA8Snorm = GL_RGBA8_SNORM,
    #endif

    /* Available everywhere except ES2 (WebGL 1 has it) */
    #if !(defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    /**
     * Red component, normalized unsigned short.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gles30 Only byte-sized normalized formats are available in
     *      OpenGL ES 2.0
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     * @requires_webgl_extension Extension @webgl_extension{EXT,texture_norm16}
     */
    #ifndef MAGNUM_TARGET_GLES
    R16 = GL_R16,
    #else
    R16 = GL_R16_EXT,
    #endif

    /**
     * Red and green component, each normalized unsigned short.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gles31 Only byte-sized normalized formats are available in
     *      OpenGL ES 3.0 and older; not defined on ES2
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     * @requires_webgl_extension Extension @webgl_extension{EXT,texture_norm16}
     */
    #ifndef MAGNUM_TARGET_GLES
    RG16 = GL_RG16,
    #else
    RG16 = GL_RG16_EXT,
    #endif

    /**
     * RGB, each component normalized unsigned short.
     * @requires_gles31 Only byte-sized normalized formats are available in
     *      OpenGL ES 3.0 and older; not defined on ES2
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     * @requires_webgl_extension Extension @webgl_extension{EXT,texture_norm16}
     */
    #ifndef MAGNUM_TARGET_GLES
    RGB16 = GL_RGB16,
    #else
    RGB16 = GL_RGB16_EXT,
    #endif

    /**
     * RGBA, each component normalized unsigned short.
     * @requires_gles31 Only byte-sized normalized formats are available in
     *      OpenGL ES 3.0 and older; not defined on ES2
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     * @requires_webgl_extension Extension @webgl_extension{EXT,texture_norm16}
     */
    #ifndef MAGNUM_TARGET_GLES
    RGBA16 = GL_RGBA16,
    #else
    RGBA16 = GL_RGBA16_EXT,
    #endif

    /**
     * Red component, normalized signed short.
     * @requires_gl31 Extension @gl_extension{EXT,texture_snorm}
     * @requires_gles31 Only byte-sized normalized formats are available in
     *      OpenGL ES 3.0 and older; not defined on ES2
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     * @requires_webgl_extension Extension @webgl_extension{EXT,texture_norm16}
     */
    #ifndef MAGNUM_TARGET_GLES
    R16Snorm = GL_R16_SNORM,
    #else
    R16Snorm = GL_R16_SNORM_EXT,
    #endif

    /**
     * Red and green component, each normalized signed short.
     * @requires_gl31 Extension @gl_extension{EXT,texture_snorm}
     * @requires_gles31 Only byte-sized normalized formats are available in
     *      OpenGL ES 3.0 and older; not defined on ES2
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     * @requires_webgl_extension Extension @webgl_extension{EXT,texture_norm16}
     */
    #ifndef MAGNUM_TARGET_GLES
    RG16Snorm = GL_RG16_SNORM,
    #else
    RG16Snorm = GL_RG16_SNORM_EXT,
    #endif

    /**
     * RGB, each component normalized signed short.
     * @requires_gl31 Extension @gl_extension{EXT,texture_snorm}
     * @requires_gles31 Only byte-sized normalized formats are available in
     *      OpenGL ES 3.0 and older; not defined on ES2
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     * @requires_webgl_extension Extension @webgl_extension{EXT,texture_norm16}
     */
    #ifndef MAGNUM_TARGET_GLES
    RGB16Snorm = GL_RGB16_SNORM,
    #else
    RGB16Snorm = GL_RGB16_SNORM_EXT,
    #endif

    /**
     * RGBA, each component normalized signed short.
     * @requires_gl31 Extension @gl_extension{EXT,texture_snorm}
     * @requires_gles31 Only byte-sized normalized formats are available in
     *      OpenGL ES 3.0 and older; not defined on ES2
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     * @requires_webgl_extension Extension @webgl_extension{EXT,texture_norm16}
     */
    #ifndef MAGNUM_TARGET_GLES
    RGBA16Snorm = GL_RGBA16_SNORM,
    #else
    RGBA16Snorm = GL_RGBA16_SNORM_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red component, non-normalized unsigned byte. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R8UI = GL_R8UI,

    /**
     * Red and green component, each non-normalized unsigned byte. Can't be
     * filtered using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG8UI = GL_RG8UI,

    /**
     * RGB, each component non-normalized unsigned byte. Can't be filtered
     * using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB8UI = GL_RGB8UI,

    /**
     * RGBA, each component non-normalized unsigned byte. Can't be filtered
     * using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA8UI = GL_RGBA8UI,

    /**
     * Red component, non-normalized signed byte. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R8I = GL_R8I,

    /**
     * Red and green component, each non-normalized signed byte. Can't be
     * filtered using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG8I = GL_RG8I,

    /**
     * RGB, each component non-normalized signed byte. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB8I = GL_RGB8I,

    /**
     * RGBA, each component non-normalized signed byte. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA8I = GL_RGBA8I,

    /**
     * Red component, non-normalized unsigned short. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R16UI = GL_R16UI,

    /**
     * Red and green component, each non-normalized unsigned short. Can't be
     * filtered using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG16UI = GL_RG16UI,

    /**
     * RGB, each component non-normalized unsigned short. Can't be filtered
     * using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB16UI = GL_RGB16UI,

    /**
     * RGBA, each component non-normalized unsigned short. Can't be filtered
     * using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA16UI = GL_RGBA16UI,

    /**
     * Red component, non-normalized signed short. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R16I = GL_R16I,

    /**
     * Red and green component, each non-normalized signed short. Can't be
     * filtered using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG16I = GL_RG16I,

    /**
     * RGB, each component non-normalized signed short. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB16I = GL_RGB16I,

    /**
     * RGBA, each component non-normalized signed short. Can't be filtered
     * using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA16I = GL_RGBA16I,

    /**
     * Red component, non-normalized unsigned int. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R32UI = GL_R32UI,

    /**
     * Red and green component, each non-normalized unsigned int. Can't be
     * filtered using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG32UI = GL_RG32UI,

    /**
     * RGB, each component non-normalized unsigned int. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB32UI = GL_RGB32UI,

    /**
     * RGBA, each component non-normalized unsigned int. Can't be filtered
     * using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA32UI = GL_RGBA32UI,

    /**
     * Red component, non-normalized signed int. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R32I = GL_R32I,

    /**
     * Red and green component, each non-normalized signed int. Can't be
     * filtered using @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RG32I = GL_RG32I,

    /**
     * RGB, each component non-normalized signed int. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGB32I = GL_RGB32I,

    /**
     * RGBA, each component non-normalized signed int. Can't be filtered using
     * @ref SamplerFilter::Linear.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    RGBA32I = GL_RGBA32I,

    /**
     * Red component, half float.
     * @see @ref Magnum::Half "Half", @ref Math::packHalf(),
     *      @ref Math::unpackHalf()
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{ARB,texture_float}
     * @requires_gles30 Use @ref TextureFormat::Red / @ref TextureFormat::Luminance
     *      in combination with @ref PixelType::Half
     *      (@gl_extension2{OES,texture_half_float,OES_texture_float}) in OpenGL
     *      ES 2.0 instead.
     * @requires_gles30 Extension @gl_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear in OpenGL ES 2.0.
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_half_float} to
     *      use as a render target. Use @ref TextureFormat::R16UI or
     *      @ref TextureFormat::R16I instead if not available.
     * @requires_webgl20 Use @ref TextureFormat::Red / @ref TextureFormat::Luminance
     *      in combination with @ref PixelType::Half (@webgl_extension{OES,texture_half_float})
     *      in WebGL 1.0.
     * @requires_webgl20 Extension @webgl_extension{OES,texture_half_float_linear}
     *      for filtering using @ref SamplerFilter::Linear in WebGL 1.0.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_float}
     *      to use as a render target in WebGL 2.0; use @ref TextureFormat::R16UI
     *      or @ref TextureFormat::R16I instead if not available. Only three-
     *      and four-component half-float texture formats can be used as
     *      a render target in WebGL 1.0, see @ref TextureFormat::RGBA16F for
     *      more information.
     */
    R16F = GL_R16F,

    /**
     * Red and green component, each half float.
     * @see @ref Magnum::Half "Half", @ref Math::packHalf(),
     *      @ref Math::unpackHalf()
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{ARB,texture_float}
     * @requires_gles30 Use @ref TextureFormat::RG / @ref TextureFormat::LuminanceAlpha
     *      in combination with @ref PixelType::Half
     *      (@gl_extension2{OES,texture_half_float,OES_texture_float}) in OpenGL
     *      ES 2.0 instead.
     * @requires_gles30 Extension @gl_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear in OpenGL ES 2.0.
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_half_float} to
     *      use as a render target in OpenGL ES. Use @ref TextureFormat::RG16UI
     *      or @ref TextureFormat::RG16I instead if not available.
     * @requires_webgl20 Use @ref TextureFormat::RG / @ref TextureFormat::LuminanceAlpha
     *      in combination with @ref PixelType::Half (@webgl_extension{OES,texture_half_float})
     *      in WebGL 1.0 instead.
     * @requires_webgl20 Extension @webgl_extension{OES,texture_half_float_linear}
     *      for filtering using @ref SamplerFilter::Linear in WebGL 1.0.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_float}
     *      to use as a render target in WebGL 2.0; use @ref TextureFormat::RG16UI
     *      or @ref TextureFormat::RG16I instead if not available. Only three-
     *      and four-component half-float texture formats can be used as
     *      a render target in WebGL 1.0, see @ref TextureFormat::RGBA16F for
     *      more information.
     */
    RG16F = GL_RG16F,

    /**
     * RGB, each component half float.
     * @see @ref Magnum::Half "Half", @ref Math::packHalf(),
     *      @ref Math::unpackHalf()
     * @requires_gl30 Extension @gl_extension{ARB,texture_float}
     * @requires_gles30 Use @ref TextureFormat::RGB in combination with
     *      @ref PixelType::Half (@gl_extension2{OES,texture_half_float,OES_texture_float})
     *      in OpenGL ES 2.0 instead.
     * @requires_gles30 Extension @gl_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear in OpenGL ES 2.0.
     * @requires_webgl20 Use @ref TextureFormat::RGB in combination with
     *      @ref PixelType::Half (@webgl_extension{OES,texture_half_float}) in
     *      WebGL 1.0 instead.
     * @requires_gl Can't be used as a render target in OpenGL ES or WebGL 2.0.
     *      Use @ref TextureFormat::RGBA16F instead. Use @ref TextureFormat::RGB
     *      in combination with @ref PixelType::Half
     *      (@webgl_extension{EXT,color_buffer_half_float}) in WebGL 1.0
     *      instead.
     */
    RGB16F = GL_RGB16F,

    /**
     * RGBA, each component half float.
     * @see @ref Magnum::Half "Half", @ref Math::packHalf(),
     *      @ref Math::unpackHalf()
     * @requires_gl30 Extension @gl_extension{ARB,texture_float}
     * @requires_gles30 Use @ref TextureFormat::RGBA in combination with
     *      @ref PixelType::Half (@gl_extension2{OES,texture_half_float,OES_texture_float})
     *      in OpenGL ES 2.0 instead.
     * @requires_gles30 Extension @gl_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear.
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_half_float} to
     *      use as a render target in OpenGL ES. Use @ref TextureFormat::RGBA16UI
     *      or @ref TextureFormat::RGBA16I instead if not available.
     * @requires_webgl20 Use @ref TextureFormat::RGBA in combination with
     *      @ref PixelType::Half (@webgl_extension{OES,texture_half_float}) in
     *      WebGL 1.0 instead.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_float}
     *      to use as a render target in WebGL 2.0. Use @ref TextureFormat::RGBA16UI
     *      or @ref TextureFormat::RGBA16I instead if not available. Use
     *      @ref TextureFormat::RGBA in combination with @ref PixelType::Half
     *      (@webgl_extension{EXT,color_buffer_half_float}) in WebGL 1.0 instead.
     */
    RGBA16F = GL_RGBA16F,

    /**
     * Red component, float.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{ARB,texture_float}
     * @requires_gles30 Use @ref TextureFormat::Red / @ref TextureFormat::Luminance
     *      in combination with @ref PixelType::Float (@gl_extension{OES,texture_float})
     *      in OpenGL ES 2.0 instead.
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_float} to use as
     *      a render target. Use @ref TextureFormat::R32UI or
     *      @ref TextureFormat::R32I instead if not available.
     * @requires_es_extension Extension @gl_extension{OES,texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear.
     * @requires_webgl20 Use @ref TextureFormat::Red / @ref TextureFormat::Luminance
     *      in combination with @ref PixelType::Float (@webgl_extension{OES,texture_float})
     *      in WebGL 1.0 instead.
     * @requires_webgl_extension Extension @webgl_extension{OES,texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_float}
     *      to use as a render target in WebGL 2.0; use @ref TextureFormat::R32UI
     *      or @ref TextureFormat::R32I instead if not available. Only three-
     *      and four-component half-float texture formats can be used as
     *      a render target in WebGL 1.0. See @ref TextureFormat::RGBA32F for
     *      more information.
     */
    R32F = GL_R32F,

    /**
     * Red and green component, each float.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg} and @gl_extension{ARB,texture_float}
     * @requires_gles30 Use @ref TextureFormat::RG / @ref TextureFormat::LuminanceAlpha
     *      in combination with @ref PixelType::Float (@gl_extension{OES,texture_float})
     *      in OpenGL ES 2.0 instead.
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_float} to use as
     *      a render target. Use @ref TextureFormat::RG32UI or
     *      @ref TextureFormat::RG32I instead if not available.
     * @requires_es_extension Extension @gl_extension{OES,texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear.
     * @requires_webgl20 Use @ref TextureFormat::RG / @ref TextureFormat::LuminanceAlpha
     *      in combination with @ref PixelType::Float (@webgl_extension{OES,texture_float})
     *      in WebGL 1.0 instead.
     * @requires_webgl_extension Extension @webgl_extension{OES,texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_float}
     *      to use as a render target in WebGL 2.0; use @ref TextureFormat::RG32UI
     *      or @ref TextureFormat::RG32I instead if not available. Only three-
     *      and four-component half-float texture formats can be used as
     *      a render target in WebGL 1.0. See @ref TextureFormat::RGBA32F for
     *      more information.
     */
    RG32F = GL_RG32F,

    /**
     * RGB, each component float.
     * @requires_gl30 Extension @gl_extension{ARB,texture_float}
     * @requires_gles30 Use @ref TextureFormat::RGB in combination with
     *      @ref PixelType::Float (@gl_extension{OES,texture_float}) in OpenGL
     *      ES 2.0 instead.
     * @requires_es_extension Extension @gl_extension{OES,texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear.
     * @requires_webgl20 Use @ref TextureFormat::RGB in combination with
     *      @ref PixelType::Float (@webgl_extension{OES,texture_float}) in
     *      WebGL 1.0 instead.
     * @requires_webgl_extension Extension @webgl_extension{OES,texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear.
     * @requires_gl Can't be used as a render target in OpenGL ES or WebGL 2.0.
     *      Use @ref TextureFormat::RGBA32F instead. Use @ref TextureFormat::RGB
     *      in combination with @ref PixelType::Float
     *      (@webgl_extension{WEBGL,color_buffer_float}) in WebGL 1.0 instead.
     */
    RGB32F = GL_RGB32F,

    /**
     * RGBA, each component float.
     * @requires_gl30 Extension @gl_extension{ARB,texture_float}
     * @requires_gles30 Use @ref TextureFormat::RGBA in combination with
     *      @ref PixelType::Float (@gl_extension{OES,texture_float}) in OpenGL
     *      ES 2.0 instead.
     * @requires_es_extension Extension @gl_extension{OES,texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear.
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_float} to
     *      use as a render target in OpenGL ES. Use @ref TextureFormat::RGBA32UI
     *      or @ref TextureFormat::RGBA32I instead if not available.
     * @requires_webgl20 Use @ref TextureFormat::RGBA in combination with
     *      @ref PixelType::Float (@webgl_extension{OES,texture_float}) in
     *      WebGL 1.0 instead.
     * @requires_webgl_extension Extension @webgl_extension{OES,texture_float_linear}
     *      for filtering using @ref SamplerFilter::Linear.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_float}
     *      to use as a render target in WebGL 2.0. Use @ref TextureFormat::RGBA32UI
     *      or @ref TextureFormat::RGBA32I instead if not available. Use
     *      @ref TextureFormat::RGBA in combination with @ref PixelType::Float
     *      (@webgl_extension{WEBGL,color_buffer_float}) in WebGL 1.0 instead.
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
    R3G3B2 = GL_R3_G3_B2,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /** RGB, normalized unsigned, red and green component 3bit, blue 2bit.
     * @m_deprecated_since{2019,10} This one had a misleading typo in the name,
     *      use @ref TextureFormat::R3G3B2 instead.
     */
    R3B3G2 CORRADE_DEPRECATED_ENUM("use R3G3B2 instead") = R3G3B2,
    #endif

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
     * @requires_gles30 Extension @gl_extension{OES,required_internalformat}
     *      (for image specification) or @gl_extension{EXT,texture_storage}
     *      (for texture storage) in OpenGL ES 2.0.
     * @requires_webgl20 Not available in WebGL 1.0, see @ref TextureFormat::RGB
     *      for an alternative.
     */
    RGB565 = GL_RGB565,

    #if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    /**
     * RGB, each component normalized unsigned 10bit.
     * @requires_es_extension Extension @gl_extension{EXT,texture_type_2_10_10_10_REV}
     *      and either @gl_extension{OES,required_internalformat} (for image
     *      specification) or @gl_extension{EXT,texture_storage} (for texture
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
     * @requires_gl30 Extension @gl_extension{EXT,packed_float}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats are available in
     *      WebGL 1.0.
     */
    R11FG11FB10F = GL_R11F_G11F_B10F,

    /**
     * RGB, unsigned with exponent, each RGB component 9bit, exponent 5bit.
     * @requires_gl30 Extension @gl_extension{EXT,texture_shared_exponent}
     * @requires_gles30 Use @ref TextureFormat::RGB in OpenGL ES 2.0 instead.
     * @requires_webgl20 Use @ref TextureFormat::RGB in WebGL 1.0 instead.
     */
    RGB9E5 = GL_RGB9_E5,
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
     * @requires_gles30 Extension @gl_extension{OES,required_internalformat}
     *      (for image specification) or @gl_extension{EXT,texture_storage}
     *      (for texture storage) in OpenGL ES 2.0. Use @ref TextureFormat::RGBA
     *      otherwise.
     * @requires_webgl20 Not available in WebGL 1.0, use @ref TextureFormat::RGBA
     *      instead.
     */
    RGBA4 = GL_RGBA4,

    /**
     * RGBA, normalized unsigned, each RGB component 5bit, alpha 1bit.
     * @requires_gles30 Extension @gl_extension{OES,required_internalformat}
     *      (for image specification) or @gl_extension{EXT,texture_storage}
     *      (for texture storage) in OpenGL ES 2.0. Use @ref TextureFormat::RGBA
     *      otherwise.
     * @requires_webgl20 Not available in WebGL 1.0, use @ref TextureFormat::RGBA
     *      instead.
     */
    RGB5A1 = GL_RGB5_A1,

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * RGBA, normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gles30 Extension @gl_extension{EXT,texture_type_2_10_10_10_REV}
     *      and either @gl_extension{OES,required_internalformat} (for image
     *      specification) or @gl_extension{EXT,texture_storage} (for texture
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
     * Can't be filtered using @ref SamplerFilter::Linear.
     * @requires_gl33 Extension @gl_extension{ARB,texture_rgb10_a2ui}
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

    #if defined(DOXYGEN_GENERATING_OUTPUT) || (defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL))
    /**
     * BGRA, type/size implementation-dependent. Not allowed in unemulated
     * @ref Texture::setStorage() "*Texture::setStorage()" calls, in that case
     * use @ref TextureFormat::RGBA8 "TextureFormat::RGBA8" instead.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref TextureFormat::BGRA8.
     * @requires_gles Not available on desktop OpenGL -- there it's perfectly
     *      legal to use @ref TextureFormat::RGBA in combination with
     *      @ref PixelFormat::BGRA.
     * @requires_es_extension Extension @gl_extension{APPLE,texture_format_BGRA8888}
     *      or @gl_extension{EXT,texture_format_BGRA8888}
     * @requires_gles Only RGBA component ordering is available in WebGL.
     * @m_since{2019,10}
     */
    BGRA = GL_BGRA_EXT,

    /**
     * BGRA, each component normalized unsigned byte.
     * @requires_gles Not available on desktop OpenGL -- there it's perfectly
     *      legal to use @ref TextureFormat::RGBA8 in combination with
     *      @ref PixelFormat::BGRA.
     * @requires_es_extension Extension @gl_extension{APPLE,texture_format_BGRA8888}. There are no signed
     *      normalized, integral or floating-point types with BGRA ordering.
     * @requires_gles Only RGBA component ordering is available in WebGL.
     * @m_since{2019,10}
     */
    BGRA8 = GL_BGRA8_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Compressed red channel, normalized unsigned. **Not available on
     * multisample textures.**
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gl Generic texture compression is not available in OpenGL ES
     *      or WebGL.
     */
    CompressedRed = GL_COMPRESSED_RED,

    /**
     * Compressed red and green channel, normalized unsigned. **Not available
     * on multisample textures.**
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
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
    CompressedRedRgtc1 = GL_COMPRESSED_RED_RGTC1,
    #else
    CompressedRedRgtc1 = GL_COMPRESSED_RED_RGTC1_EXT,
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
    CompressedRGRgtc2 = GL_COMPRESSED_RG_RGTC2,
    #else
    CompressedRGRgtc2 = GL_COMPRESSED_RED_GREEN_RGTC2_EXT, /*?!*/
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
    CompressedSignedRedRgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
    #else
    CompressedSignedRedRgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,
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
    CompressedSignedRGRgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
    #else
    CompressedSignedRGRgtc2 = GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT, /*?!*/
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
    CompressedRGBBptcUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
    #else
    CompressedRGBBptcUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT,
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
    CompressedRGBBptcSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
    #else
    CompressedRGBBptcSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT,
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
    CompressedRGBABptcUnorm = GL_COMPRESSED_RGBA_BPTC_UNORM,
    #else
    CompressedRGBABptcUnorm = GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,
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
    CompressedSRGBAlphaBptcUnorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
    #else
    CompressedSRGBAlphaBptcUnorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT,
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
    CompressedRGB8Etc2 = GL_COMPRESSED_RGB8_ETC2,

    /**
     * ETC2 compressed sRGB, normalized unsigned. **Available only on 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedSRGB8Etc2 = GL_COMPRESSED_SRGB8_ETC2,

    /**
     * ETC2 compressed RGB with punchthrough (single-bit) alpha, normalized
     * unsigned. **Available only on 2D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedRGB8PunchthroughAlpha1Etc2 = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,

    /**
     * ETC2 compressed sRGB with punchthrough (single-bit) alpha, normalized
     * unsigned. **Available only on 2D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedSRGB8PunchthroughAlpha1Etc2 = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,

    /**
     * ETC2/EAC compressed RGBA, normalized unsigned. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedRGBA8Etc2Eac = GL_COMPRESSED_RGBA8_ETC2_EAC,

    /**
     * ETC2/EAC compressed sRGB with alpha, normalized unsigned. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedSRGB8Alpha8Etc2Eac = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,

    /**
     * EAC compressed red channel, normalized unsigned. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedR11Eac = GL_COMPRESSED_R11_EAC,

    /**
     * EAC compressed red channel, normalized signed. **Available only on 2D,
     * 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedSignedR11Eac = GL_COMPRESSED_SIGNED_R11_EAC,

    /**
     * EAC compressed red and green channel, normalized unsigned. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedRG11Eac = GL_COMPRESSED_RG11_EAC,

    /**
     * EAC compressed red and green channel, normalized signed. **Available
     * only on 2D, 2D array, cube map and cube map array textures.**
     * @requires_gl43 Extension @gl_extension{ARB,ES3_compatibility}
     * @requires_gles30 ETC2 texture compression is not available in OpenGL ES
     *      2.0.
     */
    CompressedSignedRG11Eac = GL_COMPRESSED_SIGNED_RG11_EAC,
    #endif

    /**
     * S3TC DXT1 compressed RGB. **Available only on 2D, 2D array, cube map and
     * cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     *      or @gl_extension{EXT,texture_compression_dxt1}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc},
     *      @gl_extension{EXT,texture_compression_dxt1} or
     *      @gl_extension2{ANGLE,texture_compression_dxt1,ANGLE_texture_compression_dxt}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    CompressedRGBS3tcDxt1 = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,

    /**
     * S3TC DXT1 compressed sRGB. **Available only for 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc_srgb}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc_srgb}
     */
    CompressedSRGBS3tcDxt1 = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,

    /**
     * S3TC DXT1 compressed RGBA. **Available only on 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     *      or @gl_extension{EXT,texture_compression_dxt1}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc},
     *      @gl_extension{EXT,texture_compression_dxt1} or
     *      @gl_extension2{ANGLE,texture_compression_dxt1,ANGLE_texture_compression_dxt}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    CompressedRGBAS3tcDxt1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,

    /**
     * S3TC DXT1 compressed sRGB + linear alpha. **Available only for 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc_srgb}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc_srgb}
     */
    CompressedSRGBAlphaS3tcDxt1 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,

    /**
     * S3TC DXT3 compressed RGBA. **Available only on 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     *      or @gl_extension2{ANGLE,texture_compression_dxt3,ANGLE_texture_compression_dxt}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    CompressedRGBAS3tcDxt3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,

    /**
     * S3TC DXT3 compressed sRGB + linear alpha. **Available only for 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc_srgb}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc_srgb}
     */
    CompressedSRGBAlphaS3tcDxt3 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,

    /**
     * S3TC DXT5 compressed RGBA. **Available only on 2D, 2D array, cube map
     * and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     *      or @gl_extension2{ANGLE,texture_compression_dxt5,ANGLE_texture_compression_dxt}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc}
     */
    CompressedRGBAS3tcDxt5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,

    /**
     * S3TC DXT5 compressed sRGB + linear alpha. **Available only for 2D, 2D
     * array, cube map and cube map array textures.**
     * @requires_extension Extension @gl_extension{EXT,texture_compression_s3tc}
     * @requires_es_extension Extension @gl_extension{EXT,texture_compression_s3tc_srgb}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_s3tc_srgb}
     */
    CompressedSRGBAlphaS3tcDxt5 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,

    /**
     * ASTC compressed RGBA with 4x4 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc4x4 = GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
    #else
    CompressedRGBAAstc4x4 = GL_COMPRESSED_RGBA_ASTC_4x4,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 4x4 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
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
    CompressedSRGB8Alpha8Astc4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
    #else
    CompressedSRGB8Alpha8Astc4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4,
    #endif

    /**
     * ASTC compressed RGBA with 5x4 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc5x4 = GL_COMPRESSED_RGBA_ASTC_5x4_KHR,
    #else
    CompressedRGBAAstc5x4 = GL_COMPRESSED_RGBA_ASTC_5x4,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 5x4 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
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
    CompressedSRGB8Alpha8Astc5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
    #else
    CompressedSRGB8Alpha8Astc5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4,
    #endif

    /**
     * ASTC compressed RGBA with 5x5 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc5x5 = GL_COMPRESSED_RGBA_ASTC_5x5_KHR,
    #else
    CompressedRGBAAstc5x5 = GL_COMPRESSED_RGBA_ASTC_5x5,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 5x5 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
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
    CompressedSRGB8Alpha8Astc5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
    #else
    CompressedSRGB8Alpha8Astc5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5,
    #endif

    /**
     * ASTC compressed RGBA with 6x5 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc6x5 = GL_COMPRESSED_RGBA_ASTC_6x5_KHR,
    #else
    CompressedRGBAAstc6x5 = GL_COMPRESSED_RGBA_ASTC_6x5,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 6x5 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
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
    CompressedSRGB8Alpha8Astc6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
    #else
    CompressedSRGB8Alpha8Astc6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5,
    #endif

    /**
     * ASTC compressed RGBA with 6x6 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc6x6 = GL_COMPRESSED_RGBA_ASTC_6x6_KHR,
    #else
    CompressedRGBAAstc6x6 = GL_COMPRESSED_RGBA_ASTC_6x6,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 6x6 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
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
    CompressedSRGB8Alpha8Astc6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
    #else
    CompressedSRGB8Alpha8Astc6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6,
    #endif

    /**
     * ASTC compressed RGBA with 8x5 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc8x5 = GL_COMPRESSED_RGBA_ASTC_8x5_KHR,
    #else
    CompressedRGBAAstc8x5 = GL_COMPRESSED_RGBA_ASTC_8x5,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 8x5 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
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
    CompressedSRGB8Alpha8Astc8x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
    #else
    CompressedSRGB8Alpha8Astc8x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5,
    #endif

    /**
     * ASTC compressed RGBA with 8x6 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc8x6 = GL_COMPRESSED_RGBA_ASTC_8x6_KHR,
    #else
    CompressedRGBAAstc8x6 = GL_COMPRESSED_RGBA_ASTC_8x6,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 8x6 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
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
    CompressedSRGB8Alpha8Astc8x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,
    #else
    CompressedSRGB8Alpha8Astc8x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6,
    #endif

    /**
     * ASTC compressed RGBA with 8x8 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc8x8 = GL_COMPRESSED_RGBA_ASTC_8x8_KHR,
    #else
    CompressedRGBAAstc8x8 = GL_COMPRESSED_RGBA_ASTC_8x8,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 8x8 blocks. **Available only on 2D,
     * 3D, 2D array, cube map and cube map array textures.**
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
    CompressedSRGB8Alpha8Astc8x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
    #else
    CompressedSRGB8Alpha8Astc8x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8,
    #endif

    /**
     * ASTC compressed RGBA with 10x5 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc10x5 = GL_COMPRESSED_RGBA_ASTC_10x5_KHR,
    #else
    CompressedRGBAAstc10x5 = GL_COMPRESSED_RGBA_ASTC_10x5,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 10x5 blocks. **Available only on
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
    CompressedSRGB8Alpha8Astc10x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
    #else
    CompressedSRGB8Alpha8Astc10x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5,
    #endif

    /**
     * ASTC compressed RGBA with 10x6 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc10x6 = GL_COMPRESSED_RGBA_ASTC_10x6_KHR,
    #else
    CompressedRGBAAstc10x6 = GL_COMPRESSED_RGBA_ASTC_10x6,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 10x6 blocks. **Available only on
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
    CompressedSRGB8Alpha8Astc10x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,
    #else
    CompressedSRGB8Alpha8Astc10x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6,
    #endif

    /**
     * ASTC compressed RGBA with 10x8 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc10x8 = GL_COMPRESSED_RGBA_ASTC_10x8_KHR,
    #else
    CompressedRGBAAstc10x8 = GL_COMPRESSED_RGBA_ASTC_10x8,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 10x8 blocks. **Available only on
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
    CompressedSRGB8Alpha8Astc10x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,
    #else
    CompressedSRGB8Alpha8Astc10x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8,
    #endif

    /**
     * ASTC compressed RGBA with 10x10 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc10x10 = GL_COMPRESSED_RGBA_ASTC_10x10_KHR,
    #else
    CompressedRGBAAstc10x10 = GL_COMPRESSED_RGBA_ASTC_10x10,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 10x10 blocks. **Available only on
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
    CompressedSRGB8Alpha8Astc10x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,
    #else
    CompressedSRGB8Alpha8Astc10x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10,
    #endif

    /**
     * ASTC compressed RGBA with 12x10 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc12x10 = GL_COMPRESSED_RGBA_ASTC_12x10_KHR,
    #else
    CompressedRGBAAstc12x10 = GL_COMPRESSED_RGBA_ASTC_12x10,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 12x10 blocks. **Available only on
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
    CompressedSRGB8Alpha8Astc12x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,
    #else
    CompressedSRGB8Alpha8Astc12x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10,
    #endif

    /**
     * ASTC compressed RGBA with 12x12 blocks. **Available only on 2D, 3D, 2D
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
    CompressedRGBAAstc12x12 = GL_COMPRESSED_RGBA_ASTC_12x12_KHR,
    #else
    CompressedRGBAAstc12x12 = GL_COMPRESSED_RGBA_ASTC_12x12,
    #endif

    /**
     * ASTC compressed sRGB with alpha with 12x12 blocks. **Available only on
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
    CompressedSRGB8Alpha8Astc12x12 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
    #else
    CompressedSRGB8Alpha8Astc12x12 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12,
    #endif

    #if defined(DOXYGEN_GENERATING_OUTPUT) || (defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    /**
     * 3D ASTC compressed RGBA with 3x3x3 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedRGBAAstc3x3x3 = GL_COMPRESSED_RGBA_ASTC_3x3x3_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 3x3x3 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedSRGB8Alpha8Astc3x3x3 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES,

    /**
     * 3D ASTC compressed RGBA with 4x3x3 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedRGBAAstc4x3x3 = GL_COMPRESSED_RGBA_ASTC_4x3x3_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 4x3x3 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedSRGB8Alpha8Astc4x3x3 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES,

    /**
     * 3D ASTC compressed RGBA with 4x4x3 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedRGBAAstc4x4x3 = GL_COMPRESSED_RGBA_ASTC_4x4x3_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 4x4x3 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedSRGB8Alpha8Astc4x4x3 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES,

    /**
     * 3D ASTC compressed RGBA with 4x4x4 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedRGBAAstc4x4x4 = GL_COMPRESSED_RGBA_ASTC_4x4x4_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 4x4x4 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedSRGB8Alpha8Astc4x4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES,

    /**
     * 3D ASTC compressed RGBA with 5x4x4 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedRGBAAstc5x4x4 = GL_COMPRESSED_RGBA_ASTC_5x4x4_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 5x4x4 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedSRGB8Alpha8Astc5x4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES,

    /**
     * 3D ASTC compressed RGBA with 5x5x4 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedRGBAAstc5x5x4 = GL_COMPRESSED_RGBA_ASTC_5x5x4_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 5x5x4 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedSRGB8Alpha8Astc5x5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES,

    /**
     * 3D ASTC compressed RGBA with 5x5x5 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedRGBAAstc5x5x5 = GL_COMPRESSED_RGBA_ASTC_5x5x5_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 5x5x5 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedSRGB8Alpha8Astc5x5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES,

    /**
     * 3D ASTC compressed RGBA with 6x5x5 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedRGBAAstc6x5x5 = GL_COMPRESSED_RGBA_ASTC_6x5x5_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 6x5x5 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedSRGB8Alpha8Astc6x5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES,

    /**
     * 3D ASTC compressed RGBA with 6x6x5 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedRGBAAstc6x6x5 = GL_COMPRESSED_RGBA_ASTC_6x6x5_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 6x6x5 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedSRGB8Alpha8Astc6x6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES,

    /**
     * 3D ASTC compressed RGBA with 6x6x6 blocks. **Available only on 3D
     * textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedRGBAAstc6x6x6 = GL_COMPRESSED_RGBA_ASTC_6x6x6_OES,

    /**
     * 3D ASTC compressed sRGB with alpha with 6x6x6 blocks. **Available only
     * on 3D textures.**
     *
     * @requires_gles30 Not defined on desktop OpenGL, WebGL or OpenGL ES 2.0.
     * @requires_es_extension Extension @gl_extension{OES,texture_compression_astc}
     */
    CompressedSRGB8Alpha8Astc6x6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES,
    #endif

    #if defined(DOXYGEN_GENERATING_OUTPUT) || defined(MAGNUM_TARGET_GLES)
    /**
     * PVRTC compressed RGB, normalized unsigned byte with 2 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL.
     * @requires_es_extension Extension @gl_extension{IMG,texture_compression_pvrtc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_pvrtc}
     */
    CompressedRGBPvrtc2bppV1 = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * PVRTC compressed sRGB, normalized unsigned byte with 2 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL or WebGL.
     * @requires_es_extension Extension @gl_extension{EXT,pvrtc_sRGB}
     */
    CompressedSRGBPvrtc2bppV1 = GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT,
    #endif

    /**
     * PVRTC compressed RGBA, normalized unsigned byte with 2 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL.
     * @requires_es_extension Extension @gl_extension{IMG,texture_compression_pvrtc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_pvrtc}
     */
    CompressedRGBAPvrtc2bppV1 = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * PVRTC compressed sRGB + linear alpha, normalized unsigned byte with 2
     * bits per pixel. **Available only on 2D, 3D, 2D array, cube map and cube
     * map array textures.**
     * @requires_gles Not available on desktop OpenGL or WebGL.
     * @requires_es_extension Extension @gl_extension{EXT,pvrtc_sRGB}
     */
    CompressedSRGBAlphaPvrtc2bppV1 = GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT,
    #endif

    /**
     * PVRTC compressed RGB, normalized unsigned byte with 4 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL.
     * @requires_es_extension Extension @gl_extension{IMG,texture_compression_pvrtc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_pvrtc}
     */
    CompressedRGBPvrtc4bppV1 = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * PVRTC compressed sRGB, normalized unsigned byte with 4 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL or WebGL.
     * @requires_es_extension Extension @gl_extension{EXT,pvrtc_sRGB}
     */
    CompressedSRGBPvrtc4bppV1 = GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT,
    #endif

    /**
     * PVRTC compressed RGBA, normalized unsigned byte with 4 bits per pixel.
     * **Available only on 2D, 3D, 2D array, cube map and cube map array
     * textures.**
     * @requires_gles Not available on desktop OpenGL.
     * @requires_es_extension Extension @gl_extension{IMG,texture_compression_pvrtc}
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,compressed_texture_pvrtc}
     */
    CompressedRGBAPvrtc4bppV1 = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * PVRTC compressed sRGB + linear alpha, normalized unsigned byte with 4
     * bits per pixel. **Available only on 2D, 3D, 2D array, cube map and cube
     * map array textures.**
     * @requires_gles Not available on desktop OpenGL or WebGL.
     * @requires_es_extension Extension @gl_extension{EXT,pvrtc_sRGB}
     */
    CompressedSRGBAlphaPvrtc4bppV1 = GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT,
    #endif

    /* PVRTC2 variants not listed as PVRTC is mainly on Apple hardware but
       Metal doesn't support it and it doesn't have a WebGL equiv either. */
    #endif

    /**
     * Depth component, size implementation-dependent. Not supported in 3D
     * textures. Not allowed in unemulated @ref Texture::setStorage()
     * "*Texture::setStorage()" calls, in that case use e.g.
     * @ref TextureFormat::DepthComponent24 "TextureFormat::DepthComponent24" instead.
     * @requires_gles30 Extension @gl_extension{OES,depth_texture} or
     *      @gl_extension{ANGLE,depth_texture} in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{WEBGL,depth_texture} in
     *      WebGL 1.0.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref TextureFormat::DepthComponent24.
     */
    DepthComponent = GL_DEPTH_COMPONENT,

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Depth component, 16bit. Not supported in 3D textures.
     * @requires_gles30 Extension @gl_extension{OES,depth_texture} or
     *      @gl_extension{ANGLE,depth_texture} and either
     *      @gl_extension{OES,required_internalformat} (for image
     *      specification) or @gl_extension{EXT,texture_storage} (for texture
     *      storage) in OpenGL ES 2.0. Use @ref TextureFormat::DepthComponent
     *      otherwise.
     * @requires_webgl20 Not available in WebGL 1.0, use
     *      @ref TextureFormat::DepthComponent instead.
     */
    DepthComponent16 = GL_DEPTH_COMPONENT16,

    /**
     * Depth component, 24bit. Not supported in 3D textures.
     * @requires_gles30 Extension @gl_extension{OES,required_internalformat},
     *      @gl_extension{OES,depth_texture} and @gl_extension{OES,depth24} in
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
     * @requires_es_extension Extension @gl_extension{OES,depth_texture} or
     *      @gl_extension{ANGLE,depth_texture} and @gl_extension{OES,depth32}
     *      and @gl_extension{OES,required_internalformat} (for image
     *      specification) or @gl_extension{EXT,texture_storage} (for texture
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
     * @requires_gl30 Extension @gl_extension{ARB,depth_buffer_float}
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
     * @requires_gl44 Extension @gl_extension{ARB,texture_stencil8}
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension{OES,texture_stencil8}
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
     * @requires_gles30 Extension @gl_extension{OES,packed_depth_stencil} or
     *      @gl_extension{ANGLE,depth_texture} in OpenGL ES 2.0.
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
     * @requires_gl30 Extension @gl_extension{ARB,framebuffer_object}
     * @requires_gles30 Extension @gl_extension{OES,packed_depth_stencil} or
     *      @gl_extension{ANGLE,depth_texture} and either
     *      @gl_extension{OES,required_internalformat} (for image
     *      specification) or @gl_extension{EXT,texture_storage} (for texture
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
     * @requires_gl30 Extension @gl_extension{ARB,depth_buffer_float}
     * @requires_gles30 Only integral depth textures are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only integral depth textures are available in WebGL
     *      1.0.
     */
    Depth32FStencil8 = GL_DEPTH32F_STENCIL8
    #endif
};

/**
@brief Check availability of a sized generic texture format
@m_since{2019,10}

Some OpenGL targets don't support sized texture formats at all (OpenGL ES 2.0
and WebGL 1.0), some targets miss some variants (for example OpenGL ES doesn't
have any 8-bit packed formats) Returns @cpp false @ce if current target can't
support such format, @cpp true @ce otherwise. Expects
@ref isPixelFormatImplementationSpecific() returns @cpp false @ce for given
@p format as OpenGL pixel format enum values usually can't be used to specify
internal texture format as well.

For OpenGL ES 2.0 (and WebGL 1.0) targets in particular the mapping is very
complex and instead of using this function it's recommend to do the mapping
manually based on whether @ref Texture::setImage() or
@ref Texture::setStorage() is used and whether @gl_extension{EXT,texture_storage}
and other format-specific extensions are supported.

@note Support of some formats depends on presence of a particular OpenGL
    extension. Such check is outside of the scope of this function and you are
    expected to verify extension availability before using such format.

@see @ref textureFormat(), @ref hasPixelFormat(),
    @ref hasCompressedPixelFormat()
*/
MAGNUM_GL_EXPORT bool hasTextureFormat(Magnum::PixelFormat format);

/**
@brief Convert a generic pixel format to sized OpenGL texture format
@m_since{2019,10}

Not all sized texture formats may be available on all targets and this function
expects that given format is available on the target. See @ref hasTextureFormat()
for more information about checking availability of given format. Expects
@ref isPixelFormatImplementationSpecific() returns @cpp false @ce for given
@p format as OpenGL pixel format enum values usually can't be used to specify
internal texture format as well.

For OpenGL ES 2.0 (and WebGL 1.0) targets in particular the mapping is very
complex and instead of using this function it's recommend to do the mapping
manually based on whether @ref Texture::setImage() or
@ref Texture::setStorage() is used and whether @gl_extension{EXT,texture_storage}
and other format-specific extensions are supported.

@see @ref pixelType(), @ref compressedPixelFormat()
*/
MAGNUM_GL_EXPORT TextureFormat textureFormat(Magnum::PixelFormat format);

/**
@brief Check availability of a generic compressed texture format
@m_since{2019,10}

Some OpenGL targets don't support all generic pixel formats (for example PVRTC
compression might not be available on desktop OpenGL). Returns @cpp false @ce
if current target can't support such format, @cpp true @ce otherwise. Moreover,
returns @cpp true @ce also for all formats that are
@ref isCompressedPixelFormatImplementationSpecific(). The @p format value is
expected to be valid. This is different from
@ref hasTextureFormat(Magnum::PixelFormat), where the mapping of an
implementation-specific pixel format to an OpenGL texture format can't be
performed.

@note Support of some formats depends on presence of a particular OpenGL
    extension. Such check is outside of the scope of this function and you are
    expected to verify extension availability before using such format.

@see @ref textureFormat(), @ref hasCompressedPixelFormat(),
    @ref hasPixelFormat()
*/
MAGNUM_GL_EXPORT bool hasTextureFormat(Magnum::CompressedPixelFormat format);

/**
@brief Convert generic compressed pixel format to OpenGL texture format
@m_since{2019,10}

In case @ref isCompressedPixelFormatImplementationSpecific() returns
@cpp false @ce for @p format, maps it to a corresponding OpenGL pixel format.
In case @ref isCompressedPixelFormatImplementationSpecific() returns
@cpp true @ce, assumes @p format stores OpenGL-specific pixel format and
returns @ref compressedPixelFormatUnwrap() cast to @ref GL::TextureFormat. This
is different from @ref textureFormat(Magnum::PixelFormat), where the mapping of
an implementation-specific pixel format to an OpenGL texture format can't be
performed.

Not all generic pixel formats may be available on all targets and this function
expects that given format is available on the target. Use @ref hasTextureFormat()
to query availability of given format.
@see @ref compressedPixelFormat(), @ref pixelFormat()
*/
MAGNUM_GL_EXPORT TextureFormat textureFormat(Magnum::CompressedPixelFormat format);

/**
@debugoperatorenum{TextureFormat}
@m_since{2019,10}
*/
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, TextureFormat value);

}}

#endif
