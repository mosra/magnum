#ifndef Magnum_Renderbuffer_h
#define Magnum_Renderbuffer_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Renderbuffer
 */

#include "Math/Vector2.h"
#include "Magnum.h"

#include "magnumVisibility.h"

namespace Magnum {

/**
@brief %Renderbuffer

Attachable to Framebuffer as render target.

@requires_gl30 Extension @extension{EXT,framebuffer_object}
*/
class Renderbuffer {
    Renderbuffer(const Renderbuffer& other) = delete;
    Renderbuffer(Renderbuffer&& other) = delete;
    Renderbuffer& operator=(const Renderbuffer& other) = delete;
    Renderbuffer& operator=(Renderbuffer&& other) = delete;

    public:
        /**
         * @brief Internal format
         *
         * @see @ref Texture::setData() "setData()"
         * @todo RGB, RGB8 ES only (ES3 + @es_extension{OES,rgb8_rgba8})
         */
        enum class InternalFormat: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Red component, normalized unsigned, size implementation-dependent.
             * @deprecated Prefer to use the exactly specified version of this
             *      format, e.g. @ref Magnum::Renderbuffer::InternalFormat "InternalFormat::R8".
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gl Use exactly specified format in OpenGL ES instead.
             */
            Red = GL_RED,
            #endif

            /**
             * Red component, normalized unsigned byte.
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gles30 %Extension @es_extension{EXT,texture_rg}
             */
            #ifndef MAGNUM_TARGET_GLES2
            R8 = GL_R8,
            #else
            R8 = GL_R8_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Red and green component, normalized unsigned, size
             * implementation-dependent.
             * @deprecated Prefer to use the exactly specified version of this
             *      format, e.g. @ref Magnum::Renderbuffer::InternalFormat "InternalFormat::RG8".
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gl Use exactly specified format in OpenGL ES instead.
             */
            RG = GL_RG,
            #endif

            /**
             * Red and green component, each normalized unsigned byte.
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gles30 %Extension @es_extension{EXT,texture_rg}
             */
            #ifndef MAGNUM_TARGET_GLES2
            RG8 = GL_RG8,
            #else
            RG8 = GL_RG8_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * RGBA, normalized unsigned, size implementation-dependent.
             * @deprecated Prefer to use the exactly specified version of this
             *      format, e.g. @ref Magnum::Renderbuffer::InternalFormat "InternalFormat::RGBA8".
             * @requires_gl Use exactly specified format in OpenGL ES 2.0
             *      instead.
             */
            RGBA = GL_RGBA,
            #endif

            /**
             * RGBA, each component normalized unsigned byte.
             * @requires_gles30 %Extension @es_extension{ARM,rgba8} or
             *      @es_extension{OES,required_internalformat} and @es_extension{OES,rgb8_rgba8}
             */
            #ifndef MAGNUM_TARGET_GLES2
            RGBA8 = GL_RGBA8,
            #else
            RGBA8 = GL_RGBA8_OES,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Red component, normalized unsigned short.
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gl Only byte-sized normalized formats are available
             *      in OpenGL ES.
             */
            R16 = GL_R16,

            /**
             * Red and green component, each normalized unsigned short.
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gl Only byte-sized normalized formats are available
             *      in OpenGL ES.
             */
            RG16 = GL_RG16,

            /**
             * RGB, each component normalized unsigned short.
             * @requires_gl Only byte-sized normalized formats are available
             *      in OpenGL ES.
             */
            RGB16 = GL_RGB16,

            /**
             * RGBA, each component normalized unsigned short.
             * @requires_gl Only byte-sized normalized formats are available
             *      in OpenGL ES.
             */
            RGBA16 = GL_RGBA16,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Red component, non-normalized unsigned byte.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R8UI = GL_R8UI,

