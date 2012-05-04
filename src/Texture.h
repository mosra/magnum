#ifndef Magnum_Texture_h
#define Magnum_Texture_h
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
 * @brief Class Magnum::Texture
 */

#include "AbstractTexture.h"

namespace Magnum {

/**
@brief %Texture

Template class for one- to three-dimensional textures.

@attention Don't forget to call setWrapping(), setMinificationFilter() and
setMagnificationFilter() after creating the texture, otherwise the texture
will be incomplete.

The texture is bound via bind() and setting texture uniform on the shader to the
texture (see AbstractShaderProgram::setUniform(GLint, const AbstractTexture*)).
In shader, the texture is used via `sampler1D`, `sampler2D` or `sampler3D`
depending on dimension count. Note that you can have more than one texture bound
to the shader - the only requirement is to have each texture in another layer.

@section RectangleTextures Rectangle textures

If you want to use rectangle textures, set target in constructor to
`Target::Rectangle` and in shader use `sampler2DRect`. Unlike `sampler2D`,
which accepts coordinates between 0 and 1, `sampler2DRect` accepts coordinates
between 0 and `textureSizeInGivenDirection-1`. Note that rectangle textures
don't support mipmapping and repeating wrapping modes, see @ref Texture::Filter
"Filter", @ref Texture::Mipmap "Mipmap" and generateMipmap() documentation
for more information.
 */
template<size_t textureDimensions> class Texture: public AbstractTexture {
    public:
        static const size_t Dimensions = textureDimensions; /**< @brief %Texture dimension count */
        typedef typename DataHelper<Dimensions>::Target Target; /**< @brief %Texture target */

        /**
         * @brief Constructor
         * @param layer             %Texture layer (number between 0 and 31)
         * @param target            %Texture target
         *
         * Creates one OpenGL texture.
         */
        inline Texture(GLint layer = 0, Target target = DataHelper<Dimensions>::target()): AbstractTexture(layer, static_cast<GLenum>(target)), _target(target) {}

        /** @brief %Texture target */
        inline Target target() const { return _target; }

        /**
         * @brief Set wrapping
         * @param wrapping          Wrapping type for all texture dimensions
         *
         * Sets wrapping type for coordinates out of range (0, 1) for normal
         * textures and (0, textureSizeInGivenDirection-1) for rectangle
         * textures.
         * @attention This, setMinificationFilter() and
         * setMagnificationFilter() must be called after creating the texture,
         * otherwise the texture will be incomplete.
         * @attention For rectangle textures only some modes are supported,
         * see @ref AbstractTexture::Wrapping "Wrapping" documentation for
         * more information.
         */
        inline void setWrapping(const Math::Vector<Wrapping, Dimensions>& wrapping) {
            bind();
            DataHelper<Dimensions>::setWrapping(_target, wrapping);
        }

        /**
         * @brief Set texture data
         * @param mipLevel          Mip level
         * @param internalFormat    Internal texture format
         * @param image             Image, BufferedImage or for example
         *      Trade::ImageData of the same dimension count
         *
         * Sets texture data from given image. The image is not deleted
         * afterwards.
         */
        template<class T> inline void setData(GLint mipLevel, InternalFormat internalFormat, T* image) {
            bind();
            DataHelper<Dimensions>::set(_target, mipLevel, internalFormat, image);
        }

        /**
         * @brief Set texture subdata
         * @param mipLevel          Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             Image, BufferedImage or for example
         *      Trade::ImageData of the same dimension count
         *
         * Sets texture subdata from given image. The image is not deleted
         * afterwards.
         */
        template<class T> inline void setSubData(GLint mipLevel, const Math::Vector<GLint, Dimensions>& offset, T* image) {
            bind();
            DataHelper<Dimensions>::setSub(_target, mipLevel, offset, image);
        }

    private:
        Target _target;
};

/** @brief One-dimensional texture */
typedef Texture<1> Texture1D;

/** @brief Two-dimensional texture */
typedef Texture<2> Texture2D;

/** @brief Three-dimensional texture */
typedef Texture<3> Texture3D;

}

#endif
