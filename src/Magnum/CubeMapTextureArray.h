#ifndef Magnum_CubeMapTextureArray_h
#define Magnum_CubeMapTextureArray_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::CubeMapTextureArray
 */
#endif

#include "Magnum/AbstractTexture.h"
#include "Magnum/Array.h"
#include "Magnum/Math/Vector2.h"

#ifndef MAGNUM_TARGET_GLES
namespace Magnum {

/**
@brief Cube map texture array

See @ref CubeMapTexture documentation for introduction.

@section CubeMapTextureArray-usage Usage

Common usage is to specify each layer and face separately using @ref setSubImage().
You have to allocate the memory for all layers and faces first by calling
@ref setStorage(). Example: array with 4 layers of cube maps, each cube map
consisting of six 64x64 images, i.e. 24 layers total:
@code
CubeMapTextureArray texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    // ...
    .setStorage(Math::log2(64)+1, TextureFormat::RGBA8, {64, 64, 24});

for(std::size_t i = 0; i != 4; i += 6) {
    Image3D imagePositiveX(ColorFormat::RGBA, ColorType::UnsignedByte, {64, 64, 1}, data);
    // ...
    texture.setSubImage(0, Vector3i::zAxis(i+0), imagePositiveX);
    texture.setSubImage(0, Vector3i::zAxis(i+1), imageNegativeX);
    texture.setSubImage(0, Vector3i::zAxis(i+2), imagePositiveY);
    // ...
}

texture.generateMipmap();
@endcode

In shader, the texture is used via `samplerCubeArray`, `samplerCubeArrayShadow`,
`isamplerCubeArray` or `usamplerCubeArray`. Unlike in classic textures,
coordinates for cube map texture arrays is signed four-part vector. First three
parts define vector from the center of the cube which intersects with one of
the six sides of the cube map, fourth part is layer in the array. See
@ref AbstractShaderProgram for more information about usage in shaders.

@see @ref Renderer::Feature::SeamlessCubeMapTexture, @ref CubeMapTexture,
    @ref Texture, @ref BufferTexture
@requires_gl40 %Extension @extension{ARB,texture_cube_map_array}
@requires_gl Cube map texture arrays are not available in OpenGL ES.
*/
class CubeMapTextureArray: public AbstractTexture {
    public:
        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object.
         * @see @fn_gl{GenTextures} with @def_gl{TEXTURE_CUBE_MAP_ARRAY}
         */
        explicit CubeMapTextureArray(): AbstractTexture(GL_TEXTURE_CUBE_MAP_ARRAY) {}

        #ifdef CORRADE_GCC45_COMPATIBILITY
        CubeMapTextureArray(const CubeMapTextureArray&) = delete;
        CubeMapTextureArray(CubeMapTextureArray&& other): AbstractTexture(std::move(other)) {}
        CubeMapTextureArray& operator=(const CubeMapTextureArray&) = delete;
        CubeMapTextureArray& operator=(CubeMapTextureArray&& other) {
            AbstractTexture::operator=(std::move(other));
            return *this;
        }
        #endif

        /** @copydoc Texture::setBaseLevel() */
        CubeMapTextureArray& setBaseLevel(Int level) {
            AbstractTexture::setBaseLevel(level);
            return *this;
        }

        /** @copydoc Texture::setMaxLevel() */
        CubeMapTextureArray& setMaxLevel(Int level) {
            AbstractTexture::setMaxLevel(level);
            return *this;
        }

        /** @copydoc Texture::setMinificationFilter() */
        CubeMapTextureArray& setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /** @copydoc Texture::setMagnificationFilter() */
        CubeMapTextureArray& setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        /** @copydoc Texture::setWrapping() */
        CubeMapTextureArray& setWrapping(const Array3D<Sampler::Wrapping>& wrapping) {
            DataHelper<3>::setWrapping(*this, wrapping);
            return *this;
        }

        /** @copydoc Texture::setBorderColor() */
        CubeMapTextureArray& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @copydoc Texture::setBorderColor(const Vector4ui&) */
        CubeMapTextureArray& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @copydoc Texture::setBorderColor(const Vector4i&) */
        CubeMapTextureArray& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @copydoc Texture::setMaxAnisotropy() */
        CubeMapTextureArray& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        /**
         * @brief %Image size in given mip level
         * @param level             Mip level
         *
         * See @ref Texture::imageSize() for more information.
         */
        Vector3i imageSize(Int level) {
            return DataHelper<3>::imageSize(*this, GL_TEXTURE_CUBE_MAP_ARRAY, level);
        }

