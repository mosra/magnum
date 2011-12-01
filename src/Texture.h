#ifndef Magnum_Texture_h
#define Magnum_Texture_h
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
 * @brief Class Magnum::Texture
 */

#include "Magnum.h"
#include "TypeTraits.h"

namespace Magnum {

/**
@brief Texture

Template class for one- to three-dimensional textures. Recommended usage is via
subclassing and setting texture data from e.g. constructor with setData().

@attention Don't forget to call setMinificationFilter() and
setMagnificationFilter() after creating the texture, otherwise it will be
unusable.
 */
template<size_t dimensions> class Texture {
    public:
        static const size_t Dimensions = dimensions;    /**< @brief Texture dimension count */

        /** @brief Texture filtering */
        enum Filter {
            /**
             * Nearest neighbor filtering
             */
            NearestNeighbor = GL_NEAREST,

            /**
             * Linear filtering
             */
            Linear = GL_LINEAR
        };

        /** @brief Mip level selection */
        enum Mipmap {
            /**
             * Select base mipmap level.
             */
            BaseLevel = 0,

            /**
             * Select nearest mip level.
             */
            NearestLevel = 0x100,

            /**
             * Linear interpolation of nearest mip levels.
             */
            LinearInterpolation = 0x102
        };

        /** @brief Texture wrapping on the edge */
        enum Wrapping {
            /**
             * Repeat texture
             */
            Repeat = GL_REPEAT,

            /**
             * Repeat mirrored texture
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
        enum InternalFormat {
            CompressedRed = GL_COMPRESSED_RED,
            CompressedRedGreen = GL_COMPRESSED_RG,
            CompressedRGB = GL_COMPRESSED_RGB,
            CompressedRGBA = GL_COMPRESSED_RGBA
        };

        /** @brief Color format */
        enum ColorFormat {
            Red = GL_RED,
            RedGreen = GL_RG,
            RGB = GL_RGB,
            RGBA = GL_RGBA,
            BGR = GL_BGR,
            BGRA = GL_BGRA
        };

        /**
         * @brief Constructor
         * @param target    Target, e.g. GL_TEXTURE_2D. If not set, target is
         *      based on dimension count (GL_TEXTURE_1D, GL_TEXTURE_2D,
         *      GL_TEXTURE_3D).
         *
         * Creates one OpenGL texture.
         */
        inline Texture(GLenum target = GL_TEXTURE_1D + dimensions - 1): target(target) {
            glGenTextures(1, &texture);
        }

        /**
         * @brief Destructor
         *
         * Deletes assigned OpenGL texture.
         */
        inline virtual ~Texture() {
            glDeleteTextures(1, &texture);
        }

        /** @brief Bind texture for usage / rendering */
        inline void bind() const {
            glBindTexture(target, texture);
        }

        /**
         * @brief Unbind texture
         *
         * @note Unbinds any texture from given dimension, not only this
         * particular one.
         */
        inline void unbind() const {
            glBindTexture(target, 0);
        }

        /**
         * @brief Set minification filter
         * @param filter        Filter
         * @param mipmap        Mipmap filtering. If set to anything else than
         *      BaseLevel, make sure textures for all mip levels are set or call
         *      generateMipmap().
         *
         * Sets filter used when the object pixel size is smaller than the
         * texture size.
         * @attention This and setMagnificationFilter() must be called after
         * creating the texture, otherwise it will be unusable.
         * @see generateMipmap()
         */
        inline void setMinificationFilter(Filter filter, Mipmap mipmap = BaseLevel) {
            bind();
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter|mipmap);
            unbind();
        }

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
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
            unbind();
        }

        /**
         * @brief Set wrapping
         * @param wrapping      Wrapping type for all texture dimensions
         *
         * Sets wrapping type for coordinates out of range (-1, 1).
         */
        void setWrapping(const Math::Vector<Wrapping, dimensions>& wrapping);

        /**
         * @brief Set border color
         *
         * Border color when wrapping is set to ClampToBorder.
         */
        inline void setBorderColor(const Vector4& color) {
            bind();
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, color.data());
            unbind();
        }

        /**
         * @brief Generate mipmap
         *
         * @see setMinificationFilter()
         */
        inline void generateMipmap() {
            bind();
            glGenerateMipmap(target);
            unbind();
        }

    protected:
        /**
         * @brief Set texture data
         * @param mipLevel          Mip level
         * @param internalFormat    Internal texture format. One value from
         *      InternalFormat or ColorFormat enum.
         * @param _dimensions       Texture dimensions
         * @param colorFormat       Color format of passed data. Data size per
         *      color channel is detected from format of passed data array.
         * @param data              Texture data.
         */
        template<class T> inline void setData(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            bind();
            DataHelper<typename TypeTraits<T>::TextureType, dimensions>()(target, mipLevel, internalFormat, _dimensions, colorFormat, data);
            unbind();
        }

        /**
         * @brief Helper for setting texture data
         *
         * Workaround for partial partial template specialization, see
         * http://stackoverflow.com/questions/1757791/c-template-partial-specialization-specializing-one-member-function-only
         */
        template<class T, size_t textureDimensions> struct DataHelper {
            #ifdef DOXYGEN_GENERATING_OUTPUT
            /**
             * @brief Functor
             * @param target            Target, such as GL_TEXTURE_2D
             * @param mipLevel          Mip level
             * @param internalFormat    Internal texture format. One value from
             *      InternalFormat or ColorFormat enum.
             * @param _dimensions       Texture dimensions
             * @param colorFormat       Color format of passed data. Data size per
             *      color channel is detected from format of passed data array.
             * @param data              Texture data.
             *
             * Calls glTexImage1D, glTexImage2D, glTexImage3D depending on
             * dimension count.
             */
            inline void operator()(GLenum target, GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, 1>& _dimensions, ColorFormat colorFormat, const T* data);
            #endif
        };

        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class T> struct DataHelper<T, 1> {
            inline void operator()(GLenum target, GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, 1>& _dimensions, ColorFormat colorFormat, const T* data) {
                glTexImage1D(target, mipLevel, internalFormat, _dimensions.at(0), 0, colorFormat, TypeTraits<T>::glType(), data);
            }
        };
        template<class T> struct DataHelper<T, 2> {
            inline void operator()(GLenum target, GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, 2>& _dimensions, ColorFormat colorFormat, const T* data) {
                glTexImage2D(target, mipLevel, internalFormat, _dimensions.at(0), _dimensions.at(1), 0, colorFormat, TypeTraits<T>::glType(), data);
            }
        };
        template<class T> struct DataHelper<T, 3> {
            inline void operator()(GLenum target, GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, 3>& _dimensions, ColorFormat colorFormat, const T* data) {
                glTexImage3D(target, mipLevel, internalFormat, _dimensions.at(0), _dimensions.at(1), _dimensions.at(2), 0, colorFormat, TypeTraits<T>::glType(), data);
            }
        };
        #endif

    private:
        GLuint texture;
        const GLenum target;
};

/** @brief One-dimensional texture */
typedef Texture<1> Texture1D;

/** @brief Two-dimensional texture */
typedef Texture<2> Texture2D;

/** @brief Three-dimensional texture */
typedef Texture<3> Texture3D;

}

#endif
