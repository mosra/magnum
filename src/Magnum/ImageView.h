#ifndef Magnum_ImageView_h
#define Magnum_ImageView_h
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
 * @brief Class @ref Magnum::ImageView, @ref Magnum::CompressedImageView, alias @ref Magnum::BasicImageView, @ref Magnum::BasicCompressedImageView, typedef @ref Magnum::ImageView1D, @ref Magnum::ImageView2D, @ref Magnum::ImageView3D, @ref Magnum::MutableImageView1D, @ref Magnum::MutableImageView2D, @ref Magnum::MutableImageView3D, @ref Magnum::CompressedImageView1D, @ref Magnum::CompressedImageView2D, @ref Magnum::CompressedImageView3D, @ref Magnum::MutableCompressedImageView1D, @ref Magnum::MutableCompressedImageView2D, @ref Magnum::MutableCompressedImageView3D
 */

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/ImageFlags.h"
#include "Magnum/PixelStorage.h"
#include "Magnum/Math/Vector3.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Corrade { namespace Containers {

/* Forward declaration of an utility used in pixels() to avoid forcing users to
   include the relatively large StridedArrayView header *before* the Image
   class definition. */
template<unsigned newDimensions, class U, unsigned dimensions, class T> StridedArrayView<newDimensions, U> arrayCast(const StridedArrayView<dimensions, T>& view);

}}
#endif

