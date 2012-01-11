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
        /**
         * @brief Constructor
         * @param layer     Texture layer (number between 0 and 31)
         *
         * Creates texture with target @c GL_TEXTURE_CUBE_MAP.
         */
        inline CubeMapTexture(GLint layer = 0): Texture2D(layer, GL_TEXTURE_CUBE_MAP) {}

        /** @brief Deleted. Use @ref setDataPositiveX(GLint, InternalFormat, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*) "setDataPositiveX()" and others instead. */
        template<class T> inline void setData(GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) = delete;

        /** @brief Deleted. Use @ref setDataPositiveX(GLint, InternalFormat, const Image<Dimensions>*) "setDataPositiveX()" and others instead. */
        void setData(GLint mipLevel, InternalFormat internalFormat, const Image<Dimensions>* image) = delete;

        /** @brief Deleted. Use @ref setSubDataPositiveX(GLint, const Math::Vector<GLint, Dimensions>&, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*) "setSubDataPositiveX()" and others instead. */
        template<class T> inline void setSubData(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) = delete;

        /** @brief Deleted. Use @ref setSubDataPositiveX(GLint, const Math::Vector<GLint, Dimensions>&, const Image<Dimensions>*) "setSubDataPositiveX()" and others instead. */
        void setSubData(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Image<Dimensions>* image) = delete;

        /**
         * @brief Set texture data for positive X
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setDataPositiveX(GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipLevel, internalFormat, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture data for positive X
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Image<Dimensions>*)
         */
        inline void setDataPositiveX(GLint mipLevel, InternalFormat internalFormat, const Image<Dimensions>* image) {
            setData(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipLevel, internalFormat, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture subdata for positive X
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setSubDataPositiveX(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setSubData(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipLevel, offset, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture subdata for positive X
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Image<Dimensions>*)
         */
        inline void setSubDataPositiveX(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Image<Dimensions>* image) {
            setSubData(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipLevel, offset, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture data for negative X
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setDataNegativeX(GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipLevel, internalFormat, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture data for negative X
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Image<Dimensions>*)
         */
        inline void setDataNegativeX(GLint mipLevel, InternalFormat internalFormat, const Image<Dimensions>* image) {
            setData(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipLevel, internalFormat, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture subdata for negative X
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setSubDataNegativeX(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setSubData(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipLevel, offset, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture subdata for negative X
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Image<Dimensions>*)
         */
        inline void setSubDataNegativeX(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Image<Dimensions>* image) {
            setSubData(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipLevel, offset, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture data for positive Y
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setDataPositiveY(GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipLevel, internalFormat, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture data for positive Y
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Image<Dimensions>*)
         */
        inline void setDataPositiveY(GLint mipLevel, InternalFormat internalFormat, const Image<Dimensions>* image) {
            setData(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipLevel, internalFormat, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture subdata for positive Y
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setSubDataPositiveY(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setSubData(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipLevel, offset, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture subdata for positive Y
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Image<Dimensions>*)
         */
        inline void setSubDataPositiveY(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Image<Dimensions>* image) {
            setSubData(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipLevel, offset, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture data for negative Y
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setDataNegativeY(GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipLevel, internalFormat, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture data for negative Y
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Image<Dimensions>*)
         */
        inline void setDataNegativeY(GLint mipLevel, InternalFormat internalFormat, const Image<Dimensions>* image) {
            setData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipLevel, internalFormat, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture subdata for negative Y
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setSubDataNegativeY(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setSubData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipLevel, offset, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture subdata for negative Y
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Image<Dimensions>*)
         */
        inline void setSubDataNegativeY(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Image<Dimensions>* image) {
            setSubData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipLevel, offset, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture data for positive Z
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setDataPositiveZ(GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipLevel, internalFormat, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture data for positive Z
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Image<Dimensions>*)
         */
        inline void setDataPositiveZ(GLint mipLevel, InternalFormat internalFormat, const Image<Dimensions>* image) {
            setData(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipLevel, internalFormat, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture subdata for positive Z
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setSubDataPositiveZ(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setSubData(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipLevel, offset, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture subdata for positive Z
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Image<Dimensions>*)
         */
        inline void setSubDataPositiveZ(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Image<Dimensions>* image) {
            setSubData(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipLevel, offset, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture data for negative Z
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setDataNegativeZ(GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipLevel, internalFormat, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture data for negative Z
         *
         * @copydetails Texture::setData(GLint, InternalFormat, const Image<Dimensions>*)
         */
        inline void setDataNegativeZ(GLint mipLevel, InternalFormat internalFormat, const Image<Dimensions>* image) {
            setData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipLevel, internalFormat, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

        /**
         * @brief Set texture subdata for negative Z
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Math::Vector<GLsizei, Dimensions>&, ColorFormat, const T*)
         */
        template<class T> inline void setSubDataNegativeZ(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, const T* data) {
            setSubData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipLevel, offset, _dimensions, colorFormat, TypeTraits<typename TypeTraits<T>::TextureType>::glType(), data);
        }

        /**
         * @brief Set texture subdata for negative Z
         *
         * @copydetails Texture::setSubData(GLint, const Math::Vector<GLint, Dimensions>&, const Image<Dimensions>*)
         */
        inline void setSubDataNegativeZ(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Image<Dimensions>* image) {
            setSubData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipLevel, offset, image->dimensions(), image->colorFormat(), image->type(), image->data());
        }

    private:
        void setData(GLenum target, GLint mipLevel, InternalFormat internalFormat, const Math::Vector<GLsizei, Dimensions>& _dimensions, ColorFormat colorFormat, GLenum type, const void* data) {
            bind();
            DataHelper<Dimensions>::set(target, mipLevel, internalFormat, _dimensions, colorFormat, type, data);
            unbind();
        }

        void setSubData(GLenum target, GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, const Math::Vector<GLsizei, Dimensions>& dimensions, ColorFormat colorFormat, GLenum type, const void* data) {
            bind();
            DataHelper<Dimensions>::setSub(target, mipLevel, offset, dimensions, colorFormat, type, data);
            unbind();
        }
};

}

#endif
