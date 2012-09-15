#ifndef Magnum_AbstractTexture_h
#define Magnum_AbstractTexture_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::AbstractTexture
 */

#include "Magnum.h"
#include "Color.h"

namespace Magnum {

/**
@brief Base for textures

@attention Don't forget to call @ref Texture::setWrapping() "setWrapping()",
setMinificationFilter() and setMagnificationFilter() after creating the
texture, otherwise the texture will be incomplete. If you specified mipmap
filtering in setMinificationFilter(), be sure to also either explicitly set
all mip levels or call generateMipmap().

The texture is bound to shader via bind(). Texture uniform on the shader must
also be set to particular texture layer using
AbstractShaderProgram::setUniform(GLint, GLint).

See Texture, CubeMapTexture and CubeMapTextureArray documentation for more
information.

@todo Add glPixelStore encapsulation
@todo Texture copying
*/
class MAGNUM_EXPORT AbstractTexture {
    AbstractTexture(const AbstractTexture& other) = delete;
    AbstractTexture(AbstractTexture&& other) = delete;
    AbstractTexture& operator=(const AbstractTexture& other) = delete;
    AbstractTexture& operator=(AbstractTexture&& other) = delete;

    public:
        /**
         * @brief %Texture filtering
         *
         * @see setMagnificationFilter() and setMinificationFilter()
         */
        enum class Filter: GLint {
            NearestNeighbor = GL_NEAREST,   /**< Nearest neighbor filtering */
            LinearInterpolation = GL_LINEAR /**< Linear interpolation filtering */
        };

        /**
         * @brief Mip level selection
         *
         * @see setMinificationFilter()
         */
        enum class Mipmap: GLint {
            BaseLevel = GL_NEAREST & ~GL_NEAREST, /**< Select base mip level */

            /**
             * Select nearest mip level. **Unavailable on rectangle textures.**
             */
            NearestLevel = GL_NEAREST_MIPMAP_NEAREST & ~GL_NEAREST,

            /**
             * Linear interpolation of nearest mip levels. **Unavailable on
             * rectangle textures.**
             */
            LinearInterpolation = GL_NEAREST_MIPMAP_LINEAR & ~GL_NEAREST
        };

        /**
         * @brief %Texture wrapping
         *
         * @see @ref Texture::setWrapping() "setWrapping()"
         */
        enum class Wrapping: GLint {
            /** Repeat texture. **Unavailable on rectangle textures.** */
            Repeat = GL_REPEAT,

            /**
             * Repeat mirrored texture. **Unavailable on rectangle textures.**
             */
            MirroredRepeat = GL_MIRRORED_REPEAT,

            /**
             * Clamp to edge. Coordinates out of the range will be clamped to
             * first / last column / row in given direction.
             */
            ClampToEdge = GL_CLAMP_TO_EDGE

            #ifndef MAGNUM_TARGET_GLES
            ,
            /**
             * Clamp to border color. Coordinates out of range will be clamped
             * to border color (set with setBorderColor()).
             * @requires_gl
             */
            ClampToBorder = GL_CLAMP_TO_BORDER
            #endif
        };

        /** @{ @name Internal texture formats */

        /**
         * @brief Color components
         *
         * @requires_gles30 (no extension providing this functionality)
         */
        enum class Components {
            /**
             * Red component only. Green and blue are set to `0`, alpha is set
             * to `1`.
             * @requires_gl30 Extension @extension{ARB,texture_rg}
             */
            Red,

            /**
             * Red and green component. Blue is set to `0`, alpha is set to
             * `1`.
             * @requires_gl30 Extension @extension{ARB,texture_rg}
             */
            RedGreen,

            RGB,            /**< Red, green and blue component. Alpha is set to `1`. */
            RGBA            /**< Red, green, blue component and alpha. */
        };

        /**
         * @brief Type of data per each component
         *
         * `NormalizedUnsignedByte` and `NormalizedUnsignedShort` are the
         * main ones for general usage.
         * @requires_gles30 (no extension providing this functionality)
         */
        enum class ComponentType {
            /**
             * (Non-normalized) unsigned byte
             * @requires_gl30 Extension @extension{EXT,texture_integer}
             */
            UnsignedByte,

