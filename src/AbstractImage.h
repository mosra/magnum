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
#include "TypeTraits.h"

namespace Magnum {

/**
@brief Non-templated base for one-, two- or three-dimensional images

See Image, BufferedImage, Trade::ImageData documentation for more information.
*/
class AbstractImage {
    AbstractImage(const AbstractImage& other) = delete;
    AbstractImage(AbstractImage&& other) = delete;
    AbstractImage& operator=(const AbstractImage& other) = delete;
    AbstractImage& operator=(AbstractImage&& other) = delete;

    public:
        /** @brief Color format */
        enum class ColorFormat: GLenum {
            Red = GL_RED,       /**< One-component (red channel) */
            RedGreen = GL_RG,   /**< Two-component (red and green channel) */
            RGB = GL_RGB,       /**< Three-component (RGB) */
            RGBA = GL_RGBA,     /**< Four-component (RGBA) */
            BGR = GL_BGR,       /**< Three-component (BGR) */
            BGRA = GL_BGRA      /**< Four-component (BGRA) */
        };

        /**
         * @brief Pixel size (in bytes)
         * @param format    Color format
         * @param type      Data type per color channel
         */
        static size_t pixelSize(ColorFormat format, Type type);

        /**
         * @brief Constructor
         * @param colorFormat       Color format of passed data
         * @param type              %Image data type
         */
        inline AbstractImage(ColorFormat colorFormat, Type type): _colorFormat(colorFormat), _type(type) {}

        /** @brief Destructor */
        inline virtual ~AbstractImage() {}

        /** @brief Color format */
        inline ColorFormat colorFormat() const { return _colorFormat; }

        /** @brief Data type */
        inline Type type() const { return _type; }

    protected:
        ColorFormat _colorFormat;   /**< @brief Color format */
        Type _type;                 /**< @brief Data type per color channel */
};

}

#endif
