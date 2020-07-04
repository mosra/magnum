#ifndef Magnum_GL_RectangleTexture_h
#define Magnum_GL_RectangleTexture_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::GL::RectangleTexture
 */
#endif

#include "Magnum/Array.h"
#include "Magnum/Sampler.h"
#include "Magnum/GL/AbstractTexture.h"
#include "Magnum/GL/Sampler.h"
#include "Magnum/Math/Vector2.h"

#ifndef MAGNUM_TARGET_GLES
namespace Magnum { namespace GL {

/**
@brief Rectangle texture

See also @ref AbstractTexture documentation for more information.

@section GL-RectangleTexture-usage Usage

Common usage is to fully configure all texture parameters and then set the
data from e.g. @ref Image2D. Example configuration:

@snippet MagnumGL.cpp RectangleTexture-usage

In a shader, the texture is used via @glsl sampler2DRect @ce,
@glsl sampler2DRectShadow @ce, @glsl isampler2DRect @ce or @glsl usampler2DRect @ce.
See @ref AbstractShaderProgram documentation for more information about usage
in shaders.

@see @ref Texture, @ref TextureArray, @ref CubeMapTexture,
    @ref CubeMapTextureArray, @ref BufferTexture, @ref MultisampleTexture
@m_keywords{GL_TEXTURE_RECTANGLE}
@requires_gl31 Extension @gl_extension{ARB,texture_rectangle}
@requires_gl Rectangle textures are not available in OpenGL ES and WebGL.
 */
class MAGNUM_GL_EXPORT RectangleTexture: public AbstractTexture {
    public:
        /**
         * @brief Max supported rectangle texture size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If @gl_extension{ARB,texture_rectangle} (part of
         * OpenGL 3.1) is not available, returns zero vector.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_RECTANGLE_TEXTURE_SIZE}
         */
        static Vector2i maxSize();

        /**
         * @brief @copybrief Texture::compressedBlockSize()
         *
         * See @ref Texture::compressedBlockSize() for more information.
         * @requires_gl43 Extension @gl_extension{ARB,internalformat_query2}
         */
        static Vector2i compressedBlockSize(TextureFormat format) {
            return DataHelper<2>::compressedBlockSize(GL_TEXTURE_RECTANGLE, format);
        }

        /**
         * @brief @copybrief Texture::compressedBlockDataSize()
         *
         * See @ref Texture::compressedBlockDataSize() for more information.
         * @requires_gl43 Extension @gl_extension{ARB,internalformat_query2}
         * @see @ref compressedBlockSize(), @fn_gl_keyword{GetInternalformat}
         *      with @def_gl{TEXTURE_COMPRESSED_BLOCK_SIZE}
         */
        static Int compressedBlockDataSize(TextureFormat format) {
            return AbstractTexture::compressedBlockDataSize(GL_TEXTURE_RECTANGLE, format);
        }

