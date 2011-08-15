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

namespace Magnum {

/**
 * @brief Texture
 *
 * Template class for one- to three-dimensional textures. Recommended usage is
 * via subclassing and setting texture data from e.g. constructor with
 * setData().
 */
template<size_t dimensions> class Texture {
    public:
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
         *
         * Creates one OpenGL texture.
         */
        inline Texture(): target(GL_TEXTURE_1D + dimensions - 1) {
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
            glBindTexture(dimensions, texture);
        }

        /**
         * @brief Unbind texture
         *
         * @note Unbinds any texture from given dimension, not only this
         * particular one.
         */
        inline void unbind() const {
            glBindTexture(dimensions, texture);
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
        void setWrapping(const Math::Vector<Wrapping, dimensions>& wrapping) {
            bind();
            for(int i = 0; i != dimensions; ++i) switch(i) {
                case 0:
                    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapping.at(i));
                    break;
                case 1:
                    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapping.at(i));
                    break;
                case 2:
                    glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapping.at(i));
                    break;
            }
            unbind();
        }

        /**
         * @brief Set border color
         *
         * Border color when wrapping is set to ClampToBorder.
         */
        void setBorderColor(const Vector4& color) {
            bind();
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, color.data());
            unbind();
        }

        /**
         * @brief Generate mipmap
         *
         * @see setMinificationFilter()
         */
        void generateMipmap() {
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
        inline void setData(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, dimensions>& _dimensions, ColorFormat colorFormat, const GLubyte* data) {
            setData(mipLevel, internalFormat, _dimensions, colorFormat, GL_UNSIGNED_BYTE, data);
        }

        /** @copydoc setData() */
        inline void setData(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, dimensions>& _dimensions, ColorFormat colorFormat, const GLbyte* data) {
            setData(mipLevel, internalFormat, _dimensions, colorFormat, GL_BYTE, data);
        }

        /** @copydoc setData() */
        inline void setData(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, dimensions>& _dimensions, ColorFormat colorFormat, const GLushort* data) {
            setData(mipLevel, internalFormat, _dimensions, colorFormat, GL_UNSIGNED_SHORT, data);
        }

        /** @copydoc setData() */
        inline void setData(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, dimensions>& _dimensions, ColorFormat colorFormat, const GLshort* data) {
            setData(mipLevel, internalFormat, _dimensions, colorFormat, GL_SHORT, data);
        }

        /** @copydoc setData() */
        inline void setData(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, dimensions>& _dimensions, ColorFormat colorFormat, const GLuint* data) {
            setData(mipLevel, internalFormat, _dimensions, colorFormat, GL_UNSIGNED_INT, data);
        }

        /** @copydoc setData() */
        inline void setData(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, dimensions>& _dimensions, ColorFormat colorFormat, const GLint* data) {
            setData(mipLevel, internalFormat, _dimensions, colorFormat, GL_INT, data);
        }

        /** @copydoc setData() */
        inline void setData(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, dimensions>& _dimensions, ColorFormat colorFormat, const GLfloat* data) {
            setData(mipLevel, internalFormat, _dimensions, colorFormat, GL_FLOAT, data);
        }

    private:
        GLuint texture;
        const GLenum target;

        void setData(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, dimensions>& _dimensions, ColorFormat colorFormat, GLenum pixelFormat, const GLvoid* data) {
            bind();
            if(dimensions == 1)
                glTexImage1D(target, mipLevel, internalFormat, _dimensions.at(0), 0, colorFormat, pixelFormat, data);
            else if(dimensions == 2)
                glTexImage2D(target, mipLevel, internalFormat, _dimensions.at(0), _dimensions.at(1), 0, colorFormat, pixelFormat, data);
            else if(dimensions == 3)
                glTexImage3D(target, mipLevel, internalFormat, _dimensions.at(0), _dimensions.at(1), _dimensions.at(2), 0, colorFormat, pixelFormat, data);
            unbind();
        }
};

/** @brief One-dimensional texture */
typedef Texture<1> Texture1D;

/** @brief Two-dimensional texture */
typedef Texture<2> Texture2D;

/** @brief Three-dimensional texture */
typedef Texture<3> Texture3D;

}

#endif
