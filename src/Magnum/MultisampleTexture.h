#ifndef Magnum_MultisampleTexture_h
#define Magnum_MultisampleTexture_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/** @file
 * @brief Class @ref Magnum::MultisampleTexture, typedef @ref Magnum::MultisampleTexture2D, @ref Magnum::MultisampleTexture2DArray
 */
#endif

#include "Magnum/AbstractTexture.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
namespace Magnum {

namespace Implementation {
    template<UnsignedInt> constexpr GLenum multisampleTextureTarget();
    template<> constexpr GLenum multisampleTextureTarget<2>() { return GL_TEXTURE_2D_MULTISAMPLE; }
    template<> constexpr GLenum multisampleTextureTarget<3>() {
        #ifndef MAGNUM_TARGET_GLES
        return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        #else
        return GL_TEXTURE_2D_MULTISAMPLE_ARRAY_OES;
        #endif
    }

    template<UnsignedInt dimensions> VectorTypeFor<dimensions, Int> maxMultisampleTextureSize();
    template<> MAGNUM_EXPORT Vector2i maxMultisampleTextureSize<2>();
    template<> MAGNUM_EXPORT Vector3i maxMultisampleTextureSize<3>();
}

/**
@brief Multisample texture sample locations

@see @ref MultisampleTexture::setStorage()
*/
enum class MultisampleTextureSampleLocations: GLboolean {
    NotFixed = GL_FALSE,
    Fixed = GL_TRUE
};

/**
@brief Mulitsample texture

Template class for 2D mulitsample texture and 2D multisample texture array.
Used only from shaders for manual multisample resolve and other operations. See
also @ref AbstractTexture documentation for more information.

## Usage

As multisample textures have no sampler state, the only thing you need is to
set storage:
@code
MultisampleTexture2D texture;
texture.setStorage(16, TextureFormat::RGBA8, {1024, 1024});
@endcode

In shader, the texture is used via `sampler2DMS`/`sampler2DMSArray`,
`isampler2DMS`/`isampler2DMSArray` or `usampler2DMS`/`usampler2DMSArray`. See
@ref AbstractShaderProgram documentation for more information about usage in
shaders.

Note that multisample textures don't support compressed formats.

@see @ref MultisampleTexture2D, @ref MultisampleTexture2DArray, @ref Texture,
    @ref TextureArray, @ref CubeMapTexture, @ref CubeMapTextureArray,
    @ref RectangleTexture, @ref BufferTexture
@requires_gl32 Extension @extension{ARB,texture_multisample}
@requires_gles31 Multisample 2D textures are not available in OpenGL ES 3.0 and
    older.
@requires_gles30 Multisample 2D array textures are not defined in OpenGL ES
    2.0.
@requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
    @extension{OES,texture_storage_multisample_2d_array} for multisample 2D
    array textures.
@requires_gles Multisample textures are not available in WebGL.
 */
template<UnsignedInt dimensions> class MultisampleTexture: public AbstractTexture {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Texture dimension count */
        };

        /**
         * @brief Max supported multisample texture size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither extension @extension{ARB,texture_multisample}
         * (part of OpenGL 3.2) nor OpenGL ES 3.1 is available, returns zero
         * vector.
         * @see @fn_gl{Get} with @def_gl{MAX_TEXTURE_SIZE} and
         *      @def_gl{MAX_3D_TEXTURE_SIZE}
         */
        static VectorTypeFor<dimensions, Int> maxSize() {
            return Implementation::maxMultisampleTextureSize<dimensions>();
        }

        /**
         * @brief Wrap existing OpenGL multisample texture object
         * @param id            OpenGL multisample texture ID
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL texture object
         * with target @def_gl{TEXTURE_2D_MULTISAMPLE} or
         * @def_gl{TEXTURE_2D_MULTISAMPLE_ARRAY} based on dimension count.
         * Unlike texture created using constructor, the OpenGL object is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static MultisampleTexture<dimensions> wrap(GLuint id, ObjectFlags flags = {}) {
            return MultisampleTexture<dimensions>{id, flags};
        }

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the texture is created on
         * first use.
         * @see @ref MultisampleTexture(NoCreateT), @ref wrap(),
         *      @fn_gl{CreateTextures} with @def_gl{TEXTURE_2D_MULTISAMPLE} or
         *      @def_gl{TEXTURE_2D_MULTISAMPLE_ARRAY}, eventually
         *      @fn_gl{GenTextures}
         */
        explicit MultisampleTexture(): AbstractTexture(Implementation::multisampleTextureTarget<dimensions>()) {}

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * @see @ref MultisampleTexture(), @ref wrap()
         */
        explicit MultisampleTexture(NoCreateT) noexcept: AbstractTexture{NoCreate, Implementation::multisampleTextureTarget<dimensions>()} {}

