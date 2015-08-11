#ifndef Magnum_Image_h
#define Magnum_Image_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Image, @ref Magnum::CompressedImage typedef @ref Magnum::Image1D, @ref Magnum::Image2D, @ref Magnum::Image3D, @ref Magnum::CompressedImage1D, @ref Magnum::CompressedImage2D, @ref Magnum::CompressedImage3D
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/ImageView.h"

namespace Magnum {

/**
@brief Image

Stores image data on client memory. Interchangeable with @ref ImageView,
@ref BufferImage or @ref Trade::ImageData.
@see @ref Image1D, @ref Image2D, @ref Image3D, @ref CompressedImage
*/
template<UnsignedInt dimensions> class Image {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        explicit Image(ColorFormat format, ColorType type, const VectorTypeFor<dimensions, Int>& size, void* data): _format{format}, _type{type}, _size{size}, _data{reinterpret_cast<char*>(data)} {}

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Dimensions are set to zero and data pointer to `nullptr`, call
         * @ref setData() to fill the image with data.
         */
        /*implicit*/ Image(ColorFormat format, ColorType type): _format{format}, _type{type}, _data{} {}

        /** @brief Copying is not allowed */
        Image(const Image<dimensions>&) = delete;

        /** @brief Move constructor */
        Image(Image<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        Image<dimensions>& operator=(const Image<dimensions>&) = delete;

        /** @brief Move assignment */
        Image<dimensions>& operator=(Image<dimensions>&& other) noexcept;

        /** @brief Destructor */
        ~Image() { delete[] _data; }

        /** @brief Conversion to view */
        /*implicit*/ operator ImageView<dimensions>()
        #ifndef CORRADE_GCC47_COMPATIBILITY
        const &;
        #else
        const;
        #endif

        #ifndef CORRADE_GCC47_COMPATIBILITY
        /** @overload */
        /*implicit*/ operator ImageView<dimensions>() const && = delete;
        #endif

        /** @brief Format of pixel data */
        ColorFormat format() const { return _format; }

        /** @brief Data type of pixel data */
        ColorType type() const { return _type; }

        /** @brief Pixel size (in bytes) */
        std::size_t pixelSize() const { return Implementation::imagePixelSize(_format, _type); }

        /** @brief Image size */
        VectorTypeFor<dimensions, Int> size() const { return _size; }

        /**
         * @brief Size of data required to store image of given size
         *
         * Takes color format, type and row alignment of this image into
         * account.
         * @see @ref pixelSize()
         */
        std::size_t dataSize(const VectorTypeFor<dimensions, Int>& size) const {
            return Implementation::imageDataSize<dimensions>(_format, _type, size);
        }

        /**
         * @brief Pointer to raw data
         *
         * @see @ref release()
         */
        template<class T = char> T* data() {
            return reinterpret_cast<T*>(_data);
        }

        /** @overload */
        template<class T = char> const T* data() const {
            return reinterpret_cast<const T*>(_data);
        }

        /**
         * @brief Set image data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Deletes previous data and replaces them with new. Note that the
         * data are not copied, but they are deleted on destruction.
         * @see @ref release()
         */
        void setData(ColorFormat format, ColorType type, const VectorTypeFor<dimensions, Int>& size, void* data);

        /**
         * @brief Release data storage
         *
         * Releases the ownership of the data pointer and resets internal state
         * to default. Deleting the returned array is then user responsibility.
         * @see @ref setData()
         */
        char* release();

    private:
        ColorFormat _format;
        ColorType _type;
        Math::Vector<Dimensions, Int> _size;
        char* _data;
};

/** @brief One-dimensional image */
typedef Image<1> Image1D;

/** @brief Two-dimensional image */
typedef Image<2> Image2D;

/** @brief Three-dimensional image */
typedef Image<3> Image3D;

/**
@brief Compressed image

Stores image data in client memory.

See @ref Image for more information. Interchangeable with
@ref CompressedImageView, @ref CompressedBufferImage or @ref Trade::ImageData.
@see @ref CompressedImage1D, @ref CompressedImage2D, @ref CompressedImage3D
*/
template<UnsignedInt dimensions> class CompressedImage {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Constructor
         * @param format            Format of compressed data
         * @param size              Image size
         * @param data              Image data
         */
        explicit CompressedImage(CompressedColorFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data): _format{format}, _size{size}, _data{std::move(data)} {}

        /**
         * @brief Constructor
         *
         * Format is undefined, size is zero and data are empty, call
         * @ref setData() to fill the image with data.
         */
        /*implicit*/ CompressedImage(): _format{} {}

        /** @brief Copying is not allowed */
        CompressedImage(const CompressedImage<dimensions>&) = delete;

        /** @brief Move constructor */
        CompressedImage(CompressedImage<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        CompressedImage<dimensions>& operator=(const CompressedImage<dimensions>&) = delete;

        /** @brief Move assignment */
        CompressedImage<dimensions>& operator=(CompressedImage<dimensions>&& other) noexcept;

        /** @brief Conversion to view */
        /*implicit*/ operator CompressedImageView<dimensions>()
        #ifndef CORRADE_GCC47_COMPATIBILITY
        const &;
        #else
        const;
        #endif

        #ifndef CORRADE_GCC47_COMPATIBILITY
        /** @overload */
        /*implicit*/ operator CompressedImageView<dimensions>() const && = delete;
        #endif

        /** @brief Format of compressed data */
        CompressedColorFormat format() const { return _format; }

        /** @brief Image size */
        VectorTypeFor<dimensions, Int> size() const { return _size; }

        /** @brief Raw data */
        Containers::ArrayView<char> data() { return _data; }

        /** @overload */
        Containers::ArrayView<const char> data() const { return _data; }

        /**
         * @brief Pointer to raw data
         *
         * @see @ref release()
         */
        template<class T> T* data() {
            return reinterpret_cast<T*>(_data.data());
        }

        /** @overload */
        template<class T> const T* data() const {
            return reinterpret_cast<const T*>(_data.data());
        }

        /**
         * @brief Set image data
         * @param format            Format of compressed data
         * @param size              Image size
         * @param data              Image data
         *
         * Deletes previous data and replaces them with new. Note that the
         * data are not copied, but they are deleted on destruction.
         * @see @ref release()
         */
        void setData(CompressedColorFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data);

        /**
         * @brief Release data storage
         *
         * Releases the ownership of the data pointer and resets internal state
         * to default.
         * @see @ref setData()
         */
        Containers::Array<char> release();

    private:
        CompressedColorFormat _format;
        Math::Vector<Dimensions, Int> _size;
        Containers::Array<char> _data;
};

/** @brief One-dimensional compressed image */
typedef CompressedImage<1> CompressedImage1D;

/** @brief Two-dimensional compressed image */
typedef CompressedImage<2> CompressedImage2D;

/** @brief Three-dimensional compressed image */
typedef CompressedImage<3> CompressedImage3D;

template<UnsignedInt dimensions> inline Image<dimensions>::Image(Image<dimensions>&& other) noexcept: _format{std::move(other._format)}, _type{std::move(other._type)}, _size{std::move(other._size)}, _data{std::move(other._data)} {
    other._size = {};
    other._data = nullptr;
}

template<UnsignedInt dimensions> inline CompressedImage<dimensions>::CompressedImage(CompressedImage<dimensions>&& other) noexcept: _format{std::move(other._format)}, _size{std::move(other._size)}, _data{std::move(other._data)} {
    other._size = {};
    other._data = nullptr;
}

template<UnsignedInt dimensions> inline Image<dimensions>& Image<dimensions>::operator=(Image<dimensions>&& other) noexcept {
    using std::swap;
    swap(_format, other._format);
    swap(_type, other._type);
    swap(_size, other._size);
    swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> inline CompressedImage<dimensions>& CompressedImage<dimensions>::operator=(CompressedImage<dimensions>&& other) noexcept {
    using std::swap;
    swap(_format, other._format);
    swap(_size, other._size);
    swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> inline Image<dimensions>::operator ImageView<dimensions>()
#ifndef CORRADE_GCC47_COMPATIBILITY
const &
#else
const
#endif
{
    return ImageView<dimensions>{_format, _type, _size, _data};
}

template<UnsignedInt dimensions> inline CompressedImage<dimensions>::operator CompressedImageView<dimensions>()
#ifndef CORRADE_GCC47_COMPATIBILITY
const &
#else
const
#endif
{
    return CompressedImageView<dimensions>{_format, _size, _data};
}

template<UnsignedInt dimensions> inline char* Image<dimensions>::release() {
    /** @todo I need `std::exchange` NOW. */
    char* const data = _data;
    _size = {};
    _data = nullptr;
    return data;
}

template<UnsignedInt dimensions> inline Containers::Array<char> CompressedImage<dimensions>::release() {
    /** @todo I need `std::exchange` NOW. */
    Containers::Array<char> data{std::move(_data)};
    _size = {};
    _data = nullptr;
    return data;
}

}

#endif
