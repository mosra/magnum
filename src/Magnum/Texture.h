#ifndef Magnum_Texture_h
#define Magnum_Texture_h
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
 * @brief Class @ref Magnum::Texture, typedef @ref Magnum::Texture1D, @ref Magnum::Texture2D, @ref Magnum::Texture3D
 */

#include "Magnum/AbstractTexture.h"
#include "Magnum/DimensionTraits.h"

namespace Magnum {

/**
@brief %Texture

Template class for one- to three-dimensional textures. See also
@ref AbstractTexture documentation for more information.

@section Texture-usage Usage

Common usage is to fully configure all texture parameters and then set the
data from e.g. @ref Image. Example configuration of high quality texture with
trilinear anisotropic filtering, i.e. the best you can ask for:
@code
Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte, {4096, 4096}, data);

Texture2D texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    .setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
    .setWrapping(Sampler::Wrapping::ClampToEdge)
    .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
    .setStorage(Math::log2(4096)+1, TextureFormat::RGBA8, {4096, 4096})
    .setSubImage(0, {}, image)
    .generateMipmap();
@endcode

@attention Don't forget to fully configure the texture before use. Note that
    default configuration (if @ref setMinificationFilter() is not called with
    another value) is to use mipmaps, so be sure to either call @ref setMinificationFilter(),
    explicitly specify all mip levels with @ref setStorage() and @ref setImage()
    or call @ref generateMipmap(). If using rectangle texture, you must also
    call @ref setWrapping(), because the initial value is not supported on
    rectangle textures. See also @ref setMagnificationFilter() and
    @ref setBorderColor().

The texture is bound to layer specified by shader via @ref bind(). In shader,
the texture is used via `sampler2D` and friends, see @ref Target enum
documentation for more information. See also AbstractShaderProgram
documentation for more information about usage in shaders.

@section Texture-array Texture arrays

You can create texture arrays by passing
@ref Target::Texture1DArray "Texture2D::Target::Texture1DArray" or
@ref Target::Texture2DArray "Texture3D::Target::Texture2DArray" to constructor.

It is possible to specify each layer separately using @ref setSubImage(), but
you have to allocate the memory for all layers first by calling @ref setStorage().
Example: 2D texture array with 16 layers of 64x64 images:
@code
Texture3D texture(Texture3D::Target::Texture2DArray);
texture.setMagnificationFilter(Sampler::Filter::Linear)
    // ...
    .setStorage(levels, TextureFormat::RGBA8, {64, 64,16});

for(std::size_t i = 0; i != 16; ++i) {
    // ...
    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte, {64, 64}, data[i]);
    texture.setSubImage(0, Vector3i::zAxis(i), image);
}

// ...
@endcode

Similar approach can be used for any other texture types (e.g. setting
@ref Texture3D data using 2D layers, @ref Texture2D data using one-dimensional
chunks etc.).

@requires_gl30 %Extension @extension{EXT,texture_array} for texture arrays.
@requires_gles30 %Array textures are not available in OpenGL ES 2.0.

@section Texture-multisample Multisample textures

You can create multisample textures by passing
@ref Target::Texture2DMultisample "Texture2D::Target::Texture2DMultisample" or
@ref Target::Texture2DMultisampleArray "Texture3D::Target::Texture2DMultisampleArray"
to constructor.

@todoc finish this when fully implemented

@requires_gl32 %Extension @extension{ARB,texture_multisample} for multisample
    textures.
@requires_gl Multisample textures are not available in OpenGL ES.

@section Texture-rectangle Rectangle textures

Rectangle texture is created by passing @ref Target::Rectangle "Texture2D::Target::Rectangle"
to constructor. In shader, the texture is used via `sampler2DRect` and friends.
Unlike `sampler2D`, which accepts coordinates between 0 and 1, `sampler2DRect`
accepts coordinates between 0 and `textureSizeInGivenDirection-1`. Note that
rectangle textures don't support mipmapping and repeating wrapping modes, see
@ref Sampler::Filter, @ref Sampler::Mipmap and @ref generateMipmap()
documentation for more information.

@requires_gl31 %Extension @extension{ARB,texture_rectangle} for rectangle
    textures.
@requires_gl Rectangle textures are not available in OpenGL ES.

@see @ref Texture1D, @ref Texture2D, @ref Texture3D, @ref CubeMapTexture,
    @ref CubeMapTextureArray, @ref BufferTexture
@todo @extension{AMD,sparse_texture}
@todo Separate multisample, array and rectangle texture classes to avoid confusion, then remove Target enum
 */
template<UnsignedInt dimensions> class Texture: public AbstractTexture {
    public:
        static const UnsignedInt Dimensions = dimensions; /**< @brief %Texture dimension count */

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief %Texture target
         *
         * Each dimension has its own unique subset of these targets.
         */
        enum class Target: GLenum {
            /**
             * One-dimensional texture.  Use `sampler1D`, `sampler1DShadow`,
             * `isampler1D` or `usampler1D` in shader.
             * @requires_gl Only 2D and 3D textures are available in OpenGL
             *      ES.
             */
            Texture1D = GL_TEXTURE_1D,

            /**
             * Two-dimensional texture. Use `sampler2D`, `sampler2DShadow`,
             * `isampler2D` or `usampler2D` in shader.
             */
            Texture2D = GL_TEXTURE_2D,

            /**
             * Three-dimensional texture. Use `sampler3D`, `isampler3D` or
             * `usampler3D` in shader.
             * @requires_gles30 %Extension @es_extension{OES,texture_3D}
             */
            Texture3D = GL_TEXTURE_3D,

            /**
             * One-dimensional texture array (i.e. two dimensions in total).
             * Use `sampler1DArray`, `sampler1DArrayShadow`, `isampler1DArray`
             * or `usampler1DArray` in shader.
             * @requires_gl30 %Extension @extension{EXT,texture_array}
             * @requires_gl Only 2D and 3D textures are available in OpenGL
             *      ES.
             */
            Texture1DArray = GL_TEXTURE_1D_ARRAY,

            /**
             * Two-dimensional texture array (i.e. three dimensions in total).
             * Use `sampler2DArray`, `sampler2DArrayShadow`, `isampler2DArray`
             * or `usampler2DArray` in shader.
             * @requires_gl30 %Extension @extension{EXT,texture_array}
             * @requires_gles30 %Array textures are not available in OpenGL ES
             *      2.0.
             */
            Texture2DArray = GL_TEXTURE_2D_ARRAY,

            /**
             * Multisampled two-dimensional texture. Use `sampler2DMS`,
             * `isampler2DMS` or `usampler2DMS` in shader.
             * @requires_gl32 %Extension @extension{ARB,texture_multisample}
             * @requires_gl Multisample textures are not available in OpenGL
             *      ES.
             */
            Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,

            /**
             * Multisampled two-dimensional texture array (i.e. three
             * dimensions in total). Use `sampler2DMSArray`,
             * `isampler2DMSArray` or `usampler2DMSArray` in shader.
             * @requires_gl32 %Extension @extension{ARB,texture_multisample}
             * @requires_gl Multisample textures are not available in OpenGL
             *      ES.
             */
            Texture2DMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,

            /**
             * Rectangle texture (i.e. two dimensions). Use `sampler2DRect`,
             * `sampler2DRectShadow`, `isampler2DRect` or `usampler2DRect` in
             * shader.
             * @requires_gl31 %Extension @extension{ARB,texture_rectangle}
             * @requires_gl Rectangle textures are not available in OpenGL ES.
             */
            Rectangle = GL_TEXTURE_RECTANGLE
        };
        #else
        typedef typename DataHelper<Dimensions>::Target Target; /**< @brief %Texture target */
        #endif

        /**
         * @brief Constructor
         * @param target            %Texture target. If not set, default value
         *      is `Target::Texture1D`, `Target::Texture2D` or
         *      `Target::Texture3D` based on dimension count.
         *
         * Creates new OpenGL texture.
         * @see @fn_gl{GenTextures}
         */
        explicit Texture(Target target = DataHelper<Dimensions>::target()): AbstractTexture(GLenum(target)) {}

        /** @brief %Texture target */
        constexpr Target target() const { return static_cast<Target>(_target); }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief %Image size in given mip level
         *
         * The result is not cached in any way. If
         * @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some layer before the operation.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} or @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_WIDTH}, @def_gl{TEXTURE_HEIGHT} or @def_gl{TEXTURE_DEPTH}.
         * @requires_gl %Texture image queries are not available in OpenGL ES.
         */
        typename DimensionTraits<Dimensions, Int>::VectorType imageSize(Int level) {
            return DataHelper<Dimensions>::imageSize(*this, _target, level);
        }
        #endif

        /**
         * @brief Set wrapping
         * @param wrapping          Wrapping type for all texture dimensions
         * @return Reference to self (for method chaining)
         *
         * Sets wrapping type for coordinates out of range (0, 1) for normal
         * textures and (0, textureSizeInGivenDirection-1) for rectangle
         * textures. If @extension{EXT,direct_state_access} is not available,
         * the texture is bound to some layer before the operation. Initial
         * value is @ref Sampler::Wrapping::Repeat.
         * @attention For rectangle textures only some modes are supported,
         *      see @ref Sampler::Wrapping documentation for more information.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_WRAP_S}, @def_gl{TEXTURE_WRAP_T},
         *      @def_gl{TEXTURE_WRAP_R}
         */
        Texture<Dimensions>& setWrapping(const Array<Dimensions, Sampler::Wrapping>& wrapping) {
            DataHelper<Dimensions>::setWrapping(*this, wrapping);
            return *this;
        }

        /**
         * @brief Set storage
         * @param levels            Mip level count
         * @param internalFormat    Internal format
         * @param size              Size of largest mip level
         * @return Reference to self (for method chaining)
         *
         * Specifies entire structure of a texture at once, removing the need
         * for additional consistency checks and memory reallocations when
         * updating the data later. After calling this function the texture
         * is immutable and calling @ref setStorage() or @ref setImage() is not
         * allowed.
         *
         * If @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some layer before the operation. If OpenGL 4.2,
         * @extension{ARB,texture_storage}, OpenGL ES 3.0 or
         * @es_extension{EXT,texture_storage} in OpenGL ES 2.0 is not
         * available, the feature is emulated with sequence of @ref setImage()
         * calls.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexStorage1D}/@fn_gl{TexStorage2D}/@fn_gl{TexStorage3D}
         *      or @fn_gl_extension{TextureStorage1D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureStorage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureStorage3D,EXT,direct_state_access},
         *      eventually @fn_gl{TexImage1D}/@fn_gl{TexImage2D}/@fn_gl{TexImage3D} or
         *      @fn_gl_extension{TextureImage1D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage3D,EXT,direct_state_access}.
         */
        Texture<Dimensions>& setStorage(Int levels, TextureFormat internalFormat, const typename DimensionTraits<Dimensions, Int>::VectorType& size) {
            DataHelper<Dimensions>::setStorage(*this, _target, levels, internalFormat, size);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Read given mip level of texture to image
         * @param level             Mip level
         * @param image             %Image where to put the data
         *
         * %Image parameters like format and type of pixel data are taken from
         * given image, image size is taken from the texture using
         * @ref imageSize().
         *
         * If @extension{EXT,direct_state_access} is not available, the
         * texture is bound to some layer before the operation. If
         * @extension{ARB,robustness} is available, the operation is protected
         * from buffer overflow. However, if both @extension{EXT,direct_state_access}
         * and @extension{ARB,robustness} are available, the DSA version is
         * used, because it is better for performance and there isn't any
         * function combining both features.
         * @requires_gl %Texture image queries are not available in OpenGL ES.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} or @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_WIDTH}, @def_gl{TEXTURE_HEIGHT} or @def_gl{TEXTURE_DEPTH},
         *      then @fn_gl{GetTexImage}, @fn_gl_extension{GetTextureImage,EXT,direct_state_access}
         *      or @fn_gl_extension{GetnTexImage,ARB,robustness}
         */
        void image(Int level, Image<dimensions>& image) {
            AbstractTexture::image<dimensions>(_target, level, image);
        }

        /**
         * @brief Read given mip level of texture to buffer image
         * @param level             Mip level
         * @param image             %Buffer image where to put the data
         * @param usage             %Buffer usage
         *
         * See @ref image(Int, Image&) for more information.
         * @requires_gl %Texture image queries are not available in OpenGL ES.
         */
        void image(Int level, BufferImage<dimensions>& image, BufferUsage usage) {
            AbstractTexture::image<dimensions>(_target, level, image, usage);
        }
        #endif

        /**
         * @brief Set image data
         * @param level             Mip level
         * @param internalFormat    Internal format
         * @param image             @ref Image, @ref ImageReference or
         *      @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * For better performance when generating mipmaps using
         * @ref generateMipmap() or calling @ref setImage() more than once use
         * @ref setStorage() and @ref setSubImage() instead.
         *
         * If @extension{EXT,direct_state_access} is not available, the
         * texture is bound to some layer before the operation.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexImage1D}/@fn_gl{TexImage2D}/@fn_gl{TexImage3D} or
         *      @fn_gl_extension{TextureImage1D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage3D,EXT,direct_state_access}
         */
        Texture<Dimensions>& setImage(Int level, TextureFormat internalFormat, const ImageReference<dimensions>& image) {
            DataHelper<Dimensions>::setImage(*this, _target, level, internalFormat, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload */
        Texture<Dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions>& image) {
            DataHelper<Dimensions>::setImage(*this, _target, level, internalFormat, image);
            return *this;
        }

        /** @overload */
        Texture<Dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions>&& image) {
            return setImage(level, internalFormat, image);
        }
        #endif

        /**
         * @brief Set image subdata
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             @ref Image, @ref ImageReference or
         *      @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available, the
         * texture is bound to some layer before the operation.
         * @see @ref setStorage(), @ref setImage(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexSubImage1D}/
         *      @fn_gl{TexSubImage2D}/@fn_gl{TexSubImage3D} or
         *      @fn_gl_extension{TextureSubImage1D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureSubImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureSubImage3D,EXT,direct_state_access}
         */
        Texture<Dimensions>& setSubImage(Int level, const typename DimensionTraits<Dimensions, Int>::VectorType& offset, const ImageReference<dimensions>& image) {
            DataHelper<Dimensions>::setSubImage(*this, _target, level, offset, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload */
        Texture<Dimensions>& setSubImage(Int level, const typename DimensionTraits<Dimensions, Int>::VectorType& offset, BufferImage<dimensions>& image) {
            DataHelper<Dimensions>::setSubImage(*this, _target, level, offset, image);
            return *this;
        }

        /** @overload */
        Texture<Dimensions>& setSubImage(Int level, const typename DimensionTraits<Dimensions, Int>::VectorType& offset, BufferImage<dimensions>&& image) {
            return setSubImage(level, offset, image);
        }
        #endif

        /**
         * @brief Invalidate texture subimage
         * @param level             Mip level
         * @param offset            Offset into the texture
         * @param size              Size of invalidated data
         *
         * If running on OpenGL ES or extension @extension{ARB,invalidate_subdata}
         * (part of OpenGL 4.3) is not available, this function does nothing.
         * @see @ref invalidateImage(), @fn_gl{InvalidateTexSubImage}
         */
        void invalidateSubImage(Int level, const typename DimensionTraits<Dimensions, Int>::VectorType& offset, const typename DimensionTraits<Dimensions, Int>::VectorType& size) {
            DataHelper<dimensions>::invalidateSubImage(*this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Texture<Dimensions>& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        Texture<Dimensions>& setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }
        Texture<Dimensions>& setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }
        Texture<Dimensions>& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        Texture<Dimensions>& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }
        Texture<Dimensions>& generateMipmap() {
            AbstractTexture::generateMipmap();
            return *this;
        }
        #endif
};

#ifndef MAGNUM_TARGET_GLES
/**
@brief One-dimensional texture

@requires_gl Only 2D and 3D textures are available in OpenGL ES.
*/
typedef Texture<1> Texture1D;
#endif

/** @brief Two-dimensional texture */
typedef Texture<2> Texture2D;

/**
@brief Three-dimensional texture

@requires_gles30 %Extension @es_extension{OES,texture_3D}
*/
typedef Texture<3> Texture3D;

}

#endif
