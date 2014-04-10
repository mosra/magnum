#ifndef Magnum_MultisampleTexture_h
#define Magnum_MultisampleTexture_h
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
 * @brief Class @ref Magnum::MultisampleTexture, typedef @ref Magnum::MultisampleTexture2D, @ref Magnum::MultisampleTexture2DArray
 */
#endif

#include "Magnum/AbstractTexture.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"

#ifndef MAGNUM_TARGET_GLES
namespace Magnum {

namespace Implementation {
    template<UnsignedInt> constexpr GLenum multisampleTextureTarget();
    template<> inline constexpr GLenum multisampleTextureTarget<2>() { return GL_TEXTURE_2D_MULTISAMPLE; }
    template<> inline constexpr GLenum multisampleTextureTarget<3>() { return GL_TEXTURE_2D_MULTISAMPLE_ARRAY; }
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

@section Texture-usage Usage

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

@see @ref MultisampleTexture2D, @ref MultisampleTexture2DArray, @ref Texture,
    @ref TextureArray, @ref CubeMapTexture, @ref CubeMapTextureArray,
    @ref RectangleTexture, @ref BufferTexture
@requires_gl32 %Extension @extension{ARB,texture_multisample}
@requires_gl Multisample textures are not available in OpenGL ES.
 */
template<UnsignedInt dimensions> class MultisampleTexture: public AbstractTexture {
    public:
        static const UnsignedInt Dimensions = dimensions; /**< @brief %Texture dimension count */

        /**
         * @brief Constructor
         *
         * Creates new OpenGL texture object.
         * @see @fn_gl{GenTextures} with @def_gl{TEXTURE_2D_MULTISAMPLE} or
         *      @def_gl{TEXTURE_2D_MULTISAMPLE_ARRAY}
         */
        explicit MultisampleTexture(): AbstractTexture(Implementation::multisampleTextureTarget<dimensions>()) {}

        /**
         * @brief %Image size
         *
         * The result is not cached in any way. If
         * @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some texture unit before the operation.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{GetTexLevelParameter} or @fn_gl_extension{GetTextureLevelParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_WIDTH}, @def_gl{TEXTURE_HEIGHT} or
         *      @def_gl{TEXTURE_DEPTH}
         */
        typename DimensionTraits<dimensions, Int>::VectorType imageSize() {
            return DataHelper<dimensions>::imageSize(*this, _target, 0);
        }

        /**
         * @brief Set storage
         * @param samples           Sample count
         * @param internalFormat    Internal format
         * @param size              %Texture size
         * @param sampleLocations   Whether to use fixed sample locations
         * @return Reference to self (for method chaining)
         *
         * After calling this function the texture is immutable and calling
         * @ref setStorage() again is not allowed.
         *
         * If @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some texture unit before the operation. If
         * @extension{ARB,texture_storage_multisample} (part of OpenGL 4.3) is
         * not available, the feature is emulated using plain
         * @extension{ARB,texture_storage} functionality (which unfortunately
         * doesn't have any DSA alternative, so the texture must be bound
         * to some texture unit before).
         * @see @ref maxColorSamples(), @ref maxDepthSamples(),
         *      @ref maxIntegerSamples(), @fn_gl{ActiveTexture}, @fn_gl{BindTexture}
         *      and @fn_gl{TexStorage2DMultisample}/@fn_gl{TexStorage3DMultisample}
         *      or @fn_gl_extension{TextureStorage2DMultisample,EXT,direct_state_access}/
         *      @fn_gl_extension{TextureStorage3DMultisample,EXT,direct_state_access}
         *      eventually @fn_gl{TexImage2DMultisample}/@fn_gl{TexImage3DMultisample}
         * @todoc Remove the workaround when it stops breaking Doxygen layout so badly
         */
        /* The default parameter value was chosen based on discussion in
           ARB_texture_multisample specs (fixed locations is treated as the
           special case) */
        MultisampleTexture<dimensions>& setStorage(Int samples, TextureFormat internalFormat, const typename DimensionTraits<dimensions, Int>::VectorType& size, MultisampleTextureSampleLocations sampleLocations =
            #ifndef DOXYGEN_GENERATING_OUTPUT
            MultisampleTextureSampleLocations::NotFixed
            #else
            NotFixed
            #endif
        ) {
            DataHelper<dimensions>::setStorageMultisample(*this, _target, samples, internalFormat, size, GLboolean(sampleLocations));
            return *this;
        }

        /** @copydoc RectangleTexture::invalidateImage() */
        void invalidateImage() { AbstractTexture::invalidateImage(0); }

        /** @copydoc RectangleTexture::invalidateSubImage() */
        void invalidateSubImage(const typename DimensionTraits<dimensions, Int>::VectorType& offset, const typename DimensionTraits<dimensions, Int>::VectorType& size) {
            DataHelper<dimensions>::invalidateSubImage(*this, 0, offset, size);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        MultisampleTexture<dimensions>& setLabel(const std::string& label) {
            AbstractTexture::setLabel(label);
            return *this;
        }
        #endif
};

/** @brief Two-dimensional multisample texture */
typedef MultisampleTexture<2> MultisampleTexture2D;

/** @brief Two-dimensional multisample texture array */
typedef MultisampleTexture<3> MultisampleTexture2DArray;

}
#else
#error this header is available only on desktop OpenGL build
#endif

#endif
