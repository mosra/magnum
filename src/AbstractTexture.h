#ifndef Magnum_AbstractTexture_h
#define Magnum_AbstractTexture_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::AbstractTexture
 */

#include "Array.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Buffer.h"
#endif
#include "Color.h"
#include "Sampler.h"

#ifdef CORRADE_GCC45_COMPATIBILITY
#include "ImageFormat.h"
#include "TextureFormat.h"
#endif

namespace Magnum {

/**
@brief Base for textures

See Texture, CubeMapTexture and CubeMapTextureArray documentation for more
information and usage examples.

@section AbstractTexture-performance-optimization Performance optimizations and security

The engine tracks currently bound textures in all available layers to avoid
unnecessary calls to @fn_gl{ActiveTexture} and @fn_gl{BindTexture}. %Texture
configuration functions use dedicated highest available texture layer to not
affect active bindings in user layers. %Texture limits and
implementation-defined values (such as @ref maxColorSamples()) are cached, so
repeated queries don't result in repeated @fn_gl{Get} calls.

If extension @extension{EXT,direct_state_access} is available, bind() uses DSA
function to avoid unnecessary calls to @fn_gl{ActiveTexture}. Also all texture
configuration and data updating functions use DSA functions to avoid
unnecessary calls to @fn_gl{ActiveTexture} and @fn_gl{BindTexture}. See
respective function documentation for more information.

If extension @extension{ARB,robustness} is available, image reading operations
(such as Texture::image()) are protected from buffer overflow. However, if both
@extension{EXT,direct_state_access} and @extension{ARB,robustness} are
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

You can use functions invalidateImage() and @ref Texture::invalidateSubImage() "invalidateSubImage()"
if you don't need texture data anymore to avoid unnecessary memory operations
performed by OpenGL in order to preserve the data. If running on OpenGL ES or
extension @extension{ARB,invalidate_subdata} is not available, these functions
do nothing.

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
*/
class MAGNUM_EXPORT AbstractTexture {
    friend class Context;

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

        /**
         * @copybrief maxLayers()
         * @deprecated Use @ref Magnum::AbstractTexture::maxLayers() "maxLayers()"
         *      instead.
         */
        static Int maxSupportedLayerCount() { return maxLayers(); }

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

        /** @brief Copying is not allowed */
        AbstractTexture(const AbstractTexture&) = delete;

        /** @brief Move constructor */
        AbstractTexture(AbstractTexture&& other);

        /** @brief Copying is not allowed */
        AbstractTexture& operator=(const AbstractTexture&) = delete;

        /** @brief Move assignment */
        AbstractTexture& operator=(AbstractTexture&& other);

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

        /**
         * @brief Set minification filter
         * @param filter        Filter
         * @param mipmap        Mipmap filtering. If set to anything else than
         *      BaseMipLevel, make sure textures for all mip levels are set or
         *      call generateMipmap().
         * @return Reference to self (for method chaining)
         *
         * Sets filter used when the object pixel size is smaller than the
         * texture size. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some layer before the operation.
         * Initial value is (@ref Sampler::Filter::Nearest, @ref Sampler::Mipmap::Linear).
         * @attention For rectangle textures only some modes are supported,
         *      see @ref Sampler::Filter and @ref Sampler::Mipmap documentation
         *      for more information.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_MIN_FILTER}
         */
        AbstractTexture& setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base);

