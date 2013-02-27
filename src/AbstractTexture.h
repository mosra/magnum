#ifndef Magnum_AbstractTexture_h
#define Magnum_AbstractTexture_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::AbstractTexture
 */

#include "Array.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Buffer.h"
#endif
#include "Color.h"
#include "AbstractImage.h"

namespace Magnum {

/**
@brief Base for textures

See Texture, CubeMapTexture and CubeMapTextureArray documentation for more
information and usage examples.

@section AbstractTexture-performance-optimization Performance optimizations

The engine tracks currently bound textures in all available layers to avoid
unnecessary calls to @fn_gl{ActiveTexture} and @fn_gl{BindTexture}. %Texture
configuration functions use dedicated highest available texture layer to not
affect active bindings in user layers. %Texture limits (such as
maxSupportedLayerCount()) are cached, so repeated queries don't result in
repeated @fn_gl{Get} calls.

If extension @extension{EXT,direct_state_access} is available, bind() uses DSA
function to avoid unnecessary calls to @fn_gl{ActiveTexture}. Also all texture
configuration and data updating functions use DSA functions to avoid
unnecessary calls to @fn_gl{ActiveTexture} and @fn_gl{BindTexture}. See
respective function documentation for more information.

To achieve least state changes, fully configure each texture in one run --
method chaining comes in handy -- and try to have often used textures in
dedicated layers, not occupied by other textures. First configure the texture
and *then* set the data, so OpenGL can optimize them to match the settings. To
avoid redundant consistency checks and memory reallocations when updating
texture data, set texture storage at once using @ref Texture::setStorage() "setStorage()"
and then set data using @ref Texture::setSubImage() "setSubImage()".

You can use functions invalidateImage() and @ref Texture::invalidateSubImage() "invalidateSubImage()"
if you don't need texture data anymore to avoid unnecessary memory operations
performed by OpenGL in order to preserve the data. If running on OpenGL ES or
extension @extension{ARB,invalidate_subdata} is not available, these functions
do nothing.

@todo all texture [level] parameters, global texture parameters
@todo Add glPixelStore encapsulation
@todo Texture copying
@todo Move constructor/assignment - how to avoid creation of empty texture and
    then deleting it immediately?
@todo ES2 - proper support for pixel unpack buffer when extension is in headers
*/
class MAGNUM_EXPORT AbstractTexture {
    friend class Context;

    AbstractTexture(const AbstractTexture& other) = delete;
    AbstractTexture& operator=(const AbstractTexture& other) = delete;

    public:
        /**
         * @brief %Texture filtering
         *
         * @see setMagnificationFilter() and setMinificationFilter()
         */
        enum class Filter: GLint {
            NearestNeighbor = GL_NEAREST,   /**< Nearest neighbor filtering */

            /**
             * Linear interpolation filtering.
             * @requires_gles30 %Extension @es_extension{OES,texture_float_linear} /
             *      @es_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
             *      for linear interpolation of textures with
             *      @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::HalfFloat"
             *      / @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::Float"
             *      in OpenGL ES 2.0.
             */
            LinearInterpolation = GL_LINEAR
        };

        /**
         * @brief Mip level selection
         *
         * @see setMinificationFilter()
         */
        enum class Mipmap: GLint {
            BaseLevel = GL_NEAREST & ~GL_NEAREST, /**< Select base mip level */

            /**
             * Select nearest mip level. **Unavailable on rectangle textures.**
             */
            NearestLevel = GL_NEAREST_MIPMAP_NEAREST & ~GL_NEAREST,

            /**
             * Linear interpolation of nearest mip levels. **Unavailable on
             * rectangle textures.**
             * @requires_gles30 %Extension @es_extension{OES,texture_float_linear} /
             *      @es_extension2{OES,texture_half_float_linear,OES_texture_float_linear}
             *      for linear interpolation of textures with
             *      @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::HalfFloat"
             *      / @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::Float"
             *      in OpenGL ES 2.0.
             */
            LinearInterpolation = GL_NEAREST_MIPMAP_LINEAR & ~GL_NEAREST
        };

        /**
         * @brief %Texture wrapping
         *
         * @see @ref Texture::setWrapping() "setWrapping()"
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
            ClampToEdge = GL_CLAMP_TO_EDGE

            #ifndef MAGNUM_TARGET_GLES
            ,
            /**
             * Clamp to border color. Coordinates out of range will be clamped
             * to border color (set with setBorderColor()).
             * @requires_gl Texture border is not available in OpenGL ES.
             */
            ClampToBorder = GL_CLAMP_TO_BORDER
            #endif
        };

        /**
         * @brief Internal format
         *
         * @see @ref Texture::setImage() "setImage()"
         */
        enum class InternalFormat: GLint {
            /**
             * Red component, normalized unsigned, size implementation-dependent.
             * @deprecated Prefer to use the exactly specified version of this
             *      format, e.g. @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::R8".
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gles30 %Extension @es_extension{EXT,texture_rg}
             */
            #ifndef MAGNUM_TARGET_GLES2
            Red = GL_RED,
            #else
            Red = GL_RED_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Red component, normalized unsigned byte.
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gles30 Use @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::Red"
             *      in OpenGL ES 2.0 instead.
             */
            R8 = GL_R8,
            #endif