            /**
             * Red and green component, each non-normalized unsigned byte.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG8UI = GL_RG8UI,

            /**
             * RGBA, each component non-normalized unsigned byte.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA8UI = GL_RGBA8UI,

            /**
             * Red component, non-normalized signed byte.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R8I = GL_R8I,

            /**
             * Red and green component, each non-normalized signed byte.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG8I = GL_RG8I,

            /**
             * RGBA, each component non-normalized signed byte.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA8I = GL_RGBA8I,

            /**
             * Red component, non-normalized unsigned short.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R16UI = GL_R16UI,

            /**
             * Red and green component, each non-normalized unsigned short.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG16UI = GL_RG16UI,

            /**
             * RGBA, each component non-normalized unsigned short.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA16UI = GL_RGBA16UI,

            /**
             * Red component, non-normalized signed short.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R16I = GL_R16I,

            /**
             * Red and green component, each non-normalized signed short.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG16I = GL_RG16I,

            /**
             * RGBA, each component non-normalized signed short.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA16I = GL_RGBA16I,

            /**
             * Red component, non-normalized unsigned int.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R32UI = GL_R32UI,

            /**
             * Red and green component, each non-normalized unsigned int.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG32UI = GL_RG32UI,

            /**
             * RGBA, each component non-normalized unsigned int.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA32UI = GL_RGBA32UI,

            /**
             * Red component, non-normalized signed int.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R32I = GL_R32I,

            /**
             * Red and green component, each non-normalized signed int.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG32I = GL_RG32I,

            /**
             * RGBA, each component non-normalized signed int.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA32I = GL_RGBA32I,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Red component, half float.
             * @requires_gl30 %Extension @extension{ARB,texture_float}
             * @requires_gl Only (non)normalized integral formats are
             *      available in OpenGL ES.
             */
            R16F = GL_R16F,

            /**
             * Red and green component, each half float.
             * @requires_gl30 %Extension @extension{ARB,texture_float}
             * @requires_gl Only (non)normalized integral formats are
             *      available in OpenGL ES.
             */
            RG16F = GL_RG16F,

            /**
             * RGBA, each component half float.
             * @requires_gl30 %Extension @extension{ARB,texture_float}
             * @requires_gl Only (non)normalized integral formats are
             *      available in OpenGL ES.
             */
            RGBA16F = GL_RGBA16F,

            /**
             * Red component, float.
             * @requires_gl30 %Extension @extension{ARB,texture_float}
             * @requires_gl Only (non)normalized integral formats are
             *      available in OpenGL ES.
             */
            R32F = GL_R32F,

            /**
             * Red and green component, each float.
             * @requires_gl30 %Extension @extension{ARB,texture_float}
             * @requires_gl Only (non)normalized integral formats are
             *      available in OpenGL ES.
             */
            RG32F = GL_RG32F,

            /**
             * RGBA, each component float.
             * @requires_gl30 %Extension @extension{ARB,texture_float}
             * @requires_gl Only (non)normalized integral formats are
             *      available in OpenGL ES.
             */
            RGBA32F = GL_RGBA32F,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * RGBA, normalized unsigned, each RGB component 10bit, alpha 2bit.
             * @requires_gles30 Usable only as internal texture format in OpenGL
             *      ES 2.0, see @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::RGB10A2".
             */
            RGB10A2 = GL_RGB10_A2,

            /**
             * RGBA, non-normalized unsigned, each RGB component 10bit, alpha 2bit.
             * @requires_gl33 %Extension @extension{ARB,texture_rgb10_a2ui}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGB10A2UI = GL_RGB10_A2UI,
            #endif

            /** RGBA, normalized unsigned, each RGB component 5bit, alpha 1bit. */
            RGB5A1 = GL_RGB5_A1,

            /** RGBA, normalized unsigned, each component 4bit. */
            RGBA4 = GL_RGBA4,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * RGB, float, red and green 11bit, blue 10bit.
             * @requires_gl30 %Extension @extension{EXT,packed_float}
             * @requires_gl Usable only as internal texture format in OpenGL
             *      ES, see @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::R11FG11FB10F".
             */
            R11FG11FB10F = GL_R11F_G11F_B10F,
            #endif

            /* 1.5.6 <= GLEW < 1.8.0 doesn't have this, even if there is
               GL_ARB_ES2_compatibility */
            #if defined(GL_RGB565) || defined(DOXYGEN_GENERATING_OUTPUT)
            /** RGB, normalized unsigned, red and blue 5bit, green 6bit. */
            RGB565 = GL_RGB565,
            #endif

            /**
             * sRGBA, each component normalized unsigned byte.
             * @requires_gles30 %Extension @es_extension{EXT,sRGB}
             */
            #ifndef MAGNUM_TARGET_GLES2
            SRGB8Alpha8 = GL_SRGB8_ALPHA8,
            #else
            SRGB8Alpha8 = GL_SRGB8_ALPHA8_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Depth component, size implementation-dependent.
             * @todo is this allowed in core?
             * @deprecated Prefer to use exactly specified version of this
             *      format, e.g. @ref Magnum::Renderbuffer::InternalFormat "InternalFormat::DepthComponent16".
             * @requires_gl Use exactly specified format in OpenGL ES instead.
             */
            DepthComponent = GL_DEPTH_COMPONENT,
            #endif

            /** Depth component, 16bit. */
            DepthComponent16 = GL_DEPTH_COMPONENT16,

