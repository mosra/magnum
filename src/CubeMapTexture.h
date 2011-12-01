#ifndef Magnum_CubeMapTexture_h
#define Magnum_CubeMapTexture_h
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
 * @brief Cube map texture
 */

#include "Texture.h"

namespace Magnum {

/**
@brief Cube map texture

%Texture used mainly for environemnt maps, consisting of 6 square textures
generating 6 faces of the cube as following. Note that all images must be
turned upside down (+Y is top):
<pre>
          +----+
          | -Y |
+----+----+----+----+
| -Z | -X | +Z | +X |
+----+----+----+----+
          | +Y |
          +----+
</pre>
When using cube map texture in the shader, use @c samplerCube. Unlike normal
textures, coordinates for cube map textures is signed three-part vector from
the center of the cube, which intersects one of the six sides of the cube map.

See Texture documentation for more information about usage.
*/
class CubeMapTexture: public Texture2D {
    public:
        /**
         * @brief Constructor
         *
         * Creates texture with target @c GL_TEXTURE_CUBE_MAP.
         */
        inline CubeMapTexture(): Texture2D(GL_TEXTURE_CUBE_MAP) {}

    protected:
        /** @brief Deleted. Use setDataPositiveX() and others instead. */
        template<class T> inline void setData(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) = delete;

        /**
         * @brief Set texture data for positive X
         *
         * @copydetails Texture::setData()
         */
        template<class T> inline void setDataPositiveX(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipLevel, internalFormat, _dimensions, colorFormat, data);
        }

        /**
         * @brief Set texture data for negative X
         *
         * @copydetails Texture::setData()
         */
        template<class T> inline void setDataNegativeX(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipLevel, internalFormat, _dimensions, colorFormat, data);
        }

        /**
         * @brief Set texture data for positive Y
         *
         * @copydetails Texture::setData()
         */
        template<class T> inline void setDataPositiveY(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipLevel, internalFormat, _dimensions, colorFormat, data);
        }

        /**
         * @brief Set texture data for negative Y
         *
         * @copydetails Texture::setData()
         */
        template<class T> inline void setDataNegativeY(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipLevel, internalFormat, _dimensions, colorFormat, data);
        }

        /**
         * @brief Set texture data for positive Z
         *
         * @copydetails Texture::setData()
         */
        template<class T> inline void setDataPositiveZ(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipLevel, internalFormat, _dimensions, colorFormat, data);
        }

        /**
         * @brief Set texture data for negative Z
         *
         * @copydetails Texture::setData()
         */
        template<class T> inline void setDataNegativeZ(GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipLevel, internalFormat, _dimensions, colorFormat, data);
        }

    private:
        template<class T> void setData(GLenum target, GLint mipLevel, int internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            bind();
            DataHelper<Dimensions>::template set<typename TypeTraits<T>::TextureType>(target, mipLevel, internalFormat, _dimensions, colorFormat, data);
            unbind();
        }
};

}

#endif
