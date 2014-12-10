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

Texture used mainly for environment maps. It consists of 6 square textures
generating 6 faces of the cube as following. Note that all images must be
turned upside down (+Y is top):

              +----+
              | -Y |
    +----+----+----+----+
    | -Z | -X | +Z | +X |
    +----+----+----+----+
              | +Y |
              +----+

## Basic usage

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
    friend Implementation::TextureState;

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
         * Creates new OpenGL texture object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not supported, the texture is created on
         * first use.
         * @see @fn_gl{CreateTextures} with @def_gl{TEXTURE_CUBE_MAP},
         *      eventually @fn_gl{GenTextures}
         */
        explicit CubeMapTexture(): AbstractTexture(GL_TEXTURE_CUBE_MAP) {}

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copybrief Texture::setBaseLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBaseLevel() for more information.
         * @requires_gles30 Base level is always `0` in OpenGL ES 2.0.
         */
        CubeMapTexture& setBaseLevel(Int level) {
            AbstractTexture::setBaseLevel(level);
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setMaxLevel()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLevel() for more information.
         * @requires_gles30 Extension @es_extension{APPLE,texture_max_level},
         *      otherwise the max level is always set to largest possible value
         *      in OpenGL ES 2.0.
         */
        CubeMapTexture& setMaxLevel(Int level) {
            AbstractTexture::setMaxLevel(level);
            return *this;
        }

        /**
         * @copybrief Texture::setMinificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinificationFilter() for more information.
         */
        CubeMapTexture& setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /**
         * @copybrief Texture::setMagnificationFilter()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMagnificationFilter() for more information.
         */
        CubeMapTexture& setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copybrief Texture::setMinLod()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMinLod() for more information.
         * @requires_gles30 Texture LOD parameters are not available in OpenGL
         *      ES 2.0.
         */
        CubeMapTexture& setMinLod(Float lod) {
            AbstractTexture::setMinLod(lod);
            return *this;
        }

        /**
         * @copybrief Texture::setMaxLod()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxLod() for more information.
         * @requires_gles30 Texture LOD parameters are not available in OpenGL
         *      ES 2.0.
         */
        CubeMapTexture& setMaxLod(Float lod) {
            AbstractTexture::setMaxLod(lod);
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::setLodBias()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setLodBias() for more information.
         * @requires_gl Texture LOD bias can be specified only directly in
         *      fragment shader in OpenGL ES.
         */
        CubeMapTexture& setLodBias(Float bias) {
            AbstractTexture::setLodBias(bias);
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setWrapping()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setWrapping() for more information.
         */
        CubeMapTexture& setWrapping(const Array3D<Sampler::Wrapping>& wrapping) {
            DataHelper<3>::setWrapping(*this, wrapping);
            return *this;
        }

        /**
         * @copybrief Texture::setBorderColor(const Color4&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Color4&) for more
         * information.
         * @requires_es_extension Extension @es_extension{NV,texture_border_clamp}
         */
        CubeMapTexture& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::setBorderColor(const Vector4ui&)
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setBorderColor(const Vector4ui&) for more
         * information.
         * @requires_gl30 Extension @extension{EXT,texture_integer}
         * @requires_gl Border is available only for float textures in OpenGL
         *      ES.
         */
        CubeMapTexture& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @overload
         * @requires_gl30 Extension @extension{EXT,texture_integer}
         * @requires_gl Border is available only for float textures in OpenGL
         *      ES.
         */
        CubeMapTexture& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setMaxAnisotropy()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setMaxAnisotropy() for more information.
         */
        CubeMapTexture& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        /**
         * @copybrief Texture::setSRGBDecode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSRGBDecode() for more information.
         * @requires_extension Extension @extension{EXT,texture_sRGB_decode}
         * @requires_es_extension OpenGL ES 3.0 or extension
         *      @es_extension{EXT,sRGB} and
         *      @es_extension2{EXT,texture_sRGB_decode,texture_sRGB_decode}
         */
        CubeMapTexture& setSRGBDecode(bool decode) {
            AbstractTexture::setSRGBDecode(decode);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copybrief Texture::setSwizzle()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSwizzle() for more information.
         * @requires_gl33 Extension @extension{ARB,texture_swizzle}
         * @requires_gles30 Texture swizzle is not available in OpenGL ES 2.0.
         */
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
         * @requires_gles30 Extension @es_extension{EXT,shadow_samplers} and
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
         * @requires_gles30 Extension @es_extension{EXT,shadow_samplers} and
         *      @es_extension{NV,shadow_samplers_cube}
         */
        CubeMapTexture& setCompareFunction(Sampler::CompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copybrief Texture::setDepthStencilMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setDepthStencilMode() for more information.
         * @requires_gl43 Extension @extension{ARB,stencil_texturing}
         * @requires_gles31 Stencil texturing is not available in OpenGL ES 3.0
         *      and older.
         */
        CubeMapTexture& setDepthStencilMode(Sampler::DepthStencilMode mode) {
            AbstractTexture::setDepthStencilMode(mode);
            return *this;
        }
        #endif

        /**
         * @copybrief Texture::setStorage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setStorage() for more information.
         * @see @ref maxSize()
         */
        CubeMapTexture& setStorage(Int levels, TextureFormat internalFormat, const Vector2i& size) {
            DataHelper<2>::setStorage(*this, levels, internalFormat, size);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @copybrief Texture::imageSize()
         *
         * If on OpenGL ES or @extension{ARB,direct_state_access} (part of
         * OpenGL 4.5) is not available, it is assumed that faces have the same
         * size and just the size of @ref Coordinate::PositiveX face is
         * queried. See @ref Texture::imageSize() for more information.
         * @requires_gles31 Texture image size queries are not available in
         *      OpenGL ES 3.0 and older.
         */
        Vector2i imageSize(Int level);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief imageSize()
         * @deprecated Use @ref Magnum::CubeMapTexture::imageSize(Int) "imageSize(Int)"
         *      instead.
         */
        CORRADE_DEPRECATED("use imageSize(Int) instead") Vector2i imageSize(Coordinate, Int level) {
            return imageSize(level);
        }
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::image(Int, Image&)
         *
         * See @ref Texture::image(Int, Image&) for more information.
         * @requires_gl Texture image queries are not available in OpenGL ES.
         */
        void image(Coordinate coordinate, Int level, Image2D& image);

        /**
         * @copybrief Texture::image(Int, BufferImage&, BufferUsage)
         *
         * See @ref Texture::image(Int, BufferImage&, BufferUsage) for more
         * information.
         * @requires_gl Texture image queries are not available in OpenGL ES.
         */
        void image(Coordinate coordinate, Int level, BufferImage2D& image, BufferUsage usage);
        #endif

        /**
         * @copybrief Texture::setImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setImage() for more information.
         * @see @ref maxSize()
         * @deprecated_gl Prefer to use @ref Magnum::CubeMapTexture::setStorage() "setStorage()"
         *      and @ref Magnum::CubeMapTexture::setSubImage() "setSubImage()"
         *      instead.
         */
        CubeMapTexture& setImage(Coordinate coordinate, Int level, TextureFormat internalFormat, const ImageReference2D& image) {
            DataHelper<2>::setImage(*this, GLenum(coordinate), level, internalFormat, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @deprecated_gl Prefer to use @ref Magnum::CubeMapTexture::setStorage() "setStorage()"
         *      and @ref Magnum::CubeMapTexture::setSubImage() "setSubImage()"
         *      instead.
         */
        CubeMapTexture& setImage(Coordinate coordinate, Int level, TextureFormat internalFormat, BufferImage2D& image) {
            DataHelper<2>::setImage(*this, GLenum(coordinate), level, internalFormat, image);
            return *this;
        }

        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @deprecated_gl Prefer to use @ref Magnum::CubeMapTexture::setStorage() "setStorage()"
         *      and @ref Magnum::CubeMapTexture::setSubImage() "setSubImage()"
         *      instead.
         */
        CubeMapTexture& setImage(Coordinate coordinate, Int level, TextureFormat internalFormat, BufferImage2D&& image) {
            return setImage(coordinate, level, internalFormat, image);
        }
        #endif

        /**
         * @copybrief Texture::setSubImage()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setSubImage() for more information.
         */
        CubeMapTexture& setSubImage(Coordinate coordinate, Int level, const Vector2i& offset, const ImageReference2D& image);

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         */
        CubeMapTexture& setSubImage(Coordinate coordinate, Int level, const Vector2i& offset, BufferImage2D& image);

        /** @overload
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         */
        CubeMapTexture& setSubImage(Coordinate coordinate, Int level, const Vector2i& offset, BufferImage2D&& image) {
            return setSubImage(coordinate, level, offset, image);
        }
        #endif

        /**
         * @copybrief Texture::generateMipmap()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::generateMipmap() for more information.
         * @requires_gl30 Extension @extension{ARB,framebuffer_object}
         */
        CubeMapTexture& generateMipmap() {
            AbstractTexture::generateMipmap();
            return *this;
        }

        /**
         * @copybrief Texture::invalidateImage()
         *
         * See @ref Texture::invalidateImage() for more information.
         */
        void invalidateImage(Int level) { AbstractTexture::invalidateImage(level); }

        /**
         * @copybrief Texture::invalidateSubImage()
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
        template<std::size_t size> CubeMapTexture& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        Vector2i MAGNUM_LOCAL getImageSizeImplementationDefault(Int level);
        #ifndef MAGNUM_TARGET_GLES
        Vector2i MAGNUM_LOCAL getImageSizeImplementationDSA(Int level);
        Vector2i MAGNUM_LOCAL getImageSizeImplementationDSAEXT(Int level);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL getImageImplementationDefault(Coordinate coordinate, GLint level, const Vector2i& size, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationDSA(Coordinate coordinate, GLint level, const Vector2i& size, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationDSAEXT(Coordinate coordinate, GLint level, const Vector2i& size, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        void MAGNUM_LOCAL getImageImplementationRobustness(Coordinate coordinate, GLint level, const Vector2i& size, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        #endif

        void MAGNUM_LOCAL subImageImplementationDefault(Coordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(Coordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        void MAGNUM_LOCAL subImageImplementationDSAEXT(Coordinate coordinate, GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data);
        #endif
};

}

#endif
