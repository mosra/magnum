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

#include <Corrade/Utility/Macros.h>

#include "Magnum/AbstractTexture.h"
#include "Magnum/Array.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum {

namespace Implementation {
    template<UnsignedInt> constexpr GLenum textureTarget();
    #ifndef MAGNUM_TARGET_GLES
    template<> inline constexpr GLenum textureTarget<1>() { return GL_TEXTURE_1D; }
    #endif
    template<> inline constexpr GLenum textureTarget<2>() { return GL_TEXTURE_2D; }
    template<> inline constexpr GLenum textureTarget<3>() {
        #ifndef MAGNUM_TARGET_GLES2
        return GL_TEXTURE_3D;
        #else
        return GL_TEXTURE_3D_OES;
        #endif
    }

    template<UnsignedInt dimensions> typename DimensionTraits<dimensions, Int>::VectorType maxTextureSize();
    template<> MAGNUM_EXPORT Vector3i maxTextureSize<3>();
}

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
@requires_gles30 %Extension @es_extension{OES,texture_3D} for 3D textures in
    OpenGL ES 2.0
@requires_gl 1D textures are not available in OpenGL ES, only 2D and 3D ones.
 */
template<UnsignedInt dimensions> class Texture: public AbstractTexture {
    public:
        static const UnsignedInt Dimensions = dimensions; /**< @brief %Texture dimension count */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief %Texture target
         *
         * @deprecated Use dedicated classes instead, see documentation of
         *      particular enum value for more information.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class Target: GLenum {
            /** @deprecated Used implicitly in @ref Magnum::Texture1D "Texture1D" class. */
            Texture1D = GL_TEXTURE_1D,

            /** @deprecated Used implicitly in @ref Magnum::Texture2D "Texture2D" class. */
            Texture2D = GL_TEXTURE_2D,

            /** @deprecated Used implicitly in @ref Magnum::Texture3D "Texture3D" class. */
            Texture3D = GL_TEXTURE_3D,

            /** @deprecated Use @ref Magnum::Texture1DArray "Texture1DArray" class instead. */
            Texture1DArray = GL_TEXTURE_1D_ARRAY,

            /** @deprecated Use @ref Magnum::Texture2DArray "Texture2DArray" class instead. */
            Texture2DArray = GL_TEXTURE_2D_ARRAY,

            /** @deprecated Use @ref Magnum::MultisampleTexture2D "MultisampleTexture2D" class instead. */
            Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,

            /** @deprecated Use @ref Magnum::MultisampleTexture2DArray "MultisampleTexture2DArray" class instead. */
            Texture2DMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,

            /** @deprecated Use @ref Magnum::RectangleTexture "RectangleTexture" class instead. */
            Rectangle = GL_TEXTURE_RECTANGLE
        };
        #else
        typedef typename DataHelper<Dimensions>::Target Target;
        #endif
        #endif

        /**
         * @brief Max supported texture size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. For 3D textures in OpenGL ES 2.0, if
         * @es_extension{OES,texture_3D} extension is not available, returns
         * zero vector.
         * @see @fn_gl{Get} with @def_gl{MAX_TEXTURE_SIZE},
         *      @def_gl{MAX_3D_TEXTURE_SIZE}
         */
        static typename DimensionTraits<dimensions, Int>::VectorType maxSize() {
            return Implementation::maxTextureSize<dimensions>();
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object.
         * @see @fn_gl{GenTextures} with @def_gl{TEXTURE_1D}, @def_gl{TEXTURE_2D}
         *      or @def_gl{TEXTURE_3D}
         */
        explicit Texture(): AbstractTexture(Implementation::textureTarget<dimensions>()) {}

        #ifdef CORRADE_GCC45_COMPATIBILITY
        Texture(const Texture<dimensions>&) = delete;
        Texture(Texture<dimensions>&& other): AbstractTexture(std::move(other)) {}
        Texture<dimensions>& operator=(const Texture<dimensions>&) = delete;
        Texture<dimensions>& operator=(Texture<dimensions>&& other) {
            AbstractTexture::operator=(std::move(other));
            return *this;
        }
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief Texture()
         * @deprecated Use the parameterless @ref Magnum::Texture::Texture() "Texture()"
         *      constructor or dedicated @ref Magnum::TextureArray "TextureArray",
         *      @ref Magnum::MultisampleTexture "MultisampleTexture",
         *      @ref Magnum::RectangleTexture "RectangleTexture" classes
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use the parameterless constructor or dedicated TextureArray, MultisampleTexture, RectangleTexture classes instead") Texture(Target target): AbstractTexture(GLenum(target)) {}

        /** @brief %Texture target
         * @deprecated Use dedicated @ref Magnum::Texture "Texture",
         *      @ref Magnum::TextureArray "TextureArray",
         *      @ref Magnum::MultisampleTexture "MultisampleTexture",
         *      @ref Magnum::RectangleTexture "RectangleTexture" classes
         *      instead.
         */
        constexpr CORRADE_DEPRECATED("use dedicated Texture, TextureArray, MultisampleTexture, RectangleTexture classes instead") Target target() const { return static_cast<Target>(_target); }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief %Image size in given mip level
         *
         * The result is not cached in any way. If
         * @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some texture unit before the operation.
         * @see @ref image(), @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} or @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_WIDTH}, @def_gl{TEXTURE_HEIGHT} or
         *      @def_gl{TEXTURE_DEPTH}
         * @requires_gl %Texture image queries are not available in OpenGL ES.
         */
        typename DimensionTraits<dimensions, Int>::VectorType imageSize(Int level) {
            return DataHelper<dimensions>::imageSize(*this, _target, level);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set base mip level
         * @return Reference to self (for method chaining)
         *
         * Taken into account when generating mipmap using @ref generateMipmap()
         * and when considering texture completeness when using mipmap
         * filtering. Initial value is `0`.
         * @see @ref setMaxLevel(), @ref setMinificationFilter(),
         *      @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_BASE_LEVEL}
         * @requires_gles30 Base level is always `0` in OpenGL ES 2.0.
         */
        Texture<dimensions>& setBaseLevel(Int level) {
            AbstractTexture::setBaseLevel(level);
            return *this;
        }
        #endif

        /**
         * @brief Set max mip level
         * @return Reference to self (for method chaining)
         *
         * Taken into account when generating mipmap using @ref generateMipmap()
         * and when considering texture completeness when using mipmap
         * filtering. Initial value is `1000`, which is clamped to count of
         * levels specified when using @ref setStorage().
         * @see @ref setBaseLevel(), @ref setMinificationFilter(),
         *      @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_MAX_LEVEL}
         * @requires_gles30 %Extension @es_extension{APPLE,texture_max_level},
         *      otherwise the max level is always set to largest possible value
         *      in OpenGL ES 2.0.
         */
        Texture<dimensions>& setMaxLevel(Int level) {
            AbstractTexture::setMaxLevel(level);
            return *this;
        }

        /**
         * @brief Set minification filter
         * @param filter        Filter
         * @param mipmap        Mipmap filtering. If set to anything else than
         *      @ref Sampler::Mipmap::Base, make sure textures for all mip
         *      levels are set or call @ref generateMipmap().
         * @return Reference to self (for method chaining)
         *
         * Sets filter used when the object pixel size is smaller than the
         * texture size. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some texture unit before the
         * operation. Initial value is {@ref Sampler::Filter::Nearest,
         * @ref Sampler::Mipmap::Linear}.
         * @see @ref setMagnificationFilter(), @ref setBaseLevel(),
         *      @ref setMaxLevel(), @fn_gl{ActiveTexture}, @fn_gl{BindTexture}
         *      and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_MIN_FILTER}
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
         * texture size. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some texture unit before the
         * operation. Initial value is @ref Sampler::Filter::Linear.
         * @see @ref setMinificationFilter(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_MAG_FILTER}
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
         * Limits selection of highest resolution mipmap. If
         * @extension{EXT,direct_state_access} is not available, the texture is
         * bound to some texture unit before the operation. Initial value is
         * `-1000.0f`.
         * @see @ref setMaxLod(), @ref setLodBias(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_MIN_LOD}
         * @requires_gles30 %Texture LOD parameters are not available in OpenGL
         *      ES 2.0.
         */
        Texture<dimensions>& setMinLod(Float lod) {
            AbstractTexture::setMinLod(lod);
            return *this;
        }

        /**
         * @brief Set maximum level-of-detail parameter
         * @return Reference to self (for method chaining)
         *
         * Limits selection of lowest resolution mipmap. If
         * @extension{EXT,direct_state_access} is not available, the texture is
         * bound to some texture unit before the operation. Initial value is
         * `1000.0f`.
         * @see @ref setMinLod(), @ref setLodBias(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_MAX_LOD}
         * @requires_gles30 %Texture LOD parameters are not available in OpenGL
         *      ES 2.0.
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
         * @extension{EXT,direct_state_access} is not available, the texture is
         * bound to some texture unit before the operation. Initial value is
         * `0.0f`.
         * @see @ref maxLodBias(), @ref setMinLod(), @ref setMaxLod(),
         *      @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_LOD_BIAS}
         * @requires_gl %Texture LOD bias can be specified only directly in
         *      fragment shader in OpenGL ES.
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
         * If @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some texture unit before the operation. Initial value is
         * @ref Sampler::Wrapping::Repeat.
         * @see @ref setBorderColor(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_WRAP_S}, @def_gl{TEXTURE_WRAP_T},
         *      @def_gl{TEXTURE_WRAP_R}
         */
        Texture<dimensions>& setWrapping(const Array<dimensions, Sampler::Wrapping>& wrapping) {
            DataHelper<dimensions>::setWrapping(*this, wrapping);
            return *this;
        }

        /**
         * @brief Set border color
         * @return Reference to self (for method chaining)
         *
         * Border color when wrapping is set to @ref Sampler::Wrapping::ClampToBorder.
         * If @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some texture unit before the operation. Initial value is
         * `{0.0f, 0.0f, 0.0f, 0.0f}`.
         * @see @ref setWrapping(), @fn_gl{ActiveTexture}, @fn_gl{BindTexture}
         *      and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_BORDER_COLOR}
         * @requires_es_extension %Extension @es_extension{NV,texture_border_clamp}
         */
        Texture<dimensions>& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set border color for integer texture
         * @return Reference to self (for method chaining)
         *
         * Border color for integer textures when wrapping is set to
         * @ref Sampler::Wrapping::ClampToBorder. If @extension{EXT,direct_state_access}
         * is not available, the texture is bound to some texture unit before
         * the operation. Initial value is `{0, 0, 0, 0}`.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_BORDER_COLOR}
         * @requires_gl30 %Extension @extension{EXT,texture_integer}
         * @requires_gl Border is available only for float textures in OpenGL
         *      ES.
         */
        Texture<dimensions>& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @overload
         * @requires_gl30 %Extension @extension{EXT,texture_integer}
         * @requires_gl Border is available only for float textures in OpenGL
         *      ES.
         */
        Texture<dimensions>& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        #endif

        /**
         * @brief Set max anisotropy
         * @return Reference to self (for method chaining)
         *
         * Default value is `1.0f`, which means no anisotropy. Set to value
         * greater than `1.0f` for anisotropic filtering. If extension
         * @extension{EXT,texture_filter_anisotropic} (desktop or ES) is not
         * available, this function does nothing. If
         * @extension{EXT,direct_state_access} is not available, the texture is
         * bound to some texture unit before the operation.
         * @see @ref Sampler::maxMaxAnisotropy(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_MAX_ANISOTROPY_EXT}
         */
        Texture<dimensions>& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        /**
         * @brief Set sRGB decoding
         * @return Reference to self (for method chaining)
         *
         * Disables or reenables decoding of sRGB values. Initial value is
         * `true`.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_SRGB_DECODE_EXT}
         * @requires_extension %Extension @extension{EXT,texture_sRGB_decode}
         * @requires_es_extension OpenGL ES 3.0 or extension
         *      @es_extension{EXT,sRGB} and
         *      @es_extension2{EXT,texture_sRGB_decode,texture_sRGB_decode}
         */
        Texture<dimensions>& setSRGBDecode(bool decode) {
            AbstractTexture::setSRGBDecode(decode);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
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
         * If @extension{EXT,direct_state_access} is not available,
         * the texture is bound to some texture unit before the operation.
         * Initial value is `rgba`.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_SWIZZLE_RGBA} (or @def_gl{TEXTURE_SWIZZLE_R},
         *      @def_gl{TEXTURE_SWIZZLE_G}, @def_gl{TEXTURE_SWIZZLE_B} and
         *      @def_gl{TEXTURE_SWIZZLE_A} separately in OpenGL ES)
         * @requires_gl33 %Extension @extension{ARB,texture_swizzle}
         * @requires_gles30 %Texture swizzle is not available in OpenGL ES 2.0.
         */
        template<char r, char g, char b, char a> Texture<dimensions>& setSwizzle() {
            AbstractTexture::setSwizzle<r, g, b, a>();
            return *this;
        }
        #endif

        /**
         * @brief Set depth texture comparison mode
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available,
         * the texture is bound to some texture unit before the operation.
         * Initial value is @ref Sampler::CompareMode::None.
         * @note Depth textures can be only 1D or 2D.
         * @see @ref setCompareFunction(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_COMPARE_MODE}
         * @requires_gles30 %Extension @es_extension{EXT,shadow_samplers}
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
         * @ref Sampler::CompareMode::CompareRefToTexture. If
         * @extension{EXT,direct_state_access} is not available, the texture is
         * bound to some texture unit before the operation. Initial value is
         * @ref Sampler::CompareFunction::LessOrEqual.
         * @note Depth textures can be only 1D or 2D.
         * @see @ref setCompareMode(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_COMPARE_FUNC}
         * @requires_gles30 %Extension @es_extension{EXT,shadow_samplers}
         */
        Texture<dimensions>& setCompareFunction(Sampler::CompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set depth/stencil texture mode
         * @return Reference to self (for method chaining)
         *
         * Selects which component of packed depth/stencil texture is used for
         * texturing. If @extension{EXT,direct_state_access} is not available,
         * the texture is bound to some texture unit before the operation.
         * Initial value is @ref Sampler::DepthStencilMode::DepthComponent.
         * @note Depth textures can be only 1D or 2D.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{DEPTH_STENCIL_TEXTURE_MODE}
         * @requires_gl43 %Extension @extension{ARB,stencil_texturing}
         * @requires_gl Stencil texturing is not available in OpenGL ES.
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
         * available, the feature is emulated with sequence of @ref setImage()
         * calls.
         * @see @ref maxSize(), @ref setMaxLevel(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and
         *      @fn_gl{TexStorage1D}/@fn_gl{TexStorage2D}/@fn_gl{TexStorage3D}
         *      or @fn_gl_extension{TextureStorage1D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureStorage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureStorage3D,EXT,direct_state_access},
         *      eventually @fn_gl{TexImage1D}/@fn_gl{TexImage2D}/@fn_gl{TexImage3D}
         *      or @fn_gl_extension{TextureImage1D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage3D,EXT,direct_state_access}
         * @todo allow the user to specify ColorType explicitly to avoid
         *      issues in WebGL (see setSubImage())
         */
        Texture<dimensions>& setStorage(Int levels, TextureFormat internalFormat, const typename DimensionTraits<dimensions, Int>::VectorType& size) {
            DataHelper<dimensions>::setStorage(*this, _target, levels, internalFormat, size);
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
         * texture is bound to some texture unit before the operation. If
         * @extension{ARB,robustness} is available, the operation is protected
         * from buffer overflow. However, if both @extension{EXT,direct_state_access}
         * and @extension{ARB,robustness} are available, the DSA version is
         * used, because it is better for performance and there isn't any
         * function combining both features.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} or @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_WIDTH}, @def_gl{TEXTURE_HEIGHT} or @def_gl{TEXTURE_DEPTH},
         *      then @fn_gl{GetTexImage}, @fn_gl_extension{GetTextureImage,EXT,direct_state_access}
         *      or @fn_gl_extension{GetnTexImage,ARB,robustness}
         * @requires_gl %Texture image queries are not available in OpenGL ES.
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
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
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
         * texture is bound to some texture unit before the operation.
         * @see @ref maxSize(), @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexImage1D}/@fn_gl{TexImage2D}/@fn_gl{TexImage3D} or
         *      @fn_gl_extension{TextureImage1D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage3D,EXT,direct_state_access}
         */
        Texture<dimensions>& setImage(Int level, TextureFormat internalFormat, const ImageReference<dimensions>& image) {
            DataHelper<dimensions>::setImage(*this, _target, level, internalFormat, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload */
        Texture<dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions>& image) {
            DataHelper<dimensions>::setImage(*this, _target, level, internalFormat, image);
            return *this;
        }

        /** @overload */
        Texture<dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions>&& image) {
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
         * texture is bound to some texture unit before the operation.
         *
         * @attention In @ref MAGNUM_TARGET_WEBGL "WebGL" the @ref ColorType of
         *      data passed in @p image must match the original one specified
         *      in @ref setImage(). It means that you might not be able to use
         *      @ref setStorage() as it uses implicit @ref ColorType value.
         *
         * @see @ref setStorage(), @ref setImage(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexSubImage1D}/
         *      @fn_gl{TexSubImage2D}/@fn_gl{TexSubImage3D} or
         *      @fn_gl_extension{TextureSubImage1D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureSubImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureSubImage3D,EXT,direct_state_access}
         */
        Texture<dimensions>& setSubImage(Int level, const typename DimensionTraits<dimensions, Int>::VectorType& offset, const ImageReference<dimensions>& image) {
            DataHelper<Dimensions>::setSubImage(*this, _target, level, offset, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload */
        Texture<dimensions>& setSubImage(Int level, const typename DimensionTraits<dimensions, Int>::VectorType& offset, BufferImage<dimensions>& image) {
            DataHelper<Dimensions>::setSubImage(*this, _target, level, offset, image);
            return *this;
        }

        /** @overload */
        Texture<dimensions>& setSubImage(Int level, const typename DimensionTraits<dimensions, Int>::VectorType& offset, BufferImage<dimensions>&& image) {
            return setSubImage(level, offset, image);
        }
        #endif

        /**
         * @brief Generate mipmap
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some texture unit before the operation.
         * @see @ref setMinificationFilter(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{GenerateMipmap} or
         *      @fn_gl_extension{GenerateTextureMipmap,EXT,direct_state_access}
         * @requires_gl30 %Extension @extension{ARB,framebuffer_object}
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
        void invalidateSubImage(Int level, const typename DimensionTraits<dimensions, Int>::VectorType& offset, const typename DimensionTraits<dimensions, Int>::VectorType& size) {
            DataHelper<dimensions>::invalidateSubImage(*this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Texture<dimensions>& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
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

@requires_gles30 %Extension @es_extension{OES,texture_3D} in OpenGL ES 2.0
*/
typedef Texture<3> Texture3D;

}

#endif