        /**
         * @brief Set magnification filter
         * @param filter        Filter
         * @return Reference to self (for method chaining)
         *
         * Sets filter used when the object pixel size is larger than largest
         * texture size. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some layer before the operation.
         * Initial value is @ref Sampler::Filter::Linear.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_MAG_FILTER}
         */
        AbstractTexture& setMagnificationFilter(Sampler::Filter filter) {
            (this->*parameteriImplementation)(GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES3
        /**
         * @brief Set border color
         * @return Reference to self (for method chaining)
         *
         * Border color when wrapping is set to @ref Sampler::Wrapping::ClampToBorder.
         * If @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some layer before the operation. Initial value is
         * `{0.0f, 0.0f, 0.0f, 0.0f}`.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_BORDER_COLOR}
         * @requires_es_extension %Extension @es_extension{NV,texture_border_clamp}
         */
        AbstractTexture& setBorderColor(const Color4& color) {
            #ifndef MAGNUM_TARGET_GLES
            (this->*parameterfvImplementation)(GL_TEXTURE_BORDER_COLOR, color.data());
            #else
            (this->*parameterfvImplementation)(GL_TEXTURE_BORDER_COLOR_NV, color.data());
            #endif
            return *this;
        }

        /**
         * @brief Set max anisotropy
         * @return Reference to self (for method chaining)
         *
         * Default value is `1.0f`, which means no anisotropy. Set to value
         * greater than `1.0f` for anisotropic filtering. If
         * @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some layer before the operation.
         * @see @ref Sampler::maxAnisotropy(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_MAX_ANISOTROPY_EXT}
         * @requires_extension %Extension @extension{EXT,texture_filter_anisotropic}
         * @requires_es_extension %Extension @es_extension2{EXT,texture_filter_anisotropic,texture_filter_anisotropic}
         */
        AbstractTexture& setMaxAnisotropy(Float anisotropy) {
            (this->*parameterfImplementation)(GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
            return *this;
        }
        #endif

        /**
         * @brief Invalidate texture image
         * @param level             Mip level
         *
         * If running on OpenGL ES or extension @extension{ARB,invalidate_subdata}
         * is not available, this function does nothing.
         * @see @ref Texture::invalidateSubImage() "invalidateSubImage()",
         *      @fn_gl{InvalidateTexImage}
         */
        void invalidateImage(Int level) {
            (this->*invalidateImageImplementation)(level);
        }

        /**
         * @brief Generate mipmap
         * @return Reference to self (for method chaining)
         *
         * Can not be used for rectangle textures. If
         * @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some layer before the operation.
         * @see setMinificationFilter(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{GenerateMipmap} or
         *      @fn_gl_extension{GenerateTextureMipmap,EXT,direct_state_access}
         * @requires_gl30 %Extension @extension{ARB,framebuffer_object}
         */
        AbstractTexture& generateMipmap();

    protected:
        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture.
         * @see @fn_gl{GenTextures}
         */
        explicit AbstractTexture(GLenum target);

        /**
         * @brief Destructor
         *
         * Deletes assigned OpenGL texture.
         * @see @fn_gl{DeleteTextures}
         */
        ~AbstractTexture();

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        template<UnsignedInt textureDimensions> struct DataHelper {};

        /* Unlike bind() this also sets the binding layer as active */
        void MAGNUM_LOCAL bindInternal();

        #ifndef MAGNUM_TARGET_GLES
        template<UnsignedInt dimensions> void image(GLenum target, GLint level, Image<dimensions>& image);
        template<UnsignedInt dimensions> void image(GLenum target, GLint level, BufferImage<dimensions>& image, Buffer::Usage usage);
        #endif

        GLenum _target;

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context& context);

        typedef void(AbstractTexture::*BindImplementation)(GLint);
        void MAGNUM_LOCAL bindImplementationDefault(GLint layer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL bindImplementationDSA(GLint layer);
        #endif
        static MAGNUM_LOCAL BindImplementation bindImplementation;

        typedef void(AbstractTexture::*ParameteriImplementation)(GLenum, GLint);
        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, GLint value);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, GLint value);
        #endif
        static ParameteriImplementation parameteriImplementation;

        typedef void(AbstractTexture::*ParameterfImplementation)(GLenum, GLfloat);
        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, GLfloat value);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, GLfloat value);
        #endif
        static ParameterfImplementation parameterfImplementation;

        typedef void(AbstractTexture::*ParameterfvImplementation)(GLenum, const GLfloat*);
        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, const GLfloat* values);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, const GLfloat* values);
        #endif
        static ParameterfvImplementation parameterfvImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*GetLevelParameterivImplementation)(GLenum, GLint, GLenum, GLint*);
        void MAGNUM_LOCAL getLevelParameterImplementationDefault(GLenum target, GLint level, GLenum parameter, GLint* values);
        void MAGNUM_LOCAL getLevelParameterImplementationDSA(GLenum target, GLint level, GLenum parameter, GLint* values);
        static MAGNUM_LOCAL GetLevelParameterivImplementation getLevelParameterivImplementation;
        #endif

        typedef void(AbstractTexture::*MipmapImplementation)();
        void MAGNUM_LOCAL mipmapImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL mipmapImplementationDSA();
        #endif
        static MAGNUM_LOCAL MipmapImplementation mipmapImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*Storage1DImplementation)(GLenum, GLsizei, TextureFormat, const Math::Vector<1, GLsizei>&);
        void MAGNUM_LOCAL storageImplementationFallback(GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        static Storage1DImplementation storage1DImplementation;
        #endif

        typedef void(AbstractTexture::*Storage2DImplementation)(GLenum, GLsizei, TextureFormat, const Vector2i&);
        void MAGNUM_LOCAL storageImplementationFallback(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        #endif
        static Storage2DImplementation storage2DImplementation;

        typedef void(AbstractTexture::*Storage3DImplementation)(GLenum, GLsizei, TextureFormat, const Vector3i&);
        void MAGNUM_LOCAL storageImplementationFallback(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        #endif
        static Storage3DImplementation storage3DImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*GetImageImplementation)(GLenum, GLint, ColorFormat, ColorType, std::size_t, GLvoid*);
        void MAGNUM_LOCAL getImageImplementationDefault(GLenum target, GLint level, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationDSA(GLenum target, GLint level, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationRobustness(GLenum target, GLint level, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        static MAGNUM_LOCAL GetImageImplementation getImageImplementation;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*Image1DImplementation)(GLenum, GLint, TextureFormat, const Math::Vector<1, GLsizei>&, ColorFormat, ColorType, const GLvoid*);
        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);
        static Image1DImplementation image1DImplementation;
        #endif

        typedef void(AbstractTexture::*Image2DImplementation)(GLenum, GLint, TextureFormat, const Vector2i&, ColorFormat, ColorType, const GLvoid*);
        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif
        static Image2DImplementation image2DImplementation;

        typedef void(AbstractTexture::*Image3DImplementation)(GLenum, GLint, TextureFormat, const Vector3i&, ColorFormat, ColorType, const GLvoid*);
        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif
        static Image3DImplementation image3DImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*SubImage1DImplementation)(GLenum, GLint, const Math::Vector<1, GLint>&, const Math::Vector<1, GLsizei>&, ColorFormat, ColorType, const GLvoid*);
        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data);
        static SubImage1DImplementation subImage1DImplementation;
        #endif

        typedef void(AbstractTexture::*SubImage2DImplementation)(GLenum, GLint, const Vector2i&, const Vector2i&, ColorFormat, ColorType, const GLvoid*);
        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif
        static SubImage2DImplementation subImage2DImplementation;

        typedef void(AbstractTexture::*SubImage3DImplementation)(GLenum, GLint, const Vector3i&, const Vector3i&, ColorFormat, ColorType, const GLvoid*);
        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif
        static SubImage3DImplementation subImage3DImplementation;

        typedef void(AbstractTexture::*InvalidateImageImplementation)(GLint);
        void MAGNUM_LOCAL invalidateImageImplementationNoOp(GLint level);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateImageImplementationARB(GLint level);
        #endif
        static InvalidateImageImplementation invalidateImageImplementation;

        typedef void(AbstractTexture::*InvalidateSubImageImplementation)(GLint, const Vector3i&, const Vector3i&);
        void MAGNUM_LOCAL invalidateSubImageImplementationNoOp(GLint level, const Vector3i& offset, const Vector3i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateSubImageImplementationARB(GLint level, const Vector3i& offset, const Vector3i& size);
        #endif
        static InvalidateSubImageImplementation invalidateSubImageImplementation;

        void MAGNUM_LOCAL destroy();
        void MAGNUM_LOCAL move();
        ColorFormat MAGNUM_LOCAL imageFormatForInternalFormat(TextureFormat internalFormat);
        ColorType MAGNUM_LOCAL imageTypeForInternalFormat(TextureFormat internalFormat);

        GLuint _id;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_GLES
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<1> {
    enum class Target: GLenum {
        Texture1D = GL_TEXTURE_1D
    };

    constexpr static Target target() { return Target::Texture1D; }

    static Math::Vector<1, GLint> imageSize(AbstractTexture* texture, GLenum target, GLint level);

    static void setWrapping(AbstractTexture* texture, const Array1D<Sampler::Wrapping>& wrapping) {
        (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping.x()));
    }

    static void setStorage(AbstractTexture* texture, GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
        (texture->*storage1DImplementation)(target, levels, internalFormat, size);
    }

    static void setImage(AbstractTexture* texture, GLenum target, GLint level, TextureFormat internalFormat, const ImageReference1D& image);
    static void setImage(AbstractTexture* texture, GLenum target, GLint level, TextureFormat internalFormat, BufferImage1D& image);

    static void setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const ImageReference1D& image);
    static void setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Math::Vector<1, GLint>& offset, BufferImage1D& image);

    static void invalidateSubImage(AbstractTexture* texture, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLint>& size) {
        (texture->*invalidateSubImageImplementation)(level, {offset[0], 0, 0}, {size[0], 1, 1});
    }
};
#endif
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<2> {
    enum class Target: GLenum {
        Texture2D = GL_TEXTURE_2D,
        #ifndef MAGNUM_TARGET_GLES
        Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
        Texture1DArray = GL_TEXTURE_1D_ARRAY,
        Rectangle = GL_TEXTURE_RECTANGLE
        #endif
    };

    constexpr static Target target() { return Target::Texture2D; }

    #ifndef MAGNUM_TARGET_GLES
    static Vector2i imageSize(AbstractTexture* texture, GLenum target, GLint level);
    #endif

    static void setWrapping(AbstractTexture* texture, const Array2D<Sampler::Wrapping>& wrapping);

    static void setStorage(AbstractTexture* texture, GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size) {
        (texture->*storage2DImplementation)(target, levels, internalFormat, size);
    }

    static void setImage(AbstractTexture* texture, GLenum target, GLint level, TextureFormat internalFormat, const ImageReference2D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setImage(AbstractTexture* texture, GLenum target, GLint level, TextureFormat internalFormat, BufferImage2D& image);
    #endif

    static void setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Vector2i& offset, const ImageReference2D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Vector2i& offset, BufferImage2D& image);
    #endif

    static void invalidateSubImage(AbstractTexture* texture, GLint level, const Vector2i& offset, const Vector2i& size) {
        (texture->*invalidateSubImageImplementation)(level, {offset, 0}, {size, 1});
    }
};
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<3> {
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

    constexpr static Target target() { return Target::Texture3D; }

    #ifndef MAGNUM_TARGET_GLES
    static Vector3i imageSize(AbstractTexture* texture, GLenum target, GLint level);
    #endif

    static void setWrapping(AbstractTexture* texture, const Array3D<Sampler::Wrapping>& wrapping);

    static void setStorage(AbstractTexture* texture, GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size) {
        (texture->*storage3DImplementation)(target, levels, internalFormat, size);
    }

    static void setImage(AbstractTexture* texture, GLenum target, GLint level, TextureFormat internalFormat, const ImageReference3D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setImage(AbstractTexture* texture, GLenum target, GLint level, TextureFormat internalFormat, BufferImage3D& image);
    #endif

    static void setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Vector3i& offset, const ImageReference3D& image);
    #ifndef MAGNUM_TARGET_GLES2
    static void setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Vector3i& offset, BufferImage3D& image);
    #endif

    static void invalidateSubImage(AbstractTexture* texture, GLint level, const Vector3i& offset, const Vector3i& size) {
        (texture->*invalidateSubImageImplementation)(level, offset, size);
    }
};
#endif

}

#endif
