#ifndef Magnum_CubeMapTexture_h
#define Magnum_CubeMapTexture_h
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

/** @file
 * @brief Class @ref Magnum::CubeMapTexture, enum @ref Magnum::CubeMapCoordinate
 */

#include "Magnum/AbstractTexture.h"
#include "Magnum/Array.h"
#include "Magnum/Math/Vector2.h"

namespace Magnum {

/**
@brief Cube map coordinate

@see @ref CubeMapTexture
*/
enum class CubeMapCoordinate: GLenum {
    PositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,     /**< +X cube side */
    NegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,     /**< -X cube side */
    PositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,     /**< +Y cube side */
    NegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,     /**< -Y cube side */
    PositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,     /**< +Z cube side */
    NegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z      /**< -Z cube side */
};

/**
@brief Cube map texture

Texture used mainly for environment maps. It consists of 6 square textures
generating 6 faces of the cube as following. Note that all images must be
turned upside down (+Y is top):

              +----+
              | -Y |
    +----+----+----+----+
    | -Z | -X | +Z | +X |
    +----+----+----+----+
              | +Y |
              +----+

## Basic usage

See @ref Texture documentation for introduction.

Common usage is to fully configure all texture parameters and then set the
data from e.g. set of Image objects:
@code
Image2D positiveX(PixelFormat::RGBA, PixelType::UnsignedByte, {256, 256}, data);
// ...

CubeMapTexture texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    // ...
    .setStorage(Math::log2(256)+1, TextureFormat::RGBA8, {256, 256})
    .setSubImage(CubeMapCoordinate::PositiveX, 0, {}, positiveX)
    .setSubImage(CubeMapCoordinate::NegativeX, 0, {}, negativeX)
    // ...
@endcode

In shader, the texture is used via `samplerCube`, `samplerCubeShadow`,
`isamplerCube` or `usamplerCube`. Unlike in classic textures, coordinates for
cube map textures is signed three-part vector from the center of the cube,
which intersects one of the six sides of the cube map. See
@ref AbstractShaderProgram for more information about usage in shaders.

@see @ref Renderer::Feature::SeamlessCubeMapTexture, @ref CubeMapTextureArray,
    @ref Texture, @ref TextureArray, @ref RectangleTexture, @ref BufferTexture,
    @ref MultisampleTexture
*/
class MAGNUM_EXPORT CubeMapTexture: public AbstractTexture {
    friend Implementation::TextureState;

    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief CubeMapCoordinate
         * @deprecated Use @ref CubeMapCoordinate instead.
         */
        typedef CORRADE_DEPRECATED("use CubeMapCoordinate instead") CubeMapCoordinate Coordinate;
        #endif

        /**
         * @brief Max supported size of one side of cube map texture
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @fn_gl{Get} with @def_gl{MAX_CUBE_MAP_TEXTURE_SIZE}
         */
        static Vector2i maxSize();

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
            return DataHelper<2>::compressedBlockSize(GL_TEXTURE_CUBE_MAP, format);
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
            return AbstractTexture::compressedBlockDataSize(GL_TEXTURE_CUBE_MAP, format);
        }
        #endif

