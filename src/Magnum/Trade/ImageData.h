#ifndef Magnum_Trade_ImageData_h
#define Magnum_Trade_ImageData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Image data

Used mainly by @ref AbstractImporter classes to store either compressed or
non-compressed multi-dimensional image data together with layout and pixel
format description.

This class can act as a drop-in replacement for @ref Image or
@ref CompressedImage, @ref ImageView or @ref CompressedImageView and is
implicitly convertible to either @ref ImageView or @ref CompressedImageView.
Particular graphics API wrappers provide additional image classes, for example
@ref GL::BufferImage or @ref GL::CompressedBufferImage.

@section Trade-ImageData-usage Basic usage

The image usually comes out of @ref AbstractImporter::image1D(),
@ref AbstractImporter::image2D() "image2D()" or
@ref AbstractImporter::image3D() "image3D()" and, based on what format the
particular imported data is in, it stores either compressed or uncompressed
data.

@snippet MagnumTrade.cpp ImageData-construction

@snippet MagnumTrade.cpp ImageData-construction-compressed

As with @ref Image / @ref ImageView, this class supports extra storage
parameters and implementation-specific format specification, if the importer
has a need for that. See the @ref ImageView documentation for more information.

When using the image, its compression status can be distinguished using
@ref isCompressed(). Uncompressed image properties are available through
@ref storage(), @ref format(), @ref formatExtra() and @ref pixelSize();
compressed properties through @ref compressedStorage() and
@ref compressedFormat(). Example of uploading the image to
@link GL::Texture @endlink:

@snippet MagnumTrade.cpp ImageData-usage

