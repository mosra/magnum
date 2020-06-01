#ifndef Magnum_GL_Sampler_h
#define Magnum_GL_Sampler_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::GL::Sampler, enum @ref Magnum::GL::SamplerFilter, @ref Magnum::GL::SamplerMipmap, @ref Magnum::GL::SamplerWrapping, @ref Magnum::GL::SamplerCompareMode, @ref Magnum::GL::SamplerCompareFunction, @ref Magnum::GL::SamplerDepthStencilMode, function @ref Magnum::GL::samplerFilter(), @ref Magnum::GL::samplerMipmap(), @ref Magnum::GL::hasSamplerWrapping(), @ref Magnum::GL::samplerWrapping()
 */

#include "Magnum/Array.h"
#include "Magnum/Magnum.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/GL/visibility.h"

namespace Magnum { namespace GL {

/**
@brief Texture sampler filtering

@see @ref Magnum::SamplerFilter, @ref samplerFilter(),
    @ref Texture::setMinificationFilter() "*Texture::setMinificationFilter()",
    @ref Texture::setMagnificationFilter() "*Texture::setMagnificationFilter()"
@m_enum_values_as_keywords
*/
enum class SamplerFilter: GLint {
    Nearest = GL_NEAREST,   /**< Nearest neighbor filtering */

    /**
     * Linear interpolation filtering. Can't be used with integer
     * texture formats.
     * @requires_es_extension Extension @gl_extension{OES,texture_float_linear}
     *      for linear interpolation of textures with float @ref TextureFormat
     * @requires_webgl_extension Extensiion @webgl_extension{OES,texture_float_linear}
     *      for linear interpolation of textures with float @ref TextureFormat
     * @requires_gles30 Extension @gl_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
     *      for linear interpolation of textures with half-float
     *      @ref TextureFormat in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{OES,texture_half_float_linear}
     *      for linear interpolation of textures with half-float
     *      @ref TextureFormat in WebGL 1.0.
     */
    Linear = GL_LINEAR
};

/**
@brief Convert generic texture sampler filter mode to OpenGL filter mode

@see @ref samplerMipmap(), @ref samplerWrapping()
 */
MAGNUM_GL_EXPORT SamplerFilter samplerFilter(Magnum::SamplerFilter filter);

/** @debugoperatorenum{SamplerFilter} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SamplerFilter value);

/**
@brief Texture sampler mip level selection

@see @ref Magnum::SamplerMipmap, @ref samplerMipmap(),
    @ref Texture::setMinificationFilter() "*Texture::setMinificationFilter()"
*/
enum class SamplerMipmap: GLint {
    /**
     * Select base mip level
     * @m_keywords{GL_NEAREST GL_LINEAR}
     */
    Base = GL_NEAREST & ~GL_NEAREST,

    /**
     * Select nearest mip level. **Unavailable on rectangle textures.**
     * @m_keywords{GL_NEAREST_MIPMAP_NEAREST GL_LINEAR_MIPMAP_NEAREST}
     */
    Nearest = GL_NEAREST_MIPMAP_NEAREST & ~GL_NEAREST,

    /**
     * Linear interpolation of nearest mip levels. **Unavailable on
     * rectangle textures.**
     * @m_keywords{GL_NEAREST_MIPMAP_LINEAR GL_LINEAR_MIPMAP_LINEAR}
     * @requires_es_extension Extension @gl_extension{OES,texture_float_linear}
     *      for linear interpolation of textures with float @ref TextureFormat
     * @requires_webgl_extension Extensiion @webgl_extension{OES,texture_float_linear}
     *      for linear interpolation of textures with float @ref TextureFormat
     * @requires_gles30 Extension @gl_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
     *      for linear interpolation of textures with half-float
     *      @ref TextureFormat in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{OES,texture_half_float_linear}
     *      for linear interpolation of textures with half-float
     *      @ref TextureFormat in WebGL 1.0.
     */
    Linear = GL_NEAREST_MIPMAP_LINEAR & ~GL_NEAREST
};

/**
@brief Convert generic sampler filter mode to OpenGL filter mode

@see @ref samplerFilter(), @ref samplerWrapping()
 */
MAGNUM_GL_EXPORT SamplerMipmap samplerMipmap(Magnum::SamplerMipmap mipmap);

/** @debugoperatorenum{SamplerMipmap} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SamplerMipmap value);

/**
@brief Texture sampler wrapping

@see @ref Magnum::SamplerWrapping, @ref samplerWrapping(),
    @ref Texture::setWrapping() "*Texture::setWrapping()"
@m_enum_values_as_keywords
*/
enum class SamplerWrapping: GLint {
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
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension{EXT,texture_border_clamp} or
     *      @gl_extension{NV,texture_border_clamp}
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
     * @requires_gl44 Extension @gl_extension{ARB,texture_mirror_clamp_to_edge},
     *      @gl_extension{ATI,texture_mirror_once} or @gl_extension{EXT,texture_mirror_clamp}
     * @requires_gl Only separate @ref SamplerWrapping::MirroredRepeat or
     *      @ref SamplerWrapping::ClampToEdge is available in OpenGL ES and
     *      WebGL.
     */
    MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
    #endif
};

/**
@brief Check availability of a generic sampler wrapping mode

Some OpenGL targets don't support all generic sampler wrapping modes (for
example the @ref SamplerWrapping::MirrorClampToEdge is not available in OpenGL
ES or WebGL). Returns @cpp false @ce if current target can't support such
format, @cpp true @ce otherwise. The @p filter value is expected to be valid.

@note Support of some formats depends on presence of a particular OpenGL
    extension. Such check is outside of the scope of this function and you are
    expected to verify extension availability before using such format.

@see @see @ref samplerWrapping(), @ref samplerFilter(), @ref samplerMipmap()
 */
MAGNUM_GL_EXPORT bool hasSamplerWrapping(Magnum::SamplerWrapping wrapping);

/**
@brief Convert generic sampler filter mode to OpenGL filter mode

Not all generic sampler wrapping modes may be available on all targets and this
function expects that given format is available on the target. Use
@ref hasSamplerWrapping() to query availability of given mode.
@see @ref samplerFilter(), @ref samplerMipmap()
 */
MAGNUM_GL_EXPORT SamplerWrapping samplerWrapping(Magnum::SamplerWrapping wrapping);

/** @overload */
template<UnsignedInt dimensions> Array<dimensions, SamplerWrapping> samplerWrapping(const Array<dimensions, Magnum::SamplerWrapping>& wrapping) {
    Array<dimensions, SamplerWrapping> out; /** @todo NoInit */
    for(std::size_t i = 0; i != dimensions; ++i)
        out[i] = samplerWrapping(wrapping[i]);
    return out;
}

/** @debugoperatorenum{SamplerWrapping} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SamplerWrapping value);

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
/**
@brief Depth texture comparison mode

@see @ref SamplerCompareFunction,
    @ref Texture::setCompareMode() "*Texture::setCompareMode()"
@m_enum_values_as_keywords
@requires_gles30 Extension @gl_extension{EXT,shadow_samplers} in
    OpenGL ES 2.0.
@requires_webgl20 Depth texture comparison is not available in WebGL
    1.0.
*/
enum class SamplerCompareMode: GLenum {
    /** Directly output the depth value */
    None = GL_NONE,