            /**
             * (Non-normalized) byte
             * @requires_gl30 Extension @extension{EXT,texture_integer}
             */
            Byte,

            /**
             * (Non-normalized) unsigned short
             * @requires_gl30 Extension @extension{EXT,texture_integer}
             */
            UnsignedShort,

            /**
             * (Non-normalized) short
             * @requires_gl30 Extension @extension{EXT,texture_integer}
             */
            Short,

            /**
             * (Non-normalized) unsigned integer
             * @requires_gl30 Extension @extension{EXT,texture_integer}
             */
            UnsignedInt,

            /**
             * (Non-normalized) integer
             * @requires_gl30 Extension @extension{EXT,texture_integer}
             */
            Int,

            /**
             * Half float (16 bit)
             * @requires_gl30 Extension @extension{ARB,texture_float}
             */
            Half,

            /**
             * Float (32 bit)
             * @requires_gl30 Extension @extension{ARB,texture_float}
             */
            Float,

            /**
             * Normalized unsigned byte, i.e. values from range @f$ [0; 255] @f$
             * are converted to range @f$ [0.0; 1.0] @f$.
             */
            NormalizedUnsignedByte,

            /**
             * Normalized signed byte, i.e. values from range @f$ [-128; 127] @f$
             * are converted to range @f$ [-1.0; 1.0] @f$.
             * @requires_gl31 Extension @extension{EXT,texture_snorm}
             */
            NormalizedByte,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Normalized unsigned short, i.e. values from range @f$ [0; 65536] @f$
             * are converted to range @f$ [0.0; 1.0] @f$.
             * @requires_gl
             */
            NormalizedUnsignedShort,

            /**
             * Normalized signed short, i.e. values from range @f$ [-32768; 32767] @f$
             * are converted to range @f$ [-1.0; 1.0] @f$.
             * @requires_gl
             * @requires_gl31 Extension @extension{EXT,texture_snorm}
             */
            NormalizedShort
            #endif
        };

        /**
         * @brief Internal format
         *
         * For more information about default values for unused components and
         * normalization see enums Components and ComponentType.
         */
        enum class Format: GLenum {
            /**
             * One-component (red channel), unsigned normalized, probably
             * 8bit.
             * @requires_gl30 Extension @extension{ARB,texture_rg}
             * @requires_gles30 (no extension providing this functionality)
             */
            Red = GL_RED,

            /**
             * Two-component (red and green channel), unsigned normalized,
             * each component probably 8bit, 16bit total.
             * @requires_gl30 Extension @extension{ARB,texture_rg}
             * @requires_gles30 (no extension providing this functionality)
             */
            RedGreen = GL_RG,

            /**
             * Three-component RGB, unsigned normalized, each component
             * probably 8bit, 24bit total.
             *
             * Prefer to use the exactly specified version of this format, in
             * this case `Components::RGB|ComponentType::%NormalizedUnsignedByte`.
             */
            RGB = GL_RGB,

            /**
             * Four-component RGBA, unsigned normalized, each component
             * probably 8bit, 24bit total.
             *
             * Prefer to use the exactly specified version of this format, in
             * this case `Components::RGBA|ComponentType::%NormalizedUnsignedByte`.
             */
            RGBA = GL_RGBA,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Three-component BGR, unsigned normalized, each component
             * probably 8bit, 24bit total.
             * @requires_gl
             */
            BGR = GL_BGR,

            /**
             * Four-component BGRA, unsigned normalized, each component
             * probably 8bit, 24bit total.
             * @requires_gl
             */
            BGRA = GL_BGRA,
            #endif

            /**
             * Four-component sRGBA, unsigned normalized, each component
             * 8bit, 32bit total.
             * @requires_gles30 (no extension providing this functionality)
             */
            SRGBA8 = GL_SRGB8_ALPHA8,

