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

#ifndef MAGNUM_TARGET_GLES
/** @file
 * @brief Class Magnum::CubeMapTextureArray
 */
#endif

#include "AbstractTexture.h"

#ifndef MAGNUM_TARGET_GLES
namespace Magnum {

/**
@brief Cube map texture array

See CubeMapTexture documentation for introduction.

@section CubeMapTextureArray-usage Usage

Common usage is to specify each layer and face separately using setSubImage().
You have to allocate the memory for all layers and faces first either by
calling setStorage() or by passing properly sized empty Image to setImage().
Example: array with 16 layers of cube map faces, each face consisting of six
64x64 images:
@code
Image3D dummy({64, 64, 16*6}, Image3D::Components::RGBA, Image3D::ComponentType::UnsignedByte, nullptr);

CubeMapTextureArray texture;
texture.setMagnificationFilter(CubeMapTextureArray::Filter::Linear)
    // ...
    ->setStorage(Math::log2(64)+1, CubeMapTextureArray::Format::RGBA8, {64, 64, 16});

for(std::size_t i = 0; i != 16; ++i) {
    void* dataPositiveX = ...;
    Image2D imagePositiveX({64, 64}, Image3D::Components::RGBA, Image3D::ComponentType::UnsignedByte, imagePositiveX);
    // ...
    texture->setSubImage(i, CubeMapTextureArray::Coordinate::PositiveX, 0, {}, imagePositiveX);
    texture->setSubImage(i, CubeMapTextureArray::Coordinate::NegativeX, 0, {}, imageNegativeX);
    // ...
}

// ...
@endcode

The texture is bound to layer specified by shader via bind(). In shader, the
texture is used via `samplerCubeArray`. Unlike in classic textures,
coordinates for cube map texture arrays is signed four-part vector. First
three parts define vector from the center of the cube which intersects with
one of the six sides of the cube map, fourth part is layer in the array. See
also AbstractShaderProgram for more information.

@see CubeMapTexture::setSeamless()
@requires_gl40 %Extension @extension{ARB,texture_cube_map_array}
@requires_gl Cube map texture arrays are not available in OpenGL ES.
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
         * @see @fn_gl{GenTextures} with @def_gl{TEXTURE_CUBE_MAP}
         */
        inline explicit CubeMapTextureArray(): AbstractTexture(GL_TEXTURE_CUBE_MAP_ARRAY) {}

        /**
         * @brief Set wrapping
         *
         * See Texture::setWrapping() for more information.
         */
        inline CubeMapTextureArray* setWrapping(const Array3D<Wrapping>& wrapping) {
            DataHelper<3>::setWrapping(this, wrapping);
            return this;
        }

        /**
         * @brief %Image size in given mip level
         * @param coordinate        Coordinate
         * @param level             Mip level
         *
         * See Texture::imageSize() for more information.
         */
        inline Vector3i imageSize(Coordinate coordinate, GLint level) {
            return DataHelper<3>::imageSize(this, GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(coordinate), level);
        }

        /**
         * @brief Set storage
         *
         * See Texture::setStorage() for more information.
         */
        inline CubeMapTextureArray* setStorage(GLsizei levels, InternalFormat internalFormat, const Vector3i& size) {
            DataHelper<3>::setStorage(this, _target, levels, internalFormat, size);
            return this;
        }

        /**
         * @brief Set image data
         * @param level             Mip level
         * @param internalFormat    Internal format
         * @param image             Image, ImageWrapper, BufferImage or
         *      Trade::ImageData of the same dimension count
         * @return Pointer to self (for method chaining)
         *
         * Sets texture image data from three-dimensional image for all cube
         * faces for all layers. Each group of 6 2D images is one cube map
         * layer. The images are ordered the same way as Coordinate enum.
         *
         * See Texture::setImage() for more information.
         */
        template<class T> inline CubeMapTextureArray* setImage(GLint level, InternalFormat internalFormat, T* image) {
            DataHelper<3>::set(this, GL_TEXTURE_CUBE_MAP_ARRAY, level, internalFormat, image);
            return this;
        }

        /**
         * @brief Set texture image 3D subdata
         * @param level         Mip level
         * @param offset        Offset where to put data in the texture
         * @param image         Image3D, ImageWrapper3D, BufferImage3D or
         *      Trade::ImageData3D
         * @return Pointer to self (for method chaining)
         *
         * Sets texture image subdata for more than one level/face at once.
         *
         * Z coordinate of @p offset specifies layer and cube map face. If
         * you want to start at given face in layer *n*, you have to specify
         * Z coordinate as @f$ 6n + i @f$, where i is face index as specified
         * in Coordinate enum.
         *
         * See Texture::setSubImage() for more information.
         *
         * @see setSubImage(GLsizei, Coordinate, GLint, const Math::Vector<2, GLint>&, const Image*)
         */
        template<class Image> inline CubeMapTextureArray* setSubImage(GLint level, const Vector3i& offset, const Image* image) {
            DataHelper<3>::setSub(this, GL_TEXTURE_CUBE_MAP_ARRAY, level, offset, image, Vector3i(Math::Vector<Image::Dimensions, GLsizei>()));
            return this;
        }

        /**
         * @brief Set texture image 2D subdata
         * @param layer         Array layer
         * @param coordinate    Coordinate
         * @param level         Mip level
         * @param offset        Offset where to put data in the texture
         * @param image         Image2D, ImageWrapper2D, BufferImage2D or
         *      Trade::ImageData2D
         * @return Pointer to self (for method chaining)
         *
         * See Texture::setSubImage() for more information.
         *
         * @see setSubImage(GLint, const Math::Vector<3, GLint>&, const Image*)
         */
        template<class Image> inline CubeMapTextureArray* setSubImage(GLsizei layer, Coordinate coordinate, GLint level, const Vector2i& offset, const Image* image) {
            DataHelper<3>::setSub(this, GL_TEXTURE_CUBE_MAP_ARRAY, level, Vector3i(offset, layer*6+static_cast<GLsizei>(coordinate)), image, Vector2i(Math::Vector<Image::Dimensions, GLsizei>()));
            return this;
        }

        /**
         * @brief Invalidate texture subimage
         * @param level             Mip level
         * @param offset            Offset into the texture
         * @param size              Size of invalidated data
         *
         * Z coordinate is equivalent to layer * 6 + number of texture face,
         * i.e. @ref Coordinate "Coordinate::PositiveX" is `0` and so on, in
         * the same order as in the enum.
         *
         * See Texture::invalidateSubImage() for more information.
         */
        inline void invalidateSubImage(GLint level, const Vector3i& offset, const Vector3i& size) {
            DataHelper<3>::invalidateSub(this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline CubeMapTextureArray* setMinificationFilter(Filter filter, Mipmap mipmap = Mipmap::BaseLevel) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return this;
        }
        inline CubeMapTextureArray* setMagnificationFilter(Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return this;
        }
        inline CubeMapTextureArray* setBorderColor(const Color4<>& color) {
            AbstractTexture::setBorderColor(color);
            return this;
        }
        inline CubeMapTextureArray* setMaxAnisotropy(GLfloat anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return this;
        }
        inline CubeMapTextureArray* generateMipmap() {
            AbstractTexture::generateMipmap();
            return this;
        }
        #endif
};

}
#endif

#endif