    /** Use output from specified @ref SamplerCompareFunction */
    CompareRefToTexture =
        #ifndef MAGNUM_TARGET_GLES2
        GL_COMPARE_REF_TO_TEXTURE
        #else
        GL_COMPARE_REF_TO_TEXTURE_EXT
        #endif
};

/** @debugoperatorenum{SamplerCompareMode} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SamplerCompareMode value);

/**
@brief Texture sampler depth comparison function

Comparison operator used when comparison mode is set to
@ref SamplerCompareMode::CompareRefToTexture.
@see @ref Texture::setCompareFunction() "*Texture::setCompareFunction()",
    @ref Texture::setCompareMode() "*Texture::setCompareMode()"
@m_enum_values_as_keywords
@requires_gles30 Extension @gl_extension{EXT,shadow_samplers} in
    OpenGL ES 2.0.
@requires_webgl20 Depth texture comparison is not available in WebGL
    1.0.
*/
enum class SamplerCompareFunction: GLenum {
    Never = GL_NEVER,           /**< Always @glsl 0.0 @ce */
    Always = GL_ALWAYS,         /**< Always @glsl 1.0 @ce */

    /**
     * @glsl 1.0 @ce when texture coordinate is less than depth value,
     * @glsl 0.0 @ce otherwise
     */
    Less = GL_LESS,

    /**
     * @glsl 1.0 @ce when texture coordinate is less than or equal to depth
     * value, @glsl 0.0 @ce otherwise
     */
    LessOrEqual = GL_LEQUAL,

    /**
     * @glsl 1.0 @ce when texture coordinate is equal to depth value,
     * @glsl 0.0 @ce otherwise
     */
    Equal = GL_EQUAL,

    /**
     * @glsl 0.0 @ce when texture coordinate is equal to depth value,
     * @glsl 1.0 @ce otherwise
     */
    NotEqual = GL_NOTEQUAL,

    /**
     * @glsl 1.0 @ce when texture coordinate is greater than or equal
     * to depth value, @glsl 0.0 @ce otherwise
     */
    GreaterOrEqual = GL_GEQUAL,

    /**
     * @glsl 1.0 @ce when texture coordinate is greater than depth
     * value, @glsl 0.0 @ce otherwise
     */
    Greater = GL_GREATER
};

/** @debugoperatorenum{SamplerCompareFunction} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SamplerCompareFunction value);
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/**
@brief Texture sampler depth/stencil mode

@see @ref Texture::setDepthStencilMode() "*Texture::setDepthStencilMode()"
@m_enum_values_as_keywords
@requires_gl43 Extension @gl_extension{ARB,stencil_texturing}
@requires_gles31 Stencil texturing is not available in OpenGL ES 3.0
    and older.
@requires_gles Stencil texturing is not available in WebGL.
*/
enum class SamplerDepthStencilMode: GLenum {
    /** Sample depth component */
    DepthComponent = GL_DEPTH_COMPONENT,

    /** Sample stencil index (as unsigned integer texture) */
    StencilIndex = GL_STENCIL_INDEX
};

/** @debugoperatorenum{SamplerDepthStencilMode} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, SamplerDepthStencilMode value);
#endif

/**
@brief Texture sampler

@see @ref Texture, @ref TextureArray, @ref CubeMapTexture,
    @ref CubeMapTextureArray, @ref RectangleTexture
*/
class MAGNUM_GL_EXPORT Sampler {
    public:
        /**
         * @brief Max supported max anisotropy
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @gl_extension{ARB,texture_filter_anisotropic}
         * (part of OpenGL 4.6) or @gl_extension{EXT,texture_filter_anisotropic}
         * (desktop or ES) is not available, returns @cpp 0.0f @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_TEXTURE_MAX_ANISOTROPY}
         * @m_keywords{GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT}
         */
        static Float maxMaxAnisotropy();

        /* This class is currently a placeholder for GL sampler objects */
};

}}

#endif
