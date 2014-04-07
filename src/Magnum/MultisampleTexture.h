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
@brief Mulitsample texture

Template class for 2D mulitsample texture and 2D multisample texture array. See
also @ref AbstractTexture documentation for more information.

@todoc Finish when fully implemented

In shader, the texture is used via `sampler2DMS`/`sampler2DMSArray`,
`isampler2DMS`/`isampler2DMSArray` or `usampler2DMS`/`usampler2DMSArray`. See
@ref AbstractShaderProgram documentation for more information about usage in
shaders.

@requires_gl32 %Extension @extension{ARB,texture_multisample}
@requires_gl Multisample textures are not available in OpenGL ES.

@see @ref MultisampleTexture2D, @ref MultisampleTexture2DArray, @ref Texture,
    @ref TextureArray, @ref BufferTexture, @ref CubeMapTexture,
    @ref CubeMapTextureArray, @ref RectangleTexture
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

        /** @copydoc Texture::imageSize() */
        typename DimensionTraits<dimensions, Int>::VectorType imageSize(Int level) {
            return DataHelper<dimensions>::imageSize(*this, _target, level);
        }

        /** @copydoc Texture::invalidateImage() */
        void invalidateImage(Int level) { AbstractTexture::invalidateImage(level); }

        /** @copydoc Texture::invalidateSubImage() */
        void invalidateSubImage(Int level, const typename DimensionTraits<dimensions, Int>::VectorType& offset, const typename DimensionTraits<dimensions, Int>::VectorType& size) {
            DataHelper<dimensions>::invalidateSubImage(*this, level, offset, size);
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
