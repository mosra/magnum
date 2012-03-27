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
#include "TypeTraits.h"

namespace Magnum {

/**
@brief Non-templated base for one-, two- or three-dimensional textures.

See Texture, CubeMapTexture documentation for more information.
*/
class MAGNUM_EXPORT AbstractTexture {
    AbstractTexture(const AbstractTexture& other) = delete;
    AbstractTexture(AbstractTexture&& other) = delete;
    AbstractTexture& operator=(const AbstractTexture& other) = delete;
    AbstractTexture& operator=(AbstractTexture&& other) = delete;

    public:
        /** @brief %Texture filtering */
        enum class Filter: GLint {
            /**
             * Nearest neighbor filtering
             */
            NearestNeighbor = GL_NEAREST,

            /**
             * Linear interpolation filtering
             */
            LinearInterpolation = GL_LINEAR
        };

        /** @brief Mip level selection */
        enum class Mipmap: GLint {
            /**
             * Select base mip level
             */
            BaseLevel = GL_NEAREST & ~GL_NEAREST,

            /**
             * Select nearest mip level. Unavailable on rectangle textures.
             */
            NearestLevel = GL_NEAREST_MIPMAP_NEAREST & ~GL_NEAREST,

            /**
             * Linear interpolation of nearest mip levels. Unavailable on
             * rectangle textures.
             */
            LinearInterpolation = GL_NEAREST_MIPMAP_LINEAR & ~GL_NEAREST
        };

        /** @brief %Texture wrapping on the edge */
        enum class Wrapping: GLint {
            /**
             * Repeat texture. Unavailable on rectangle textures.
             */
            Repeat = GL_REPEAT,

            /**
             * Repeat mirrored texture. Unavailable on rectangle textures.
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

        /** @brief Internal format */
        enum class InternalFormat: GLint {
            Red = GL_RED,       /**< One-component (red channel) */
            RedGreen = GL_RG,   /**< Two-component (red and green channel) */
            RGB = GL_RGB,       /**< Three-component (RGB) */
            RGBA = GL_RGBA,     /**< Four-component (RGBA) */
            BGR = GL_BGR,       /**< Three-component (BGR) */
            BGRA = GL_BGRA,     /**< Four-component (BGRA) */

            /** Compressed red channel */
            CompressedRed = GL_COMPRESSED_RED,

            /** Compressed red and green channel */
            CompressedRedGreen = GL_COMPRESSED_RG,

            /** Compressed RGB */
            CompressedRGB = GL_COMPRESSED_RGB,

            /** Compressed RGBA */
            CompressedRGBA = GL_COMPRESSED_RGBA
        };

        /** @brief Color format */
        enum class ColorFormat: GLenum {
            Red = GL_RED,       /**< One-component (red channel) */
            RedGreen = GL_RG,   /**< Two-component (red and green channel) */
            RGB = GL_RGB,       /**< Three-component (RGB) */
            RGBA = GL_RGBA,     /**< Four-component (RGBA) */
            BGR = GL_BGR,       /**< Three-component (BGR) */
            BGRA = GL_BGRA      /**< Four-component (BGRA) */
        };

        /**
         * @brief Pixel size (in bytes)
         * @param format    Color format
         * @param type      Data type per color channel
         */
        static size_t pixelSize(ColorFormat format, Type type);

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
         * texture size. For rectangle textures only some modes are supported,
         * see @ref AbstractTexture::Filter "Filter" and
         * @ref AbstractTexture::Mipmap "Mipmap" documentation for more
         * information.
         * @attention This and setMagnificationFilter() must be called after
         * creating the texture, otherwise it will be unusable.
         */
        void setMinificationFilter(Filter filter, Mipmap mipmap = Mipmap::BaseLevel);

        /**
         * @brief Set magnification filter
         * @param filter        Filter
         *
         * Sets filter used when the object pixel size is larger than largest
         * texture size.
         * @attention This and setMinificationFilter() must be called after
         * creating the texture, otherwise it will be unusable.
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
         * Does nothing on rectangle textures.
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
                CubeMapPositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                CubeMapNegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                CubeMapPositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                CubeMapNegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                CubeMapPositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                CubeMapNegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
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
             * @param dimensions        %Texture dimensions
             * @param colorFormat       Color format of passed data
             * @param type              Data type
             * @param data              %Texture data
             *
             * Calls `glTexImage1D`, `glTexImage2D`, `glTexImage3D` depending
             * on dimension count.
             */
            inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, textureDimensions>& dimensions, ColorFormat colorFormat, Type type, const void* data);

            /**
             * @brief Set texture data from image
             * @param target            %Target
             * @param mipLevel          Mip level
             * @param internalFormat    Internal texture format
             * @param image             Image, BufferedImage or for example
             *      Trade::ImageData of the same dimension count
             *
             * Calls set() with image data.
             */
            template<class T> inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, T* image);

            /**
             * @brief Set texture subdata
             * @param target            %Target
             * @param mipLevel          Mip level
             * @param offset            Offset where to put data in the texture
             * @param dimensions        %Texture dimensions
             * @param colorFormat       Color format of passed data
             * @param type              Data type
             * @param data              %Texture data
             *
             * Calls `glTexSubImage1D`, `glTexSubImage2D`, `glTexSubImage3D`
             * depending on dimension count.
             */
            inline static void setSub(SubTarget target, GLint mipLevel, const Math::Vector<GLint, textureDimensions>& offset, const Math::Vector<GLsizei, textureDimensions>& dimensions, ColorFormat colorFormat, Type type, const void* data);