        /**
         * @brief Wrap existing OpenGL cube map texture object
         * @param id            OpenGL cube map texture ID
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL texture object
         * with target @def_gl{TEXTURE_CUBE_MAP}. Unlike texture created using
         * constructor, the OpenGL object is by default not deleted on
         * destruction, use @p flags for different behavior.
         * @see @ref release()
         */
        static CubeMapTexture wrap(GLuint id, ObjectFlags flags = {}) {
            return CubeMapTexture{id, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref CubeMapTexture(NoCreateT), @ref wrap(), @fn_gl{CreateTextures}
         *      with @def_gl{TEXTURE_CUBE_MAP}, eventually @fn_gl{GenTextures}
         */
        explicit CubeMapTexture(): AbstractTexture(GL_TEXTURE_CUBE_MAP) {}

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * @see @ref CubeMapTexture(), @ref wrap()
         */
        explicit CubeMapTexture(NoCreateT) noexcept: AbstractTexture{NoCreate, GL_TEXTURE_CUBE_MAP} {}

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Bind level of given cube map texture coordinate to given image unit
         * @param imageUnit     Image unit
         * @param level         Texture level
         * @param coordinate    Cube map coordinate
         * @param access        Image access
         * @param format        Image format
         *
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
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        void bindImage(Int imageUnit, Int level, CubeMapCoordinate coordinate, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, level, false, GLenum(coordinate) - GL_TEXTURE_CUBE_MAP_POSITIVE_X, access, format);
        }

        /**
         * @brief Bind level of layered cube map texture to given image unit
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
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        void bindImageLayered(Int imageUnit, Int level, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, level, true, 0, access, format);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copybrief Texture::setBaseLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBaseLevel() for more information.
         * @requires_gles30 Base level is always `0` in OpenGL ES 2.0.
         * @requires_webgl20 Base level is always `0` in WebGL 1.0.
         */
        CubeMapTexture& setBaseLevel(Int level) {
            AbstractTexture::setBaseLevel(level);
            return *this;
        }
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @copybrief Texture::setMaxLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLevel() for more information.
         * @requires_gles30 Extension @extension{APPLE,texture_max_level},
         *      otherwise the max level is always set to largest possible value
         *      in OpenGL ES 2.0.
         * @requires_webgl20 Always set to largest possible value in WebGL 1.0.
         */
        CubeMapTexture& setMaxLevel(Int level) {
            AbstractTexture::setMaxLevel(level);
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setMinificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinificationFilter() for more information.
         */
        CubeMapTexture& setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /**
         * @copybrief Texture::setMagnificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMagnificationFilter() for more information.
         */
        CubeMapTexture& setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copybrief Texture::setMinLod()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinLod() for more information.
         * @requires_gles30 Texture LOD parameters are not available in OpenGL
         *      ES 2.0.
         * @requires_webgl20 Texture LOD parameters are not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setMinLod(Float lod) {
            AbstractTexture::setMinLod(lod);
            return *this;
        }

        /**
         * @copybrief Texture::setMaxLod()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLod() for more information.
         * @requires_gles30 Texture LOD parameters are not available in OpenGL
         *      ES 2.0.
         * @requires_webgl20 Texture LOD parameters are not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setMaxLod(Float lod) {
            AbstractTexture::setMaxLod(lod);
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::setLodBias()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setLodBias() for more information.
         * @requires_gl Texture LOD bias can be specified only directly in
         *      fragment shader in OpenGL ES and WebGL.
         */
        CubeMapTexture& setLodBias(Float bias) {
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
        CubeMapTexture& setWrapping(const Array2D<Sampler::Wrapping>& wrapping) {
            DataHelper<2>::setWrapping(*this, wrapping);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @copybrief Texture::setBorderColor(const Color4&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Color4&) for more
         * information.
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,texture_border_clamp} or
         *      @extension{NV,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        CubeMapTexture& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copybrief Texture::setBorderColor(const Vector4ui&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Vector4ui&) for more
         * information.
         * @requires_gl30 Extension @extension{EXT,texture_integer}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_es_extension Extension
         *      @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        CubeMapTexture& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @overload
         * @requires_gl30 Extension @extension{EXT,texture_integer}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_es_extension Extension
         *      @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        CubeMapTexture& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        #endif
        #endif

        /**
         * @copybrief Texture::setMaxAnisotropy()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxAnisotropy() for more information.
         */
        CubeMapTexture& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @copybrief Texture::setSRGBDecode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSRGBDecode() for more information.
         * @requires_extension Extension @extension{EXT,texture_sRGB_decode}
         * @requires_es_extension OpenGL ES 3.0 or extension
         *      @extension{EXT,sRGB} and
         *      @extension{ANDROID,extension_pack_es31a}/
         *      @extension2{EXT,texture_sRGB_decode,texture_sRGB_decode}
         * @requires_gles SRGB decode is not available in WebGL.
         */
        CubeMapTexture& setSRGBDecode(bool decode) {
            AbstractTexture::setSRGBDecode(decode);
            return *this;
        }
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @copybrief Texture::setSwizzle()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSwizzle() for more information.
         * @requires_gl33 Extension @extension{ARB,texture_swizzle}
         * @requires_gles30 Texture swizzle is not available in OpenGL ES 2.0.
         * @requires_gles Texture swizzle is not available in WebGL.
         */
        template<char r, char g, char b, char a> CubeMapTexture& setSwizzle() {
            AbstractTexture::setSwizzle<r, g, b, a>();
            return *this;
        }
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @copybrief Texture::setCompareMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareMode() for more information.
         * @requires_gles30 Extension @extension{EXT,shadow_samplers} and
         *      @extension{NV,shadow_samplers_cube} in OpenGL ES 2.0.
         * @requires_webgl20 Depth texture comparison is not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setCompareMode(Sampler::CompareMode mode) {
            AbstractTexture::setCompareMode(mode);
            return *this;
        }

        /**
         * @copybrief Texture::setCompareFunction()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareFunction() for more information.
         * @requires_gles30 Extension @extension{EXT,shadow_samplers} and
         *      @extension{NV,shadow_samplers_cube} in OpenGL ES 2.0.
         * @requires_webgl20 Depth texture comparison is not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setCompareFunction(Sampler::CompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @copybrief Texture::setDepthStencilMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setDepthStencilMode() for more information.
         * @requires_gl43 Extension @extension{ARB,stencil_texturing}
         * @requires_gles31 Stencil texturing is not available in OpenGL ES 3.0
         *      and older.
         * @requires_gles Stencil texturing is not available in WebGL.
         */
        CubeMapTexture& setDepthStencilMode(Sampler::DepthStencilMode mode) {
            AbstractTexture::setDepthStencilMode(mode);
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setStorage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setStorage() for more information.
         * @see @ref maxSize()
         */
        CubeMapTexture& setStorage(Int levels, TextureFormat internalFormat, const Vector2i& size) {
            DataHelper<2>::setStorage(*this, levels, internalFormat, size);
            return *this;
        }

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @copybrief Texture::imageSize()
         *
         * If @extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, it is assumed that faces have the same size and just the
         * size of @ref Coordinate::PositiveX face is queried. See
         * @ref Texture::imageSize() for more information.
         * @requires_gles31 Texture image size queries are not available in
         *      OpenGL ES 3.0 and older.
         * @requires_gles Texture image size queries are not available in
         *      WebGL.
         */
        Vector2i imageSize(Int level);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief imageSize()
         * @deprecated Use @ref imageSize(Int) instead.
         */
        CORRADE_DEPRECATED("use imageSize(Int) instead") Vector2i imageSize(CubeMapCoordinate, Int level) {
            return imageSize(level);
        }
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Read given mip level of texture to image
         *
         * Image parameters like format and type of pixel data are taken from
         * given image, image size is taken from the texture using
         * @ref imageSize(). The storage is not reallocated if it is large
         * enough to contain the new data.
         *
         * The operation is protected from buffer overflow.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter} with
         *      @def_gl{TEXTURE_WIDTH}, @def_gl{TEXTURE_HEIGHT}, then
         *      @fn_gl{PixelStore}, then @fn_gl2{GetTextureImage,GetTexImage}
         * @requires_gl45 Extension @extension{ARB,direct_state_access}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void image(Int level, Image3D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * Image3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});
         * @endcode
         */
        Image3D image(Int level, Image3D&& image);

        /**
         * @brief Read given mip level of texture to buffer image
         *
         * See @ref image(Int, Image3D&) for more information. The storage is
         * not reallocated if it is large enough to contain the new data, which
         * means that @p usage might get ignored.
         * @requires_gl45 Extension @extension{ARB,direct_state_access}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void image(Int level, BufferImage3D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * BufferImage3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
         * @endcode
         */
        BufferImage3D image(Int level, BufferImage3D&& image, BufferUsage usage);

        /**
         * @brief Read given mip level of compressed texture to image
         *
         * Compression format and data size are taken from the texture, image
         * size is taken using @ref imageSize(). The storage is not reallocated
         * if it is large enough to contain the new data.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter} with
         *      @def_gl{TEXTURE_COMPRESSED_IMAGE_SIZE},
         *      @def_gl{TEXTURE_INTERNAL_FORMAT}, @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, then
         *      @fn_gl2{GetCompressedTextureImage,GetCompressedTexImage}
         * @requires_gl45 Extension @extension{ARB,direct_state_access}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void compressedImage(Int level, CompressedImage3D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedImage3D image = texture.compressedImage(0, {});
         * @endcode
         */
        CompressedImage3D compressedImage(Int level, CompressedImage3D&& image);

        /**
         * @brief Read given mip level of compressed texture to buffer image
         *
         * See @ref compressedImage(Int, CompressedImage3D&) for more
         * information. The storage is not reallocated if it is large enough to
         * contain the new data, which means that @p usage might get ignored.
         * @requires_gl45 Extension @extension{ARB,direct_state_access}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void compressedImage(Int level, CompressedBufferImage3D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
         * @endcode
         */
        CompressedBufferImage3D compressedImage(Int level, CompressedBufferImage3D&& image, BufferUsage usage);

        /**
         * @brief Read given mip level and coordinate of texture to image
         *
         * Image parameters like format and type of pixel data are taken from
         * given image, image size is taken from the texture using
         * @ref imageSize(). The storage is not reallocated if it is large
         * enough to contain the new data.
         *
         * If neither @extension{ARB,get_texture_sub_image} (part of OpenGL
         * 4.5) nor @extension{EXT,direct_state_access} is available, the
         * texture is bound before the operation (if not already). If either
         * @extension{ARB,get_texture_sub_image} or @extension{ARB,robustness}
         * is available, the operation is protected from buffer overflow.
         * However, if @extension{ARB,get_texture_sub_image} is not available
         * and both @extension{EXT,direct_state_access} and
         * @extension{ARB,robustness} are available, the robust operation is
         * preferred over DSA.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} with @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, then @fn_gl{PixelStore}, then
         *      @fn_gl{GetTextureSubImage}, @fn_gl_extension{GetnTexImage,ARB,robustness},
         *      @fn_gl_extension{GetTextureImage,EXT,direct_state_access},
         *      eventually @fn_gl{GetTexImage}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void image(CubeMapCoordinate coordinate, Int level, Image2D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * Image2D image = texture.image(CubeMapCoordinate::PositiveX, 0, {PixelFormat::RGBA, PixelType::UnsignedByte});
         * @endcode
         */
        Image2D image(CubeMapCoordinate coordinate, Int level, Image2D&& image);

        /**
         * @brief Read given mip level and coordinate of texture to buffer image
         *
         * See @ref image(Coordinate, Int, Image2D&) for more information. The
         * storage is not reallocated if it is large enough to contain the new
         * data, which means that @p usage might get ignored.
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void image(CubeMapCoordinate coordinate, Int level, BufferImage2D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * BufferImage2D image = texture.image(CubeMapCoordinate::PositiveX, 0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
         * @endcode
         */
        BufferImage2D image(CubeMapCoordinate coordinate, Int level, BufferImage2D&& image, BufferUsage usage);

        /**
         * @brief Read given mip level and coordinate of compressed texture to image
         *
         * Compression format and data size are taken from the texture, image
         * size is taken using @ref imageSize(). The storage is not reallocated
         * if it is large enough to contain the new data.
         *
         * If neither @extension{ARB,get_texture_sub_image} (part of OpenGL
         * 4.5) nor @extension{EXT,direct_state_access} is available, the
         * texture is bound before the operation (if not already). If either
         * @extension{ARB,get_texture_sub_image} or @extension{ARB,robustness}
         * is available, the operation is protected from buffer overflow.
         * However, if @extension{ARB,get_texture_sub_image} is not available
         * and both @extension{EXT,direct_state_access} and
         * @extension{ARB,robustness} are available, the robust operation is
         * preferred over DSA.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access},
         *      eventually @fn_gl{GetTexLevelParameter} with
         *      @def_gl{TEXTURE_COMPRESSED_IMAGE_SIZE},
         *      @def_gl{TEXTURE_INTERNAL_FORMAT}, @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, then @fn_gl{PixelStore}, then
         *      @fn_gl{GetCompressedTextureSubImage},
         *      @fn_gl_extension{GetnCompressedTexImage,ARB,robustness},
         *      @fn_gl_extension{GetCompressedTextureImage,EXT,direct_state_access},
         *      eventually @fn_gl{GetCompressedTexImage}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void compressedImage(CubeMapCoordinate coordinate, Int level, CompressedImage2D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0, {});
         * @endcode
         */
        CompressedImage2D compressedImage(CubeMapCoordinate coordinate, Int level, CompressedImage2D&& image);

        /**
         * @brief Read given mip level and coordinate of compressed texture to buffer image
         *
         * See @ref compressedImage(Coordinate, Int, CompressedImage2D&) for
         * more information. The storage is not reallocated if it is large
         * enough to contain the new data, which means that @p usage might get
         * ignored.
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void compressedImage(CubeMapCoordinate coordinate, Int level, CompressedBufferImage2D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedBufferImage2D image = texture.compressedImage(CubeMapCoordinate::PositiveX, 0, {}, BufferUsage::StaticRead);
         * @endcode
         */
        CompressedBufferImage2D compressedImage(CubeMapCoordinate coordinate, Int level, CompressedBufferImage2D&& image, BufferUsage usage);

        /**
         * @copybrief Texture::subImage(Int, const RangeTypeFor<dimensions, Int>&, Image&)
         *
         * See @ref Texture::subImage(Int, const RangeTypeFor<dimensions, Int>&, Image&)
         * for more information.
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
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
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
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
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
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
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
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
         * See @ref Texture::setImage() for more information.
         * @see @ref maxSize()
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        CubeMapTexture& setImage(CubeMapCoordinate coordinate, Int level, TextureFormat internalFormat, const ImageView2D& image) {
            DataHelper<2>::setImage(*this, GLenum(coordinate), level, internalFormat, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        CubeMapTexture& setImage(CubeMapCoordinate coordinate, Int level, TextureFormat internalFormat, BufferImage2D& image) {
            DataHelper<2>::setImage(*this, GLenum(coordinate), level, internalFormat, image);
            return *this;
        }

        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        CubeMapTexture& setImage(CubeMapCoordinate coordinate, Int level, TextureFormat internalFormat, BufferImage2D&& image) {
            return setImage(coordinate, level, internalFormat, image);
        }
        #endif

        /**
         * @copybrief Texture::setCompressedImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompressedImage() for more information.
         * @see @ref maxSize()
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        CubeMapTexture& setCompressedImage(CubeMapCoordinate coordinate, Int level, const CompressedImageView2D& image) {
            DataHelper<2>::setCompressedImage(*this, GLenum(coordinate), level, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        CubeMapTexture& setCompressedImage(CubeMapCoordinate coordinate, Int level, CompressedBufferImage2D& image) {
            DataHelper<2>::setCompressedImage(*this, GLenum(coordinate), level, image);
            return *this;
        }

        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        CubeMapTexture& setCompressedImage(CubeMapCoordinate coordinate, Int level, CompressedBufferImage2D&& image) {
            return setCompressedImage(coordinate, level, image);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set image subdata
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             @ref Image3D, @ref ImageView3D or
         *      @ref Trade::ImageData3D
         * @return Reference to self (for method chaining)
         *
         * @see @ref setStorage(), @fn_gl2{TextureSubImage3D,TexSubImage3D}
         * @requires_gl45 Extension @extension{ARB,direct_state_access}
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setSubImage(Int level, const Vector3i& offset, const ImageView3D& image);

        /** @overload
         * @requires_gl45 Extension @extension{ARB,direct_state_access}
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setSubImage(Int level, const Vector3i& offset, BufferImage3D& image);

        /** @overload
         * @requires_gl45 Extension @extension{ARB,direct_state_access}
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setSubImage(Int level, const Vector3i& offset, BufferImage3D&& image) {
            return setSubImage(level, offset, image);
        }

        /**
         * @brief Set compressed image subdata
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             @ref CompressedImage3D, @ref CompressedImageView3D
         *      or compressed @ref Trade::ImageData3D
         * @return Reference to self (for method chaining)
         *
         * @see @ref setStorage(), @fn_gl2{CompressedTextureSubImage3D,CompressedTexSubImage3D}
         * @requires_gl45 Extension @extension{ARB,direct_state_access}
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setCompressedSubImage(Int level, const Vector3i& offset, const CompressedImageView3D& image);

        /** @overload
         * @requires_gl45 Extension @extension{ARB,direct_state_access}
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setCompressedSubImage(Int level, const Vector3i& offset, CompressedBufferImage3D& image);

        /** @overload
         * @requires_gl45 Extension @extension{ARB,direct_state_access}
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setCompressedSubImage(Int level, const Vector3i& offset, CompressedBufferImage3D&& image) {
            return setCompressedSubImage(level, offset, image);
        }
        #endif

        /**
         * @copybrief Texture::setSubImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSubImage() for more information.
         */
        CubeMapTexture& setSubImage(CubeMapCoordinate coordinate, Int level, const Vector2i& offset, const ImageView2D& image);

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setSubImage(CubeMapCoordinate coordinate, Int level, const Vector2i& offset, BufferImage2D& image);

        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setSubImage(CubeMapCoordinate coordinate, Int level, const Vector2i& offset, BufferImage2D&& image) {
            return setSubImage(coordinate, level, offset, image);
        }
        #endif

        /**
         * @copybrief Texture::setCompressedSubImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompressedSubImage() for more information.
         */
        CubeMapTexture& setCompressedSubImage(CubeMapCoordinate coordinate, Int level, const Vector2i& offset, const CompressedImageView2D& image);

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setCompressedSubImage(CubeMapCoordinate coordinate, Int level, const Vector2i& offset, CompressedBufferImage2D& image);

        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setCompressedSubImage(CubeMapCoordinate coordinate, Int level, const Vector2i& offset, CompressedBufferImage2D&& image) {
            return setCompressedSubImage(coordinate, level, offset, image);
        }
        #endif

        /**
         * @copybrief Texture::generateMipmap()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::generateMipmap() for more information.
         * @requires_gl30 Extension @extension{ARB,framebuffer_object}
         */
        CubeMapTexture& generateMipmap() {
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
         * Z coordinate is equivalent to number of texture face, i.e.
         * @ref Coordinate::PositiveX is `0` and so on, in the same order as in
         * the enum.
         *
         * See @ref Texture::invalidateSubImage() for more information.
         */
        void invalidateSubImage(Int level, const Vector3i& offset, const Vector3i& size) {
            DataHelper<3>::invalidateSubImage(*this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(MAGNUM_TARGET_WEBGL)
        CubeMapTexture& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        template<std::size_t size> CubeMapTexture& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit CubeMapTexture(GLuint id, ObjectFlags flags) noexcept: AbstractTexture{id, GL_TEXTURE_CUBE_MAP, flags} {}

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_LOCAL getLevelParameterImplementationDefault(GLint level, GLenum parameter, GLint* values);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL getLevelParameterImplementationDSA(GLint level, GLenum parameter, GLint* values);
        void MAGNUM_LOCAL getLevelParameterImplementationDSAEXT(GLint level, GLenum parameter, GLint* values);
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES
        GLint MAGNUM_LOCAL getLevelCompressedImageSizeImplementationDefault(GLint level);
        GLint MAGNUM_LOCAL getLevelCompressedImageSizeImplementationDefaultImmutableWorkaround(GLint level);
        GLint MAGNUM_LOCAL getLevelCompressedImageSizeImplementationDSA(GLint level);
        GLint MAGNUM_LOCAL getLevelCompressedImageSizeImplementationDSANonImmutableWorkaround(GLint level);
        GLint MAGNUM_LOCAL getLevelCompressedImageSizeImplementationDSAEXT(GLint level);
        GLint MAGNUM_LOCAL getLevelCompressedImageSizeImplementationDSAEXTImmutableWorkaround(GLint level);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL getCompressedImageImplementationDSA(GLint level, const Vector2i& size, std::size_t dataOffset, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getCompressedImageImplementationDSASingleSliceWorkaround(GLint level, const Vector2i& size, std::size_t dataOffset, std::size_t dataSize, GLvoid* data);

        void MAGNUM_LOCAL getImageImplementationDefault(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationDSA(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationDSAEXT(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationRobustness(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data);

        void MAGNUM_LOCAL getCompressedImageImplementationDefault(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getCompressedImageImplementationDSA(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getCompressedImageImplementationDSAEXT(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getCompressedImageImplementationRobustness(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, std::size_t dataSize, GLvoid* data);
        #endif

        void MAGNUM_LOCAL subImageImplementationDefault(GLint level, const Vector3i& offset, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* data, const PixelStorage&);
        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_LOCAL subImageImplementationSvga3DSliceBySlice(GLint level, const Vector3i& offset, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* data, const PixelStorage&);
        #endif

        void MAGNUM_LOCAL subImageImplementationDefault(CubeMapCoordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, PixelFormat format, PixelType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(CubeMapCoordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, PixelFormat format, PixelType type, const GLvoid* data);
        void MAGNUM_LOCAL subImageImplementationDSAEXT(CubeMapCoordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, PixelFormat format, PixelType type, const GLvoid* data);
        #endif

        void MAGNUM_LOCAL compressedSubImageImplementationDefault(CubeMapCoordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, CompressedPixelFormat format, const GLvoid* data, GLsizei dataSize);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL compressedSubImageImplementationDSA(CubeMapCoordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, CompressedPixelFormat format, const GLvoid* data, GLsizei dataSize);
        void MAGNUM_LOCAL compressedSubImageImplementationDSAEXT(CubeMapCoordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, CompressedPixelFormat format, const GLvoid* data, GLsizei dataSize);
        #endif
};

}

#endif
