#ifndef Magnum_Trade_ImageData_h
#define Magnum_Trade_ImageData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "Magnum/DimensionTraits.h"
#include "Magnum/ImageFlags.h"
#include "Magnum/PixelStorage.h"
#include "Magnum/Trade/Data.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Corrade { namespace Containers {

/* Forward declaration of an utility used in pixels() to avoid forcing users to
   include the relatively large StridedArrayView header *before* the Image
   class definition. */
template<unsigned newDimensions, class U, unsigned dimensions, class T> StridedArrayView<newDimensions, U> arrayCast(const StridedArrayView<dimensions, T>& view);

}}
#endif

namespace Magnum { namespace Trade {

/**
@brief Image data

Provides access to both uncompressed and compressed image data together with
information about data layout, image size and pixel format. Populated instances
of this class are returned from @ref AbstractImporter::image1D(),
@relativeref{AbstractImporter,image2D()},
@relativeref{AbstractImporter,image3D()}, can be passed to
@ref AbstractImageConverter::convert(),
@ref AbstractSceneConverter::add(const ImageData2D&, Containers::StringView)
and related APIs, as well as used in various @ref TextureTools algorithms. Like
with other @ref Trade types, the internal representation is fixed upon
construction and allows only optional in-place modification of the data itself,
but not of the overall structure.

This class can act as a drop-in replacement for @ref Image or
@ref CompressedImage, @ref ImageView or @ref CompressedImageView and is
implicitly convertible to either @ref ImageView or @ref CompressedImageView.
Particular graphics API wrappers provide additional image classes, for example
@ref GL::BufferImage or @ref GL::CompressedBufferImage.

@section Trade-ImageData-usage Basic usage

Based on whether the @ref ImageData has an uncompressed or compressed pixel
format, it behaves either like an @ref Image / @ref ImageView or like a
@ref CompressedImage / @ref CompressedImageView. It can be distinguished using
@ref isCompressed(); uncompressed image properties are then available through
@ref storage(), @ref format(), @ref formatExtra() and @ref pixelSize(),
compressed properties through @ref compressedStorage() and
@ref compressedFormat(). Example of uploading the image to a
@link GL::Texture @endlink:

@snippet Trade.cpp ImageData-usage

Uncompressed image data instances provide pixel data access via @ref pixels()
in the same way as the @ref Image class. See @ref Image-pixel-access "its documentation for more information".

@section Trade-ImageData-usage-mutable Mutable data access

The interfaces implicitly provide @cpp const @ce views on the contained
pixel data through the @ref data() and @ref pixels() accessors. This is done
because in general case the data can also refer to a memory-mapped file or
constant memory. In cases when it's desirable to modify the data in-place,
there's the @ref mutableData() and @ref mutablePixels() set of functions. To
use these, you need to check that the data are mutable using @ref dataFlags()
first. The following snippet flips the R and B channels of the imported image:

@snippet Trade.cpp ImageData-usage-mutable

@section Trade-ImageData-populating Populating an instance

An @ref ImageData instance by default takes over the ownership of an
@relativeref{Corrade,Containers::Array} containing the pixel data together
with size and either @ref PixelFormat or @ref CompressedPixelFormat, similarly
to the @ref Image and @ref CompressedImage classes:

@snippet Trade.cpp ImageData-populating

The constructor internally checks that the passed array is large enough and as
with other image classes, care must be taken in presence of
non-four-byte-aligned rows. This often closely depends on behavior of the code
or library that operates with the image data and the recommended way is to pad
the row data to satisfy the alignment:

@snippet Trade.cpp ImageData-populating-padding

Alternatively, if padding is not possible or desirable, you can pass a
@ref PixelStorage instance with the alignment overriden to @cpp 1 @ce:

@snippet Trade.cpp ImageData-populating-alignment

As with @ref Image / @ref ImageView, this class supports extra storage
parameters and implementation-specific format specification, if the importer
has a need for that. See the @ref ImageView documentation for more information.

@subsection Trade-ImageData-populating-non-owned Non-owned instances

In some cases you may want the @ref ImageData instance to only refer to
external data without taking ownership, for example with a memory-mapped file,
global data etc. For that, instead of moving in an
@relativeref{Corrade,Containers::Array}, pass @ref DataFlags describing data
mutability and ownership together with an
@relativeref{Corrade,Containers::ArrayView}:

@snippet Trade.cpp ImageData-populating-non-owned

@see @ref ImageData1D, @ref ImageData2D, @ref ImageData3D
*/
template<UnsignedInt dimensions> class ImageData {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Construct an uncompressed image data
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * The @p data array is expected to be of proper size for given
         * parameters. For a 3D image, if @p flags contain
         * @ref ImageFlag3D::CubeMap, the @p size is expected to match its
         * restrictions.
         *
         * The @ref dataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable. For non-owned data
         * use the @ref ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * constructor instead.
         *
         * The @p format is expected to not be implementation-specific, use the
         * @ref ImageData(PixelStorage, PixelFormat, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * overload to explicitly pass an implementation-specific
         * @ref PixelFormat along with a pixel size, or the
         * @ref ImageData(PixelStorage, T, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * overload with the original implementation-specific enum type to have
         * the pixel size determined implicitly.
         */
        explicit ImageData(PixelStorage storage, PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelStorage storage, PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState) noexcept: ImageData{storage, format, size, Utility::move(data), {}, importerState} {}
        #endif

        /**
         * @brief Construct a non-owned uncompressed image data
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param dataFlags         Data flags
         * @param data              View on image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Compared to @ref ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p dataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set.
         */
        explicit ImageData(PixelStorage storage, PixelFormat format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelStorage storage, PixelFormat format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, const void* importerState) noexcept: ImageData{storage, format, size, dataFlags, data, {}, importerState} {}
        #endif

        /**
         * @brief Construct an uncompressed image data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Equivalent to calling @ref ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * with default-constructed @ref PixelStorage.
         */
        explicit ImageData(PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState) noexcept: ImageData{format, size, Utility::move(data), {}, importerState} {}
        #endif

        /**
         * @brief Construct a non-owned uncompressed image data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param dataFlags         Data flags
         * @param data              View on image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Equivalent to calling @ref ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * with default-constructed @ref PixelStorage.
         */
        explicit ImageData(PixelFormat format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(PixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(PixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelFormat format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, const void* importerState) noexcept: ImageData{format, size, dataFlags, data, {}, importerState} {}
        #endif

        /**
         * @brief Construct an uncompressed image data with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param pixelSize         Size of a pixel in given format, in bytes
         * @param size              Image size, in pixels
         * @param data              Image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Unlike with @ref ImageData(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*),
         * where pixel size is determined automatically using
         * @ref pixelFormatSize(), this allows you to specify an
         * implementation-specific pixel format and pixel size directly. Uses
         * @ref pixelFormatWrap() internally to wrap @p format in
         * @ref Magnum::PixelFormat "PixelFormat". The @p pixelSize is expected
         * to be non-zero and less than @cpp 256 @ce.
         *
         * The @p data array is expected to be of proper size for given
         * parameters. For a 3D image, if @p flags contain
         * @ref ImageFlag3D::CubeMap, the @p size is expected to match its
         * restrictions. The @ref dataFlags() are implicitly set to a
         * combination of @ref DataFlag::Owned and @ref DataFlag::Mutable. For
         * non-owned data use the @ref ImageData(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * constructor instead.
         */
        explicit ImageData(PixelStorage storage, UnsignedInt format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        /** @overload
         * @m_since_latest
         *
         * Equivalent to the above for @p format already wrapped with
         * @ref pixelFormatWrap().
         */
        explicit ImageData(PixelStorage storage, PixelFormat format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelStorage storage, UnsignedInt format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState) noexcept: ImageData{storage, format, formatExtra, pixelSize, size, Utility::move(data), {}, importerState} {}

        /**
         * @overload
         * @m_deprecated_since_latest Use @ref ImageData(PixelStorage, PixelFormat, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelStorage storage, PixelFormat format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState) noexcept: ImageData{storage, format, formatExtra, pixelSize, size, Utility::move(data), {}, importerState} {}
        #endif

        /**
         * @brief Construct a non-owned uncompressed image data with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param pixelSize         Size of a pixel in given format, in bytes
         * @param size              Image size, in pixels
         * @param dataFlags         Data flags
         * @param data              View on image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Compared to @ref ImageData(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p dataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set.
         */
        explicit ImageData(PixelStorage storage, UnsignedInt format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        /** @overload
         * @m_since_latest
         *
         * Equivalent to the above for @p format already wrapped with
         * @ref pixelFormatWrap().
         */
        explicit ImageData(PixelStorage storage, PixelFormat format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelStorage storage, UnsignedInt format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, const void* importerState) noexcept: ImageData{storage, format, formatExtra, pixelSize, size, dataFlags, data, {}, importerState} {}

        /**
         * @overload
         * @m_deprecated_since_latest Use @ref ImageData(PixelStorage, PixelFormat, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelStorage storage, PixelFormat format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, const void* importerState) noexcept: ImageData{storage, format, formatExtra, pixelSize, size, dataFlags, data, {}, importerState} {}
        #endif

        /**
         * @brief Construct an uncompressed image data with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Uses ADL to find a corresponding @cpp pixelFormatSize(T, U) @ce
         * overload, then calls @ref ImageData(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * with determined pixel size.
         */
        template<class T, class U> explicit ImageData(PixelStorage storage, T format, U formatExtra, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(PixelStorage, T, U, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(PixelStorage, T, U, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        template<class T, class U> explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelStorage storage, T format, U formatExtra, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState) noexcept: ImageData{storage, format, formatExtra, size, Utility::move(data), {}, importerState} {}
        #endif

        /**
         * @brief Construct a non-owned uncompressed image data with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         * @param dataFlags         Data flags
         * @param data              View on image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Compared to @ref ImageData(PixelStorage, T, U, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p dataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set.
         */
        template<class T, class U> explicit ImageData(PixelStorage storage, T format, U formatExtra, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(PixelStorage, T, U, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(PixelStorage, T, U, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        template<class T, class U> explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelStorage storage, T format, U formatExtra, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, const void* importerState) noexcept: ImageData{storage, format, formatExtra, size, dataFlags, data, {}, importerState} {}
        #endif

        /**
         * @brief Construct an uncompressed image data with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Uses ADL to find a corresponding @cpp pixelFormatSize(T) @ce
         * overload, then calls @ref ImageData(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * with determined pixel size and @p formatExtra set to @cpp 0 @ce.
         */
        template<class T> explicit ImageData(PixelStorage storage, T format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(PixelStorage, T, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(PixelStorage, T, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        template<class T> explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelStorage storage, T format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState) noexcept: ImageData{storage, format, size, Utility::move(data), {}, importerState} {}
        #endif

        /**
         * @brief Construct a non-owned uncompressed image data with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param dataFlags         Data flags
         * @param data              view on image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Compared to @ref ImageData(PixelStorage, T, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p dataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set.
         */
        template<class T> explicit ImageData(PixelStorage storage, T format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(PixelStorage, T, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(PixelStorage, T, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        template<class T> explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(PixelStorage storage, T format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, const void* importerState) noexcept: ImageData{storage, format, size, dataFlags, data, {}, importerState} {}
        #endif

        /**
         * @brief Construct a compressed image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         */
        explicit ImageData(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState) noexcept: ImageData{storage, format, size, Utility::move(data), {}, importerState} {}
        #endif

        /**
         * @brief Construct a non-owned compressed image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param dataFlags         Data flags
         * @param data              View on image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Compared to @ref ImageData(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p dataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set.
         */
        explicit ImageData(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, const void* importerState) noexcept: ImageData{storage, format, size, dataFlags, data, {}, importerState} {}
        #endif

        /**
         * @brief Construct a compressed image data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Equivalent to calling @ref ImageData(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit ImageData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState) noexcept: ImageData{format, size, Utility::move(data), {}, importerState} {}
        #endif

        /**
         * @brief Construct a non-owned compressed image data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param dataFlags         Data flags
         * @param data              View on image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Equivalent to calling @ref ImageData(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit ImageData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, const void* importerState) noexcept: ImageData{format, size, dataFlags, data, {}, importerState} {}
        #endif

        /**
         * @brief Construct a compressed image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Uses @ref compressedPixelFormatWrap() internally to convert
         * @p format to @ref CompressedPixelFormat.
         *
         * For a 3D image, if @p flags contain @ref ImageFlag3D::CubeMap, the
         * @p size is expected to match its restrictions.
         */
        template<class T> explicit ImageData(CompressedPixelStorage storage, T format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(CompressedPixelStorage, T, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(CompressedPixelStorage, T, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        template<class T> explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(CompressedPixelStorage storage, T format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* importerState) noexcept: ImageData{storage, format, size, Utility::move(data), {}, importerState} {}
        #endif

        /**
         * @brief Construct a non-owned compressed image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param dataFlags         Data flags
         * @param data              View on image data
         * @param flags             Image layout flags
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * Compared to @ref ImageData(CompressedPixelStorage, T, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&, ImageFlags<dimensions>, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p dataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set.
         */
        template<class T> explicit ImageData(CompressedPixelStorage storage, T format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, ImageFlags<dimensions> flags = {}, const void* importerState = nullptr) noexcept;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief ImageData(CompressedPixelStorage, T, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         * @m_deprecated_since_latest Use @ref ImageData(CompressedPixelStorage, T, const VectorTypeFor<dimensions, Int>&, DataFlags, Containers::ArrayView<const void>, ImageFlags<dimensions>, const void*)
         *      instead.
         */
        template<class T> explicit CORRADE_DEPRECATED("use a constructor with an extra ImageFlags argument instead") ImageData(CompressedPixelStorage storage, T format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, const void* importerState) noexcept: ImageData{storage, format, size, dataFlags, data, {}, importerState} {}
        #endif

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

        /**
         * @brief Data flags
         * @m_since{2020,06}
         */
        DataFlags dataFlags() const { return _dataFlags; }

        /** @brief Whether the image is compressed */
        bool isCompressed() const { return _compressed; }

        /**
         * @brief Conversion to a view
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed()
         */
        /* Not restricted to const&, because we might want to pass the view to
           another function in an oneliner (e.g. saving screenshot) */
        /*implicit*/ operator BasicImageView<dimensions>() const;

        /**
         * @brief Conversion to a mutable view
         * @m_since{2019,10}
         *
         * The image is expected to be uncompressed and mutable.
         * @see @ref isCompressed(), @ref dataFlags()
         */
        /*implicit*/ operator BasicMutableImageView<dimensions>();

        /**
         * @brief Conversion to a compressed view
         *
         * The image is expected to be compressed.
         * @see @ref isCompressed()
         */
        /* Not restricted to const&, because we might want to pass the view to
           another function in an oneliner (e.g. saving screenshot) */
        /*implicit*/ operator BasicCompressedImageView<dimensions>() const;

        /**
         * @brief Conversion to a mutable compressed view
         * @m_since{2019,10}
         *
         * The image is expected to be compressed and mutable.
         * @see @ref isCompressed(), @ref dataFlags()
         */
        /*implicit*/ operator BasicMutableCompressedImageView<dimensions>();

        /**
         * @brief Layout flags
         * @m_since_latest
         */
        ImageFlags<dimensions> flags() const { return _flags; }

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
         * @brief Size of a pixel in bytes
         *
         * The image is expected to be uncompressed.
         * @see @ref isCompressed(), @ref pixelFormatSize()
         */
        UnsignedInt pixelSize() const;

        /** @brief Image size in pixels */
        /* Unlike other getters this one is a const& so it's possible to slice
           to the sizes when all images are in an array, for example for use
           in TextureTools atlas APIs */
        const VectorTypeFor<dimensions, Int>& size() const { return _size; }

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
         * @brief Raw image data
         *
         * @see @ref release(), @ref pixels()
         */
        Containers::ArrayView<const char> data() const & { return _data; }

        /**
         * @brief Image data from a r-value
         * @m_since{2019,10}
         *
         * Unlike @ref data(), which returns a view, this is equivalent to
         * @ref release() to avoid a dangling view when the temporary instance
         * goes out of scope. Note that the returned array has a custom no-op
         * deleter when the data are not owned by the image, and while the
         * returned array type is mutable, the actual memory might be not.
         * @todoc stupid doxygen can't link to & overloads ffs
         */
        Containers::Array<char> data() && { return release(); }

        /** @overload
         * @m_since{2019,10}
         * @todo what to do here?!
         */
        Containers::Array<char> data() const && = delete;

        /**
         * @brief Mutable image data
         * @m_since{2020,06}
         *
         * Like @ref data(), but returns a non-const view. Expects that the
         * image is mutable.
         * @see @ref dataFlags()
         */
        Containers::ArrayView<char> mutableData() &;

        /**
         * @brief Pixel data
         * @m_since{2019,10}
         *
         * Provides direct and easy-to-use access to image pixels. Expects that
         * the image is not compressed. The last dimension represents the
         * actual data type (its size is equal to type size) and is guaranteed
         * to be contiguous. Use the templated overload below to get pixels in
         * a concrete type. See @ref Image-pixel-access for more information.
         * @see @ref isCompressed(),
         *      @ref Corrade::Containers::StridedArrayView::isContiguous()
         */
        Containers::StridedArrayView<dimensions + 1, const char> pixels() const;

        /**
         * @brief Mutable pixel data
         * @m_since{2020,06}
         *
         * Like @ref pixels() const, but returns a non-const view. Expects that
         * the image is mutable. See also @ref Image-pixel-access for more
         * information.
         * @see @ref dataFlags()
         */
        Containers::StridedArrayView<dimensions + 1, char> mutablePixels();

        /**
         * @brief View on pixel data with a concrete pixel type
         * @m_since{2019,10}
         *
         * Compared to non-templated @ref pixels() in addition casts the pixel
         * data to a specified type. The user is responsible for choosing
         * correct type for given @ref format() --- checking it on the library
         * side is not possible for the general case. See also
         * @ref Image-pixel-access for more information.
         */
        template<class T> Containers::StridedArrayView<dimensions, const T> pixels() const {
            /* Deliberately not adding a StridedArrayView include, it should
               work without since this is a templated function and we declare
               arrayCast() above to satisfy two-phase lookup. */
            return Containers::arrayCast<dimensions, const T>(pixels());
        }

        /**
         * @brief Mutable view on pixel data with a concrete pixel type
         * @m_since{2019,10}
         *
         * Like @ref pixels() const, but returns a non-const view. Expects that
         * the image is mutable.
         * @see @ref dataFlags()
         */
        template<class T> Containers::StridedArrayView<dimensions, T> mutablePixels() {
            /* Deliberately not adding a StridedArrayView include, it should
               work without since this is a templated function */
            return Containers::arrayCast<dimensions, T>(mutablePixels());
        }

        /**
         * @brief Release data storage
         *
         * Releases the ownership of the data array and resets internal state
         * to default. The image then behaves like it's empty. Note that
         * the returned array has a custom no-op deleter when the data are not
         * owned by the image, and while the returned array type is mutable,
         * the actual memory might be not.
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
        /* For custom deleter checks. Not done in the constructors here because
           the restriction is pointless when used outside of plugin
           implementations. */
        friend AbstractImporter;
        friend AbstractImageConverter;

        explicit ImageData(CompressedPixelStorage storage, UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, ImageFlags<dimensions> flags, const void* importerState = nullptr) noexcept;

        explicit ImageData(CompressedPixelStorage storage, UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, DataFlags dataFlags, Containers::ArrayView<const void> data, ImageFlags<dimensions> flags, const void* importerState = nullptr) noexcept;

        DataFlags _dataFlags;
        bool _compressed;
        ImageFlags<dimensions> _flags;
        union {
            PixelStorage _storage;
            CompressedPixelStorage _compressedStorage;
        };
        union {
            PixelFormat _format;
            CompressedPixelFormat _compressedFormat;
        };
        UnsignedInt _formatExtra;
        UnsignedByte _pixelSize;
        /* 3 bytes free */
        VectorTypeFor<dimensions, Int> _size;
        Containers::Array<char> _data;
        const void* _importerState;
};

/** @brief One-dimensional image data */
typedef ImageData<1> ImageData1D;

/** @brief Two-dimensional image data */
typedef ImageData<2> ImageData2D;

/** @brief Three-dimensional image data */
typedef ImageData<3> ImageData3D;

template<UnsignedInt dimensions> template<class T, class U> ImageData<dimensions>::ImageData(const PixelStorage storage, const T format, const U formatExtra, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const ImageFlags<dimensions> flags, const void* const importerState) noexcept: ImageData{storage, UnsignedInt(format), UnsignedInt(formatExtra), pixelFormatSize(format, formatExtra), size, Utility::move(data), flags, importerState} {
    static_assert(sizeof(T) <= 4 && sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions> template<class T, class U> ImageData<dimensions>::ImageData(const PixelStorage storage, const T format, const U formatExtra, const VectorTypeFor<dimensions, Int>& size, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const ImageFlags<dimensions> flags, const void* const importerState) noexcept: ImageData{storage, UnsignedInt(format), UnsignedInt(formatExtra), pixelFormatSize(format, formatExtra), size, dataFlags, data, flags, importerState} {
    static_assert(sizeof(T) <= 4 && sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions> template<class T> ImageData<dimensions>::ImageData(const PixelStorage storage, const T format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const ImageFlags<dimensions> flags, const void* const importerState) noexcept: ImageData{storage, UnsignedInt(format), {}, pixelFormatSize(format), size, Utility::move(data), flags, importerState} {
    static_assert(sizeof(T) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions> template<class T> ImageData<dimensions>::ImageData(const PixelStorage storage, const T format, const VectorTypeFor<dimensions, Int>& size, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const ImageFlags<dimensions> flags, const void* const importerState) noexcept: ImageData{storage, UnsignedInt(format), {}, pixelFormatSize(format), size, dataFlags, data, flags, importerState} {
    static_assert(sizeof(T) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions> template<class T> ImageData<dimensions>::ImageData(const CompressedPixelStorage storage, const T format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const ImageFlags<dimensions> flags, const void* const importerState) noexcept: ImageData{storage, UnsignedInt(format), size, Utility::move(data), flags, importerState} {
    static_assert(sizeof(T) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions> template<class T> ImageData<dimensions>::ImageData(const CompressedPixelStorage storage, const T format, const VectorTypeFor<dimensions, Int>& size, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const ImageFlags<dimensions> flags, const void* const importerState) noexcept: ImageData{storage, UnsignedInt(format), size, dataFlags, data, flags, importerState} {
    static_assert(sizeof(T) <= 4,
        "format types larger than 32bits are not supported");
}

}}

#endif
