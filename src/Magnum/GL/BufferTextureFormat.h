#ifndef Magnum_GL_BufferTextureFormat_h
#define Magnum_GL_BufferTextureFormat_h
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

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/** @file
 * @brief Enum @ref Magnum::GL::BufferTextureFormat
 */
#endif

#include "Magnum/GL/OpenGL.h"

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
namespace Magnum { namespace GL {

/**
@brief Internal buffer texture format

@see @ref BufferTexture
@m_enum_values_as_keywords
@requires_gl31 Extension @gl_extension{ARB,texture_buffer_object}
@requires_gles30 Not defined in OpenGL ES 2.0.
@requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
    @gl_extension{EXT,texture_buffer}
@requires_gles Texture buffers are not available in WebGL.
*/
enum class BufferTextureFormat: GLenum {
    /** Red component, normalized unsigned byte. */
    R8 = GL_R8,

    /** Red and green component, each normalized unsigned byte. */
    RG8 = GL_RG8,

    /** RGBA, each component normalized unsigned byte. */
    RGBA8 = GL_RGBA8,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, normalized unsigned short.
     * @requires_gl Only @ref BufferTextureFormat::R8 is available in OpenGL
     *      ES.
     */
    R16 = GL_R16,

    /**
     * Red and green component, each normalized unsigned short.
     * @requires_gl Only @ref BufferTextureFormat::RG8 is available in OpenGL
     *      ES.
     */
    RG16 = GL_RG16,

    /**
     * RGBA, each component normalized unsigned short.
     * @requires_gl Only @ref BufferTextureFormat::RGBA8 is available in OpenGL
     *      ES.
     */
    RGBA16 = GL_RGBA16,
    #endif

    /** Red component, non-normalized unsigned byte. */
    R8UI = GL_R8UI,

    /** Red and green component, each non-normalized unsigned byte. */
    RG8UI = GL_RG8UI,

    /** RGBA, each component non-normalized unsigned byte. */
    RGBA8UI = GL_RGBA8UI,

    /** Red component, non-normalized signed byte. */
    R8I = GL_R8I,

    /** Red and green component, each non-normalized signed byte. */
    RG8I = GL_RG8I,

    /** RGBA, each component non-normalized signed byte. */
    RGBA8I = GL_RGBA8I,

    /** Red component, non-normalized unsigned short. */
    R16UI = GL_R16UI,

    /** Red and green component, each non-normalized unsigned short. */
    RG16UI = GL_RG16UI,

    /** RGBA, each component non-normalized unsigned short. */
    RGBA16UI = GL_RGBA16UI,

    /** Red component, non-normalized signed short. */
    R16I = GL_R16I,

    /** Red and green component, each non-normalized signed short. */
    RG16I = GL_RG16I,

    /** RGBA, each component non-normalized signed short. */
    RGBA16I = GL_RGBA16I,

    /** Red component, non-normalized unsigned int. */
    R32UI = GL_R32UI,

    /** Red and green component, each non-normalized unsigned int. */
    RG32UI = GL_RG32UI,

    /**
     * RGB, each component non-normalized unsigned int.
     * @requires_gl40 Extension @gl_extension{ARB,texture_buffer_object_rgb32}
     */
    RGB32UI = GL_RGB32UI,

    /** RGBA, each component non-normalized unsigned int. */
    RGBA32UI = GL_RGBA32UI,

    /** Red component, non-normalized signed int. */
    R32I = GL_R32I,

    /** Red and green component, each non-normalized signed int. */
    RG32I = GL_RG32I,

    /**
     * RGB, each component non-normalized signed int.
     * @requires_gl40 Extension @gl_extension{ARB,texture_buffer_object_rgb32}
     */
    RGB32I = GL_RGB32I,

    /** RGBA, each component non-normalized signed int. */
    RGBA32I = GL_RGBA32I,

    /** Red component, half float. */
    R16F = GL_R16F,

    /** Red and green component, each half float. */
    RG16F = GL_RG16F,

    /** RGBA, each component half float. */
    RGBA16F = GL_RGBA16F,

    /** Red component, float. */
    R32F = GL_R32F,

    /** Red and green component, each float. */
    RG32F = GL_RG32F,

    /**
     * RGB, each component float.
     * @requires_gl40 Extension @gl_extension{ARB,texture_buffer_object_rgb32}
     */
    RGB32F = GL_RGB32F,

    /** RGBA, each component float. */
    RGBA32F = GL_RGBA32F
};

}}
#else
#error this header is not available in OpenGL ES 2.0 and WebGL build
#endif

#endif
