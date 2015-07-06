#ifndef Magnum_BufferTexture_h
#define Magnum_BufferTexture_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::BufferTexture, enum @ref Magnum::BufferTextureFormat
 */
#endif

#include "Magnum/AbstractTexture.h"

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
namespace Magnum {

/**
@brief Internal buffer texture format

@see @ref BufferTexture
@requires_gl31 Extension @extension{ARB,texture_buffer_object}
@requires_es_extension Extension @es_extension{ANDROID,extension_pack_es31a}/
    @es_extension{EXT,texture_buffer}
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
     * @requires_gl40 Extension @extension{ARB,texture_buffer_object_rgb32}
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
     * @requires_gl40 Extension @extension{ARB,texture_buffer_object_rgb32}
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
     * @requires_gl40 Extension @extension{ARB,texture_buffer_object_rgb32}
     */
    RGB32F = GL_RGB32F,

    /** RGBA, each component float. */
    RGBA32F = GL_RGBA32F
};

/**
@brief Buffer texture

This texture is, unlike classic textures such as @ref Texture, used as simple
data source, without any unnecessary interpolation and wrapping methods.

## Usage

Texture data are stored in buffer and after binding the buffer to the texture
using @ref setBuffer(), you can fill the buffer at any time using data setting
functions in Buffer itself.

Note that the buffer is not managed (e.g. deleted on destruction) by the
texture, so you have to manage it on your own and ensure that it is available
for whole texture lifetime. On the other hand it allows you to use one buffer
for more textures or store more than one data in it.

Example usage:
@code
Buffer buffer;
BufferTexture texture;
texture.setBuffer(BufferTextureFormat::RGB32F, buffer);

constexpr static Vector3 data[] = {
    // ...
};
buffer.setData(data, BufferUsage::StaticDraw);
@endcode

In shader, the texture is used via `samplerBuffer`, `isamplerBuffer` or
`usamplerBuffer`. Unlike in classic textures, coordinates for buffer textures
are integer coordinates passed to `texelFetch()`. See @ref AbstractShaderProgram
documentation for more information about usage in shaders.

## Performance optimizations

If either @extension{ARB,direct_state_access} (part of OpenGL 4.5) or
@extension{EXT,direct_state_access} is available, @ref setBuffer() functions
use DSA to avoid unnecessary calls to @fn_gl{ActiveTexture} and
@fn_gl{BindTexture}. See
@ref AbstractTexture-performance-optimization "relevant section in AbstractTexture documentation"
and respective function documentation for more information.

@see @ref Texture, @ref TextureArray, @ref CubeMapTexture,
    @ref CubeMapTextureArray, @ref RectangleTexture, @ref MultisampleTexture
@requires_gl31 Extension @extension{ARB,texture_buffer_object}
@requires_gles30 Not defined in OpenGL ES 2.0.
@requires_es_extension Extension @es_extension{ANDROID,extension_pack_es31a}/
    @es_extension{EXT,texture_buffer}
@requires_gles Texture buffers are not available in WebGL.
*/
class MAGNUM_EXPORT BufferTexture: public AbstractTexture {
    friend Implementation::TextureState;

    public:
        /**
         * @brief Max supported buffer texture size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If @extension{ARB,texture_buffer_object} (part of
         * OpenGL 3.1) is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_TEXTURE_BUFFER_SIZE}
         */
        static Int maxSize();

        /**
         * @brief Minimum required alignment for texture buffer offsets
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,texture_buffer_range}
         * (part of OpenGL 4.3) is not available, returns `1`.
         * @see @fn_gl{Get} with @def_gl{TEXTURE_BUFFER_OFFSET_ALIGNMENT}
         */
        static Int offsetAlignment();

        /**
         * @brief Wrap existing OpenGL buffer texture object
         * @param id            OpenGL buffer texture ID
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL texture object
         * with target @def_gl{TEXTURE_BUFFER}. Unlike texture created using
         * constructor, the OpenGL object is by default not deleted on
         * destruction, use @p flags for different behavior.
         * @see @ref release()
         */
        static BufferTexture wrap(GLuint id, ObjectFlags flags = {}) {
            return BufferTexture{id, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref BufferTexture(NoCreateT), @ref wrap(), @fn_gl{CreateTextures}
         *      with @def_gl{TEXTURE_BUFFER}, eventually @fn_gl{GenTextures}
         */
        explicit BufferTexture():
            #ifndef MAGNUM_TARGET_GLES
            AbstractTexture(GL_TEXTURE_BUFFER) {}
            #else
            AbstractTexture(GL_TEXTURE_BUFFER_EXT) {}
            #endif

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         * @see @ref BufferTexture(), @ref wrap()
         */
        explicit BufferTexture(NoCreateT) noexcept:
            #ifndef MAGNUM_TARGET_GLES
            AbstractTexture{NoCreate, GL_TEXTURE_BUFFER} {}
            #else
            AbstractTexture{NoCreate, GL_TEXTURE_BUFFER_EXT} {}
            #endif

        /**
         * @brief Set texture buffer
         * @param internalFormat    Internal format
         * @param buffer            Buffer with data
         * @return Reference to self (for method chaining)
         *
         * Binds given buffer to this texture. The buffer itself can be then
         * filled with data of proper format at any time using @ref Buffer "Buffer"'s
         * own data setting functions. If neither @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) nor @extension{EXT,direct_state_access} is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref maxSize(), @fn_gl2{TextureBuffer,TexBuffer},
         *      @fn_gl_extension{TextureBuffer,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexBuffer}
         */
        BufferTexture& setBuffer(BufferTextureFormat internalFormat, Buffer& buffer);

        /**
         * @brief Set texture buffer
         * @param internalFormat    Internal format
         * @param buffer            Buffer
         * @param offset            Offset
         * @param size              Data size
         * @return Reference to self (for method chaining)
         *
         * Binds range of given buffer to this texture. The buffer itself can
         * be then filled with data of proper format at any time using @ref Buffer "Buffer"'s
         * own data setting functions. If neither @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) nor @extension{EXT,direct_state_access} is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref maxSize(), @fn_gl2{TextureBufferRange,TexBufferRange},
         *      @fn_gl_extension{TextureBufferRange,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexBufferRange}
         * @requires_gl43 Extension @extension{ARB,texture_buffer_range}
         */
        BufferTexture& setBuffer(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        BufferTexture& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        template<std::size_t size> BufferTexture& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit BufferTexture(GLuint id, ObjectFlags flags): AbstractTexture{id,
            #ifndef MAGNUM_TARGET_GLES
            GL_TEXTURE_BUFFER,
            #else
            GL_TEXTURE_BUFFER_EXT,
            #endif
            flags} {}

        void MAGNUM_LOCAL setBufferImplementationDefault(BufferTextureFormat internalFormat, Buffer& buffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL setBufferImplementationDSA(BufferTextureFormat internalFormat, Buffer& buffer);
        void MAGNUM_LOCAL setBufferImplementationDSAEXT(BufferTextureFormat internalFormat, Buffer& buffer);
        #endif

        void MAGNUM_LOCAL setBufferRangeImplementationDefault(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL setBufferRangeImplementationDSA(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size);
        void MAGNUM_LOCAL setBufferRangeImplementationDSAEXT(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size);
        #endif
};

}
#else
#error this header is not available in OpenGL ES 2.0 and WebGL build
#endif

#endif
