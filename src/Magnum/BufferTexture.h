#ifndef Magnum_BufferTexture_h
#define Magnum_BufferTexture_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::BufferTexture
 */
#endif

#include "Magnum/AbstractTexture.h"

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
namespace Magnum {

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
@requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
    @extension{EXT,texture_buffer}
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
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * @see @ref BufferTexture(), @ref wrap()
         */
        explicit BufferTexture(NoCreateT) noexcept:
            #ifndef MAGNUM_TARGET_GLES
            AbstractTexture{NoCreate, GL_TEXTURE_BUFFER} {}
            #else
            AbstractTexture{NoCreate, GL_TEXTURE_BUFFER_EXT} {}
            #endif

        /**
         * @brief Bind texture to given image unit
         * @param imageUnit Image unit
         * @param access    Image access
         * @param format    Image format
         *
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bindImages(Int, std::initializer_list<AbstractTexture*>),
         *      @ref unbindImage(), @ref unbindImages(),
         *      @ref AbstractShaderProgram::maxImageUnits(),
         *      @fn_gl{BindImageTexture}
         * @requires_gl42 Extension @extension{ARB,shader_image_load_store}
         * @requires_gles31 Shader image load/store is not available in OpenGL
         *      ES 3.0 and older.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        void bindImage(Int imageUnit, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, 0, false, 0, access, format);
        }

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
