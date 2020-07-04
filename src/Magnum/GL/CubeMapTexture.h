#ifndef Magnum_GL_CubeMapTexture_h
#define Magnum_GL_CubeMapTexture_h
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

/** @file
 * @brief Class @ref Magnum::GL::CubeMapTexture, enum @ref Magnum::GL::CubeMapCoordinate
 */

#include "Magnum/Array.h"
#include "Magnum/Sampler.h"
#include "Magnum/GL/AbstractTexture.h"
#include "Magnum/GL/Sampler.h"
#include "Magnum/Math/Vector2.h"

namespace Magnum { namespace GL {

/**
@brief Cube map coordinate

@see @ref CubeMapTexture
@m_enum_values_as_keywords
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

@section GL-CubeMapTexture-usage Basic usage

See @ref Texture documentation for introduction.

Common usage is to fully configure all texture parameters and then set the
data from e.g. set of Image objects:

@snippet MagnumGL.cpp CubeMapTexture-usage

In shader, the texture is used via @glsl samplerCube @ce, @glsl samplerCubeShadow @ce,
@glsl isamplerCube @ce or @glsl usamplerCube @ce. Unlike in classic textures,
coordinates for cube map textures is signed three-part vector from the center
of the cube, which intersects one of the six sides of the cube map. See
@ref AbstractShaderProgram for more information about usage in shaders.

@see @ref Renderer::Feature::SeamlessCubeMapTexture, @ref CubeMapTextureArray,
    @ref Texture, @ref TextureArray, @ref RectangleTexture, @ref BufferTexture,
    @ref MultisampleTexture
@m_keywords{GL_TEXTURE_CUBE_MAP}
*/
class MAGNUM_GL_EXPORT CubeMapTexture: public AbstractTexture {
    friend Implementation::TextureState;

    public:
        /**
         * @brief Max supported size of one side of cube map texture
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_CUBE_MAP_TEXTURE_SIZE}
         */
        static Vector2i maxSize();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief @copybrief Texture::compressedBlockSize()
         *
         * See @ref Texture::compressedBlockSize() for more information.
         * @requires_gl43 Extension @gl_extension{ARB,internalformat_query2}
         * @requires_gl Compressed texture queries are not available in OpenGL
         *      ES.
         */
        static Vector2i compressedBlockSize(TextureFormat format) {
            return DataHelper<2>::compressedBlockSize(GL_TEXTURE_CUBE_MAP, format);
        }

        /**
         * @brief @copybrief Texture::compressedBlockDataSize()
         *
         * See @ref Texture::compressedBlockDataSize() for more information.
         * @requires_gl43 Extension @gl_extension{ARB,internalformat_query2}
         * @requires_gl Compressed texture queries are not available in OpenGL
         *      ES.
         * @see @ref compressedBlockSize(), @fn_gl_keyword{GetInternalformat}
         *      with @def_gl_keyword{TEXTURE_COMPRESSED_BLOCK_SIZE}
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
         * Creates new OpenGL texture object. If @gl_extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref CubeMapTexture(NoCreateT), @ref wrap(),
         *      @fn_gl_keyword{CreateTextures} with @def_gl{TEXTURE_CUBE_MAP},
         *      eventually @fn_gl_keyword{GenTextures}
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
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         * @see @ref CubeMapTexture(), @ref wrap()
         */
        explicit CubeMapTexture(NoCreateT) noexcept: AbstractTexture{NoCreate, GL_TEXTURE_CUBE_MAP} {}

        /** @brief Copying is not allowed */
        CubeMapTexture(const CubeMapTexture&) = delete;

        /** @brief Move constructor */
        CubeMapTexture(CubeMapTexture&&) noexcept = default;

        /** @brief Copying is not allowed */
        CubeMapTexture& operator=(const CubeMapTexture&) = delete;

