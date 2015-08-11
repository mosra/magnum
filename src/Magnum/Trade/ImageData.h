#ifndef Magnum_Trade_ImageData_h
#define Magnum_Trade_ImageData_h
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
 * @brief Class @ref Magnum::Trade::ImageData, typedef @ref Magnum::Trade::ImageData1D, @ref Magnum::Trade::ImageData2D, @ref Magnum::Trade::ImageData3D
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/ImageView.h"

namespace Magnum { namespace Trade {

/**
@brief Image data

Access to either uncompressed or compressed image data provided by
@ref AbstractImporter subclasses, the compression state is distinguished with
@ref isCompressed(). Uncompressed images have @ref format(), @ref type(),
@ref pixelSize() and @ref dataSize() properties and are convertible to
@ref ImageView. Compressed images have just the @ref compressedFormat()
property and are convertible to @ref CompressedImageView.

Uncompressed image is interchangeable with @ref Image, @ref ImageView or
@ref BufferImage, compressed with @ref CompressedImage, @ref CompressedImageView
or @ref CompressedBufferImage.
@see @ref ImageData1D, @ref ImageData2D, @ref ImageData3D
*/
template<UnsignedInt dimensions> class ImageData {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Construct uncompressed image data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        explicit ImageData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, void* data): _compressed{false}, _format{format}, _type{type}, _size{size}, _data{reinterpret_cast<char*>(data), dataSize(size)} {}

        /**
         * @brief Construct compressed image data
         * @param format            Format of compressed data
         * @param size              Image size
         * @param data              Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        explicit ImageData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data): _compressed{true}, _compressedFormat{format}, _size{size}, _data{std::move(data)} {}

        /** @brief Copying is not allowed */
        ImageData(const ImageData<dimensions>&) = delete;

        /** @brief Move constructor */
        ImageData(ImageData<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        ImageData<dimensions>& operator=(const ImageData<dimensions>&) = delete;

        /** @brief Move assignment */
        ImageData<dimensions>& operator=(ImageData<dimensions>&& other) noexcept;

        /** @brief Whether the image is compressed */
        bool isCompressed() const { return _compressed; }

        /**
         * @brief Conversion to view
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed()
         */
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

        /**
         * @brief Conversion to compressed view
         *
         * The image is expected to be compressed.
         * @see @ref isCompressed()
         */
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

        /**
         * @brief Format of pixel data
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed()
         */
        PixelFormat format() const;

        /**
         * @brief Data type of pixel data
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed()
         */
        PixelType type() const;

        /**
         * @brief Format of compressed data
         *
         * The image is expected to be compressed.
         * @see @ref isCompressed()
         */
        CompressedPixelFormat compressedFormat() const;

        /**
         * @brief Pixel size (in bytes)
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed()
         */
        std::size_t pixelSize() const;

        /** @brief Image size */
        VectorTypeFor<dimensions, Int> size() const { return _size; }

        /**
         * @brief Size of data required to store uncompressed image of given size
         *
         * The image is expected to be uncompressed. Takes color format, type
         * and row alignment of this image into account.
         * @see @ref isCompressed(), @ref pixelSize()
         */
        std::size_t dataSize(const VectorTypeFor<dimensions, Int>& size) const;

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
         * @brief Release data storage
         *
         * Releases the ownership of the data pointer and resets internal state
         * to default. Deleting the returned array is then user responsibility.
         * @see @ref data()
         */
        Containers::Array<char> release();

    private:
        bool _compressed;
        union {
            PixelFormat _format;
            CompressedPixelFormat _compressedFormat;
        };
        PixelType _type;
        Math::Vector<Dimensions, Int> _size;
        Containers::Array<char> _data;
};

/** @brief One-dimensional image */
typedef ImageData<1> ImageData1D;

/** @brief Two-dimensional image */
typedef ImageData<2> ImageData2D;

/** @brief Three-dimensional image */
typedef ImageData<3> ImageData3D;

template<UnsignedInt dimensions> inline ImageData<dimensions>::ImageData(ImageData<dimensions>&& other) noexcept: _compressed{std::move(other._compressed)}, _type{std::move(other._type)}, _size{std::move(other._size)}, _data{std::move(other._data)} {
    if(_compressed) _compressedFormat = std::move(other._compressedFormat);
    else _format = std::move(other._format);

    other._size = {};
    other._data = nullptr;
}

template<UnsignedInt dimensions> inline ImageData<dimensions>& ImageData<dimensions>::operator=(ImageData<dimensions>&& other) noexcept {
    using std::swap;
    swap(_compressed, other._compressed);
    if(_compressed) swap(_compressedFormat, other._compressedFormat);
    else swap(_format, other._format);
    swap(_type, other._type);
    swap(_size, other._size);
    swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> inline Containers::Array<char> ImageData<dimensions>::release() {
    Containers::Array<char> data{std::move(_data)};
    _size = {};
    _data = nullptr;
    return data;
}

}}

#endif
