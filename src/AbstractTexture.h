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

namespace Magnum {

/**
@brief Non-templated base for one-, two- or three-dimensional textures.

See Texture, CubeMapTexture documentation for more information.
@todo Add glPixelStore encapsulation
@todo Anisotropic filtering
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
            ClampToEdge = GL_CLAMP_TO_EDGE,

            /**
             * Clamp to border color. Coordinates out of range will be clamped
             * to border color (set with setBorderColor()).
             */
            ClampToBorder = GL_CLAMP_TO_BORDER
        };

        /** @{ @name Internal texture formats */

        /** @brief Color components */
        enum class Components {
            /**
             * Red component only. Green and blue are set to `0`, alpha is set
             * to `1`.
             */
            Red,

            /**
             * Red and green component. Blue is set to `0`, alpha is set to
             * `1`.
             */
            RedGreen,

            RGB,            /**< Red, green and blue component. Alpha is set to `1`. */
            RGBA            /**< Red, green, blue component and alpha. */
        };

        /** @brief Type of data per each component */
        enum class ComponentType {
            UnsignedByte,   /**< Unsigned byte (char) */
            Byte,           /**< Byte (char) */
            UnsignedShort,  /**< Unsigned short */
            Short,          /**< Short */
            UnsignedInt,    /**< Unsigned integer */
            Int,            /**< Integer */
            Half,           /**< Half float (16 bit) */
            Float,          /**< Float (32 bit) */

            /**
             * Normalized unsigned byte, i.e. values from range
             * @f$ [0; 255] @f$ are converted to range @f$ [0.0; 1.0] @f$.
             */
            NormalizedUnsignedByte,

            /**
             * Normalized byte, i.e. values from range
             * @f$ [-128; 127] @f$ are converted to range @f$ [0.0; 1.0] @f$.
             */
            NormalizedByte,

            /**
             * Normalized unsigned short, i.e. values from range
             * @f$ [0; 65536] @f$ are converted to range @f$ [0.0; 1.0] @f$.
             */
            NormalizedUnsignedShort,

            /**
             * Normalized short, i.e. values from range
             * @f$ [-32768; 32767] @f$ are converted to range @f$ [0.0; 1.0] @f$.
             */
            NormalizedShort
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
             */
            Red = GL_RED,

            /**
             * Two-component (red and green channel), unsigned normalized,
             * each component probably 8bit, 16bit total.
             */
            RedGreen = GL_RG,

            /**
             * Three-component RGB, unsigned normalized, each component
             * probably 8bit, 24bit total.
             */
            RGB = GL_RGB,

            /**
             * Four-component RGBA, unsigned normalized, each component
             * probably 8bit, 24bit total.
             */
            RGBA = GL_RGBA,

            /**
             * Three-component BGR, unsigned normalized, each component
             * probably 8bit, 24bit total.
             */
            BGR = GL_BGR,

            /**
             * Four-component BGRA, unsigned normalized, each component
             * probably 8bit, 24bit total.
             */
            BGRA = GL_BGRA,

            /**
             * Four-component sRGBA, unsigned normalized, each component
             * 8bit, 32bit total.
             */
            SRGBA8 = GL_SRGB8_ALPHA8,

            /**
             * Three-component sRGB, unsigned normalized, each component
             * 8bit, 24bit total.
             */
            SRGB8 = GL_SRGB8,

            /**
             * Four-component RGBA, unsigned normalized, each RGB component
             * 10bit, alpha 2bit, 32bit total.
             */
            RGB10Alpha2 = GL_RGB10_A2,

            /**
             * Four-component RGBA, unsigned integers, each RGB component
             * 10bit, alpha channel 2bit, 32bit total.
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
             */
            RG11B10Float = GL_R11F_G11F_B10F,

            #if defined(GL_RGB565) || defined(DOXYGEN_GENERATING_OUTPUT)
            /**
             * Three-component RGB, unsigned normalized, red and blue 5bit,
             * green 6bit, 16bit total.
             */
            RGB565 = GL_RGB565,
            #endif

            /**
             * Three-component RGB, unsigned integers with exponent, each
             * component 9bit, exponent 5bit, 32bit total.
             */
            RGB9Exponent5 = GL_RGB9_E5,

            /** Compressed red channel, unsigned normalized. */
            CompressedRed = GL_COMPRESSED_RED,

            /** Compressed red and green channel, unsigned normalized. */
            CompressedRedGreen = GL_COMPRESSED_RG,

            /** Compressed RGB, unsigned normalized. */
            CompressedRGB = GL_COMPRESSED_RGB,

            /** Compressed RGBA, unsigned normalized. */
            CompressedRGBA = GL_COMPRESSED_RGBA,

            /** RTGC compressed red channel, unsigned normalized. */
            CompressedRtgcRed = GL_COMPRESSED_RED_RGTC1,

            /** RTGC compressed red channel, signed normalized. */
            CompressedRtgcSignedRed = GL_COMPRESSED_SIGNED_RED_RGTC1,

            /** RTGC compressed red and green channel, unsigned normalized. */
            CompressedRtgcRedGreen = GL_COMPRESSED_RG_RGTC2,

            /** RTGC compressed red and green channel, signed normalized. */
            CompressedRtgcSignedRedGreen = GL_COMPRESSED_SIGNED_RG_RGTC2,

            #if defined(GL_COMPRESSED_RGBA_BPTC_UNORM) || defined(DOXYGEN_GENERATING_OUTPUT)
            /** BTPC compressed RGBA, unsigned normalized. */
            CompressedBtpcRGBA = GL_COMPRESSED_RGBA_BPTC_UNORM,

            /** BTPC compressed sRGBA, unsigned normalized. */
            CompressedBtpcSRGBA = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,

            /** BTPC compressed RGB, signed float. */
            CompressedBtpcRGBSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,

            /** BTPC compressed RGB, unsigned float. */
            CompressedBtpcRGBUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
            #endif

            /** Depth component. */
            Depth = GL_DEPTH_COMPONENT,

            /** Depth and stencil component. */
            DepthStencil = GL_DEPTH_STENCIL,

            /** 16bit depth component. */
            Depth16 = GL_DEPTH_COMPONENT16,

            /** 24bit depth component. */
            Depth24 = GL_DEPTH_COMPONENT24,

            /** 32bit float depth component. */
            Depth32Float = GL_DEPTH_COMPONENT32F,

            /** 24bit depth and 8bit stencil component.  */
            Depth24Stencil8 = GL_DEPTH24_STENCIL8,

            /** 32bit float depth component and 8bit stencil component. */
            Depth32FloatStencil8 = GL_DEPTH32F_STENCIL8
        };

        class InternalFormat;

        /*@}*/

        /**
         * @brief Constructor
         * @param layer     %Texture layer (number between 0 and 31)
         * @param target    Target, e.g. `GL_TEXTURE_2D`.
         *
         * Creates one OpenGL texture.
         */
        inline AbstractTexture(GLint layer, GLenum target): target(target), _layer(layer) {
            glActiveTexture(GL_TEXTURE0 + layer);
            glGenTextures(1, &texture);
        }

        /**
         * @brief Destructor
         *
         * Deletes assigned OpenGL texture.
         */
        inline virtual ~AbstractTexture() {
            glDeleteTextures(1, &texture);
        }

        /** @brief %Texture layer */
        inline GLint layer() const { return _layer; }

        /** @brief OpenGL internal texture ID */
        inline GLuint id() const { return texture; }

        /**
         * @brief Bind texture for usage / rendering
         *
         * Sets current texture as active in its texture layer. Note that
         * only one texture can be bound to given layer.
         *
         * @see layer()
         */
        inline void bind() {
            glActiveTexture(GL_TEXTURE0 + _layer);
            glBindTexture(target, texture);
        }

        /**
         * @brief Set minification filter
         * @param filter        Filter
         * @param mipmap        Mipmap filtering. If set to anything else than
         *      BaseMipLevel, make sure textures for all mip levels are set or
         *      call generateMipmap().
         *
         * Sets filter used when the object pixel size is smaller than the
         * texture size.
         * @attention This, @ref Texture::setWrapping() "setWrapping()" and
         * setMagnificationFilter() must be called after creating the texture,
         * otherwise the texture will be incomplete.
         * @attention For rectangle textures only some modes are supported,
         * see @ref AbstractTexture::Filter "Filter" and
         * @ref AbstractTexture::Mipmap "Mipmap" documentation for more
         * information.
         */
        void setMinificationFilter(Filter filter, Mipmap mipmap = Mipmap::BaseLevel);

        /**
         * @brief Set magnification filter
         * @param filter        Filter
         *
         * Sets filter used when the object pixel size is larger than largest
         * texture size.
         * @attention This, @ref Texture::setWrapping() "setWrapping()" and
         * setMinificationFilter() must be called after creating the texture,
         * otherwise the texture will be incomplete.
         */
        inline void setMagnificationFilter(Filter filter) {
            bind();
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
        }

        /**
         * @brief Set border color
         *
         * Border color when @ref AbstractTexture::Wrapping "wrapping" is set
         * to `ClampToBorder`.
         */
        inline void setBorderColor(const Vector4& color) {
            bind();
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, color.data());
        }

        /**
         * @brief Generate mipmap
         *
         * Can not be used for rectangle textures.
         * @see setMinificationFilter()
         */
        void generateMipmap();

    protected:
        /**
         * @brief Helper for setting texture data
         *
         * Workaround for partial template specialization.
         */
        template<size_t textureDimensions> struct DataHelper {
            #ifdef DOXYGEN_GENERATING_OUTPUT
            /**
             * @brief %Texture target
             *
             * Each dimension has its own unique subset of these targets.
             */
            enum class Target: GLenum {
                Texture1D = GL_TEXTURE_1D,
                Texture2D = GL_TEXTURE_2D,
                Texture3D = GL_TEXTURE_3D,
                Array1D = GL_TEXTURE_1D_ARRAY,
                Array2D = GL_TEXTURE_2D_ARRAY,
                Rectangle = GL_TEXTURE_RECTANGLE,
                CubeMap = GL_TEXTURE_CUBE_MAP,
            };

            /**
             * @brief Target for given dimension
             *
             * Returns `Target::Texture1D`, `Target::Texture2D` or
             * `Target::Texture3D` based on dimension count.
             */
            inline constexpr static Target target();

            /**
             * @brief Set texture wrapping
             * @param target            Target, such as `GL_TEXTURE_RECTANGLE`
             * @param wrapping          Wrapping type for all texture dimensions
             */
            inline static void setWrapping(GLenum target, const Math::Vector<Wrapping, Dimensions>& wrapping);

            /**
             * @brief Set texture data
             * @param target            %Target
             * @param mipLevel          Mip level
             * @param internalFormat    Internal texture format
             * @param image             Image, BufferedImage or for example
             *      Trade::ImageData of the same dimension count
             *
             * Calls `glTexImage1D`, `glTexImage2D`, `glTexImage3D` depending
             * on dimension count.
             */
            template<class T> inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, T* image);

            /**
             * @brief Set texture subdata
             * @param target            %Target
             * @param mipLevel          Mip level
             * @param offset            Offset where to put data in the texture
             * @param image             Image, BufferedImage or for example
             *      Trade::ImageData of the same dimension count
             *
             * Calls `glTexSubImage1D`, `glTexSubImage2D`, `glTexSubImage3D`
             * depending on dimension count.
             */
            template<class T> inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, textureDimensions>& offset, T* image);
            #endif
        };

    private:
        const GLenum target;
        const GLint _layer;
        GLuint texture;
};