            /**
             * Three-component sRGB, unsigned normalized, each component
             * 8bit, 24bit total.
             * @requires_gles30 (no extension providing this functionality)
             */
            SRGB8 = GL_SRGB8,

            /**
             * Four-component RGBA, unsigned normalized, each RGB component
             * 10bit, alpha 2bit, 32bit total.
             * @requires_gles30 (no extension providing this functionality)
             */
            RGB10Alpha2 = GL_RGB10_A2,

            /**
             * Four-component RGBA, unsigned non-normalized, each RGB
             * component 10bit, alpha channel 2bit, 32bit total.
             * @requires_gl33 Extension @extension{ARB,texture_rgb10_a2ui}
             * @requires_gles30 (no extension providing this functionality)
             */
            RGB10Alpha2Unsigned = GL_RGB10_A2UI,

            /**
             * Four-component RGBA, unsigned normalized, each RGB component
             * 5bit, alpha 1bit, 16bit total.
             */
            RGB5Alpha1 = GL_RGB5_A1,

            /**
             * Four-component RGBA, unsigned normalized, each component 4bit,
             * 16bit total.
             */
            RGBA4 = GL_RGBA4,

            /**
             * Three-component RGB, float, red and green 11bit, blue 10bit,
             * 32bit total.
             * @requires_gl30 Extension @extension{EXT,packed_float}
             * @requires_gles30 (no extension providing this functionality)
             */
            RG11B10Float = GL_R11F_G11F_B10F,

            /* 1.5.6 <= GLEW < 1.8.0 doesn't have this, even if there is
               GL_ARB_ES2_compatibility */
            #if defined(GL_RGB565) || defined(DOXYGEN_GENERATING_OUTPUT)
            /**
             * Three-component RGB, unsigned normalized, red and blue 5bit,
             * green 6bit, 16bit total.
             */
            RGB565 = GL_RGB565,
            #endif

            /**
             * Three-component RGB, unsigned with exponent, each component
             * 9bit, exponent 5bit, 32bit total.
             * @requires_gl30 Extension @extension{EXT,texture_shared_exponent}
             * @requires_gles30 (no extension providing this functionality)
             */
            RGB9Exponent5 = GL_RGB9_E5,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Compressed red channel, unsigned normalized.
             * @requires_gl
             * @requires_gl30 Extension @extension{ARB,texture_rg}
             */
            CompressedRed = GL_COMPRESSED_RED,

            /**
             * Compressed red and green channel, unsigned normalized.
             * @requires_gl
             * @requires_gl30 Extension @extension{ARB,texture_rg}
             */
            CompressedRedGreen = GL_COMPRESSED_RG,

            /**
             * Compressed RGB, unsigned normalized.
             * @requires_gl
             */
            CompressedRGB = GL_COMPRESSED_RGB,

            /**
             * Compressed RGBA, unsigned normalized.
             * @requires_gl
             */
            CompressedRGBA = GL_COMPRESSED_RGBA,

            /**
             * RTGC compressed red channel, unsigned normalized.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
             */
            CompressedRtgcRed = GL_COMPRESSED_RED_RGTC1,

            /**
             * RTGC compressed red channel, signed normalized.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
             */
            CompressedRtgcSignedRed = GL_COMPRESSED_SIGNED_RED_RGTC1,

            /**
             * RTGC compressed red and green channel, unsigned normalized.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
             */
            CompressedRtgcRedGreen = GL_COMPRESSED_RG_RGTC2,

            /**
             * RTGC compressed red and green channel, signed normalized.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,texture_compression_rgtc}
             */
            CompressedRtgcSignedRedGreen = GL_COMPRESSED_SIGNED_RG_RGTC2,

            /* These are named with _ARB suffix, because glcorearb.h doesn't
               have suffixless version (?!) and GLEW has it without suffix as
               late as of 1.8.0 { */

            /**
             * BPTC compressed RGBA, unsigned normalized.
             * @requires_gl
             * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
             */
            CompressedBptcRGBA = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,

