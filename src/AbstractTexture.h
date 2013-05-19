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
#else
#include "OpenGL.h"
#endif
#include "Color.h"

namespace Magnum {

/**
@brief Base for textures

See Texture, CubeMapTexture and CubeMapTextureArray documentation for more
information and usage examples.

@section AbstractTexture-performance-optimization Performance optimizations and security

The engine tracks currently bound textures in all available layers to avoid
unnecessary calls to @fn_gl{ActiveTexture} and @fn_gl{BindTexture}. %Texture
configuration functions use dedicated highest available texture layer to not
affect active bindings in user layers. %Texture limits (such as
maxSupportedLayerCount()) are cached, so repeated queries don't result in
repeated @fn_gl{Get} calls.

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
*/
class MAGNUM_EXPORT AbstractTexture {
    friend class Context;

    AbstractTexture(const AbstractTexture&) = delete;
    AbstractTexture& operator=(const AbstractTexture&) = delete;

    public:
        /**
         * @brief %Texture filtering
         *
         * @see setMagnificationFilter() and setMinificationFilter()
         */
        enum class Filter: GLint {
            Nearest = GL_NEAREST,   /**< Nearest neighbor filtering */

            /**
             * Linear interpolation filtering.
             * @requires_gles30 %Extension @es_extension{OES,texture_float_linear} /
             *      @es_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
             *      for linear interpolation of textures with
             *      @ref Magnum::TextureFormat "TextureFormat::HalfFloat" /
             *      @ref Magnum::TextureFormat "TextureFormat::Float" in OpenGL
             *      ES 2.0.
             */
            Linear = GL_LINEAR
        };

        /**
         * @brief Mip level selection
         *
         * @see setMinificationFilter()
         */
        enum class Mipmap: GLint {
            Base = GL_NEAREST & ~GL_NEAREST, /**< Select base mip level */

            /**
             * Select nearest mip level. **Unavailable on rectangle textures.**
             */
            Nearest = GL_NEAREST_MIPMAP_NEAREST & ~GL_NEAREST,

            /**
             * Linear interpolation of nearest mip levels. **Unavailable on
             * rectangle textures.**
             * @requires_gles30 %Extension @es_extension{OES,texture_float_linear} /
             *      @es_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
             *      for linear interpolation of textures with
             *      @ref Magnum::TextureFormat "TextureFormat::HalfFloat" /
             *      @ref Magnum::TextureFormat "TextureFormat::Float" in OpenGL
             *      ES 2.0.
             */
            Linear = GL_NEAREST_MIPMAP_LINEAR & ~GL_NEAREST
        };

        /**
         * @brief %Texture wrapping
         *
         * @see @ref Texture::setWrapping() "setWrapping()"
         */
        enum class Wrapping: GLint {
            /** Repeat texture. **Unavailable on rectangle textures.** */
            Repeat = GL_REPEAT,

            /**
             * Repeat mirrored texture. **Unavailable on rectangle textures.**
             */
            MirroredRepeat = GL_MIRRORED_REPEAT,

            /**
             * Clamp to edge. Coordinates out of the range will be clamped to
             * first / last column / row in given direction.
             */
            ClampToEdge = GL_CLAMP_TO_EDGE,

            #ifndef MAGNUM_TARGET_GLES3
            /**
             * Clamp to border color. Coordinates out of range will be clamped
             * to border color (set with setBorderColor()).
             * @requires_es_extension %Extension @es_extension{NV,texture_border_clamp}
             */
            #ifndef MAGNUM_TARGET_GLES
            ClampToBorder = GL_CLAMP_TO_BORDER
            #else
            ClampToBorder = GL_CLAMP_TO_BORDER_NV
            #endif
            #endif
        };

        /**
         * @brief Max supported layer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref AbstractShaderProgram-subclassing, bind(Int),
         *      @fn_gl{Get} with @def_gl{MAX_COMBINED_TEXTURE_IMAGE_UNITS},
         *      @fn_gl{ActiveTexture}
         */
        static Int maxSupportedLayerCount();

        #ifndef MAGNUM_TARGET_GLES3
        /**
         * @brief Max supported anisotropy
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see setMaxAnisotropy(), @fn_gl{Get} with @def_gl{MAX_TEXTURE_MAX_ANISOTROPY_EXT}
         * @requires_extension %Extension @extension{EXT,texture_filter_anisotropic}
         * @requires_es_extension %Extension @es_extension2{EXT,texture_filter_anisotropic,texture_filter_anisotropic}
         */
        static Float maxSupportedAnisotropy();
        #endif

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline explicit AbstractTexture(GLenum target): _target(target) {
            glGenTextures(1, &_id);
        }
        #endif

