#ifndef Magnum_Image_h
#define Magnum_Image_h
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
 * @brief Class Magnum::Image
 */

#include "AbstractTexture.h"

namespace Magnum {

/**
@brief %Image

Class for storing image data on client memory. Can be replaced with
BufferedImage, which stores image data in GPU memory, or for example with
Trade::ImageData. See also Image2D, which has additional data updating
functions.
*/
template<size_t imageDimensions> class Image {
    Image<imageDimensions>(const Image<imageDimensions>& other) = delete;
    Image<imageDimensions>(Image<imageDimensions>&& other) = delete;
    Image<imageDimensions>& operator=(const Image<imageDimensions>& other) = delete;
    Image<imageDimensions>& operator=(Image<imageDimensions>&& other) = delete;

    public:
        const static size_t Dimensions = imageDimensions;   /**< @brief Image dimension count */

        /**
         * @brief Constructor
         * @param colorFormat       Color format of passed data. Data size
         *      per color channel is detected from format of passed data array.
         * @param dimensions        %Image dimensions
         * @param data              %Image data with proper size
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        template<class T> inline Image(AbstractTexture::ColorFormat colorFormat, const Math::Vector<GLsizei, Dimensions>& dimensions, T* data): _colorFormat(colorFormat), _type(TypeTraits<typename TypeTraits<T>::TextureType>::glType()), _dimensions(dimensions), _data(data) {}

        /**
         * @brief Constructor
         * @param colorFormat       Color format of passed data
         * @param type              Data type per color channel
         *
         * Dimensions and data pointer are are set to zero, call
         * setDimensions() and setData() to fill the image with data.
         */
        inline Image(AbstractTexture::ColorFormat colorFormat, Type type): _colorFormat(colorFormat), _type(type), _data(nullptr) {}

        /** @brief Destructor */
        inline virtual ~Image() { delete[] _data; }

        /** @brief %Image dimensions */
        inline const Math::Vector<GLsizei, Dimensions>& dimensions() const { return _dimensions; }

        /**
         * @brief Set image dimensions
         * @param dimensions    %Image dimensions
         *
         * Saves the dimensions and deletes the internal data array.
         */
        inline void setDimensions(const Math::Vector2<GLsizei>& dimensions) {
            _dimensions = dimensions;
            delete _data;
            _data = 0;
        }

        /** @brief Color format */
        inline AbstractTexture::ColorFormat colorFormat() const { return _colorFormat; }

        /** @brief Data type */
        inline Type type() const { return _type; }

        /** @brief Pointer to raw data */
        inline const void* data() const { return _data; }

        /**
         * @brief Set image data
         * @param data              %Image data
         *
         * Deletes previous data and replaces them with new. Note that the
         * data are not copied, but they are deleted on destruction. Also the
         * type of passed data must be the same as data type passed in
         * constructor.
         */
        template<class T> void setData(const T* data) {
            if(TypeTraits<typename TypeTraits<T>::TextureType>::glType() != _type) {
                Corrade::Utility::Error() << "Image: Passed data have wrong type";
                return;
            }

            delete _data;
            _data = reinterpret_cast<const char*>(data);
        }

    protected:
        AbstractTexture::ColorFormat _colorFormat;      /**< @brief Color format */
        Type _type;                                     /**< @brief Data type per color channel */
        Math::Vector<GLsizei, Dimensions> _dimensions;  /**< @brief %Image dimensions */
        char* _data;                                    /**< @brief %Image data */
};

/** @brief One-dimensional image */
typedef Image<1> Image1D;

/** @brief Two-dimensional image */
class MAGNUM_EXPORT Image2D: public Image<2> {
    public:
        /**
         * @brief Constructor
         * @param colorFormat       Color format of passed data
         * @param type              Data type per color channel
         *
         * Dimensions and data pointer are are set to zero, call
         * setDimensions() and setData() to fill the image with data.
         */
        /* doxygen: @copydoc Image::Image doesn't work */
        inline Image2D(AbstractTexture::ColorFormat colorFormat, Type type): Image(colorFormat, type) {}

        /**
         * @brief Set image data from current framebuffer
         * @param offset        Offset of the pixamp to read
         *
         * Reads pixmap from given offset with already set dimensions.
         */
        void setDataFromFramebuffer(const Math::Vector2<GLint>& offset);
};

/** @brief Three-dimensional image */
typedef Image<3> Image3D;

}

#endif
