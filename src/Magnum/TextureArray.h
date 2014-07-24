#ifndef Magnum_TextureArray_h
#define Magnum_TextureArray_h
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

#ifndef MAGNUM_TARGET_GLES2
/** @file
 * @brief Class @ref Magnum::TextureArray, typedef @ref Magnum::Texture1DArray, @ref Magnum::Texture2DArray
 */
#endif

#include "Magnum/AbstractTexture.h"
#include "Magnum/Array.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"

#ifndef MAGNUM_TARGET_GLES2
namespace Magnum {

namespace Implementation {
    template<UnsignedInt> constexpr GLenum textureArrayTarget();
    #ifndef MAGNUM_TARGET_GLES
    template<> inline constexpr GLenum textureArrayTarget<1>() { return GL_TEXTURE_1D_ARRAY; }
    #endif
    template<> inline constexpr GLenum textureArrayTarget<2>() { return GL_TEXTURE_2D_ARRAY; }
}

/**
@brief %Texture array

Template class for one- and two-dimensional texture arrays. See also
@ref AbstractTexture documentation for more information.

## Usage

See @ref Texture documentation for introduction.

Common usage is to fully configure all texture parameters and then set the
data. Example configuration:
@code
Texture2DArray texture;
texture.setMagnificationFilter(Sampler::Filter::Linear)
    .setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
    .setWrapping(Sampler::Wrapping::ClampToEdge)
    .setMaxAnisotropy(Sampler::maxMaxAnisotropy());;
@endcode

It is often more convenient to first allocate the memory for all layers by
calling @ref setStorage() and then specify each layer separately using
@ref setSubImage():
@code
texture.setStorage(levels, TextureFormat::RGBA8, {64, 64, 16});

for(std::size_t i = 0; i != 16; ++i) {
    Image3D image(ColorFormat::RGBA, ColorType::UnsignedByte, {64, 64, 1}, ...);
    texture.setSubImage(0, Vector3i::zAxis(i), image);
}
@endcode

In shader, the texture is used via `sampler1DArray`/`sampler2DArray`,
`sampler1DArrayShadow`/`sampler1DArrayShadow`, `isampler1DArray`/`isampler2DArray`
or `usampler1DArray`/`usampler2DArray`. See @ref AbstractShaderProgram
documentation for more information about usage in shaders.

@see @ref Texture1DArray, @ref Texture2DArray, @ref Texture,
    @ref CubeMapTexture, @ref CubeMapTextureArray, @ref RectangleTexture,
    @ref BufferTexture, @ref MultisampleTexture
@requires_gl30 %Extension @extension{EXT,texture_array}
@requires_gles30 %Array textures are not available in OpenGL ES 2.0.
@requires_gl 1D array textures are not available in OpenGL ES, only 2D ones.
@todo Fix this when @es_extension{NV,texture_array} is in ES2 extension headers
 */
template<UnsignedInt dimensions> class TextureArray: public AbstractTexture {
    public:
        static const UnsignedInt Dimensions = dimensions; /**< @brief %Texture dimension count */

        /**
         * @brief Max supported texture array size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{EXT,texture_array} (part of
         * OpenGL 3.0) is not available, returns zero vector.
         * @see @fn_gl{Get} with @def_gl{MAX_TEXTURE_SIZE} and
         *      @def_gl{MAX_ARRAY_TEXTURE_LAYERS}
         */
        static VectorTypeFor<dimensions+1, Int> maxSize();

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object.
         * @see @fn_gl{GenTextures} with @def_gl{TEXTURE_1D_ARRAY} or
         *      @def_gl{TEXTURE_2D_ARRAY}
         */
        explicit TextureArray(): AbstractTexture(Implementation::textureArrayTarget<dimensions>()) {}

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc Texture::setBaseLevel() */
        TextureArray<dimensions>& setBaseLevel(Int level) {
            AbstractTexture::setBaseLevel(level);
            return *this;
        }
        #endif

        /** @copydoc Texture::setMaxLevel() */
        TextureArray<dimensions>& setMaxLevel(Int level) {
            AbstractTexture::setMaxLevel(level);
            return *this;
        }

        /** @copydoc Texture::setMinificationFilter() */
        TextureArray<dimensions>& setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap = Sampler::Mipmap::Base) {
            AbstractTexture::setMinificationFilter(filter, mipmap);
            return *this;
        }

