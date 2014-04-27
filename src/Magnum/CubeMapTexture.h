#ifndef Magnum_CubeMapTexture_h
#define Magnum_CubeMapTexture_h
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

/** @file
 * @brief Class @ref Magnum::CubeMapTexture
 */

#include "Magnum/AbstractTexture.h"
#include "Magnum/Array.h"
#include "Magnum/Math/Vector2.h"

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

See @ref Texture documentation for introduction.

Common usage is to fully configure all texture parameters and then set the
data from e.g. set of Image objects:
@code
Image2D positiveX(ColorFormat::RGBA, ColorType::UnsignedByte, {256, 256}, data);
// ...

CubeMapTexture texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    // ...
    .setStorage(Math::log2(256)+1, TextureFormat::RGBA8, {256, 256})
    .setSubImage(CubeMapTexture::Coordinate::PositiveX, 0, {}, positiveX)
    .setSubImage(CubeMapTexture::Coordinate::NegativeX, 0, {}, negativeX)
    // ...
@endcode

In shader, the texture is used via `samplerCube`, `samplerCubeShadow`,
`isamplerCube` or `usamplerCube`. Unlike in classic textures, coordinates for
cube map textures is signed three-part vector from the center of the cube,
which intersects one of the six sides of the cube map. See
@ref AbstractShaderProgram for more information about usage in shaders.

@see @ref Renderer::Feature::SeamlessCubeMapTexture, @ref CubeMapTextureArray,
    @ref Texture, @ref TextureArray, @ref RectangleTexture, @ref BufferTexture,
    @ref MultisampleTexture
*/
class MAGNUM_EXPORT CubeMapTexture: public AbstractTexture {
    public:
        /** @brief Cube map coordinate */
        enum class Coordinate: GLenum {
            PositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,     /**< +X cube side */
            NegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,     /**< -X cube side */
            PositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,     /**< +Y cube side */
            NegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,     /**< -Y cube side */
            PositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,     /**< +Z cube side */
            NegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z      /**< -Z cube side */
        };

        /**
         * @brief Max supported size of one side of cube map texture
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @fn_gl{Get} with @def_gl{MAX_CUBE_MAP_TEXTURE_SIZE}
         */
        static Vector2i maxSize();

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object.
         * @see @fn_gl{GenTextures} with @def_gl{TEXTURE_CUBE_MAP}
         */
        explicit CubeMapTexture(): AbstractTexture(GL_TEXTURE_CUBE_MAP) {}