        /**
         * @brief Destructor
         *
         * Deletes assigned OpenGL texture.
         * @see @fn_gl{DeleteTextures}
         */
        virtual ~AbstractTexture() = 0;

        /** @brief Move constructor */
        AbstractTexture(AbstractTexture&& other);

        /** @brief Move assignment */
        AbstractTexture& operator=(AbstractTexture&& other);

        /** @brief OpenGL texture ID */
        inline GLuint id() const { return _id; }

        /**
         * @brief Bind texture for rendering
         *
         * Sets current texture as active in given layer. The layer must be
         * between 0 and maxSupportedLayerCount(). Note that only one texture
         * can be bound to given layer. If @extension{EXT,direct_state_access}
         * is not available, the layer is made active before binding the
         * texture.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} or
         *      @fn_gl_extension{BindMultiTexture,EXT,direct_state_access}
         */
        void bind(Int layer);

        /**
         * @brief Set minification filter
         * @param filter        Filter
         * @param mipmap        Mipmap filtering. If set to anything else than
         *      BaseMipLevel, make sure textures for all mip levels are set or
         *      call generateMipmap().
         * @return Pointer to self (for method chaining)
         *
         * Sets filter used when the object pixel size is smaller than the
         * texture size. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some layer before the operation.
         * Initial value is (@ref AbstractTexture::Filter "Filter::Nearest",
         * @ref AbstractTexture::Mipmap "Mipmap::Linear").
         * @attention For rectangle textures only some modes are supported,
         *      see @ref AbstractTexture::Filter "Filter" and
         *      @ref AbstractTexture::Mipmap "Mipmap" documentation for more
         *      information.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_MIN_FILTER}
         */
        AbstractTexture* setMinificationFilter(Filter filter, Mipmap mipmap = Mipmap::Base);

        /**
         * @brief Set magnification filter
         * @param filter        Filter
         * @return Pointer to self (for method chaining)
         *
         * Sets filter used when the object pixel size is larger than largest
         * texture size. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some layer before the operation.
         * Initial value is @ref AbstractTexture::Filter "Filter::Linear".
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_MAG_FILTER}
         */
        inline AbstractTexture* setMagnificationFilter(Filter filter) {
            (this->*parameteriImplementation)(GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
            return this;
        }

        #ifndef MAGNUM_TARGET_GLES3
        /**
         * @brief Set border color
         * @return Pointer to self (for method chaining)
         *
         * Border color when @ref AbstractTexture::Wrapping "wrapping" is set
         * to `ClampToBorder`. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some layer before the operation.
         * Initial value is `{0.0f, 0.0f, 0.0f, 0.0f}`.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_BORDER_COLOR}
         * @requires_es_extension %Extension @es_extension{NV,texture_border_clamp}
         */
        inline AbstractTexture* setBorderColor(const Color4<>& color) {
            #ifndef MAGNUM_TARGET_GLES
            (this->*parameterfvImplementation)(GL_TEXTURE_BORDER_COLOR, color.data());
            #else
            (this->*parameterfvImplementation)(GL_TEXTURE_BORDER_COLOR_NV, color.data());
            #endif
            return this;
        }

        /**
         * @brief Set max anisotropy
         * @return Pointer to self (for method chaining)
         *
         * Default value is `1.0f`, which means no anisotropy. Set to value
         * greater than `1.0f` for anisotropic filtering. If
         * @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some layer before the operation.
         * @see maxSupportedAnisotropy(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_MAX_ANISOTROPY_EXT}
         * @requires_extension %Extension @extension{EXT,texture_filter_anisotropic}
         * @requires_es_extension %Extension @es_extension2{EXT,texture_filter_anisotropic,texture_filter_anisotropic}
         */
        inline AbstractTexture* setMaxAnisotropy(Float anisotropy) {
            (this->*parameterfImplementation)(GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
            return this;
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
        inline void invalidateImage(Int level) {
            (this->*invalidateImageImplementation)(level);
        }

        /**
         * @brief Generate mipmap
         * @return Pointer to self (for method chaining)
         *
         * Can not be used for rectangle textures. If
         * @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some layer before the operation.
         * @see setMinificationFilter(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{GenerateMipmap} or
         *      @fn_gl_extension{GenerateTextureMipmap,EXT,direct_state_access}
         * @requires_gl30 %Extension @extension{EXT,framebuffer_object}
         */
        AbstractTexture* generateMipmap();

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        template<UnsignedInt textureDimensions> struct DataHelper {};

        /* Unlike bind() this also sets the binding layer as active */
        void MAGNUM_LOCAL bindInternal();

        #ifndef MAGNUM_TARGET_GLES
        template<UnsignedInt dimensions> void MAGNUM_LOCAL image(GLenum target, GLint level, Image<dimensions>* image);
        template<UnsignedInt dimensions> void MAGNUM_LOCAL image(GLenum target, GLint level, BufferImage<dimensions>* image, Buffer::Usage usage);
        #endif

        GLenum _target;

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

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
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        static Storage1DImplementation storage1DImplementation;
        #endif

        typedef void(AbstractTexture::*Storage2DImplementation)(GLenum, GLsizei, TextureFormat, const Vector2i&);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size);
        #endif
        static Storage2DImplementation storage2DImplementation;

        typedef void(AbstractTexture::*Storage3DImplementation)(GLenum, GLsizei, TextureFormat, const Vector3i&);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size);
        #endif
        static Storage3DImplementation storage3DImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*GetImageImplementation)(GLenum, GLint, ImageFormat, ImageType, std::size_t, GLvoid*);
        void MAGNUM_LOCAL getImageImplementationDefault(GLenum target, GLint level, ImageFormat format, ImageType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationDSA(GLenum target, GLint level, ImageFormat format, ImageType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationRobustness(GLenum target, GLint level, ImageFormat format, ImageType type, std::size_t dataSize, GLvoid* data);
        static MAGNUM_LOCAL GetImageImplementation getImageImplementation;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*Image1DImplementation)(GLenum, GLint, TextureFormat, const Math::Vector<1, GLsizei>&, ImageFormat, ImageType, const GLvoid*);
        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size, ImageFormat format, ImageType type, const GLvoid* data);
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size, ImageFormat format, ImageType type, const GLvoid* data);
        static Image1DImplementation image1DImplementation;
        #endif

