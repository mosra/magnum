#ifndef Magnum_GL_RenderbufferFormat_h
#define Magnum_GL_RenderbufferFormat_h
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
 * @brief Enum @ref Magnum::GL::RenderbufferFormat
 */

#include "Magnum/GL/OpenGL.h"

namespace Magnum { namespace GL {

/**
@brief Internal renderbuffer format

@see @ref Renderbuffer
@m_enum_values_as_keywords
@requires_gl30 Extension @gl_extension{ARB,framebuffer_object}
@todo RGB, RGB8 ES only (ES3 + @gl_extension{OES,rgb8_rgba8})
 */
enum class RenderbufferFormat: GLenum {
    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, normalized unsigned, size implementation-dependent.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gl Use exactly specified format in OpenGL ES or WebGL instead.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref RenderbufferFormat::R8.
     */
    Red = GL_RED,
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Red component, normalized unsigned byte.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gles30 Extension @gl_extension{EXT,texture_rg} in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only three- and four-component formats (such as
     *      @ref RenderbufferFormat::RGB565 or @ref RenderbufferFormat::RGBA4)
     *      are available in WebGL 1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    R8 = GL_R8,
    #else
    R8 = GL_R8_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red and green component, normalized unsigned, size
     * implementation-dependent.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gl Use exactly specified format in OpenGL ES or WebGL instead.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref RenderbufferFormat::RG8.
     */
    RG = GL_RG,
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Red and green component, each normalized unsigned byte.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gles30 Extension @gl_extension{EXT,texture_rg} in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only three- and four-component formats (such as
     *      @ref RenderbufferFormat::RGB565 or @ref RenderbufferFormat::RGBA4)
     *      are available in WebGL 1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RG8 = GL_RG8,
    #else
    RG8 = GL_RG8_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGBA, normalized unsigned, size implementation-dependent.
     * @requires_gl Use exactly specified format in OpenGL ES or WebGL instead.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref RenderbufferFormat::RGBA8.
     */
    RGBA = GL_RGBA,
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * RGBA, each component normalized unsigned byte.
     * @requires_gles30 Extension @gl_extension{ARM,rgba8} or @gl_extension{OES,rgb8_rgba8}
     *      in OpenGL ES 2.0.
     * @requires_webgl20 Not availabe in WebGL 1.0, use for example
     *      @ref RenderbufferFormat::RGB565 or @ref RenderbufferFormat::RGBA4
     *      instead.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RGBA8 = GL_RGBA8,
    #else
    RGBA8 = GL_RGBA8_OES,
    #endif
    #endif

    /* Available everywhere except ES2 (WebGL 1 has it) */
    #if !(defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    /**
     * Red component, normalized unsigned short.
     * @requires_gl30 Extension @gl_extension{ARB,texture_rg}
     * @requires_gles31 Only byte-sized normalized formats (such as
     *      @ref RenderbufferFormat::RG8) are available in OpenGL ES 3.0 and
     *      older; not defined on ES2
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
     * @requires_gles31 Only byte-sized normalized formats (such as
     *      @ref RenderbufferFormat::RG8) are available in OpenGL ES 3.0 and
     *      older; not defined on ES2
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     * @requires_webgl_extension Extension @webgl_extension{EXT,texture_norm16}
     */
    #ifndef MAGNUM_TARGET_GLES
    RG16 = GL_RG16,
    #else
    RG16 = GL_RG16_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGB, each component normalized unsigned short.
     * @requires_gl Three-component formats are not renderable in OpenGL ES
     *      or WebGL.
     */
    RGB16 = GL_RGB16,
    #endif