namespace Magnum {

/**
@brief Image view

Non-owning view on multi-dimensional image data together with layout and pixel
format description. Unlike @ref Image, this class doesn't take ownership of the
data, so it is targeted for wrapping data that is either stored in
stack/constant memory (and shouldn't be deleted) or is managed by something
else.

This class can act as drop-in replacement for @ref Image or
@ref Trade::ImageData, these two are additionally implicitly convertible to it.
Particular graphics API wrappers provide additional image classes, for example
@ref GL::BufferImage. See also @ref CompressedImageView for equivalent
functionality targeted on compressed image formats.

@section ImageView-usage Basic usage

The view is created from a @ref PixelFormat, size in pixels and a data view:

@snippet Magnum.cpp ImageView-usage

The constructor internally checks that the passed array is large enough. For
performance reasons it by default expects rows aligned to four bytes, which you
need to account for if using odd image sizes in combination with one-, two- or
three-component formats. While the recommended way is to pad the row data to
satisfy the alignment similarly as shown in @ref Image-usage "Image usage docs",
but with views it's more likely that you have to adapt to a layout of an
existing data array by passing a @ref PixelStorage instance with the alignment
value overriden if needed:

@snippet Magnum.cpp ImageView-usage-alignment

It's also possible to create an empty view and assign the memory later. That is
useful for example in case of multi-buffered video streaming, where each frame
has the same properties but a different memory location:

@snippet Magnum.cpp ImageView-usage-streaming

The @ref PixelStorage also allows for specifying arbitrary image slices by
passing appropriate row length, image height and skip parameters. In the
following snippet, the view is the center 25x25 sub-rectangle of a 75x75 8-bit
RGB image (with tightly packed rows again):

@snippet Magnum.cpp ImageView-usage-storage

Image views provides pixel data access via @ref pixels() in the same way as the
@ref Image class. See @ref Image-pixel-access "its documentation for more information".

@section ImageView-mutable Data mutability

The @ref ImageView2D type and related one- and three-dimensional variants only
provide immutable access to the referenced data, as that's the most common use
case. In order to be able to modify the data (for example in order to read into
a pre-allocated memory), use @ref MutableImageView2D and friends instead.
@ref Image and @ref Trade::ImageData are convertible to either of these.
Similarly to @ref Corrade::Containers::ArrayView etc., a mutable view is also
implicitly convertible to a const one.

@section ImageView-usage-implementation-specific Implementation-specific formats

For known graphics APIs, there's a set of utility functions converting from
@ref PixelFormat to implementation-specific format identifiers and such
conversion is done implicitly when passing the view to a particular API. See
the enum documentation and documentation of its values for more information.

In some cases, for example when there's no corresponding generic format
available, it's desirable to specify the pixel format using
implementation-specific identifiers directly. In case of OpenGL that would be
the @ref GL::PixelFormat and @ref GL::PixelType pair:

@snippet Magnum.cpp ImageView-usage-gl

In such cases, pixel size is determined using either
@cpp pixelFormatSize(T, U) @ce or @cpp pixelFormatSize(T) @ce that is found
using [ADL](https://en.wikipedia.org/wiki/Argument-dependent_name_lookup), with
@cpp T @ce and @cpp U @ce corresponding to types of passed arguments. The
implementation-specific format is wrapped in @ref PixelFormat using
@ref pixelFormatWrap() and @ref format() returns the wrapped value. In order to
distinguish if the format is wrapped, use @ref isPixelFormatImplementationSpecific()
and then extract the implementation-specific identifier using @ref pixelFormatUnwrap().
For APIs that have an additional format specifier (such as OpenGL), the second
value is stored verbatim in @ref formatExtra():

@snippet Magnum.cpp ImageView-usage-gl-extract

As a final fallback, types for which the @cpp pixelFormatSize() @ce overload is
not available can be specified directly together with pixel size. In
particular, pixel size of @cpp 0 @ce will cause the image to be treated as
fully opaque data, disabling all slicing operations. The following shows a
image view using Metal-specific format identifier:

@snippet Magnum.cpp ImageView-usage-metal

@see @ref BasicImageView, @ref ImageView1D, @ref ImageView2D, @ref ImageView3D,
    @ref BasicMutableImageView, @ref MutableImageView1D,
    @ref MutableImageView2D, @ref MutableImageView3D
*/
template<UnsignedInt dimensions, class T> class ImageView {
    public:
        /* Pointer arithmetic relies on the type being a single byte */
        static_assert(std::is_same<T, char>::value ||std::is_same<T, const char>::value,
            "image view type can be either char or const char");

        /**
         * @brief Raw data type
         *
         * @cpp const char @ce for @ref BasicImageView and @cpp char @ce for
         * @ref BasicMutableImageView. See also @ref ErasedType.
         */
        typedef T Type;

        /**
         * @brief Erased data type
         *
         * @cpp const void @ce for @ref BasicImageView and @cpp const void @ce
         * for @ref BasicMutableImageView. See also @ref Type.
         */
        typedef typename std::conditional<std::is_const<T>::value, const void, void>::type ErasedType;

        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Constructor
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * The @p data array is expected to be of proper size for given
         * parameters. For a 3D image, if @p flags contain
         * @ref ImageFlag3D::CubeMap, the @p size is expected to match its
         * restrictions.
         *
         * The @p format is expected to not be implementation-specific, use the
         * @ref ImageView(PixelStorage, PixelFormat, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>)
         * overload to explicitly pass an implementation-specific
         * @ref PixelFormat along with a pixel size, or the
         * @ref ImageView(PixelStorage, U, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>)
         * overload with the original implementation-specific enum type to have
         * the pixel size determined implicitly.
         */
        explicit ImageView(PixelStorage storage, PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * Equivalent to calling @ref ImageView(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>)
         * with default-constructed @ref PixelStorage.
         */
        explicit ImageView(PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept: ImageView{{}, format, size, data, flags} {}

        /**
         * @brief Construct an empty view
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param flags             Image layout flags
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image. For a 3D image, if @p flags
         * contain @ref ImageFlag3D::CubeMap, the @p size is expected to match
         * its restrictions.
         *
         * The @p format is expected to not be implementation-specific, use the
         * @ref ImageView(PixelStorage, PixelFormat, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, ImageFlags<dimensions>)
         * overload to explicitly pass an implementation-specific
         * @ref PixelFormat along with a pixel size, or the
         * @ref ImageView(PixelStorage, U, const VectorTypeFor<dimensions, Int>&, ImageFlags<dimensions>)
         * overload with the original implementation-specific enum type to have
         * the pixel size determined implicitly.
         */
        explicit ImageView(PixelStorage storage, PixelFormat format, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Construct an empty view
         * @param format            Format of pixel data
         * @param size              Image size
         * @param flags             Image layout flags
         *
         * Equivalent to calling @ref ImageView(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, ImageFlags<dimensions>)
         * with default-constructed @ref PixelStorage.
         */
        explicit ImageView(PixelFormat format, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept: ImageView{{}, format, size, flags} {}

        /**
         * @brief Construct an image view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param pixelSize         Size of a pixel in given format, in bytes
         * @param size              Image size, in pixels
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * Unlike with @ref ImageView(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>),
         * where pixel size is determined automatically using
         * @ref pixelFormatSize(), this allows you to specify an
         * implementation-specific pixel format and pixel size directly. Uses
         * @ref pixelFormatWrap() internally to wrap @p format in
         * @ref PixelFormat. The @p pixelSize is expected to be non-zero and
         * less than @cpp 256 @ce.
         *
         * The @p data array is expected to be of proper size for given
         * parameters. For a 3D image, if @p flags contain
         * @ref ImageFlag3D::CubeMap, the @p size is expected to match its
         * restrictions.
         */
        explicit ImageView(PixelStorage storage, UnsignedInt format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept;

        /** @overload
         *
         * Equivalent to the above for @p format already wrapped with
         * @ref pixelFormatWrap().
         */
        explicit ImageView(PixelStorage storage, PixelFormat format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Construct an empty view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param pixelSize         Size of a pixel in given format, in bytes
         * @param size              Image size, in pixels
         * @param flags             Image layout flags
         *
         * Unlike with @ref ImageView(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, ImageFlags<dimensions>),
         * where pixel size is determined automatically using
         * @ref pixelFormatSize(), this allows you to specify an
         * implementation-specific pixel format and pixel size directly. Uses
         * @ref pixelFormatWrap() internally to wrap @p format in
         * @ref PixelFormat. The @p pixelSize is expected to be non-zero and
         * less than @cpp 256 @ce.
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image. For a 3D image, if @p flags
         * contain @ref ImageFlag3D::CubeMap, the @p size is expected to match
         * its restrictions.
         */
        explicit ImageView(PixelStorage storage, UnsignedInt format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept;

        /** @overload
         *
         * Equivalent to the above for @p format already wrapped with
         * @ref pixelFormatWrap().
         */
        explicit ImageView(PixelStorage storage, PixelFormat format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Construct an image view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * Uses ADL to find a corresponding @cpp pixelFormatSize(U, V) @ce
         * overload, then calls @ref ImageView(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>)
         * with determined pixel size.
         */
        template<class U, class V> explicit ImageView(PixelStorage storage, U format, V formatExtra, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Construct an image view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * Uses ADL to find a corresponding @cpp pixelFormatSize(U) @ce
         * overload, then calls @ref ImageView(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>)
         * with determined pixel size and @p formatExtra set to @cpp 0 @ce.
         */
        template<class U> explicit ImageView(PixelStorage storage, U format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Construct an image view with implementation-specific pixel format
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * Equivalent to calling @ref ImageView(PixelStorage, U, V, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>)
         * with default-constructed @ref PixelStorage.
         */
        template<class U, class V> explicit ImageView(U format, V formatExtra, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept: ImageView{{}, format, formatExtra, size, data, flags} {}

        /**
         * @brief Construct an image view with implementation-specific pixel format
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * Equivalent to calling @ref ImageView(PixelStorage, U, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>)
         * with default-constructed @ref PixelStorage.
         */
        template<class U> explicit ImageView(U format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept: ImageView{{}, format, size, data, flags} {}

        /**
         * @brief Construct an empty view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         * @param flags             Image layout flags
         *
         * Uses ADL to find a corresponding @cpp pixelFormatSize(U, V) @ce
         * overload, then calls @ref ImageView(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, ImageFlags<dimensions>)
         * with determined pixel size.
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image.
         */
        template<class U, class V> explicit ImageView(PixelStorage storage, U format, V formatExtra, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Construct an empty view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param flags             Image layout flags
         *
         * Uses ADL to find a corresponding @cpp pixelFormatSize(U) @ce
         * overload, then calls @ref ImageView(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, ImageFlags<dimensions>)
         * with determined pixel size and @p formatExtra set to @cpp 0 @ce.
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image.
         */
        template<class U> explicit ImageView(PixelStorage storage, U format, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Construct an empty view with implementation-specific pixel format
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         * @param flags             Image layout flags
         *
         * Equivalent to calling @ref ImageView(PixelStorage, U, V, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>)
         * with default-constructed @ref PixelStorage.
         */
        template<class U, class V> explicit ImageView(U format, V formatExtra, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept: ImageView{{}, format, formatExtra, size, flags} {}

        /**
         * @brief Construct an empty view with implementation-specific pixel format
         * @param format            Format of pixel data
         * @param size              Image size
         * @param flags             Image layout flags
         *
         * Equivalent to calling @ref ImageView(PixelStorage, U, const VectorTypeFor<dimensions, Int>&, ImageFlags<dimensions>)
         * with default-constructed @ref PixelStorage.
         */
        template<class U> explicit ImageView(U format, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept: ImageView{{}, format, size, flags} {}

        /**
         * @brief Construct from a view of lower dimension count
         * @m_since{2019,10}
         *
         * Size in the new dimension(s) is set to @cpp 1 @ce. Original image
         * flags are preserved, except for @ref ImageFlag2D::Array when
         * constructing a 3D image from a 2D image (i.e., a 1D array image), as
         * there's no concept of 2D arrays of 1D images. Use the @p flags
         * parameter to add arbitrary other flags.
         */
        template<UnsignedInt otherDimensions
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<(otherDimensions < dimensions), int>::type = 0
            #endif
        > /*implicit*/ ImageView(const ImageView<otherDimensions, T>& other, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Convert a mutable view to a const one
         * @m_since{2019,10}
         */
        template<class U
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_const<T>::value && !std::is_const<U>::value, int>::type = 0
            #endif
        > /*implicit*/ ImageView(const ImageView<dimensions, U>& other) noexcept;

        /**
         * @brief Layout flags
         * @m_since_latest
         */
        ImageFlags<dimensions> flags() const { return _flags; }

        /** @brief Storage of pixel data */
        PixelStorage storage() const { return _storage; }

        /**
         * @brief Format of pixel data
         *
         * Returns either a defined value from the @ref PixelFormat enum or a
         * wrapped implementation-specific value. Use
         * @ref isPixelFormatImplementationSpecific() to distinguish the case
         * and @ref pixelFormatUnwrap() to extract an implementation-specific
         * value, if needed.
         * @see @ref formatExtra()
         */
        PixelFormat format() const { return _format; }

        /**
         * @brief Additional pixel format specifier
         *
         * Some implementations (such as OpenGL) define a pixel format using
         * two values. This field contains the second implementation-specific
         * value verbatim, if any. See @ref format() for more information.
         */
        UnsignedInt formatExtra() const { return _formatExtra; }

        /**
         * @brief Size of a pixel in bytes
         *
         * @see @ref pixelFormatSize()
         */
        UnsignedInt pixelSize() const { return _pixelSize; }

        /** @brief Image size in pixels */
        /* Unlike other getters this one is a const& so it's possible to slice
           to the sizes when all images are in an array, for example for use
           in TextureTools atlas APIs */
        const VectorTypeFor<dimensions, Int>& size() const { return _size; }

        /**
         * @brief Image data properties
         *
         * See @ref PixelStorage::dataProperties() for more information.
         */
        std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> dataProperties() const;

        /**
         * @brief Raw image data
         *
         * @see @ref pixels()
         */
        Containers::ArrayView<Type> data() const { return _data; }

        /**
         * @brief Set image data
         *
         * The data array is expected to be of proper size for parameters
         * specified in the constructor.
         */
        void setData(Containers::ArrayView<ErasedType> data);

        /**
         * @brief Pixel data
         * @m_since{2019,10}
         *
         * Provides direct and easy-to-use access to image pixels. See
         * @ref Image-pixel-access for more information. If the view is empty
         * (with @ref data() being @cpp nullptr @ce), returns @cpp nullptr @ce
         * as well.
         */
        Containers::StridedArrayView<dimensions + 1, Type> pixels() const;

        /**
         * @brief Pixel data in a concrete type
         * @m_since{2019,10}
         *
         * Compared to non-templated @ref pixels() in addition casts the pixel
         * data to a specified type. The user is responsible for choosing
         * correct type for given @ref format() --- checking it on the library
         * side is not possible for the general case. If the view is empty
         * (with @ref data() being @cpp nullptr @ce), returns @cpp nullptr @ce
         * as well. See also @ref Image-pixel-access for more information.
         */
        template<class U> Containers::StridedArrayView<dimensions, typename std::conditional<std::is_const<Type>::value, typename std::add_const<U>::type, U>::type> pixels() const {
            if(!_data && !_data.size()) return {};
            /* Deliberately not adding a StridedArrayView include, it should
               work without since this is a templated function and we declare
               arrayCast() above to satisfy two-phase lookup. */
            return Containers::arrayCast<dimensions, typename std::conditional<std::is_const<Type>::value, typename std::add_const<U>::type, U>::type>(pixels());
        }

    private:
        /* Needed for mutable->const conversion */
        template<UnsignedInt, class> friend class ImageView;

        PixelStorage _storage;
        PixelFormat _format;
        UnsignedInt _formatExtra;
        UnsignedByte _pixelSize;
        /* 1 byte free */
        ImageFlags<dimensions> _flags;
        VectorTypeFor<dimensions, Int> _size;
        Containers::ArrayView<Type> _data;
};

/**
@brief Const image view
@m_since{2019,10}

@see @ref ImageView1D, @ref ImageView2D, @ref ImageView3D,
    @ref BasicMutableImageView
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions> using BasicImageView = ImageView<dimensions, const char>;
#endif

/**
@brief One-dimensional image view

@see @ref MutableImageView1D, @ref CompressedImageView1D, @ref ImageView
*/
typedef BasicImageView<1> ImageView1D;

/**
@brief Two-dimensional image view

@see @ref MutableImageView2D, @ref CompressedImageView2D, @ref ImageView
*/
typedef BasicImageView<2> ImageView2D;

/**
@brief Three-dimensional image view

@see @ref MutableImageView3D, @ref CompressedImageView3D, @ref ImageView
*/
typedef BasicImageView<3> ImageView3D;

/**
@brief Mutable image view
@m_since{2019,10}

@see @ref MutableImageView1D, @ref MutableImageView2D, @ref MutableImageView3D,
    @ref BasicImageView
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions> using BasicMutableImageView = ImageView<dimensions, char>;
#endif

/**
@brief One-dimensional mutable image view
@m_since{2019,10}

@see @ref ImageView1D, @ref MutableCompressedImageView1D, @ref ImageView
*/
typedef BasicMutableImageView<1> MutableImageView1D;

/**
@brief Two-dimensional mutable image view
@m_since{2019,10}

@see @ref ImageView2D, @ref MutableCompressedImageView2D, @ref ImageView
*/
typedef BasicMutableImageView<2> MutableImageView2D;

/**
@brief Three-dimensional mutable image view
@m_since{2019,10}

@see @ref ImageView3D, @ref MutableCompressedImageView3D, @ref ImageView
*/
typedef BasicMutableImageView<3> MutableImageView3D;

/**
@brief Compressed image view

Non-owning view on multi-dimensional compressed image data together with layout
and compressed block format description. Unlike @ref CompressedImage, this
class doesn't take ownership of the data, so it is targeted for wrapping data
that is either stored in stack/constant memory (and shouldn't be deleted) or is
managed by something else.

This class can act as drop-in replacement for @ref CompressedImage or
@ref Trade::ImageData, these two are additionally implicitly convertible to it.
Particular graphics API wrappers provide additional image classes, for example
@ref GL::CompressedBufferImage. See also @ref ImageView for equivalent
functionality targeted on non-compressed image formats.

@section CompressedImageView-usage Basic usage

The view is created from a @ref CompressedPixelFormat, size in pixels and a
data view:

@snippet Magnum.cpp CompressedImageView-usage

It's also possible to create an empty view and assign the memory later. That is
useful for example in case of multi-buffered video streaming, where each frame
has the same properties but a different memory location:

@snippet Magnum.cpp CompressedImageView-usage-streaming

It's possible to have views on image sub-rectangles, 3D texture slices or
images with over-aligned rows by passing a particular @ref CompressedPixelStorage
as first parameter. In the following snippet, the view is the bottom-right
32x32 sub-rectangle of a 64x64 image:

@snippet Magnum.cpp CompressedImageView-usage-storage

@section CompressedImageView-mutable Data mutability

When using types derived from @ref BasicCompressedImageView (e.g.
@ref CompressedImageView2D), the viewed data are immutable. This is the most
common use case. In order to be able to mutate the underlying data (for example
in order to read into a pre-allocated memory), use
@ref BasicMutableCompressedImageView (e.g. @ref MutableCompressedImageView2D)
instead. @ref CompressedImage and @ref Trade::ImageData are convertible to
either of these. Similarly to @ref Corrade::Containers::ArrayView etc., a
mutable view is also implicitly convertible to a const one.

@subsection CompressedImageView-usage-implementation-specific Implementation-specific formats

For known graphics APIs, there's a set of utility functions converting from
@ref CompressedPixelFormat to implementation-specific format identifiers and
such conversion is done implicitly when passing the view to a particular API.
See the enum documentation and documentation of its values for more
information.

In some cases, for example when there's no corresponding generic format
available, it's desirable to specify the pixel format using
implementation-specific identifiers directly. In case of OpenGL that would be
@link GL::CompressedPixelFormat @endlink:

@snippet Magnum.cpp CompressedImageView-usage-gl

In such cases, the implementation-specific format is wrapped in
@ref CompressedPixelFormat using @ref compressedPixelFormatWrap() and
@ref format() returns the wrapped value. In order to distinguish if the format
is wrapped, use @ref isCompressedPixelFormatImplementationSpecific() and then
extract the implementation-specific identifier using
@ref compressedPixelFormatUnwrap():

@snippet Magnum.cpp CompressedImageView-usage-gl-extract

@see @ref CompressedImageView1D, @ref CompressedImageView2D,
    @ref CompressedImageView3D, @ref MutableCompressedImageView1D,
    @ref MutableCompressedImageView2D, @ref MutableCompressedImageView3D
*/
template<UnsignedInt dimensions, class T> class CompressedImageView {
    public:
        /* Pointer arithmetic relies on the type being a single byte */
        static_assert(std::is_same<T, char>::value ||std::is_same<T, const char>::value,
            "image view type can be either char or const char");

        /**
         * @brief Raw data type
         *
         * @cpp const char @ce for @ref CompressedImageView1D /
         * @ref CompressedImageView2D / @ref CompressedImageView3D and
         * @cpp char @ce for @ref MutableCompressedImageView1D /
         * @ref MutableCompressedImageView2D /
         * @ref MutableCompressedImageView3D. See also @ref ErasedType.
         */
        typedef T Type;

        /**
         * @brief Erased data type
         *
         * @cpp const void @ce for @ref CompressedImageView1D /
         * @ref CompressedImageView2D / @ref CompressedImageView3D and
         * @cpp const void @ce for @ref MutableCompressedImageView1D /
         * @ref MutableCompressedImageView2D / @ref MutableCompressedImageView3D.
         * See also @ref Type.
         */
        typedef typename std::conditional<std::is_const<T>::value, const void, void>::type ErasedType;

        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Constructor
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * For a 3D image, if @p flags contain @ref ImageFlag3D::CubeMap, the
         * @p size is expected to match its restrictions.
         */
        explicit CompressedImageView(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Constructor
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * Equivalent to calling @ref CompressedImageView(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit CompressedImageView(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept: CompressedImageView{{}, format, size, data, flags} {}

        /**
         * @brief Construct an empty view
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param flags             Image layout flags
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image. For a 3D image, if @p flags
         * contain @ref ImageFlag3D::CubeMap, the @p size is expected to match
         * its restrictions.
         */
        explicit CompressedImageView(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Construct an empty view
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param flags             Image layout flags
         *
         * Equivalent to calling @ref CompressedImageView(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, ImageFlags<dimensions>)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit CompressedImageView(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept: CompressedImageView{{}, format, size, flags} {}

        /**
         * @brief Construct an image view with implementation-specific format
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * Uses @ref compressedPixelFormatWrap() internally to convert
         * @p format to @ref CompressedPixelFormat.
         *
         * For a 3D image, if @p flags contain @ref ImageFlag3D::CubeMap, the
         * @p size is expected to match its restrictions.
         */
        template<class U> explicit CompressedImageView(CompressedPixelStorage storage, U format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Construct an image view with implementation-specific format
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * Equivalent to calling @ref CompressedImageView(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>, ImageFlags<dimensions>)
         * with default-constructed @ref CompressedPixelStorage.
         */
        template<class U> explicit CompressedImageView(U format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags = {}) noexcept: CompressedImageView{{}, format, size, data, flags} {}

        /**
         * @brief Construct an empty view with implementation-specific format
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param flags             Image layout flags
         *
         * Uses @ref compressedPixelFormatWrap() internally to convert
         * @p format to @ref CompressedPixelFormat.
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image. For a 3D image, if @p flags
         * contain @ref ImageFlag3D::CubeMap, the @p size is expected to match
         * its restrictions.
         */
        template<class U> explicit CompressedImageView(CompressedPixelStorage storage, U format, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Construct an empty view with implementation-specific format
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param flags             Image layout flags
         *
         * Equivalent to calling @ref CompressedImageView(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, ImageFlags<dimensions>)
         * with default-constructed @ref CompressedPixelStorage.
         */
        template<class U> explicit CompressedImageView(U format, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags = {}) noexcept: CompressedImageView{{}, format, size, flags} {}

        /**
         * @brief Construct from a view of lower dimension count
         * @m_since{2019,10}
         *
         * Size in the new dimension(s) is set to @cpp 1 @ce. Original image
         * flags are preserved, except for @ref ImageFlag2D::Array when
         * constructing a 3D image from a 2D image (i.e., a 1D array image), as
         * there's no concept of 2D arrays of 1D images. Use the @p flags
         * parameter to add arbitrary other flags.
         */
        template<UnsignedInt otherDimensions
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<(otherDimensions < dimensions), int>::type = 0
            #endif
        > /*implicit*/ CompressedImageView(const CompressedImageView<otherDimensions, T>& other, ImageFlags<dimensions> flags = {}) noexcept;

        /**
         * @brief Convert a mutable view to a const one
         * @m_since{2019,10}
         */
        template<class U
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_const<T>::value && !std::is_const<U>::value, int>::type = 0
            #endif
        > /*implicit*/ CompressedImageView(const CompressedImageView<dimensions, U>& other) noexcept;

        /**
         * @brief Layout flags
         * @m_since_latest
         */
        ImageFlags<dimensions> flags() const { return _flags; }

        /** @brief Storage of compressed pixel data */
        CompressedPixelStorage storage() const { return _storage; }

        /**
         * @brief Format of compressed pixel data
         *
         * Returns either a defined value from the @ref CompressedPixelFormat
         * enum or a wrapped implementation-specific value. Use
         * @ref isCompressedPixelFormatImplementationSpecific() to distinguish
         * the case and @ref compressedPixelFormatUnwrap() to extract an
         * implementation-specific value, if needed.
         */
        CompressedPixelFormat format() const { return _format; }

        /** @brief Image size in pixels */
        /* Unlike other getters this one is a const& so it's possible to slice
           to the sizes when all images are in an array, for example for use
           in TextureTools atlas APIs */
        const VectorTypeFor<dimensions, Int>& size() const { return _size; }

        /**
         * @brief Compressed image data properties
         *
         * See @ref CompressedPixelStorage::dataProperties() for more
         * information.
         */
        std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> dataProperties() const;

        /** @brief Raw image data */
        Containers::ArrayView<Type> data() const { return _data; }

        /**
         * @brief Set image data
         *
         * The data array is expected to be of proper size for parameters
         * specified in the constructor.
         */
        void setData(Containers::ArrayView<ErasedType> data) {
            _data = {reinterpret_cast<Type*>(data.data()), data.size()};
        }

    private:
        /* Needed for mutable->const conversion */
        template<UnsignedInt, class> friend class CompressedImageView;

        /* To be made public once block size and block data size are stored
           together with the image */
        explicit CompressedImageView(CompressedPixelStorage storage, UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data, ImageFlags<dimensions> flags) noexcept;
        explicit CompressedImageView(CompressedPixelStorage storage, UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, ImageFlags<dimensions> flags) noexcept;

        CompressedPixelStorage _storage;
        CompressedPixelFormat _format;
        ImageFlags<dimensions> _flags;
        VectorTypeFor<dimensions, Int> _size;
        Containers::ArrayView<Type> _data;
};

/**
@brief Const compressed image view
@m_since{2019,10}

@see @ref CompressedImageView1D, @ref CompressedImageView2D,
    @ref CompressedImageView3D, @ref BasicMutableImageView
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions> using BasicCompressedImageView = CompressedImageView<dimensions, const char>;
#endif

/**
@brief One-dimensional compressed image view

@see @ref MutableCompressedImageView1D, @ref ImageView1D,
    @ref CompressedImageView
*/
typedef BasicCompressedImageView<1> CompressedImageView1D;

/**
@brief Two-dimensional compressed image view

@see @ref MutableCompressedImageView2D, @ref ImageView2D,
    @ref CompressedImageView
*/
typedef BasicCompressedImageView<2> CompressedImageView2D;

/**
@brief Three-dimensional compressed image view

@see @ref MutableCompressedImageView3D, @ref ImageView3D,
    @ref CompressedImageView
*/
typedef BasicCompressedImageView<3> CompressedImageView3D;

/**
@brief Mutable compressed image view
@m_since{2019,10}

@see @ref MutableCompressedImageView1D, @ref MutableCompressedImageView2D,
    @ref MutableCompressedImageView3D, @ref BasicCompressedImageView
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions> using BasicMutableCompressedImageView = CompressedImageView<dimensions, char>;
#endif

/**
@brief One-dimensional mutable compressed image view
@m_since{2019,10}

@see @ref CompressedImageView1D, @ref MutableImageView1D,
    @ref CompressedImageView
*/
typedef BasicMutableCompressedImageView<1> MutableCompressedImageView1D;

/**
@brief Two-dimensional mutable compressed image view
@m_since{2019,10}

@see @ref CompressedImageView2D, @ref MutableImageView2D,
    @ref CompressedImageView
*/
typedef BasicMutableCompressedImageView<2> MutableCompressedImageView2D;

/**
@brief Three-dimensional mutable compressed image view
@m_since{2019,10}

@see @ref CompressedImageView3D, @ref MutableImageView3D,
    @ref CompressedImageView
*/
typedef BasicMutableCompressedImageView<3> MutableCompressedImageView3D;

template<UnsignedInt dimensions, class T> template<class U, class V> inline ImageView<dimensions, T>::ImageView(const PixelStorage storage, const U format, const V formatExtra, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data, const ImageFlags<dimensions> flags) noexcept: ImageView{storage, UnsignedInt(format), UnsignedInt(formatExtra), pixelFormatSize(format, formatExtra), size, data, flags} {
    static_assert(sizeof(T) <= 4 && sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions, class T> template<class U> inline ImageView<dimensions, T>::ImageView(const PixelStorage storage, const U format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data, const ImageFlags<dimensions> flags) noexcept: ImageView{storage, UnsignedInt(format), {}, pixelFormatSize(format), size, data, flags} {
    static_assert(sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions, class T> template<class U, class V> inline ImageView<dimensions, T>::ImageView(const PixelStorage storage, const U format, const V formatExtra, const VectorTypeFor<dimensions, Int>& size, const ImageFlags<dimensions> flags) noexcept: ImageView{storage, UnsignedInt(format), UnsignedInt(formatExtra), pixelFormatSize(format, formatExtra), size, flags} {
    static_assert(sizeof(U) <= 4 && sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions, class T> template<class U> inline ImageView<dimensions, T>::ImageView(const PixelStorage storage, const U format, const VectorTypeFor<dimensions, Int>& size, const ImageFlags<dimensions> flags) noexcept: ImageView{storage, UnsignedInt(format), {}, pixelFormatSize(format), size, flags} {
    static_assert(sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class T> template<UnsignedInt otherDimensions, typename std::enable_if<(otherDimensions < dimensions), int>::type> ImageView<dimensions, T>::ImageView(const ImageView<otherDimensions, T>& other, const ImageFlags<dimensions> flags) noexcept:
    _storage{other._storage},
    _format{other._format},
    _formatExtra{other._formatExtra},
    _pixelSize{other._pixelSize},
    /* Removing the Array bit and transferring the rest, as documented */
    _flags{ImageFlag<dimensions>(UnsignedShort(other._flags)&~UnsignedShort(ImageFlag2D::Array))|flags},
    _size{Math::Vector<dimensions, Int>::pad(other._size, 1)},
    _data{other._data} {}

template<UnsignedInt dimensions, class T> template<class U, typename std::enable_if<std::is_const<T>::value && !std::is_const<U>::value, int>::type> ImageView<dimensions, T>::ImageView(const ImageView<dimensions, U>& other) noexcept: _storage{other._storage}, _format{other._format}, _formatExtra{other._formatExtra}, _pixelSize{other._pixelSize}, _flags{other._flags}, _size{other._size}, _data{other._data} {}
#endif

template<UnsignedInt dimensions, class T> template<class U> inline  CompressedImageView<dimensions, T>::CompressedImageView(const CompressedPixelStorage storage, const U format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data, const ImageFlags<dimensions> flags) noexcept: CompressedImageView{storage, UnsignedInt(format), size, data, flags} {
    static_assert(sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions, class T> template<class U> inline CompressedImageView<dimensions, T>::CompressedImageView(const CompressedPixelStorage storage, const U format, const VectorTypeFor<dimensions, Int>& size, const ImageFlags<dimensions> flags) noexcept: CompressedImageView{storage, UnsignedInt(format), size, flags} {
    static_assert(sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class T> template<UnsignedInt otherDimensions, typename std::enable_if<(otherDimensions < dimensions), int>::type> CompressedImageView<dimensions, T>::CompressedImageView(const CompressedImageView<otherDimensions, T>& other, const ImageFlags<dimensions> flags) noexcept:
    _storage{other._storage},
    _format{other._format},
    /* Removing the Array bit and transferring the rest, as documented */
    _flags{ImageFlag<dimensions>(UnsignedShort(other._flags)&~UnsignedShort(ImageFlag2D::Array))|flags},
    _size{Math::Vector<dimensions, Int>::pad(other._size, 1)},
    _data{other._data} {}

template<UnsignedInt dimensions, class T> template<class U, typename std::enable_if<std::is_const<T>::value && !std::is_const<U>::value, int>::type> CompressedImageView<dimensions, T>::CompressedImageView(const CompressedImageView<dimensions, U>& other) noexcept: _storage{other._storage}, _format{other._format}, _flags{other._flags}, _size{other._size}, _data{other._data} {}
#endif

}

#endif
