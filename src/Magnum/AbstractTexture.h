#ifndef Magnum_AbstractTexture_h
#define Magnum_AbstractTexture_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

/** @file
 * @brief Class @ref Magnum::AbstractTexture
 */

#include "Magnum/Sampler.h"
#include "Magnum/AbstractObject.h"

namespace Magnum {

namespace Implementation { struct TextureState; }

/**
@brief Base for textures

Encapsulates one OpenGL texture object. See @ref Texture, @ref CubeMapTexture
and @ref CubeMapTextureArray documentation for more information and usage
examples.

@section AbstractTexture-performance-optimization Performance optimizations and security

The engine tracks currently bound textures in all available layers to avoid
unnecessary calls to @fn_gl{ActiveTexture} and @fn_gl{BindTexture}. %Texture
configuration functions use dedicated highest available texture layer to not
affect active bindings in user layers. %Texture limits and
implementation-defined values (such as @ref maxColorSamples()) are cached, so
repeated queries don't result in repeated @fn_gl{Get} calls.

If extension @extension{EXT,direct_state_access} is available, @ref bind() uses
DSA function to avoid unnecessary calls to @fn_gl{ActiveTexture}. Also all
texture configuration and data updating functions use DSA functions to avoid
unnecessary calls to @fn_gl{ActiveTexture} and @fn_gl{BindTexture}. See
respective function documentation for more information.

If extension @extension{ARB,robustness} is available, image reading operations
(such as @ref Texture::image()) are protected from buffer overflow. However, if
both @extension{EXT,direct_state_access} and @extension{ARB,robustness} are
available, the DSA version is used, because it is better for performance and
there isn't any function combining both features.

To achieve least state changes, fully configure each texture in one run --
method chaining comes in handy -- and try to have often used textures in
dedicated layers, not occupied by other textures. First configure the texture
and *then* set the data, so OpenGL can optimize them to match the settings. To
avoid redundant consistency checks and memory reallocations when updating
texture data, set texture storage at once using @ref Texture::setStorage() "setStorage()"
and then set data using @ref Texture::setSubImage() "setSubImage()".

Function @ref Texture::setStorage() "setStorage()" creates immutable texture
storage, removing the need for additional consistency checks and memory
reallocations when updating the data later. If OpenGL 4.2, @extension{ARB,texture_storage},
OpenGL ES 3.0 or @es_extension{EXT,texture_storage} in OpenGL ES 2.0 is not
available, the feature is emulated with sequence of @ref Texture::setImage() "setImage()"
calls.

You can use functions @ref Texture::invalidateImage() and
@ref Texture::invalidateSubImage() "invalidateSubImage()" if you don't need
texture data anymore to avoid unnecessary memory operations performed by OpenGL
in order to preserve the data. If running on OpenGL ES or extension
@extension{ARB,invalidate_subdata} is not available, these functions do
nothing.

@todo all texture [level] parameters, global texture parameters
@todo Add glPixelStore encapsulation
@todo Texture copying
@todo Move constructor/assignment - how to avoid creation of empty texture and
    then deleting it immediately?
@todo ES2 - proper support for pixel unpack buffer when extension is in headers
@todo `GL_MAX_3D_TEXTURE_SIZE`, `GL_MAX_ARRAY_TEXTURE_LAYERS`, `GL_MAX_CUBE_MAP_TEXTURE_SIZE`, `GL_MAX_RECTANGLE_TEXTURE_SIZE`, `GL_MAX_TEXTURE_SIZE`, `GL_MAX_TEXTURE_BUFFER_SIZE` enable them only where it makes sense?
@todo `GL_MAX_TEXTURE_LOD_BIAS` when `TEXTURE_LOD_BIAS` is implemented
@todo `GL_NUM_COMPRESSED_TEXTURE_FORMATS` when compressed textures are implemented
@todo `GL_MAX_SAMPLE_MASK_WORDS` when @extension{ARB,texture_multisample} is done
@todo Query for immutable levels (@extension{ARB,ES3_compatibility})
*/
class MAGNUM_EXPORT AbstractTexture: public AbstractObject {
    friend struct Implementation::TextureState;