    /**
     * RGBA, each component normalized unsigned short.
     * @requires_gles31 Only byte-sized normalized formats (such as
     *      @ref RenderbufferFormat::RG8) are available in OpenGL ES 3.0 and
     *      older; not defined on ES2
     * @requires_es_extension OpenGL ES 3.1 and @gl_extension{EXT,texture_norm16}
     * @requires_webgl_extension Extension @webgl_extension{EXT,texture_norm16}
     */
    #ifndef MAGNUM_TARGET_GLES
    RGBA16 = GL_RGBA16,
    #else
    RGBA16 = GL_RGBA16_EXT,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red component, non-normalized unsigned byte.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats (such as
     *      @ref RenderbufferFormat::R8) are available in OpenGL ES 2.0.
     * @requires_webgl20 Only three- and four-component normalized integral
     *      formats (such as @ref RenderbufferFormat::RGB565) are available in
     *      WebGL 1.0.
     */
    R8UI = GL_R8UI,

    /**
     * Red and green component, each non-normalized unsigned byte.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats (such as
     *      @ref RenderbufferFormat::RG8) are available in OpenGL ES 2.0.
     * @requires_webgl20 Only three- and four-component normalized integral
     *      formats (such as @ref RenderbufferFormat::RGB565) are available in
     *      WebGL 1.0.
     */
    RG8UI = GL_RG8UI,

    /**
     * RGBA, each component non-normalized unsigned byte.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats (such as
     *      @ref RenderbufferFormat::RGBA8) are available in OpenGL ES 2.0.
     * @requires_webgl20 Only three- and four-component normalized integral
     *      formats (such as @ref RenderbufferFormat::RGBA4) are available in
     *      WebGL 1.0.
     */
    RGBA8UI = GL_RGBA8UI,

    /**
     * Red component, non-normalized signed byte.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats (such as
     *      @ref RenderbufferFormat::R8) are available in OpenGL ES 2.0.
     * @requires_webgl20 Only three- and four-component normalized integral
     *      formats (such as @ref RenderbufferFormat::RGB565) are available in
     *      WebGL 1.0.
     */
    R8I = GL_R8I,

    /**
     * Red and green component, each non-normalized signed byte.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats (such as
     *      @ref RenderbufferFormat::RG8) are available in OpenGL ES 2.0.
     * @requires_webgl20 Only three- and four-component normalized integral
     *      formats (such as @ref RenderbufferFormat::RGB565) are available in
     *      WebGL 1.0.
     */
    RG8I = GL_RG8I,

    /**
     * RGBA, each component non-normalized signed byte.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats (such as
     *      @ref RenderbufferFormat::RGBA8) are available in OpenGL ES 2.0.
     * @requires_webgl20 Only three- and four-component normalized integral
     *      formats (such as @ref RenderbufferFormat::RGBA4) are available in
     *      WebGL 1.0.
     */
    RGBA8I = GL_RGBA8I,

    /**
     * Red component, non-normalized unsigned short.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::R8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGB565) are
     *      available in WebGL 1.0.
     */
    R16UI = GL_R16UI,

    /**
     * Red and green component, each non-normalized unsigned short.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::RG8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGB565) are
     *      available in WebGL 1.0.
     */
    RG16UI = GL_RG16UI,

    /**
     * RGBA, each component non-normalized unsigned short.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::RGBA8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGBA4) are
     *      available in WebGL 1.0.
     */
    RGBA16UI = GL_RGBA16UI,

    /**
     * Red component, non-normalized signed short.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::R8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGB565) are
     *      available in WebGL 1.0.
     */
    R16I = GL_R16I,

    /**
     * Red and green component, each non-normalized signed short.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::RG8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGB565) are
     *      available in WebGL 1.0.
     */
    RG16I = GL_RG16I,

    /**
     * RGBA, each component non-normalized signed short.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::RGBA8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGBA4) are
     *      available in WebGL 1.0.
     */
    RGBA16I = GL_RGBA16I,

    /**
     * Red component, non-normalized unsigned int.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::R8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGB565) are
     *      available in WebGL 1.0.
     */
    R32UI = GL_R32UI,

    /**
     * Red and green component, each non-normalized unsigned int.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::RG8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGB565) are
     *      available in WebGL 1.0.
     */
    RG32UI = GL_RG32UI,