            /**
             * Depth component, 24bit.
             * @requires_gles30 %Extension @es_extension{OES,depth24}
             */
            #ifndef MAGNUM_TARGET_GLES2
            DepthComponent24 = GL_DEPTH_COMPONENT24,
            #else
            DepthComponent24 = GL_DEPTH_COMPONENT24_OES,
            #endif

            /**
             * Depth component, 32bit.
             * @requires_es_extension %Extension @es_extension{OES,depth32}
             */
            #ifndef MAGNUM_TARGET_GLES
            DepthComponent32 = GL_DEPTH_COMPONENT32,
            #else
            DepthComponent32 = GL_DEPTH_COMPONENT32_OES,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Depth component, 32bit float.
             * @requires_gl30 %Extension @extension{ARB,depth_buffer_float}
             * @requires_gles30 Only integral depth textures are available in
             *      OpenGL ES 2.0.
             */
            DepthComponent32F = GL_DEPTH_COMPONENT32F,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Stencil index, size implementation-dependent.
             * @deprecated Prefer to use exactly specified version of this
             *      format, e.g. @ref Magnum::Renderbuffer::InternalFormat "InternalFormat::StencilIndex8".
             * @requires_gl Use exactly specified format in OpenGL ES instead.
             */
            StencilIndex = GL_STENCIL_INDEX,
            #endif

            /**
             * 1-bit stencil index.
             * @requires_es_extension %Extension @es_extension{OES,stencil1}
             */
            #ifndef MAGNUM_TARGET_GLES
            StencilIndex1 = GL_STENCIL_INDEX1,
            #else
            StencilIndex1 = GL_STENCIL_INDEX1_OES,
            #endif

            /**
             * 4-bit stencil index.
             * @requires_es_extension %Extension @es_extension{OES,stencil4}
             */
            #ifndef MAGNUM_TARGET_GLES
            StencilIndex4 = GL_STENCIL_INDEX4,
            #else
            StencilIndex4 = GL_STENCIL_INDEX4_OES,
            #endif

            /** 8-bit stencil index. */
            StencilIndex8 = GL_STENCIL_INDEX8,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * 16-bit stencil index.
             * @requires_gl At most 8bit stencil index is available in OpenGL
             *      ES.
             */
            StencilIndex16 = GL_STENCIL_INDEX16,

            /**
             * Depth and stencil component, size implementation-dependent.
             * @deprecated Prefer to use exactly specified version of this
             *      format, e.g. @ref Magnum::Renderbuffer::InternalFormat "InternalFormat::Depth24Stencil8".
             * @requires_gl Use exactly specified format in OpenGL ES instead.
             */
            DepthStencil = GL_DEPTH_STENCIL,
            #endif

            /**
             * 24bit depth and 8bit stencil component.
             * @requires_gl30 %Extension @extension{EXT,packed_depth_stencil}
             * @requires_gles30 %Extension @es_extension{OES,packed_depth_stencil}
             */
            #ifdef MAGNUM_TARGET_GLES2
            Depth24Stencil8 = GL_DEPTH24_STENCIL8_OES,
            #else
            Depth24Stencil8 = GL_DEPTH24_STENCIL8,

            /**
             * 32bit float depth component and 8bit stencil component.
             * @requires_gl30 %Extension @extension{ARB,depth_buffer_float}
             * @requires_gles30 Only integral depth textures are available in
             *      OpenGL ES 2.0.
             */
            Depth32FStencil8 = GL_DEPTH32F_STENCIL8,
            #endif
        };

        /**
         * @brief Constructor
         *
         * Generates new OpenGL renderbuffer.
         * @see @fn_gl{GenRenderbuffers}
         */
        inline Renderbuffer() {
            glGenRenderbuffers(1, &renderbuffer);
        }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL renderbuffer.
         * @see @fn_gl{DeleteRenderbuffers}
         */
        inline ~Renderbuffer() {
            glDeleteRenderbuffers(1, &renderbuffer);
        }

        /** @brief OpenGL internal renderbuffer ID */
        inline GLuint id() const { return renderbuffer; }

        /**
         * @brief Bind renderbuffer
         *
         * @see @fn_gl{BindRenderbuffer}
         */
        inline void bind() {
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        }

        /**
         * @brief Set renderbuffer storage
         * @param internalFormat    Internal format
         * @param size              Renderbuffer size
         *
         * @see bind(), @fn_gl{RenderbufferStorage}
         */
        inline void setStorage(InternalFormat internalFormat, const Vector2i& size) {
            bind();
            glRenderbufferStorage(GL_RENDERBUFFER, GLenum(internalFormat), size.x(), size.y());
        }

    private:
        GLuint renderbuffer;
};

}

#endif