            /**
             * Red and green component, normalized unsigned, size
             * implementation-dependent.
             * @deprecated Prefer to use the exactly specified version of this
             *      format, e.g. @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::RG8".
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gles30 %Extension @es_extension{EXT,texture_rg}
             */
            #ifndef MAGNUM_TARGET_GLES2
            RG = GL_RG,
            #else
            RG = GL_RG_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Red and green component, each normalized unsigned byte.
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gles30 Use @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::RG"
             *      in OpenGL ES 2.0 instead.
             */
            RG8 = GL_RG8,
            #endif

            /**
             * RGB, normalized unsigned, size implementation-dependent.
             * @deprecated Prefer to use the exactly specified version of this
             *      format, e.g. @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::RGB8".
             */
            RGB = GL_RGB,

            /**
             * RGB, each component normalized unsigned byte.
             * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
             */
            #ifndef MAGNUM_TARGET_GLES2
            RGB8 = GL_RGB8,
            #else
            RGB8 = GL_RGB8_OES,
            #endif

            /**
             * RGBA, normalized unsigned, size implementation-dependent.
             * @deprecated Prefer to use the exactly specified version of this
             *      format, e.g. @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::RGBA8".
             */
            RGBA = GL_RGBA,

            /**
             * RGBA, each component normalized unsigned byte.
             * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
             */
            #ifndef MAGNUM_TARGET_GLES2
            RGBA8 = GL_RGBA8,
            #else
            RGBA8 = GL_RGBA8_OES,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Red component, normalized signed byte.
             * @requires_gl31 %Extension @extension{EXT,texture_snorm}
             * @requires_gles30 Only unsigned formats are available in OpenGL
             *      ES 2.0.
             */
            R8Snorm = GL_R8_SNORM,

            /**
             * Red and green component, each normalized signed byte.
             * @requires_gl31 %Extension @extension{EXT,texture_snorm}
             * @requires_gles30 Only unsigned formats are available in OpenGL
             *      ES 2.0.
             */
            RG8Snorm = GL_RG8_SNORM,

            /**
             * RGB, each component normalized signed byte.
             * @requires_gl31 %Extension @extension{EXT,texture_snorm}
             * @requires_gles30 Only unsigned formats are available in OpenGL
             *      ES 2.0.
             */
            RGB8Snorm = GL_RGB8_SNORM,

            /**
             * RGBA, each component normalized signed byte.
             * @requires_gl31 %Extension @extension{EXT,texture_snorm}
             * @requires_gles30 Only unsigned formats are available in OpenGL
             *      ES 2.0.
             */
            RGBA8Snorm = GL_RGBA8_SNORM,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Red component, normalized unsigned short.
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gl Only byte-sized normalized formats are available
             *      in OpenGL ES.
             */
            R16 = GL_R16,

            /**
             * Red and green component, each normalized unsigned short.
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gl Only byte-sized normalized formats are available
             *      in OpenGL ES.
             */
            RG16 = GL_RG16,

            /**
             * RGB, each component normalized unsigned short.
             * @requires_gl Only byte-sized normalized formats are available
             *      in OpenGL ES.
             */
            RGB16 = GL_RGB16,

            /**
             * RGBA, each component normalized unsigned short.
             * @requires_gl Only byte-sized normalized formats are available
             *      in OpenGL ES.
             */
            RGBA16 = GL_RGBA16,
            #endif

            /**
             * Red component, normalized signed short.
             * @requires_gl31 %Extension @extension{EXT,texture_snorm}
             * @requires_gles30 Only unsigned normalized formats are available
             *      in OpenGL ES 2.0.
             */
            R16Snorm = GL_R16_SNORM,

            /**
             * Red and green component, each normalized signed short.
             * @requires_gl31 %Extension @extension{EXT,texture_snorm}
             * @requires_gles30 Only unsigned normalized formats are available
             *      in OpenGL ES 2.0.
             */
            RG16Snorm = GL_RG16_SNORM,

            /**
             * RGB, each component normalized signed short.
             * @requires_gl31 %Extension @extension{EXT,texture_snorm}
             * @requires_gles30 Only unsigned normalized formats are available
             *      in OpenGL ES 2.0.
             */
            RGB16Snorm = GL_RGB16_SNORM,

