#ifndef Magnum_AbstractImage_h
#define Magnum_AbstractImage_h
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
 * @brief Class Magnum::AbstractImage
 */

#include <cstddef>

#include "Magnum.h"

#include "magnumVisibility.h"

namespace Magnum {

/**
@brief Non-templated base for one-, two- or three-dimensional images

See Image, ImageWrapper, BufferImage, Trade::ImageData documentation for
more information.
*/
class MAGNUM_EXPORT AbstractImage {
    AbstractImage(const AbstractImage& other) = delete;
    AbstractImage(AbstractImage&& other) = delete;
    AbstractImage& operator=(const AbstractImage& other) = delete;
    AbstractImage& operator=(AbstractImage&& other) = delete;

    public:
        /**
         * @{ @name Image formats
         *
         * Note that some formats can be used only for framebuffer reading
         * (using Framebuffer::read()) and some only for texture data (using
         * Texture::setImage() and others).
         */

        /**
         * @brief Format of pixel data
         *
         * @todo What is allowed for FB reading and what for image
         *      specification?
         * @see pixelSize()
         */
        enum class Format: GLenum {
            /**
             * Floating-point red channel.
             * @requires_gles30 %Extension @es_extension{EXT,texture_rg}
             */
            #ifndef MAGNUM_TARGET_GLES2
            Red = GL_RED,
            #else
            Red = GL_RED_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Floating-point green channel. For framebuffer reading only.
             * @requires_gl Only @ref Magnum::AbstractImage::Format "Format::Red"
             *      is available in OpenGL ES.
             */
            Green = GL_GREEN,

            /**
             * Floating-point blue channel. For framebuffer reading only.
             * @requires_gl Only @ref Magnum::AbstractImage::Format "Format::Red"
             *      is available in OpenGL ES.
             */
            Blue = GL_BLUE,
            #endif

            /**
             * Floating-point red and green channel. For texture data only.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 %Extension @es_extension{EXT,texture_rg}
             */
            #ifndef MAGNUM_TARGET_GLES2
            RG = GL_RG,
            #else
            RG = GL_RG_EXT,
            #endif

            /** Floating-point RGB. */
            RGB = GL_RGB,

            /** Floating-point RGBA. */
            RGBA = GL_RGBA,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Floating-point BGR.
             * @requires_gl Only RGB component ordering is available in OpenGL
             *      ES.
             */
            BGR = GL_BGR,
            #endif

            /**
             * Floating-point BGRA.
             * @requires_es_extension %Extension @es_extension{EXT,read_format_bgra}
             *      for framebuffer reading, extension @es_extension{APPLE,texture_format_BGRA8888} or
             *      @es_extension{EXT,texture_format_BGRA8888} for texture data.
             */
            #ifndef MAGNUM_TARGET_GLES
            BGRA = GL_BGRA,
            #else
            BGRA = GL_BGRA_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Integer red channel.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only floating-point image data are available
             *      in OpenGL ES 2.0.
             */
            RedInteger = GL_RED_INTEGER,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Integer green channel.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gl Only @ref Format "Format::RedInteger" is available
             *      in OpenGL ES 3.0, only floating-point image data are
             *      available in OpenGL ES 2.0.
             */
            GreenInteger = GL_GREEN_INTEGER,

            /**
             * Integer blue channel.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gl Only @ref Format "Format::RedInteger" is available
             *      in OpenGL ES 3.0, only floating-point image data are
             *      available in OpenGL ES 2.0.
             */
            BlueInteger = GL_BLUE_INTEGER,
            #endif

            /**
             * Integer red and green channel.
             * @requires_gl30 %Extension @extension{ARB,texture_rg} and
             *      @extension{EXT,texture_integer}
             * @requires_gles30 Only floating-point image data are available
             *      in OpenGL ES 2.0.
             */
            RGInteger = GL_RG_INTEGER,

            /**
             * Integer RGB.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only floating-point image data are available
             *      in OpenGL ES 2.0.
             */
            RGBInteger = GL_RGB_INTEGER,

            /**
             * Integer RGBA.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gles30 Only floating-point image data are available
             *      in OpenGL ES 2.0.
             */
            RGBAInteger = GL_RGBA_INTEGER,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Integer BGR.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gl Only @ref Format "Format::RGBInteger" is available
             *      in OpenGL ES 3.0, only floating-point image data are
             *      available in OpenGL ES 2.0.
             */
            BGRInteger = GL_BGR_INTEGER,

