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

#include "AbstractTexture.h"

namespace Magnum {

/**
@brief Cube map texture

%Texture used mainly for environment maps. It consists of 6 square textures
generating 6 faces of the cube as following. Note that all images must be
turned upside down (+Y is top):

              +----+
              | -Y |
    +----+----+----+----+
    | -Z | -X | +Z | +X |
    +----+----+----+----+
              | +Y |
              +----+

@section CubeMapTexture-usage Basic usage

See Texture documentation for introduction.

Common usage is to fully configure all texture parameters and then set the
data from e.g. set of Image objects:
@code
Image2D positiveX({256, 256}, Image2D::Components::RGBA, Image2D::ComponentType::UnsignedByte, dataPositiveX);
// ...

CubeMapTexture texture;
texture.setMagnificationFilter(Texture2D::Filter::Linear)
    // ...
    ->setData(CubeMapTexture::Coordinate::PositiveX, 0, Texture2D::Format::RGBA8, &positiveX)
    ->setData(CubeMapTexture::Coordinate::NegativeX, 0, Texture2D::Format::RGBA8, &negativeX)
    // ...
@endcode

The texture is bound to layer specified by shader via bind(). In shader, the
texture is used via `samplerCube`. Unlike in classic textures, coordinates for
cube map textures is signed three-part vector from the center of the cube,
which intersects one of the six sides of the cube map. See also
AbstractShaderProgram for more information.

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
         * @requires_gl32 %Extension @extension{ARB,seamless_cube_map}
         * @requires_gl Not available in OpenGL ES 2.0, always enabled in
         *      OpenGL ES 3.0.
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
        inline explicit CubeMapTexture(): AbstractTexture(GL_TEXTURE_CUBE_MAP) {}

        /**
         * @copydoc Texture::setWrapping()
         */
        inline CubeMapTexture* setWrapping(const Array3D<Wrapping>& wrapping) {
            DataHelper<3>::setWrapping(this, wrapping);
            return this;
        }

        /**
         * @copydoc Texture::setData(GLint, InternalFormat, Image*)
         * @param coordinate    Coordinate
         * @return Pointer to self (for method chaining)
         */
        template<class Image> inline CubeMapTexture* setData(Coordinate coordinate, GLint mipLevel, InternalFormat internalFormat, Image* image) {
            DataHelper<2>::set(this, static_cast<GLenum>(coordinate), mipLevel, internalFormat, image);
            return this;
        }

        /**
         * @copydoc Texture::setSubData(GLint, const typename DimensionTraits<Dimensions, GLint>::VectorType&, Image*)
         * @param coordinate    Coordinate
         * @return Pointer to self (for method chaining)
         */
        template<class Image> inline CubeMapTexture* setSubData(Coordinate coordinate, GLint mipLevel, const Vector2i& offset, const Image* image) {
            DataHelper<2>::setSub(this, static_cast<GLenum>(coordinate), mipLevel, offset, image);
            return this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline CubeMapTexture* setMinificationFilter(Filter filter, Mipmap mipmap = Mipmap::BaseLevel) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return this;
        }
        inline CubeMapTexture* setMagnificationFilter(Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return this;
        }
        #ifndef MAGNUM_TARGET_GLES
        inline CubeMapTexture* setBorderColor(const Color4<>& color) {
            AbstractTexture::setBorderColor(color);
            return this;
        }
        #endif
        inline CubeMapTexture* setMaxAnisotropy(GLfloat anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return this;
        }
        inline CubeMapTexture* generateMipmap() {
            AbstractTexture::generateMipmap();
            return this;
        }
        #endif
};

}

#endif