            /**
             * RGBA, each component normalized signed short.
             * @requires_gl31 %Extension @extension{EXT,texture_snorm}
             * @requires_gles30 Only unsigned normalized formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA16Snorm = GL_RGBA16_SNORM,

            /**
             * Red component, non-normalized unsigned byte.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R8UI = GL_R8UI,

            /**
             * Red and green component, each non-normalized unsigned byte.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG8UI = GL_RG8UI,

            /**
             * RGB, each component non-normalized unsigned byte.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGB8UI = GL_RGB8UI,

            /**
             * RGBA, each component non-normalized unsigned byte.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA8UI = GL_RGBA8UI,

            /**
             * Red component, non-normalized signed byte.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R8I = GL_R8I,

            /**
             * Red and green component, each non-normalized signed byte.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG8I = GL_RG8I,

            /**
             * RGB, each component non-normalized signed byte.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGB8I = GL_RGB8I,

            /**
             * RGBA, each component non-normalized signed byte.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA8I = GL_RGBA8I,

            /**
             * Red component, non-normalized unsigned short.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R16UI = GL_R16UI,

            /**
             * Red and green component, each non-normalized unsigned short.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG16UI = GL_RG16UI,

            /**
             * RGB, each component non-normalized unsigned short.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGB16UI = GL_RGB16UI,

            /**
             * RGBA, each component non-normalized unsigned short.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA16UI = GL_RGBA16UI,

            /**
             * Red component, non-normalized signed short.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R16I = GL_R16I,

            /**
             * Red and green component, each non-normalized signed short.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG16I = GL_RG16I,

            /**
             * RGB, each component non-normalized signed short.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGB16I = GL_RGB16I,

            /**
             * RGBA, each component non-normalized signed short.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA16I = GL_RGBA16I,

            /**
             * Red component, non-normalized unsigned int.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R32UI = GL_R32UI,

            /**
             * Red and green component, each non-normalized unsigned int.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG32UI = GL_RG32UI,

            /**
             * RGB, each component non-normalized unsigned int.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGB32UI = GL_RGB32UI,

            /**
             * RGBA, each component non-normalized unsigned int.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA32UI = GL_RGBA32UI,

            /**
             * Red component, non-normalized signed int.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R32I = GL_R32I,

            /**
             * Red and green component, each non-normalized signed int.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG32I = GL_RG32I,

            /**
             * RGB, each component non-normalized signed int.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGB32I = GL_RGB32I,

            /**
             * RGBA, each component non-normalized signed int.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA32I = GL_RGBA32I,

            /**
             * Red component, half float.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{ARB,texture_float}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R16F = GL_R16F,

            /**
             * Red and green component, each half float.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{ARB,texture_float}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG16F = GL_RG16F,

            /**
             * RGB, each component half float.
             * @requires_gl30 %Extension @extension{ARB,texture_float}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGB16F = GL_RGB16F,

            /**
             * RGBA, each component half float.
             * @requires_gl30 %Extension @extension{ARB,texture_float}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA16F = GL_RGBA16F,

            /**
             * Red component, float.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{ARB,texture_float}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R32F = GL_R32F,

            /**
             * Red and green component, each float.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{ARB,texture_float}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RG32F = GL_RG32F,

            /**
             * RGB, each component float.
             * @requires_gl30 %Extension @extension{ARB,texture_float}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGB32F = GL_RGB32F,

            /**
             * RGBA, each component float.
             * @requires_gl30 %Extension @extension{ARB,texture_float}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGBA32F = GL_RGBA32F,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * RGB, normalized unsigned, red and green component 3bit, blue
             * 2bit.
             * @requires_gl Packed 8bit types are not available in OpenGL ES.
             */
            R3B3G2 = GL_R3_G3_B2,

            /**
             * RGB, each component normalized unsigned 4bit.
             * @requires_gl Packed 12bit types are not available in OpenGL ES.
             */
            RGB4 = GL_RGB4,

            /**
             * RGB, each component normalized unsigned 5bit.
             * @requires_gl Use @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::RGB5A1"
             *      or @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::RGB565" in OpenGL ES.
             */
            RGB5 = GL_RGB5,
            #endif

            /* 1.5.6 <= GLEW < 1.8.0 doesn't have this, even if there is
               GL_ARB_ES2_compatibility */
            #if defined(GL_RGB565) || defined(DOXYGEN_GENERATING_OUTPUT)
            /**
             * RGB, normalized unsigned, red and blue component 5bit, green 6bit.
             * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
             */
            #ifndef MAGNUM_TARGET_GLES2
            RGB565 = GL_RGB565,
            #else
            RGB565 = GL_RGB565_OES,
            #endif
            #endif

            /**
             * RGB, each component normalized unsigned 10bit.
             * @requires_es_extension %Extension @es_extension{OES,required_internalformat}
             *      and @es_extension{EXT,texture_type_2_10_10_10_REV}
             */
            #ifndef MAGNUM_TARGET_GLES
            RGB10 = GL_RGB10,
            #else
            RGB10 = GL_RGB10_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * RGB, each component normalized unsigned 12bit.
             * @requires_gl Packed 36bit types are not available in OpenGL ES.
             */
            RGB12 = GL_RGB12,

            /**
             * RGBA, normalized unsigned, each component 2bit.
             * @requires_gl Packed 8bit types are not available in OpenGL ES.
             */
            RGBA2 = GL_RGBA2,
            #endif

            /**
             * RGBA, normalized unsigned, each component 4bit.
             * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
             */
            #ifndef MAGNUM_TARGET_GLES2
            RGBA4 = GL_RGBA4,
            #else
            RGBA4 = GL_RGBA4_OES,
            #endif

            /**
             * RGBA, normalized unsigned, each RGB component 5bit, alpha 1bit.
             * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
             */
            #ifndef MAGNUM_TARGET_GLES2
            RGB5A1 = GL_RGB5_A1,
            #else
            RGB5A1 = GL_RGB5_A1_OES,
            #endif

