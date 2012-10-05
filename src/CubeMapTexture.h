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

%Texture used mainly for environemnt maps. See AbstractTexture documentation
for more information about usage. It consists of 6 square textures generating
6 faces of the cube as following. Note that all images must be turned upside
down (+Y is top):

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

See AbstractTexture documentation for more information about usage.

@see CubeMapTextureArray
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

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Enable/disable seamless cube map textures
         *
         * Initially disabled on desktop OpenGL.
         * @see @fn_gl{Enable}/@fn_gl{Disable} with @def_gl{TEXTURE_CUBE_MAP_SEAMLESS}
         * @requires_gl Not available in OpenGL ES 2.0, always enabled in
         *      OpenGL ES 3.0.
         * @requires_gl32 Extension @extension{ARB,seamless_cube_map}
         */
        inline static void setSeamless(bool enabled) {
            enabled ? glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS) : glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        }
        #endif

        /**
         * @brief Constructor
         *
         * Creates one cube map OpenGL texture.
         * @see @def_gl{TEXTURE_CUBE_MAP}
         */
        inline CubeMapTexture(): AbstractTexture(GL_TEXTURE_CUBE_MAP) {}

        /**
         * @copydoc Texture::setWrapping()
         */
        inline CubeMapTexture* setWrapping(const Math::Vector<3, Wrapping>& wrapping) {
            bind();
            DataHelper<3>::setWrapping(GL_TEXTURE_CUBE_MAP, wrapping);
            return this;
        }

        /**
         * @copydoc Texture::setData(GLint, InternalFormat, Image*)
         * @param coordinate    Coordinate
         * @return Pointer to self (for method chaining)
         */
        template<class Image> inline CubeMapTexture* setData(Coordinate coordinate, GLint mipLevel, InternalFormat internalFormat, Image* image) {
            bind();
            DataHelper<2>::set(static_cast<GLenum>(coordinate), mipLevel, internalFormat, image);
            return this;
        }

        /**
         * @copydoc Texture::setSubData(GLint, const typename DimensionTraits<Dimensions, GLint>::VectorType&, Image*)
         * @param coordinate    Coordinate
         * @return Pointer to self (for method chaining)
         */
        template<class Image> inline CubeMapTexture* setSubData(Coordinate coordinate, GLint mipLevel, const Math::Vector<2, GLint>& offset, const Image* image) {
            bind();
            DataHelper<2>::setSub(static_cast<GLenum>(coordinate), mipLevel, offset, image);
            return this;
        }
};

}

#endif
