#ifndef Magnum_AbstractTexture_h
#define Magnum_AbstractTexture_h
/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

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
#include "Image.h"

namespace Magnum {

/**
@brief Non-templated base for Texture

See Texture documentation for more information.
*/
class AbstractTexture {
    DISABLE_COPY(AbstractTexture)

    public:
        /** @brief Texture filtering */
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

        /** @brief Texture wrapping on the edge */
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
            Red = GL_RED,
            RedGreen = GL_RG,
            RGB = GL_RGB,
            RGBA = GL_RGBA,
            BGR = GL_BGR,
            BGRA = GL_BGRA,
            CompressedRed = GL_COMPRESSED_RED,
            CompressedRedGreen = GL_COMPRESSED_RG,
            CompressedRGB = GL_COMPRESSED_RGB,
            CompressedRGBA = GL_COMPRESSED_RGBA
        };

        /**
         * @brief Constructor
         * @param layer     %Texture layer (number between 0 and 31)
         * @param target    Target, e.g. @c GL_TEXTURE_2D.
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

        /** @brief Texture layer */
        inline GLint layer() const { return _layer; }

        /** @brief Bind texture for usage / rendering */
        inline void bind() const {
            glActiveTexture(GL_TEXTURE0 + _layer);
            glBindTexture(target, texture);
        }

        /**
         * @brief Unbind texture
         *
         * @note Unbinds any texture from given dimension, not only this
         * particular one.
         */
        inline void unbind() const {
            glActiveTexture(GL_TEXTURE0 + _layer);
            glBindTexture(target, 0);
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
            unbind();
        }

        /**
         * @brief Set border color
         *
         * Border color when @ref AbstractTexture::Wrapping "wrapping" is set
         * to @c ClampToBorder.
         */
        inline void setBorderColor(const Vector4& color) {
            bind();
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, color.data());
            unbind();
        }

        /**
         * @brief Generate mipmap
         *
         * Does nothing on rectangle textures.
         * @see setMinificationFilter()
         */
        void generateMipmap();

    protected:
        const GLenum target;        /**< @brief Texture target */

        /**
         * @brief Helper for setting texture data
         *
         * Workaround for partial template specialization.
         */
        template<size_t textureDimensions> struct DataHelper {
            #ifdef DOXYGEN_GENERATING_OUTPUT
            /**
             * @brief Target for given dimension
             *
             * Returns @c GL_TEXTURE_1D, @c GL_TEXTURE_2D or @c GL_TEXTURE_3D
             * based on dimension count.
             */
            inline constexpr static GLenum target();

            /**
             * @brief Set texture data
             * @param target            Target, such as @c GL_TEXTURE_RECTANGLE
             * @param mipLevel          Mip level
             * @param internalFormat    Internal texture format
             * @param dimensions        %Texture dimensions
             * @param colorFormat       Color format of passed data. Data size
             *      per color channel is detected from format of passed data
             *      array.
             * @param data              %Texture data
             *
             * Calls @c glTexImage1D, @c glTexImage2D, @c glTexImage3D depending
             * on dimension count.
             */
            template<class T> inline static void set(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, textureDimensions>& dimensions, ColorFormat colorFormat, const T* data);

            /**
             * @brief Set texture subdata
             * @param target            Target, such as @c GL_TEXTURE_RECTANGLE
             * @param mipLevel          Mip level
             * @param offset            Offset where to put data in the texture
             * @param dimensions        %Texture dimensions
             * @param colorFormat       Color format of passed data. Data size
             *      per color channel is detected from format of passed data
             *      array.
             * @param data              %Texture data
             *
             * Calls @c glTexSubImage1D, @c glTexSubImage2D, @c glTexSubImage3D
             * depending on dimension count.
             */
            template<class T> inline static void setSub(GLenum target, GLint mipLevel, const Math::Vector<GLsizei, textureDimensions>& offset, const Math::Vector<GLsizei, textureDimensions>& dimensions, ColorFormat colorFormat, const T* data);
            #endif
        };

    private:
        const GLint _layer;
        GLuint texture;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct AbstractTexture::DataHelper<1> {
    inline constexpr static GLenum target() { return GL_TEXTURE_1D; }

    template<class T> inline static void set(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, 1>& dimensions, ColorFormat colorFormat, const T* data) {
        glTexImage1D(target, mipLevel, static_cast<GLint>(internalFormat), dimensions.at(0), 0, static_cast<GLenum>(colorFormat), TypeTraits<T>::glType(), data);
    }

    template<class T> inline static void setSub(GLenum target, GLint mipLevel, const Math::Vector<GLsizei, 1>& offset, const Math::Vector<GLsizei, 1>& dimensions, ColorFormat colorFormat, const T* data) {
        glTexSubImage1D(target, mipLevel, offset.at(0), dimensions.at(0), static_cast<GLenum>(colorFormat), TypeTraits<T>::glType(), data);
    }
};
template<> struct AbstractTexture::DataHelper<2> {
    inline constexpr static GLenum target() { return GL_TEXTURE_2D; }

    template<class T> inline static void set(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, 2>& dimensions, ColorFormat colorFormat, const T* data) {
        glTexImage2D(target, mipLevel, static_cast<GLint>(internalFormat), dimensions.at(0), dimensions.at(1), 0, static_cast<GLenum>(colorFormat), TypeTraits<T>::glType(), data);
    }

    template<class T> inline static void setSub(GLenum target, GLint mipLevel, const Math::Vector<GLsizei, 2>& offset, const Math::Vector<GLsizei, 2>& dimensions, ColorFormat colorFormat, const T* data) {
        glTexSubImage2D(target, mipLevel, offset.at(0), offset.at(1), dimensions.at(0), dimensions.at(1), static_cast<GLenum>(colorFormat), TypeTraits<T>::glType(), data);
    }
};
template<> struct AbstractTexture::DataHelper<3> {
    inline constexpr static GLenum target() { return GL_TEXTURE_3D; }

    template<class T> inline static void set(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, 3>& dimensions, ColorFormat colorFormat, const T* data) {
        glTexImage3D(target, mipLevel, static_cast<GLint>(internalFormat), dimensions.at(0), dimensions.at(1), dimensions.at(2), 0, static_cast<GLenum>(colorFormat), TypeTraits<T>::glType(), data);
    }

    template<class T> inline static void setSub(GLenum target, GLint mipLevel, const Math::Vector<GLsizei, 2>& offset, const Math::Vector<GLsizei, 2>& dimensions, ColorFormat colorFormat, const T* data) {
        glTexSubImage3D(target, mipLevel, offset.at(0), offset.at(1), offset.at(2), dimensions.at(0), dimensions.at(1), dimensions.at(2), static_cast<GLenum>(colorFormat), TypeTraits<T>::glType(), data);
    }
};
#endif

}

#endif