    /**
     * RGBA, each component non-normalized unsigned int.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::RGBA8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGBA4) are
     *      available in WebGL 1.0.
     */
    RGBA32UI = GL_RGBA32UI,

    /**
     * Red component, non-normalized signed int.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::R8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGB565) are
     *      available in WebGL 1.0.
     */
    R32I = GL_R32I,

    /**
     * Red and green component, each non-normalized signed int.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::RG8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGB565) are
     *      available in WebGL 1.0.
     */
    RG32I = GL_RG32I,

    /**
     * RGBA, each component non-normalized signed int.
     * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
     * @requires_gles30 Only byte-sized normalized integral formats
     *      (such as @ref RenderbufferFormat::RGBA8) are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only byte-sized three- and four-component normalized
     *      integral formats (such as @ref RenderbufferFormat::RGBA4) are
     *      available in WebGL 1.0.
     */
    RGBA32I = GL_RGBA32I,
    #endif

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /**
     * Red component, half float.
     * @requires_gl30 Extension @gl_extension{ARB,texture_float}
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_half_float}. Use
     *      @ref RenderbufferFormat::R16UI or @ref RenderbufferFormat::R16I
     *      instead if not available.
     * @requires_webgl20 Not defined in WebGL 1.0 builds. Only four-component
     *      half-float formats can be ised as a render target in WebGL 1.0, see
     *      @ref RenderbufferFormat::RGBA16F for more information.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_float}
     *      in WebGL 2.0. Use @ref RenderbufferFormat::R16UI or
     *      @ref RenderbufferFormat::R16I instead if not available.
     */
    #ifndef MAGNUM_TARGET_GLES2
    R16F = GL_R16F,
    #else
    R16F = GL_R16F_EXT,
    #endif

    /**
     * Red and green component, each half float.
     * @requires_gl30 Extension @gl_extension{ARB,texture_float}
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_half_float}. Use
     *      @ref RenderbufferFormat::RG16UI or @ref RenderbufferFormat::RG16I
     *      instead if not available.
     * @requires_webgl20 Not defined in WebGL 1.0 builds. Only four-component
     *      half-float formats can be ised as a render target in WebGL 1.0, see
     *      @ref RenderbufferFormat::RGBA16F for more information.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_float}
     *      in WebGL 2.0. Use @ref RenderbufferFormat::RG16UI or
     *      @ref RenderbufferFormat::RG16I instead if not available.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RG16F = GL_RG16F,
    #else
    RG16F = GL_RG16F_EXT,
    #endif
    #endif

    /**
     * RGBA, each component half float.
     * @requires_gl30 Extension @gl_extension{ARB,texture_float}
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_half_float}. Use
     *      @ref RenderbufferFormat::RGBA16UI or @ref RenderbufferFormat::RGBA16I
     *      instead if not available.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_half_float}
     *      in WebGL 1.0, @webgl_extension{EXT,color_buffer_half_float} in
     *      WebGL 2.0. Use @ref RenderbufferFormat::RGBA16UI or
     *      @ref RenderbufferFormat::RGBA16I instead if not available.
     */
    #ifndef MAGNUM_TARGET_GLES2
    RGBA16F = GL_RGBA16F,
    #else
    RGBA16F = GL_RGBA16F_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red component, float.
     * @requires_gl30 Extension @gl_extension{ARB,texture_float}
     * @requires_gles30 Not defined in OpenGL ES 2.0 builds.
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_float} in OpenGL
     *      ES 3.0. Use @ref RenderbufferFormat::R32UI or @ref RenderbufferFormat::R32I
     *      instead if not available.
     * @requires_webgl20 Not defined in WebGL 1.0 builds. Only four-component
     *      float formats can be ised as a render target in WebGL 1.0, see
     *      @ref RenderbufferFormat::RGBA32F for more information.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_float}
     *      in WebGL 2.0. Use @ref RenderbufferFormat::R32UI or
     *      @ref RenderbufferFormat::R32I instead if not available.
     */
    R32F = GL_R32F,