            /**
             * RGBA, normalized unsigned, each RGB component 10bit, alpha 2bit.
             * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
             *      and @es_extension{EXT,texture_type_2_10_10_10_REV}
             */
            #ifndef MAGNUM_TARGET_GLES2
            RGB10A2 = GL_RGB10_A2,
            #else
            RGB10A2 = GL_RGB10_A2_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * RGBA, non-normalized unsigned, each RGB component 10bit, alpha
             * 2bit.
             * @requires_gl33 %Extension @extension{ARB,texture_rgb10_a2ui}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            RGB10A2UI = GL_RGB10_A2UI,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * RGBA, each component normalized unsigned 12bit.
             * @requires_gl Packed 48bit types are not available in OpenGL ES.
             */
            RGBA12 = GL_RGBA12,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * RGB, float, red and green component 11bit, blue 10bit.
             * @requires_gl30 %Extension @extension{EXT,packed_float}
             * @requires_gles30 Only normalized integral formats are available
             *      in OpenGL ES 2.0.
             */
            R11FG11FB10F = GL_R11F_G11F_B10F,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * RGB, unsigned with exponent, each RGB component 9bit, exponent 5bit.
             * @requires_gl30 %Extension @extension{EXT,texture_shared_exponent}
             * @requires_gles30 Use @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::RGB"
             *      in OpenGL ES 2.0 instead.
             */
            RGB9E5 = GL_RGB9_E5,
            #endif

            /**
             * sRGB, normalized unsigned, size implementation-dependent.
             * @todo is this allowed in core?
             * @deprecated Prefer to use the exactly specified version of this
             *      format, i.e. @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::SRGB8".
             * @requires_es_extension %Extension @es_extension{EXT,sRGB} in
             *      OpenGL ES 2.0, use @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::SRGB8"
             *      in OpenGL ES 3.0 instead.
             */
            #ifndef MAGNUM_TARGET_GLES
            SRGB = GL_SRGB,
            #else
            SRGB = GL_SRGB_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * sRGB, each component normalized unsigned byte.
             * @requires_gles30 Use @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::SRGB"
             *      in OpenGL ES 2.0 instead.
             */
            SRGB8 = GL_SRGB8,
            #endif

            /**
             * sRGBA, normalized unsigned, size implementation-dependent.
             * @todo is this allowed in core?
             * @deprecated Prefer to use the exactly specified version of this
             *      format, i.e. @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::SRGB8Alpha8".
             * @requires_es_extension %Extension @es_extension{EXT,sRGB} in
             *      OpenGL ES 2.0, use @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::SRGB8Alpha8"
             *      in OpenGL ES 3.0 instead.
             */
            #ifndef MAGNUM_TARGET_GLES
            SRGBAlpha = GL_SRGB_ALPHA,
            #else
            SRGBAlpha = GL_SRGB_ALPHA_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * sRGBA, each component normalized unsigned byte.
             * @requires_gles30 Use @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::SRGBAlpha"
             *      in OpenGL ES 2.0 instead.
             */
            SRGB8Alpha8 = GL_SRGB8_ALPHA8,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Compressed red channel, normalized unsigned.
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gl Generic texture compression is not available in
             *      OpenGL ES.
             */
            CompressedRed = GL_COMPRESSED_RED,

            /**
             * Compressed red and green channel, normalized unsigned.
             * @requires_gl30 %Extension @extension{ARB,texture_rg}
             * @requires_gl Generic texture compression is not available in
             *      OpenGL ES.
             */
            CompressedRG = GL_COMPRESSED_RG,

            /**
             * Compressed RGB, normalized unsigned.
             * @requires_gl Generic texture compression is not available in
             *      OpenGL ES.
             */
            CompressedRGB = GL_COMPRESSED_RGB,

            /**
             * Compressed RGBA, normalized unsigned.
             * @requires_gl Generic texture compression is not available in
             *      OpenGL ES.
             */
            CompressedRGBA = GL_COMPRESSED_RGBA,

            /**
             * RTGC compressed red channel, normalized unsigned.
             * @requires_gl30 %Extension @extension{EXT,texture_compression_rgtc}
             * @requires_gl RGTC texture compression is not available in
             *      OpenGL ES.
             */
            CompressedRedRtgc1 = GL_COMPRESSED_RED_RGTC1,

            /**
             * RTGC compressed red and green channel, normalized unsigned.
             * @requires_gl30 %Extension @extension{EXT,texture_compression_rgtc}
             * @requires_gl RGTC texture compression is not available in
             *      OpenGL ES.
             */
            CompressedRGRgtc2 = GL_COMPRESSED_RG_RGTC2,

            /**
             * RTGC compressed red channel, normalized signed.
             * @requires_gl30 %Extension @extension{EXT,texture_compression_rgtc}
             * @requires_gl RGTC texture compression is not available in
             *      OpenGL ES.
             */
            CompressedSignedRedRgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,

            /**
             * RTGC compressed red and green channel, normalized signed.
             * @requires_gl30 %Extension @extension{EXT,texture_compression_rgtc}
             * @requires_gl RGTC texture compression is not available in
             *      OpenGL ES.
             */
            CompressedSignedRGRgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,

            /* These are named with _ARB suffix, because glcorearb.h doesn't
               have suffixless version (?!) and GLEW has it without suffix as
               late as of 1.8.0 { */

            /**
             * BPTC compressed RGBA, normalized unsigned.
             * @requires_gl42 %Extension @extension{ARB,texture_compression_bptc}
             * @requires_gl BPTC texture compression is not available in
             *      OpenGL ES.
             */
            CompressedRGBABtpcUnorm = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,

