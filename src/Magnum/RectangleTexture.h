#ifndef Magnum_RectangleTexture_h
#define Magnum_RectangleTexture_h
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

#ifndef MAGNUM_TARGET_GLES
/** @file
 * @brief Class @ref Magnum::RectangleTexture
 */
#endif

#include "Magnum/AbstractTexture.h"
#include "Magnum/Array.h"
#include "Magnum/Math/Vector2.h"

#ifndef MAGNUM_TARGET_GLES
namespace Magnum {

/**
@brief Rectangle texture

See also @ref AbstractTexture documentation for more information.

@section RectangleTexture-usage Usage

Common usage is to fully configure all texture parameters and then set the
data from e.g. @ref Image2D. Example configuration:
@code
Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte, {526, 137}, data);

RectangleTexture texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    .setMinificationFilter(Sampler::Filter::Linear)
    .setWrapping(Sampler::Wrapping::ClampToEdge)
    .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
    .setStorage(TextureFormat::RGBA8, {526, 137})
    .setSubImage({}, image);
@endcode

In shader, the texture is used via sampler2DRect`, `sampler2DRectShadow`,
`isampler2DRect` or `usampler2DRect`. See @ref AbstractShaderProgram
documentation for more information about usage in shaders.

@see @ref Texture, @ref TextureArray, @ref CubeMapTexture,
    @ref CubeMapTextureArray, @ref BufferTexture, @ref MultisampleTexture
@requires_gl31 %Extension @extension{ARB,texture_rectangle}
@requires_gl Rectangle textures are not available in OpenGL ES.
 */
class RectangleTexture: public AbstractTexture {
    public:
        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object.
         * @see @fn_gl{GenTextures} with @def_gl{TEXTURE_RECTANGLE}
         */
        explicit RectangleTexture(): AbstractTexture(GL_TEXTURE_RECTANGLE) {}

        /**
         * @brief Set minification filter
         * @param filter        Filter
         * @return Reference to self (for method chaining)
         *
         * Sets filter used when the object pixel size is smaller than the
         * texture size. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some texture unit before the
         * operation. Initial value is @ref Sampler::Filter::Linear.
         * @see @ref setMagnificationFilter(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_MIN_FILTER}
         */
        RectangleTexture& setMinificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMinificationFilter(filter, Sampler::Mipmap::Base);
            return *this;
        }

        /** @copydoc Texture::setMagnificationFilter() */
        RectangleTexture& setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        /**
         * @brief %Image size
         *
         * The result is not cached in any way. If
         * @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some texture unit before the operation.
         * @see @ref image(), @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} or @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_WIDTH} and @def_gl{TEXTURE_HEIGHT}
         */
        Vector2i imageSize() { return DataHelper<2>::imageSize(*this, _target, 0); }

        /**
         * @brief Set wrapping
         * @param wrapping          Wrapping type for all texture dimensions
         * @return Reference to self (for method chaining)
         *
         * Sets wrapping type for coordinates out of @f$ [ 0, size - 1 ] @f$
         * range. If @extension{EXT,direct_state_access} is not available, the
         * texture is bound to some texture unit before the operation. Initial
         * value is @ref Sampler::Wrapping::ClampToEdge.
         * @attention Only @ref Sampler::Wrapping::ClampToEdge and
         *      @ref Sampler::Wrapping::ClampToBorder is supported on this
         *      texture type.
         * @see @ref setBorderColor(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_WRAP_S}, @def_gl{TEXTURE_WRAP_T},
         *      @def_gl{TEXTURE_WRAP_R}
         */
        RectangleTexture& setWrapping(const Array2D<Sampler::Wrapping>& wrapping) {
            DataHelper<2>::setWrapping(*this, wrapping);
            return *this;
        }

        /**
         * @brief Set border color
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Color4&) for more
         * information.
         */
        RectangleTexture& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /**
         * @brief Set border color for integer texture
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Vector4ui&) for more
         * information.
         */
        RectangleTexture& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /**
         * @brief Set border color for integer texture
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Vector4i&) for more
         * information.
         */
        RectangleTexture& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @copydoc Texture::setMaxAnisotropy() */
        RectangleTexture& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        /**
         * @brief Set storage
         * @param internalFormat    Internal format
         * @param size              %Texture size
         * @return Reference to self (for method chaining)
         *
         * Specifies entire structure of a texture at once, removing the need
         * for additional consistency checks and memory reallocations when
         * updating the data later. After calling this function the texture
         * is immutable and calling @ref setStorage() or @ref setImage() is not
         * allowed.
         *
         * If @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some texture unit before the operation. If
         * @extension{ARB,texture_storage} (part of OpenGL 4.2), OpenGL ES 3.0
         * or @es_extension{EXT,texture_storage} in OpenGL ES 2.0 is not
         * available, the feature is emulated with @ref setImage() call.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexStorage2D}
         *      or @fn_gl_extension{TextureStorage2D,EXT,direct_state_access},
         *      eventually @fn_gl{TexImage2D} or
         *      @fn_gl_extension{TextureImage2D,EXT,direct_state_access}.
         */
        RectangleTexture& setStorage(TextureFormat internalFormat, const Vector2i& size) {
            DataHelper<2>::setStorage(*this, _target, 1, internalFormat, size);
            return *this;
        }