        /** @copydoc Texture::setMagnificationFilter() */
        TextureArray<dimensions>& setMagnificationFilter(Sampler::Filter filter) {
            AbstractTexture::setMagnificationFilter(filter);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc Texture::setMinLod() */
        TextureArray<dimensions>& setMinLod(Float lod) {
            AbstractTexture::setMinLod(lod);
            return *this;
        }

        /** @copydoc Texture::setMaxLod() */
        TextureArray<dimensions>& setMaxLod(Float lod) {
            AbstractTexture::setMaxLod(lod);
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /** @copydoc Texture::setLodBias() */
        TextureArray<dimensions>& setLodBias(Float bias) {
            AbstractTexture::setLodBias(bias);
            return *this;
        }
        #endif

        /** @copydoc Texture::setWrapping() */
        TextureArray<dimensions>& setWrapping(const Array<dimensions+1, Sampler::Wrapping>& wrapping) {
            DataHelper<dimensions+1>::setWrapping(*this, wrapping);
            return *this;
        }

        /** @copydoc Texture::setBorderColor(const Color4&) */
        TextureArray<dimensions>& setBorderColor(const Color4& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /** @copydoc Texture::setBorderColor(const Vector4ui&) */
        TextureArray<dimensions>& setBorderColor(const Vector4ui& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }

        /** @copydoc Texture::setBorderColor(const Vector4i&) */
        TextureArray<dimensions>& setBorderColor(const Vector4i& color) {
            AbstractTexture::setBorderColor(color);
            return *this;
        }
        #endif

        /** @copydoc Texture::setMaxAnisotropy() */
        TextureArray<dimensions>& setMaxAnisotropy(Float anisotropy) {
            AbstractTexture::setMaxAnisotropy(anisotropy);
            return *this;
        }

        /** @copydoc Texture::setSRGBDecode() */
        TextureArray<dimensions>& setSRGBDecode(bool decode) {
            AbstractTexture::setSRGBDecode(decode);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @copydoc Texture::setSwizzle() */
        template<char r, char g, char b, char a> TextureArray<dimensions>& setSwizzle() {
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
         *      @es_extension{NV,shadow_samplers_array}
         */
        TextureArray<dimensions>& setCompareMode(Sampler::CompareMode mode) {
            AbstractTexture::setCompareMode(mode);
            return *this;
        }

        /**
         * @copybrief Texture::setCompareFunction()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setCompareFunction() for more information.
         * @requires_gles30 %Extension @es_extension{EXT,shadow_samplers} and
         *      @es_extension{NV,shadow_samplers_array}
         */
        TextureArray<dimensions>& setCompareFunction(Sampler::CompareFunction function) {
            AbstractTexture::setCompareFunction(function);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief Texture::setDepthStencilMode()
         * @return Reference to self (for method chaining)
         *
         * See @ref Texture::setDepthStencilMode() for more information.
         */
        TextureArray<dimensions>& setDepthStencilMode(Sampler::DepthStencilMode mode) {
            AbstractTexture::setDepthStencilMode(mode);
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /** @copydoc Texture::imageSize() */
        VectorTypeFor<dimensions+1, Int> imageSize(Int level) {
            return DataHelper<dimensions+1>::imageSize(*this, _target, level);
        }
        #endif

        /**
         * @brief Set storage
         * @param levels            Mip level count
         * @param internalFormat    Internal format
         * @param size              Size of largest mip level
         * @return Reference to self (for method chaining)
         *
         * Specifies entire structure of a texture at once, removing the need
         * for additional consistency checks and memory reallocations when
         * updating the data later. After calling this function the texture
         * is immutable and calling @ref setStorage() or @ref setImage() is not
         * allowed.
         *
         * If @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some texture unit before the operation. If
         * @extension{ARB,texture_storage} (part of OpenGL 4.2) or OpenGL ES
         * 3.0 is not available, the feature is emulated with sequence of
         * @ref setImage() calls.
         * @see @ref maxSize(), @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexStorage2D}/@fn_gl{TexStorage3D} or
         *      @fn_gl_extension{TextureStorage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureStorage3D,EXT,direct_state_access},
         *      eventually @fn_gl{TexImage2D}/@fn_gl{TexImage3D} or
         *      @fn_gl_extension{TextureImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage3D,EXT,direct_state_access}.
         */
        TextureArray<dimensions>& setStorage(Int levels, TextureFormat internalFormat, const VectorTypeFor<dimensions+1, Int>& size) {
            DataHelper<dimensions+1>::setStorage(*this, _target, levels, internalFormat, size);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /** @copydoc Texture::image(Int, Image<dimensions>&) */
        void image(Int level, Image<dimensions+1>& image) {
            AbstractTexture::image<dimensions+1>(_target, level, image);
        }

        /** @copydoc Texture::imate(Int, BufferImage<dimensions>&, BufferUsage) */
        void image(Int level, BufferImage<dimensions+1>& image, BufferUsage usage) {
            AbstractTexture::image<dimensions+1>(_target, level, image, usage);
        }
        #endif

        /**
         * @brief Set image data
         * @param level             Mip level
         * @param internalFormat    Internal format
         * @param image             @ref Image, @ref ImageReference or
         *      @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * For better performance when generating mipmaps using
         * @ref generateMipmap() or calling @ref setImage() more than once use
         * @ref setStorage() and @ref setSubImage() instead.
         *
         * If @extension{EXT,direct_state_access} is not available, the
         * texture is bound to some texture unit before the operation.
         * @see @ref maxSize(), @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{TexImage2D}/@fn_gl{TexImage3D} or
         *      @fn_gl_extension{TextureImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureImage3D,EXT,direct_state_access}
         */
        TextureArray<dimensions>& setImage(Int level, TextureFormat internalFormat, const ImageReference<dimensions+1>& image) {
            DataHelper<dimensions+1>::setImage(*this, _target, level, internalFormat, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload */
        TextureArray<dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions+1>& image) {
            DataHelper<dimensions+1>::setImage(*this, _target, level, internalFormat, image);
            return *this;
        }

        /** @overload */
        TextureArray<dimensions>& setImage(Int level, TextureFormat internalFormat, BufferImage<dimensions+1>&& image) {
            return setImage(level, internalFormat, image);
        }
        #endif

        /**
         * @brief Set image subdata
         * @param level             Mip level
         * @param offset            Offset where to put data in the texture
         * @param image             @ref Image, @ref ImageReference or
         *      @ref Trade::ImageData of the same dimension count
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available, the
         * texture is bound to some texture unit before the operation.
         * @see @ref setStorage(), @ref setImage(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexSubImage2D}/@fn_gl{TexSubImage3D}
         *      or @fn_gl_extension{TextureSubImage2D,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureSubImage3D,EXT,direct_state_access}
         */
        TextureArray<dimensions>& setSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, const ImageReference<dimensions+1>& image) {
            DataHelper<dimensions+1>::setSubImage(*this, _target, level, offset, image);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload */
        TextureArray<dimensions>& setSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, BufferImage<dimensions+1>& image) {
            DataHelper<dimensions+1>::setSubImage(*this, _target, level, offset, image);
            return *this;
        }

        /** @overload */
        TextureArray<dimensions>& setSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, BufferImage<dimensions+1>&& image) {
            return setSubImage(level, offset, image);
        }
        #endif

        /** @copydoc Texture::generateMipmap() */
        TextureArray<dimensions>& generateMipmap() {
            AbstractTexture::generateMipmap();
            return *this;
        }

        /** @copydoc Texture::invalidateImage() */
        void invalidateImage(Int level) { AbstractTexture::invalidateImage(level); }

        /** @copydoc Texture::invalidateSubImage() */
        void invalidateSubImage(Int level, const VectorTypeFor<dimensions+1, Int>& offset, const VectorTypeFor<dimensions+1, Int>& size) {
            DataHelper<dimensions+1>::invalidateSubImage(*this, level, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        TextureArray<dimensions>& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        template<std::size_t size> TextureArray<dimensions>& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif
};

#ifndef MAGNUM_TARGET_GLES
/**
@brief One-dimensional texture array

@requires_gl Only @ref Magnum::Texture2DArray "Texture2DArray" is available in
    OpenGL ES.
*/
typedef TextureArray<1> Texture1DArray;
#endif

/** @brief Two-dimensional texture array */
typedef TextureArray<2> Texture2DArray;

}
#else
#error this header is not available on OpenGL ES 2.0 build
#endif

#endif
