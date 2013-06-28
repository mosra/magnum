#ifndef Magnum_ImageWrapper_h
#define Magnum_ImageWrapper_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
template<UnsignedInt dimensions> class ImageWrapper: public AbstractImage {
    public:
        const static UnsignedInt Dimensions = dimensions; /**< @brief %Image dimension count */

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
        constexpr explicit ImageWrapper(const typename DimensionTraits<Dimensions, Int>::VectorType& size, ImageFormat format, ImageType type, void* data): AbstractImage(format, type), _size(size), _data(reinterpret_cast<unsigned char*>(data)) {}

        /**
         * @brief Constructor
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Data pointer is set to zero, call setData() to fill the image with
         * data.
         */
        constexpr explicit ImageWrapper(const typename DimensionTraits<Dimensions, Int>::VectorType& size, ImageFormat format, ImageType type): AbstractImage(format, type), _size(size), _data(nullptr) {}

        /** @brief %Image size */
        constexpr typename DimensionTraits<Dimensions, Int>::VectorType size() const { return _size; }

        /** @brief Pointer to raw data */
        unsigned char* data() { return _data; }
        constexpr const unsigned char* data() const { return _data; } /**< @overload */

        /**
         * @brief Set image data
         * @param data              %Image data
         *
         * Dimensions, color compnents and data type remains the same as
         * passed in constructor. The data are not copied nor deleted on
         * destruction.
         */
        void setData(void* data) {
            _data = reinterpret_cast<unsigned char*>(data);
        }

    private:
        Math::Vector<Dimensions, Int> _size;
        unsigned char* _data;
};

/** @brief One-dimensional image wrapper */
typedef ImageWrapper<1> ImageWrapper1D;

/** @brief Two-dimensional image wrapper */
typedef ImageWrapper<2> ImageWrapper2D;

/** @brief Three-dimensional image wrapper */
typedef ImageWrapper<3> ImageWrapper3D;

}

#endif