@see @ref ImageData1D, @ref ImageData2D, @ref ImageData3D
*/
template<UnsignedInt dimensions> class ImageData {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Construct uncompressed image data
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         *
         * The @p data array is expected to be of proper size for given
         * parameters.
         */
        explicit ImageData(PixelStorage storage, PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct uncompressed image data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         *
         * Equivalent to calling @ref ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, const void*)
         * with default-constructed @ref PixelStorage.
         */
        explicit ImageData(PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept: ImageData{{}, format, size, std::move(data), importerState} {}

        /**
         * @brief Construct uncompressed image data with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param pixelSize         Size of a pixel in given format
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         *
         * Unlike with @ref ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, const void*),
         * where pixel size is calculated automatically using
         * @ref pixelSize(PixelFormat), this allows you to specify an
         * implementation-specific pixel format and pixel size directly. Uses
         * @ref pixelFormatWrap() internally to wrap @p format in
         * @ref Magnum::PixelFormat "PixelFormat".
         *
         * The @p data array is expected to be of proper size for given
         * parameters.
         */
        explicit ImageData(PixelStorage storage, UnsignedInt format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;

        /** @overload
         *
         * Equivalent to the above for @p format already wrapped with
         * @ref pixelFormatWrap().
         */
        explicit ImageData(PixelStorage storage, PixelFormat format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct uncompressed image data with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         *
         * Uses ADL to find a corresponding @cpp pixelSize(T, U) @ce overload,
         * then calls @ref ImageData(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, const void*)
         * with calculated pixel size.
         */
        template<class T, class U> explicit ImageData(PixelStorage storage, T format, U formatExtra, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct uncompressed image data with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         *
         * Uses ADL to find a corresponding @cpp pixelSize(T) @ce overload,
         * then calls @ref ImageData(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, const void*)
         * with calculated pixel size and @p formatExtra set to @cpp 0 @ce.
         */
        template<class T> explicit ImageData(PixelStorage storage, T format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct compressed image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         */
        explicit ImageData(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct compressed image data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         *
         * Equivalent to calling @ref ImageData(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, const void*)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit ImageData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept: ImageData{{}, format, size, std::move(data), importerState} {}

        /**
         * @brief Construct compressed image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         *
         * Uses @ref compressedPixelFormatWrap() internally to convert
         * @p format to @ref CompressedPixelFormat.
         */
        template<class T> explicit ImageData(CompressedPixelStorage storage, T format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct from existing data with attached importer state
         *
         * Useful in cases where importer plugins proxy image loading through
         * other importers but want to attach its own importer state to the
         * imported data. Importer state from the @p other object is replaced
         * with @p importerState, data ownership is transferred and everything
         * else stays the same.
         */
        explicit ImageData(ImageData<dimensions>&& other, const void* importerState) noexcept;

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
        /* Not restricted to const&, because we might want to pass the view to
           another function in an oneliner (e.g. saving screenshot) */
        /*implicit*/ operator ImageView<dimensions>() const;

        /**
         * @brief Conversion to compressed view
         *
         * The image is expected to be compressed.
         * @see @ref isCompressed()
         */
        /* Not restricted to const&, because we might want to pass the view to
           another function in an oneliner (e.g. saving screenshot) */
        /*implicit*/ operator CompressedImageView<dimensions>() const;

        /**
         * @brief Storage of pixel data
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed(), @ref compressedStorage()
         */
        PixelStorage storage() const;

        /**
         * @brief Format of pixel data
         *
         * Returns either a defined value from the
         * @ref Magnum::PixelFormat "PixelFormat" enum or a wrapped
         * implementation-specific value. Use
         * @ref isPixelFormatImplementationSpecific() to distinguish the case
         * and @ref pixelFormatUnwrap() to extract an implementation-specific
         * value, if needed.
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed(), @ref compressedFormat()
         */
        PixelFormat format() const;

        /**
         * @brief Additional pixel format specifier
         *
         * Some implementations (such as OpenGL) define a pixel format using
         * two values. This field contains the second implementation-specific
         * value verbatim, if any. See @ref format() for more information.
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed()
         */
        UnsignedInt formatExtra() const;

        /**
         * @brief Storage of compressed pixel data
         *
         * The image is expected to be compressed.
         * @see @ref isCompressed(), @ref storage()
         */
        CompressedPixelStorage compressedStorage() const;

        /**
         * @brief Format of compressed pixel data
         *
         * Returns either a defined value from the @ref CompressedPixelFormat
         * enum or a wrapped implementation-specific value. Use
         * @ref isCompressedPixelFormatImplementationSpecific() to distinguish
         * the case and @ref compressedPixelFormatUnwrap() to extract an
         * implementation-specific value, if needed.
         *
         * The image is expected to be compressed.
         * @see @ref isCompressed(), @ref format()
         */
        CompressedPixelFormat compressedFormat() const;

        /**
         * @brief Pixel size (in bytes)
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed(), @ref Magnum::pixelSize()
         */
        UnsignedInt pixelSize() const;

        /** @brief Image size */
        VectorTypeFor<dimensions, Int> size() const { return _size; }

        /**
         * @brief Uncompressed image data properties
         *
         * The image is expected to be uncompressed. See
         * @ref PixelStorage::dataProperties() for more information.
         * @see @ref isCompressed()
         */
        std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> dataProperties() const;

        /* compressed data properties are not available because the importers
           are not setting any block size pixel storage properties to avoid
           needless state changes -- thus the calculation can't be done */

        /**
         * @brief Raw data
         *
         * @see @ref release()
         */
        Containers::ArrayView<char> data() & { return _data; }
        Containers::ArrayView<char> data() && = delete; /**< @overload */

        /** @overload */
        Containers::ArrayView<const char> data() const & { return _data; }
        Containers::ArrayView<const char> data() const && = delete; /**< @overload */

        /** @overload */
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
         * Releases the ownership of the data array and resets internal state
         * to default.
         * @see @ref data()
         */
        Containers::Array<char> release();

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        explicit ImageData(CompressedPixelStorage storage, UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;

        bool _compressed;
        union {
            PixelStorage _storage;
            CompressedPixelStorage _compressedStorage;
        };
        union {
            PixelFormat _format;
            CompressedPixelFormat _compressedFormat;
        };
        UnsignedInt _formatExtra;
        UnsignedInt _pixelSize;
        Math::Vector<Dimensions, Int> _size;
        Containers::Array<char> _data;
        const void* _importerState;
};

/** @brief One-dimensional image */
typedef ImageData<1> ImageData1D;

/** @brief Two-dimensional image */
typedef ImageData<2> ImageData2D;

/** @brief Three-dimensional image */
typedef ImageData<3> ImageData3D;

template<UnsignedInt dimensions> template<class T, class U> ImageData<dimensions>::ImageData(const PixelStorage storage, const T format, const U formatExtra, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: ImageData{storage, UnsignedInt(format), UnsignedInt(formatExtra), Magnum::Implementation::pixelSizeAdl(format, formatExtra), size, std::move(data), importerState} {
    static_assert(sizeof(T) <= 4 && sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions> template<class T> ImageData<dimensions>::ImageData(const PixelStorage storage, const T format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: ImageData{storage, UnsignedInt(format), {}, Magnum::Implementation::pixelSizeAdl(format), size, std::move(data), importerState} {
    static_assert(sizeof(T) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions> template<class T> ImageData<dimensions>::ImageData(const CompressedPixelStorage storage, const T format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: ImageData{storage, UnsignedInt(format), size, std::move(data), importerState} {
    static_assert(sizeof(T) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions> inline ImageData<dimensions>::ImageData(ImageData<dimensions>&& other) noexcept: _compressed{std::move(other._compressed)}, _size{std::move(other._size)}, _data{std::move(other._data)}, _importerState{std::move(other._importerState)} {
    if(_compressed) {
        new(&_compressedStorage) CompressedPixelStorage{std::move(other._compressedStorage)};
        _compressedFormat = std::move(other._compressedFormat);
    }
    else {
        new(&_storage) PixelStorage{std::move(other._storage)};
        _format = std::move(other._format);
        _formatExtra = std::move(other._formatExtra);
        _pixelSize = std::move(other._pixelSize);
    }

    other._size = {};
}

template<UnsignedInt dimensions> inline ImageData<dimensions>& ImageData<dimensions>::operator=(ImageData<dimensions>&& other) noexcept {
    using std::swap;
    swap(_compressed, other._compressed);
    if(_compressed) {
        swap(_compressedStorage, other._compressedStorage);
        swap(_compressedFormat, other._compressedFormat);
    }
    else {
        swap(_storage, other._storage);
        swap(_format, other._format);
    }
    swap(_formatExtra, other._formatExtra);
    swap(_pixelSize, other._pixelSize);
    swap(_size, other._size);
    swap(_data, other._data);
    swap(_importerState, other._importerState);
    return *this;
}

template<UnsignedInt dimensions> inline Containers::Array<char> ImageData<dimensions>::release() {
    Containers::Array<char> data{std::move(_data)};
    _size = {};
    return data;
}

}}

#endif