            /**
             * BPTC compressed sRGBA, unsigned normalized.
             * @requires_gl
             * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
             */
            CompressedBptcSRGBA = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,

            /**
             * BPTC compressed RGB, signed float.
             * @requires_gl
             * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
             */
            CompressedBptcRGBSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,

            /**
             * BPTC compressed RGB, unsigned float.
             * @requires_gl
             * @requires_gl42 Extension @extension{ARB,texture_compression_bptc}
             */
            CompressedBptcRGBUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,

            /*}*/

            /**
             * Depth component, at least 16bit.
             *
             * Prefer to use the exactly specified version of this format, in
             * this case e.g. `Format::Depth16`.
             * @requires_gl
             */
            Depth = GL_DEPTH_COMPONENT,

            /**
             * Depth and stencil component, at least 24bit depth and 8bit
             * stencil.
             *
             * Prefer to use the exactly specified version of this format, in
             * this case e.g. `Format::Depth24Stencil8`.
             * @requires_gl
             */
            DepthStencil = GL_DEPTH_STENCIL,
            #endif

            /**
             * 16bit depth component.
             * @requires_gles30 (no extension providing this functionality)
             */
            Depth16 = GL_DEPTH_COMPONENT16,

            /**
             * 24bit depth component.
             * @requires_gles30 (no extension providing this functionality)
             */
            Depth24 = GL_DEPTH_COMPONENT24,

            /**
             * 32bit float depth component.
             * @requires_gl30 Extension @extension{ARB,depth_buffer_float}
             * @requires_gles30 (no extension providing this functionality)
             */
            Depth32Float = GL_DEPTH_COMPONENT32F,

            /**
             * 24bit depth and 8bit stencil component.
             * @requires_gl30 Extension @extension{EXT,packed_depth_stencil}
             * @requires_gles30 (no extension providing this functionality)
             */
            Depth24Stencil8 = GL_DEPTH24_STENCIL8,

            /**
             * 32bit float depth component and 8bit stencil component.
             * @requires_gl30 Extension @extension{ARB,depth_buffer_float}
             * @requires_gles30 (no extension providing this functionality)
             */
            Depth32FloatStencil8 = GL_DEPTH32F_STENCIL8
        };

        /**
         * @brief Internal format
         *
         * When specifying internal format, you can either specify as binary
         * OR of component count (using Component enum) and data type per
         * component (value from ComponentType enum), or using one of named
         * internal formats from Format enum, e.g.:
         * @code
         * InternalFormat fmt1 = Format::RGBA;
         * InternalFormat fmt2 = Components::RGBA|ComponentType::NormalizedUnsignedByte;
         * @endcode
         * You can also use the constructor directly instead of binary OR:
         * @code
         * InternalFormat fmt2(Components::RGBA, ComponentType::NormalizedUnsignedByte);
         * @endcode
         */
        class MAGNUM_EXPORT InternalFormat {
            public:
                /**
                 * @brief Constructor from component count and data type per component
                 *
                 * @requires_gles30 (no extension providing this functionality)
                 */
                InternalFormat(Components components, ComponentType type);

                /** @brief Constructor from named internal format */
                inline constexpr InternalFormat(Format format): internalFormat(static_cast<GLint>(format)) {}

                /** @brief OpenGL internal format ID */
                inline constexpr operator GLint() const { return internalFormat; }

            private:
                GLint internalFormat;
        };

        /*@}*/

        /**
         * @brief Max supported layer count
         *
         * At least 48.
         * @see bind(GLint), @fn_gl{Get} with @def_gl{MAX_COMBINED_TEXTURE_IMAGE_UNITS}
         */
        static GLint maxSupportedLayerCount();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Max supported anisotropy
         *
         * @see setMaxAnisotropy(), @fn_gl{Get} with @def_gl{MAX_TEXTURE_MAX_ANISOTROPY_EXT}
         * @requires_gl
         * @requires_extension @extension{EXT,texture_filter_anisotropic}
         */
        static GLfloat maxSupportedAnisotropy();
        #endif

