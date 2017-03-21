#ifndef Magnum_Sampler_h
#define Magnum_Sampler_h
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

/** @file
 * @brief Class @ref Magnum::Sampler
 */

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief Texture sampler

@see @ref Texture, @ref TextureArray, @ref CubeMapTexture,
    @ref CubeMapTextureArray, @ref RectangleTexture
*/
class MAGNUM_EXPORT Sampler {
    public:
        /**
         * @brief Texture filtering
         *
         * @see @ref Texture::setMinificationFilter() "*Texture::setMinificationFilter()",
         *      @ref Texture::setMagnificationFilter() "*Texture::setMagnificationFilter()"
         */
        enum class Filter: GLint {
            Nearest = GL_NEAREST,   /**< Nearest neighbor filtering */

            /**
             * Linear interpolation filtering.
             * @requires_gles30 Extension @extension{OES,texture_float_linear} /
             *      @extension2{OES,texture_half_float_linear,OES_texture_float_linear}
             *      for linear interpolation of textures with
             *      @ref TextureFormat::HalfFloat / @ref TextureFormat::Float
             *      in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{OES,texture_float_linear}
             *      / @webgl_extension{OES,texture_half_float_linear} for
             *      linear interpolation of textures with @ref TextureFormat::HalfFloat
             *      / @ref TextureFormat::Float in WebGL 1.0.
             */
            Linear = GL_LINEAR
        };

        /**
         * @brief Mip level selection
         *
         * @see @ref Texture::setMinificationFilter() "*Texture::setMinificationFilter()"
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
             * @requires_gles30 Extension @extension{OES,texture_float_linear} /
             *      @extension2{OES,texture_half_float_linear,OES_texture_float_linear}
             *      for linear interpolation of textures with
             *      @ref TextureFormat::HalfFloat / @ref TextureFormat::Float
             *      in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{OES,texture_float_linear}
             *      / @webgl_extension{OES,texture_half_float_linear} for
             *      linear interpolation of textures with @ref TextureFormat::HalfFloat
             *      / @ref TextureFormat::Float in WebGL 1.0.
             */
            Linear = GL_NEAREST_MIPMAP_LINEAR & ~GL_NEAREST
        };

        /**
         * @brief Texture wrapping
         *
         * @see @ref Texture::setWrapping() "*Texture::setWrapping()"
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

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Clamp to border color. Coordinates out of range will be clamped
             * to border color (set with
             * @ref Texture::setBorderColor() "*Texture::setBorderColor()").
             * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
             *      @extension{EXT,texture_border_clamp} or
             *      @extension{NV,texture_border_clamp}
             * @requires_gles Border clamp is not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            ClampToBorder = GL_CLAMP_TO_BORDER,
            #else
            ClampToBorder = GL_CLAMP_TO_BORDER_EXT,
            #endif
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Mirror the texture once in negative coordinates and clamp to
             * edge after that. **Unavailable on rectangle textures.**
             * @requires_gl44 Extension @extension{ARB,texture_mirror_clamp_to_edge},
             *      @extension{ATI,texture_mirror_once} or @extension{EXT,texture_mirror_clamp}
             * @requires_gl Only separate @ref Wrapping::MirroredRepeat or
             *      @ref Wrapping::ClampToEdge is available in OpenGL ES and
             *      WebGL.
             */
            MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
            #endif
        };

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Depth texture comparison mode
         *
         * @see @ref CompareFunction,
         *      @ref Texture::setCompareMode() "*Texture::setCompareMode()"
         * @requires_gles30 Extension @extension{EXT,shadow_samplers} in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Depth texture comparison is not available in WebGL
         *      1.0.
         */
        enum class CompareMode: GLenum {
            /** Directly output the depth value */
            None = GL_NONE,

            /** Use output from specified @ref CompareFunction */
            CompareRefToTexture =
                #ifndef MAGNUM_TARGET_GLES2
                GL_COMPARE_REF_TO_TEXTURE
                #else
                GL_COMPARE_REF_TO_TEXTURE_EXT
                #endif
        };

        /**
         * @brief Depth texture comparison function
         *
         * Comparison operator used when comparison mode is set to
         * @ref CompareMode::CompareRefToTexture.
         * @see @ref Texture::setCompareFunction() "*Texture::setCompareFunction()",
         *      @ref Texture::setCompareMode() "*Texture::setCompareMode()"
         * @requires_gles30 Extension @extension{EXT,shadow_samplers} in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Depth texture comparison is not available in WebGL
         *      1.0.
         */
        enum class CompareFunction: GLenum {
            Never = GL_NEVER,           /**< Always `0.0` */
            Always = GL_ALWAYS,         /**< Always `1.0` */

            /**
             * `1.0` when texture coordinate is less than depth value, `0.0`
             * otherwise
             */
            Less = GL_LESS,

            /**
             * `1.0` when texture coordinate is less than or equal to depth
             * value, `0.0` otherwise
             */
            LessOrEqual = GL_LEQUAL,

            /**
             * `1.0` when texture coordinate is equal to depth value, `0.0`
             * otherwise
             */
            Equal = GL_EQUAL,

            /**
             * `0.0` when texture coordinate is equal to depth value, `1.0`
             * otherwise
             */
            NotEqual = GL_NOTEQUAL,

            /**
             * `1.0` when texture coordinate is greater than or equal to depth
             * value, `0.0` otherwise
             */
            GreaterOrEqual = GL_GEQUAL,

            /**
             * `1.0` when texture coordinate is greater than depth value, `0.0`
             * otherwise
             */
            Greater = GL_GREATER
        };
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Depth/stencil texture mode
         *
         * @see @ref Texture::setDepthStencilMode() "*Texture::setDepthStencilMode()"
         * @requires_gl43 Extension @extension{ARB,stencil_texturing}
         * @requires_gles31 Stencil texturing is not available in OpenGL ES 3.0
         *      and older.
         * @requires_gles Stencil texturing is not available in WebGL.
         */
        enum class DepthStencilMode: GLenum {
            /** Sample depth component */
            DepthComponent = GL_DEPTH_COMPONENT,

            /** Sample stencil index (as unsigned integer texture) */
            StencilIndex = GL_STENCIL_INDEX
        };
        #endif

        /**
         * @brief Max supported max anisotropy
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{EXT,texture_filter_anisotropic}
         * (desktop or ES) is not available, returns `0.0f`.
         * @see @fn_gl{Get} with @def_gl{MAX_TEXTURE_MAX_ANISOTROPY_EXT}
         */
        static Float maxMaxAnisotropy();
};

/** @debugoperatorclassenum{Magnum::Sampler,Magnum::Sampler::Filter} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Sampler::Filter value);

/** @debugoperatorclassenum{Magnum::Sampler,Magnum::Sampler::Mipmap} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Sampler::Mipmap value);

/** @debugoperatorclassenum{Magnum::Sampler,Magnum::Sampler::Wrapping} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Sampler::Wrapping value);

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
/** @debugoperatorclassenum{Magnum::Sampler,Magnum::Sampler::CompareMode} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Sampler::CompareMode value);

/** @debugoperatorclassenum{Magnum::Sampler,Magnum::Sampler::CompareFunction} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Sampler::CompareFunction value);
#endif

#ifndef MAGNUM_TARGET_GLES
/** @debugoperatorclassenum{Magnum::Sampler,Magnum::Sampler::DepthStencilMode} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Sampler::DepthStencilMode value);
#endif

}

#endif
