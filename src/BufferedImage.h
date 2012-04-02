#ifndef Magnum_BufferedImage_h
#define Magnum_BufferedImage_h
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
 * @brief Class Magnum::BufferedImage
 */

#include "AbstractImage.h"
#include "Buffer.h"
#include "TypeTraits.h"

namespace Magnum {

/**
@brief %Buffered image

Class for storing image data in GPU memory. Can be replaced with Image, which
stores image data in client memory, or for example with Trade::ImageData.
*/
template<size_t imageDimensions> class BufferedImage: public AbstractImage {
    public:
        const static size_t Dimensions = imageDimensions;   /**< @brief Image dimension count */

        /**
         * @brief Constructor
         * @param components        Color components
         * @param type              Data type
         *
         * Dimensions and buffer are empty, call setData() to fill the image
         * with data.
         */
        BufferedImage(Components components, ComponentType type): AbstractImage(components, type), _buffer(Buffer::Target::PixelPack) {}

        /** @brief %Image dimensions */
        inline Math::Vector<GLsizei, Dimensions> dimensions() const { return _dimensions; }

        /**
         * @brief Data
         *
         * Binds the buffer to @ref Buffer::Target "pixel unpack
         * target" and returns nullptr, so it can be used for texture updating
         * functions the same way as Image::data().
         */
        void* data() {
            _buffer.bind(Buffer::Target::PixelUnpack);
            return nullptr;
        }

        /** @brief %Image buffer */
        inline Buffer* buffer() { return &_buffer; }

        /**
         * @brief Set image data
         * @param dimensions        %Image dimensions
         * @param components        Color components. Data type is detected
         *      from passed data array.
         * @param data              %Image data
         * @param usage             %Image buffer usage
         *
         * Updates the image buffer with given data. The data are not deleted
         * after filling the buffer.
         */
        template<class T> inline void setData(const Math::Vector<GLsizei, Dimensions>& dimensions, Components components, const T* data, Buffer::Usage usage) {
            setData(dimensions, components, TypeTraits<T>::imageType(), data, usage);
        }

        /**
         * @brief Set image data
         * @param dimensions        %Image dimensions
         * @param components        Color components
         * @param type              Data type
         * @param data              %Image data
         * @param usage             %Image buffer usage
         *
         * Updates the image buffer with given data. The data are not deleted
         * after filling the buffer.
         */
        void setData(const Math::Vector<GLsizei, Dimensions>& dimensions, Components components, ComponentType type, const GLvoid* data, Buffer::Usage usage) {
            _components = components;
            _type = type;
            _dimensions = dimensions;
            _buffer.setData(Buffer::Target::PixelPack, pixelSize(_components, _type)*dimensions.product(), data, usage);
        }

    protected:
        Math::Vector<GLsizei, Dimensions> _dimensions;  /**< @brief %Image dimensions */
        Buffer _buffer;                                 /**< @brief %Image buffer */
};

/** @brief One-dimensional buffered image */
typedef BufferedImage<1> BufferedImage1D;

/** @brief Two-dimensional buffered image */
typedef BufferedImage<2> BufferedImage2D;

/** @brief Three-dimensional buffered image */
typedef BufferedImage<3> BufferedImage3D;

}

#endif
