#ifndef Magnum_GL_TextureArray_h
#define Magnum_GL_TextureArray_h
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

#ifndef MAGNUM_TARGET_GLES2
/** @file
 * @brief Class @ref Magnum::GL::TextureArray, typedef @ref Magnum::GL::Texture1DArray, @ref Magnum::GL::Texture2DArray
 */
#endif

#include "Magnum/Array.h"
#include "Magnum/Sampler.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractTexture.h"
#include "Magnum/GL/Sampler.h"
#include "Magnum/Math/Vector3.h"

#ifndef MAGNUM_TARGET_GLES2
namespace Magnum { namespace GL {

namespace Implementation {
    template<UnsignedInt> constexpr GLenum textureArrayTarget();
    #ifndef MAGNUM_TARGET_GLES
    template<> constexpr GLenum textureArrayTarget<1>() { return GL_TEXTURE_1D_ARRAY; }
    #endif
    template<> constexpr GLenum textureArrayTarget<2>() { return GL_TEXTURE_2D_ARRAY; }
}

/**
@brief Texture array

Template class for one- and two-dimensional texture arrays. See also
@ref AbstractTexture documentation for more information.

@section GL-TextureArray-usage Usage

See @ref Texture documentation for introduction.

Common usage is to fully configure all texture parameters and then set the
data. Example configuration:

@snippet MagnumGL.cpp TextureArray-usage1

It is often more convenient to first allocate the memory for all layers by
calling @ref setStorage() and then specify each layer separately using
@ref setSubImage():

@snippet MagnumGL.cpp TextureArray-usage2

In shader, the texture is used via @glsl sampler1DArray @ce / @glsl sampler2DArray @ce,
@glsl sampler1DArrayShadow @ce / @glsl sampler1DArrayShadow @ce, @glsl isampler1DArray @ce
/ @glsl isampler2DArray @ce or @glsl usampler1DArray @ce / @glsl usampler2DArray @ce.
See @ref AbstractShaderProgram documentation for more information about usage
in shaders.

@see @ref Texture1DArray, @ref Texture2DArray, @ref Texture,
    @ref CubeMapTexture, @ref CubeMapTextureArray, @ref RectangleTexture,
    @ref BufferTexture, @ref MultisampleTexture
@m_keywords{GL_TEXTURE_1D_ARRAY GL_TEXTURE_2D_ARRAY}
@requires_gl30 Extension @gl_extension{EXT,texture_array}
@requires_gles30 Array textures are not available in OpenGL ES 2.0.
@requires_webgl20 Array textures are not available in WebGL 1.0.
@requires_gl 1D array textures are not available in OpenGL ES or WebGL, only
    2D ones.
@todo Fix this when @gl_extension{NV,texture_array} is in ES2 extension headers
 */
template<UnsignedInt dimensions> class TextureArray: public AbstractTexture {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Texture dimension count */
        };

        /**
         * @brief Max supported texture array size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @gl_extension{EXT,texture_array} (part of
         * OpenGL 3.0) is not available, returns zero vector.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_TEXTURE_SIZE} and
         *      @def_gl_keyword{MAX_ARRAY_TEXTURE_LAYERS}
         */
        static VectorTypeFor<dimensions+1, Int> maxSize();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief @copybrief Texture::compressedBlockSize()
         *
         * See @ref Texture::compressedBlockSize() for more information.
         * @requires_gl43 Extension @gl_extension{ARB,internalformat_query2}
         * @requires_gl Compressed texture queries are not available in OpenGL
         *      ES.
         */
        static VectorTypeFor<dimensions, Int> compressedBlockSize(TextureFormat format) {
            return DataHelper<dimensions>::compressedBlockSize(Implementation::textureArrayTarget<dimensions>(), format);
        }

