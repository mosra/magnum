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
@todo The wrap mode is 3D, not 2D! http://www.opengl.org/wiki/Common_Mistakes#Creating_a_Cubemap_Texture
@todo Cube map arrays (OpenGL 4.0, ARB_texture_cube_map_array)
*/
class CubeMapTexture: public Texture2D {
    public:
        /** @brief Cube map coordinate */
        enum Coordinate: GLenum {
            PositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            NegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            PositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            NegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            PositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            NegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
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
         * Creates texture with target Target::CubeMap.
         */
        inline CubeMapTexture(GLint layer = 0): Texture(layer, Target::CubeMap) {}

        template<class T> void setData(GLint mipLevel, InternalFormat internalFormat, T* image) = delete;
        template<class T> void setSubData(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, T* image) = delete;

        /**
         * @copydetails Texture::setData(GLint, InternalFormat, T*)
         * @param coordinate   Coordinate
         */
        template<class T> inline void setData(Coordinate coordinate, GLint mipLevel, InternalFormat internalFormat, T* image) {
            bind();
            DataHelper<Dimensions>::set(static_cast<Target>(coordinate), mipLevel, internalFormat, image);
        }

        /**
         * @copydoc Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, T*)
         * @param coordinate   Coordinate
         */
        template<class T> inline void setSubData(Coordinate coordinate, GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const T* image) {
            bind();
            DataHelper<Dimensions>::setSub(static_cast<Target>(coordinate), mipLevel, offset, image);
        }
};

}

#endif