            /**
             * BPTC compressed sRGBA, normalized unsigned.
             * @requires_gl42 %Extension @extension{ARB,texture_compression_bptc}
             * @requires_gl BPTC texture compression is not available in
             *      OpenGL ES.
             */
            CompressedSRGBAlphaBtpcUnorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,

            /**
             * BPTC compressed RGB, unsigned float.
             * @requires_gl42 %Extension @extension{ARB,texture_compression_bptc}
             * @requires_gl BPTC texture compression is not available in
             *      OpenGL ES.
             */
            CompressedRGBBptcUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,

            /**
             * BPTC compressed RGB, signed float.
             * @requires_gl42 %Extension @extension{ARB,texture_compression_bptc}
             * @requires_gl BPTC texture compression is not available in
             *      OpenGL ES.
             */
            CompressedRGBBptcSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,

            /*}*/
            #endif

            /**
             * Depth component, size implementation-dependent.
             * @deprecated Prefer to use exactly specified version of this
             *      format, e.g. @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::DepthComponent16".
             * @requires_gles30 %Extension @es_extension{OES,depth_texture}
             */
            DepthComponent = GL_DEPTH_COMPONENT,

            /**
             * Depth and stencil component, size implementation-dependent.
             * @deprecated Prefer to use exactly specified version of this
             *      format, e.g. @ref Magnum::AbstractTexture::InternalFormat "InternalFormat::Depth24Stencil8".
             * @requires_gles30 %Extension @es_extension{OES,packed_depth_stencil}
             */
            #ifndef MAGNUM_TARGET_GLES2
            DepthStencil = GL_DEPTH_STENCIL,
            #else
            DepthStencil = GL_DEPTH_STENCIL_OES,
            #endif

            /**
             * Depth component, 16bit.
             * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
             *      and @es_extension{OES,depth_texture}
             */
            DepthComponent16 = GL_DEPTH_COMPONENT16,

            /**
             * Depth component, 24bit.
             * @requires_gles30 %Extension @es_extension{OES,required_internalformat},
             *      @es_extension{OES,depth_texture} and @es_extension{OES,depth24}
             */
            #ifndef MAGNUM_TARGET_GLES2
            DepthComponent24 = GL_DEPTH_COMPONENT24,
            #else
            DepthComponent24 = GL_DEPTH_COMPONENT24_OES,
            #endif

            /**
             * Depth component, 32bit.
             * @requires_es_extension %Extension @es_extension{OES,required_internalformat},
             *      @es_extension{OES,depth_texture} and @es_extension{OES,depth32}
             */
            #ifndef MAGNUM_TARGET_GLES2
            DepthComponent32 = GL_DEPTH_COMPONENT32,
            #else
            DepthComponent32 = GL_DEPTH_COMPONENT32_OES,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Depth component, 32bit float.
             * @requires_gl30 %Extension @extension{ARB,depth_buffer_float}
             * @requires_gles30 Only integral depth textures are available in
             *      OpenGL ES 2.0.
             */
            DepthComponent32F = GL_DEPTH_COMPONENT32F,
            #endif

            /**
             * 24bit depth and 8bit stencil component.
             * @requires_gl30 %Extension @extension{EXT,packed_depth_stencil}
             * @requires_gles30 %Extension @es_extension{OES,required_internalformat}
             *      and @es_extension{OES,packed_depth_stencil}
             */
            #ifdef MAGNUM_TARGET_GLES2
            Depth24Stencil8 = GL_DEPTH24_STENCIL8_OES
            #else
            Depth24Stencil8 = GL_DEPTH24_STENCIL8,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * 32bit float depth component and 8bit stencil component.
             * @requires_gl30 %Extension @extension{ARB,depth_buffer_float}
             * @requires_gles30 Only integral depth textures are available in
             *      OpenGL ES 2.0.
             */
            Depth32FStencil8 = GL_DEPTH32F_STENCIL8
            #endif
        };

        /**
         * @brief Max supported layer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref AbstractShaderProgram-subclassing, bind(Int),
         *      @fn_gl{Get} with @def_gl{MAX_COMBINED_TEXTURE_IMAGE_UNITS},
         *      @fn_gl{ActiveTexture}
         */
        static Int maxSupportedLayerCount();

        /**
         * @brief Max supported anisotropy
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see setMaxAnisotropy(), @fn_gl{Get} with @def_gl{MAX_TEXTURE_MAX_ANISOTROPY_EXT}
         * @requires_extension %Extension @extension{EXT,texture_filter_anisotropic}
         * @requires_es_extension %Extension @es_extension2{EXT,texture_filter_anisotropic,texture_filter_anisotropic}
         */
        static Float maxSupportedAnisotropy();

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline explicit AbstractTexture(GLenum target): _target(target) {
            glGenTextures(1, &_id);
        }
        #endif

        /**
         * @brief Destructor
         *
         * Deletes assigned OpenGL texture.
         * @see @fn_gl{DeleteTextures}
         */
        virtual ~AbstractTexture() = 0;

        /** @brief Move constructor */
        AbstractTexture(AbstractTexture&& other);

        /** @brief Move assignment */
        AbstractTexture& operator=(AbstractTexture&& other);

        /** @brief OpenGL texture ID */
        inline GLuint id() const { return _id; }

