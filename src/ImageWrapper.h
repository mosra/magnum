#ifndef Magnum_ImageWrapper_h
#define Magnum_ImageWrapper_h
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
 * @brief Class Magnum::ImageWrapper
 */

#include "Math/Vector3.h"
#include "AbstractImage.h"
#include "DimensionTraits.h"

namespace Magnum {

/**
@brief %Image wrapper

Adds information about dimensions, color components and component type to some
data in memory.

Unlike Image, this class doesn't delete the data on destruction, so it is
targeted for wrapping data which are either stored in stack/constant memory
(and shouldn't be deleted) or they are managed by someone else and have the
same properties for each frame, such as video stream. Thus it is not possible
to change image properties, only data pointer.

Interchangeable with Image, BufferImage or Trade::ImageData.
@see ImageWrapper1D, ImageWrapper2D, ImageWrapper3D
*/
template<std::uint8_t dimensions> class ImageWrapper: public AbstractImage {
    public:
        const static std::uint8_t Dimensions = dimensions; /**< @brief %Image dimension count */
        /**
         * @brief Constructor
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param data              %Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        inline explicit ImageWrapper(const typename DimensionTraits<Dimensions, GLsizei>::VectorType& size, Format format, Type type, GLvoid* data): AbstractImage(format, type), _size(size), _data(reinterpret_cast<char*>(data)) {}

        /**
         * @brief Constructor
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Dimensions and data pointer are set to zero, call setData() to fill
         * the image with data.
         */
        inline explicit ImageWrapper(const typename DimensionTraits<Dimensions, GLsizei>::VectorType& size, Format format, Type type): AbstractImage(format, type), _size(size), _data(nullptr) {}

        /** @brief %Image size */
        inline typename DimensionTraits<Dimensions, GLsizei>::VectorType size() const { return _size; }

        /** @brief Pointer to raw data */
        inline void* data() { return _data; }
        inline const void* data() const { return _data; } /**< @overload */

        /**
         * @brief Set image data
         * @param data              %Image data
         *
         * Dimensions, color compnents and data type remains the same as
         * passed in constructor. The data are not copied nor deleted on
         * destruction.
         */
        inline void setData(GLvoid* data) {
            _data = reinterpret_cast<char*>(data);
        }

    private:
        Math::Vector<Dimensions, GLsizei> _size;
        char* _data;
};

/** @brief One-dimensional image wrapper */
typedef ImageWrapper<1> ImageWrapper1D;

/** @brief Two-dimensional image wrapper */
typedef ImageWrapper<2> ImageWrapper2D;

/** @brief Three-dimensional image wrapper */
typedef ImageWrapper<3> ImageWrapper3D;

}

#endif
