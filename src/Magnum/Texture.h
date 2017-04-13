#ifndef Magnum_Texture_h
#define Magnum_Texture_h
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
 * @brief Class @ref Magnum::Texture, typedef @ref Magnum::Texture1D, @ref Magnum::Texture2D, @ref Magnum::Texture3D
 */

#include <Corrade/Utility/Macros.h>

#include "Magnum/AbstractTexture.h"
#include "Magnum/Array.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum {

namespace Implementation {
    template<UnsignedInt> constexpr GLenum textureTarget();
    #ifndef MAGNUM_TARGET_GLES
    template<> constexpr GLenum textureTarget<1>() { return GL_TEXTURE_1D; }
    #endif
    template<> constexpr GLenum textureTarget<2>() { return GL_TEXTURE_2D; }
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    template<> constexpr GLenum textureTarget<3>() {
        #ifndef MAGNUM_TARGET_GLES2
        return GL_TEXTURE_3D;
        #else
        return GL_TEXTURE_3D_OES;
        #endif
    }
    #endif

    template<UnsignedInt dimensions> VectorTypeFor<dimensions, Int> maxTextureSize();
    template<> MAGNUM_EXPORT Vector3i maxTextureSize<3>();
}

/**
@brief Texture

Template class for one- to three-dimensional textures. See also
@ref AbstractTexture documentation for more information.

## Usage

Common usage is to fully configure all texture parameters and then set the
data from e.g. @ref Image. Example configuration of high quality texture with
trilinear anisotropic filtering, i.e. the best you can ask for:
@code
Image2D image(PixelFormat::RGBA, PixelType::UnsignedByte, {4096, 4096}, data);

Texture2D texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    .setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
    .setWrapping(Sampler::Wrapping::ClampToEdge)
    .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
    .setStorage(Math::log2(4096)+1, TextureFormat::RGBA8, {4096, 4096})
    .setSubImage(0, {}, image)
    .generateMipmap();
@endcode

@attention Note that default configuration is to use mipmaps. Be sure to either
    reduce mip level count using @ref setBaseLevel() and @ref setMaxLevel(),
    explicitly allocate all mip levels using @ref setStorage(), call
    @ref generateMipmap() after uploading the base level to generate the rest
    of the mip chain or call @ref setMinificationFilter() with another value to
    disable mipmapping.

In shader, the texture is used via `sampler1D`/`sampler2D`/`sampler3D`,
`sampler1DShadow`/`sampler2DShadow`/`sampler3DShadow`,
`isampler1D`/`isampler2D`/`isampler3D` or `usampler1D`/`usampler2D`/`usampler3D`.
See @ref AbstractShaderProgram documentation for more information about usage
in shaders.

@see @ref Texture1D, @ref Texture2D, @ref Texture3D, @ref TextureArray,
    @ref CubeMapTexture, @ref CubeMapTextureArray, @ref RectangleTexture,
    @ref BufferTexture, @ref MultisampleTexture
@requires_gles30 Extension @extension{OES,texture_3D} for 3D textures in
    OpenGL ES 2.0.
@requires_webgl20 3D textures are not available in WebGL 1.0.
@requires_gl 1D textures are not available in OpenGL ES or WebGL.
 */