        /**
         * @brief Bind texture for rendering
         *
         * Sets current texture as active in given layer. The layer must be
         * between 0 and maxSupportedLayerCount(). Note that only one texture
         * can be bound to given layer. If @extension{EXT,direct_state_access}
         * is not available, the layer is made active before binding the
         * texture.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} or
         *      @fn_gl_extension{BindMultiTexture,EXT,direct_state_access}
         */
        void bind(Int layer);

        /**
         * @brief Set minification filter
         * @param filter        Filter
         * @param mipmap        Mipmap filtering. If set to anything else than
         *      BaseMipLevel, make sure textures for all mip levels are set or
         *      call generateMipmap().
         * @return Pointer to self (for method chaining)
         *
         * Sets filter used when the object pixel size is smaller than the
         * texture size. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some layer before the operation.
         * Initial value is (@ref AbstractTexture::Filter "Filter::NearestNeighbor",
         * @ref AbstractTexture::Mipmap "Mipmap::Linear").
         * @attention For rectangle textures only some modes are supported,
         *      see @ref AbstractTexture::Filter "Filter" and
         *      @ref AbstractTexture::Mipmap "Mipmap" documentation for more
         *      information.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_MIN_FILTER}
         */
        AbstractTexture* setMinificationFilter(Filter filter, Mipmap mipmap = Mipmap::BaseLevel);

