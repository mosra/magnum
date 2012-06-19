#ifndef Magnum_BufferedTexture_h
#define Magnum_BufferedTexture_h
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
 * @brief Class Magnum::BufferedTexture
 */

#include "Renderbuffer.h"
#include "Buffer.h"

namespace Magnum {

/** @ingroup textures
@brief Buffered texture

This texture is, unlike classic textures such as Texture or CubeMapTexture,
used as simple data source, without any unneccessary interpolation and
wrapping methods. Texture data are stored in buffer and after binding the
buffer to the texture using setBuffer(), you can fill the buffer at any time
using data setting functions in Buffer itself.

When using buffered texture in the shader, use `samplerBuffer` and fetch the
data using integer coordinates in `texelFetch()`.

@requires_gl31 Extension @extension{ARB,texture_buffer_object}
*/
class BufferedTexture {
    BufferedTexture(const BufferedTexture& other) = delete;
    BufferedTexture(BufferedTexture&& other) = delete;
    BufferedTexture& operator=(const BufferedTexture& other) = delete;
    BufferedTexture& operator=(BufferedTexture&& other) = delete;

    public:
        /** @{ @name Internal buffered texture formats */

        /** @copydoc Renderbuffer::Components */
        enum class Components {
            Red, RedGreen, RGBA
        };

        /** @copydoc Renderbuffer::ComponentType */
        enum class ComponentType {
            UnsignedByte, Byte, UnsignedShort, Short, UnsignedInt, Int, Half,
            Float, NormalizedUnsignedByte, NormalizedUnsignedShort
        };

        /** @copydoc AbstractTexture::Format */
        enum class Format: GLenum {
            /**
             * Three-component RGB, float, each component 32bit, 96bit total.
             *
             * @requires_gl40 Extension @extension{ARB,texture_buffer_object_rgb32}
             */
            RGB32Float = GL_RGB32F,

            /**
             * Three-component RGB, unsigned non-normalized, each component
             * 32bit, 96bit total.
             *
             * @requires_gl40 Extension @extension{ARB,texture_buffer_object_rgb32}
             */
            RGB32UnsignedInt = GL_RGB32UI,

            /**
             * Three-component RGB, signed non-normalized, each component
             * 32bit, 96bit total.
             *
             * @requires_gl40 Extension @extension{ARB,texture_buffer_object_rgb32}
             */
            RGB32Int = GL_RGB32I
        };

        /** @copydoc AbstractTexture::InternalFormat */
        class MAGNUM_EXPORT InternalFormat {
            public:
                /** @copydoc AbstractTexture::InternalFormat::InternalFormat(AbstractTexture::Components, AbstractTexture::ComponentType) */
                InternalFormat(Components components, ComponentType type);

                /** @copydoc AbstractTexture::InternalFormat::InternalFormat(AbstractTexture::Format) */
                inline constexpr InternalFormat(Format format): internalFormat(static_cast<GLenum>(format)) {}

                /**
                 * @brief OpenGL internal format ID
                 *
                 * @todoc Remove workaround when Doxygen supports \@copydoc for conversion operators
                 */
                inline constexpr operator GLint() const { return internalFormat; }

            private:
                GLint internalFormat;
        };

        /*@}*/

        /**
         * @brief Constructor
         *
         * Creates one OpenGL texture.
         */
        inline BufferedTexture() {
            glGenTextures(1, &texture);
        }

        /** @copydoc AbstractTexture::~AbstractTexture() */
        inline virtual ~BufferedTexture() {
            glDeleteTextures(1, &texture);
        }

        /** @copydoc AbstractTexture::bind(GLint) */
        inline void bind(GLint layer) {
            glActiveTexture(GL_TEXTURE0 + layer);
            bind();
        }

        /**
         * @brief Set texture buffer
         * @param internalFormat    Internal format
         * @param buffer            %Buffer with data
         *
         * Binds given buffer to this texture. The buffer itself can be then
         * filled with data of proper format at any time using Buffer own data
         * setting functions.
         */
        void setBuffer(InternalFormat internalFormat, Buffer* buffer) {
            bind();
            glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, buffer->id());
        }

    private:
        GLuint texture;

        /** @copydoc AbstractTexture::bind() */
        inline void bind() {
            glBindTexture(GL_TEXTURE_BUFFER, texture);
        }
};

/** @relates BufferedTexture
@brief Convertor of component count and data type to InternalFormat
*/
inline BufferedTexture::InternalFormat operator|(BufferedTexture::Components components, BufferedTexture::ComponentType type) {
    return BufferedTexture::InternalFormat(components, type);
}
/** @relates BufferedTexture
 * @overload
 */
inline BufferedTexture::InternalFormat operator|(BufferedTexture::ComponentType type, BufferedTexture::Components components) {
    return BufferedTexture::InternalFormat(components, type);
}

}

#endif
