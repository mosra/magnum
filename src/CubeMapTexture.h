#ifndef Magnum_CubeMapTexture_h
#define Magnum_CubeMapTexture_h
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
 * @brief Class Magnum::CubeMapTexture
 */

#include "Texture.h"

namespace Magnum {

/**
@brief Cube map texture

%Texture used mainly for environemnt maps, consisting of 6 square textures
generating 6 faces of the cube as following. Note that all images must be
turned upside down (+Y is top):

              +----+
              | -Y |
    +----+----+----+----+
    | -Z | -X | +Z | +X |
    +----+----+----+----+
              | +Y |
              +----+

When using cube map texture in the shader, use `samplerCube`. Unlike classic
textures, coordinates for cube map textures is signed three-part vector from
the center of the cube, which intersects one of the six sides of the cube map.

See Texture documentation for more information about usage.
@todo Cube map arrays (OpenGL 4.0, ARB_texture_cube_map_array)
*/
class CubeMapTexture: public AbstractTexture {
    public:
        /** @brief Cube map coordinate */
        enum Coordinate: GLenum {
            PositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,     /**< +X cube side */
            NegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,     /**< -X cube side */
            PositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,     /**< +Y cube side */
            NegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,     /**< -Y cube side */
            PositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,     /**< +Z cube side */
            NegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z      /**< -Z cube side */
        };

        /**
         * @brief Enable/disable seamless cube map textures
         *
         * @requires_gl32 Extension <tt>ARB_seamless_cube_map</tt>
         */
        inline static void setSeamless(bool enabled) {
            enabled ? glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS) : glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        }

        /**
         * @brief Constructor
         * @param layer     Texture layer (number between 0 and 31)
         *
         * Creates one cube map OpenGL texture.
         */
        inline CubeMapTexture(GLint layer = 0): AbstractTexture(layer, GL_TEXTURE_CUBE_MAP) {}

        /**
         * @copydoc Texture::setWrapping()
         */
        inline void setWrapping(const Math::Vector<3, Wrapping>& wrapping) {
            bind();
            DataHelper<3>::setWrapping(GL_TEXTURE_CUBE_MAP, wrapping);
        }

        /**
         * @copydoc Texture::setData(GLint, InternalFormat, Image*)
         * @param coordinate    Coordinate
         */
        template<class Image> inline void setData(Coordinate coordinate, GLint mipLevel, InternalFormat internalFormat, Image* image) {
            bind();
            DataHelper<2>::set(static_cast<GLenum>(coordinate), mipLevel, internalFormat, image);
        }

        /**
         * @copydoc Texture::setSubData(GLint, const Math::Vector<Dimensions, GLint>&, Image*)
         * @param coordinate    Coordinate
         */
        template<class Image> inline void setSubData(Coordinate coordinate, GLint mipLevel, const Math::Vector<2, GLint>& offset, const Image* image) {
            bind();
            DataHelper<2>::setSub(static_cast<GLenum>(coordinate), mipLevel, offset, image);
        }
};

}

#endif
