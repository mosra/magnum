#ifndef Magnum_Sampler_h
#define Magnum_Sampler_h
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
 * @brief Class Magnum::Sampler
 */

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief %Texture sampler

@see Texture, CubeMapTexture, CubeMapTextureArray
*/
class MAGNUM_EXPORT Sampler {
    public:
        /**
         * @brief %Texture filtering
         *
         * @see setMagnificationFilter() and setMinificationFilter()
         */
        enum class Filter: GLint {
            Nearest = GL_NEAREST,   /**< Nearest neighbor filtering */

            /**
             * Linear interpolation filtering.
             * @requires_gles30 %Extension @es_extension{OES,texture_float_linear} /
             *      @es_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
             *      for linear interpolation of textures with
             *      @ref Magnum::TextureFormat "TextureFormat::HalfFloat" /
             *      @ref Magnum::TextureFormat "TextureFormat::Float" in OpenGL
             *      ES 2.0
             */
            Linear = GL_LINEAR
        };

        /**
         * @brief Mip level selection
         *
         * @see setMinificationFilter()
         */
        enum class Mipmap: GLint {
            Base = GL_NEAREST & ~GL_NEAREST, /**< Select base mip level */

            /**
             * Select nearest mip level. **Unavailable on rectangle textures.**
             */
            Nearest = GL_NEAREST_MIPMAP_NEAREST & ~GL_NEAREST,

            /**
             * Linear interpolation of nearest mip levels. **Unavailable on
             * rectangle textures.**
             * @requires_gles30 %Extension @es_extension{OES,texture_float_linear} /
             *      @es_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
             *      for linear interpolation of textures with
             *      @ref Magnum::TextureFormat "TextureFormat::HalfFloat" /
             *      @ref Magnum::TextureFormat "TextureFormat::Float" in OpenGL
             *      ES 2.0
             */
            Linear = GL_NEAREST_MIPMAP_LINEAR & ~GL_NEAREST
        };

        /**
         * @brief %Texture wrapping
         *
         * @see setWrapping()
         */
        enum class Wrapping: GLint {
            /** Repeat texture. **Unavailable on rectangle textures.** */
            Repeat = GL_REPEAT,

            /**
             * Repeat mirrored texture. **Unavailable on rectangle textures.**
             */
            MirroredRepeat = GL_MIRRORED_REPEAT,

            /**
             * Clamp to edge. Coordinates out of the range will be clamped to
             * first / last column / row in given direction.
             */
            ClampToEdge = GL_CLAMP_TO_EDGE,

            /**
             * Clamp to border color. Coordinates out of range will be clamped
             * to border color (set with setBorderColor()).
             * @requires_es_extension %Extension @es_extension{NV,texture_border_clamp}
             */
            #ifndef MAGNUM_TARGET_GLES
            ClampToBorder = GL_CLAMP_TO_BORDER,
            #else
            ClampToBorder = GL_CLAMP_TO_BORDER_NV,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Mirror the texture once in negative coordinates and clamp to
             * edge after that. **Unavailable on rectangle textures.**
             * @requires_gl44 %Extension @extension{ARB,texture_mirror_clamp_to_edge},
             *      @extension{ATI,texture_mirror_once} or @extension{EXT,texture_mirror_clamp}
             * @requires_gl Only separate @ref Magnum::Sampler::Wrapping "Wrapping::MirroredRepeat"
             *      or @ref Magnum::Sampler::Wrapping "Wrapping::ClampToEdge"
             *      is available in OpenGL ES.
             */
            MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
            #endif
        };

        /**
         * @brief Max supported max anisotropy
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{EXT,texture_filter_anisotropic}
         * (desktop or ES) is not available, returns `0.0f`.
         * @see setMaxAnisotropy(), @fn_gl{Get} with @def_gl{MAX_TEXTURE_MAX_ANISOTROPY_EXT}
         */
        static Float maxMaxAnisotropy();

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief maxMaxAnisotropy()
         * @deprecated Use @ref Magnum::Sampler::maxMaxAnisotropy() "maxMaxAnisotropy()"
         *      instead.
         */
        static CORRADE_DEPRECATED("use maxMaxAnisotropy() instead") Float maxAnisotropy() { return maxMaxAnisotropy(); }

        /**
         * @copybrief maxMaxAnisotropy()
         * @deprecated Use @ref Magnum::Sampler::maxMaxAnisotropy() "maxMaxAnisotropy()"
         *      instead.
         */
        static CORRADE_DEPRECATED("use maxMaxAnisotropy() instead") Float maxSupportedAnisotropy() { return maxMaxAnisotropy(); }
        #endif
};

/** @debugoperator{Magnum::Sampler} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Sampler::Filter value);

/** @debugoperator{Magnum::Sampler} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Sampler::Mipmap value);

/** @debugoperator{Magnum::Sampler} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Sampler::Wrapping value);

}

#endif