        /**
         * @brief Constructor
         * @param target    Target, e.g. `GL_TEXTURE_2D`.
         *
         * Creates one OpenGL texture.
         * @see @fn_gl{GenTextures}
         */
        inline AbstractTexture(GLenum target): _target(target) {
            glGenTextures(1, &texture);
        }

        /**
         * @brief Destructor
         *
         * Deletes assigned OpenGL texture.
         * @see @fn_gl{DeleteTextures}
         */
        virtual ~AbstractTexture() = 0;

        /** @brief OpenGL internal texture ID */
        inline GLuint id() const { return texture; }

        /**
         * @brief Bind texture for rendering
         *
         * Sets current texture as active in given layer. The layer must be
         * between 0 and maxSupportedLayerCount(). Note that only one texture
         * can be bound to given layer.
         * @see bind(), @fn_gl{ActiveTexture}
         */
        inline void bind(GLint layer) {
            glActiveTexture(GL_TEXTURE0 + layer);
            bind();
        }

        /**
         * @brief Set minification filter
         * @param filter        Filter
         * @param mipmap        Mipmap filtering. If set to anything else than
         *      BaseMipLevel, make sure textures for all mip levels are set or
         *      call generateMipmap().
         * @return Pointer to self (for method chaining)
         *
         * Sets filter used when the object pixel size is smaller than the
         * texture size.
         * @attention For rectangle textures only some modes are supported,
         * see @ref AbstractTexture::Filter "Filter" and
         * @ref AbstractTexture::Mipmap "Mipmap" documentation for more
         * information.
         * @see bind(), @fn_gl{TexParameter} with @def_gl{TEXTURE_MIN_FILTER}
         */
        AbstractTexture* setMinificationFilter(Filter filter, Mipmap mipmap = Mipmap::BaseLevel);