        typedef void(AbstractTexture::*Image2DImplementation)(GLenum, GLint, TextureFormat, const Vector2i&, ImageFormat, ImageType, const GLvoid*);
        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Vector2i& size, ImageFormat format, ImageType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Vector2i& size, ImageFormat format, ImageType type, const GLvoid* data);
        #endif
        static Image2DImplementation image2DImplementation;

        typedef void(AbstractTexture::*Image3DImplementation)(GLenum, GLint, TextureFormat, const Vector3i&, ImageFormat, ImageType, const GLvoid*);
        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Vector3i& size, ImageFormat format, ImageType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Vector3i& size, ImageFormat format, ImageType type, const GLvoid* data);
        #endif
        static Image3DImplementation image3DImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*SubImage1DImplementation)(GLenum, GLint, const Math::Vector<1, GLint>&, const Math::Vector<1, GLsizei>&, ImageFormat, ImageType, const GLvoid*);
        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ImageFormat format, ImageType type, const GLvoid* data);
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ImageFormat format, ImageType type, const GLvoid* data);
        static SubImage1DImplementation subImage1DImplementation;
        #endif

        typedef void(AbstractTexture::*SubImage2DImplementation)(GLenum, GLint, const Vector2i&, const Vector2i&, ImageFormat, ImageType, const GLvoid*);
        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, ImageFormat format, ImageType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, ImageFormat format, ImageType type, const GLvoid* data);
        #endif
        static SubImage2DImplementation subImage2DImplementation;

        typedef void(AbstractTexture::*SubImage3DImplementation)(GLenum, GLint, const Vector3i&, const Vector3i&, ImageFormat, ImageType, const GLvoid*);
        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, ImageFormat format, ImageType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, ImageFormat format, ImageType type, const GLvoid* data);
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

        GLuint _id;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<class Image> struct ImageHelper {
        inline static const GLvoid* dataOrPixelUnpackBuffer(Image* image) {
            #ifndef MAGNUM_TARGET_GLES2
            Buffer::unbind(Buffer::Target::PixelUnpack);
            #endif
            return image->data();
        }
    };

    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> struct MAGNUM_EXPORT ImageHelper<BufferImage<dimensions>> {
        static const GLvoid* dataOrPixelUnpackBuffer(BufferImage<dimensions>* image);
    };
    #endif
}

