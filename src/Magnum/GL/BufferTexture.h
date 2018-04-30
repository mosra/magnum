#ifndef Magnum_GL_BufferTexture_h
#define Magnum_GL_BufferTexture_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
 * @brief Class @ref Magnum::GL::BufferTexture
 */
#endif

#include "Magnum/GL/AbstractTexture.h"

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
namespace Magnum { namespace GL {

/**
@brief Buffer texture

This texture is, unlike classic textures such as @ref Texture, used as simple
data source, without any unnecessary interpolation and wrapping methods.

@section GL-BufferTexture-usage Usage

Texture data are stored in buffer and after binding the buffer to the texture
using @ref setBuffer(), you can fill the buffer at any time using data setting
functions in Buffer itself.

Note that the buffer is not managed (e.g. deleted on destruction) by the
texture, so you have to manage it on your own and ensure that it is available
for whole texture lifetime. On the other hand it allows you to use one buffer
for more textures or store more than one data in it.

Example usage:

@snippet MagnumGL.cpp BufferTexture-usage

In shader, the texture is used via @glsl samplerBuffer @ce,
@glsl isamplerBuffer @ce or @glsl usamplerBuffer @ce. Unlike in classic
textures, coordinates for buffer textures are integer coordinates passed to
@glsl texelFetch() @ce. See @ref AbstractShaderProgram documentation for more
information about usage in shaders.

@section GL-BufferTexture-performance-optimizations Performance optimizations

If either @extension{ARB,direct_state_access} (part of OpenGL 4.5) or
@extension{EXT,direct_state_access} is available, @ref setBuffer() functions
use DSA to avoid unnecessary calls to @fn_gl{ActiveTexture} and
@fn_gl{BindTexture}. See
@ref GL-AbstractTexture-performance-optimization "relevant section in AbstractTexture documentation"
and respective function documentation for more information.

@see @ref Texture, @ref TextureArray, @ref CubeMapTexture,
    @ref CubeMapTextureArray, @ref RectangleTexture, @ref MultisampleTexture
@m_keywords{GL_TEXTURE_BUFFER}
@requires_gl31 Extension @extension{ARB,texture_buffer_object}
@requires_gles30 Not defined in OpenGL ES 2.0.
@requires_gles32 Extension @extension{ANDROID,extension_pack_es31a} /
    @extension{EXT,texture_buffer}
@requires_gles Texture buffers are not available in WebGL.
*/
class MAGNUM_GL_EXPORT BufferTexture: public AbstractTexture {
    friend Implementation::TextureState;

    public:
        /**
         * @brief Max supported buffer texture size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If @extension{ARB,texture_buffer_object} (part of
         * OpenGL 3.1) is not available, returns @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_TEXTURE_BUFFER_SIZE}
         */
        static Int maxSize();

        /**
         * @brief Minimum required alignment for texture buffer offsets
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,texture_buffer_range}
         * (part of OpenGL 4.3) is not available, returns @cpp 1 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{TEXTURE_BUFFER_OFFSET_ALIGNMENT}
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
         * @see @ref BufferTexture(NoCreateT), @ref wrap(),
         *      @fn_gl_keyword{CreateTextures} with @def_gl{TEXTURE_BUFFER},
         *      eventually @fn_gl_keyword{GenTextures}
         */
        explicit BufferTexture(): AbstractTexture(GL_TEXTURE_BUFFER) {}

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
        explicit BufferTexture(NoCreateT) noexcept: AbstractTexture{NoCreate, GL_TEXTURE_BUFFER} {}

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
         *      @fn_gl_keyword{BindImageTexture}
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
         * @see @ref maxSize(), @fn_gl2_keyword{TextureBuffer,TexBuffer},
         *      @fn_gl_extension_keyword{TextureBuffer,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexBuffer}
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
         * @see @ref maxSize(), @fn_gl2_keyword{TextureBufferRange,TexBufferRange},
         *      @fn_gl_extension_keyword{TextureBufferRange,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexBufferRange}
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
        explicit BufferTexture(GLuint id, ObjectFlags flags): AbstractTexture{id, GL_TEXTURE_BUFFER, flags} {}

        void MAGNUM_GL_LOCAL setBufferImplementationDefault(BufferTextureFormat internalFormat, Buffer& buffer);
        #ifdef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL setBufferImplementationEXT(BufferTextureFormat internalFormat, Buffer& buffer);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL setBufferImplementationDSA(BufferTextureFormat internalFormat, Buffer& buffer);
        void MAGNUM_GL_LOCAL setBufferImplementationDSAEXT(BufferTextureFormat internalFormat, Buffer& buffer);
        #endif

        void MAGNUM_GL_LOCAL setBufferRangeImplementationDefault(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size);
        #ifdef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL setBufferRangeImplementationEXT(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL setBufferRangeImplementationDSA(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size);
        void MAGNUM_GL_LOCAL setBufferRangeImplementationDSAEXT(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size);
        #endif
};

}

#ifdef MAGNUM_BUILD_DEPRECATED
/* Note: needs to be prefixed with Magnum:: otherwise Doxygen can't find it */

/** @brief @copybrief GL::BufferTexture
 * @deprecated Use @ref GL::BufferTexture instead.
 */
typedef CORRADE_DEPRECATED("use GL::BufferTexture instead") Magnum::GL::BufferTexture BufferTexture;
#endif

}
#else
#error this header is not available in OpenGL ES 2.0 and WebGL build
#endif

#endif
