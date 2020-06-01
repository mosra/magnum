#ifndef Magnum_PixelStorage_h
#define Magnum_PixelStorage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::PixelStorage, @ref Magnum::CompressedPixelStorage
 */

#include <cstddef>
#include <utility>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum {

/**
@brief Pixel storage parameters

Descibes how to interpret data which are read from or stored into @ref Image,
@ref ImageView, @ref Trade::ImageData or for example @ref GL::BufferImage.

@see @ref CompressedPixelStorage
*/
class MAGNUM_EXPORT PixelStorage {
    public:
        /**
         * @brief Default constructor
         *
         * Sets all parameters to default values, i.e. all values set to
         * @cpp false @ce / @cpp 0 @ce except for alignment, which is @cpp 4 @ce.
         */
        constexpr /*implicit*/ PixelStorage() noexcept;

        /** @brief Equality comparison */
        bool operator==(const PixelStorage& other) const;

        /** @brief Non-equality comparison */
        bool operator!=(const PixelStorage& other) const {
            return !operator==(other);
        }

        /** @brief Row alignment */
        constexpr Int alignment() const { return _alignment; }

        /**
         * @brief Set row alignment
         *
         * Not applicable for @ref CompressedPixelStorage. Valid values are
         * @cpp 1 @ce, @cpp 2 @ce, @cpp 4 @ce and @cpp 8 @ce. Default is
         * @cpp 4 @ce.
         */
        PixelStorage& setAlignment(Int alignment) {
            _alignment = alignment;
            return *this;
        }

        /** @brief Row length */
        constexpr Int rowLength() const { return _rowLength; }

        /**
         * @brief Set row length
         *
         * Used only on 2D and 3D images. If set to @cpp 0 @ce, size
         * information from the actual image is used. Default is @cpp 0 @ce.
         */
        PixelStorage& setRowLength(Int length) {
            _rowLength = length;
            return *this;
        }

        /** @brief Image height */
        constexpr Int imageHeight() const { return _imageHeight; }

        /**
         * @brief Set image height
         *
         * Used only on 3D images. If set to @cpp 0 @ce, size information from
         * the actual image is used. Default is @cpp 0 @ce.
         */
        PixelStorage& setImageHeight(Int height) {
            _imageHeight = height;
            return *this;
        }

        /** @brief Pixel, row and image skip */
        constexpr Vector3i skip() const { return _skip; }

        /**
         * @brief Set pixel, row and image skip
         *
         * The Y value is used only for 2D and 3D images, the Z value is used
         * only for 3D images. Default is @cpp 0 @ce.
         */
        PixelStorage& setSkip(const Vector3i& skip) {
            _skip = skip;
            return *this;
        }

        /**
         * @brief Data properties for given parameters
         *
         * Returns byte offset in each direction and @cpp {rowLength, rowCount, layerCount} @ce
         * for image of given @p size with current pixel storage parameters,
         * and given @p pixelSize. The offset reflects the @ref skip()
         * parameter. Sum of the byte offset vector gives the byte offset of
         * first pixel in the data array.
         * @see @ref GL::pixelSize()
         */
        std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties(std::size_t pixelSize, const Vector3i& size) const;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        Int _rowLength,
            _imageHeight;
        Vector3i _skip;

    private:
        Int _alignment;
};

/**
@brief Compressed pixel storage parameters

Descibes how to interpret data which are read from or stored into
@ref CompressedImage, @ref CompressedImageView, @ref Trade::ImageData or for
example @ref GL::CompressedBufferImage.

Includes all parameters from @ref PixelStorage, except for @ref alignment(),
which is ignored for compressed images.
*/
class MAGNUM_EXPORT CompressedPixelStorage: public PixelStorage {
    public:
        /**
         * @brief Default constructor
         *
         * Sets all parameters to default values, i.e. all values set to
         * @cpp false @ce / @cpp 0 @ce except for alignment, which is @cpp 4 @ce.
         */
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* What am I doing wrong? */
        constexpr
        #endif
        /*implicit*/ CompressedPixelStorage() noexcept: _blockSize{0}, _blockDataSize{0} {}

        /** @brief Equality comparison */
        bool operator==(const CompressedPixelStorage& other) const;

        /** @brief Non-equality comparison */
        bool operator!=(const CompressedPixelStorage& other) const {
            return !operator==(other);
        }

        /** @brief Compressed block size */
        constexpr Vector3i compressedBlockSize() const { return _blockSize; }

        /**
         * @brief Set compressed block size
         *
         * If set to @cpp 0 @ce for given dimension, size information from
         * particular compressed format is used. Default is @cpp 0 @ce in all
         * dimensions.
         * @see @ref Magnum::compressedBlockSize()
         */
        CompressedPixelStorage& setCompressedBlockSize(const Vector3i& size) {
            _blockSize = size;
            return *this;
        }

