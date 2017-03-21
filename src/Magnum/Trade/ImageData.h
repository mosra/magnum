#ifndef Magnum_Trade_ImageData_h
#define Magnum_Trade_ImageData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
@ref pixelSize() and @ref dataProperties() properties and are convertible to
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
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         *
         * The data are expected to be of proper size for given @p storage
         * parameters.
         */
        explicit ImageData(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        explicit ImageData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept: ImageData{{}, format, type, size, std::move(data), importerState} {}

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief ImageData(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, const void*)
         * @deprecated Use @ref ImageData(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, const void*) instead.
         */
        explicit CORRADE_DEPRECATED("use ImageData(PixelFormat, PixelType, const VectorTypeFor&, Containers::Array&&) instead") ImageData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, void* data) noexcept: ImageData{format, type, size, Containers::Array<char>{reinterpret_cast<char*>(data), Magnum::Implementation::imageDataSizeFor(format, type, size)}} {}
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* To avoid decay of nullptr to const void* and unwanted use of
           deprecated function */
        explicit ImageData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, std::nullptr_t) noexcept: ImageData{{}, format, type, size, nullptr} {}
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Construct compressed image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        explicit ImageData(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;
        #endif

        /**
         * @brief Construct compressed image data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param importerState     Importer-specific state
         *
         * Similar the above, but uses default @ref CompressedPixelStorage
         * parameters (or the hardcoded ones in OpenGL ES and WebGL).
         */
        explicit ImageData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState = nullptr) noexcept;

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
         * The image is expected to be uncompressed.
         * @see @ref isCompressed(), @ref compressedFormat()
         */
        PixelFormat format() const;

        /**
         * @brief Data type of pixel data
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed()
         */
        PixelType type() const;

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Storage of compressed pixel data
         *
         * The image is expected to be compressed.
         * @see @ref isCompressed(), @ref storage())
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        CompressedPixelStorage compressedStorage() const;
        #endif

        /**
         * @brief Format of compressed pixel data
         *
         * The image is expected to be compressed.
         * @see @ref isCompressed(), @ref format()
         */
        CompressedPixelFormat compressedFormat() const;

        /**
         * @brief Pixel size (in bytes)
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed(), @ref PixelStorage::pixelSize()
         */
        std::size_t pixelSize() const;

        /** @brief Image size */
        VectorTypeFor<dimensions, Int> size() const { return _size; }

        /**
         * @brief Uncompressed image data properties
         *
         * The image is expected to be uncompressed. See
         * @ref PixelStorage::dataProperties() for more information.
         * @see @ref isCompressed()
         */
        std::tuple<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>, std::size_t> dataProperties() const;

        /* compressed data properties are not available because the importers
           are not setting any block size pixel storage properties to avoid
           needless state changes -- thus the calculation can't be done */

        /**
         * @brief Raw data
         *
         * @see @ref release()
         */
        Containers::ArrayView<char> data()
            #ifndef CORRADE_GCC47_COMPATIBILITY
            &
            #endif
            { return _data; }
        #ifndef CORRADE_GCC47_COMPATIBILITY
        Containers::ArrayView<char> data() && = delete; /**< @overload */
        #endif

        /** @overload */
        Containers::ArrayView<const char> data() const
            #ifndef CORRADE_GCC47_COMPATIBILITY
            &
            #endif
            { return _data; }
        #ifndef CORRADE_GCC47_COMPATIBILITY
        Containers::ArrayView<const char> data() const && = delete; /**< @overload */
        #endif

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
        bool _compressed;
        union {
            PixelStorage _storage;
            #ifndef MAGNUM_TARGET_GLES
            CompressedPixelStorage _compressedStorage;
            #endif
        };
        union {
            PixelFormat _format;
            CompressedPixelFormat _compressedFormat;
        };
        PixelType _type;
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

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(
    #ifndef MAGNUM_TARGET_GLES
    const CompressedPixelStorage storage,
    #endif
    const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState) noexcept: _compressed{true},
    #ifndef MAGNUM_TARGET_GLES
    _compressedStorage{storage},
    #endif
    _compressedFormat{format}, _size{size}, _data{std::move(data)}, _importerState{importerState} {}

#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> inline ImageData<dimensions>::ImageData(const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: ImageData{{}, format, size, std::move(data), importerState} {}
#endif

template<UnsignedInt dimensions> inline ImageData<dimensions>::ImageData(ImageData<dimensions>&& other) noexcept: _compressed{std::move(other._compressed)}, _size{std::move(other._size)}, _data{std::move(other._data)}, _importerState{std::move(other._importerState)} {
    if(_compressed) {
        #ifndef MAGNUM_TARGET_GLES
        new(&_compressedStorage) CompressedPixelStorage{std::move(other._compressedStorage)};
        #endif
        _compressedFormat = std::move(other._compressedFormat);
    }
    else {
        new(&_storage) PixelStorage{std::move(other._storage)};
        _format = std::move(other._format);
        _type = std::move(other._type);
    }

    other._size = {};
}

template<UnsignedInt dimensions> inline ImageData<dimensions>& ImageData<dimensions>::operator=(ImageData<dimensions>&& other) noexcept {
    using std::swap;
    swap(_compressed, other._compressed);
    if(_compressed) {
        #ifndef MAGNUM_TARGET_GLES
        swap(_compressedStorage, other._compressedStorage);
        #endif
        swap(_compressedFormat, other._compressedFormat);
    }
    else {
        swap(_storage, other._storage);
        swap(_format, other._format);
    }
    swap(_type, other._type);
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
