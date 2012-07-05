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

#include "Magnum.h"

namespace Magnum {

/** @ingroup textures
@brief Non-templated base for one-, two- or three-dimensional images

See Image, BufferedImage, Trade::ImageData documentation for more information.
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
         * Texture::setData() and others).
         */

        /** @brief Color components */
        enum class Components: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * One-component (red channel)
             * @requires_gl
             */
            Red = GL_RED,

            /**
             * One-component (green channel). For framebuffer reading only.
             * @requires_gl
             */
            Green = GL_GREEN,

            /**
             * One-component (green channel). For framebuffer reading only.
             * @requires_gl
             */
            Blue = GL_BLUE,

            /**
             * Two-component (red and green channel). For texture data only.
             * @requires_gl
             */
            RedGreen = GL_RG,
            #endif

            RGB = GL_RGB,                   /**< Three-component RGB */
            RGBA = GL_RGBA,                 /**< Four-component RGBA */

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Three-component BGR
             * @requires_gl
             */
            BGR = GL_BGR,

            /**
             * Four-component BGRA
             * @requires_gl
             */
            BGRA = GL_BGRA,
            #endif

            /** Depth component. For framebuffer reading only. */
            Depth = GL_DEPTH_COMPONENT,

            /** Stencil index. For framebuffer reading only. */
            StencilIndex = GL_STENCIL_INDEX

            #ifndef MAGNUM_TARGET_GLES
            ,
            /**
             * Depth and stencil component. For framebuffer reading only.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,packed_depth_stencil}
             */
            DepthStencil = GL_DEPTH_STENCIL
            #endif
        };

        /** @brief Data type */
        enum class ComponentType: GLenum {
            UnsignedByte = GL_UNSIGNED_BYTE, /**< Each component unsigned byte */
            Byte = GL_BYTE,                 /**< Each component byte */
            UnsignedShort = GL_UNSIGNED_SHORT, /**< Each component unsigned short */
            Short = GL_SHORT,               /**< Each component short */
            UnsignedInt = GL_UNSIGNED_INT,  /**< Each component unsigned int */
            Int = GL_INT,                   /**< Each component int */

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Each component half float (16bit). For framebuffer reading only.
             *
             * @requires_gl
             * @requires_gl30 Extension @extension{NV,half_float} / @extension{ARB,half_float_pixel}
             */
            HalfFloat = GL_HALF_FLOAT,
            #endif

            Float = GL_FLOAT,               /**< Each component float (32bit) */

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Three-component RGB, unsigned normalized, red and green 3bit,
             * blue 2bit, 8bit total.
             * @requires_gl
             */
            RGB332 = GL_UNSIGNED_BYTE_3_3_2,

            /**
             * Three-component BGR, unsigned normalized, red and green 3bit,
             * blue 2bit, 8bit total.
             * @requires_gl
             */
            BGR233 = GL_UNSIGNED_BYTE_2_3_3_REV,
            #endif

            /**
             * Three-component RGB, unsigned normalized, red and blue 5bit,
             * green 6bit, 16bit total.
             */
            RGB565 = GL_UNSIGNED_SHORT_5_6_5,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Three-component BGR, unsigned normalized, red and blue 5bit,
             * green 6bit, 16bit total.
             * @requires_gl
             */
            BGR565 = GL_UNSIGNED_SHORT_5_6_5_REV,
            #endif

            /**
             * Four-component RGBA, unsigned normalized, each component 4bit,
             * 16bit total.
             */
            RGBA4 = GL_UNSIGNED_SHORT_4_4_4_4,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Four-component ABGR, unsigned normalized, each component 4bit,
             * 16bit total.
             * @requires_gl
             */
            ABGR4 = GL_UNSIGNED_SHORT_4_4_4_4_REV,
            #endif

            /**
             * Four-component RGBA, unsigned normalized, each RGB component
             * 5bit, alpha 1bit, 16bit total.
             */
            RGB5Alpha1 = GL_UNSIGNED_SHORT_5_5_5_1

            #ifndef MAGNUM_TARGET_GLES
            ,

            /**
             * Four-component ABGR, unsigned normalized, each RGB component
             * 5bit, alpha 1bit, 16bit total.
             * @requires_gl
             */
            Alpha1BGR5 = GL_UNSIGNED_SHORT_1_5_5_5_REV,

            /**
             * Four-component RGBA, unsigned normalized, each component 8bit,
             * 32bit total.
             * @requires_gl
             */
            RGBA8 = GL_UNSIGNED_INT_8_8_8_8,

            /**
             * Four-component ABGR, unsigned normalized, each component 8bit,
             * 32bit total.
             * @requires_gl
             */
            ABGR8 = GL_UNSIGNED_INT_8_8_8_8_REV,

            /**
             * Four-component RGBA, unsigned normalized, each RGB component
             * 10bit, alpha 2bit, 32bit total.
             * @requires_gl
             */
            RGB10Alpha2 = GL_UNSIGNED_INT_10_10_10_2,

            /**
             * Four-component ABGR, unsigned normalized, each RGB component
             * 10bit, alpha 2bit, 32bit total.
             * @requires_gl
             */
            Alpha2RGB10 = GL_UNSIGNED_INT_2_10_10_10_REV,

            /**
             * Three-component BGR, float, red and green 11bit, blue 10bit,
             * 32bit total. For framebuffer reading only.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,packed_float}
             */
            B10GR11Float = GL_UNSIGNED_INT_10F_11F_11F_REV,

            /**
             * Three-component BGR, unsigned integers with exponent, each
             * component 9bit, exponent 5bit, 32bit total. For framebuffer
             * reading only.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,texture_shared_exponent}
             */
            Exponent5RGB9 = GL_UNSIGNED_INT_5_9_9_9_REV,

            /**
             * 24bit depth and 8bit stencil component, 32bit total. For
             * framebuffer reading only.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,packed_depth_stencil}
             */
            Depth24Stencil8 = GL_UNSIGNED_INT_24_8,

            /**
             * 32bit float depth component and 8bit stencil component, 64bit
             * total. For framebuffer reading only.
             * @requires_gl
             * @requires_gl30 Extension @extension{ARB,depth_buffer_float}
             */
            Depth32FloatStencil8 = GL_FLOAT_32_UNSIGNED_INT_24_8_REV
            #endif
        };

        /*@}*/

        /**
         * @brief Pixel size (in bytes)
         * @param components        Color components
         * @param type              Data type
         */
        static size_t pixelSize(Components components, ComponentType type);

        /**
         * @brief Constructor
         * @param components        Color components of passed data
         * @param type              %Image data type
         */
        inline AbstractImage(Components components, ComponentType type): _components(components), _type(type) {}

        /** @brief Destructor */
        virtual ~AbstractImage() = 0;

        /** @brief Color components */
        inline Components components() const { return _components; }

        /** @brief Data type */
        inline ComponentType type() const { return _type; }

    protected:
        Components _components;     /**< @brief Color components */
        ComponentType _type;        /**< @brief Data type */
};

inline AbstractImage::~AbstractImage() {}

}

#endif