        /**
         * @brief Wrap existing OpenGL rectangle texture object
         * @param id            OpenGL rectangle texture ID
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL texture object
         * with target @def_gl{TEXTURE_RECTANGLE}. Unlike texture created using
         * constructor, the OpenGL object is by default not deleted on
         * destruction, use @p flags for different behavior.
         * @see @ref release()
         */
        static RectangleTexture wrap(GLuint id, ObjectFlags flags = {}) {
            return RectangleTexture{id, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object. If @gl_extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref RectangleTexture(NoCreateT), @ref wrap(),
         *      @fn_gl_keyword{CreateTextures} with @def_gl{TEXTURE_RECTANGLE},
         *      eventually @fn_gl_keyword{GenTextures}
         */
        explicit RectangleTexture(): AbstractTexture(GL_TEXTURE_RECTANGLE) {}

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         * @see @ref RectangleTexture(), @ref wrap()
         */
        explicit RectangleTexture(NoCreateT) noexcept: AbstractTexture{NoCreate, GL_TEXTURE_RECTANGLE} {}

        /** @brief Copying is not allowed */
        RectangleTexture(const RectangleTexture&) = delete;

        /** @brief Move constructor */
        RectangleTexture(RectangleTexture&&) noexcept = default;

        /** @brief Copying is not allowed */
        RectangleTexture& operator=(const RectangleTexture&) = delete;

        /** @brief Move assignment */
        RectangleTexture& operator=(RectangleTexture&&) noexcept = default;

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
         * @requires_gl42 Extension @gl_extension{ARB,shader_image_load_store}
         */
        void bindImage(Int imageUnit, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, 0, false, 0, access, format);
        }

        /**
         * @brief @copybrief Texture::setMinificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinificationFilter() for more information.
         * Initial value is @ref SamplerFilter::Linear.
         */
        RectangleTexture& setMinificationFilter(SamplerFilter filter) {
            AbstractTexture::setMinificationFilter(filter, SamplerMipmap::Base);
            return *this;
        }

        /** @overload */
        RectangleTexture& setMinificationFilter(Magnum::SamplerFilter filter) {
            return setMinificationFilter(samplerFilter(filter));
        }

        /**
         * @brief @copybrief Texture::setMagnificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMagnificationFilter() for more information.
         */
        RectangleTexture& setMagnificationFilter(SamplerFilter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        /** @overload */
        RectangleTexture& setMagnificationFilter(Magnum::SamplerFilter filter) {
            return setMagnificationFilter(samplerFilter(filter));
        }

        /**
         * @brief @copybrief Texture::setWrapping()
         * @return Reference to self (for method chaining)
         *
         * Sets wrapping type for coordinates out of @f$ [ 0, size - 1 ] @f$
         * range. See @ref Texture::setWrapping() for more information. Initial
         * value is @ref SamplerWrapping::ClampToEdge.
         * @attention Only @ref SamplerWrapping::ClampToEdge and
         *      @ref SamplerWrapping::ClampToBorder is supported on this
         *      texture type.
         */
        RectangleTexture& setWrapping(const Array2D<SamplerWrapping>& wrapping) {
            DataHelper<2>::setWrapping(*this, wrapping);
            return *this;
        }

        /** @overload */
        RectangleTexture& setWrapping(const Array2D<Magnum::SamplerWrapping>& wrapping) {
            return setWrapping(samplerWrapping(wrapping));
        }

        /**
         * @brief @copybrief Texture::setBorderColor(const Color4&)
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
         * @brief @copybrief Texture::setBorderColor(const Vector4ui&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Vector4ui&) for more
         * information.
         * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
         */
        RectangleTexture& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @overload
         * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
         */
        RectangleTexture& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setMaxAnisotropy()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxAnisotropy() for more information.
         */
        RectangleTexture& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setSrgbDecode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSrgbDecode() for more information.
         * @requires_extension Extension @gl_extension{EXT,texture_sRGB_decode}
         */
        RectangleTexture& setSrgbDecode(bool decode) {
            AbstractTexture::setSrgbDecode(decode);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setSwizzle()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSwizzle() for more information.
         * @requires_gl33 Extension @gl_extension{ARB,texture_swizzle}
         */
        template<char r, char g, char b, char a> RectangleTexture& setSwizzle() {
            AbstractTexture::setSwizzle<r, g, b, a>();
            return *this;
        }

        /**
         * @brief @copybrief Texture::setCompareMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareMode() for more information.
         */
        RectangleTexture& setCompareMode(SamplerCompareMode mode) {
            AbstractTexture::setCompareMode(mode);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setCompareFunction()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareFunction() for more information.
         */
        RectangleTexture& setCompareFunction(SamplerCompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setDepthStencilMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setDepthStencilMode() for more information.
         * @requires_gl43 Extension @gl_extension{ARB,stencil_texturing}
         */
        RectangleTexture& setDepthStencilMode(SamplerDepthStencilMode mode) {
            AbstractTexture::setDepthStencilMode(mode);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setStorage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setStorage() for more information.
         * @see @ref maxSize()
         */
        RectangleTexture& setStorage(TextureFormat internalFormat, const Vector2i& size) {
            DataHelper<2>::setStorage(*this, 1, internalFormat, size);
            return *this;
        }

        /**
         * @brief Texture image size
         *
         * See @ref Texture::imageSize() for more information.
         */
        Vector2i imageSize() { return DataHelper<2>::imageSize(*this, 0); }

        /**
         * @brief Read texture to an image
         *
         * See @ref Texture::image(Int, Image&) for more information.
         */
        void image(Image2D& image) {
            AbstractTexture::image<2>(0, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp RectangleTexture-image1
         */
        Image2D image(Image2D&& image);

        /**
         * @brief Read texture to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref image(Image2D&) the function reads the pixels into
         * the memory provided by @p image, expecting it's not @cpp nullptr @ce
         * and its size is the same as texture size.
         */
        void image(const MutableImageView2D& image) {
            AbstractTexture::image<2>(0, image);
        }

        /**
         * @brief Read texture to a buffer image
         *
         * See @ref Texture::image(Int, BufferImage&, BufferUsage) for more
         * information.
         */
        void image(BufferImage2D& image, BufferUsage usage) {
            AbstractTexture::image<2>(0, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp RectangleTexture-image2
         */
        BufferImage2D image(BufferImage2D&& image, BufferUsage usage);

        /**
         * @brief Read compressed texture to an image
         *
         * See @ref Texture::compressedImage(Int, CompressedImage&) for more
         * information.
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         */
        void compressedImage(CompressedImage2D& image) {
            AbstractTexture::compressedImage<2>(0, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp RectangleTexture-compressedImage1
         */
        CompressedImage2D compressedImage(CompressedImage2D&& image);

        /**
         * @brief Read compressed texture to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref compressedImage(CompressedImage2D&) the function
         * reads the pixels into the memory provided by @p image, expecting
         * it's not @cpp nullptr @ce, its format is the same as texture format
         * and its size is the same as texture size.
         */
        void compressedImage(const MutableCompressedImageView2D& image) {
            AbstractTexture::compressedImage<2>(0, image);
        }

        /**
         * @brief Read compressed texture to a buffer image
         *
         * See @ref Texture::compressedImage(Int, CompressedBufferImage&, BufferUsage)
         * for more information.
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         */
        void compressedImage(CompressedBufferImage2D& image, BufferUsage usage) {
            AbstractTexture::compressedImage<2>(0, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp RectangleTexture-compressedImage2
         */
        CompressedBufferImage2D compressedImage(CompressedBufferImage2D&& image, BufferUsage usage);

        /**
         * @brief Read a texture range to an image
         *
         * See @ref Texture::subImage(Int, const RangeTypeFor<dimensions, Int>&, Image&)
         * for more information.
         * @requires_gl45 Extension @gl_extension{ARB,get_texture_sub_image}
         */
        void subImage(const Range2Di& range, Image2D& image) {
            AbstractTexture::subImage<2>(0, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp RectangleTexture-subImage1
         */
        Image2D subImage(const Range2Di& range, Image2D&& image);

        /**
         * @brief Read a range of given texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref subImage(const Range2Di&, Image2D&) the function
         * reads the pixels into the memory provided by @p image, expecting
         * it's not @cpp nullptr @ce and its size is the same as @p range size.
         */
        void subImage(const Range2Di& range, const MutableImageView2D& image) {
            AbstractTexture::subImage<2>(0, range, image);
        }

        /**
         * @brief Read a texture range to a buffer image
         *
         * See @ref Texture::subImage(Int, const RangeTypeFor<dimensions, Int>&, BufferImage&, BufferUsage)
         * for more information.
         * @requires_gl45 Extension @gl_extension{ARB,get_texture_sub_image}
         */
        void subImage(const Range2Di& range, BufferImage2D& image, BufferUsage usage) {
            AbstractTexture::subImage<2>(0, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp RectangleTexture-subImage2
         */
        BufferImage2D subImage(const Range2Di& range, BufferImage2D&& image, BufferUsage usage);

        /**
         * @brief Read a compressed texture range to an image
         *
         * See @ref Texture::compressedSubImage(Int, const RangeTypeFor<dimensions, Int>&, CompressedImage&)
         * for more information.
         * @requires_gl45 Extension @gl_extension{ARB,get_texture_sub_image}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl43 Extension @gl_extension{ARB,internalformat_query2} if
         *      @ref CompressedPixelStorage::compressedBlockSize() and
         *      @ref CompressedPixelStorage::compressedBlockDataSize() are not
         *      set to non-zero values
         */
        void compressedSubImage(const Range2Di& range, CompressedImage2D& image) {
            AbstractTexture::compressedSubImage<2>(0, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp RectangleTexture-compressedSubImage1
         */
        CompressedImage2D compressedSubImage(const Range2Di& range, CompressedImage2D&& image);

        /**
         * @brief Read a compressed texture range to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref compressedSubImage(const Range2Di&, CompressedImage2D&)
         * the function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce, its format is the same as
         * texture format and its size is the same as @p range size.
         */
        void compressedSubImage(const Range2Di& range, const MutableCompressedImageView2D& image) {
            AbstractTexture::compressedSubImage<2>(0, range, image);
        }

        /**
         * @brief Read a compressed texture range to a buffer image
         *
         * See @ref Texture::compressedSubImage(Int, const RangeTypeFor<dimensions, Int>&, CompressedBufferImage&, BufferUsage)
         * for more information.
         * @requires_gl45 Extension @gl_extension{ARB,get_texture_sub_image}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl43 Extension @gl_extension{ARB,internalformat_query2} if
         *      @ref CompressedPixelStorage::compressedBlockSize() and
         *      @ref CompressedPixelStorage::compressedBlockDataSize() are not
         *      set to non-zero values
         */
        void compressedSubImage(const Range2Di& range, CompressedBufferImage2D& image, BufferUsage usage) {
            AbstractTexture::compressedSubImage<2>(0, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp RectangleTexture-compressedSubImage2
         */
        CompressedBufferImage2D compressedSubImage(const Range2Di& range, CompressedBufferImage2D&& image, BufferUsage usage);

        /**
         * @brief @copybrief Texture::setImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setImage() for more information.
         * @see @ref maxSize()
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        RectangleTexture& setImage(TextureFormat internalFormat, const ImageView2D& image) {
            DataHelper<2>::setImage(*this, 0, internalFormat, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        RectangleTexture& setImage(TextureFormat internalFormat, BufferImage2D& image) {
            DataHelper<2>::setImage(*this, 0, internalFormat, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        RectangleTexture& setImage(TextureFormat internalFormat, BufferImage2D&& image) {
            return setImage(internalFormat, image);
        }

        /**
         * @brief @copybrief Texture::setCompressedImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompressedImage() for more information.
         * @see @ref maxSize()
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        RectangleTexture& setCompressedImage(const CompressedImageView2D& image) {
            DataHelper<2>::setCompressedImage(*this, 0, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        RectangleTexture& setCompressedImage(CompressedBufferImage2D& image) {
            DataHelper<2>::setCompressedImage(*this, 0, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        RectangleTexture& setCompressedImage(CompressedBufferImage2D&& image) {
            return setCompressedImage(image);
        }

        /**
         * @brief @copybrief Texture::setSubImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSubImage() for more information.
         */
        RectangleTexture& setSubImage(const Vector2i& offset, const ImageView2D& image) {
            DataHelper<2>::setSubImage(*this, 0, offset, image);
            return *this;
        }

        /** @overload */
        RectangleTexture& setSubImage(const Vector2i& offset, BufferImage2D& image) {
            DataHelper<2>::setSubImage(*this, 0, offset, image);
            return *this;
        }

        /** @overload */
        RectangleTexture& setSubImage(const Vector2i& offset, BufferImage2D&& image) {
            return setSubImage(offset, image);
        }

        /**
         * @brief @copybrief Texture::setCompressedSubImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompressedSubImage() for more information.
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         */
        RectangleTexture& setCompressedSubImage(const Vector2i& offset, const CompressedImageView2D& image) {
            DataHelper<2>::setCompressedSubImage(*this, 0, offset, image);
            return *this;
        }

        /** @overload */
        RectangleTexture& setCompressedSubImage(const Vector2i& offset, CompressedBufferImage2D& image) {
            DataHelper<2>::setCompressedSubImage(*this, 0, offset, image);
            return *this;
        }

        /** @overload */
        RectangleTexture& setCompressedSubImage(const Vector2i& offset, CompressedBufferImage2D&& image) {
            return setCompressedSubImage(offset, image);
        }

        /**
         * @brief Invalidate texture
         *
         * See @ref Texture::invalidateImage() for more information.
         */
        void invalidateImage() { AbstractTexture::invalidateImage(0); }

        /**
         * @brief Invalidate subtexture
         * @param offset            Offset into the texture
         * @param size              Size of invalidated data
         *
         * See @ref Texture::invalidateSubImage() for more information.
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
        template<std::size_t size> RectangleTexture& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit RectangleTexture(GLuint id, ObjectFlags flags) noexcept: AbstractTexture{id, GL_TEXTURE_RECTANGLE, flags} {}
};

}}
#else
#error this header is not available in OpenGL ES build
#endif

#endif