        /** @brief Compressed block data size (in bytes) */
        constexpr Int compressedBlockDataSize() const { return _blockDataSize; }

        /**
         * @brief Set compressed block data size (in bytes)
         *
         * If set to @cpp 0 @ce, size information from particular compressed
         * format is used. Default is @cpp 0 @ce in all dimensions.
         * @see @ref Magnum::compressedBlockDataSize()
         */
        CompressedPixelStorage& setCompressedBlockDataSize(Int size) {
            _blockDataSize = size;
            return *this;
        }

        /**
         * @brief Data properties for given parameters
         *
         * Returns byte offset in each dimension, count of blocks in each
         * dimension and block data size for image of given @p size with
         * current pixel storage parameters. Adding byte offset and product of
         * the vector multiplied with block data size gives minimal byte count
         * to store given data.
         *
         * Expects @ref compressedBlockSize() and @ref compressedBlockDataSize()
         * to be non-zero.
         */
        std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties(const Vector3i& size) const;

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        CompressedPixelStorage& setRowLength(Int length) {
            PixelStorage::setRowLength(length);
            return *this;
        }
        CompressedPixelStorage& setImageHeight(Int height) {
            PixelStorage::setImageHeight(height);
            return *this;
        }
        CompressedPixelStorage& setSkip(const Vector3i& skip) {
            PixelStorage::setSkip(skip);
            return *this;
        }
        #endif

    private:
        using PixelStorage::alignment;
        using PixelStorage::setAlignment;

        Vector3i _blockSize;
        Int _blockDataSize;
};

constexpr PixelStorage::PixelStorage() noexcept: _rowLength{0}, _imageHeight{0}, _skip{0}, _alignment{4} {}

namespace Implementation {
    /* Used in templated image[view] constructors */
    template<class T> inline UnsignedInt pixelSizeAdl(T format) {
        return pixelSize(format);
    }

    template<class T, class U> inline UnsignedInt pixelSizeAdl(T format, U formatExtra) {
        return pixelSize(format, formatExtra);
    }

    /* Used in image query functions */
    template<std::size_t dimensions, class T> std::size_t imageDataSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
        std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties = image.storage().dataProperties(image.pixelSize(), Vector3i::pad(size, 1));

        /* Smallest line/rectangle/cube that covers the area */
        std::size_t dataOffset = 0;
        if(dataProperties.first.z())
            dataOffset += dataProperties.first.z();
        else if(dataProperties.first.y()) {
            if(!image.storage().imageHeight())
                dataOffset += dataProperties.first.y();
        } else if(dataProperties.first.x()) {
            if(!image.storage().rowLength())
                dataOffset += dataProperties.first.x();
        }
        return dataOffset + dataProperties.second.product();
    }

    /* Used in data size assertions */
    template<class T> inline std::size_t imageDataSize(const T& image) {
        return imageDataSizeFor(image, image.size());
    }

    template<std::size_t dimensions, class T> std::pair<std::size_t, std::size_t> compressedImageDataOffsetSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
        CORRADE_INTERNAL_ASSERT(image.storage().compressedBlockSize().product() && image.storage().compressedBlockDataSize());

        std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties = image.storage().dataProperties(Vector3i::pad(size, 1));

        const auto realBlockCount = Math::Vector3<std::size_t>{(Vector3i::pad(size, 1) + image.storage().compressedBlockSize() - Vector3i{1})/image.storage().compressedBlockSize()};

        return {dataProperties.first.sum(), (dataProperties.second.product() - (dataProperties.second.x() - realBlockCount.x()) - (dataProperties.second.y() - realBlockCount.y())*dataProperties.second.x())*image.storage().compressedBlockDataSize()};
    }

    /* Used in image query functions */
    template<std::size_t dimensions, class T> std::size_t compressedImageDataSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
        auto r = compressedImageDataOffsetSizeFor(image, size);
        return r.first + r.second;
    }

    /* Use in compressed image upload functions */
    template<class T> std::size_t occupiedCompressedImageDataSize(const T& image, std::size_t dataSize) {
        return image.storage().compressedBlockSize().product() && image.storage().compressedBlockDataSize()
            ? compressedImageDataOffsetSizeFor(image, image.size()).second : dataSize;
    }

    template<std::size_t dimensions, class T> std::ptrdiff_t pixelStorageSkipOffsetFor(const T& image, const Math::Vector<dimensions, Int>& size) {
        return image.storage().dataProperties(image.pixelSize(), Vector3i::pad(size, 1)).first.sum();
    }
    template<class T> std::ptrdiff_t pixelStorageSkipOffset(const T& image) {
        return pixelStorageSkipOffsetFor(image, image.size());
    }
}

}

#endif
