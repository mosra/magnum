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
         * @brief Constructor
         * @param layer     Texture layer (number between 0 and 31)
         *
         * Creates texture with target @c GL_TEXTURE_CUBE_MAP.
         */
        inline CubeMapTexture(GLint layer = 0): Texture2D(layer, GL_TEXTURE_CUBE_MAP) {}

        /** @brief Deleted. Use setData(Coordinate, GLint, InternalFormat, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*) instead. */
        template<class T> inline void setData(GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) = delete;

        /** @brief Deleted. Use setData(Coordinate, GLint, InternalFormat, const Trade::Image<Dimensions>*) instead. */
        void setData(GLint mipLevel, InternalFormat internalFormat, const Trade::Image<Dimensions>* image) = delete;

        /** @brief Deleted. Use @ref setSubData(Coordinate, GLint, const Math::Vector<GLint, Dimensions>&, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*) "setSubDataPositiveX()" and others instead. */
        template<class T> inline void setSubData(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) = delete;

        /** @brief Deleted. Use @ref setSubData(Coordinate, GLint, const Math::Vector<GLint, Dimensions>&, const Trade::Image<Dimensions>*) "setSubDataPositiveX()" and others instead. */
        void setSubData(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Trade::Image<Dimensions>* image) = delete;

        /**
         * @copydoc Texture::setData(GLint, InternalFormat, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         * @param coordinate   Coordinate
         */
        template<class T> inline void setData(Coordinate coordinate, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(coordinate, mipLevel, internalFormat, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @copydetails Texture::setData(GLint, InternalFormat, const Trade::Image<Dimensions>*)
         * @param coordinate   Coordinate
         */
        inline void setData(Coordinate coordinate, GLint mipLevel, InternalFormat internalFormat, const Trade::Image<Dimensions>* image) {
            setData(coordinate, mipLevel, internalFormat, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @copydoc Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         * @param coordinate   Coordinate
         */
        template<class T> inline void setSubData(Coordinate coordinate, GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setSubData(coordinate, mipLevel, offset, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @copydoc Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Trade::Image<Dimensions>*)
         * @param coordinate   Coordinate
         */
        inline void setSubData(Coordinate coordinate, GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Trade::Image<Dimensions>* image) {
            setSubData(coordinate, mipLevel, offset, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

    private:
        void setData(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, Type type, const void* data) {
            bind();
            DataHelper<Dimensions>::set(target, mipLevel, internalFormat, _dimensions, colorFormat, type, data);
        }

        void setSubData(GLenum target, GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& dimensions, ColorFormat colorFormat, Type type, const void* data) {
            bind();
            DataHelper<Dimensions>::setSub(target, mipLevel, offset, dimensions, colorFormat, type, data);
        }
};

}

#endif
