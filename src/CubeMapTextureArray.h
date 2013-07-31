#ifndef Magnum_CubeMapTextureArray_h
#define Magnum_CubeMapTextureArray_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
Image3D dummy({64, 64, 16*6}, ImageFormat::RGBA, ImageType::UnsignedByte, nullptr);

CubeMapTextureArray texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    // ...
    ->setStorage(Math::log2(64)+1, TextureFormat::RGBA8, {64, 64, 16});

for(std::size_t i = 0; i != 16; ++i) {
    void* dataPositiveX = ...;
    Image2D imagePositiveX({64, 64}, ImageFormat::RGBA, ImageType::UnsignedByte, imagePositiveX);
    // ...
    texture.setSubImage(i, CubeMapTextureArray::Coordinate::PositiveX, 0, {}, imagePositiveX);
    texture.setSubImage(i, CubeMapTextureArray::Coordinate::NegativeX, 0, {}, imageNegativeX);
    // ...
}

// ...
@endcode

The texture is bound to layer specified by shader via bind(). In shader, the
texture is used via `samplerCubeArray`, `samplerCubeArrayShadow`,
`isamplerCubeArray` or `usamplerCubeArray`. Unlike in classic textures,
coordinates for cube map texture arrays is signed four-part vector. First three
parts define vector from the center of the cube which intersects with one of
the six sides of the cube map, fourth part is layer in the array. See also
AbstractShaderProgram for more information about usage in shaders.

@see @ref Renderer::Feature "Renderer::Feature::SeamlessCubeMapTexture",
    CubeMapTexture, Texture, BufferTexture
@requires_gl40 %Extension @extension{ARB,texture_cube_map_array}
@requires_gl Cube map texture arrays are not available in OpenGL ES.
*/
class CubeMapTextureArray: public AbstractTexture {
    public:
        /** @brief Cube map coordinate */
        enum class Coordinate: GLsizei {
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
        explicit CubeMapTextureArray(): AbstractTexture(GL_TEXTURE_CUBE_MAP_ARRAY) {}

        /**
         * @brief Set wrapping
         *
         * See Texture::setWrapping() for more information.
         */
        CubeMapTextureArray* setWrapping(const Array3D<Sampler::Wrapping>& wrapping) {
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
        Vector3i imageSize(Coordinate coordinate, Int level) {
            return DataHelper<3>::imageSize(this, GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(coordinate), level);
        }

        /**
         * @brief Set storage
         *
         * See Texture::setStorage() for more information.
         */
        CubeMapTextureArray* setStorage(Int levels, TextureFormat internalFormat, const Vector3i& size) {
            DataHelper<3>::setStorage(this, _target, levels, internalFormat, size);
            return this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Read given mip level of texture to image
         * @param coordinate        Coordinate
         * @param level             Mip level
         * @param image             %Image where to put the data
         *
         * See Texture::image(Int, Image&) for more information.
         * @requires_gl %Texture image queries are not available in OpenGL ES.
         */
        void image(Coordinate coordinate, Int level, Image3D& image) {
            AbstractTexture::image<3>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + GLenum(coordinate), level, image);
        }

        /**
         * @brief Read given mip level of texture to buffer image
         * @param coordinate        Coordinate
         * @param level             Mip level
         * @param image             %Buffer image where to put the data
         * @param usage             %Buffer usage
         *
         * See Texture::image(Int, BufferImage&, Buffer::Usage) for more
         * information.
         * @requires_gl %Texture image queries are not available in OpenGL ES.
         */
        void image(Coordinate coordinate, Int level, BufferImage3D& image, Buffer::Usage usage) {
            AbstractTexture::image<3>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + GLenum(coordinate), level, image, usage);
        }
        #endif

        /**
         * @brief Set image data
         * @param level             Mip level
         * @param internalFormat    Internal format
         * @param image             Image, ImageReference, BufferImage or
         *      Trade::ImageData of the same dimension count
         * @return Pointer to self (for method chaining)
         *
         * Sets texture image data from three-dimensional image for all cube
         * faces for all layers. Each group of 6 2D images is one cube map
         * layer. The images are ordered the same way as Coordinate enum.
         *
         * See Texture::setImage() for more information.
         */
        CubeMapTextureArray* setImage(Int level, TextureFormat internalFormat, const ImageReference3D& image) {
            DataHelper<3>::setImage(this, GL_TEXTURE_CUBE_MAP_ARRAY, level, internalFormat, image);
            return this;
        }

        /** @overload */
        CubeMapTextureArray* setImage(Int level, TextureFormat internalFormat, BufferImage3D& image) {
            DataHelper<3>::setImage(this, GL_TEXTURE_CUBE_MAP_ARRAY, level, internalFormat, image);
            return this;
        }

        /**
         * @brief Set texture image 3D subdata
         * @param level         Mip level
         * @param offset        Offset where to put data in the texture
         * @param image         Image3D, ImageReference3D, BufferImage3D or
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
         * @see setSubImage(Int, Coordinate, Int, const Math::Vector<2, Int>&, const Image*)
         */
        CubeMapTextureArray* setSubImage(Int level, const Vector3i& offset, const ImageReference3D& image) {
            DataHelper<3>::setSubImage(this, GL_TEXTURE_CUBE_MAP_ARRAY, level, offset, image);
            return this;
        }

        /** @overload */
        CubeMapTextureArray* setSubImage(Int level, const Vector3i& offset, BufferImage3D& image) {
            DataHelper<3>::setSubImage(this, GL_TEXTURE_CUBE_MAP_ARRAY, level, offset, image);
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
        void invalidateSubImage(Int level, const Vector3i& offset, const Vector3i& size) {
            DataHelper<3>::invalidateSubImage(this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        CubeMapTextureArray* setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return this;
        }
        CubeMapTextureArray* setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return this;
        }
        #ifndef MAGNUM_TARGET_GLES3
        CubeMapTextureArray* setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return this;
        }
        CubeMapTextureArray* setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return this;
        }
        #endif
        CubeMapTextureArray* generateMipmap() {
            AbstractTexture::generateMipmap();
            return this;
        }
        #endif
};

}
#endif

#endif