        /**
         * @brief @copybrief Texture::compressedBlockDataSize()
         *
         * See @ref Texture::compressedBlockDataSize() for more information.
         * @requires_gl43 Extension @gl_extension{ARB,internalformat_query2}
         * @requires_gl Compressed texture queries are not available in OpenGL
         *      ES.
         * @see @ref compressedBlockSize(), @fn_gl{Getinternalformat} with
         *      @def_gl_keyword{TEXTURE_COMPRESSED_BLOCK_SIZE}
         */
        static Int compressedBlockDataSize(TextureFormat format) {
            return AbstractTexture::compressedBlockDataSize(Implementation::textureArrayTarget<dimensions>(), format);
        }
        #endif

        /**
         * @brief Wrap existing OpenGL texture array object
         * @param id            OpenGL texture array ID
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL texture object
         * with target @def_gl{TEXTURE_1D_ARRAY} or @def_gl{TEXTURE_2D_ARRAY}
         * based on dimension count. Unlike texture created using constructor,
         * the OpenGL object is by default not deleted on destruction, use
         * @p flags for different behavior.
         * @see @ref release()
         */
        static TextureArray<dimensions> wrap(GLuint id, ObjectFlags flags = {}) {
            return TextureArray<dimensions>{id, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object. If @gl_extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref TextureArray(NoCreateT), @ref wrap(), @fn_gl_keyword{CreateTextures}
         *      with @def_gl{TEXTURE_1D_ARRAY} or @def_gl{TEXTURE_2D_ARRAY},
         *      eventually @fn_gl_keyword{GenTextures}
         */
        explicit TextureArray(): AbstractTexture(Implementation::textureArrayTarget<dimensions>()) {}

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
         * @see @ref TextureArray(), @ref wrap()
         */
        explicit TextureArray(NoCreateT) noexcept: AbstractTexture{NoCreate, Implementation::textureArrayTarget<dimensions>()} {}

        /** @brief Copying is not allowed */
        TextureArray(const TextureArray<dimensions>&) = delete;

        /** @brief Move constructor */
        TextureArray(TextureArray<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        TextureArray<dimensions>& operator=(const TextureArray<dimensions>&) = delete;

        /** @brief Move assignment */
        TextureArray<dimensions>& operator=(TextureArray<dimensions>&&) noexcept = default;

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Bind level of given texture layer to given image unit
         * @param imageUnit Image unit
         * @param level     Texture level
         * @param layer     Texture layer
         * @param access    Image access
         * @param format    Image format
         *
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bindImages(Int, std::initializer_list<AbstractTexture*>),
         *      @ref bindImageLayered(), @ref unbindImage(), @ref unbindImages(),
         *      @ref AbstractShaderProgram::maxImageUnits(),
         *      @fn_gl_keyword{BindImageTexture}
         * @requires_gl42 Extension @gl_extension{ARB,shader_image_load_store}
         * @requires_gles31 Shader image load/store is not available in OpenGL
         *      ES 3.0 and older.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        void bindImage(Int imageUnit, Int level, Int layer, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, level, false, layer, access, format);
        }

        /**
         * @brief Bind level of layered texture to given image unit
         * @param imageUnit Image unit
         * @param level     Texture level
         * @param access    Image access
         * @param format    Image format
         *
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bindImages(Int, std::initializer_list<AbstractTexture*>),
         *      @ref bindImage(), @ref unbindImage(), @ref unbindImages(),
         *      @ref AbstractShaderProgram::maxImageUnits(),
         *      @fn_gl_keyword{BindImageTexture}
         * @requires_gl42 Extension @gl_extension{ARB,shader_image_load_store}
         * @requires_gles31 Shader image load/store is not available in OpenGL
         *      ES 3.0 and older.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        void bindImageLayered(Int imageUnit, Int level, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, level, true, 0, access, format);
        }
        #endif

        /**
         * @brief @copybrief Texture::setBaseLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBaseLevel() for more information.
         */
        TextureArray<dimensions>& setBaseLevel(Int level) {
            AbstractTexture::setBaseLevel(level);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setMaxLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLevel() for more information.
         */
        TextureArray<dimensions>& setMaxLevel(Int level) {
            AbstractTexture::setMaxLevel(level);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setMinificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinificationFilter() for more information.
         */
        TextureArray<dimensions>& setMinificationFilter(SamplerFilter filter, SamplerMipmap mipmap = SamplerMipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /** @overload */
        TextureArray<dimensions>& setMinificationFilter(Magnum::SamplerFilter filter, Magnum::SamplerMipmap mipmap = Magnum::SamplerMipmap::Base) {
            return setMinificationFilter(samplerFilter(filter), samplerMipmap(mipmap));
        }

        /**
         * @brief @copybrief Texture::setMagnificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMagnificationFilter() for more information.
         */
        TextureArray<dimensions>& setMagnificationFilter(SamplerFilter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        /** @overload */
        TextureArray<dimensions>& setMagnificationFilter(Magnum::SamplerFilter filter) {
            return setMagnificationFilter(samplerFilter(filter));
        }

        /**
         * @brief @copybrief Texture::setMinLod()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinLod() for more information.
         */
        TextureArray<dimensions>& setMinLod(Float lod) {
            AbstractTexture::setMinLod(lod);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setMaxLod()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLod() for more information.
         */
        TextureArray<dimensions>& setMaxLod(Float lod) {
            AbstractTexture::setMaxLod(lod);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief @copybrief Texture::setLodBias()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setLodBias() for more information.
         * @requires_gl Texture LOD bias can be specified only directly in
         *      fragment shader in OpenGL ES and WebGL.
         */
        TextureArray<dimensions>& setLodBias(Float bias) {
            AbstractTexture::setLodBias(bias);
            return *this;
        }
        #endif

        /**
         * @brief @copybrief Texture::setWrapping()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setWrapping() for more information.
         */
        TextureArray<dimensions>& setWrapping(const Array<dimensions, SamplerWrapping>& wrapping) {
            DataHelper<dimensions>::setWrapping(*this, wrapping);
            return *this;
        }

        /** @overload */
        TextureArray<dimensions>& setWrapping(const Array<dimensions, Magnum::SamplerWrapping>& wrapping) {
            return setWrapping(samplerWrapping(wrapping));
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief @copybrief Texture::setBorderColor(const Color4&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Color4&) for more
         * information.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,texture_border_clamp} or
         *      @gl_extension{NV,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        TextureArray<dimensions>& setBorderColor(const Color4& color) {
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
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        TextureArray<dimensions>& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @overload
         * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        TextureArray<dimensions>& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        #endif

        /**
         * @brief @copybrief Texture::setMaxAnisotropy()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxAnisotropy() for more information.
         */
        TextureArray<dimensions>& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief @copybrief Texture::setSrgbDecode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSrgbDecode() for more information.
         * @requires_extension Extension @gl_extension{EXT,texture_sRGB_decode}
         * @requires_es_extension Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,texture_sRGB_decode}
         * @requires_gles sRGB decode is not available in WebGL.
         */
        TextureArray<dimensions>& setSrgbDecode(bool decode) {
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
        template<char r, char g, char b, char a> TextureArray<dimensions>& setSwizzle() {
            AbstractTexture::setSwizzle<r, g, b, a>();
            return *this;
        }
        #endif

        /**
         * @brief @copybrief Texture::setCompareMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareMode() for more information.
         */
        TextureArray<dimensions>& setCompareMode(SamplerCompareMode mode) {
            AbstractTexture::setCompareMode(mode);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setCompareFunction()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareFunction() for more information.
         */
        TextureArray<dimensions>& setCompareFunction(SamplerCompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief @copybrief Texture::setDepthStencilMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setDepthStencilMode() for more information.
         * @requires_gl43 Extension @gl_extension{ARB,stencil_texturing}
         * @requires_gles31 Stencil texturing is not available in OpenGL ES 3.0
         *      and older.
         * @requires_gles Stencil texturing is not available in WebGL.
         */
        TextureArray<dimensions>& setDepthStencilMode(SamplerDepthStencilMode mode) {
            AbstractTexture::setDepthStencilMode(mode);
            return *this;
        }
        #endif

        /**
         * @brief @copybrief Texture::setStorage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setStorage() for more information.
         * @see @ref maxSize()
         */
        TextureArray<dimensions>& setStorage(Int levels, TextureFormat internalFormat, const VectorTypeFor<dimensions+1, Int>& size) {
            DataHelper<dimensions+1>::setStorage(*this, levels, internalFormat, size);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief @copybrief Texture::imageSize()
         *
         * See @ref Texture::imageSize() for more information.
         * @requires_gles31 Texture image size queries are not available in
         *      OpenGL ES 3.0 and older.
         * @requires_gles Texture image size queries are not available in
         *      WebGL.
         */
        VectorTypeFor<dimensions+1, Int> imageSize(Int level) {
            return DataHelper<dimensions+1>::imageSize(*this, level);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Read given texture mip level to an image
         *
         * See @ref Texture::image(Int, Image&) for more information.
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void image(Int level, Image<dimensions+1>& image) {
            AbstractTexture::image<dimensions+1>(level, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp TextureArray-image1
         */
        Image<dimensions+1> image(Int level, Image<dimensions+1>&& image);

        /**
         * @brief Read given texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref image(Int, Image<dimensions+1>&) the function reads
         * the pixels into the memory provided by @p image, expecting it's not
         * @cpp nullptr @ce and its size is the same as size of given @p level.
         */
        void image(Int level, const BasicMutableImageView<dimensions+1>& image) {
            AbstractTexture::image<dimensions+1>(level, image);
        }

        /**
         * @brief Read given texture mip level to a buffer image
         *
         * See @ref Texture::image(Int, BufferImage&, BufferUsage) for more
         * information.
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void image(Int level, BufferImage<dimensions+1>& image, BufferUsage usage) {
            AbstractTexture::image<dimensions+1>(level, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp TextureArray-image2
         */
        BufferImage<dimensions+1> image(Int level, BufferImage<dimensions+1>&& image, BufferUsage usage);

        /**
         * @brief Read given compressed texture mip level to an image
         *
         * See @ref Texture::compressedImage(Int, CompressedImage&) for more
         * information.
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void compressedImage(Int level, CompressedImage<dimensions+1>& image) {
            AbstractTexture::compressedImage<dimensions+1>(level, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp TextureArray-compressedImage1
         */
        CompressedImage<dimensions+1> compressedImage(Int level, CompressedImage<dimensions+1>&& image);

        /**
         * @brief Read given compressed texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref compressedImage(Int, CompressedImage<dimensions+1>&)
         * the function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce, its format is the same as
         * texture format and its size is the same as size of given @p level.
         */
        void compressedImage(Int level, const BasicMutableCompressedImageView<dimensions+1>& image) {
            AbstractTexture::compressedImage<dimensions+1>(level, image);
        }

        /**
         * @brief Read given compressed texture mip level to a buffer image
         *
         * See @ref Texture::compressedImage(Int, CompressedBufferImage&, BufferUsage)
         * for more information.
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void compressedImage(Int level, CompressedBufferImage<dimensions+1>& image, BufferUsage usage) {
            AbstractTexture::compressedImage<dimensions+1>(level, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp TextureArray-compressedImage2
         */
        CompressedBufferImage<dimensions+1> compressedImage(Int level, CompressedBufferImage<dimensions+1>&& image, BufferUsage usage);

        /**
         * @brief Read a range of given texture mip level to an image
         *
         * See @ref Texture::subImage(Int, const RangeTypeFor<dimensions, Int>&, Image&)
         * for more information.
         * @requires_gl45 Extension @gl_extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void subImage(Int level, const RangeTypeFor<dimensions+1, Int>& range, Image<dimensions+1>& image) {
            AbstractTexture::subImage<dimensions+1>(level, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp TextureArray-subImage1
         */
        Image<dimensions+1> subImage(Int level, const RangeTypeFor<dimensions+1, Int>& range, Image<dimensions+1>&& image);

        /**
         * @brief Read a range of given texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref subImage(Int, const RangeTypeFor<dimensions+1, Int>&, Image<dimensions+1>&)
         * the function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce and its size is the same as
         * @p range size.
         */
        void subImage(Int level, const RangeTypeFor<dimensions+1, Int>& range, const BasicMutableImageView<dimensions+1>& image) {
            AbstractTexture::subImage<dimensions+1>(level, range, image);
        }

        /**
         * @brief Read a range of given texture mip level to a buffer image
         *
         * See @ref Texture::subImage(Int, const RangeTypeFor<dimensions, Int>&, BufferImage&, BufferUsage)
         * for more information.
         * @requires_gl45 Extension @gl_extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void subImage(Int level, const RangeTypeFor<dimensions+1, Int>& range, BufferImage<dimensions+1>& image, BufferUsage usage) {
            AbstractTexture::subImage<dimensions+1>(level, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp TextureArray-subImage2
         */
        BufferImage<dimensions+1> subImage(Int level, const RangeTypeFor<dimensions+1, Int>& range, BufferImage<dimensions+1>&& image, BufferUsage usage);

        /**
         * @brief Read a range of given compressed texture mip level to an image
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
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void compressedSubImage(Int level, const RangeTypeFor<dimensions+1, Int>& range, CompressedImage<dimensions+1>& image) {
            AbstractTexture::compressedSubImage<dimensions+1>(level, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp TextureArray-compressedSubImage1
         */
        CompressedImage<dimensions+1> compressedSubImage(Int level, const RangeTypeFor<dimensions+1, Int>& range, CompressedImage<dimensions+1>&& image);

        /**
         * @brief Read a range of given compressed texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref compressedSubImage(Int, const RangeTypeFor<dimensions+1, Int>&, CompressedImage<dimensions+1>&)
         * the function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce, its format is the same as
         * texture format and its size is the same as @p range size.
         */
        void compressedSubImage(Int level, const RangeTypeFor<dimensions+1, Int>& range, const BasicMutableCompressedImageView<dimensions+1>& image) {
            AbstractTexture::compressedSubImage<dimensions+1>(level, range, image);
        }

        /**
         * @brief Read a range of given compressed texture mip level to a buffer image
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
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void compressedSubImage(Int level, const RangeTypeFor<dimensions+1, Int>& range, CompressedBufferImage<dimensions+1>& image, BufferUsage usage) {
            AbstractTexture::compressedSubImage<dimensions+1>(level, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp TextureArray-compressedSubImage2
         */
        CompressedBufferImage<dimensions+1> compressedSubImage(Int level, const RangeTypeFor<dimensions+1, Int>& range, CompressedBufferImage<dimensions+1>&& image, BufferUsage usage);
        #endif

        /**
         * @brief @copybrief Texture::setImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setImage() for more information.
         * @see @ref maxSize()
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        TextureArray<dimensions>& setImage(Int level, TextureFormat internalFormat, const BasicImageView<dimensions+1>& image) {
            DataHelper<dimensions+1>::setImage(*this, level, internalFormat, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        TextureArray<dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions+1>& image) {
            DataHelper<dimensions+1>::setImage(*this, level, internalFormat, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        TextureArray<dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions+1>&& image) {
            return setImage(level, internalFormat, image);
        }

        /**
         * @brief @copybrief Texture::setCompressedImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompressedImage() for more information.
         * @see @ref maxSize()
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        TextureArray<dimensions>& setCompressedImage(Int level, const BasicCompressedImageView<dimensions+1>& image) {
            DataHelper<dimensions+1>::setCompressedImage(*this, level, image);
            return *this;
        }

        /** @overload
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        TextureArray<dimensions>& setCompressedImage(Int level, CompressedBufferImage<dimensions+1>& image) {
            DataHelper<dimensions+1>::setCompressedImage(*this, level, image);
            return *this;
        }

        /** @overload
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        TextureArray<dimensions>& setCompressedImage(Int level, CompressedBufferImage<dimensions+1>&& image) {
            return setCompressedImage(level, image);
        }

        /**
         * @brief Set image subdata
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             @ref Image, @ref ImageView or
         *      @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already).
         * @see @ref setStorage(), @fn_gl{PixelStore}, then
         *      @fn_gl2_keyword{TextureSubImage2D,TexSubImage2D}/
         *      @fn_gl2_keyword{TextureSubImage3D,TexSubImage3D},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexSubImage2D} / @fn_gl_keyword{TexSubImage3D}
         */
        TextureArray<dimensions>& setSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, const BasicImageView<dimensions+1>& image) {
            DataHelper<dimensions+1>::setSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload */
        TextureArray<dimensions>& setSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, BufferImage<dimensions+1>& image) {
            DataHelper<dimensions+1>::setSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload */
        TextureArray<dimensions>& setSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, BufferImage<dimensions+1>&& image) {
            return setSubImage(level, offset, image);
        }

        /**
         * @brief Set compressed image subdata
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             @ref CompressedImage, @ref CompressedImageView
         *      or compressed @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already).
         * @see @ref setStorage(), @fn_gl{PixelStore}, then
         *      @fn_gl2_keyword{CompressedTextureSubImage2D,CompressedTexSubImage2D}/
         *      @fn_gl2_keyword{CompressedTextureSubImage3D,CompressedTexSubImage3D},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{CompressedTexSubImage2D} / @fn_gl_keyword{CompressedTexSubImage3D}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         */
        TextureArray<dimensions>& setCompressedSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, const BasicCompressedImageView<dimensions+1>& image) {
            DataHelper<dimensions+1>::setCompressedSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         */
        TextureArray<dimensions>& setCompressedSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, CompressedBufferImage<dimensions+1>& image) {
            DataHelper<dimensions+1>::setCompressedSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         */
        TextureArray<dimensions>& setCompressedSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, CompressedBufferImage<dimensions+1>&& image) {
            return setCompressedSubImage(level, offset, image);
        }

        /**
         * @brief @copybrief Texture::generateMipmap()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::generateMipmap() for more information.
         * @requires_gl30 Extension @gl_extension{ARB,framebuffer_object}
         */
        TextureArray<dimensions>& generateMipmap() {
            AbstractTexture::generateMipmap();
            return *this;
        }

        /**
         * @brief @copybrief Texture::invalidateImage()
         *
         * See @ref Texture::invalidateImage() for more information.
         */
        void invalidateImage(Int level) { AbstractTexture::invalidateImage(level); }

        /**
         * @brief @copybrief Texture::invalidateSubImage()
         *
         * See @ref Texture::invalidateSubImage() for more information.
         */
        void invalidateSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, const VectorTypeFor<dimensions+1, Int>& size) {
            DataHelper<dimensions+1>::invalidateSubImage(*this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(MAGNUM_TARGET_WEBGL)
        TextureArray<dimensions>& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        template<std::size_t size> TextureArray<dimensions>& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit TextureArray(GLuint id, ObjectFlags flags): AbstractTexture{id, Implementation::textureArrayTarget<dimensions>(), flags} {}
};

#ifndef MAGNUM_TARGET_GLES
/**
@brief One-dimensional texture array

@requires_gl30 Extension @gl_extension{EXT,texture_array}
@requires_gl Only @ref Texture2DArray is available in OpenGL ES and WebGL.
*/
typedef TextureArray<1> Texture1DArray;
#endif

/**
@brief Two-dimensional texture array

@requires_gl30 Extension @gl_extension{EXT,texture_array}
@requires_gles30 Array textures are not available in OpenGL ES 2.0.
@requires_webgl20 Array textures are not available in WebGL 1.0.
*/
typedef TextureArray<2> Texture2DArray;

}}
#else
#error this header is not available in OpenGL ES 2.0 build
#endif

#endif
