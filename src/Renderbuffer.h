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
        /** @{ @name Internal renderbuffer formats */

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief AbstractTexture::Components
         *
         * Like AbstractTexture::Components, without three-component RGB.
         * @requires_gl
         */
        enum class Components {
            Red, RedGreen, RGBA
        };

        /**
         * @copybrief AbstractTexture::ComponentType
         *
         * Like AbstractTexture::ComponentType, without normalized signed
         * types.
         * @requires_gl
         */
        enum class ComponentType {
            UnsignedByte, Byte, UnsignedShort, Short, UnsignedInt, Int, Half,
            Float, NormalizedUnsignedByte, NormalizedUnsignedShort
        };
        #endif

        /**
         * @copybrief AbstractTexture::Format
         *
         * Like AbstractTexture::Format without
         * AbstractTexture::Format::RGB9Intensity5, three-component and
         * compressed formats, but with added separate stencil index.
         */
        enum class Format: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            Red = GL_RED, RedGreen = GL_RG,
            #endif

            RGBA = GL_RGBA,

            #ifndef MAGNUM_TARGET_GLES
            BGRA = GL_BGRA, SRGBA = GL_SRGB8_ALPHA8,
            RGB10Alpha2 = GL_RGB10_A2, RGB10AlphaUnsigned2 = GL_RGB10_A2UI,
            #endif

            RGB5Alpha1 = GL_RGB5_A1, RGBA4 = GL_RGBA4,

            #ifndef MAGNUM_TARGET_GLES
            RFloat11GFloat11BFloat10 = GL_R11F_G11F_B10F,
            #endif

            #if defined(GL_RGB565) || defined(DOXYGEN_GENERATING_OUTPUT)
            RGB565 = GL_RGB565,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Depth component, at least 16bit.
             *
             * Prefer to use the exactly specified version of this format, in
             * this case e.g. `Format::%Depth16`.
             * @requires_gl Use exactly specified format <tt>Format::%Depth16</tt> instead.
             */
            Depth = GL_DEPTH_COMPONENT,

            DepthStencil = GL_DEPTH_STENCIL,
            #endif

            Depth16 = GL_DEPTH_COMPONENT16,

            #ifndef MAGNUM_TARGET_GLES
            Depth24 = GL_DEPTH_COMPONENT24,
            DepthFloat = GL_DEPTH_COMPONENT32F,

            /**
             * Stencil index (unspecified size).
             *
             * Prefer to use the exactly specified version of this format, in
             * this case e.g. `Format::%Stencil8`.
             * @requires_gl Use exactly specified format <tt>Format::%Stencil8</tt> instead.
             */
            Stencil = GL_STENCIL_INDEX,

            /**
             * 1-bit stencil index.
             *
             * @requires_gl Use <tt>Format::%Stencil8</tt> instead.
             */
            Stencil1 = GL_STENCIL_INDEX1,

            /**
             * 4-bit stencil index.
             *
             * @requires_gl Use <tt>Format::%Stencil8</tt> instead.
             */
            Stencil4 = GL_STENCIL_INDEX4,
            #endif

            /** 8-bit stencil index. */
            Stencil8 = GL_STENCIL_INDEX8

            #ifndef MAGNUM_TARGET_GLES
            ,

            /**
             * 16-bit stencil index.
             *
             * @requires_gl Use <tt>Format::%Stencil8</tt> instead.
             */
            Stencil16 = GL_STENCIL_INDEX1,

            Depth24Stencil8 = GL_DEPTH24_STENCIL8,
            DepthFloatStencil8 = GL_DEPTH32F_STENCIL8
            #endif
        };

        /** @copydoc AbstractTexture::InternalFormat */
        class MAGNUM_EXPORT InternalFormat {
            public:
                #ifndef MAGNUM_TARGET_GLES
                /** @copydoc AbstractTexture::InternalFormat::InternalFormat(AbstractTexture::Components, AbstractTexture::ComponentType) */
                InternalFormat(Components components, ComponentType type);
                #endif

                /** @copydoc AbstractTexture::InternalFormat::InternalFormat(AbstractTexture::Format) */
                inline constexpr InternalFormat(Format format): internalFormat(static_cast<GLenum>(format)) {}

                /**
                 * @brief OpenGL internal format ID
                 *
                 * @todoc Remove workaround when Doxygen supports \@copydoc for conversion operators
                 */
                inline constexpr operator GLenum() const { return internalFormat; }

            private:
                GLenum internalFormat;
        };

        /*@}*/

        /**
         * @brief Constructor
         *
         * Generates new OpenGL renderbuffer.
         */
        inline Renderbuffer() {
            glGenRenderbuffers(1, &renderbuffer);
        }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL renderbuffer.
         */
        inline ~Renderbuffer() {
            glDeleteRenderbuffers(1, &renderbuffer);
        }

        /** @brief OpenGL internal renderbuffer ID */
        inline GLuint id() const { return renderbuffer; }

        /** @brief Bind renderbuffer */
        inline void bind() {
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        }

        /**
         * @brief Set renderbuffer storage
         * @param internalFormat    Internal format
         * @param size              Renderbuffer size
         */
        inline void setStorage(InternalFormat internalFormat, const Math::Vector2<GLsizei>& size) {
            bind();
            glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, size.x(), size.y());
        }

    private:
        GLuint renderbuffer;
};

#ifndef MAGNUM_TARGET_GLES
/** @relates Renderbuffer
@brief Convertor of component count and data type to InternalFormat

@requires_gl
*/
inline Renderbuffer::InternalFormat operator|(Renderbuffer::Components components, Renderbuffer::ComponentType type) {
    return Renderbuffer::InternalFormat(components, type);
}
/** @relates Renderbuffer
 * @overload
 */
inline Renderbuffer::InternalFormat operator|(Renderbuffer::ComponentType type, Renderbuffer::Components components) {
    return Renderbuffer::InternalFormat(components, type);
}
#endif

}

#endif