            /**
             * Integer BGRA.
             * @requires_gl30 %Extension @extension{EXT,texture_integer}
             * @requires_gl Only @ref Format "Format::RGBAInteger" is
             *      available in OpenGL ES 3.0, only floating-point image data
             *      are available in OpenGL ES 2.0.
             */
            BGRAInteger = GL_BGRA_INTEGER,
            #endif
            #endif

            /**
             * Depth component. For framebuffer reading only.
             * @requires_gles30 %Extension @es_extension2{NV,read_depth,GL_NV_read_depth_stencil}
             */
            DepthComponent = GL_DEPTH_COMPONENT,

            /**
             * Stencil index. For framebuffer reading only.
             * @requires_es_extension %Extension @es_extension2{NV,read_stencil,GL_NV_read_depth_stencil}
             * @todo Where to get GL_STENCIL_INDEX in ES?
             */
            #ifndef MAGNUM_TARGET_GLES
            StencilIndex = GL_STENCIL_INDEX,
            #else
            StencilIndex = 0x1901,
            #endif

            /**
             * Depth and stencil. For framebuffer reading only.
             * @requires_gl30 %Extension @extension{EXT,packed_depth_stencil}
             * @requires_gles30 %Extension @es_extension2{NV,read_depth_stencil,GL_NV_read_depth_stencil}
             */
            #ifndef MAGNUM_TARGET_GLES2
            DepthStencil = GL_DEPTH_STENCIL
            #else
            DepthStencil = GL_DEPTH_STENCIL_OES
            #endif
        };

        /**
         * @brief Data type of pixel data
         *
         * @see pixelSize()
         */
        enum class Type: GLenum {
            /** Each component unsigned byte. */
            UnsignedByte = GL_UNSIGNED_BYTE,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Each component signed byte.
             * @requires_gles30 Only @ref Magnum::AbstractImage::Type "Type::UnsignedByte"
             *      is available in OpenGL ES 2.0.
             */
            Byte = GL_BYTE,
            #endif

            /**
             * Each component unsigned short.
             * @requires_gles30 %Extension @es_extension{OES,depth_texture}
             */
            UnsignedShort = GL_UNSIGNED_SHORT,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Each component signed short.
             * @requires_gles30 Only @ref Magnum::AbstractImage::Type "Type::UnsignedShort"
             *      is available in OpenGL ES 2.0.
             */
            Short = GL_SHORT,
            #endif

            /**
             * Each component unsigned int.
             * @requires_gles30 %Extension @es_extension{OES,depth_texture}
             */
            UnsignedInt = GL_UNSIGNED_INT,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Each component signed int.
             * @requires_gles30 Only @ref Magnum::AbstractImage::Type "Type::UnsignedInt"
             *      is available in OpenGL ES 2.0.
             */
            Int = GL_INT,
            #endif

            /**
             * Each component half float. For framebuffer reading only.
             * @requires_gl30 %Extension @extension{NV,half_float} / @extension{ARB,half_float_pixel}
             * @requires_gles30 %Extension @es_extension2{OES,texture_half_float,OES_texture_float},
             *      for texture data only.
             */
            #ifndef MAGNUM_TARGET_GLES2
            HalfFloat = GL_HALF_FLOAT,
            #else
            HalfFloat = GL_HALF_FLOAT_OES,
            #endif

            /**
             * Each component float.
             * @requires_gles30 %Extension @es_extension{OES,texture_float}
             */
            Float = GL_FLOAT,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * RGB, unsigned byte, red and green component 3bit, blue
             * component 2bit.
             * @requires_gl Packed 12bit types are not available in OpenGL ES.
             */
            UnsignedByte332 = GL_UNSIGNED_BYTE_3_3_2,

            /**
             * BGR, unsigned byte, red and green component 3bit, blue
             * component 2bit.
             * @requires_gl Packed 12bit types are not available in OpenGL ES.
             */
            UnsignedByte233Rev = GL_UNSIGNED_BYTE_2_3_3_REV,
            #endif

            /** RGB, unsigned byte, red and blue component 5bit, green 6bit. */
            UnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * BGR, unsigned short, red and blue 5bit, green 6bit.
             * @requires_gl Only @ref Magnum::AbstractImage::Type "Type::RGB565"
             *      is available in OpenGL ES.
             */
            UnsignedShort565Rev = GL_UNSIGNED_SHORT_5_6_5_REV,
            #endif

            /** RGBA, unsigned short, each component 4bit. */
            UnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,

            /**
             * ABGR, unsigned short, each component 4bit.
             * @requires_es_extension %Extension @es_extension{EXT,read_format_bgra},
             *      for framebuffer reading only.
             */
            #ifndef MAGNUM_TARGET_GLES
            UnsignedShort4444Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
            #else
            UnsignedShort4444Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT,
            #endif

