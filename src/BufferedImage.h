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

#include "AbstractTexture.h"
#include "Buffer.h"

namespace Magnum {

/**
@brief %Buffered image

Class for storing image data in GPU memory. Can be replaced with Image, which
stores image data in client memory, or for example with Trade::ImageData. See
also BufferedImage2D, which has additional data updating functions.
*/
template<size_t imageDimensions> class BufferedImage {
    BufferedImage(const BufferedImage<imageDimensions>& other) = delete;
    BufferedImage(BufferedImage<imageDimensions>&& other) = delete;
    BufferedImage& operator=(const BufferedImage<imageDimensions>& other) = delete;
    BufferedImage& operator=(BufferedImage<imageDimensions>&& other) = delete;

    public:
        const static size_t Dimensions = imageDimensions;   /**< @brief Image dimension count */

        /**
         * @brief Constructor
         * @param colorFormat   Color format of the data.
         * @param type          Data type per color channel
         */
        BufferedImage(AbstractTexture::ColorFormat colorFormat, Type type): _colorFormat(colorFormat), _type(type), _buffer(Buffer::Target::PixelPack) {}

        inline virtual ~BufferedImage() {}

        /** @brief %Image dimensions */
        inline Math::Vector<GLsizei, Dimensions> dimensions() const { return _dimensions; }

        /**
         * @brief Set image dimensions
         * @param dimensions    %Image dimensions
         * @param usage         %Image buffer usage
         *
         * Saves the dimensions and resizes the buffer to be able to hold
         * given pixmap size.
         */
        void setDimensions(const Math::Vector<GLsizei, Dimensions>& dimensions, Buffer::Usage usage) {
            _dimensions = dimensions;
            size_t textureSize = AbstractTexture::pixelSize(AbstractTexture::ColorFormat::RGB, Type::UnsignedByte)*dimensions.product();
            _buffer.setData(Buffer::Target::PixelPack, textureSize, nullptr, usage);
        }

        /** @brief Color format */
        inline AbstractTexture::ColorFormat colorFormat() const { return _colorFormat; }

        /** @brief Data type */
        inline Type type() const { return _type; }

        /**
         * @brief Data
         *
         * Binds the buffer to @ref Buffer::Target::PixelUnpack "pixel unpack
         * target" and returns nullptr, so it can be used for texture updating
         * functions the same way as Image::data().
         */
        void* data() {
            _buffer.bind(Buffer::Target::PixelUnpack);
            return nullptr;
        }

        /**
         * @brief Set image data
         * @param data              %Image data
         *
         * Updates the image buffer with given data. The data are not deleted
         * after filling the buffer. Note that the data must have the right
         * size and type of passed data must be the same as data type passed
         * in constructor.
         */
        template<class T> void setData(const T* data) {
            if(TypeTraits<typename TypeTraits<T>::TextureType>::glType() != _type) {
                Corrade::Utility::Error() << "BufferedImage: Passed data have type" << TypeTraits<typename TypeTraits<T>::TextureType>::glType() << "but it should be" << _type;
                return;
            }

            size_t textureSize = AbstractTexture::pixelSize(AbstractTexture::ColorFormat::RGB, Type::UnsignedByte)*dimensions.product();
            _buffer.setSubData(Buffer::Target::PixelPack, 0, textureSize, data);
        }

    protected:
        AbstractTexture::ColorFormat _colorFormat;      /**< @brief Color format */
        Type _type;                                     /**< @brief Data type per color channel */
        Math::Vector<GLsizei, Dimensions> _dimensions;  /**< @brief %Image dimensions */
        Buffer _buffer;                                 /**< @brief %Image buffer */
};

/** @brief One-dimensional buffered image */
typedef BufferedImage<1> BufferedImage1D;

/** @brief Two-dimensional buffered image */
class MAGNUM_EXPORT BufferedImage2D: public BufferedImage<2> {
    public:
        /** @copydoc BufferedImage::BufferedImage */
        inline BufferedImage2D(AbstractTexture::ColorFormat colorFormat, Type type): BufferedImage(colorFormat, type) {}

        /**
         * @brief Set image data from current framebuffer
         * @param offset        Offset of the pixamp to read
         *
         * Reads pixmap from given offset with already set dimensions.
         */
        void setDataFromFramebuffer(const Math::Vector2<GLint>& offset);
};

/** @brief Three-dimensional buffered image */
typedef BufferedImage<3> BufferedImage3D;

}

#endif
