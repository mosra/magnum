#ifndef Magnum_PixelStorage_h
#define Magnum_PixelStorage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
 * @brief Class @ref Magnum::PixelStorage, @ref Magnum::CompressedPixelStorage
 */

#include <cstddef>
#include <tuple>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector3.h"

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
#include "Magnum/GL/PixelFormat.h"
#endif

namespace Magnum {

/**
@brief Pixel storage parameters

Descibes how to interpret data which are read from or stored into @ref Image,
@ref ImageView, @ref Trade::ImageData or for example @ref GL::BufferImage.

@see @ref CompressedPixelStorage
*/
class MAGNUM_EXPORT PixelStorage {
    public:
        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
        /** @brief @copybrief GL::pixelSize()
         * @deprecated Use @ref GL::pixelSize() instead.
         */
        static CORRADE_DEPRECATED("use GL::pixelSize() instead") std::size_t pixelSize(GL::PixelFormat format, GL::PixelType type);
        #endif

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

        /** @brief Pixel, row and image skipping */
        constexpr Vector3i skip() const { return _skip; }

        /**
         * @brief Set pixel, row and image skipping
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

        #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
        /** @brief @copybrief dataProperties(std::size_t, const Vector3i&) const
         *
         * Returns byte offset in each direction, (row length, row count, layer
         * count) and pixel size for image of given @p size with current pixel
         * storage parameters, @p format and @p type.
         * @deprecated Use @ref dataProperties(std::size_t, const Vector3i&) const
         *      instead.
         */
        CORRADE_DEPRECATED("use dataProperties(std::size_t, const Vector3i&) instead") std::tuple<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>, std::size_t> dataProperties(GL::PixelFormat format, GL::PixelType type, const Vector3i& size) const;
        #endif

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
        std::tuple<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>, std::size_t> dataProperties(const Vector3i& size) const;

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

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
/* These have to be in the header because GL::pixelSize() is in Magnum::GL and
   putting that in the source would mean undefined reference */
inline std::size_t PixelStorage::pixelSize(const GL::PixelFormat format, const GL::PixelType type) {
    return GL::pixelSize(format, type);
}

inline std::tuple<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>, std::size_t> PixelStorage::dataProperties(const GL::PixelFormat format, const GL::PixelType type, const Vector3i& size) const {
    const std::size_t pixelSize = GL::pixelSize(format, type);
    Math::Vector3<std::size_t> offset, dataSize;
    std::tie(offset, dataSize) = dataProperties(pixelSize, size);
    return std::make_tuple(offset, dataSize, pixelSize);
}
#endif

namespace Implementation {
    /* Used in *Image::dataProperties() */
    template<std::size_t dimensions, class T> std::tuple<Math::Vector<dimensions, std::size_t>, Math::Vector<dimensions, std::size_t>, std::size_t> imageDataProperties(const T& image) {
        Math::Vector3<std::size_t> offset, dataSize;
        std::size_t pixelSize;
        std::tie(offset, dataSize, pixelSize) = image.storage().dataProperties(image.format(), image.type(), Vector3i::pad(image.size(), 1));
        return std::make_tuple(Math::Vector<dimensions, std::size_t>::pad(offset), Math::Vector<dimensions, std::size_t>::pad(dataSize), pixelSize);
    }

    /* Used in Compressed*Image::dataProperties() */
    template<std::size_t dimensions, class T> std::tuple<Math::Vector<dimensions, std::size_t>, Math::Vector<dimensions, std::size_t>, std::size_t> compressedImageDataProperties(const T& image) {
        Math::Vector3<std::size_t> offset, blockCount;
        std::size_t blockSize;
        std::tie(offset, blockCount, blockSize) = image.storage().dataProperties(Vector3i::pad(image.size(), 1));
        return std::make_tuple(Math::Vector<dimensions, std::size_t>::pad(offset), Math::Vector<dimensions, std::size_t>::pad(blockCount), blockSize);
    }

    /* Used in image query functions */
    template<std::size_t dimensions, class T> std::size_t imageDataSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
        Math::Vector3<std::size_t> offset, dataSize;
        std::size_t pixelSize;
        std::tie(offset, dataSize, pixelSize) = image.storage().dataProperties(image.format(), image.type(), Vector3i::pad(size, 1));

        /* Smallest line/rectangle/cube that covers the area */
        std::size_t dataOffset = 0;
        if(offset.z())
            dataOffset += offset.z();
        else if(offset.y()) {
            if(!image.storage().imageHeight())
                dataOffset += offset.y();
        } else if(offset.x()) {
            if(!image.storage().rowLength())
                dataOffset += offset.x();
        }
        return dataOffset + dataSize.product();
    }

    /* Used in data size assertions */
    template<class T> inline std::size_t imageDataSize(const T& image) {
        return imageDataSizeFor(image, image.size());
    }

    template<std::size_t dimensions, class T> std::pair<std::size_t, std::size_t> compressedImageDataOffsetSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
        CORRADE_INTERNAL_ASSERT(image.storage().compressedBlockSize().product() && image.storage().compressedBlockDataSize());

        Math::Vector3<std::size_t> offset, blockCount;
        std::size_t blockDataSize;
        std::tie(offset, blockCount, blockDataSize) = image.storage().dataProperties(Vector3i::pad(size, 1));

        const auto realBlockCount = Math::Vector3<std::size_t>{(Vector3i::pad(size, 1) + image.storage().compressedBlockSize() - Vector3i{1})/image.storage().compressedBlockSize()};

        return {offset.sum(), (blockCount.product() - (blockCount.x() - realBlockCount.x()) - (blockCount.y() - realBlockCount.y())*blockCount.x())*blockDataSize};
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
        return std::get<0>(image.storage().dataProperties(image.format(), image.type(), Vector3i::pad(size, 1))).sum();
    }
    template<class T> std::ptrdiff_t pixelStorageSkipOffset(const T& image) {
        return pixelStorageSkipOffsetFor(image, image.size());
    }
}

}

#endif
