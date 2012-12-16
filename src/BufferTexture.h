#ifndef Magnum_BufferTexture_h
#define Magnum_BufferTexture_h
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

#ifndef MAGNUM_TARGET_GLES
/** @file
 * @brief Class Magnum::BufferTexture
 */
#endif

#include "AbstractTexture.h"

#ifndef MAGNUM_TARGET_GLES
namespace Magnum {

/**
@brief %Buffer texture

This texture is, unlike classic textures such as Texture or CubeMapTexture,
used as simple data source, without any unnecessary interpolation and
wrapping methods.

@section BufferTexture-usage Usage

%Texture data are stored in buffer and after binding the buffer to the texture
using setBuffer(), you can fill the buffer at any time using data setting
functions in Buffer itself.

Note that the buffer is not managed (e.g. deleted on destruction) by the
texture, so you have to manage it on your own. On the other hand it allows you
to use one buffer for more textures or store more than one data in it.

Example usage:
@code
Buffer* buffer;
BufferTexture texture;
texture.setBuffer(buffer);

constexpr static Vector3 data[] = {
    // ...
};
buffer.setData(data, Buffer::Usage::StaticDraw);
@endcode

The texture is bound to layer specified by shader via bind(). In shader, the
texture is used via `samplerBuffer`.  Unlike in classic textures, coordinates
for buffer textures are integer coordinates passed to `texelFetch()`. See also
AbstractShaderProgram documentation for more information.

@section BufferTexture-performance-optimization Performance optimizations
If extension @extension{EXT,direct_state_access} is available, setBuffer()
uses DSA function to avoid unnecessary calls to @fn_gl{ActiveTexture} and
@fn_gl{BindTexture}. See @ref AbstractTexture-performance-optimization
"relevant section in AbstractTexture documentation" and respective function
documentation for more information.

@requires_gl31 %Extension @extension{ARB,texture_buffer_object}
@requires_gl Texture buffers are not available in OpenGL ES.
*/
class MAGNUM_EXPORT BufferTexture: private AbstractTexture {
    friend class Context;

    BufferTexture(const BufferTexture& other) = delete;
    BufferTexture(BufferTexture&& other) = delete;
    BufferTexture& operator=(const BufferTexture& other) = delete;
    BufferTexture& operator=(BufferTexture&& other) = delete;

    public:
        /**
         * @brief Internal format
         *
         * @see setBuffer()
         */
        enum class InternalFormat: GLenum {
            /** Red component, normalized unsigned byte. */
            R8 = GL_R8,

            /** Red and green component, each normalized unsigned byte. */
            RG8 = GL_RG8,

            /** RGBA, each component normalized unsigned byte. */
            RGBA8 = GL_RGBA8,

            /** Red component, normalized unsigned short. */
            R16 = GL_R16,

            /** Red and green component, each normalized unsigned short. */
            RG16 = GL_RG16,

            /** RGBA, each component normalized unsigned short. */
            RGBA16 = GL_RGBA16,

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
             * @requires_gl40 %Extension @extension{ARB,texture_buffer_object_rgb32}
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
             * @requires_gl40 %Extension @extension{ARB,texture_buffer_object_rgb32}
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
             * @requires_gl40 %Extension @extension{ARB,texture_buffer_object_rgb32}
             */
            RGB32F = GL_RGB32F,

            /** RGBA, each component float. */
            RGBA32F = GL_RGBA32F
        };

        inline explicit BufferTexture(): AbstractTexture(GL_TEXTURE_BUFFER) {}

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

        typedef void(BufferTexture::*SetBufferImplementation)(InternalFormat, Buffer*);
        void MAGNUM_LOCAL setBufferImplementationDefault(InternalFormat internalFormat, Buffer* buffer);
        void MAGNUM_LOCAL setBufferImplementationDSA(InternalFormat internalFormat, Buffer* buffer);
        static SetBufferImplementation setBufferImplementation;
};

}
#endif

#endif
