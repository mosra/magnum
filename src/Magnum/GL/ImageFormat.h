#ifndef Magnum_GL_ImageFormat_h
#define Magnum_GL_ImageFormat_h
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
 * @brief Enum @ref Magnum::GL::ImageAccess, @ref Magnum::GL::ImageFormat
 */
#endif

#include "Magnum/GL/OpenGL.h"

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
namespace Magnum { namespace GL {

/**
@brief Image access

@see @ref Texture::bindImage() "*Texture::bindImage()",
    @ref Texture::bindImageLayered() "*Texture::bindImageLayered()"
@m_enum_values_as_keywords
@requires_gl42 Extension @gl_extension{ARB,shader_image_load_store}
@requires_gles31 Shader image load/store is not available in OpenGL ES 3.0 and
    older.
@requires_gles Shader image load/store is not available in WebGL.
*/
enum class ImageAccess: GLenum {
    ReadOnly = GL_READ_ONLY,
    WriteOnly = GL_WRITE_ONLY,
    ReadWrite = GL_READ_WRITE
};

/**
@brief Image format

@see @ref Texture::bindImage() "*Texture::bindImage()",
    @ref Texture::bindImageLayered() "*Texture::bindImageLayered()"
@m_enum_values_as_keywords
@requires_gl42 Extension @gl_extension{ARB,shader_image_load_store}
@requires_gles31 Shader image load/store is not available in OpenGL ES 3.0 and
    older.
@requires_gles Shader image load/store is not available in WebGL.
*/
enum class ImageFormat: GLenum {
    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, normalized unsigned byte.
     * @requires_gl Only four-component byte image formats are available in
     *      OpenGL ES.
     */
    R8 = GL_R8,

    /**
     * Red and green component, each normalized unsigned byte.
     * @requires_gl Only four-component byte image formats are available in
     *      OpenGL ES.
     */
    RG8 = GL_RG8,
    #endif

    /** RGBA, each component normalized unsigned byte. */
    RGBA8 = GL_RGBA8,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, normalized signed byte.
     * @requires_gl Only four-component byte image formats are available in
     *      OpenGL ES.
     */
    R8Snorm = GL_R8_SNORM,

    /**
     * Red and green component, each normalized signed byte.
     * @requires_gl Only four-component byte image formats are available in
     *      OpenGL ES.
     */
    RG8Snorm = GL_RG8_SNORM,
    #endif

    /** RGBA, each component normalized unsigned byte. */
    RGBA8Snorm = GL_RGBA8_SNORM,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, normalized unsigned short.
     * @requires_gl Only byte-size normalized image formats are available in
     *      OpenGL ES.
     */
    R16 = GL_R16,

    /**
     * Red and green component, each normalized unsigned short.
     * @requires_gl Only byte-size normalized image formats are available in
     *      OpenGL ES.
     */
    RG16 = GL_RG16,

    /**
     * RGBA, each component normalized unsigned short.
     * @requires_gl Only byte-size normalized image formats are available in
     *      OpenGL ES.
     */
    RGBA16 = GL_RGBA16,

    /**
     * Red component, normalized signed short.
     * @requires_gl Only byte-size normalized image formats are available in
     *      OpenGL ES.
     */
    R16Snorm = GL_R16_SNORM,

    /**
     * Red and green component, each normalized signed short.
     * @requires_gl Only byte-size normalized image formats are available in
     *      OpenGL ES.
     */
    RG16Snorm = GL_RG16_SNORM,

    /**
     * RGBA, each component normalized unsigned short.
     * @requires_gl Only byte-size normalized image formats are available in
     *      OpenGL ES.
     */
    RGBA16Snorm = GL_RGBA16_SNORM,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, non-normalized unsigned byte.
     * @requires_gl Only four-component byte image formats are available in
     *      OpenGL ES.
     */
    R8UI = GL_R8UI,