            /**
             * RGBA, unsigned short, each RGB component 5bit, alpha component
             * 1bit.
             */
            UnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,

            /**
             * ABGR, unsigned short, each RGB component 5bit, alpha component
             * 1bit.
             * @requires_es_extension %Extension @es_extension{EXT,read_format_bgra},
             *      for framebuffer reading only.
             */
            #ifndef MAGNUM_TARGET_GLES
            UnsignedShort1555Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
            #else
            UnsignedShort1555Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * RGBA, unsigned int, each component 8bit.
             * @requires_gl Use @ref Magnum::AbstractImage::Type "Type::UnsignedByte"
             *      in OpenGL ES instead.
             */
            UnsignedInt8888 = GL_UNSIGNED_INT_8_8_8_8,

            /**
             * ABGR, unsigned int, each component 8bit.
             * @requires_gl Only RGBA component ordering is available in
             *      OpenGL ES, see @ref Magnum::AbstractImage::Format "Format::UnsignedInt8888"
             *      for more information.
             */
            UnsignedInt8888Rev = GL_UNSIGNED_INT_8_8_8_8_REV,

            /**
             * RGBA, unsigned int, each RGB component 10bit, alpha component
             * 2bit.
             * @requires_gl Only @ref Magnum::AbstractImage::Type "Type::UnsignedInt2101010Rev"
             *      is available in OpenGL ES.
             */
            UnsignedInt1010102 = GL_UNSIGNED_INT_10_10_10_2,
            #endif

            /**
             * ABGR, unsigned int, each RGB component 10bit, alpha component
             * 2bit.
             * @requires_gles30 %Extension @es_extension{EXT,texture_type_2_10_10_10_REV},
             *      for texture data only.
             */
            #ifndef MAGNUM_TARGET_GLES2
            UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,
            #else
            UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV_EXT,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * BGR, unsigned int, red and green 11bit float, blue 10bit float.
             * For framebuffer reading only.
             * @requires_gl30 %Extension @extension{EXT,packed_float}
             * @requires_gles30 Floating-point types are not available in
             *      OpenGL ES 2.0.
             */
            UnsignedInt10F11F11FRev = GL_UNSIGNED_INT_10F_11F_11F_REV,

            /**
             * BGR, unsigned int, each component 9bit + 5bit exponent. For
             * framebuffer reading only.
             * @requires_gl30 %Extension @extension{EXT,texture_shared_exponent}
             * @requires_gles30 Only 8bit and 16bit types are available in
             *      OpenGL ES 2.0.
             */
            UnsignedInt5999Rev = GL_UNSIGNED_INT_5_9_9_9_REV,
            #endif

            /**
             * Unsigned int, depth component 24bit, stencil index 8bit. For
             * framebuffer reading only.
             * @requires_gl30 %Extension @extension{EXT,packed_depth_stencil}
             * @requires_gles30 %Extension @es_extension{OES,packed_depth_stencil}
             */
            #ifdef MAGNUM_TARGET_GLES2
            UnsignedInt248 = GL_UNSIGNED_INT_24_8_OES,
            #else
            UnsignedInt248 = GL_UNSIGNED_INT_24_8,

            /**
             * Float + unsigned int, depth component 32bit float, 24bit gap,
             * stencil index 8bit. For framebuffer reading only.
             * @requires_gl30 %Extension @extension{ARB,depth_buffer_float}
             * @requires_gles30 Only @ref Magnum::AbstractImage::Type "Type::UnsignedInt248"
             *      is available in OpenGL ES 2.0.
             */
            Float32UnsignedInt248Rev = GL_FLOAT_32_UNSIGNED_INT_24_8_REV
            #endif
        };

        /*@}*/

        /**
         * @brief Pixel size (in bytes)
         * @param format            Format of the pixel
         * @param type              Data type of the pixel
         */
        static std::size_t pixelSize(Format format, Type type);

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         */
        inline explicit AbstractImage(Format format, Type type): _format(format), _type(type) {}

        /** @brief Destructor */
        virtual ~AbstractImage() = 0;

        /** @brief Format of pixel data */
        inline Format format() const { return _format; }

        /** @brief Data type of pixel data */
        inline Type type() const { return _type; }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        Format _format;
        Type _type;
    #endif
};

inline AbstractImage::~AbstractImage() {}

/** @debugoperator{Magnum::AbstractImage} */
Debug MAGNUM_EXPORT operator<<(Debug debug, AbstractImage::Format value);

/** @debugoperator{Magnum::AbstractImage} */
Debug MAGNUM_EXPORT operator<<(Debug debug, AbstractImage::Type value);

}

#endif