        /**
         * @brief Set magnification filter
         * @param filter        Filter
         * @return Pointer to self (for method chaining)
         *
         * Sets filter used when the object pixel size is larger than largest
         * texture size. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some layer before the operation.
         * Initial value is @ref AbstractTexture::Filter "Filter::Linear".
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_MAG_FILTER}
         */
        inline AbstractTexture* setMagnificationFilter(Filter filter) {
            (this->*parameteriImplementation)(GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
            return this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set border color
         * @return Pointer to self (for method chaining)
         *
         * Border color when @ref AbstractTexture::Wrapping "wrapping" is set
         * to `ClampToBorder`. If @extension{EXT,direct_state_access} is not
         * available, the texture is bound to some layer before the operation.
         * Initial value is `{0.0f, 0.0f, 0.0f, 0.0f}`.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexParameter}
         *      or @fn_gl_extension{TextureParameter,EXT,direct_state_access}
         *      with @def_gl{TEXTURE_BORDER_COLOR}
         * @requires_gl Texture border is not available in OpenGL ES.
         */
        inline AbstractTexture* setBorderColor(const Color4<>& color) {
            (this->*parameterfvImplementation)(GL_TEXTURE_BORDER_COLOR, color.data());
            return this;
        }
        #endif

        /**
         * @brief Set max anisotropy
         * @return Pointer to self (for method chaining)
         *
         * Default value is `1.0f`, which means no anisotropy. Set to value
         * greater than `1.0f` for anisotropic filtering. If
         * @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some layer before the operation.
         * @see maxSupportedAnisotropy(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{TexParameter} or
         *      @fn_gl_extension{TextureParameter,EXT,direct_state_access} with
         *      @def_gl{TEXTURE_MAX_ANISOTROPY_EXT}
         * @requires_extension %Extension @extension{EXT,texture_filter_anisotropic}
         * @requires_es_extension %Extension @es_extension2{EXT,texture_filter_anisotropic,texture_filter_anisotropic}
         */
        inline AbstractTexture* setMaxAnisotropy(Float anisotropy) {
            (this->*parameterfImplementation)(GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
            return this;
        }

        /**
         * @brief Invalidate texture image
         * @param level             Mip level
         *
         * If running on OpenGL ES or extension @extension{ARB,invalidate_subdata}
         * is not available, this function does nothing.
         * @see @ref Texture::invalidateSubImage() "invalidateSubImage()",
         *      @fn_gl{InvalidateTexImage}
         */
        inline void invalidateImage(Int level) {
            (this->*invalidateImplementation)(level);
        }

        /**
         * @brief Generate mipmap
         * @return Pointer to self (for method chaining)
         *
         * Can not be used for rectangle textures. If
         * @extension{EXT,direct_state_access} is not available, the texture
         * is bound to some layer before the operation.
         * @see setMinificationFilter(), @fn_gl{ActiveTexture},
         *      @fn_gl{BindTexture} and @fn_gl{GenerateMipmap} or
         *      @fn_gl_extension{GenerateTextureMipmap,EXT,direct_state_access}
         * @requires_gl30 %Extension @extension{EXT,framebuffer_object}
         */
        AbstractTexture* generateMipmap();

    protected:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt textureDimensions> struct DataHelper {};

        /* Unlike bind() this also sets the binding layer as active */
        void MAGNUM_LOCAL bindInternal();

        GLenum _target;
        #endif

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

        typedef void(AbstractTexture::*BindImplementation)(GLint);
        void MAGNUM_LOCAL bindImplementationDefault(GLint layer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL bindImplementationDSA(GLint layer);
        #endif
        static MAGNUM_LOCAL BindImplementation bindImplementation;

        typedef void(AbstractTexture::*ParameteriImplementation)(GLenum, GLint);
        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, GLint value);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, GLint value);
        #endif
        static ParameteriImplementation parameteriImplementation;

        typedef void(AbstractTexture::*ParameterfImplementation)(GLenum, GLfloat);
        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, GLfloat value);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, GLfloat value);
        #endif
        static ParameterfImplementation parameterfImplementation;

        typedef void(AbstractTexture::*ParameterfvImplementation)(GLenum, const GLfloat*);
        void MAGNUM_LOCAL parameterImplementationDefault(GLenum parameter, const GLfloat* values);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL parameterImplementationDSA(GLenum parameter, const GLfloat* values);
        #endif
        static ParameterfvImplementation parameterfvImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*GetLevelParameterivImplementation)(GLenum, GLint, GLenum, GLint*);
        void MAGNUM_LOCAL getLevelParameterImplementationDefault(GLenum target, GLint level, GLenum parameter, GLint* values);
        void MAGNUM_LOCAL getLevelParameterImplementationDSA(GLenum target, GLint level, GLenum parameter, GLint* values);
        static MAGNUM_LOCAL GetLevelParameterivImplementation getLevelParameterivImplementation;
        #endif

        typedef void(AbstractTexture::*MipmapImplementation)();
        void MAGNUM_LOCAL mipmapImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL mipmapImplementationDSA();
        #endif
        static MAGNUM_LOCAL MipmapImplementation mipmapImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*Storage1DImplementation)(GLenum, GLsizei, InternalFormat, const Math::Vector<1, GLsizei>&);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, InternalFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, InternalFormat internalFormat, const Math::Vector<1, GLsizei>& size);
        static Storage1DImplementation storage1DImplementation;
        #endif

        typedef void(AbstractTexture::*Storage2DImplementation)(GLenum, GLsizei, InternalFormat, const Vector2i&);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, InternalFormat internalFormat, const Vector2i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, InternalFormat internalFormat, const Vector2i& size);
        #endif
        static Storage2DImplementation storage2DImplementation;

        typedef void(AbstractTexture::*Storage3DImplementation)(GLenum, GLsizei, InternalFormat, const Vector3i&);
        void MAGNUM_LOCAL storageImplementationDefault(GLenum target, GLsizei levels, InternalFormat internalFormat, const Vector3i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL storageImplementationDSA(GLenum target, GLsizei levels, InternalFormat internalFormat, const Vector3i& size);
        #endif
        static Storage3DImplementation storage3DImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*Image1DImplementation)(GLenum, GLint, InternalFormat, const Math::Vector<1, GLsizei>&, AbstractImage::Format, AbstractImage::Type, const GLvoid*);
        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, InternalFormat internalFormat, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, InternalFormat internalFormat, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        static Image1DImplementation image1DImplementation;
        #endif

        typedef void(AbstractTexture::*Image2DImplementation)(GLenum, GLint, InternalFormat, const Vector2i&, AbstractImage::Format, AbstractImage::Type, const GLvoid*);
        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, InternalFormat internalFormat, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, InternalFormat internalFormat, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        #endif
        static Image2DImplementation image2DImplementation;

        typedef void(AbstractTexture::*Image3DImplementation)(GLenum, GLint, InternalFormat, const Vector3i&, AbstractImage::Format, AbstractImage::Type, const GLvoid*);
        void MAGNUM_LOCAL imageImplementationDefault(GLenum target, GLint level, InternalFormat internalFormat, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL imageImplementationDSA(GLenum target, GLint level, InternalFormat internalFormat, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        #endif
        static Image3DImplementation image3DImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(AbstractTexture::*SubImage1DImplementation)(GLenum, GLint, const Math::Vector<1, GLint>&, const Math::Vector<1, GLsizei>&, AbstractImage::Format, AbstractImage::Type, const GLvoid*);
        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        static SubImage1DImplementation subImage1DImplementation;
        #endif

        typedef void(AbstractTexture::*SubImage2DImplementation)(GLenum, GLint, const Vector2i&, const Vector2i&, AbstractImage::Format, AbstractImage::Type, const GLvoid*);
        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        #endif
        static SubImage2DImplementation subImage2DImplementation;

        typedef void(AbstractTexture::*SubImage3DImplementation)(GLenum, GLint, const Vector3i&, const Vector3i&, AbstractImage::Format, AbstractImage::Type, const GLvoid*);
        void MAGNUM_LOCAL subImageImplementationDefault(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL subImageImplementationDSA(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, AbstractImage::Format format, AbstractImage::Type type, const GLvoid* data);
        #endif
        static SubImage3DImplementation subImage3DImplementation;

        typedef void(AbstractTexture::*InvalidateImplementation)(GLint);
        void MAGNUM_LOCAL invalidateImplementationNoOp(GLint level);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateImplementationARB(GLint level);
        #endif
        static InvalidateImplementation invalidateImplementation;

        typedef void(AbstractTexture::*InvalidateSubImplementation)(GLint, const Vector3i&, const Vector3i&);
        void MAGNUM_LOCAL invalidateSubImplementationNoOp(GLint level, const Vector3i& offset, const Vector3i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateSubImplementationARB(GLint level, const Vector3i& offset, const Vector3i& size);
        #endif
        static InvalidateSubImplementation invalidateSubImplementation;

        void MAGNUM_LOCAL destroy();
        void MAGNUM_LOCAL move();

        GLuint _id;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<class Image> struct ImageHelper {
        inline static const GLvoid* dataOrPixelUnpackBuffer(Image* image) {
            #ifndef MAGNUM_TARGET_GLES2
            Buffer::unbind(Buffer::Target::PixelUnpack);
            #endif
            return image->data();
        }
    };

    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> struct MAGNUM_EXPORT ImageHelper<BufferImage<dimensions>> {
        static const GLvoid* dataOrPixelUnpackBuffer(BufferImage<dimensions>* image);
    };
    #endif
}

#ifndef MAGNUM_TARGET_GLES
template<> struct AbstractTexture::DataHelper<1> {
    enum class Target: GLenum {
        Texture1D = GL_TEXTURE_1D
    };

    inline constexpr static Target target() { return Target::Texture1D; }

    static Math::Vector<1, GLint> imageSize(AbstractTexture* texture, GLenum target, GLint level);

    inline static void setWrapping(AbstractTexture* texture, const Array1D<Wrapping>& wrapping) {
        (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping.x()));
    }

    inline static void setStorage(AbstractTexture* texture, GLenum target, GLsizei levels, InternalFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
        (texture->*storage1DImplementation)(target, levels, internalFormat, size);
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 1, void>::type set(AbstractTexture* texture, GLenum target, GLint level, InternalFormat internalFormat, Image* image) {
        (texture->*image1DImplementation)(target, level, internalFormat, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 1, void>::type setSub(AbstractTexture* texture, GLenum target, GLint level, const Math::Vector<1, GLint>& offset, Image* image) {
        (texture->*subImage1DImplementation)(target, level, offset, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    inline static void invalidateSub(AbstractTexture* texture, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLint>& size) {
        (texture->*invalidateSubImplementation)(level, {offset[0], 0, 0}, {size[0], 1, 1});
    }
};
#endif
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<2> {
    enum class Target: GLenum {
        Texture2D = GL_TEXTURE_2D
        #ifndef MAGNUM_TARGET_GLES
        ,
        Texture1DArray = GL_TEXTURE_1D_ARRAY,
        Rectangle = GL_TEXTURE_RECTANGLE
        #endif
    };

    inline constexpr static Target target() { return Target::Texture2D; }

    #ifndef MAGNUM_TARGET_GLES
    static Vector2i imageSize(AbstractTexture* texture, GLenum target, GLint level);
    #endif

    static void setWrapping(AbstractTexture* texture, const Array2D<Wrapping>& wrapping);

    inline static void setStorage(AbstractTexture* texture, GLenum target, GLsizei levels, InternalFormat internalFormat, const Vector2i& size) {
        (texture->*storage2DImplementation)(target, levels, internalFormat, size);
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 2, void>::type set(AbstractTexture* texture, GLenum target, GLint level, InternalFormat internalFormat, Image* image) {
        (texture->*image2DImplementation)(target, level, internalFormat, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 2, void>::type setSub(AbstractTexture* texture, GLenum target, GLint level, const Vector2i& offset, Image* image) {
        (texture->*subImage2DImplementation)(target, level, offset, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 1, void>::type setSub(AbstractTexture* texture, GLenum target, GLint level, const Vector2i& offset, Image* image) {
        (texture->*subImage2DImplementation)(target, level, offset, Vector2i(image->size(), 1), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    inline static void invalidateSub(AbstractTexture* texture, GLint level, const Vector2i& offset, const Vector2i& size) {
        (texture->*invalidateSubImplementation)(level, {offset, 0}, {size, 1});
    }
};
template<> struct MAGNUM_EXPORT AbstractTexture::DataHelper<3> {
    enum class Target: GLenum {
        #ifndef MAGNUM_TARGET_GLES2
        Texture3D = GL_TEXTURE_3D,
        Texture2DArray = GL_TEXTURE_2D_ARRAY
        #else
        Texture3D = GL_TEXTURE_3D_OES
        #endif
    };

    inline constexpr static Target target() { return Target::Texture3D; }

    #ifndef MAGNUM_TARGET_GLES
    static Vector3i imageSize(AbstractTexture* texture, GLenum target, GLint level);
    #endif

    static void setWrapping(AbstractTexture* texture, const Array3D<Wrapping>& wrapping);

    inline static void setStorage(AbstractTexture* texture, GLenum target, GLsizei levels, InternalFormat internalFormat, const Vector3i& size) {
        (texture->*storage3DImplementation)(target, levels, internalFormat, size);
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 3, void>::type set(AbstractTexture* texture, GLenum target, GLint level, InternalFormat internalFormat, Image* image) {
        (texture->*image3DImplementation)(target, level, internalFormat, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 3, void>::type setSub(AbstractTexture* texture, GLenum target, GLint level, const Vector3i& offset, Image* image) {
        (texture->*subImage3DImplementation)(target, level, offset, image->size(), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    template<class Image> inline static typename std::enable_if<Image::Dimensions == 2, void>::type setSub(AbstractTexture* texture, GLenum target, GLint level, const Vector3i& offset, Image* image) {
        (texture->*subImage3DImplementation)(target, level, offset, Vector3i(image->size(), 1), image->format(), image->type(), Implementation::ImageHelper<Image>::dataOrPixelUnpackBuffer(image));
    }

    inline static void invalidateSub(AbstractTexture* texture, GLint level, const Vector3i& offset, const Vector3i& size) {
        (texture->*invalidateSubImplementation)(level, offset, size);
    }
};
#endif

}

#endif