    public:
        /**
         * @brief Max supported layer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. This function is in fact alias to
         * @ref Shader::maxCombinedTextureImageUnits().
         * @see @ref bind(Int)
         */
        static Int maxLayers();

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief maxLayers()
         * @deprecated Use @ref Magnum::AbstractTexture::maxLayers() "maxLayers()"
         *      instead.
         */
        static CORRADE_DEPRECATED("use maxLayers() instead") Int maxSupportedLayerCount() { return maxLayers(); }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Max supported color sample count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,texture_multisample} is
         * not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_COLOR_TEXTURE_SAMPLES}
         * @requires_gl Multisample textures are not available in OpenGL ES.
         */
        static Int maxColorSamples();

        /**
         * @brief Max supported depth sample count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,texture_multisample} is
         * not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_DEPTH_TEXTURE_SAMPLES}
         * @requires_gl Multisample textures are not available in OpenGL ES.
         */
        static Int maxDepthSamples();

        /**
         * @brief Max supported integer sample count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,texture_multisample} is
         * not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_INTEGER_SAMPLES}
         * @requires_gl Multisample textures are not available in OpenGL ES.
         */
        static Int maxIntegerSamples();
        #endif

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL texture.
         * @see @fn_gl{DeleteTextures}
         */
        ~AbstractTexture();

        /** @brief Copying is not allowed */
        AbstractTexture(const AbstractTexture&) = delete;

        /** @brief Move constructor */
        AbstractTexture(AbstractTexture&& other) noexcept;

        /** @brief Copying is not allowed */
        AbstractTexture& operator=(const AbstractTexture&) = delete;

        /** @brief Move assignment */
        AbstractTexture& operator=(AbstractTexture&& other) noexcept;

        /**
         * @brief %Texture label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If neither @extension{KHR,debug} nor
         * @extension2{EXT,debug_label} desktop or ES extension is available,
         * this function returns empty string.
         * @see @fn_gl{GetObjectLabel} or
         *      @fn_gl_extension2{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{TEXTURE}
         */
        std::string label() const;

        /**
         * @brief Set texture label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If neither @extension{KHR,debug} nor
         * @extension2{EXT,debug_label} desktop or ES extension is available,
         * this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} or
         *      @fn_gl_extension2{LabelObject,EXT,debug_label} with
         *      @def_gl{TEXTURE}
         */
        AbstractTexture& setLabel(const std::string& label);

        /** @brief OpenGL texture ID */
        GLuint id() const { return _id; }

        /**
         * @brief Bind texture for rendering
         *
         * Sets current texture as active in given layer. The layer must be
         * between 0 and @ref maxLayers(). Note that only one texture can be
         * bound to given layer. If @extension{EXT,direct_state_access} is not
         * available, the layer is made active before binding the texture.
         * @see @ref maxLayers(), @fn_gl{ActiveTexture}, @fn_gl{BindTexture} or
         *      @fn_gl_extension{BindMultiTexture,EXT,direct_state_access}
         */
        void bind(Int layer);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        template<UnsignedInt textureDimensions> struct DataHelper {};

        explicit AbstractTexture(GLenum target);

        /* Unlike bind() this also sets the binding layer as active */
        void MAGNUM_LOCAL bindInternal();

        void setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap);
        void setMagnificationFilter(Sampler::Filter filter);
        void setBorderColor(const Color4& color);
        void setMaxAnisotropy(Float anisotropy);
        void invalidateImage(Int level);
        void generateMipmap();

        #ifndef MAGNUM_TARGET_GLES
        template<UnsignedInt dimensions> void image(GLenum target, GLint level, Image<dimensions>& image);
        template<UnsignedInt dimensions> void image(GLenum target, GLint level, BufferImage<dimensions>& image, BufferUsage usage);
        #endif

        GLenum _target;

    private:
        void MAGNUM_LOCAL bindImplementationDefault(GLint layer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL bindImplementationDSA(GLint layer);
        #endif

        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, GLint value);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, GLint value);
        #endif

        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, GLfloat value);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, GLfloat value);
        #endif

        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, const GLfloat* values);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, const GLfloat* values);
        #endif

        void MAGNUM_LOCAL setMaxAnisotropyImplementationNoOp(GLfloat);
        void MAGNUM_LOCAL setMaxAnisotropyImplementationExt(GLfloat anisotropy);

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL getLevelParameterImplementationDefault(GLenum target, GLint level, GLenum parameter, GLint* values);
        void MAGNUM_LOCAL getLevelParameterImplementationDSA(GLenum target, GLint level, GLenum parameter, GLint* values);
        #endif

        void MAGNUM_LOCAL mipmapImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL mipmapImplementationDSA();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationFallback(GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        #endif

        void MAGNUM_LOCAL storageImplementationFallback(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        #endif

        void MAGNUM_LOCAL storageImplementationFallback(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL getImageImplementationDefault(GLenum target, GLint level, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationDSA(GLenum target, GLint level, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationRobustness(GLenum target, GLint level, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif

        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif

        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif

        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif

        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif

        void MAGNUM_LOCAL invalidateImageImplementationNoOp(GLint level);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateImageImplementationARB(GLint level);
        #endif

        void MAGNUM_LOCAL invalidateSubImageImplementationNoOp(GLint level, const Vector3i& offset, const Vector3i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateSubImageImplementationARB(GLint level, const Vector3i& offset, const Vector3i& size);
        #endif

        ColorFormat MAGNUM_LOCAL imageFormatForInternalFormat(TextureFormat internalFormat);
        ColorType MAGNUM_LOCAL imageTypeForInternalFormat(TextureFormat internalFormat);

        GLuint _id;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_GLES
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<1> {
    #ifdef MAGNUM_BUILD_DEPRECATED
    enum class Target: GLenum {
        Texture1D = GL_TEXTURE_1D
    };
    #endif

    static Math::Vector<1, GLint> imageSize(AbstractTexture& texture, GLenum target, GLint level);

    static void setWrapping(AbstractTexture& texture, const Array1D<Sampler::Wrapping>& wrapping);

    static void setStorage(AbstractTexture& texture, GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);

    static void setImage(AbstractTexture& texture, GLenum target, GLint level, TextureFormat internalFormat, const ImageReference1D& image);
    static void setImage(AbstractTexture& texture, GLenum target, GLint level, TextureFormat internalFormat, BufferImage1D& image);

    static void setSubImage(AbstractTexture& texture, GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const ImageReference1D& image);
    static void setSubImage(AbstractTexture& texture, GLenum target, GLint level, const Math::Vector<1, GLint>& offset, BufferImage1D& image);

    static void invalidateSubImage(AbstractTexture& texture, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLint>& size);
};
#endif
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<2> {
    #ifdef MAGNUM_BUILD_DEPRECATED
    enum class Target: GLenum {
        Texture2D = GL_TEXTURE_2D,
        #ifndef MAGNUM_TARGET_GLES
        Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
        Texture1DArray = GL_TEXTURE_1D_ARRAY,
        Rectangle = GL_TEXTURE_RECTANGLE
        #endif
    };
    #endif

    #ifndef MAGNUM_TARGET_GLES
    static Vector2i imageSize(AbstractTexture& texture, GLenum target, GLint level);
    #endif

    static void setWrapping(AbstractTexture& texture, const Array2D<Sampler::Wrapping>& wrapping);

    static void setStorage(AbstractTexture& texture, GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size);

    static void setImage(AbstractTexture& texture, GLenum target, GLint level, TextureFormat internalFormat, const ImageReference2D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setImage(AbstractTexture& texture, GLenum target, GLint level, TextureFormat internalFormat, BufferImage2D& image);
    #endif

    static void setSubImage(AbstractTexture& texture, GLenum target, GLint level, const Vector2i& offset, const ImageReference2D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setSubImage(AbstractTexture& texture, GLenum target, GLint level, const Vector2i& offset, BufferImage2D& image);
    #endif

    static void invalidateSubImage(AbstractTexture& texture, GLint level, const Vector2i& offset, const Vector2i& size);
};
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<3> {
    #ifdef MAGNUM_BUILD_DEPRECATED
    enum class Target: GLenum {
        #ifndef MAGNUM_TARGET_GLES2
        Texture3D = GL_TEXTURE_3D,
        Texture2DArray = GL_TEXTURE_2D_ARRAY,
        #ifndef MAGNUM_TARGET_GLES
        Texture2DMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
        #endif
        #else
        Texture3D = GL_TEXTURE_3D_OES
        #endif
    };
    #endif

    #ifndef MAGNUM_TARGET_GLES
    static Vector3i imageSize(AbstractTexture& texture, GLenum target, GLint level);
    #endif

    static void setWrapping(AbstractTexture& texture, const Array3D<Sampler::Wrapping>& wrapping);

    static void setStorage(AbstractTexture& texture, GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size);

    static void setImage(AbstractTexture& texture, GLenum target, GLint level, TextureFormat internalFormat, const ImageReference3D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setImage(AbstractTexture& texture, GLenum target, GLint level, TextureFormat internalFormat, BufferImage3D& image);
    #endif

    static void setSubImage(AbstractTexture& texture, GLenum target, GLint level, const Vector3i& offset, const ImageReference3D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setSubImage(AbstractTexture& texture, GLenum target, GLint level, const Vector3i& offset, BufferImage3D& image);
    #endif

    static void invalidateSubImage(AbstractTexture& texture, GLint level, const Vector3i& offset, const Vector3i& size);
};
#endif

inline AbstractTexture::AbstractTexture(AbstractTexture&& other) noexcept: _target(other._target), _id(other._id) {
    other._id = 0;
}

inline AbstractTexture& AbstractTexture::operator=(AbstractTexture&& other) noexcept {
    std::swap(_target, other._target);
    std::swap(_id, other._id);
    return *this;
}

}

#endif
