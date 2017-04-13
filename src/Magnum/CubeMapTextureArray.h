#ifndef Magnum_CubeMapTextureArray_h
#define Magnum_CubeMapTextureArray_h
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
 * @brief Class @ref Magnum::CubeMapTextureArray
 */
#endif

#include "Magnum/AbstractTexture.h"
#include "Magnum/Array.h"
#include "Magnum/Math/Vector3.h"

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
namespace Magnum {

/**
@brief Cube map texture array

See @ref CubeMapTexture documentation for introduction.

## Usage

See @ref Texture documentation for introduction.

Common usage is to specify each layer and face separately using @ref setSubImage().
You have to allocate the memory for all layers and faces first by calling
@ref setStorage(). Example: array with 4 layers of cube maps, each cube map
consisting of six 64x64 images, i.e. 24 layers total:
@code
CubeMapTextureArray texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    // ...
    .setStorage(Math::log2(64)+1, TextureFormat::RGBA8, {64, 64, 24});

for(std::size_t i = 0; i != 4; i += 6) {
    Image3D imagePositiveX(PixelFormat::RGBA, PixelType::UnsignedByte, {64, 64, 1}, data);
    // ...
    texture.setSubImage(0, Vector3i::zAxis(i+0), imagePositiveX);
    texture.setSubImage(0, Vector3i::zAxis(i+1), imageNegativeX);
    texture.setSubImage(0, Vector3i::zAxis(i+2), imagePositiveY);
    // ...
}

texture.generateMipmap();
@endcode

In shader, the texture is used via `samplerCubeArray`, `samplerCubeArrayShadow`,
`isamplerCubeArray` or `usamplerCubeArray`. Unlike in classic textures,
coordinates for cube map texture arrays is signed four-part vector. First three
parts define vector from the center of the cube which intersects with one of
the six sides of the cube map, fourth part is layer in the array. See
@ref AbstractShaderProgram for more information about usage in shaders.

@see @ref Renderer::Feature::SeamlessCubeMapTexture, @ref CubeMapTexture,
    @ref Texture, @ref TextureArray, @ref RectangleTexture, @ref BufferTexture,
    @ref MultisampleTexture
@requires_gl40 Extension @extension{ARB,texture_cube_map_array}
@requires_gles30 Not defined in OpenGL ES 2.0.
@requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
    @extension{EXT,texture_cube_map_array}
@requires_gles Cube map texture arrays are not available in WebGL.
*/
class MAGNUM_EXPORT CubeMapTextureArray: public AbstractTexture {
    public:
        /**
         * @brief Max supported size of one side of cube map texture array
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If @extension{ARB,texture_cube_map_array} (part of
         * OpenGL 4.0) is not available, returns zero vector.
         * @see @fn_gl{Get} with @def_gl{MAX_CUBE_MAP_TEXTURE_SIZE} and
         *      @def_gl{MAX_ARRAY_TEXTURE_LAYERS}
         */
        static Vector3i maxSize();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::compressedBlockSize()
         *
         * See @ref Texture::compressedBlockSize() for more information.
         * @requires_gl43 Extension @extension{ARB,internalformat_query2}
         * @requires_gl Compressed texture queries are not available in OpenGL
         *      ES.
         */
        static Vector2i compressedBlockSize(TextureFormat format) {
            return DataHelper<2>::compressedBlockSize(GL_TEXTURE_CUBE_MAP_ARRAY, format);
        }

        /**
         * @copybrief Texture::compressedBlockDataSize()
         *
         * See @ref Texture::compressedBlockDataSize() for more information.
         * @requires_gl43 Extension @extension{ARB,internalformat_query2}
         * @requires_gl Compressed texture queries are not available in OpenGL
         *      ES.
         * @see @ref compressedBlockSize(), @fn_gl{Getinternalformat} with
         *      @def_gl{TEXTURE_COMPRESSED_BLOCK_SIZE}
         */
        static Int compressedBlockDataSize(TextureFormat format) {
            return AbstractTexture::compressedBlockDataSize(GL_TEXTURE_CUBE_MAP_ARRAY, format);
        }
        #endif

