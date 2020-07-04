#ifndef Magnum_GL_Texture_h
#define Magnum_GL_Texture_h
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
 * @brief Class @ref Magnum::GL::Texture, typedef @ref Magnum::GL::Texture1D, @ref Magnum::GL::Texture2D, @ref Magnum::GL::Texture3D
 */

#include "Magnum/Array.h"
#include "Magnum/Sampler.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractTexture.h"
#include "Magnum/GL/Sampler.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace GL {

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
    template<> MAGNUM_GL_EXPORT Vector3i maxTextureSize<3>();
}

/**
@brief Texture

Template class for one- to three-dimensional textures. See also
@ref AbstractTexture documentation for more information.

@section GL-Texture-usage Usage

Common usage is to fully configure all texture parameters and then set the
data from e.g. @ref Image. Example configuration of high quality texture with
trilinear anisotropic filtering, i.e. the best you can ask for:

@snippet MagnumGL.cpp Texture-usage

@attention Note that default configuration is to use mipmaps. Be sure to either
    reduce mip level count using @ref setBaseLevel() and @ref setMaxLevel(),
    explicitly allocate all mip levels using @ref setStorage(), call
    @ref generateMipmap() after uploading the base level to generate the rest
    of the mip chain or call @ref setMinificationFilter() with another value to
    disable mipmapping.

In shader, the texture is used via @glsl sampler1D @ce / @glsl sampler2D @ce /
@glsl sampler3D @ce, @glsl sampler1DShadow @ce / @glsl sampler2DShadow @ce /
@glsl sampler3DShadow @ce, @glsl isampler1D @ce / @glsl isampler2D @ce /
@glsl isampler3D @ce or @glsl usampler1D @ce / @glsl usampler2D @ce /
@glsl usampler3D @ce. See @ref AbstractShaderProgram documentation for more
information about usage in shaders.

@see @ref Texture1D, @ref Texture2D, @ref Texture3D, @ref TextureArray,
    @ref CubeMapTexture, @ref CubeMapTextureArray, @ref RectangleTexture,
    @ref BufferTexture, @ref MultisampleTexture
@m_keywords{GL_TEXTURE_1D GL_TEXTURE_2D GL_TEXTURE_3D}
@requires_gles30 Extension @gl_extension{OES,texture_3D} for 3D textures in
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
         * @gl_extension{OES,texture_3D} extension is not available, returns
         * zero vector.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_TEXTURE_SIZE},
         *      @def_gl_keyword{MAX_3D_TEXTURE_SIZE}
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
         * @see @ref compressedBlockDataSize(), @fn_gl_keyword{GetInternalformat}
         *      with @def_gl_keyword{TEXTURE_COMPRESSED_BLOCK_WIDTH},
         *      @def_gl_keyword{TEXTURE_COMPRESSED_BLOCK_HEIGHT}
         * @requires_gl43 Extension @gl_extension{ARB,internalformat_query2}
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
         * @see @ref compressedBlockSize(), @fn_gl_keyword{GetInternalformat}
         *      with @def_gl_keyword{TEXTURE_COMPRESSED_BLOCK_SIZE}
         * @requires_gl43 Extension @gl_extension{ARB,internalformat_query2}
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
         * Creates new OpenGL texture object. If @gl_extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref Texture(NoCreateT), @ref wrap(), @fn_gl_keyword{CreateTextures}
         *      with @def_gl{TEXTURE_1D}, @def_gl{TEXTURE_2D} or
         *      @def_gl{TEXTURE_3D}, eventually @fn_gl_keyword{GenTextures}
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
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         * @see @ref Texture(), @ref wrap()
         */
        explicit Texture(NoCreateT) noexcept: AbstractTexture{NoCreate, Implementation::textureTarget<dimensions>()} {}

        /** @brief Copying is not allowed */
        Texture(const Texture<dimensions>&) = delete;

        /** @brief Move constructor */
        Texture(Texture<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        Texture<dimensions>& operator=(const Texture<dimensions>&) = delete;

        /** @brief Move assignment */
        Texture<dimensions>& operator=(Texture<dimensions>&&) noexcept = default;

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
         *      @fn_gl_keyword{BindImageTexture}
         * @requires_gl42 Extension @gl_extension{ARB,shader_image_load_store}
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
         *      @fn_gl_keyword{BindImageTexture}
         * @requires_gl42 Extension @gl_extension{ARB,shader_image_load_store}
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
         * @requires_gl42 Extension @gl_extension{ARB,shader_image_load_store}
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
         * filtering. If @gl_extension{ARB,direct_state_access} (part of OpenGL
         * 4.5) is not available, the texture is bound before the operation (if
         * not already). Initial value is @cpp 0 @ce.
         * @see @ref setMaxLevel(), @ref setMinificationFilter(),
         *      @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_BASE_LEVEL}
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
         * filtering. If @gl_extension{ARB,direct_state_access} (part of OpenGL
         * 4.5) is not available, the texture is bound before the operation (if
         * not already). Initial value is `1000`, which is clamped to count of
         * levels specified when using @ref setStorage().
         * @see @ref setBaseLevel(), @ref setMinificationFilter(),
         *      @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_MAX_LEVEL}
         * @requires_gles30 Extension @gl_extension{APPLE,texture_max_level},
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
         *      @ref SamplerMipmap::Base, make sure textures for all mip
         *      levels are set or call @ref generateMipmap().
         * @return Reference to self (for method chaining)
         *
         * Sets filter used when the object pixel size is smaller than the
         * texture size. If @gl_extension{ARB,direct_state_access} (part of
         * OpenGL 4.5) is not available, the texture is bound before the
         * operation (if not already). Initial value is
         * {@ref SamplerFilter::Nearest, @ref SamplerMipmap::Linear}.
         * @see @ref setMagnificationFilter(), @ref setBaseLevel(),
         *      @ref setMaxLevel(), @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_MIN_FILTER}
         */
        Texture<dimensions>& setMinificationFilter(SamplerFilter filter, SamplerMipmap mipmap = SamplerMipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /** @overload */
        Texture<dimensions>& setMinificationFilter(Magnum::SamplerFilter filter, Magnum::SamplerMipmap mipmap = Magnum::SamplerMipmap::Base) {
            return setMinificationFilter(samplerFilter(filter), samplerMipmap(mipmap));
        }

        /**
         * @brief Set magnification filter
         * @param filter        Filter
         * @return Reference to self (for method chaining)
         *
         * Sets filter used when the object pixel size is larger than largest
         * texture size. If @gl_extension{ARB,direct_state_access} (part of
         * OpenGL 4.5) is not available, the texture is bound before the
         * operation (if not already). Initial value is
         * @ref SamplerFilter::Linear.
         * @see @ref setMinificationFilter(), @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_MAG_FILTER}
         */
        Texture<dimensions>& setMagnificationFilter(SamplerFilter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        /** @overload */
        Texture<dimensions>& setMagnificationFilter(Magnum::SamplerFilter filter) {
            return setMagnificationFilter(samplerFilter(filter));
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set the minimum level-of-detail
         * @return Reference to self (for method chaining)
         *
         * Limits selection of highest resolution mipmap. If
         * @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the texture is bound before the operation (if not
         * already). Initial value is @cpp -1000.0f @ce.
         * @see @ref setMaxLod(), @ref setLodBias(), @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_MIN_LOD}
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
         * @brief Set the maximum level-of-detail
         * @return Reference to self (for method chaining)
         *
         * Limits selection of lowest resolution mipmap. If
         * @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the texture is bound before the operation (if not
         * already). Initial value is @cpp 1000.0f @ce.
         * @see @ref setMinLod(), @ref setLodBias(), @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_MAX_LOD}
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
         * @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the texture is bound before the operation (if not
         * already). Initial value is @cpp 0.0f @ce.
         * @see @ref maxLodBias(), @ref setMinLod(), @ref setMaxLod(),
         *      @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_LOD_BIAS}
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
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already). Initial value is @ref SamplerWrapping::Repeat.
         * @see @ref setBorderColor(), @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_WRAP_S},
         *      @def_gl_keyword{TEXTURE_WRAP_T}, @def_gl_keyword{TEXTURE_WRAP_R}
         */
        Texture<dimensions>& setWrapping(const Array<dimensions, SamplerWrapping>& wrapping) {
            DataHelper<dimensions>::setWrapping(*this, wrapping);
            return *this;
        }

        /** @overload */
        Texture<dimensions>& setWrapping(const Array<dimensions, Magnum::SamplerWrapping>& wrapping) {
            return setWrapping(samplerWrapping(wrapping));
        }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Set border color
         * @return Reference to self (for method chaining)
         *
         * Border color when wrapping is set to @ref SamplerWrapping::ClampToBorder.
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already). Initial value is @cpp 0x00000000_rgbaf @ce.
         * @see @ref setWrapping(), @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_BORDER_COLOR}
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,texture_border_clamp} or
         *      @gl_extension{NV,texture_border_clamp}
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
         * @ref SamplerWrapping::ClampToBorder. If
         * @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the texture is bound before the operation (if not
         * already). Initial value is @cpp {0, 0, 0, 0} @ce.
         * @see @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_BORDER_COLOR}
         * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,texture_border_clamp}
         * @requires_gles Border clamp is not available in WebGL.
         */
        Texture<dimensions>& setBorderColor(const Vector4ui& color) {
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
         * Default value is @cpp 1.0f @ce, which means no anisotropy. Set to
         * value greater than @cpp 1.0f @ce for anisotropic filtering. If
         * extension @gl_extension{EXT,texture_filter_anisotropic} (desktop or
         * ES) is not available, this function does nothing. If
         * @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref Sampler::maxMaxAnisotropy(), @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with
         *      @def_gl_keyword{TEXTURE_MAX_ANISOTROPY_EXT}
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
         * Disables or reenables decoding of sRGB values. If
         * @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the texture is bound before the operation (if not
         * already). Initial value is @cpp true @ce.
         * @see @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_SRGB_DECODE_EXT}
         * @requires_extension Extension @gl_extension{EXT,texture_sRGB_decode}
         * @requires_es_extension OpenGL ES 3.0 or extension
         *      @gl_extension{EXT,sRGB} and
         *      @gl_extension{ANDROID,extension_pack_es31a} /
         *      @gl_extension{EXT,texture_sRGB_decode}
         * @requires_gles sRGB decode is not available in WebGL.
         */
        Texture<dimensions>& setSrgbDecode(bool decode) {
            AbstractTexture::setSrgbDecode(decode);
            return *this;
        }
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Set component swizzle
         * @return Reference to self (for method chaining)
         *
         * You can use letters @cpp 'r' @ce, @cpp 'g' @ce, @cpp 'b' @ce,
         * @cpp 'a' @ce for addressing components or letters @cpp '0' @ce and
         * @cpp '1' @ce for zero and one, similarly as in the
         * @ref Math::gather() function. Example usage:
         *
         * @snippet MagnumGL.cpp Texture-setSwizzle
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already). Initial value is @cpp 'r', 'g', 'b', 'a' @ce.
         * @see @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_SWIZZLE_RGBA} (or
         *      @def_gl_keyword{TEXTURE_SWIZZLE_R}, @def_gl_keyword{TEXTURE_SWIZZLE_G},
         *      @def_gl_keyword{TEXTURE_SWIZZLE_B} and @def_gl_keyword{TEXTURE_SWIZZLE_A}
         *      separately in OpenGL ES)
         * @requires_gl33 Extension @gl_extension{ARB,texture_swizzle}
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
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already). Initial value is @ref SamplerCompareMode::None.
         * @note Depth textures can be only 1D or 2D.
         * @see @ref setCompareFunction(), @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_COMPARE_MODE}
         * @requires_gles30 Extension @gl_extension{EXT,shadow_samplers} in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Depth texture comparison is not available in WebGL
         *      1.0.
         */
        Texture<dimensions>& setCompareMode(SamplerCompareMode mode) {
            AbstractTexture::setCompareMode(mode);
            return *this;
        }

        /**
         * @brief Set depth texture comparison function
         * @return Reference to self (for method chaining)
         *
         * Comparison operator used when comparison mode is set to
         * @ref SamplerCompareMode::CompareRefToTexture. If
         * @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the texture is bound before the operation (if not
         * already). Initial value is @ref SamplerCompareFunction::LessOrEqual.
         * @note Depth textures can be only 1D or 2D.
         * @see @ref setCompareMode(), @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{TEXTURE_COMPARE_FUNC}
         * @requires_gles30 Extension @gl_extension{EXT,shadow_samplers} in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Depth texture comparison is not available in WebGL
         *      1.0.
         */
        Texture<dimensions>& setCompareFunction(SamplerCompareFunction function) {
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
         * texturing. If @gl_extension{ARB,direct_state_access} (part of OpenGL
         * 4.5) is not available, the texture is bound before the operation (if
         * not already). Initial value is
         * @ref SamplerDepthStencilMode::DepthComponent.
         * @note Depth textures can be only 1D or 2D.
         * @see @fn_gl2_keyword{TextureParameter,TexParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexParameter} with @def_gl_keyword{DEPTH_STENCIL_TEXTURE_MODE}
         * @requires_gl43 Extension @gl_extension{ARB,stencil_texturing}
         * @requires_gles31 Stencil texturing is not available in OpenGL ES 3.0
         *      and older.
         * @requires_gles Stencil texturing is not available in WebGL.
         */
        Texture<dimensions>& setDepthStencilMode(SamplerDepthStencilMode mode) {
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
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already). If neither @gl_extension{ARB,texture_storage} (part of OpenGL
         * 4.2), OpenGL ES 3.0 nor @gl_extension{EXT,texture_storage} in OpenGL
         * ES 2.0 is available, the feature is emulated with sequence of
         * @ref setImage() calls.
         * @see @ref maxSize(), @ref setMaxLevel(),
         *      @fn_gl2_keyword{TextureStorage1D,TexStorage1D} /
         *      @fn_gl2_keyword{TextureStorage2D,TexStorage2D} /
         *      @fn_gl2_keyword{TextureStorage3D,TexStorage3D},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexStorage1D} / @fn_gl_keyword{TexStorage2D} /
         *      @fn_gl_keyword{TexStorage3D}
         * @todo allow the user to specify PixelType explicitly to avoid
         *      issues in WebGL (see setSubImage())
         */
        Texture<dimensions>& setStorage(Int levels, TextureFormat internalFormat, const VectorTypeFor<dimensions, Int>& size) {
            DataHelper<dimensions>::setStorage(*this, levels, internalFormat, size);
            return *this;
        }

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Image size in given mip level
         *
         * The result is not cached in any way. If
         * @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the texture is bound before the operation (if not already).
         * @see @ref image(), @fn_gl2_keyword{GetTextureLevelParameter,GetTexLevelParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{GetTexLevelParameter} with
         *      @def_gl_keyword{TEXTURE_WIDTH}, @def_gl_keyword{TEXTURE_HEIGHT},
         *      @def_gl_keyword{TEXTURE_DEPTH}
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
         * @brief Read given texture mip level to an image
         * @param level             Mip level
         * @param image             Image where to put the data
         *
         * Image parameters like format and type of pixel data are taken from
         * given image, image size is taken from the texture using
         * @ref imageSize(). The storage is not reallocated if it is large
         * enough to contain the new data --- however if you want to read into
         * existing memory or *ensure* a reallocation does not happen, use
         * @ref image(Int, const BasicMutableImageView<dimensions>&) instead.
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already). If either @gl_extension{ARB,direct_state_access} or
         * @gl_extension{ARB,robustness} is available, the operation is
         * protected from buffer overflow.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} with @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, @def_gl{TEXTURE_DEPTH}, then
         *      @fn_gl{PixelStore}, then @fn_gl2_keyword{GetTextureImage,GetTexImage},
         *      @fn_gl_extension_keyword{GetnTexImage,ARB,robustness},
         *      eventually @fn_gl_keyword{GetTexImage}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void image(Int level, Image<dimensions>& image) {
            AbstractTexture::image<dimensions>(level, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp Texture-image1
         */
        Image<dimensions> image(Int level, Image<dimensions>&& image);

        /**
         * @brief Read given texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref image(Int, Image<dimensions>&) the function reads
         * the pixels into the memory provided by @p image, expecting it's not
         * @cpp nullptr @ce and its size is the same as size of given @p level.
         */
        void image(Int level, const BasicMutableImageView<dimensions>& image) {
            AbstractTexture::image<dimensions>(level, image);
        }

        /**
         * @brief Read given texture mip level to a buffer image
         * @param level             Mip level
         * @param image             Buffer image where to put the data
         * @param usage             Buffer usage
         *
         * See @ref image(Int, Image&) for more information. The storage is not
         * reallocated if it is large enough to contain the new data, which
         * means that @p usage might get ignored.
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         * @todo Make it more flexible (usable with
         *      @gl_extension{ARB,buffer_storage}, avoiding relocations...)
         */
        void image(Int level, BufferImage<dimensions>& image, BufferUsage usage) {
            AbstractTexture::image<dimensions>(level, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp Texture-image2
         */
        BufferImage<dimensions> image(Int level, BufferImage<dimensions>&& image, BufferUsage usage);

        /**
         * @brief Read given compressed texture mip level to an image
         * @param level             Mip level
         * @param image             Image where to put the compressed data
         *
         * Compression format and data size are taken from the texture, image
         * size is taken using @ref imageSize(). The storage is not reallocated
         * if it is large enough to contain the new data --- however if you
         * want to read into existing memory or *ensure* a reallocation does
         * not happen, use @ref compressedImage(Int, const BasicMutableCompressedImageView<dimensions>&)
         * instead.
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already). If either @gl_extension{ARB,direct_state_access} or
         * @gl_extension{ARB,robustness} is available, the operation is
         * protected from buffer overflow.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      eventually @fn_gl{GetTexLevelParameter} with
         *      @def_gl{TEXTURE_COMPRESSED_IMAGE_SIZE},
         *      @def_gl{TEXTURE_INTERNAL_FORMAT}, @def_gl{TEXTURE_WIDTH},
         *      @def_gl{TEXTURE_HEIGHT}, @def_gl{TEXTURE_DEPTH}, then
         *      @fn_gl{PixelStore}, then
         *      @fn_gl2_keyword{GetCompressedTextureImage,GetCompressedTexImage},
         *      @fn_gl_extension_keyword{GetnCompressedTexImage,ARB,robustness},
         *      eventually @fn_gl_keyword{GetCompressedTexImage}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         */
        void compressedImage(Int level, CompressedImage<dimensions>& image) {
            AbstractTexture::compressedImage<dimensions>(level, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp Texture-compressedImage1
         */
        CompressedImage<dimensions> compressedImage(Int level, CompressedImage<dimensions>&& image);

        /**
         * @brief Read given compressed texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref compressedImage(Int, CompressedImage<dimensions>&)
         * the function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce, its format is the same as
         * texture format and its size is the same as size of given @p level.
         */
        void compressedImage(Int level, const BasicMutableCompressedImageView<dimensions>& image) {
            AbstractTexture::compressedImage<dimensions>(level, image);
        }

        /**
         * @brief Read given compressed texture mip level to a buffer image
         * @param level             Mip level
         * @param image             Buffer image where to put the compressed data
         * @param usage             Buffer usage
         *
         * See @ref compressedImage(Int, CompressedImage&) for more
         * information. The storage is not reallocated if it is large enough to
         * contain the new data, which means that @p usage might get ignored.
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() or @ref DebugTools::textureSubImage()
         *      for possible workarounds.
         * @todo Make it more flexible (usable with
         *      @gl_extension{ARB,buffer_storage}, avoiding relocations...)
         */
        void compressedImage(Int level, CompressedBufferImage<dimensions>& image, BufferUsage usage) {
            AbstractTexture::compressedImage<dimensions>(level, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp Texture-compressedImage2
         */
        CompressedBufferImage<dimensions> compressedImage(Int level, CompressedBufferImage<dimensions>&& image, BufferUsage usage);

        /**
         * @brief Read a range of given texture mip level to an image
         * @param level             Mip level
         * @param range             Range to read
         * @param image             Image where to put the data
         *
         * Image parameters like format and type of pixel data are taken from
         * given image. The storage is not reallocated if it is large enough to
         * contain the new data --- however if you want to read into existing
         * memory or *ensure* a reallocation does not happen, use
         * @ref subImage(Int, const RangeTypeFor<dimensions, Int>&, const BasicMutableImageView<dimensions>&)
         * instead.
         *
         * The operation is protected from buffer overflow.
         * @see @fn_gl_keyword{GetTextureSubImage}
         * @requires_gl45 Extension @gl_extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void subImage(Int level, const RangeTypeFor<dimensions, Int>& range, Image<dimensions>& image) {
            AbstractTexture::subImage<dimensions>(level, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp Texture-subImage1
         */
        Image<dimensions> subImage(Int level, const RangeTypeFor<dimensions, Int>& range, Image<dimensions>&& image);

        /**
         * @brief Read a range of given texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref subImage(Int, const RangeTypeFor<dimensions, Int>&, Image<dimensions>&)
         * the function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce and its size is the same as
         * @p range size.
         */
        void subImage(Int level, const RangeTypeFor<dimensions, Int>& range, const BasicMutableImageView<dimensions>& image) {
            AbstractTexture::subImage<dimensions>(level, range, image);
        }

        /**
         * @brief Read a range of given texture mip level to a buffer image
         * @param level             Mip level
         * @param range             Range to read
         * @param image             Buffer image where to put the data
         * @param usage             Buffer usage
         *
         * See @ref subImage(Int, const RangeTypeFor<dimensions, Int>&, Image&)
         * for more information. The storage is not reallocated if it is large
         * enough to contain the new data, which means that @p usage might get
         * ignored.
         * @requires_gl45 Extension @gl_extension{ARB,get_texture_sub_image}
         * @requires_gl Texture image queries are not available in OpenGL ES or
         *      WebGL. See @ref Framebuffer::read() for possible workaround.
         */
        void subImage(Int level, const RangeTypeFor<dimensions, Int>& range, BufferImage<dimensions>& image, BufferUsage usage) {
            AbstractTexture::subImage<dimensions>(level, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp Texture-subImage2
         */
        BufferImage<dimensions> subImage(Int level, const RangeTypeFor<dimensions, Int>& range, BufferImage<dimensions>&& image, BufferUsage usage);

        /**
         * @brief Read a range of given compressed texture mip level to an image
         * @param level             Mip level
         * @param range             Range to read
         * @param image             Image where to put the compressed data
         *
         * Compression format and data size are taken from the texture. The
         * storage is not reallocated if it is large enough to contain the new
         * data --- however if you want to read into existing memory or
         * *ensure* a reallocation does not happen, use
         * @ref compressedSubImage(Int, const RangeTypeFor<dimensions, Int>&, const BasicMutableCompressedImageView<dimensions>&)
         * instead.
         * @see @fn_gl2{GetTextureLevelParameter,GetTexLevelParameter},
         *      eventually @fn_gl{GetTexLevelParameter} with
         *      @def_gl{TEXTURE_INTERNAL_FORMAT}, then possibly
         *      @fn_gl{GetInternalformat} with @def_gl{TEXTURE_COMPRESSED_BLOCK_SIZE},
         *      @def_gl{TEXTURE_COMPRESSED_BLOCK_WIDTH} and
         *      @def_gl{TEXTURE_COMPRESSED_BLOCK_HEIGHT}, then
         *      @fn_gl_keyword{GetCompressedTextureSubImage}
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
        void compressedSubImage(Int level, const RangeTypeFor<dimensions, Int>& range, CompressedImage<dimensions>& image) {
            AbstractTexture::compressedSubImage<dimensions>(level, range, image);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp Texture-compressedSubImage1
         */
        CompressedImage<dimensions> compressedSubImage(Int level, const RangeTypeFor<dimensions, Int>& range, CompressedImage<dimensions>&& image);

        /**
         * @brief Read a range of given compressed texture mip level to an image view
         * @m_since{2019,10}
         *
         * Compared to @ref compressedSubImage(Int, const RangeTypeFor<dimensions, Int>&, CompressedImage<dimensions>&)
         * the function reads the pixels into the memory provided by @p image,
         * expecting it's not @cpp nullptr @ce, its format is the same as
         * texture format and its size is the same as @p range size.
         */
        void compressedSubImage(Int level, const RangeTypeFor<dimensions, Int>& range, const BasicMutableCompressedImageView<dimensions>& image) {
            AbstractTexture::compressedSubImage<dimensions>(level, range, image);
        }

        /**
         * @brief Read a range of given compressed texture mip level to a buffer image
         * @param level             Mip level
         * @param range             Range to read
         * @param image             Buffer image where to put the compressed data
         * @param usage             Buffer usage
         *
         * See @ref compressedSubImage(Int, const RangeTypeFor<dimensions, Int>&, CompressedBufferImage&, BufferUsage)
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
        void compressedSubImage(Int level, const RangeTypeFor<dimensions, Int>& range, CompressedBufferImage<dimensions>& image, BufferUsage usage) {
            AbstractTexture::compressedSubImage<dimensions>(level, range, image, usage);
        }

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         *
         * @snippet MagnumGL.cpp Texture-compressedSubImage2
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
         * equivalent in @gl_extension{ARB,direct_state_access}, thus the texture
         * needs to be bound to some texture unit before the operation.
         *
         * On OpenGL ES 2.0 and WebGL 1.0, if @ref PixelStorage::skip() is set,
         * the functionality is emulated by adjusting the data pointer.
         * @see @ref maxSize(), @ref Framebuffer::copyImage(), @fn_gl{PixelStore},
         *      then @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexImage1D} / @fn_gl_keyword{TexImage2D} /
         *      @fn_gl_keyword{TexImage3D}
         * @requires_gles30 Extension @gl_extension{EXT,unpack_subimage}/
         *      @gl_extension{NV,pack_subimage} in OpenGL ES 2.0 if
         *      @ref PixelStorage::rowLength() is set to a non-zero value.
         * @requires_gles30 Non-zero @ref PixelStorage::imageHeight() for 3D
         *      images is not available in OpenGL ES 2.0.
         * @requires_webgl20 Non-zero @ref PixelStorage::rowLength() is not
         *      supported in WebGL 1.0.
         * @deprecated_gl Prefer to use @ref setStorage() and @ref setSubImage()
         *      instead.
         */
        Texture<dimensions>& setImage(Int level, TextureFormat internalFormat, const BasicImageView<dimensions>& image) {
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
         * also has no equivalent in @gl_extension{ARB,direct_state_access},
         * thus the texture needs to be bound to some texture unit before the
         * operation.
         * @see @ref maxSize(), @fn_gl{PixelStore}, then @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl_keyword{CompressedTexImage1D} /
         *      @fn_gl_keyword{CompressedTexImage2D} /
         *      @fn_gl_keyword{CompressedTexImage3D}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         * @deprecated_gl Prefer to use @ref setStorage() and
         *      @ref setCompressedSubImage() instead.
         */
        Texture<dimensions>& setCompressedImage(Int level, const BasicCompressedImageView<dimensions>& image) {
            DataHelper<dimensions>::setCompressedImage(*this, level, image);
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
        Texture<dimensions>& setCompressedImage(Int level, CompressedBufferImage<dimensions>& image) {
            DataHelper<dimensions>::setCompressedImage(*this, level, image);
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
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already).
         *
         * On OpenGL ES 2.0 and WebGL 1.0, if @ref PixelStorage::skip() is set,
         * the functionality is emulated by adjusting the data pointer.
         * @see @ref setStorage(), @ref Framebuffer::copySubImage(),
         *      @fn_gl{PixelStore}, @fn_gl2_keyword{TextureSubImage1D,TexSubImage1D} /
         *      @fn_gl2_keyword{TextureSubImage2D,TexSubImage2D} /
         *      @fn_gl2_keyword{TextureSubImage3D,TexSubImage3D},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{TexSubImage1D} / @fn_gl_keyword{TexSubImage2D} /
         *      @fn_gl_keyword{TexSubImage3D}
         * @requires_gles30 Extension @gl_extension{EXT,unpack_subimage}/
         *      @gl_extension{NV,pack_subimage} in OpenGL ES 2.0 if
         *      @ref PixelStorage::rowLength() is set to a non-zero value.
         * @requires_gles30 Non-zero @ref PixelStorage::imageHeight() for 3D
         *      images is not available in OpenGL ES 2.0.
         * @requires_webgl20 Non-zero @ref PixelStorage::rowLength() is not
         *      supported in WebGL 1.0.
         * @requires_gles In @ref MAGNUM_TARGET_WEBGL "WebGL" the @ref PixelType
         *      of data passed in @p image must match the original one
         *      specified in @ref setImage(). It means that you might not be
         *      able to use @ref setStorage() as it uses implicit @ref PixelType
         *      value.
         */
        Texture<dimensions>& setSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, const BasicImageView<dimensions>& image) {
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
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already).
         * @see @ref setStorage(), @fn_gl{PixelStore},
         *      @fn_gl2_keyword{CompressedTextureSubImage1D,CompressedTexSubImage1D} /
         *      @fn_gl2_keyword{CompressedTextureSubImage2D,CompressedTexSubImage2D} /
         *      @fn_gl2_keyword{CompressedTextureSubImage3D,CompressedTexSubImage3D},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{CompressedTexSubImage1D} /
         *      @fn_gl_keyword{CompressedTexSubImage2D} /
         *      @fn_gl_keyword{CompressedTexSubImage3D}
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         */
        Texture<dimensions>& setCompressedSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, const BasicCompressedImageView<dimensions>& image) {
            DataHelper<Dimensions>::setCompressedSubImage(*this, level, offset, image);
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
         */
        Texture<dimensions>& setCompressedSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, CompressedBufferImage<dimensions>& image) {
            DataHelper<Dimensions>::setCompressedSubImage(*this, level, offset, image);
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
         */
        Texture<dimensions>& setCompressedSubImage(Int level, const VectorTypeFor<dimensions, Int>& offset, CompressedBufferImage<dimensions>&& image) {
            return setCompressedSubImage(level, offset, image);
        }
        #endif

        /**
         * @brief Generate mipmap
         * @return Reference to self (for method chaining)
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the texture is bound before the operation (if not
         * already).
         * @see @ref setMinificationFilter(), @fn_gl2_keyword{GenerateTextureMipmap,GenerateMipmap},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl_keyword{GenerateMipmap}
         * @requires_gl30 Extension @gl_extension{ARB,framebuffer_object}
         */
        Texture<dimensions>& generateMipmap() {
            AbstractTexture::generateMipmap();
            return *this;
        }

        /**
         * @brief Invalidate texture image
         * @param level             Mip level
         *
         * If running on OpenGL ES or extension @gl_extension{ARB,invalidate_subdata}
         * (part of OpenGL 4.3) is not available, this function does nothing.
         * @see @ref invalidateSubImage(), @fn_gl_keyword{InvalidateTexImage}
         */
        void invalidateImage(Int level) { AbstractTexture::invalidateImage(level); }

        /**
         * @brief Invalidate texture subimage
         * @param level             Mip level
         * @param offset            Offset into the texture
         * @param size              Size of invalidated data
         *
         * If running on OpenGL ES or extension @gl_extension{ARB,invalidate_subdata}
         * (part of OpenGL 4.3) is not available, this function does nothing.
         * @see @ref invalidateImage(), @fn_gl_keyword{InvalidateTexSubImage}
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

@requires_gles30 Extension @gl_extension{OES,texture_3D} in OpenGL ES 2.0
@requires_webgl20 3D textures are not available in WebGL 1.0.
*/
typedef Texture<3> Texture3D;
#endif

}}

#endif