        #ifdef CORRADE_GCC45_COMPATIBILITY
        CubeMapTexture(const CubeMapTexture&) = delete;
        CubeMapTexture(CubeMapTexture&& other): AbstractTexture(std::move(other)) {}
        CubeMapTexture& operator=(CubeMapTexture&) = delete;
        CubeMapTexture& operator=(CubeMapTexture&& other) {
            AbstractTexture::operator=(std::move(other));
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc Texture::setBaseLevel() */
        CubeMapTexture& setBaseLevel(Int level) {
            AbstractTexture::setBaseLevel(level);
            return *this;
        }
        #endif

        /** @copydoc Texture::setMaxLevel() */
        CubeMapTexture& setMaxLevel(Int level) {
            AbstractTexture::setMaxLevel(level);
            return *this;
        }

        /** @copydoc Texture::setMinificationFilter() */
        CubeMapTexture& setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /** @copydoc Texture::setMagnificationFilter() */
        CubeMapTexture& setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc Texture::setMinLod() */
        CubeMapTexture& setMinLod(Float lod) {
            AbstractTexture::setMinLod(lod);
            return *this;
        }

        /** @copydoc Texture::setMaxLod() */
        CubeMapTexture& setMaxLod(Float lod) {
            AbstractTexture::setMaxLod(lod);
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /** @copydoc Texture::setLodBias() */
        CubeMapTexture& setLodBias(Float bias) {
            AbstractTexture::setLodBias(bias);
            return *this;
        }
        #endif

        /** @copydoc Texture::setWrapping() */
        CubeMapTexture& setWrapping(const Array3D<Sampler::Wrapping>& wrapping) {
            DataHelper<3>::setWrapping(*this, wrapping);
            return *this;
        }

        /** @copydoc Texture::setBorderColor(const Color4&) */
        CubeMapTexture& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /** @copydoc Texture::setBorderColor(const Vector4ui&) */
        CubeMapTexture& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @copydoc Texture::setBorderColor(const Vector4i&) */
        CubeMapTexture& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        #endif

        /** @copydoc Texture::setMaxAnisotropy() */
        CubeMapTexture& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc Texture::setSwizzle() */
        template<char r, char g, char b, char a> CubeMapTexture& setSwizzle() {
            AbstractTexture::setSwizzle<r, g, b, a>();
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setCompareMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareMode() for more information.
         * @requires_gles30 %Extension @es_extension{EXT,shadow_samplers} and
         *      @es_extension{NV,shadow_samplers_cube}
         */
        CubeMapTexture& setCompareMode(Sampler::CompareMode mode) {
            AbstractTexture::setCompareMode(mode);
            return *this;
        }

        /**
         * @copybrief Texture::setCompareFunction()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareFunction() for more information.
         * @requires_gles30 %Extension @es_extension{EXT,shadow_samplers} and
         *      @es_extension{NV,shadow_samplers_cube}
         */
        CubeMapTexture& setCompareFunction(Sampler::CompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /** @copydoc Texture::setDepthStencilMode() */
        CubeMapTexture& setDepthStencilMode(Sampler::DepthStencilMode mode) {
            AbstractTexture::setDepthStencilMode(mode);
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief %Image size in given mip level
         * @param coordinate        Coordinate
         * @param level             Mip level
         *
         * See @ref Texture::imageSize() for more information.
         * @requires_gl %Texture image queries are not available in OpenGL ES.
         */
        Vector2i imageSize(Coordinate coordinate, Int level) {
            return DataHelper<2>::imageSize(*this, GLenum(coordinate), level);
        }
        #endif

        /**
         * @brief Set storage
         *
         * See @ref Texture::setStorage() for more information.
         * @see @ref maxSize()
         */
        CubeMapTexture& setStorage(Int levels, TextureFormat internalFormat, const Vector2i& size) {
            DataHelper<2>::setStorage(*this, _target, levels, internalFormat, size);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Read given mip level of texture to image
         * @param coordinate        Coordinate
         * @param level             Mip level
         * @param image             %Image where to put the data
         *
         * See @ref Texture::image(Int, Image&) for more information.
         * @requires_gl %Texture image queries are not available in OpenGL ES.
         */
        void image(Coordinate coordinate, Int level, Image2D& image) {
            AbstractTexture::image<2>(GLenum(coordinate), level, image);
        }

        /**
         * @brief Read given mip level of texture to buffer image
         * @param coordinate        Coordinate
         * @param level             Mip level
         * @param image             %Buffer image where to put the data
         * @param usage             %Buffer usage
         *
         * See @ref Texture::image(Int, BufferImage&, BufferUsage) for more
         * information.
         * @requires_gl %Texture image queries are not available in OpenGL ES.
         */
        void image(Coordinate coordinate, Int level, BufferImage2D& image, BufferUsage usage) {
            AbstractTexture::image<2>(GLenum(coordinate), level, image, usage);
        }
        #endif

        /**
         * @brief Set image data
         * @param coordinate        Coordinate
         * @param level             Mip level
         * @param internalFormat    Internal format
         * @param image             @ref Image2D, @ref ImageReference2D or
         *      @ref Trade::ImageData2D
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setImage() for more information.
         * @see @ref maxSize()
         */
        CubeMapTexture& setImage(Coordinate coordinate, Int level, TextureFormat internalFormat, const ImageReference2D& image) {
            DataHelper<2>::setImage(*this, GLenum(coordinate), level, internalFormat, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload */
        CubeMapTexture& setImage(Coordinate coordinate, Int level, TextureFormat internalFormat, BufferImage2D& image) {
            DataHelper<2>::setImage(*this, GLenum(coordinate), level, internalFormat, image);
            return *this;
        }

        /** @overload */
        CubeMapTexture& setImage(Coordinate coordinate, Int level, TextureFormat internalFormat, BufferImage2D&& image) {
            return setImage(coordinate, level, internalFormat, image);
        }
        #endif

        /**
         * @brief Set image subdata
         * @param coordinate        Coordinate
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             @ref Image2D, @ref ImageReference2D or
         *      @ref Trade::ImageData2D
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSubImage() for more information.
         */
        CubeMapTexture& setSubImage(Coordinate coordinate, Int level, const Vector2i& offset, const ImageReference2D& image) {
            DataHelper<2>::setSubImage(*this, GLenum(coordinate), level, offset, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload */
        CubeMapTexture& setSubImage(Coordinate coordinate, Int level, const Vector2i& offset, BufferImage2D& image) {
            DataHelper<2>::setSubImage(*this, GLenum(coordinate), level, offset, image);
            return *this;
        }

        /** @overload */
        CubeMapTexture& setSubImage(Coordinate coordinate, Int level, const Vector2i& offset, BufferImage2D&& image) {
            DataHelper<2>::setSubImage(*this, GLenum(coordinate), level, offset, image);
            return *this;
        }
        #endif

        /** @copydoc Texture::generateMipmap() */
        CubeMapTexture& generateMipmap() {
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
         * Z coordinate is equivalent to number of texture face, i.e.
         * @ref Coordinate::PositiveX is `0` and so on, in the same order as in
         * the enum.
         *
         * See @ref Texture::invalidateSubImage() for more information.
         */
        void invalidateSubImage(Int level, const Vector3i& offset, const Vector3i& size) {
            DataHelper<3>::invalidateSubImage(*this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        CubeMapTexture& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        #endif
};

}

#endif