    /**
     * Red and green component, each float.
     * @requires_gl30 Extension @gl_extension{ARB,texture_float}
     * @requires_gles30 Not defined in OpenGL ES 2.0 builds.
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_float} in OpenGL
     *      ES 3.0. Use @ref RenderbufferFormat::RG32UI or @ref RenderbufferFormat::RG32I
     *      instead if not available.
     * @requires_webgl20 Not defined in WebGL 1.0 builds. Only four-component
     *      float formats can be ised as a render target in WebGL 1.0, see
     *      @ref RenderbufferFormat::RGBA32F for more information.
     * @requires_webgl_extension Extension @webgl_extension{EXT,color_buffer_float}
     *      in WebGL 2.0. Use @ref RenderbufferFormat::RG32UI or
     *      @ref RenderbufferFormat::RG32I instead if not available.
     */
    RG32F = GL_RG32F,
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
    /**
     * RGBA, each component float.
     * @requires_gl30 Extension @gl_extension{ARB,texture_float}
     * @requires_gles30 Not defined in OpenGL ES 2.0.
     * @requires_gles32 Extension @gl_extension{EXT,color_buffer_float}. Use
     *      @ref RenderbufferFormat::RGBA32UI or @ref RenderbufferFormat::RGBA32I
     *      instead if not available.
     * @requires_webgl_extension Extension @webgl_extension{WEBGL,color_buffer_float}
     *      in WebGL 1.0, @webgl_extension{EXT,color_buffer_float} in WebGL 2.0.
     *      Use @ref RenderbufferFormat::RGBA32UI or @ref RenderbufferFormat::RGBA32I
     *      instead if not available.
     */
    RGBA32F =
        #ifndef MAGNUM_TARGET_WEBGL
        GL_RGBA32F
        #else
        0x8814
        #endif
        ,
    #endif

    /** RGB, normalized unsigned, red and blue 5bit, green 6bit. */
    RGB565 = GL_RGB565,

    /** RGBA, normalized unsigned, each component 4bit. */
    RGBA4 = GL_RGBA4,

    /** RGBA, normalized unsigned, each RGB component 5bit, alpha 1bit. */
    RGB5A1 = GL_RGB5_A1,

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * RGBA, normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gles30 Usable only as internal texture format in OpenGL ES
     *      2.0, see @ref TextureFormat::RGB10A2.
     * @requires_webgl20 Not available in WebGL 1.0, use e.g.
     *      @ref RenderbufferFormat::RGB5A1 instead.
     */
    RGB10A2 = GL_RGB10_A2,

    /**
     * RGBA, non-normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gl33 Extension @gl_extension{ARB,texture_rgb10_a2ui}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Only normalized integral formats (such as
     *      @ref RenderbufferFormat::RGB5A1) are available in WebGL 1.0.
     */
    RGB10A2UI = GL_RGB10_A2UI,
    #endif

    /* NV_sRGB_formats makes GL_SRGB8_NV accepted here but since that format
       doesn't make sense anywhere else and there's GL_SRGB8_ALPHA8_EXT anyway,
       I don't think it's worth supporting. */

    /**
     * sRGBA, each component normalized unsigned byte.
     * @requires_gles30 Extension @gl_extension{EXT,sRGB} in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{EXT,sRGB} in WebGL 1.0.
     */
    #ifndef MAGNUM_TARGET_GLES2
    SRGB8Alpha8 = GL_SRGB8_ALPHA8,
    #else
    SRGB8Alpha8 = GL_SRGB8_ALPHA8_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGB, float, red and green 11bit, blue 10bit.
     * @requires_gl30 Extension @gl_extension{EXT,packed_float}
     * @requires_gl Usable only as internal texture format in OpenGL ES and
     *      WebGL, see @ref TextureFormat::R11FG11FB10F.
     */
    R11FG11FB10F = GL_R11F_G11F_B10F,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Depth component, size implementation-dependent.
     * @todo is this allowed in core?
     * @requires_gl Use exactly specified format in OpenGL ES and WebGL
     *      instead.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref RenderbufferFormat::DepthComponent16.
     */
    DepthComponent = GL_DEPTH_COMPONENT,
    #endif

