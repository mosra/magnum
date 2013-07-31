#ifndef Magnum_Trade_ImageData_h
#define Magnum_Trade_ImageData_h
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
 * @brief Class Magnum::Trade::ImageData
 */

#include "ImageReference.h"

namespace Magnum { namespace Trade {

/**
@brief %Image data

Access to image data provided by AbstractImporter subclasses. Interchangeable
with Image, ImageReference or BufferImage.
@see ImageData1D, ImageData2D, ImageData3D
*/
template<UnsignedInt dimensions> class ImageData: public AbstractImage {
    public:
        const static UnsignedInt Dimensions = dimensions; /**< @brief %Image dimension count */

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              %Image size
         * @param data              %Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        explicit ImageData(ImageFormat format, ImageType type, const typename DimensionTraits<Dimensions, Int>::VectorType& size, void* data): AbstractImage(format, type), _size(size), _data(reinterpret_cast<unsigned char*>(data)) {}

        /** @brief Copying is not allowed */
        ImageData(const ImageData<dimensions>&& other) = delete;

        /** @brief Move constructor */
        ImageData(ImageData<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        ImageData<dimensions>& operator=(const ImageData<dimensions>&& other) = delete;

        /** @brief Move assignment */
        ImageData<dimensions>& operator=(ImageData<dimensions>&& other) noexcept;

        /** @brief Destructor */
        ~ImageData() { delete[] _data; }

        /**
         * @brief Conversion to reference
         *
         * @todo GCC 4.8: don't allow this on rvalue-ref
         */
        /*implicit*/ operator ImageReference<dimensions>() const;

        /** @brief %Image size */
        typename DimensionTraits<Dimensions, Int>::VectorType size() const { return _size; }

        /** @brief Pointer to raw data */
        unsigned char* data() { return _data; }
        const unsigned char* data() const { return _data; } /**< @overload */

    private:
        Math::Vector<Dimensions, Int> _size;
        unsigned char* _data;
};

/** @brief One-dimensional image */
typedef ImageData<1> ImageData1D;

/** @brief Two-dimensional image */
typedef ImageData<2> ImageData2D;

/** @brief Three-dimensional image */
typedef ImageData<3> ImageData3D;

template<UnsignedInt dimensions> inline ImageData<dimensions>::ImageData(ImageData<dimensions>&& other) noexcept: AbstractImage(std::move(other)), _size(std::move(other._size)), _data(std::move(other._data)) {
    other._size = {};
    other._data = nullptr;
}

template<UnsignedInt dimensions> inline ImageData<dimensions>& ImageData<dimensions>::operator=(ImageData<dimensions>&& other) noexcept {
    AbstractImage::operator=(std::move(other));
    std::swap(_size, other._size);
    std::swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> inline ImageData<dimensions>::operator ImageReference<dimensions>() const {
    return ImageReference<dimensions>(AbstractImage::format(), AbstractImage::type(), _size, _data);
}

}}

#endif