        /**
         * @brief Read texture to image
         * @param image             %Image where to put the data
         *
         * %Image parameters like format and type of pixel data are taken from
         * given image, image size is taken from the texture using
         * @ref imageSize().
         *
         * If @extension{EXT,direct_state_access} is not available, the
         * texture is bound to some texture unit before the operation. If
         * @extension{ARB,robustness} is available, the operation is protected
         * from buffer overflow. However, if both @extension{EXT,direct_state_access}
         * and @extension{ARB,robustness} are available, the DSA version is
         * used, because it is better for performance and there isn't any
         * function combining both features.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} or @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_WIDTH} and @def_gl{TEXTURE_HEIGHT}, then
         *      @fn_gl{GetTexImage}, @fn_gl_extension{GetTextureImage,EXT,direct_state_access}
         *      or @fn_gl_extension{GetnTexImage,ARB,robustness}
         */
        void image(Image2D& image) {
            AbstractTexture::image<2>(_target, 0, image);
        }

        /**
         * @brief Read given mip level of texture to buffer image
         * @param image             %Buffer image where to put the data
         * @param usage             %Buffer usage
         *
         * See @ref image(Image2D&) for more information.
         */
        void image(BufferImage2D& image, BufferUsage usage) {
            AbstractTexture::image<2>(_target, 0, image, usage);
        }

        /**
         * @brief Set image data
         * @param internalFormat    Internal format
         * @param image             @ref Image2D, @ref ImageReference2D or
         *      @ref Trade::ImageData2D
         * @return Reference to self (for method chaining)
         *
         * For better performance when calling @ref setImage() more than once
         * use @ref setStorage() and @ref setSubImage() instead.
         *
         * If @extension{EXT,direct_state_access} is not available, the
         * texture is bound to some texture unit before the operation.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexImage2D}
         *      or @fn_gl_extension{TextureImage2D,EXT,direct_state_access}
         */
        RectangleTexture& setImage(TextureFormat internalFormat, const ImageReference2D& image) {
            DataHelper<2>::setImage(*this, _target, 0, internalFormat, image);
            return *this;
        }

        /** @overload */
        RectangleTexture& setImage(TextureFormat internalFormat, BufferImage2D& image) {
            DataHelper<2>::setImage(*this, _target, 0, internalFormat, image);
            return *this;
        }

        /** @overload */
        RectangleTexture& setImage(TextureFormat internalFormat, BufferImage2D&& image) {
            return setImage(internalFormat, image);
        }

        /**
         * @brief Set image subdata
         * @param offset            Offset where to put data in the texture
         * @param image             @ref Image2D, @ref ImageReference2D or
         *      @ref Trade::ImageData2D
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available, the
         * texture is bound to some texture unit before the operation.
         * @see @ref setStorage(), @ref setImage(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexSubImage2D} or
         *      @fn_gl_extension{TextureSubImage2D,EXT,direct_state_access}
         */
        RectangleTexture& setSubImage(const Vector2i& offset, const ImageReference2D& image) {
            DataHelper<2>::setSubImage(*this, _target, 0, offset, image);
            return *this;
        }

        /** @overload */
        RectangleTexture& setSubImage(const Vector2i& offset, BufferImage2D& image) {
            DataHelper<2>::setSubImage(*this, _target, 0, offset, image);
            return *this;
        }

        /** @overload */
        RectangleTexture& setSubImage(const Vector2i& offset, BufferImage2D&& image) {
            return setSubImage(offset, image);
        }

        /**
         * @brief Invalidate texture image
         *
         * If extension @extension{ARB,invalidate_subdata} (part of OpenGL 4.3)
         * is not available, this function does nothing.
         * @see @ref invalidateSubImage(), @fn_gl{InvalidateTexImage}
         */
        void invalidateImage() { AbstractTexture::invalidateImage(0); }

        /**
         * @brief Invalidate texture subimage
         * @param offset            Offset into the texture
         * @param size              Size of invalidated data
         *
         * If extension @extension{ARB,invalidate_subdata} (part of OpenGL 4.3)
         * is not available, this function does nothing.
         * @see @ref invalidateImage(), @fn_gl{InvalidateTexSubImage}
         */
        void invalidateSubImage(const Vector2i& offset, const Vector2i& size) {
            DataHelper<2>::invalidateSubImage(*this, 0, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        RectangleTexture& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        #endif
};

}
#else
#error this header is available only on desktop OpenGL build
#endif

#endif