            /**
             * @brief Set texture subdata from image
             * @param target            %Target
             * @param mipLevel          Mip level
             * @param offset            Offset where to put data in the texture
             * @param image             Image, BufferedImage or for example
             *      Trade::ImageData of the same dimension count
             *
             * Calls setSub() with image data.
             */
            template<class T> inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, textureDimensions>& offset, T* image);
            #endif
        };

    private:
        const GLenum target;
        const GLint _layer;
        GLuint texture;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct AbstractTexture::DataHelper<1> {
    enum class Target: GLenum {
        Texture1D = GL_TEXTURE_1D
    };

    inline constexpr static Target target() { return Target::Texture1D; }

    inline static void setWrapping(Target target, const Math::Vector<Wrapping, 1>& wrapping) {
        glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping.at(0)));
    }

    inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, 1>& dimensions, ColorFormat colorFormat, Type type, const void* data) {
        glTexImage1D(static_cast<GLenum>(target), mipLevel, static_cast<GLint>(internalFormat), dimensions.at(0), 0, static_cast<GLenum>(colorFormat), static_cast<GLenum>(type), data);
    }

    template<class T> inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, T* image) {
        set(target, mipLevel, internalFormat, image->dimensions(), image->colorFormat(), image->type(), image->data());
    }

    inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, 1>& offset, const Math::Vector<GLsizei, 1>& dimensions, ColorFormat colorFormat, Type type, const void* data) {
        glTexSubImage1D(static_cast<GLenum>(target), mipLevel, offset.at(0), dimensions.at(0), static_cast<GLenum>(colorFormat), static_cast<GLenum>(type), data);
    }

    template<class T> inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, 1>& offset, T* image) {
        setSub(target, mipLevel, offset, image->dimensions(), image->colorFormat(), image->type(), image->data());
    }
};
template<> struct AbstractTexture::DataHelper<2> {
    enum class Target: GLenum {
        Texture2D = GL_TEXTURE_2D,
        Array1D = GL_TEXTURE_1D_ARRAY,
        Rectangle = GL_TEXTURE_RECTANGLE,
        CubeMap = GL_TEXTURE_CUBE_MAP,
        CubeMapPositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        CubeMapNegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        CubeMapPositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        CubeMapNegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        CubeMapPositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        CubeMapNegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    inline constexpr static Target target() { return Target::Texture2D; }

    inline static void setWrapping(Target target, const Math::Vector<Wrapping, 2>& wrapping) {
        glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping.at(0)));
        glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping.at(1)));
    }

    inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, 2>& dimensions, ColorFormat colorFormat, Type type, const void* data) {
        glTexImage2D(static_cast<GLenum>(target), mipLevel, static_cast<GLint>(internalFormat), dimensions.at(0), dimensions.at(1), 0, static_cast<GLenum>(colorFormat), static_cast<GLenum>(type), data);
    }

    template<class T> inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, T* image) {
        set(target, mipLevel, internalFormat, image->dimensions(), image->colorFormat(), image->type(), image->data());
    }

    inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, 2>& offset, const Math::Vector<GLsizei, 2>& dimensions, ColorFormat colorFormat, Type type, const void* data) {
        glTexSubImage2D(static_cast<GLenum>(target), mipLevel, offset.at(0), offset.at(1), dimensions.at(0), dimensions.at(1), static_cast<GLenum>(colorFormat), static_cast<GLenum>(type), data);
    }

    template<class T> inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, 2>& offset, T* image) {
        setSub(target, mipLevel, offset, image->dimensions(), image->colorFormat(), image->type(), image->data());
    }
};
template<> struct AbstractTexture::DataHelper<3> {
    enum class Target: GLenum {
        Texture3D = GL_TEXTURE_3D,
        Array2D = GL_TEXTURE_2D_ARRAY
    };

    inline constexpr static Target target() { return Target::Texture3D; }

    inline static void setWrapping(Target target, const Math::Vector<Wrapping, 3>& wrapping) {
        glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping.at(0)));
        glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping.at(1)));
        glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapping.at(2)));
    }

    inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, 3>& dimensions, ColorFormat colorFormat, Type type, const void* data) {
        glTexImage3D(static_cast<GLenum>(target), mipLevel, static_cast<GLint>(internalFormat), dimensions.at(0), dimensions.at(1), dimensions.at(2), 0, static_cast<GLenum>(colorFormat), static_cast<GLenum>(type), data);
    }

    template<class T> inline static void set(Target target, GLint mipLevel, InternalFormat internalFormat, T* image) {
        set(target, mipLevel, internalFormat, image->dimensions(), image->colorFormat(), image->type(), image->data());
    }

    inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, 3>& offset, const Math::Vector<GLsizei, 3>& dimensions, ColorFormat colorFormat, Type type, const void* data) {
        glTexSubImage3D(static_cast<GLenum>(target), mipLevel, offset.at(0), offset.at(1), offset.at(2), dimensions.at(0), dimensions.at(1), dimensions.at(2), static_cast<GLenum>(colorFormat), static_cast<GLenum>(type), data);
    }

    template<class T> inline static void setSub(Target target, GLint mipLevel, const Math::Vector<GLint, 3>& offset, T* image) {
        setSub(target, mipLevel, offset, image->dimensions(), image->colorFormat(), image->type(), image->data());
    }
};
#endif

}

#endif