        /**
         * @brief Bind texture to given image unit
         * @param imageUnit Image unit
         * @param access    Image access
         * @param format    Image format
         *
         * Available only on 2D multisample textures.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bindImages(Int, std::initializer_list<AbstractTexture*>),
         *      @ref bindImageLayered(), @ref unbindImage(), @ref unbindImages(),
         *      @ref AbstractShaderProgram::maxImageUnits(),
         *      @fn_gl{BindImageTexture}
         * @requires_gl42 Extension @extension{ARB,shader_image_load_store}
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, class = typename std::enable_if<d == 2>::type>
        #endif
        void bindImage(Int imageUnit, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, 0, false, 0, access, format);
        }

        /**
         * @brief Bind texture layer to given image unit
         * @param imageUnit Image unit
         * @param layer     Texture layer
         * @param access    Image access
         * @param format    Image format
         *
         * Available only on 2D multisample texture arrays.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bindImages(Int, std::initializer_list<AbstractTexture*>),
         *      @ref bindImageLayered(), @ref unbindImage(), @ref unbindImages(),
         *      @ref AbstractShaderProgram::maxImageUnits(),
         *      @fn_gl{BindImageTexture}
         * @requires_gl42 Extension @extension{ARB,shader_image_load_store}
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{OES,texture_storage_multisample_2d_array} for
         *      multisample 2D array textures.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, class = typename std::enable_if<d == 3>::type>
        #endif
        void bindImage(Int imageUnit, Int layer, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, 0, false, layer, access, format);
        }

        /**
         * @brief Bind layered texture to given image unit
         * @param imageUnit Image unit
         * @param access    Image access
         * @param format    Image format
         *
         * Available only on 2D multisample texture arrays.
         * @note This function is meant to be used only internally from
         *      @ref AbstractShaderProgram subclasses. See its documentation
         *      for more information.
         * @see @ref bindImages(Int, std::initializer_list<AbstractTexture*>),
         *      @ref bindImage(), @ref unbindImages(), @ref unbindImage(),
         *      @ref AbstractShaderProgram::maxImageUnits(),
         *      @fn_gl{BindImageTexture}
         * @requires_gl42 Extension @extension{ARB,shader_image_load_store}
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{OES,texture_storage_multisample_2d_array} for
         *      multisample 2D array textures.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, class = typename std::enable_if<d == 3>::type>
        #endif
        void bindImageLayered(Int imageUnit, ImageAccess access, ImageFormat format) {
            bindImageInternal(imageUnit, 0, true, 0, access, format);
        }

        /**
         * @brief Set storage
         * @param samples           Sample count
         * @param internalFormat    Internal format
         * @param size              Texture size
         * @param sampleLocations   Whether to use fixed sample locations
         * @return Reference to self (for method chaining)
         *
         * After calling this function the texture is immutable and calling
         * @ref setStorage() again is not allowed.
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already). If @extension{ARB,texture_storage_multisample} (part of
         * OpenGL 4.3) is not available, the texture is bound and the feature
         * is emulated using plain @extension{ARB,texture_multisample}
         * functionality.
         * @see @ref maxSize(), @ref maxColorSamples(), @ref maxDepthSamples(),
         *      @ref maxIntegerSamples(), @fn_gl2{TextureStorage2DMultisample,TexStorage2DMultisample} /
         *      @fn_gl2{TextureStorage3DMultisample,TexStorage3DMultisample},
         *      @fn_gl_extension{TextureStorage2DMultisample,EXT,direct_state_access} /
         *      @fn_gl_extension{TextureStorage3DMultisample,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture}
         *      and @fn_gl{TexStorage2DMultisample} / @fn_gl{TexStorage3DMultisample}
         *      or @fn_gl{TexImage2DMultisample} / @fn_gl{TexImage3DMultisample}
         * @todoc Remove the workaround when it stops breaking Doxygen layout so badly
         */
        /* The default parameter value was chosen based on discussion in
           ARB_texture_multisample specs (fixed locations is treated as the
           special case) */
        MultisampleTexture<dimensions>& setStorage(Int samples, TextureFormat internalFormat, const VectorTypeFor<dimensions, Int>& size, MultisampleTextureSampleLocations sampleLocations =
            #ifndef DOXYGEN_GENERATING_OUTPUT
            MultisampleTextureSampleLocations::NotFixed
            #else
            NotFixed
            #endif
        ) {
            DataHelper<dimensions>::setStorageMultisample(*this, samples, internalFormat, size, GLboolean(sampleLocations));
            return *this;
        }

        /**
         * @brief Texture image size
         *
         * See @ref Texture::imageSize() for more information.
         * @requires_gles31 Texture image size queries are not available in
         *      OpenGL ES 3.0 and older.
         */
        VectorTypeFor<dimensions, Int> imageSize() {
            return DataHelper<dimensions>::imageSize(*this, 0);
        }

        /**
         * @copybrief Texture::invalidateImage()
         *
         * See @ref Texture::invalidateImage() for more information.
         */
        void invalidateImage() { AbstractTexture::invalidateImage(0); }

        /**
         * @copybrief Texture::invalidateSubImage()
         *
         * See @ref Texture::invalidateSubImage() for more information.
         */
        void invalidateSubImage(const VectorTypeFor<dimensions, Int>& offset, const VectorTypeFor<dimensions, Int>& size) {
            DataHelper<dimensions>::invalidateSubImage(*this, 0, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        MultisampleTexture<dimensions>& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        template<std::size_t size> MultisampleTexture<dimensions>& setLabel(const char(&label)[size]) {
            AbstractTexture::setLabel<size>(label);
            return *this;
        }
        #endif

    private:
        explicit MultisampleTexture(GLuint id, ObjectFlags flags): AbstractTexture{id, Implementation::multisampleTextureTarget<dimensions>(), flags} {}
};

/**
@brief Two-dimensional multisample texture

@requires_gl32 Extension @extension{ARB,texture_multisample}
@requires_gles31 Multisample textures are not available in OpenGL ES 3.0 and
    older.
@requires_gles Multisample textures are not available in WebGL.
*/
typedef MultisampleTexture<2> MultisampleTexture2D;

/**
@brief Two-dimensional multisample texture array

@requires_gl32 Extension @extension{ARB,texture_multisample}
@requires_gles30 Not defined in OpenGL ES 2.0.
@requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
    @extension{OES,texture_storage_multisample_2d_array}
@requires_gles Multisample textures are not available in WebGL.
*/
typedef MultisampleTexture<3> MultisampleTexture2DArray;

}
#else
#error this header is not available in OpenGL ES 2.0 and WebGL build
#endif

#endif