        /** @brief Move assignment */
        CubeMapTexture& operator=(CubeMapTexture&&) noexcept = default;

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
         *      @fn_gl_keyword{BindImageTexture}
         * @requires_gl42 Extension @gl_extension{ARB,shader_image_load_store}
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

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief @copybrief Texture::setBaseLevel()
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
         * @brief @copybrief Texture::setMaxLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLevel() for more information.
         * @requires_gles30 Extension @gl_extension{APPLE,texture_max_level},
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
         * @brief @copybrief Texture::setMinificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinificationFilter() for more information.
         */
        CubeMapTexture& setMinificationFilter(SamplerFilter filter, SamplerMipmap mipmap = SamplerMipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /** @overload */
        CubeMapTexture& setMinificationFilter(Magnum::SamplerFilter filter, Magnum::SamplerMipmap mipmap = Magnum::SamplerMipmap::Base) {
            return setMinificationFilter(samplerFilter(filter), samplerMipmap(mipmap));
        }

        /**
         * @brief @copybrief Texture::setMagnificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMagnificationFilter() for more information.
         */
        CubeMapTexture& setMagnificationFilter(SamplerFilter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        /** @overload */
        CubeMapTexture& setMagnificationFilter(Magnum::SamplerFilter filter) {
            return setMagnificationFilter(samplerFilter(filter));
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief @copybrief Texture::setMinLod()
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
         * @brief @copybrief Texture::setMaxLod()
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
         * @brief @copybrief Texture::setLodBias()
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
         * @brief @copybrief Texture::setWrapping()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setWrapping() for more information.
         */
        CubeMapTexture& setWrapping(const Array2D<SamplerWrapping>& wrapping) {
            DataHelper<2>::setWrapping(*this, wrapping);
            return *this;
        }

        /** @overload */
        CubeMapTexture& setWrapping(const Array2D<Magnum::SamplerWrapping>& wrapping) {
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
        CubeMapTexture& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief @copybrief Texture::setBorderColor(const Vector4ui&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Vector4ui&) for more
         * information.
         * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        CubeMapTexture& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @overload
         * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        CubeMapTexture& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        #endif
        #endif

        /**
         * @brief @copybrief Texture::setMaxAnisotropy()
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
         * @brief @copybrief Texture::setSrgbDecode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSrgbDecode() for more information.
         * @requires_extension Extension @gl_extension{EXT,texture_sRGB_decode}
         * @requires_es_extension OpenGL ES 3.0 or extension
         *      @gl_extension{EXT,sRGB} and
         *      @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,texture_sRGB_decode}
         * @requires_gles sRGB decode is not available in WebGL.
         */
        CubeMapTexture& setSrgbDecode(bool decode) {
            AbstractTexture::setSrgbDecode(decode);
            return *this;
        }
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief @copybrief Texture::setSwizzle()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSwizzle() for more information.
         * @requires_gl33 Extension @gl_extension{ARB,texture_swizzle}
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
         * @brief @copybrief Texture::setCompareMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareMode() for more information.
         * @requires_gles30 Extension @gl_extension{EXT,shadow_samplers} and
         *      @gl_extension{NV,shadow_samplers_cube} in OpenGL ES 2.0.
         * @requires_webgl20 Depth texture comparison is not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setCompareMode(SamplerCompareMode mode) {
            AbstractTexture::setCompareMode(mode);
            return *this;
        }

        /**
         * @brief @copybrief Texture::setCompareFunction()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareFunction() for more information.
         * @requires_gles30 Extension @gl_extension{EXT,shadow_samplers} and
         *      @gl_extension{NV,shadow_samplers_cube} in OpenGL ES 2.0.
         * @requires_webgl20 Depth texture comparison is not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setCompareFunction(SamplerCompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
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
        CubeMapTexture& setDepthStencilMode(SamplerDepthStencilMode mode) {
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
        CubeMapTexture& setStorage(Int levels, TextureFormat internalFormat, const Vector2i& size) {
            DataHelper<2>::setStorage(*this, levels, internalFormat, size);
            return *this;
        }

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief @copybrief Texture::imageSize()
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, it is assumed that faces have the same size and just the
         * size of @ref CubeMapCoordinate::PositiveX face is queried. See
         * @ref Texture::imageSize() for more information.
         * @requires_gles31 Texture image size queries are not available in
         *      OpenGL ES 3.0 and older.
         * @requires_gles Texture image size queries are not available in
         *      WebGL.
         */
        Vector2i imageSize(Int level);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Read given texture mip level to an image
         *
         * Image parameters like format and type of pixel data are taken from
         * given image, image size is taken from the texture using
         * @ref imageSize(). The storage is not reallocated if it is large
         * enough to contain the new data.
         *
         * The operation is protected from buffer overflow.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter} with
         *      @def_gl{TEXTURE_WIDTH}, @def_gl{TEXTURE_HEIGHT}, then
         *      @fn_gl{PixelStore}, then @fn_gl2_keyword{GetTextureImage,GetTexImage}
         * @requires_gl45 Extension @gl_extension{ARB,direct_state_access}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void image(Int level, Image3D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-image1
         */
        Image3D image(Int level, Image3D&& image);

        /**
         * @brief Read given texture mip level to an image view
         *
         * Compared to @ref image(Int, Image3D&) the function reads the pixels
         * into the memory provided by @p image, expecting it's not
         * @cpp nullptr @ce and its size is the same as size of given @p level.
         */
        void image(Int level, const MutableImageView3D& image);

        /**
         * @brief Read given texture mip level to a buffer image
         *
         * See @ref image(Int, Image3D&) for more information. The storage is
         * not reallocated if it is large enough to contain the new data, which
         * means that @p usage might get ignored.
         * @requires_gl45 Extension @gl_extension{ARB,direct_state_access}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void image(Int level, BufferImage3D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-image2
         */
        BufferImage3D image(Int level, BufferImage3D&& image, BufferUsage usage);

        /**
         * @brief Read given mip level of a compressed texture to an image
         *
         * Compression format and data size are taken from the texture, image
         * size is taken using @ref imageSize(). The storage is not reallocated
         * if it is large enough to contain the new data.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter} with
         *      @def_gl{TEXTURE_COMPRESSED_IMAGE_SIZE},
         *      @def_gl{TEXTURE_INTERNAL_FORMAT}, @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, then
         *      @fn_gl2_keyword{GetCompressedTextureImage,GetCompressedTexImage}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl45 Extension @gl_extension{ARB,direct_state_access}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void compressedImage(Int level, CompressedImage3D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-compressedImage1
         */
        CompressedImage3D compressedImage(Int level, CompressedImage3D&& image);

        /**
         * @brief Read given compressed texture mip level to an image view
         *
         * Compared to @ref compressedImage(Int, CompressedImage3D&) the
         * function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce, its format is the same as
         * texture format and its size is the same as size of given @p level.
         */
        void compressedImage(Int level, const MutableCompressedImageView3D& image);

        /**
         * @brief Read given compressed texture mip level to a buffer image
         *
         * See @ref compressedImage(Int, CompressedImage3D&) for more
         * information. The storage is not reallocated if it is large enough to
         * contain the new data, which means that @p usage might get ignored.
         * @requires_gl45 Extension @gl_extension{ARB,direct_state_access}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void compressedImage(Int level, CompressedBufferImage3D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-compressedImage2
         */
        CompressedBufferImage3D compressedImage(Int level, CompressedBufferImage3D&& image, BufferUsage usage);

        /**
         * @brief Read given texture mip level and coordinate to an image
         *
         * Image parameters like format and type of pixel data are taken from
         * given image, image size is taken from the texture using
         * @ref imageSize(). The storage is not reallocated if it is large
         * enough to contain the new data.
         *
         * If @gl_extension{ARB,get_texture_sub_image} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already). If either @gl_extension{ARB,get_texture_sub_image} or
         * @gl_extension{ARB,robustness} is available, the operation is
         * protected from buffer overflow.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} with @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, then @fn_gl{PixelStore}, then
         *      @fn_gl_keyword{GetTextureSubImage},
         *      @fn_gl_extension_keyword{GetnTexImage,ARB,robustness},
         *      eventually @fn_gl_keyword{GetTexImage}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void image(CubeMapCoordinate coordinate, Int level, Image2D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-image3
         */
        Image2D image(CubeMapCoordinate coordinate, Int level, Image2D&& image);

        /**
         * @brief Read given texture mip level and coordinate to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref image(CubeMapCoordinate, Int, Image2D&) the
         * function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce and its size is the same as size
         * of given @p level.
         */
        void image(CubeMapCoordinate coordinate, Int level, const MutableImageView2D& image);

        /**
         * @brief Read given texture mip level and coordinate to a buffer image
         *
         * See @ref image(CubeMapCoordinate, Int, Image2D&) for more
         * information. The storage is not reallocated if it is large enough to
         * contain the new data, which means that @p usage might get ignored.
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void image(CubeMapCoordinate coordinate, Int level, BufferImage2D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-image4
         */
        BufferImage2D image(CubeMapCoordinate coordinate, Int level, BufferImage2D&& image, BufferUsage usage);

        /**
         * @brief Read given compressed texture mip level and coordinate to an image
         *
         * Compression format and data size are taken from the texture, image
         * size is taken using @ref imageSize(). The storage is not reallocated
         * if it is large enough to contain the new data.
         *
         * If @gl_extension{ARB,get_texture_sub_image} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already). If either @gl_extension{ARB,get_texture_sub_image} or
         * @gl_extension{ARB,robustness} is available, the operation is
         * protected from buffer overflow.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      eventually @fn_gl{GetTexLevelParameter} with
         *      @def_gl{TEXTURE_COMPRESSED_IMAGE_SIZE},
         *      @def_gl{TEXTURE_INTERNAL_FORMAT}, @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, then @fn_gl{PixelStore}, then
         *      @fn_gl_keyword{GetCompressedTextureSubImage},
         *      @fn_gl_extension_keyword{GetnCompressedTexImage,ARB,robustness},
         *      eventually @fn_gl_keyword{GetCompressedTexImage}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void compressedImage(CubeMapCoordinate coordinate, Int level, CompressedImage2D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-compressedImage3
         */
        CompressedImage2D compressedImage(CubeMapCoordinate coordinate, Int level, CompressedImage2D&& image);

        /**
         * @brief Read given compressed texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref compressedImage(CubeMapCoordinate, Int, CompressedImage2D&)
         * the function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce, its format is the same as
         * texture format and its size is the same as size of given @p level.
         */
        void compressedImage(CubeMapCoordinate coordinate, Int level, const MutableCompressedImageView2D& image);

        /**
         * @brief Read given compressed texture mip level and coordinate to a buffer image
         *
         * See @ref compressedImage(CubeMapCoordinate, Int, CompressedImage2D&)
         * for more information. The storage is not reallocated if it is large
         * enough to contain the new data, which means that @p usage might get
         * ignored.
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void compressedImage(CubeMapCoordinate coordinate, Int level, CompressedBufferImage2D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-compressedImage4
         */
        CompressedBufferImage2D compressedImage(CubeMapCoordinate coordinate, Int level, CompressedBufferImage2D&& image, BufferUsage usage);

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
        void subImage(Int level, const Range3Di& range, Image3D& image) {
            AbstractTexture::subImage<3>(level, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-subImage1
         */
        Image3D subImage(Int level, const Range3Di& range, Image3D&& image);

        /**
         * @brief Read a range of given texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref subImage(Int, const Range3Di&, Image3D&) the
         * function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce and its size is the same as
         * @p range size.
         */
        void subImage(Int level, const Range3Di& range, const MutableImageView3D& image) {
            AbstractTexture::subImage<3>(level, range, image);
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
        void subImage(Int level, const Range3Di& range, BufferImage3D& image, BufferUsage usage) {
            AbstractTexture::subImage<3>(level, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-subImage2
         */
        BufferImage3D subImage(Int level, const Range3Di& range, BufferImage3D&& image, BufferUsage usage);

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
        void compressedSubImage(Int level, const Range3Di& range, CompressedImage3D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-compressedSubImage1
         */
        CompressedImage3D compressedSubImage(Int level, const Range3Di& range, CompressedImage3D&& image);

        /**
         * @brief Read a range of given compressed texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref compressedSubImage(Int, const Range3Di&, CompressedImage3D&)
         * the function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce, its format is the same as
         * texture format and its size is the same as @p range size.
         */
        void compressedSubImage(Int level, const Range3Di& range, const MutableCompressedImageView3D& image);

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
        void compressedSubImage(Int level, const Range3Di& range, CompressedBufferImage3D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp CubeMapTexture-compressedSubImage2
         */
        CompressedBufferImage3D compressedSubImage(Int level, const Range3Di& range, CompressedBufferImage3D&& image, BufferUsage usage);
        #endif

        /**
         * @brief @copybrief Texture::setImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setImage() for more information.
         * @see @ref maxSize()
         * @requires_gles30 Extension @gl_extension{EXT,unpack_subimage}/
         *      @gl_extension{NV,pack_subimage} in OpenGL ES 2.0 if
         *      @ref PixelStorage::rowLength() is set to a non-zero value.
         * @requires_webgl20 Non-zero @ref PixelStorage::rowLength() is not
         *      supported in WebGL 1.0.
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
        CubeMapTexture& setCompressedImage(CubeMapCoordinate coordinate, Int level, const CompressedImageView2D& image) {
            DataHelper<2>::setCompressedImage(*this, GLenum(coordinate), level, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
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
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
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
         * @requires_gl45 Extension @gl_extension{ARB,direct_state_access}
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setSubImage(Int level, const Vector3i& offset, const ImageView3D& image);

        /** @overload
         * @requires_gl45 Extension @gl_extension{ARB,direct_state_access}
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setSubImage(Int level, const Vector3i& offset, BufferImage3D& image);

        /** @overload
         * @requires_gl45 Extension @gl_extension{ARB,direct_state_access}
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
         * @requires_gl45 Extension @gl_extension{ARB,direct_state_access}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setCompressedSubImage(Int level, const Vector3i& offset, const CompressedImageView3D& image);

        /** @overload
         * @requires_gl45 Extension @gl_extension{ARB,direct_state_access}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setCompressedSubImage(Int level, const Vector3i& offset, CompressedBufferImage3D& image);

        /** @overload
         * @requires_gl45 Extension @gl_extension{ARB,direct_state_access}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl In OpenGL ES and WebGL you need to set image for each
         *      face separately.
         */
        CubeMapTexture& setCompressedSubImage(Int level, const Vector3i& offset, CompressedBufferImage3D&& image) {
            return setCompressedSubImage(level, offset, image);
        }
        #endif

        /**
         * @brief @copybrief Texture::setSubImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSubImage() for more information.
         * @requires_gles30 Extension @gl_extension{EXT,unpack_subimage}/
         *      @gl_extension{NV,pack_subimage} in OpenGL ES 2.0 if
         *      @ref PixelStorage::rowLength() is set to a non-zero value.
         * @requires_webgl20 Non-zero @ref PixelStorage::rowLength() is not
         *      supported in WebGL 1.0.
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
         * @brief @copybrief Texture::setCompressedSubImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompressedSubImage() for more information.
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         */
        CubeMapTexture& setCompressedSubImage(CubeMapCoordinate coordinate, Int level, const Vector2i& offset, const CompressedImageView2D& image);

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         */
        CubeMapTexture& setCompressedSubImage(CubeMapCoordinate coordinate, Int level, const Vector2i& offset, CompressedBufferImage2D& image);

        /** @overload
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
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
         * @brief @copybrief Texture::generateMipmap()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::generateMipmap() for more information.
         * @requires_gl30 Extension @gl_extension{ARB,framebuffer_object}
         */
        CubeMapTexture& generateMipmap() {
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
         * Z coordinate is equivalent to number of texture face, i.e.
         * @ref CubeMapCoordinate::PositiveX is `0` and so on, in the same
         * order as in the enum.
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
        void MAGNUM_GL_LOCAL getLevelParameterImplementationDefault(GLint level, GLenum parameter, GLint* values);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL getLevelParameterImplementationDSA(GLint level, GLenum parameter, GLint* values);
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES
        GLint MAGNUM_GL_LOCAL getLevelCompressedImageSizeImplementationDefault(GLint level);
        GLint MAGNUM_GL_LOCAL getLevelCompressedImageSizeImplementationDefaultImmutableWorkaround(GLint level);
        GLint MAGNUM_GL_LOCAL getLevelCompressedImageSizeImplementationDSA(GLint level);
        GLint MAGNUM_GL_LOCAL getLevelCompressedImageSizeImplementationDSANonImmutableWorkaround(GLint level);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL getImageImplementationDSA(GLint level, const Vector3i& size, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data, const PixelStorage& storage);
        void MAGNUM_GL_LOCAL getImageImplementationDSAAmdSliceBySlice(GLint level, const Vector3i& size, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data, const PixelStorage& storage);
        void MAGNUM_GL_LOCAL getImageImplementationSliceBySlice(GLint level, const Vector3i& size, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data, const PixelStorage& storage);

        void MAGNUM_GL_LOCAL getCompressedImageImplementationDSA(GLint level, const Vector2i& size, std::size_t dataOffset, std::size_t dataSize, GLvoid* data);
        void MAGNUM_GL_LOCAL getCompressedImageImplementationDSASingleSliceWorkaround(GLint level, const Vector2i& size, std::size_t dataOffset, std::size_t dataSize, GLvoid* data);

        void MAGNUM_GL_LOCAL getImageImplementationDefault(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_GL_LOCAL getImageImplementationDSA(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_GL_LOCAL getImageImplementationRobustness(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data);

        void MAGNUM_GL_LOCAL getCompressedImageImplementationDefault(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, std::size_t dataSize, GLvoid* data);
        void MAGNUM_GL_LOCAL getCompressedImageImplementationDSA(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, std::size_t dataSize, GLvoid* data);
        void MAGNUM_GL_LOCAL getCompressedImageImplementationRobustness(CubeMapCoordinate coordinate, GLint level, const Vector2i& size, std::size_t dataSize, GLvoid* data);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL subImageImplementationDSA(GLint level, const Vector3i& offset, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* data, const PixelStorage&);
        void MAGNUM_GL_LOCAL subImageImplementationDSASliceBySlice(GLint level, const Vector3i& offset, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* data, const PixelStorage&);
        void MAGNUM_GL_LOCAL subImageImplementationSliceBySlice(GLint level, const Vector3i& offset, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* data, const PixelStorage&);
        #endif

        void MAGNUM_GL_LOCAL subImageImplementationDefault(CubeMapCoordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, PixelFormat format, PixelType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL subImageImplementationDSA(CubeMapCoordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, PixelFormat format, PixelType type, const GLvoid* data);
        #endif

        void MAGNUM_GL_LOCAL compressedSubImageImplementationDefault(CubeMapCoordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, CompressedPixelFormat format, const GLvoid* data, GLsizei dataSize);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL compressedSubImageImplementationDSA(CubeMapCoordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, CompressedPixelFormat format, const GLvoid* data, GLsizei dataSize);
        #endif
};

}}

#endif