        /**
         * @brief Set storage
         *
         * Z coordinate of @p size must be multiple of 6. See
         * @ref Texture::setStorage() for more information.
         */
        CubeMapTextureArray& setStorage(Int levels, TextureFormat internalFormat, const Vector3i& size) {
            DataHelper<3>::setStorage(*this, _target, levels, internalFormat, size);
            return *this;
        }

        /**
         * @brief Read given mip level of texture to image
         * @param level             Mip level
         * @param image             %Image where to put the data
         *
         * See @ref Texture::image(Int, Image&) for more information.
         */
        void image(Int level, Image3D& image) {
            AbstractTexture::image<3>(GL_TEXTURE_CUBE_MAP_ARRAY, level, image);
        }

        /**
         * @brief Read given mip level of texture to buffer image
         * @param level             Mip level
         * @param image             %Buffer image where to put the data
         * @param usage             %Buffer usage
         *
         * See @ref Texture::image(Int, BufferImage&, BufferUsage) for more
         * information.
         */
        void image(Int level, BufferImage3D& image, BufferUsage usage) {
            AbstractTexture::image<3>(GL_TEXTURE_CUBE_MAP_ARRAY, level, image, usage);
        }

        /**
         * @brief Set image data
         * @param level             Mip level
         * @param internalFormat    Internal format
         * @param image             @ref Image3D, @ref ImageReference3D or
         *      @ref Trade::ImageData3D
         * @return Reference to self (for method chaining)
         *
         * Sets texture image data from three-dimensional image for all cube
         * faces for all layers. Each group of 6 2D images is one cube map
         * layer, thus Z coordinate of @p image size must be multiple of 6. The
         * images are in order of (+X, -X, +Y, -Y, +Z, -Z).
         *
         * See @ref Texture::setImage() for more information.
         */
        CubeMapTextureArray& setImage(Int level, TextureFormat internalFormat, const ImageReference3D& image) {
            DataHelper<3>::setImage(*this, GL_TEXTURE_CUBE_MAP_ARRAY, level, internalFormat, image);
            return *this;
        }

        /** @overload */
        CubeMapTextureArray& setImage(Int level, TextureFormat internalFormat, BufferImage3D& image) {
            DataHelper<3>::setImage(*this, GL_TEXTURE_CUBE_MAP_ARRAY, level, internalFormat, image);
            return *this;
        }

        /** @overload */
        CubeMapTextureArray& setImage(Int level, TextureFormat internalFormat, BufferImage3D&& image) {
            return setImage(level, internalFormat, image);
        }

        /**
         * @brief Set texture image 3D subdata
         * @param level         Mip level
         * @param offset        Offset where to put data in the texture
         * @param image         @ref Image3D, @ref ImageReference3D or
         *      @ref Trade::ImageData3D
         * @return Reference to self (for method chaining)
         *
         * Z coordinate is equivalent to layer * 6 + number of texture face,
         * i.e. +X is `0` and so on, in order of (+X, -X, +Y, -Y, +Z, -Z).
         *
         * See @ref Texture::setSubImage() for more information.
         */
        CubeMapTextureArray& setSubImage(Int level, const Vector3i& offset, const ImageReference3D& image) {
            DataHelper<3>::setSubImage(*this, GL_TEXTURE_CUBE_MAP_ARRAY, level, offset, image);
            return *this;
        }

        /** @overload */
        CubeMapTextureArray& setSubImage(Int level, const Vector3i& offset, BufferImage3D& image) {
            DataHelper<3>::setSubImage(*this, GL_TEXTURE_CUBE_MAP_ARRAY, level, offset, image);
            return *this;
        }

        /** @overload */
        CubeMapTextureArray& setSubImage(Int level, const Vector3i& offset, BufferImage3D&& image) {
            return setSubImage(level, offset, image);
        }

        /** @copydoc Texture::generateMipmap() */
        CubeMapTextureArray& generateMipmap() {
            AbstractTexture::generateMipmap();
            return *this;
        }

        /** @copydoc Texture::invalidateImage() */
        void invalidateImage(Int level) { AbstractTexture::invalidateImage(level); }

        /**
         * @brief Invalidate texture subimage
         * @param level             Mip level
         * @param offset            Offset into the texture
         * @param size              Size of invalidated data
         *
         * Z coordinate is equivalent to layer * 6 + number of texture face,
         * i.e. +X is `0` and so on, in order of (+X, -X, +Y, -Y, +Z, -Z).
         *
         * See @ref Texture::invalidateSubImage() for more information.
         */
        void invalidateSubImage(Int level, const Vector3i& offset, const Vector3i& size) {
            DataHelper<3>::invalidateSubImage(*this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        CubeMapTextureArray& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        #endif
};

}
#else
#error this header is available only on desktop OpenGL build
#endif

#endif
