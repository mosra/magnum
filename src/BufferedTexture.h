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

#include "AbstractTexture.h"

namespace Magnum {

class Buffer;
class Context;

#ifndef MAGNUM_TARGET_GLES
/**
@brief Buffered texture

This texture is, unlike classic textures such as Texture or CubeMapTexture,
used as simple data source, without any unneccessary interpolation and
wrapping methods. Texture data are stored in buffer and after binding the
buffer to the texture using setBuffer(), you can fill the buffer at any time
using data setting functions in Buffer itself.

When using buffered texture in the shader, use `samplerBuffer` and fetch the
data using integer coordinates in `texelFetch()`.

@section BufferedTexture-performance-optimization Performance optimizations
If extension @extension{EXT,direct_state_access} is available, setBuffer()
uses DSA function to avoid unnecessary calls to @fn_gl{ActiveTexture} and
@fn_gl{BindTexture}. See @ref AbstractTexture-performance-optimization
"relevant section in AbstractTexture documentation" and respective function
documentation for more information.

@requires_gl
@requires_gl31 Extension @extension{ARB,texture_buffer_object}
*/
class MAGNUM_EXPORT BufferedTexture: private AbstractTexture {
    friend class Context;

    BufferedTexture(const BufferedTexture& other) = delete;
    BufferedTexture(BufferedTexture&& other) = delete;
    BufferedTexture& operator=(const BufferedTexture& other) = delete;
    BufferedTexture& operator=(BufferedTexture&& other) = delete;

    public:
        /** @{ @name Internal buffered texture formats */

        /**
         * @copybrief AbstractTexture::Components
         *
         * Like AbstractTexture::Components, without three-component RGB.
         */
        enum class Components {
            Red, RedGreen, RGBA
        };

        /**
         * @copybrief AbstractTexture::ComponentType
         *
         * Like AbstractTexture::ComponentType, without normalized signed
         * types.
         */
        enum class ComponentType {
            UnsignedByte, Byte, UnsignedShort, Short, UnsignedInt, Int, Half,
            Float, NormalizedUnsignedByte, NormalizedUnsignedShort
        };

        /** @copybrief AbstractTexture::Format */
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
                /** @copybrief AbstractTexture::InternalFormat::InternalFormat(AbstractTexture::Components, AbstractTexture::ComponentType) */
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

        inline BufferedTexture(): AbstractTexture(GL_TEXTURE_BUFFER) {}

        /** @copydoc AbstractTexture::bind() */
        inline void bind(GLint layer) { AbstractTexture::bind(layer); }

        /**
         * @brief Set texture buffer
         * @param internalFormat    Internal format
         * @param buffer            %Buffer with data
         *
         * Binds given buffer to this texture. The buffer itself can be then
         * filled with data of proper format at any time using Buffer own data
         * setting functions.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexBuffer}
         *      or @fn_gl_extension{TextureBuffer,EXT,direct_state_access}
         */
        inline void setBuffer(InternalFormat internalFormat, Buffer* buffer) {
            (this->*setBufferImplementation)(internalFormat, buffer);
        }

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

        typedef void(BufferedTexture::*SetBufferImplementation)(InternalFormat, Buffer*);
        void MAGNUM_LOCAL setBufferImplementationDefault(InternalFormat internalFormat, Buffer* buffer);
        void MAGNUM_LOCAL setBufferImplementationDSA(InternalFormat internalFormat, Buffer* buffer);
        static SetBufferImplementation setBufferImplementation;
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
#endif

}

#endif