template<UnsignedInt dimensions> class Texture: public AbstractTexture {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Texture dimension count */
        };

        /**
         * @brief Max supported texture size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. For 3D textures in OpenGL ES 2.0, if
         * @extension{OES,texture_3D} extension is not available, returns
         * zero vector.
         * @see @fn_gl{Get} with @def_gl{MAX_TEXTURE_SIZE},
         *      @def_gl{MAX_3D_TEXTURE_SIZE}
         */
        static VectorTypeFor<dimensions, Int> maxSize() {
            return Implementation::maxTextureSize<dimensions>();
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Compressed block size
         *
         * If @p format is compressed, returns compressed block size (in
         * pixels). For uncompressed formats returns zero vector.
         * @see @ref compressedBlockDataSize(), @fn_gl{GetInternalformat} with
         *      @def_gl{TEXTURE_COMPRESSED_BLOCK_WIDTH},
         *      @def_gl{TEXTURE_COMPRESSED_BLOCK_HEIGHT}
         * @requires_gl43 Extension @extension{ARB,internalformat_query2}
         * @requires_gl Compressed texture queries are not available in OpenGL
         *      ES.
         */
        static VectorTypeFor<dimensions, Int> compressedBlockSize(TextureFormat format) {
            return DataHelper<dimensions>::compressedBlockSize(Implementation::textureTarget<dimensions>(), format);
        }

        /**
         * @brief Compressed block data size
         *
         * If @p format is compressed, returns compressed block data size (in
         * bytes). For uncompressed formats returns zero.
         * @see @ref compressedBlockSize(), @fn_gl{GetInternalformat} with
         *      @def_gl{TEXTURE_COMPRESSED_BLOCK_SIZE}
         * @requires_gl43 Extension @extension{ARB,internalformat_query2}
         * @requires_gl Compressed texture queries are not available in OpenGL
         *      ES.
         */
        static Int compressedBlockDataSize(TextureFormat format) {
            return AbstractTexture::compressedBlockDataSize(Implementation::textureTarget<dimensions>(), format);
        }
        #endif

        /**
         * @brief Wrap existing OpenGL texture object
         * @param id            OpenGL texture ID
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL texture object
         * with target @def_gl{TEXTURE_1D}, @def_gl{TEXTURE_2D} or
         * @def_gl{TEXTURE_3D} based on dimension count. Unlike texture created
         * using constructor, the OpenGL object is by default not deleted on
         * destruction, use @p flags for different behavior.
         * @see @ref release()
         */
        static Texture<dimensions> wrap(GLuint id, ObjectFlags flags = {}) {
            return Texture<dimensions>{id, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref Texture(NoCreateT), @ref wrap(), @fn_gl{CreateTextures}
         *      with @def_gl{TEXTURE_1D}, @def_gl{TEXTURE_2D} or
         *      @def_gl{TEXTURE_3D}, eventually @fn_gl{GenTextures}
         */
        explicit Texture(): AbstractTexture(Implementation::textureTarget<dimensions>()) {}

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * @see @ref Texture(), @ref wrap()
         */
        explicit Texture(NoCreateT) noexcept: AbstractTexture{NoCreate, Implementation::textureTarget<dimensions>()} {}

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Bind level of texture to given image unit
         * @param imageUnit Image unit
         * @param level     Texture level
         * @param access    Image access
         * @param format    Image format
         *
         * Available only on 1D and 2D textures.
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
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, class = typename std::enable_if<d == 1 || d == 2>::type>
        #endif
        void bindImage(Int imageUnit, Int level, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, level, false, 0, access, format);
        }

        /**
         * @brief Bind level of given texture layer to given image unit
         * @param imageUnit Image unit
         * @param level     Texture level
         * @param layer     Texture layer
         * @param access    Image access
         * @param format    Image format
         *
         * Available only on 3D textures.
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
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, class = typename std::enable_if<d == 3>::type>
        #endif
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
         * Available only on 3D textures.
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
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, class = typename std::enable_if<d == 3>::type>
        #endif
        void bindImageLayered(Int imageUnit, Int level, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, level, true, 0, access, format);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set base mip level
         * @return Reference to self (for method chaining)
         *
         * Taken into account when generating mipmap using @ref generateMipmap()
         * and when considering texture completeness when using mipmap
         * filtering. If neither @extension{ARB,direct_state_access} (part of
         * OpenGL 4.5) nor @extension{EXT,direct_state_access} desktop
         * extension is available, the texture is bound before the operation
         * (if not already). Initial value is `0`.
         * @see @ref setMaxLevel(), @ref setMinificationFilter(),
         *      @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_BASE_LEVEL}
         * @requires_gles30 Base level is always `0` in OpenGL ES 2.0.
         * @requires_webgl20 Base level is always `0` in WebGL 1.0.
         */
        Texture<dimensions>& setBaseLevel(Int level) {
            AbstractTexture::setBaseLevel(level);
            return *this;
        }
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Set max mip level
         * @return Reference to self (for method chaining)
         *
         * Taken into account when generating mipmap using @ref generateMipmap()
         * and when considering texture completeness when using mipmap
         * filtering. If neither @extension{ARB,direct_state_access} (part of
         * OpenGL 4.5) nor @extension{EXT,direct_state_access} desktop
         * extension is available, the texture is bound before the operation
         * (if not already). Initial value is `1000`, which is clamped to count
         * of levels specified when using @ref setStorage().
         * @see @ref setBaseLevel(), @ref setMinificationFilter(),
         *      @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_MAX_LEVEL}
         * @requires_gles30 Extension @extension{APPLE,texture_max_level},
         *      otherwise the max level is always set to largest possible value
         *      in OpenGL ES 2.0.
         * @requires_webgl20 Always set to largest possible value in WebGL 1.0.
         */
        Texture<dimensions>& setMaxLevel(Int level) {
            AbstractTexture::setMaxLevel(level);
            return *this;
        }
        #endif

        /**
         * @brief Set minification filter
         * @param filter        Filter
         * @param mipmap        Mipmap filtering. If set to anything else than
         *      @ref Sampler::Mipmap::Base, make sure textures for all mip
         *      levels are set or call @ref generateMipmap().
         * @return Reference to self (for method chaining)
         *
         * Sets filter used when the object pixel size is smaller than the
         * texture size. If neither @extension{ARB,direct_state_access} (part
         * of OpenGL 4.5) nor @extension{EXT,direct_state_access} desktop
         * extension is available, the texture is bound before the operation
         * (if not already). Initial value is {@ref Sampler::Filter::Nearest,
         * @ref Sampler::Mipmap::Linear}.
         * @see @ref setMagnificationFilter(), @ref setBaseLevel(),
         *      @ref setMaxLevel(), @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_MIN_FILTER}
         */
        Texture<dimensions>& setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /**
         * @brief Set magnification filter
         * @param filter        Filter
         * @return Reference to self (for method chaining)
         *
         * Sets filter used when the object pixel size is larger than largest
         * texture size. If neither @extension{ARB,direct_state_access} (part
         * of OpenGL 4.5) nor @extension{EXT,direct_state_access} desktop
         * extension is available, the texture is bound before the operation
         * (if not already). Initial value is @ref Sampler::Filter::Linear.
         * @see @ref setMinificationFilter(), @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_MAG_FILTER}
         */
        Texture<dimensions>& setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set minimum level-of-detail parameter
         * @return Reference to self (for method chaining)
         *
         * Limits selection of highest resolution mipmap. If neither
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} desktop extension is available,
         * the texture is bound before the operation (if not already). Initial
         * value is `-1000.0f`.
         * @see @ref setMaxLod(), @ref setLodBias(), @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_MIN_LOD}
         * @requires_gles30 Texture LOD parameters are not available in OpenGL
         *      ES 2.0.
         * @requires_webgl20 Texture LOD parameters are not available in WebGL
         *      1.0.
         */
        Texture<dimensions>& setMinLod(Float lod) {
            AbstractTexture::setMinLod(lod);
            return *this;
        }

        /**
         * @brief Set maximum level-of-detail parameter
         * @return Reference to self (for method chaining)
         *
         * Limits selection of lowest resolution mipmap. If neither
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} desktop extension is available,
         * the texture is bound before the operation (if not already). Initial
         * value is `1000.0f`.
         * @see @ref setMinLod(), @ref setLodBias(), @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_MAX_LOD}
         * @requires_gles30 Texture LOD parameters are not available in OpenGL
         *      ES 2.0.
         * @requires_webgl20 Texture LOD parameters are not available in WebGL
         *      1.0.
         */
        Texture<dimensions>& setMaxLod(Float lod) {
            AbstractTexture::setMaxLod(lod);
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set level-of-detail bias
         * @return Reference to self (for method chaining)
         *
         * Fixed bias value that is added to the level-of-detail parameter. If
         * neither @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} is available, the texture is
         * bound before the operation (if not already). Initial value is
         * `0.0f`.
         * @see @ref maxLodBias(), @ref setMinLod(), @ref setMaxLod(),
         *      @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_LOD_BIAS}
         * @requires_gl Texture LOD bias can be specified only directly in
         *      fragment shader in OpenGL ES and WebGL.
         */
        Texture<dimensions>& setLodBias(Float bias) {
            AbstractTexture::setLodBias(bias);
            return *this;
        }
        #endif

        /**
         * @brief Set wrapping
         * @param wrapping          Wrapping type for all texture dimensions
         * @return Reference to self (for method chaining)
         *
         * Sets wrapping type for coordinates out of range @f$ [ 0.0, 1.0 ] @f$.
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already). Initial value is @ref Sampler::Wrapping::Repeat.
         * @see @ref setBorderColor(), @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_WRAP_S},
         *      @def_gl{TEXTURE_WRAP_T}, @def_gl{TEXTURE_WRAP_R}
         */
        Texture<dimensions>& setWrapping(const Array<dimensions, Sampler::Wrapping>& wrapping) {
            DataHelper<dimensions>::setWrapping(*this, wrapping);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Set border color
         * @return Reference to self (for method chaining)
         *
         * Border color when wrapping is set to @ref Sampler::Wrapping::ClampToBorder.
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} is available, the texture is
         * bound before the operation (if not already). Initial value is
         * `{0.0f, 0.0f, 0.0f, 0.0f}`.
         * @see @ref setWrapping(), @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_BORDER_COLOR}
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,texture_border_clamp} or
         *      @extension{NV,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        Texture<dimensions>& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set border color for integer texture
         * @return Reference to self (for method chaining)
         *
         * Border color for integer textures when wrapping is set to
         * @ref Sampler::Wrapping::ClampToBorder. If neither
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} is available, the texture is
         * bound before the operation (if not already). Initial value is
         * `{0, 0, 0, 0}`.
         * @see @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_BORDER_COLOR}
         * @requires_gl30 Extension @extension{EXT,texture_integer}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_es_extension Extension
         *      @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        Texture<dimensions>& setBorderColor(const Vector4ui& color) {
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
        Texture<dimensions>& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        #endif
        #endif

        /**
         * @brief Set max anisotropy
         * @return Reference to self (for method chaining)
         *
         * Default value is `1.0f`, which means no anisotropy. Set to value
         * greater than `1.0f` for anisotropic filtering. If extension
         * @extension{EXT,texture_filter_anisotropic} (desktop or ES) is not
         * available, this function does nothing. If neither
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} desktop extension is available,
         * the texture is bound before the operation (if not already).
         * @see @ref Sampler::maxMaxAnisotropy(), @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_MAX_ANISOTROPY_EXT}
         */
        Texture<dimensions>& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Set sRGB decoding
         * @return Reference to self (for method chaining)
         *
         * Disables or reenables decoding of sRGB values. If neither
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} desktop extension is available,
         * the texture is bound before the operation (if not already). Initial
         * value is `true`.
         * @see @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_SRGB_DECODE_EXT}
         * @requires_extension Extension @extension{EXT,texture_sRGB_decode}
         * @requires_es_extension OpenGL ES 3.0 or extension
         *      @extension{EXT,sRGB} and
         *      @extension{ANDROID,extension_pack_es31a}/
         *      @extension2{EXT,texture_sRGB_decode,texture_sRGB_decode}
         * @requires_gles SRGB decode is not available in WebGL.
         */
        Texture<dimensions>& setSRGBDecode(bool decode) {
            AbstractTexture::setSRGBDecode(decode);
            return *this;
        }
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Set component swizzle
         * @return Reference to self (for method chaining)
         *
         * You can use letters `r`, `g`, `b`, `a` for addressing components or
         * letters `0` and `1` for zero and one, similarly as in
         * @ref Math::swizzle() function. Example usage:
         * @code
         * texture.setSwizzle<'b', 'g', 'r', '0'>();
         * @endcode
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already). Initial value is `rgba`.
         * @see @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_SWIZZLE_RGBA} (or
         *      @def_gl{TEXTURE_SWIZZLE_R}, @def_gl{TEXTURE_SWIZZLE_G},
         *      @def_gl{TEXTURE_SWIZZLE_B} and @def_gl{TEXTURE_SWIZZLE_A}
         *      separately in OpenGL ES)
         * @requires_gl33 Extension @extension{ARB,texture_swizzle}
         * @requires_gles30 Texture swizzle is not available in OpenGL ES 2.0.
         * @requires_gles Texture swizzle is not available in WebGL.
         */
        template<char r, char g, char b, char a> Texture<dimensions>& setSwizzle() {
            AbstractTexture::setSwizzle<r, g, b, a>();
            return *this;
        }
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Set depth texture comparison mode
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already). Initial value is @ref Sampler::CompareMode::None.
         * @note Depth textures can be only 1D or 2D.
         * @see @ref setCompareFunction(), @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_COMPARE_MODE}
         * @requires_gles30 Extension @extension{EXT,shadow_samplers} in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Depth texture comparison is not available in WebGL
         *      1.0.
         */
        Texture<dimensions>& setCompareMode(Sampler::CompareMode mode) {
            AbstractTexture::setCompareMode(mode);
            return *this;
        }

        /**
         * @brief Set depth texture comparison function
         * @return Reference to self (for method chaining)
         *
         * Comparison operator used when comparison mode is set to
         * @ref Sampler::CompareMode::CompareRefToTexture. If neither
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} desktop extension is available,
         * the texture is bound before the operation (if not already). Initial
         * value is @ref Sampler::CompareFunction::LessOrEqual.
         * @note Depth textures can be only 1D or 2D.
         * @see @ref setCompareMode(), @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{TEXTURE_COMPARE_FUNC}
         * @requires_gles30 Extension @extension{EXT,shadow_samplers} in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Depth texture comparison is not available in WebGL
         *      1.0.
         */
        Texture<dimensions>& setCompareFunction(Sampler::CompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Set depth/stencil texture mode
         * @return Reference to self (for method chaining)
         *
         * Selects which component of packed depth/stencil texture is used for
         * texturing. If neither @extension{ARB,direct_state_access} (part of
         * OpenGL 4.5) nor @extension{EXT,direct_state_access} is available,
         * the texture is bound before the operation (if not already). Initial
         * value is @ref Sampler::DepthStencilMode::DepthComponent.
         * @note Depth textures can be only 1D or 2D.
         * @see @fn_gl2{TextureParameter,TexParameter},
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} with @def_gl{DEPTH_STENCIL_TEXTURE_MODE}
         * @requires_gl43 Extension @extension{ARB,stencil_texturing}
         * @requires_gles31 Stencil texturing is not available in OpenGL ES 3.0
         *      and older.
         * @requires_gles Stencil texturing is not available in WebGL.
         */
        Texture<dimensions>& setDepthStencilMode(Sampler::DepthStencilMode mode) {
            AbstractTexture::setDepthStencilMode(mode);
            return *this;
        }
        #endif

        /**
         * @brief Set storage
         * @param levels            Mip level count
         * @param internalFormat    Internal format
         * @param size              Size of largest mip level
         * @return Reference to self (for method chaining)
         *
         * After calling this function the texture is immutable and calling
         * @ref setStorage() or @ref setImage() is not allowed.
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already). If neither @extension{ARB,texture_storage} (part of OpenGL
         * 4.2), OpenGL ES 3.0 nor @extension{EXT,texture_storage} in OpenGL
         * ES 2.0 is available, the feature is emulated with sequence of
         * @ref setImage() calls.
         * @see @ref maxSize(), @ref setMaxLevel(), @fn_gl2{TextureStorage1D,TexStorage1D} /
         *      @fn_gl2{TextureStorage2D,TexStorage2D} / @fn_gl2{TextureStorage3D,TexStorage3D},
         *      @fn_gl_extension{TextureStorage1D,EXT,direct_state_access} /
         *      @fn_gl_extension{TextureStorage2D,EXT,direct_state_access} /
         *      @fn_gl_extension{TextureStorage3D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexStorage1D}/@fn_gl{TexStorage2D}/@fn_gl{TexStorage3D}
         * @todo allow the user to specify PixelType explicitly to avoid
         *      issues in WebGL (see setSubImage())
         */
        Texture<dimensions>& setStorage(Int levels, TextureFormat internalFormat, const VectorTypeFor<dimensions, Int>& size) {
            DataHelper<dimensions>::setStorage(*this, levels, internalFormat, size);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Image size in given mip level
         *
         * The result is not cached in any way. If neither
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} desktop extension is available,
         * the texture is bound before the operation (if not already).
         * @see @ref image(), @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} with @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, @def_gl{TEXTURE_DEPTH}
         * @requires_gles31 Texture image size queries are not available in
         *      OpenGL ES 3.0 and older.
         * @requires_gles Texture image size queries are not available in
         *      WebGL.
         */
        VectorTypeFor<dimensions, Int> imageSize(Int level) {
            return DataHelper<dimensions>::imageSize(*this, level);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Read given mip level of texture to image
         * @param level             Mip level
         * @param image             Image where to put the data
         *
         * Image parameters like format and type of pixel data are taken from
         * given image, image size is taken from the texture using
         * @ref imageSize().  The storage is not reallocated if it is large
         * enough to contain the new data.
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} is available, the texture is
         * bound before the operation (if not already). If either
         * @extension{ARB,direct_state_access} or @extension{ARB,robustness}
         * is available, the operation is protected from buffer overflow.
         * However, if @extension{ARB,direct_state_access} is not available and
         * both @extension{EXT,direct_state_access} and @extension{ARB,robustness}
         * are available, the robust operation is preferred over DSA.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} with @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, @def_gl{TEXTURE_DEPTH}, then
         *      @fn_gl{PixelStore}, then @fn_gl2{GetTextureImage,GetTexImage},
         *      @fn_gl_extension{GetnTexImage,ARB,robustness},
         *      @fn_gl_extension{GetTextureImage,EXT,direct_state_access},
         *      eventually @fn_gl{GetTexImage}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void image(Int level, Image<dimensions>& image) {
            AbstractTexture::image<dimensions>(level, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * Image2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});
         * @endcode
         */
        Image<dimensions> image(Int level, Image<dimensions>&& image);

        /**
         * @brief Read given mip level of texture to buffer image
         * @param level             Mip level
         * @param image             Buffer image where to put the data
         * @param usage             Buffer usage
         *
         * See @ref image(Int, Image&) for more information. The storage is not
         * reallocated if it is large enough to contain the new data, which
         * means that @p usage might get ignored.
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        void image(Int level, BufferImage<dimensions>& image, BufferUsage usage) {
            AbstractTexture::image<dimensions>(level, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * BufferImage2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
         * @endcode
         */
        BufferImage<dimensions> image(Int level, BufferImage<dimensions>&& image, BufferUsage usage);

        /**
         * @brief Read given mip level of compressed texture to image
         * @param level             Mip level
         * @param image             Image where to put the compressed data
         *
         * Compression format and data size are taken from the texture, image
         * size is taken using @ref imageSize(). The storage is not reallocated
         * if it is large enough to contain the new data.
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} is available, the texture is
         * bound before the operation (if not already). If either
         * @extension{ARB,direct_state_access} or @extension{ARB,robustness}
         * is available, the operation is protected from buffer overflow.
         * However, if @extension{ARB,direct_state_access} is not available and
         * both @extension{EXT,direct_state_access} and @extension{ARB,robustness}
         * are available, the robust operation is preferred over DSA.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access},
         *      eventually @fn_gl{GetTexLevelParameter} with
         *      @def_gl{TEXTURE_COMPRESSED_IMAGE_SIZE},
         *      @def_gl{TEXTURE_INTERNAL_FORMAT}, @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, @def_gl{TEXTURE_DEPTH}, then
         *      @fn_gl{PixelStore}, then
         *      @fn_gl2{GetCompressedTextureImage,GetCompressedTexImage},
         *      @fn_gl_extension{GetnCompressedTexImage,ARB,robustness},
         *      @fn_gl_extension{GetCompressedTextureImage,EXT,direct_state_access},
         *      eventually @fn_gl{GetCompressedTexImage}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void compressedImage(Int level, CompressedImage<dimensions>& image) {
            AbstractTexture::compressedImage<dimensions>(level, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedImage2D image = texture.compressedImage(0, {});
         * @endcode
         */
        CompressedImage<dimensions> compressedImage(Int level, CompressedImage<dimensions>&& image);

        /**
         * @brief Read given mip level of compressed texture to buffer image
         * @param level             Mip level
         * @param image             Buffer image where to put the compressed data
         * @param usage             Buffer usage
         *
         * See @ref compressedImage(Int, CompressedImage&) for more
         * information. The storage is not reallocated if it is large enough to
         * contain the new data, which means that @p usage might get ignored.
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        void compressedImage(Int level, CompressedBufferImage<dimensions>& image, BufferUsage usage) {
            AbstractTexture::compressedImage<dimensions>(level, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedBufferImage2D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
         * @endcode
         */
        CompressedBufferImage<dimensions> compressedImage(Int level, CompressedBufferImage<dimensions>&& image, BufferUsage usage);

        /**
         * @brief Read range of given texture mip level to image
         * @param level             Mip level
         * @param range             Range to read
         * @param image             Image where to put the data
         *
         * Image parameters like format and type of pixel data are taken from
         * given image. The storage is not reallocated if it is large enough to
         * contain the new data.
         *
         * The operation is protected from buffer overflow.
         * @see @fn_gl{GetTextureSubImage}
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void subImage(Int level, const RangeTypeFor<dimensions, Int>& range, Image<dimensions>& image) {
            AbstractTexture::subImage<dimensions>(level, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * Image2D image = texture.subImage(0, rect, {PixelFormat::RGBA, PixelType::UnsignedByte});
         * @endcode
         */
        Image<dimensions> subImage(Int level, const RangeTypeFor<dimensions, Int>& range, Image<dimensions>&& image);

        /**
         * @brief Read range of given texture mip level to buffer image
         * @param level             Mip level
         * @param range             Range to read
         * @param image             Buffer image where to put the data
         * @param usage             Buffer usage
         *
         * See @ref subImage(Int, const RangeTypeFor<dimensions, Int>&, Image&)
         * for more information. The storage is not reallocated if it is large
         * enough to contain the new data, which means that @p usage might get
         * ignored.
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void subImage(Int level, const RangeTypeFor<dimensions, Int>& range, BufferImage<dimensions>& image, BufferUsage usage) {
            AbstractTexture::subImage<dimensions>(level, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * BufferImage2D image = texture.subImage(0, rect, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
         * @endcode
         */
        BufferImage<dimensions> subImage(Int level, const RangeTypeFor<dimensions, Int>& range, BufferImage<dimensions>&& image, BufferUsage usage);

        /**
         * @brief Read range of given compressed texture mip level to image
         * @param level             Mip level
         * @param range             Range to read
         * @param image             Image where to put the compressed data
         *
         * Compression format and data size are taken from the texture.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access},
         *      eventually @fn_gl{GetTexLevelParameter} with
         *      @def_gl{TEXTURE_INTERNAL_FORMAT}, then possibly
         *      @fn_gl{GetInternalformat} with @def_gl{TEXTURE_COMPRESSED_BLOCK_SIZE},
         *      @def_gl{TEXTURE_COMPRESSED_BLOCK_WIDTH} and
         *      @def_gl{TEXTURE_COMPRESSED_BLOCK_HEIGHT}, then
         *      @fn_gl{GetCompressedTextureSubImage}
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl43 Extension @extension{ARB,internalformat_query2} if
         *      @ref CompressedPixelStorage::compressedBlockSize() and
         *      @ref CompressedPixelStorage::compressedBlockDataSize() are not
         *      set to non-zero values
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void compressedSubImage(Int level, const RangeTypeFor<dimensions, Int>& range, CompressedImage<dimensions>& image) {
            AbstractTexture::compressedSubImage<dimensions>(level, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedImage2D image = texture.compressedSubImage(0, rect, {});
         * @endcode
         */
        CompressedImage<dimensions> compressedSubImage(Int level, const RangeTypeFor<dimensions, Int>& range, CompressedImage<dimensions>&& image);

        /**
         * @brief Read range of given compressed texture mip level to buffer image
         * @param level             Mip level
         * @param range             Range to read
         * @param image             Buffer image where to put the compressed data
         * @param usage             Buffer usage
         *
         * See @ref compressedSubImage(Int, const RangeTypeFor<dimensions, Int>&, CompressedBufferImage&, BufferUsage)
         * for more information.
         * @requires_gl45 Extension @extension{ARB,get_texture_sub_image}
         * @requires_gl43 Extension @extension{ARB,internalformat_query2} if
         *      @ref CompressedPixelStorage::compressedBlockSize() and
         *      @ref CompressedPixelStorage::compressedBlockDataSize() are not
         *      set to non-zero values
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void compressedSubImage(Int level, const RangeTypeFor<dimensions, Int>& range, CompressedBufferImage<dimensions>& image, BufferUsage usage) {
            AbstractTexture::compressedSubImage<dimensions>(level, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * CompressedBufferImage2D image = texture.compressedSubImage(0, rect, {}, BufferUsage::StaticRead);
         * @endcode
         */
        CompressedBufferImage<dimensions> compressedSubImage(Int level, const RangeTypeFor<dimensions, Int>& range, CompressedBufferImage<dimensions>&& image, BufferUsage usage);
        #endif

        /**
         * @brief Set image data
         * @param level             Mip level
         * @param internalFormat    Internal format
         * @param image             @ref Image, @ref ImageView or
         *      @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * On platforms that support it prefer to use @ref setStorage() and
         * @ref setSubImage() instead, as it avoids unnecessary reallocations
         * and has better performance characteristics. This call also has no
         * equivalent in @extension{ARB,direct_state_access}, thus the texture
         * needs to be bound to some texture unit before the operation.
         * @see @ref maxSize(), @ref Framebuffer::copyImage(), @fn_gl{PixelStore},
         *      then @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexImage1D}
         *      / @fn_gl{TexImage2D} / @fn_gl{TexImage3D}
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        Texture<dimensions>& setImage(Int level, TextureFormat internalFormat, const ImageView<dimensions>& image) {
            DataHelper<dimensions>::setImage(*this, level, internalFormat, image);
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
        Texture<dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions>& image) {
            DataHelper<dimensions>::setImage(*this, level, internalFormat, image);
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
        Texture<dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions>&& image) {
            return setImage(level, internalFormat, image);
        }
        #endif

        /**
         * @brief Set compressed image data
         * @param level             Mip level
         * @param image             @ref CompressedImage, @ref CompressedImageView
         *      or compressed @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * On platforms that support it prefer to use @ref setStorage() and
         * @ref setCompressedSubImage() instead, as it avoids unnecessary
         * reallocations and has better performance characteristics. This call
         * also has no equivalent in @extension{ARB,direct_state_access}, thus
         * the texture needs to be bound to some texture unit before the
         * operation.
         * @see @ref maxSize(), @fn_gl{PixelStore}, then @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{CompressedTexImage1D} /
         *      @fn_gl{CompressedTexImage2D} / @fn_gl{CompressedTexImage3D}
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        Texture<dimensions>& setCompressedImage(Int level, const CompressedImageView<dimensions>& image) {
            DataHelper<dimensions>::setCompressedImage(*this, level, image);
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
        Texture<dimensions>& setCompressedImage(Int level, CompressedBufferImage<dimensions>& image) {
            DataHelper<dimensions>::setCompressedImage(*this, level, image);
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
        Texture<dimensions>& setCompressedImage(Int level, CompressedBufferImage<dimensions>&& image) {
            return setCompressedImage(level, image);
        }
        #endif

        /**
         * @brief Set image subdata
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             @ref Image, @ref ImageView or
         *      @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref setStorage(), @ref Framebuffer::copySubImage(),
         *      @fn_gl{PixelStore}, @fn_gl2{TextureSubImage1D,TexSubImage1D} /
         *      @fn_gl2{TextureSubImage2D,TexSubImage2D} / @fn_gl2{TextureSubImage3D,TexSubImage3D},
         *      @fn_gl_extension{TextureSubImage1D,EXT,direct_state_access} /
         *      @fn_gl_extension{TextureSubImage2D,EXT,direct_state_access} /
         *      @fn_gl_extension{TextureSubImage3D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexSubImage1D} / @fn_gl{TexSubImage2D} / @fn_gl{TexSubImage3D}
         * @requires_gles In @ref MAGNUM_TARGET_WEBGL "WebGL" the @ref PixelType
         *      of data passed in @p image must match the original one
         *      specified in @ref setImage(). It means that you might not be
         *      able to use @ref setStorage() as it uses implicit @ref PixelType
         *      value.
         */
        Texture<dimensions>& setSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, const ImageView<dimensions>& image) {
            DataHelper<Dimensions>::setSubImage(*this, level, offset, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         */
        Texture<dimensions>& setSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, BufferImage<dimensions>& image) {
            DataHelper<Dimensions>::setSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         */
        Texture<dimensions>& setSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, BufferImage<dimensions>&& image) {
            return setSubImage(level, offset, image);
        }
        #endif

        /**
         * @brief Set compressed image subdata
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             @ref CompressedImage, @ref CompressedImageView
         *      or compressed @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref setStorage(), @fn_gl{PixelStore},
         *      @fn_gl2{CompressedTextureSubImage1D,CompressedTexSubImage1D} /
         *      @fn_gl2{CompressedTextureSubImage2D,CompressedTexSubImage2D} /
         *      @fn_gl2{CompressedTextureSubImage3D,CompressedTexSubImage3D},
         *      @fn_gl_extension{CompressedTextureSubImage1D,EXT,direct_state_access} /
         *      @fn_gl_extension{CompressedTextureSubImage2D,EXT,direct_state_access} /
         *      @fn_gl_extension{CompressedTextureSubImage3D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CompressedTexSubImage1D} / @fn_gl{CompressedTexSubImage2D} /
         *      @fn_gl{CompressedTexSubImage3D}
         */
        Texture<dimensions>& setCompressedSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, const CompressedImageView<dimensions>& image) {
            DataHelper<Dimensions>::setCompressedSubImage(*this, level, offset, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         */
        Texture<dimensions>& setCompressedSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, CompressedBufferImage<dimensions>& image) {
            DataHelper<Dimensions>::setCompressedSubImage(*this, level, offset, image);
            return *this;
        }

        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         */
        Texture<dimensions>& setCompressedSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, CompressedBufferImage<dimensions>&& image) {
            return setCompressedSubImage(level, offset, image);
        }
        #endif

        /**
         * @brief Generate mipmap
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref setMinificationFilter(), @fn_gl2{GenerateTextureMipmap,GenerateMipmap},
         *      @fn_gl_extension{GenerateTextureMipmap,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GenerateMipmap}
         * @requires_gl30 Extension @extension{ARB,framebuffer_object}
         */
        Texture<dimensions>& generateMipmap() {
            AbstractTexture::generateMipmap();
            return *this;
        }

        /**
         * @brief Invalidate texture image
         * @param level             Mip level
         *
         * If running on OpenGL ES or extension @extension{ARB,invalidate_subdata}
         * (part of OpenGL 4.3) is not available, this function does nothing.
         * @see @ref invalidateSubImage(), @fn_gl{InvalidateTexImage}
         */
        void invalidateImage(Int level) { AbstractTexture::invalidateImage(level); }

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
        void invalidateSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, const VectorTypeFor<dimensions, Int>& size) {
            DataHelper<dimensions>::invalidateSubImage(*this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(MAGNUM_TARGET_WEBGL)
        Texture<dimensions>& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        template<std::size_t size> Texture<dimensions>& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit Texture(GLuint id, ObjectFlags flags) noexcept: AbstractTexture{id, Implementation::textureTarget<dimensions>(), flags} {}
};

#ifndef MAGNUM_TARGET_GLES
/**
@brief One-dimensional texture

@requires_gl Only 2D and 3D textures are available in OpenGL ES and WebGL.
*/
typedef Texture<1> Texture1D;
#endif

/** @brief Two-dimensional texture */
typedef Texture<2> Texture2D;

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
/**
@brief Three-dimensional texture

@requires_gles30 Extension @extension{OES,texture_3D} in OpenGL ES 2.0
@requires_webgl20 3D textures are not available in WebGL 1.0.
*/
typedef Texture<3> Texture3D;
#endif

}

#endif