        /**
         * @brief Wrap existing OpenGL cube map array texture object
         * @param id            OpenGL cube map array texture ID
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL texture object
         * with target @def_gl{TEXTURE_CUBE_MAP_ARRAY}. Unlike texture created
         * using constructor, the OpenGL object is by default not deleted on
         * destruction, use @p flags for different behavior.
         * @see @ref release()
         */
        static CubeMapTextureArray wrap(GLuint id, ObjectFlags flags = {}) {
            return CubeMapTextureArray{id, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref CubeMapTextureArray(NoCreateT), @ref wrap(),
         *      @fn_gl{CreateTextures} with @def_gl{TEXTURE_CUBE_MAP_ARRAY},
         *      eventually @fn_gl{GenTextures}
         */
        explicit CubeMapTextureArray():
            #ifndef MAGNUM_TARGET_GLES
            AbstractTexture{GL_TEXTURE_CUBE_MAP_ARRAY} {}
            #else
            AbstractTexture{GL_TEXTURE_CUBE_MAP_ARRAY_EXT} {}
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
         * @see @ref CubeMapTextureArray(), @ref wrap()
         */
        explicit CubeMapTextureArray(NoCreateT) noexcept:
            #ifndef MAGNUM_TARGET_GLES
            AbstractTexture{NoCreate, GL_TEXTURE_CUBE_MAP_ARRAY} {}
            #else
            AbstractTexture{NoCreate, GL_TEXTURE_CUBE_MAP_ARRAY_EXT} {}
            #endif

        /**
         * @brief Bind level of given texture layer to given image unit
         * @param imageUnit Image unit
         * @param level     Texture level
         * @param layer     Texture layer
         * @param access    Image access
         * @param format    Image format
         *
         * Layer is equivalent to layer * 6 + number of texture face, i.e. +X
         * is `0` and so on, in order of (+X, -X, +Y, -Y, +Z, -Z).
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bindImages(Int, std::initializer_list<AbstractTexture*>),
         *      @ref bindImageLayered(), @ref unbindImage(), @ref unbindImages(),
         *      @ref AbstractShaderProgram::maxImageUnits(),
         *      @fn_gl{BindImageTexture}
         * @requires_gl42 Extension @extension{ARB,shader_image_load_store}
         * @requires_gles31 Shader image load/store is not available in OpenGL
         *      ES 3.0 and older.
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
         *      @fn_gl{BindImageTexture}
         * @requires_gl42 Extension @extension{ARB,shader_image_load_store}
         * @requires_gles31 Shader image load/store is not available in OpenGL
         *      ES 3.0 and older.
         */
        void bindImageLayered(Int imageUnit, Int level, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, level, true, 0, access, format);
        }

        /**
         * @copybrief Texture::setBaseLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBaseLevel() for more information.
         */
        CubeMapTextureArray& setBaseLevel(Int level) {
            AbstractTexture::setBaseLevel(level);
            return *this;
        }

        /**
         * @copybrief Texture::setMaxLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLevel() for more information.
         */
        CubeMapTextureArray& setMaxLevel(Int level) {
            AbstractTexture::setMaxLevel(level);
            return *this;
        }

        /**
         * @copybrief Texture::setMinificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinificationFilter() for more information.
         */
        CubeMapTextureArray& setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /**
         * @copybrief Texture::setMagnificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMagnificationFilter() for more information.
         */
        CubeMapTextureArray& setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        /**
         * @copybrief Texture::setMinLod()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinLod() for more information.
         */
        CubeMapTextureArray& setMinLod(Float lod) {
            AbstractTexture::setMinLod(lod);
            return *this;
        }

        /**
         * @copybrief Texture::setMaxLod()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLod() for more information.
         */
        CubeMapTextureArray& setMaxLod(Float lod) {
            AbstractTexture::setMaxLod(lod);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::setLodBias()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setLodBias() for more information.
         * @requires_gl Texture LOD bias can be specified only directly in
         *      fragment shader in OpenGL ES.
         */
        CubeMapTextureArray& setLodBias(Float bias) {
            AbstractTexture::setLodBias(bias);
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setWrapping()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setWrapping() for more information.
         */
        CubeMapTextureArray& setWrapping(const Array2D<Sampler::Wrapping>& wrapping) {
            DataHelper<2>::setWrapping(*this, wrapping);
            return *this;
        }

        /**
         * @copybrief Texture::setBorderColor(const Color4&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Color4&) for more
         * information.
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,texture_border_clamp} or
         *      @extension{NV,texture_border_clamp}
         */
        CubeMapTextureArray& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /**
         * @copybrief Texture::setBorderColor(const Vector4ui&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Vector4ui&) for more
         * information.
         * @requires_gl30 Extension @extension{EXT,texture_integer}
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,texture_border_clamp}
         */
        CubeMapTextureArray& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @overload
         * @requires_gl30 Extension @extension{EXT,texture_integer}
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,texture_border_clamp}
         */
        CubeMapTextureArray& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /**
         * @copybrief Texture::setMaxAnisotropy()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxAnisotropy() for more information.
         */
        CubeMapTextureArray& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        /**
         * @copybrief Texture::setSRGBDecode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSRGBDecode() for more information.
         * @requires_extension Extension @extension{EXT,texture_sRGB_decode}
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension2{EXT,texture_sRGB_decode,texture_sRGB_decode}
         */
        CubeMapTextureArray& setSRGBDecode(bool decode) {
            AbstractTexture::setSRGBDecode(decode);
            return *this;
        }

        /**
         * @copybrief Texture::setSwizzle()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSwizzle() for more information.
         * @requires_gl33 Extension @extension{ARB,texture_swizzle}
         */
        template<char r, char g, char b, char a> CubeMapTextureArray& setSwizzle() {
            AbstractTexture::setSwizzle<r, g, b, a>();
            return *this;
        }

        /**
         * @copybrief Texture::setCompareMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareMode() for more information.
         */
        CubeMapTextureArray& setCompareMode(Sampler::CompareMode mode) {
            AbstractTexture::setCompareMode(mode);
            return *this;
        }

        /**
         * @copybrief Texture::setCompareFunction()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareFunction() for more information.
         */
        CubeMapTextureArray& setCompareFunction(Sampler::CompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }

        /**
         * @copybrief Texture::setDepthStencilMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setDepthStencilMode() for more information.
         * @requires_gl43 Extension @extension{ARB,stencil_texturing}
         */
        CubeMapTextureArray& setDepthStencilMode(Sampler::DepthStencilMode mode) {
            AbstractTexture::setDepthStencilMode(mode);
            return *this;
        }

        /**
         * @copybrief Texture::setStorage()
         * @return Reference to self (for method chaining)
         *
         * Z coordinate of @p size must be multiple of 6.
         *
         * See @ref Texture::setStorage() for more information.
         * @see @ref maxSize()
         */
        CubeMapTextureArray& setStorage(Int levels, TextureFormat internalFormat, const Vector3i& size) {
            DataHelper<3>::setStorage(*this, levels, internalFormat, size);
            return *this;
        }

        /**
         * @copybrief Texture::imageSize()
         *
         * See @ref Texture::imageSize() for more information.
         */
        Vector3i imageSize(Int level) {
            return DataHelper<3>::imageSize(*this, level);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::image(Int, Image&)
         *
         * See @ref Texture::image(Int, Image&) for more information.
         * @requires_gl Texture image queries are not available in OpenGL ES.
         *      See @ref Framebuffer::read() for possible workaround.
         */
        void image(Int level, Image3D& image) {
            AbstractTexture::image<3>(level, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * Image3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});
         * @endcode
         */
        Image3D image(Int level, Image3D&& image);

        /**
         * @copybrief Texture::image(Int, BufferImage&, BufferUsage)
         *
         * See @ref Texture::image(Int, BufferImage&, BufferUsage) for more
         * information.
         * @requires_gl Texture image queries are not available in OpenGL ES.
         *      See @ref Framebuffer::read() for possible workaround.
         */
        void image(Int level, BufferImage3D& image, BufferUsage usage) {
            AbstractTexture::image<3>(level, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * BufferImage3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
         * @endcode
         */
        BufferImage3D image(Int level, BufferImage3D&& image, BufferUsage usage);

        /**
         * @copybrief Texture::compressedImage(Int, CompressedImage&)
         *
         * See @ref Texture::compressedImage(Int, CompressedImage&) for more
         *      information.
         * @requires_gl Texture image queries are not available in OpenGL ES.
         *      See @ref Framebuffer::read() for possible workaround.
         */
        void compressedImage(Int level, CompressedImage3D& image) {
            AbstractTexture::compressedImage<3>(level, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedImage3D image = texture.compressedImage(0, {});
         * @endcode
         */
        CompressedImage3D compressedImage(Int level, CompressedImage3D&& image);

        /**
         * @copybrief Texture::compressedImage(Int, CompressedBufferImage&, BufferUsage)
         *
         * See @ref Texture::compressedImage(Int, CompressedBufferImage&, BufferUsage)
         * for more information.
         * @requires_gl Texture image queries are not available in OpenGL ES.
         *      See @ref Framebuffer::read() for possible workaround.
         */
        void compressedImage(Int level, CompressedBufferImage3D& image, BufferUsage usage) {
            AbstractTexture::compressedImage<3>(level, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
         * @endcode
         */
        CompressedBufferImage3D compressedImage(Int level, CompressedBufferImage3D&& image, BufferUsage usage);

        /**
         * @copybrief Texture::subImage(Int, const RangeTypeFor<dimensions, Int>&, Image&)
         *
         * See @ref Texture::subImage(Int, const RangeTypeFor<dimensions, Int>&, Image&)
         * for more information.
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES.
         *      See @ref Framebuffer::read() for possible workaround.
         */
        void subImage(Int level, const Range3Di& range, Image3D& image) {
            AbstractTexture::subImage<3>(level, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * Image3D image = texture.subImage(0, range, {PixelFormat::RGBA, PixelType::UnsignedByte});
         * @endcode
         */
        Image3D subImage(Int level, const Range3Di& range, Image3D&& image);

        /**
         * @copybrief Texture::subImage(Int, const RangeTypeFor<dimensions, Int>&, BufferImage&, BufferUsage)
         *
         * See @ref Texture::subImage(Int, const RangeTypeFor<dimensions, Int>&, BufferImage&, BufferUsage)
         * for more information.
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES.
         *      See @ref Framebuffer::read() for possible workaround.
         */
        void subImage(Int level, const Range3Di& range, BufferImage3D& image, BufferUsage usage) {
            AbstractTexture::subImage<3>(level, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * BufferImage3D image = texture.subImage(0, range, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
         * @endcode
         */
        BufferImage3D subImage(Int level, const Range3Di& range, BufferImage3D&& image, BufferUsage usage);

        /**
         * @copybrief Texture::compressedSubImage(Int, const RangeTypeFor<dimensions, Int>&, CompressedImage&)
         *
         * See @ref Texture::compressedSubImage(Int, const RangeTypeFor<dimensions, Int>&, CompressedImage&)
         * for more information.
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl43 Extension @extension{ARB,internalformat_query2} if
         *      @ref CompressedPixelStorage::compressedBlockSize() and
         *      @ref CompressedPixelStorage::compressedBlockDataSize() are not
         *      set to non-zero values
         * @requires_gl Texture image queries are not available in OpenGL ES.
         *      See @ref Framebuffer::read() for possible workaround.
         */
        void compressedSubImage(Int level, const Range3Di& range, CompressedImage3D& image) {
            AbstractTexture::compressedSubImage<3>(level, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedImage3D image = texture.compressedSubImage(0, range, {});
         * @endcode
         */
        CompressedImage3D compressedSubImage(Int level, const Range3Di& range, CompressedImage3D&& image);

        /**
         * @copybrief Texture::compressedSubImage(Int, const RangeTypeFor<dimensions, Int>&, CompressedBufferImage&, BufferUsage)
         *
         * See @ref Texture::compressedSubImage(Int, const RangeTypeFor<dimensions, Int>&, CompressedBufferImage&, BufferUsage)
         * for more information.
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl43 Extension @extension{ARB,internalformat_query2} if
         *      @ref CompressedPixelStorage::compressedBlockSize() and
         *      @ref CompressedPixelStorage::compressedBlockDataSize() are not
         *      set to non-zero values
         * @requires_gl Texture image queries are not available in OpenGL ES.
         *      See @ref Framebuffer::read() for possible workaround.
         */
        void compressedSubImage(Int level, const Range3Di& range, CompressedBufferImage3D& image, BufferUsage usage) {
            AbstractTexture::compressedSubImage<3>(level, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedBufferImage3D image = texture.compressedSubImage(0, range, {}, BufferUsage::StaticRead);
         * @endcode
         */
        CompressedBufferImage3D compressedSubImage(Int level, const Range3Di& range, CompressedBufferImage3D&& image, BufferUsage usage);
        #endif

        /**
         * @copybrief Texture::setImage()
         * @return Reference to self (for method chaining)
         *
         * Sets texture image data from three-dimensional image for all cube
         * faces for all layers. Each group of 6 2D images is one cube map
         * layer, thus Z coordinate of @p image size must be multiple of 6. The
         * images are in order of (+X, -X, +Y, -Y, +Z, -Z).
         *
         * See @ref Texture::setImage() for more information.
         * @see @ref maxSize()
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        CubeMapTextureArray& setImage(Int level, TextureFormat internalFormat, const ImageView3D& image) {
            DataHelper<3>::setImage(*this, level, internalFormat, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        CubeMapTextureArray& setImage(Int level, TextureFormat internalFormat, BufferImage3D& image) {
            DataHelper<3>::setImage(*this, level, internalFormat, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        CubeMapTextureArray& setImage(Int level, TextureFormat internalFormat, BufferImage3D&& image) {
            return setImage(level, internalFormat, image);
        }

        /**
         * @copybrief Texture::setCompressedImage()
         * @return Reference to self (for method chaining)
         *
         * Sets texture image data from three-dimensional image for all cube
         * faces for all layers. Each group of 6 2D images is one cube map
         * layer, thus Z coordinate of @p image size must be multiple of 6. The
         * images are in order of (+X, -X, +Y, -Y, +Z, -Z).
         *
         * See @ref Texture::setCompressedImage() for more information.
         * @see @ref maxSize()
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        CubeMapTextureArray& setCompressedImage(Int level, const CompressedImageView3D& image) {
            DataHelper<3>::setCompressedImage(*this, level, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        CubeMapTextureArray& setCompressedImage(Int level, CompressedBufferImage3D& image) {
            DataHelper<3>::setCompressedImage(*this, level, image);
            return *this;
        }

        /** @overload
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        CubeMapTextureArray& setCompressedImage(Int level, CompressedBufferImage3D&& image) {
            return setCompressedImage(level, image);
        }

        /**
         * @copybrief Texture::setSubImage()
         * @return Reference to self (for method chaining)
         *
         * Z coordinate is equivalent to layer * 6 + number of texture face,
         * i.e. +X is `0` and so on, in order of (+X, -X, +Y, -Y, +Z, -Z).
         *
         * See @ref Texture::setSubImage() for more information.
         */
        CubeMapTextureArray& setSubImage(Int level, const Vector3i& offset, const ImageView3D& image) {
            DataHelper<3>::setSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload */
        CubeMapTextureArray& setSubImage(Int level, const Vector3i& offset, BufferImage3D& image) {
            DataHelper<3>::setSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload */
        CubeMapTextureArray& setSubImage(Int level, const Vector3i& offset, BufferImage3D&& image) {
            return setSubImage(level, offset, image);
        }

        /**
         * @copybrief Texture::setCompressedSubImage()
         * @return Reference to self (for method chaining)
         *
         * Z coordinate is equivalent to layer * 6 + number of texture face,
         * i.e. +X is `0` and so on, in order of (+X, -X, +Y, -Y, +Z, -Z).
         *
         * See @ref Texture::setCompressedSubImage() for more information.
         */
        CubeMapTextureArray& setCompressedSubImage(Int level, const Vector3i& offset, const CompressedImageView3D& image) {
            DataHelper<3>::setCompressedSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload */
        CubeMapTextureArray& setCompressedSubImage(Int level, const Vector3i& offset, CompressedBufferImage3D& image) {
            DataHelper<3>::setCompressedSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload */
        CubeMapTextureArray& setCompressedSubImage(Int level, const Vector3i& offset, CompressedBufferImage3D&& image) {
            return setCompressedSubImage(level, offset, image);
        }

        /**
         * @copybrief Texture::generateMipmap()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::generateMipmap() for more information.
         * @requires_gl30 Extension @extension{ARB,framebuffer_object}
         */
        CubeMapTextureArray& generateMipmap() {
            AbstractTexture::generateMipmap();
            return *this;
        }

        /**
         * @copybrief Texture::invalidateImage()
         *
         * See @ref Texture::invalidateImage() for more information.
         */
        void invalidateImage(Int level) { AbstractTexture::invalidateImage(level); }

        /**
         * @copybrief Texture::invalidateSubImage()
         *
         * Z coordinate is equivalent to layer * 6 + number of texture face,
         * i.e. +X is `0` and so on, in order of (+X, -X, +Y, -Y, +Z, -Z).
         *
         * See @ref Texture::invalidateSubImage() for more information.
         */
        void invalidateSubImage(Int level, const Vector3i& offset, const Vector3i& size) {
            DataHelper<3>::invalidateSubImage(*this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        CubeMapTextureArray& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        template<std::size_t size> CubeMapTextureArray& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit CubeMapTextureArray(GLuint id, ObjectFlags flags) noexcept: AbstractTexture{id,
            #ifndef MAGNUM_TARGET_GLES
            GL_TEXTURE_CUBE_MAP_ARRAY,
            #else
            GL_TEXTURE_CUBE_MAP_ARRAY_EXT,
            #endif
            flags} {}
};

}
#else
#error this header is not available in OpenGL ES 2.0 and WebGL build
#endif

#endif