    /**
     * Red and green component, each non-normalized unsigned byte.
     * @requires_gl Only four-component byte image formats are available in
     *      OpenGL ES.
     */
    RG8UI = GL_RG8UI,
    #endif

    /** RGBA, each component non-normalized unsigned byte. */
    RGBA8UI = GL_RGBA8UI,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, non-normalized signed byte.
     * @requires_gl Only four-component byte image formats are available in
     *      OpenGL ES.
     */
    R8I = GL_R8I,

    /**
     * Red and green component, each non-normalized signed byte.
     * @requires_gl Only four-component byte image formats are available in
     *      OpenGL ES.
     */
    RG8I = GL_RG8I,
    #endif

    /** RGBA, each component non-normalized signed byte. */
    RGBA8I = GL_RGBA8I,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, non-normalized unsigned short.
     * @requires_gl Only four-component short image formats are available in
     *      OpenGL ES.
     */
    R16UI = GL_R16UI,

    /**
     * Red and green component, each non-normalized unsigned short.
     * @requires_gl Only four-component short image formats are available in
     *      OpenGL ES.
     */
    RG16UI = GL_RG16UI,
    #endif

    /** RGBA, each component non-normalized unsigned short. */
    RGBA16UI = GL_RGBA16UI,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, non-normalized signed short.
     * @requires_gl Only four-component short image formats are available in
     *      OpenGL ES.
     */
    R16I = GL_R16I,

    /**
     * Red and green component, each non-normalized signed short.
     * @requires_gl Only four-component short image formats are available in
     *      OpenGL ES.
     */
    RG16I = GL_RG16I,
    #endif

    /** RGBA, each component non-normalized signed short. */
    RGBA16I = GL_RGBA16I,

    /** Red component, non-normalized unsigned int. */
    R32UI = GL_R32UI,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red and green component, each non-normalized unsigned int.
     * @requires_gl Only one- or four-component int image formats are available
     *      in OpenGL ES.
     */
    RG32UI = GL_RG32UI,
    #endif

    /** RGBA, each component non-normalized unsigned int. */
    RGBA32UI = GL_RGBA32UI,

    /** Red component, non-normalized signed int. */
    R32I = GL_R32I,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red and green component, each non-normalized signed int.
     * @requires_gl Only one- or four-component int image formats are available
     *      in OpenGL ES.
     */
    RG32I = GL_RG32I,
    #endif

    /** RGBA, each component non-normalized signed int. */
    RGBA32I = GL_RGBA32I,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, half float.
     * @requires_gl Only four-component half float image formats are available
     *      in OpenGL ES.
     */
    R16F = GL_R16F,

    /**
     * Red and green component, each half float.
     * @requires_gl Only four-component half float image formats are available
     *      in OpenGL ES.
     */
    RG16F = GL_RG16F,
    #endif

    /** RGBA, each component half float. */
    RGBA16F = GL_RGBA16F,

    /** Red component, float. */
    R32F = GL_R32F,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red and green component, each float.
     * @requires_gl Only one- or four-component float image formats are
     *      available in OpenGL ES.
     */
    RG32F = GL_RG32F,
    #endif

    /** RGBA, each component float. */
    RGBA32F = GL_RGBA32F,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGB, float, red and green component 11bit, blue 10bit.
     * @requires_gl Packed image formats are not available in OpenGL ES.
     */
    R11FG11FB10F = GL_R11F_G11F_B10F,

    /**
     * RGBA, normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gl Packed image formats are not available in OpenGL ES.
     */
    RGB10A2 = GL_RGB10_A2,

    /**
     * RGBA, non-normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gl33 Extension @gl_extension{ARB,texture_rgb10_a2ui}
     * @requires_gl Packed image formats are not available in OpenGL ES.
     */
    RGB10A2UI = GL_RGB10_A2UI,
    #endif
};

}}
#else
#error this header is not available in OpenGL ES 2.0 and WebGL build
#endif

#endif
