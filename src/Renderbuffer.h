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

#include "Magnum.h"

namespace Magnum {

/**
@brief Renderbuffer

Attachable to Framebuffer as render target.
*/
class Renderbuffer {
    Renderbuffer(const Renderbuffer& other) = delete;
    Renderbuffer(Renderbuffer&& other) = delete;
    Renderbuffer& operator=(const Renderbuffer& other) = delete;
    Renderbuffer& operator=(Renderbuffer&& other) = delete;

    public:
        /** @{ @name Internal renderbuffer formats */

        /**
         * @copydoc AbstractTexture::Components
         *
         * Like AbstractTexture::Components, without three-component RGB.
         */
        enum class Components {
            Red, RedGreen, RGBA
        };

        /**
         * @copydoc AbstractTexture::ComponentType
         *
         * Like AbstractTexture::ComponentType, without normalized signed
         * types.
         */
        enum class ComponentType {
            UnsignedByte, Byte, UnsignedShort, Short, UnsignedInt, Int, Half,
            Float, NormalizedUnsignedByte, NormalizedUnsignedShort
        };

        /**
         * @copydoc AbstractTexture::Format
         *
         * Like AbstractTexture::Format without
         * AbstractTexture::Format::RGB9Intensity5, three-component and
         * compressed formats.
         */
        enum class Format: GLenum {
            Red = GL_RED, RedGreen = GL_RG, RGBA = GL_RGBA, BGRA = GL_BGRA,
            SRGBA = GL_SRGB8_ALPHA8,
            RGB10Alpha2 = GL_RGB10_A2, RGB10AlphaUnsigned2 = GL_RGB10_A2UI,
            RGB5Alpha1 = GL_RGB5_A1, RGBA4 = GL_RGBA4,
            RFloat11GFloat11BFloat10 = GL_R11F_G11F_B10F,

            #if defined(GL_RGB565) || defined(DOXYGEN_GENERATING_OUTPUT)
            RGB565 = GL_RGB565,
            #endif

            Depth = GL_DEPTH_COMPONENT, DepthStencil = GL_DEPTH_STENCIL,
            Depth16 = GL_DEPTH_COMPONENT16, Depth24 = GL_DEPTH_COMPONENT24,
            DepthFloat = GL_DEPTH_COMPONENT32F,
            Depth24Stencil8 = GL_DEPTH24_STENCIL8,
            DepthFloatStencil8 = GL_DEPTH32F_STENCIL8
        };

        /** @copydoc AbstractTexture::InternalFormat */
        class InternalFormat {
            public:
                /** @copydoc AbstractTexture::InternalFormat::InternalFormat(AbstractTexture::Components, AbstractTexture::ComponentType) */
                InternalFormat(Components components, ComponentType type);

                /** @copydoc AbstractTexture::InternalFormat::InternalFormat(AbstractTexture::Format) */
                inline constexpr InternalFormat(Format format): internalFormat(static_cast<GLint>(format)) {}

                /** @brief OpenGL internal format ID */
                /* doxygen: @copydoc AbstractTexture::InternalFormat::operator GLint() doesn't work */
                inline constexpr operator GLint() const { return internalFormat; }

            private:
                GLint internalFormat;
        };

        /*@}*/

        /**
         * @brief Constructor
         *
         * Generates new OpenGL renderbuffer.
         */
        Renderbuffer() {
            glGenRenderbuffers(1, &renderbuffer);
        }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL renderbuffer.
         */
        ~Renderbuffer() {
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
            glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(internalFormat), size.x(), size.y());
        }

    private:
        GLuint renderbuffer;
};

/** @copydoc operator|(AbstractTexture::Components, AbstractTexture::ComponentType) */
inline Renderbuffer::InternalFormat operator|(Renderbuffer::Components components, Renderbuffer::ComponentType type) {
    return Renderbuffer::InternalFormat(components, type);
}
/** @copydoc operator|(AbstractTexture::ComponentType, AbstractTexture::Components) */
inline Renderbuffer::InternalFormat operator|(Renderbuffer::ComponentType type, Renderbuffer::Components components) {
    return Renderbuffer::InternalFormat(components, type);
}

}

#endif