#ifndef MAGNUM_TARGET_GLES
template<> struct AbstractTexture::DataHelper<1> {
    enum class Target: GLenum {
        Texture1D = GL_TEXTURE_1D
    };

    inline constexpr static Target target() { return Target::Texture1D; }

    static Math::Vector<1, GLint> imageSize(AbstractTexture* texture, GLenum target, GLint level);

    inline static void setWrapping(AbstractTexture* texture, const Array1D<Wrapping>& wrapping) {
        (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping.x()));
    }

    inline static void setStorage(AbstractTexture* texture, GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
        (texture->*storage1DImplementation)(target, levels, internalFormat, size);
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 1, void>::type setImage(AbstractTexture* texture, GLenum target, GLint level, TextureFormat internalFormat, Image* image) {
        (texture->*image1DImplementation)(target, level, internalFormat, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 1, void>::type setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Math::Vector<1, GLint>& offset, Image* image) {
        (texture->*subImage1DImplementation)(target, level, offset, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    inline static void invalidateSubImage(AbstractTexture* texture, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLint>& size) {
        (texture->*invalidateSubImageImplementation)(level, {offset[0], 0, 0}, {size[0], 1, 1});
    }
};
#endif
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<2> {
    enum class Target: GLenum {
        Texture2D = GL_TEXTURE_2D
        #ifndef MAGNUM_TARGET_GLES
        ,
        Texture1DArray = GL_TEXTURE_1D_ARRAY,
        Rectangle = GL_TEXTURE_RECTANGLE
        #endif
    };

    inline constexpr static Target target() { return Target::Texture2D; }

    #ifndef MAGNUM_TARGET_GLES
    static Vector2i imageSize(AbstractTexture* texture, GLenum target, GLint level);
    #endif

    static void setWrapping(AbstractTexture* texture, const Array2D<Wrapping>& wrapping);

    inline static void setStorage(AbstractTexture* texture, GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size) {
        (texture->*storage2DImplementation)(target, levels, internalFormat, size);
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 2, void>::type setImage(AbstractTexture* texture, GLenum target, GLint level, TextureFormat internalFormat, Image* image) {
        (texture->*image2DImplementation)(target, level, internalFormat, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 2, void>::type setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Vector2i& offset, Image* image) {
        (texture->*subImage2DImplementation)(target, level, offset, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 1, void>::type setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Vector2i& offset, Image* image) {
        (texture->*subImage2DImplementation)(target, level, offset, Vector2i(image->size(), 1), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    inline static void invalidateSubImage(AbstractTexture* texture, GLint level, const Vector2i& offset, const Vector2i& size) {
        (texture->*invalidateSubImageImplementation)(level, {offset, 0}, {size, 1});
    }
};
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<3> {
    enum class Target: GLenum {
        #ifndef MAGNUM_TARGET_GLES2
        Texture3D = GL_TEXTURE_3D,
        Texture2DArray = GL_TEXTURE_2D_ARRAY
        #else
        Texture3D = GL_TEXTURE_3D_OES
        #endif
    };

    inline constexpr static Target target() { return Target::Texture3D; }

    #ifndef MAGNUM_TARGET_GLES
    static Vector3i imageSize(AbstractTexture* texture, GLenum target, GLint level);
    #endif

    static void setWrapping(AbstractTexture* texture, const Array3D<Wrapping>& wrapping);

    inline static void setStorage(AbstractTexture* texture, GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size) {
        (texture->*storage3DImplementation)(target, levels, internalFormat, size);
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 3, void>::type setImage(AbstractTexture* texture, GLenum target, GLint level, TextureFormat internalFormat, Image* image) {
        (texture->*image3DImplementation)(target, level, internalFormat, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 3, void>::type setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Vector3i& offset, Image* image) {
        (texture->*subImage3DImplementation)(target, level, offset, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 2, void>::type setSubImage(AbstractTexture* texture, GLenum target, GLint level, const Vector3i& offset, Image* image) {
        (texture->*subImage3DImplementation)(target, level, offset, Vector3i(image->size(), 1), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    inline static void invalidateSubImage(AbstractTexture* texture, GLint level, const Vector3i& offset, const Vector3i& size) {
        (texture->*invalidateSubImageImplementation)(level, offset, size);
    }
};
#endif

}

#endif
