#ifndef Magnum_Image_h
#define Magnum_Image_h
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
 * @brief Class @ref Magnum::Image, typedef @ref Magnum::Image1D, @ref Magnum::Image2D, @ref Magnum::Image3D
 */

#include "ImageReference.h"

namespace Magnum {

/**
@brief %Image

Stores image data on client memory. Interchangeable with @ref ImageReference,
@ref BufferImage or @ref Trade::ImageData.
@see @ref Image1D, @ref Image2D, @ref Image3D
*/
template<UnsignedInt dimensions> class Image: public AbstractImage {
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
        explicit Image(ColorFormat format, ColorType type, const typename DimensionTraits<Dimensions, Int>::VectorType& size, void* data): AbstractImage(format, type), _size(size), _data(reinterpret_cast<unsigned char*>(data)) {}

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Dimensions are set to zero and data pointer to `nullptr`, call
         * @ref setData() to fill the image with data.
         */
        explicit Image(ColorFormat format, ColorType type): AbstractImage(format, type), _data(nullptr) {}

        /** @brief Copying is not allowed */
        Image(const Image<dimensions>&& other) = delete;

        /** @brief Move constructor */
        Image(Image<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        Image<dimensions>& operator=(const Image<dimensions>&& other) = delete;

        /** @brief Move assignment */
        Image<dimensions>& operator=(Image<dimensions>&& other) noexcept;

        /** @brief Destructor */
        ~Image() { delete[] _data; }

        /** @brief Conversion to reference */
        /*implicit*/ operator ImageReference<dimensions>()
        #ifndef CORRADE_GCC47_COMPATIBILITY
        const &;
        #else
        const;
        #endif

        #ifndef CORRADE_GCC47_COMPATIBILITY
        /** @overload */
        /*implicit*/ operator ImageReference<dimensions>() const && = delete;
        #endif

        /** @brief %Image size */
        typename DimensionTraits<Dimensions, Int>::VectorType size() const { return _size; }

        /**
         * @brief Pointer to raw data
         *
         * @see @ref release()
         */
        unsigned char* data() { return _data; }
        const unsigned char* data() const { return _data; } /**< @overload */

        /**
         * @brief Set image data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              %Image size
         * @param data              %Image data
         *
         * Deletes previous data and replaces them with new. Note that the
         * data are not copied, but they are deleted on destruction.
         * @see @ref release()
         */
        void setData(ColorFormat format, ColorType type, const typename DimensionTraits<Dimensions, Int>::VectorType& size, void* data);

        /**
         * @brief Release data storage
         *
         * Returns the data pointer and resets internal state to default.
         * Deleting the returned array is user responsibility.
         * @see @ref setData()
         */
        unsigned char* release();

    private:
        Math::Vector<Dimensions, Int> _size;
        unsigned char* _data;
};

/** @brief One-dimensional image */
typedef Image<1> Image1D;

/** @brief Two-dimensional image */
typedef Image<2> Image2D;

/** @brief Three-dimensional image */
typedef Image<3> Image3D;

template<UnsignedInt dimensions> inline Image<dimensions>::Image(Image<dimensions>&& other) noexcept: AbstractImage(std::move(other)), _size(std::move(other._size)), _data(std::move(other._data)) {
    other._size = {};
    other._data = nullptr;
}

template<UnsignedInt dimensions> inline Image<dimensions>& Image<dimensions>::operator=(Image<dimensions>&& other) noexcept {
    AbstractImage::operator=(std::move(other));
    std::swap(_size, other._size);
    std::swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> inline Image<dimensions>::operator ImageReference<dimensions>()
#ifndef CORRADE_GCC47_COMPATIBILITY
const &
#else
const
#endif
{
    return ImageReference<dimensions>(AbstractImage::format(), AbstractImage::type(), _size, _data);
}

template<UnsignedInt dimensions> inline unsigned char* Image<dimensions>::release() {
    /** @todo I need `std::exchange` NOW. */
    unsigned char* const data = _data;
    _size = {};
    _data = nullptr;
    return data;
}

}

#endif
