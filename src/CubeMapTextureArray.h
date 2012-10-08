#ifndef Magnum_CubeMapTextureArray_h
#define Magnum_CubeMapTextureArray_h
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
 * @brief Class Magnum::CubeMapTextureArray
 */

#include "Texture.h"

namespace Magnum {

/**
@brief Cube map texture array

For information about usage, see CubeMapTexture documentation.

When using cube map texture in the shader, use `samplerCubeArray`. Unlike
classic textures, coordinates for cube map textures is signed three-part
vector from the center of the cube, which intersects one of the six sides of
the cube map.

@see CubeMapTexture::setSeamless()
@requires_gl40 Extension @extension{ARB,texture_cube_map_array}
*/
class CubeMapTextureArray: public AbstractTexture {
    public:
        /** @brief Cube map coordinate */
        enum Coordinate: GLsizei {
            PositiveX = 0,  /**< +X cube side */
            NegativeX = 1,  /**< -X cube side */
            PositiveY = 2,  /**< +Y cube side */
            NegativeY = 3,  /**< -Y cube side */
            PositiveZ = 4,  /**< +Z cube side */
            NegativeZ = 5   /**< -Z cube side */
        };

        /**
         * @brief Constructor
         *
         * Creates one cube map OpenGL texture.
         * @see @def_gl{TEXTURE_CUBE_MAP_ARRAY}
         */
        inline CubeMapTextureArray(): AbstractTexture(GL_TEXTURE_CUBE_MAP_ARRAY) {}

        /**
         * @copydoc Texture::setWrapping()
         */
        inline CubeMapTextureArray* setWrapping(const Math::Vector<3, Wrapping>& wrapping) {
            bind();
            DataHelper<3>::setWrapping(GL_TEXTURE_CUBE_MAP_ARRAY, wrapping);
            return this;
        }

        /**
         * @copydoc Texture::setData(GLint, InternalFormat, Image*)
         *
         * Sets texture data from three-dimensional image for all cube faces
         * for all layers. Each group of 6 2D images is one cube map layer.
         * The images are ordered the same way as Coordinate enum.
         */
        template<class T> inline CubeMapTextureArray* setData(GLint mipLevel, InternalFormat internalFormat, T* image) {
            bind();
            DataHelper<3>::set(GL_TEXTURE_CUBE_MAP_ARRAY, mipLevel, internalFormat, image);
            return this;
        }

        /**
         * @brief Set texture subdata from 3D image
         * @param mipLevel      Mip level
         * @param offset        Offset where to put data in the texture
         * @param image         Three-dimensional Image, BufferedImage or for
         *      example Trade::ImageData
         * @return Pointer to self (for method chaining)
         *
         * Sets texture subdata from given image. The image is not deleted
         * afterwards.
         *
         * Z coordinate of @p offset specifies layer and cube map face. If
         * you want to start at given face in layer *n*, you have to specify
         * Z coordinate as @f$ 6n + i @f$, where i is face index as specified
         * in Coordinate enum.
         *
         * @see setSubData(GLsizei, Coordinate, GLint, const Math::Vector<2, GLint>&, const Image*)
         */
        template<class Image> inline CubeMapTextureArray* setSubData(GLint mipLevel, const Math::Vector<3, GLint>& offset, const Image* image) {
            bind();
            DataHelper<3>::setSub(GL_TEXTURE_CUBE_MAP_ARRAY, mipLevel, offset, image, Math::Vector<3, GLsizei>(Math::Vector<Image::Dimensions, GLsizei>()));
            return this;
        }

        /**
         * @brief Set texture subdata from 2D image
         * @param layer         Array layer
         * @param coordinate    Coordinate
         * @param mipLevel      Mip level
         * @param offset        Offset where to put data in the texture
         * @param image         Two-dimensional Image, BufferedImage or for
         *      example Trade::ImageData
         * @return Pointer to self (for method chaining)
         *
         * Sets texture subdata from given image. The image is not deleted
         * afterwards.
         *
         * @see setSubData(GLint, const Math::Vector<3, GLint>&, const Image*)
         */
        template<class Image> inline CubeMapTextureArray* setSubData(GLsizei layer, Coordinate coordinate, GLint mipLevel, const Math::Vector<2, GLint>& offset, const Image* image) {
            bind();
            DataHelper<3>::setSub(GL_TEXTURE_CUBE_MAP_ARRAY, mipLevel, Math::Vector<3, GLint>(offset, layer*6+static_cast<GLsizei>(coordinate)), image, Math::Vector<2, GLsizei>(Math::Vector<Image::Dimensions, GLsizei>()));
            return this;
        }
};

}

#endif