        /**
         * @brief Set magnification filter
         * @param filter        Filter
         * @return Pointer to self (for method chaining)
         *
         * Sets filter used when the object pixel size is larger than largest
         * texture size.
         * @see bind(), @fn_gl{TexParameter} with @def_gl{TEXTURE_MAG_FILTER}
         */
        inline AbstractTexture* setMagnificationFilter(Filter filter) {
            bind();
            glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
            return this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set border color
         * @return Pointer to self (for method chaining)
         *
         * Border color when @ref AbstractTexture::Wrapping "wrapping" is set
         * to `ClampToBorder`.
         * @see bind(), @fn_gl{TexParameter} with @def_gl{TEXTURE_BORDER_COLOR}
         * @requires_gl
         */
        inline AbstractTexture* setBorderColor(const Color4<GLfloat>& color) {
            bind();
            glTexParameterfv(_target, GL_TEXTURE_BORDER_COLOR, color.data());
            return this;
        }

        /**
         * @brief Set max anisotropy
         * @return Pointer to self (for method chaining)
         *
         * Default value is `1.0`, which means no anisotropy. Set to value
         * greater than `1.0` for anisotropic filtering.
         * @see maxSupportedAnisotropy(), bind(), @fn_gl{TexParameter} with @def_gl{TEXTURE_MAX_ANISOTROPY_EXT}
         * @requires_gl
         * @requires_extension @extension{EXT,texture_filter_anisotropic}
         */
        inline AbstractTexture* setMaxAnisotropy(GLfloat anisotropy) {
            bind();
            glTexParameterf(_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
            return this;
        }
        #endif

        /**
         * @brief Generate mipmap
         * @return Pointer to self (for method chaining)
         *
         * Can not be used for rectangle textures.
         * @see setMinificationFilter(), @fn_gl{GenerateMipmap}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        AbstractTexture* generateMipmap();

    protected:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<size_t textureDimensions> struct DataHelper {};
        #endif

        const GLenum _target;       /**< @brief Target */

        /**
         * @brief Bind texture for parameter modification
         *
         * Unlike bind(GLint) doesn't bind the texture to any particular
         * layer, thus unusable for binding for rendering.
         * @see @fn_gl{BindTexture}
         */
        inline void bind() {
            glBindTexture(_target, texture);
        }

    private:
        GLuint texture;
};

inline AbstractTexture::~AbstractTexture() { glDeleteTextures(1, &texture); }

/** @relates AbstractTexture
@brief Convertor of component count and data type to InternalFormat

@requires_gles30 (no extension providing this functionality)
*/
inline AbstractTexture::InternalFormat operator|(AbstractTexture::Components components, AbstractTexture::ComponentType type) {
    return AbstractTexture::InternalFormat(components, type);
}

/** @relates AbstractTexture
 * @overload
 */
inline AbstractTexture::InternalFormat operator|(AbstractTexture::ComponentType type, AbstractTexture::Components components) {
    return AbstractTexture::InternalFormat(components, type);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_GLES
template<> struct AbstractTexture::DataHelper<1> {
    enum class Target: GLenum {
        Texture1D = GL_TEXTURE_1D
    };

    inline constexpr static Target target() { return Target::Texture1D; }

    inline static void setWrapping(GLenum target, const Math::Vector<1, Wrapping>& wrapping) {
        glTexParameteri(target, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping[0]));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 1, void>::type set(GLenum target, GLint mipLevel, InternalFormat internalFormat, Image* image) {
        glTexImage1D(target, mipLevel, internalFormat, image->dimensions()[0], 0, static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 1, void>::type setSub(GLenum target, GLint mipLevel, const Math::Vector<1, GLint>& offset, Image* image) {
        glTexSubImage1D(target, mipLevel, offset[0], image->dimensions()[0], static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }
};
#endif
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<2> {
    enum class Target: GLenum {
        Texture2D = GL_TEXTURE_2D
        #ifndef MAGNUM_TARGET_GLES
        ,
        Texture1DArray = GL_TEXTURE_1D_ARRAY,
        Rectangle = GL_TEXTURE_RECTANGLE
        #endif
    };

    inline constexpr static Target target() { return Target::Texture2D; }

    static void setWrapping(GLenum target, const Math::Vector<2, Wrapping>& wrapping);

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 2, void>::type set(GLenum target, GLint mipLevel, InternalFormat internalFormat, Image* image) {
        glTexImage2D(target, mipLevel, internalFormat, image->dimensions()[0], image->dimensions()[1], 0, static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 2, void>::type setSub(GLenum target, GLint mipLevel, const Math::Vector<2, GLint>& offset, Image* image) {
        glTexSubImage2D(target, mipLevel, offset[0], offset[1], image->dimensions()[0], image->dimensions()[1], static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 1, void>::type setSub(GLenum target, GLint mipLevel, const Math::Vector<2, GLint>& offset, Image* image) {
        glTexSubImage2D(target, mipLevel, offset[0], offset[1], image->dimensions()[0], 1, static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }
};
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<3> {
    enum class Target: GLenum {
        Texture3D = GL_TEXTURE_3D,
        Texture2DArray = GL_TEXTURE_2D_ARRAY
    };

    inline constexpr static Target target() { return Target::Texture3D; }

    static void setWrapping(GLenum target, const Math::Vector<3, Wrapping>& wrapping);

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 3, void>::type set(GLenum target, GLint mipLevel, InternalFormat internalFormat, Image* image) {
        glTexImage3D(target, mipLevel, internalFormat, image->dimensions()[0], image->dimensions()[1], image->dimensions()[2], 0, static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 3, void>::type setSub(GLenum target, GLint mipLevel, const Math::Vector<3, GLint>& offset, Image* image) {
        glTexSubImage3D(target, mipLevel, offset[0], offset[1], offset[2], image->dimensions()[0], image->dimensions()[1], image->dimensions()[2], static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 2, void>::type setSub(GLenum target, GLint mipLevel, const Math::Vector<3, GLint>& offset, Image* image) {
        glTexSubImage3D(target, mipLevel, offset[0], offset[1], offset[2], image->dimensions()[0], image->dimensions()[1], 1, static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }
};
#endif

}

#endif