/**
@brief Internal format

When specifying internal format, you can either specify as binary OR of
component count (using Component enum) and data type per component (value from
ComponentType enum), or using one of named internal formats from Format enum,
e.g.:
@code
InternalFormat fmt1 = Format::RGBA;
InternalFormat fmt2 = Components::RGBA|ComponentType::NormalizedUnsignedByte;
@endcode
You can also use the constructor directly instead of binary OR:
@code
InternalFormat fmt2(Components::RGBA, ComponentType::NormalizedUnsignedByte);
@endcode
*/
class AbstractTexture::InternalFormat {
    public:
        /** @brief Constructor from component count and data type per component */
        InternalFormat(Components components, ComponentType type);

        /** @brief Constructor from named internal format */
        inline constexpr InternalFormat(Format format): internalFormat(static_cast<GLint>(format)) {}

        /** @brief OpenGL internal format ID */
        inline constexpr operator GLint() const { return internalFormat; }

    private:
        GLint internalFormat;
};

/** @brief Convertor of component count and data type to InternalFormat */
inline AbstractTexture::InternalFormat operator|(AbstractTexture::Components components, AbstractTexture::ComponentType type) {
    return AbstractTexture::InternalFormat(components, type);
}
/** @brief Convertor of component count and data type to InternalFormat */
inline AbstractTexture::InternalFormat operator|(AbstractTexture::ComponentType type, AbstractTexture::Components components) {
    return AbstractTexture::InternalFormat(components, type);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct AbstractTexture::DataHelper<1> {
    enum class Target: GLenum {
        Texture1D = GL_TEXTURE_1D
    };

    inline constexpr static Target target() { return Target::Texture1D; }

    inline static void setWrapping(Target target, const Math::Vector<Wrapping, 1>& wrapping) {
        glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping[0]));
    }

    template<class T> inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, T* image) {
        glTexImage1D(static_cast<GLenum>(target), mipLevel, internalFormat, image->dimensions()[0], 0, static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }

    template<class T> inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, 1>& offset, T* image) {
        glTexSubImage1D(static_cast<GLenum>(target), mipLevel, offset[0], image->dimensions()[0], static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }
};
template<> struct AbstractTexture::DataHelper<2> {
    enum class Target: GLenum {
        Texture2D = GL_TEXTURE_2D,
        Array1D = GL_TEXTURE_1D_ARRAY,
        Rectangle = GL_TEXTURE_RECTANGLE,
        CubeMap = GL_TEXTURE_CUBE_MAP
    };

    inline constexpr static Target target() { return Target::Texture2D; }

    static void setWrapping(Target target, const Math::Vector<Wrapping, 2>& wrapping);

    template<class T> inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, T* image) {
        glTexImage2D(static_cast<GLenum>(target), mipLevel, internalFormat, image->dimensions()[0], image->dimensions()[1], 0, static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }

    template<class T> inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, 2>& offset, T* image) {
        glTexSubImage2D(static_cast<GLenum>(target), mipLevel, offset[0], offset[1], image->dimensions()[0], image->dimensions()[1], static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }
};
template<> struct AbstractTexture::DataHelper<3> {
    enum class Target: GLenum {
        Texture3D = GL_TEXTURE_3D,
        Array2D = GL_TEXTURE_2D_ARRAY
    };

    inline constexpr static Target target() { return Target::Texture3D; }

    static void setWrapping(Target target, const Math::Vector<Wrapping, 3>& wrapping);

    template<class T> inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, T* image) {
        glTexImage3D(static_cast<GLenum>(target), mipLevel, internalFormat, image->dimensions()[0], image->dimensions()[1], image->dimensions()[2], 0, static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }

    template<class T> inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, 3>& offset, T* image) {
        glTexSubImage3D(static_cast<GLenum>(target), mipLevel, offset[0], offset[1], offset[2], image->dimensions()[0], image->dimensions()[1], image->dimensions()[2], static_cast<GLenum>(image->components()), static_cast<GLenum>(image->type()), image->data());
    }
};
#endif

}

#endif