    /** Depth component, 16bit. */
    DepthComponent16 = GL_DEPTH_COMPONENT16,

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Depth component, 24bit.
     * @requires_gles30 Extension @gl_extension{OES,depth24} in OpenGL ES 2.0.
     * @requires_webgl20 Only 16bit depth component is available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES2
    DepthComponent24 = GL_DEPTH_COMPONENT24,
    #else
    DepthComponent24 = GL_DEPTH_COMPONENT24_OES,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Depth component, 32bit.
     * @requires_es_extension Extension @gl_extension{OES,depth32}
     * @requires_gles At most 24bit depth component is available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    DepthComponent32 = GL_DEPTH_COMPONENT32,
    #else
    DepthComponent32 = GL_DEPTH_COMPONENT32_OES,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Depth component, 32bit float.
     * @requires_gl30 Extension @gl_extension{ARB,depth_buffer_float}
     * @requires_gles30 Only integral depth textures are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only integral depth textures are available in WebGL
     *      1.0.
     */
    DepthComponent32F = GL_DEPTH_COMPONENT32F,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Stencil index, size implementation-dependent.
     * @requires_gl Use exactly specified format in OpenGL ES and WebGL
     *      instead.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref RenderbufferFormat::StencilIndex8.
     */
    StencilIndex = GL_STENCIL_INDEX,
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * 1-bit stencil index.
     * @requires_es_extension Extension @gl_extension{OES,stencil1}
     * @requires_gles Only 8bit stencil index is available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    StencilIndex1 = GL_STENCIL_INDEX1,
    #else
    StencilIndex1 = GL_STENCIL_INDEX1_OES,
    #endif

    /**
     * 4-bit stencil index.
     * @requires_es_extension Extension @gl_extension{OES,stencil4}
     * @requires_gles Only 8bit stencil index is available in WebGL.
     */
    #ifndef MAGNUM_TARGET_GLES
    StencilIndex4 = GL_STENCIL_INDEX4,
    #else
    StencilIndex4 = GL_STENCIL_INDEX4_OES,
    #endif
    #endif

    /** 8-bit stencil index. */
    StencilIndex8 = GL_STENCIL_INDEX8,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * 16-bit stencil index.
     * @requires_gl At most 8bit stencil index is available in OpenGL ES and
     *      WebGL.
     */
    StencilIndex16 = GL_STENCIL_INDEX16,
    #endif

    #if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Depth and stencil component, size implementation-dependent.
     * @requires_gl Use exactly specified format in OpenGL ES instead. This is,
     *      however, available in WebGL 1.0.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref RenderbufferFormat::Depth24Stencil8.
     */
    #ifndef MAGNUM_TARGET_GLES
    DepthStencil = GL_DEPTH_STENCIL,
    #else
    DepthStencil = GL_DEPTH_STENCIL_OES,
    #endif
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * 24bit depth and 8bit stencil component.
     * @requires_gles30 Extension @gl_extension{OES,packed_depth_stencil} in
     *      OpenGL ES 2.0.
     * @requires_webgl20 Use @ref RenderbufferFormat::DepthStencil in WebGL
     *      1.0 instead.
     */
    #ifdef MAGNUM_TARGET_GLES2
    Depth24Stencil8 = GL_DEPTH24_STENCIL8_OES
    #else
    Depth24Stencil8 = GL_DEPTH24_STENCIL8,
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * 32bit float depth component and 8bit stencil component.
     * @requires_gl30 Extension @gl_extension{ARB,depth_buffer_float}
     * @requires_gles30 Only integral depth textures are available in OpenGL ES
     *      2.0.
     * @requires_webgl20 Only integral depth textures are available in WebGL
     *      1.0.
     */
    Depth32FStencil8 = GL_DEPTH32F_STENCIL8
    #endif
};

}}

#endif
