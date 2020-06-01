#ifndef Magnum_ImageView_h
#define Magnum_ImageView_h
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
 * @brief Class @ref Magnum::ImageView, @ref Magnum::CompressedImageView, alias @ref Magnum::BasicImageView, @ref Magnum::BasicCompressedImageView, typedef @ref Magnum::ImageView1D, @ref Magnum::ImageView2D, @ref Magnum::ImageView3D, @ref Magnum::MutableImageView1D, @ref Magnum::MutableImageView2D, @ref Magnum::MutableImageView3D, @ref Magnum::CompressedImageView1D, @ref Magnum::CompressedImageView2D, @ref Magnum::CompressedImageView3D, @ref Magnum::MutableCompressedImageView1D, @ref Magnum::MutableCompressedImageView2D, @ref Magnum::MutableCompressedImageView3D
 */

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/DimensionTraits.h"
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

Usually, the view is created on some pre-existing data array in order to
describe its layout, with pixel format being one of the values from the generic
@link PixelFormat @endlink:

@snippet Magnum.cpp ImageView-usage

On construction, the image view internally calculates pixel size corresponding
to given pixel format using @ref pixelSize(). This value is needed to check
that the passed data array is large enough and is also required by most image
manipulation operations.

It's also possible to create an empty view and assign the memory later. That is
useful for example in case of multi-buffered video streaming, where each frame
has the same properties but a different memory location:

@snippet Magnum.cpp ImageView-usage-streaming

It's possible to have views on image sub-rectangles, 3D texture slices or
images with over-aligned rows by passing a particular @ref PixelStorage as
first parameter. In the following snippet, the view is the center 25x25
sub-rectangle of a 75x75 8-bit RGB image , with rows aligned to four bytes:

@snippet Magnum.cpp ImageView-usage-storage

@section ImageView-mutable Data mutability

When using types derived from @ref BasicImageView (e.g. @ref ImageView2D), the
viewed data are immutable. This is the most common use case. In order to be
able to mutate the underlying data (for example in order to read into a
pre-allocated memory), use @ref BasicMutableImageView
(e.g. @ref MutableImageView2D) instead. @ref Image and @ref Trade::ImageData
are convertible to either of these. Similarly to
@ref Corrade::Containers::ArrayView etc., a mutable view is also implicitly
convertible to a const one.

@subsection ImageView-usage-implementation-specific Implementation-specific formats

For known graphics APIs, there's a set of utility functions converting from
@ref PixelFormat to implementation-specific format identifiers and such
conversion is done implicitly when passing the view to a particular API. See
the enum documentation and documentation of its values for more information.

In some cases, for example when there's no corresponding generic format
available, it's desirable to specify the pixel format using
implementation-specific identifiers directly. In case of OpenGL that would be
the @ref GL::PixelFormat and @ref GL::PixelType pair:

@snippet Magnum.cpp ImageView-usage-gl

In such cases, pixel size is calculated using either @cpp pixelSize(T, U) @ce
or @cpp pixelSize(T) @ce that is found using
[ADL](https://en.wikipedia.org/wiki/Argument-dependent_name_lookup), with
@cpp T @ce and @cpp U @ce corresponding to types of passed arguments. The
implementation-specific format is wrapped in @ref PixelFormat using
@ref pixelFormatWrap() and @ref format() returns the wrapped value. In order to
distinguish if the format is wrapped, use @ref isPixelFormatImplementationSpecific()
and then extract the implementation-specific identifier using @ref pixelFormatUnwrap().
For APIs that have an additional format specifier (such as OpenGL), the second
value is stored verbatim in @ref formatExtra():

@snippet Magnum.cpp ImageView-usage-gl-extract

As a final fallback, types for which the @cpp pixelSize() @ce overload is not
available can be specified directly together with pixel size. In particular,
pixel size of @cpp 0 @ce will cause the image to be treated as fully opaque
data, disabling all slicing operations. The following shows a image view using
Metal-specific format identifier:

@snippet Magnum.cpp ImageView-usage-metal

@see @ref BasicImageView, @ref ImageView1D, @ref ImageView2D, @ref ImageView3D,
    @ref BasicMutableImageView, @ref MutableImageView1D,
    @ref MutableImageView2D, @ref MutableImageView3D, @ref Image-pixel-views
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
         *
         * The @p data array is expected to be of proper size for given
         * parameters.
         */
        explicit ImageView(PixelStorage storage, PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept;

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Equivalent to calling @ref ImageView(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>)
         * with default-constructed @ref PixelStorage.
         */
        explicit ImageView(PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept: ImageView{{}, format, size, data} {}

        /**
         * @brief Construct an empty view
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image.
         */
        explicit ImageView(PixelStorage storage, PixelFormat format, const VectorTypeFor<dimensions, Int>& size) noexcept;

        /**
         * @brief Construct an empty view
         * @param format            Format of pixel data
         * @param size              Image size
         *
         * Equivalent to calling @ref ImageView(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&)
         * with default-constructed @ref PixelStorage.
         */
        explicit ImageView(PixelFormat format, const VectorTypeFor<dimensions, Int>& size) noexcept: ImageView{{}, format, size} {}

        /**
         * @brief Construct an image view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param pixelSize         Size of a pixel in given format
         * @param size              Image size
         * @param data              Image data
         *
         * Unlike with @ref ImageView(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>),
         * where pixel size is calculated automatically using
         * @ref pixelSize(PixelFormat), this allows you to specify an
         * implementation-specific pixel format and pixel size directly. Uses
         * @ref pixelFormatWrap() internally to wrap @p format in
         * @ref PixelFormat.
         *
         * The @p data array is expected to be of proper size for given
         * parameters.
         */
        explicit ImageView(PixelStorage storage, UnsignedInt format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept;

        /** @overload
         *
         * Equivalent to the above for @p format already wrapped with
         * @ref pixelFormatWrap().
         */
        explicit ImageView(PixelStorage storage, PixelFormat format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept;

        /**
         * @brief Construct an empty view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param pixelSize         Size of a pixel in given format
         * @param size              Image size
         *
         * Unlike with @ref ImageView(PixelStorage, PixelFormat, const VectorTypeFor<dimensions, Int>&),
         * where pixel size is calculated automatically using
         * @ref pixelSize(PixelFormat), this allows you to specify an
         * implementation-specific pixel format and pixel size directly. Uses
         * @ref pixelFormatWrap() internally to wrap @p format in
         * @ref PixelFormat.
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image.
         */
        explicit ImageView(PixelStorage storage, UnsignedInt format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size) noexcept;

        /** @overload
         *
         * Equivalent to the above for @p format already wrapped with
         * @ref pixelFormatWrap().
         */
        explicit ImageView(PixelStorage storage, PixelFormat format, UnsignedInt formatExtra, UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size) noexcept;

        /**
         * @brief Construct an image view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         * @param data              Image data
         *
         * Uses ADL to find a corresponding @cpp pixelSize(T, U) @ce overload,
         * then calls @ref ImageView(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>)
         * with calculated pixel size.
         */
        template<class U, class V> explicit ImageView(PixelStorage storage, U format, V formatExtra, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept;

        /**
         * @brief Construct an image view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Uses ADL to find a corresponding @cpp pixelSize(T) @ce overload,
         * then calls @ref ImageView(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>)
         * with calculated pixel size and @p formatExtra set to @cpp 0 @ce.
         */
        template<class U> explicit ImageView(PixelStorage storage, U format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept;

        /**
         * @brief Construct an image view with implementation-specific pixel format
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         * @param data              Image data
         *
         * Equivalent to calling @ref ImageView(PixelStorage, U, V, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>)
         * with default-constructed @ref PixelStorage.
         */
        template<class U, class V> explicit ImageView(U format, V formatExtra, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept: ImageView{{}, format, formatExtra, size, data} {}

        /**
         * @brief Construct an image view with implementation-specific pixel format
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Equivalent to calling @ref ImageView(PixelStorage, U, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>)
         * with default-constructed @ref PixelStorage.
         */
        template<class U> explicit ImageView(U format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept: ImageView{{}, format, size, data} {}

        /**
         * @brief Construct an empty view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         *
         * Uses ADL to find a corresponding @cpp pixelSize(T, U) @ce overload,
         * then calls @ref ImageView(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&)
         * with calculated pixel size.
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image.
         */
        template<class U, class V> explicit ImageView(PixelStorage storage, U format, V formatExtra, const VectorTypeFor<dimensions, Int>& size) noexcept;

        /**
         * @brief Construct an empty view with implementation-specific pixel format
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         *
         * Uses ADL to find a corresponding @cpp pixelSize(T) @ce overload,
         * then calls @ref ImageView(PixelStorage, UnsignedInt, UnsignedInt, UnsignedInt, const VectorTypeFor<dimensions, Int>&)
         * with calculated pixel size and @p formatExtra set to @cpp 0 @ce.
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image.
         */
        template<class U> explicit ImageView(PixelStorage storage, U format, const VectorTypeFor<dimensions, Int>& size) noexcept;

        /**
         * @brief Construct an empty view with implementation-specific pixel format
         * @param format            Format of pixel data
         * @param formatExtra       Additional pixel format specifier
         * @param size              Image size
         *
         * Equivalent to calling @ref ImageView(PixelStorage, U, V, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>)
         * with default-constructed @ref PixelStorage.
         */
        template<class U, class V> explicit ImageView(U format, V formatExtra, const VectorTypeFor<dimensions, Int>& size) noexcept: ImageView{{}, format, formatExtra, size} {}

        /**
         * @brief Construct an empty view with implementation-specific pixel format
         * @param format            Format of pixel data
         * @param size              Image size
         *
         * Equivalent to calling @ref ImageView(PixelStorage, U, const VectorTypeFor<dimensions, Int>&)
         * with default-constructed @ref PixelStorage.
         */
        template<class U> explicit ImageView(U format, const VectorTypeFor<dimensions, Int>& size) noexcept: ImageView{{}, format, size} {}

        /**
         * @brief Construct from a view of lower dimension count
         * @m_since{2019,10}
         */
        template<UnsignedInt otherDimensions, class = typename std::enable_if<(otherDimensions < dimensions)>::type> /*implicit*/ ImageView(const ImageView<otherDimensions, T>& other) noexcept;

        /**
         * @brief Convert a mutable view to a const one
         * @m_since{2019,10}
         */
        template<class U, class = typename std::enable_if<std::is_const<T>::value && !std::is_const<U>::value>::type> /*implicit*/ ImageView(const ImageView<dimensions, U>& other) noexcept;

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
         * @brief Pixel size (in bytes)
         *
         * @see @ref pixelSize(PixelFormat)
         */
        UnsignedInt pixelSize() const { return _pixelSize; }

        /** @brief Image size */
        constexpr VectorTypeFor<dimensions, Int> size() const { return _size; }

        /**
         * @brief Image data properties
         *
         * See @ref PixelStorage::dataProperties() for more information.
         */
        std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> dataProperties() const;

        /**
         * @brief Image data
         *
         * @see @ref pixels()
         */
        Containers::ArrayView<Type> data() const { return _data; }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Image data in a particular type
         * @m_deprecated_since{2019,10} Use non-templated @ref data() together
         *      with @ref Corrade::Containers::arrayCast() instead for properly
         *      bounds-checked type conversion.
         */
        template<class U> CORRADE_DEPRECATED("use data() together with Containers::arrayCast() instead") const U* data() const {
            return reinterpret_cast<const U*>(_data.data());
        }
        #endif

        /**
         * @brief Set image data
         *
         * The data array is expected to be of proper size for parameters
         * specified in the constructor.
         */
        void setData(Containers::ArrayView<ErasedType> data);

        /**
         * @brief View on pixel data
         * @m_since{2019,10}
         *
         * Provides direct and easy-to-use access to image pixels. See
         * @ref Image-pixel-views for more information. If the view is empty
         * (with @ref data() being @cpp nullptr @ce), returns @cpp nullptr @ce
         * as well.
         */
        Containers::StridedArrayView<dimensions + 1, Type> pixels() const;

        /**
         * @brief View on pixel data with a concrete pixel type
         * @m_since{2019,10}
         *
         * Compared to non-templated @ref pixels() in addition casts the pixel
         * data to a specified type. The user is responsible for choosing
         * correct type for given @ref format() --- checking it on the library
         * side is not possible for the general case. If the view is empty
         * (with @ref data() being @cpp nullptr @ce), returns @cpp nullptr @ce
         * as well.
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
        UnsignedInt _pixelSize;
        Math::Vector<Dimensions, Int> _size;
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

Usually, the view is created on some pre-existing data array in order to
describe its layout, with pixel format being one of the values from the generic
@link CompressedPixelFormat @endlink:

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
         */
        explicit CompressedImageView(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept;

        /**
         * @brief Constructor
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Equivalent to calling @ref CompressedImageView(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit CompressedImageView(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept: CompressedImageView{{}, format, size, data} {}

        /**
         * @brief Construct an empty view
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         *
         * Data pointer is set to @cpp nullptr @ce, call @ref setData() to
         * assign a memory view to the image.
         */
        explicit CompressedImageView(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size) noexcept;

        /**
         * @brief Construct an empty view
         * @param format            Format of compressed pixel data
         * @param size              Image size
         *
         * Equivalent to calling @ref CompressedImageView(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit CompressedImageView(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size) noexcept: CompressedImageView{{}, format, size} {}

        /**
         * @brief Construct an image view with implementation-specific format
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Uses @ref compressedPixelFormatWrap() internally to convert
         * @p format to @ref CompressedPixelFormat.
         */
        template<class U> explicit CompressedImageView(CompressedPixelStorage storage, U format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept;

        /**
         * @brief Construct an image view with implementation-specific format
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Equivalent to calling @ref CompressedImageView(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<ErasedType>)
         * with default-constructed @ref CompressedPixelStorage.
         */
        template<class U> explicit CompressedImageView(U format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept: CompressedImageView{{}, format, size, data} {}

        /**
         * @brief Construct an empty view with implementation-specific format
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         *
         * Uses @ref compressedPixelFormatWrap() internally to convert
         * @p format to @ref CompressedPixelFormat. Data pointer is set to
         * @cpp nullptr @ce, call @ref setData() to assign a memory view to the
         * image.
         */
        template<class U> explicit CompressedImageView(CompressedPixelStorage storage, U format, const VectorTypeFor<dimensions, Int>& size) noexcept;

        /**
         * @brief Construct an empty view with implementation-specific format
         * @param format            Format of compressed pixel data
         * @param size              Image size
         *
         * Equivalent to calling @ref CompressedImageView(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&)
         * with default-constructed @ref CompressedPixelStorage.
         */
        template<class U> explicit CompressedImageView(U format, const VectorTypeFor<dimensions, Int>& size) noexcept: CompressedImageView{{}, format, size} {}

        /**
         * @brief Construct from a view of lower dimension count
         * @m_since{2019,10}
         */
        template<UnsignedInt otherDimensions, class = typename std::enable_if<(otherDimensions < dimensions)>::type> /*implicit*/ CompressedImageView(const CompressedImageView<otherDimensions, T>& other) noexcept;

        /**
         * @brief Convert a mutable view to a const one
         * @m_since{2019,10}
         */
        template<class U, class = typename std::enable_if<std::is_const<T>::value && !std::is_const<U>::value>::type> /*implicit*/ CompressedImageView(const CompressedImageView<dimensions, U>& other) noexcept;

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

        /** @brief Image size */
        constexpr VectorTypeFor<dimensions, Int> size() const { return _size; }

        /**
         * @brief Compressed image data properties
         *
         * See @ref CompressedPixelStorage::dataProperties() for more
         * information.
         */
        std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> dataProperties() const;

        /** @brief Image data */
        Containers::ArrayView<Type> data() const { return _data; }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Image data in a particular type
         * @m_deprecated_since{2019,10} Use non-templated @ref data() together
         *      with @ref Corrade::Containers::arrayCast() instead for properly
         *      bounds-checked type conversion.
         */
        template<class U> CORRADE_DEPRECATED("use data() together with Containers::arrayCast() instead") const U* data() const {
            return reinterpret_cast<const U*>(_data.data());
        }
        #endif

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
        explicit CompressedImageView(CompressedPixelStorage storage, UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<ErasedType> data) noexcept;
        explicit CompressedImageView(CompressedPixelStorage storage, UnsignedInt format, const VectorTypeFor<dimensions, Int>& size) noexcept;

        CompressedPixelStorage _storage;
        CompressedPixelFormat _format;
        Math::Vector<Dimensions, Int> _size;
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

template<UnsignedInt dimensions, class T> template<class U, class V> inline ImageView<dimensions, T>::ImageView(const PixelStorage storage, const U format, const V formatExtra, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data) noexcept: ImageView{storage, UnsignedInt(format), UnsignedInt(formatExtra), Implementation::pixelSizeAdl(format, formatExtra), size, data} {
    static_assert(sizeof(T) <= 4 && sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions, class T> template<class U> inline ImageView<dimensions, T>::ImageView(const PixelStorage storage, const U format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data) noexcept: ImageView{storage, UnsignedInt(format), {}, Implementation::pixelSizeAdl(format), size, data} {
    static_assert(sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions, class T> template<class U, class V> inline ImageView<dimensions, T>::ImageView(const PixelStorage storage, const U format, const V formatExtra, const VectorTypeFor<dimensions, Int>& size) noexcept: ImageView{storage, UnsignedInt(format), UnsignedInt(formatExtra), Implementation::pixelSizeAdl(format, formatExtra), size} {
    static_assert(sizeof(U) <= 4 && sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions, class T> template<class U> inline ImageView<dimensions, T>::ImageView(const PixelStorage storage, const U format, const VectorTypeFor<dimensions, Int>& size) noexcept: ImageView{storage, UnsignedInt(format), {}, Implementation::pixelSizeAdl(format), size} {
    static_assert(sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

#ifndef DOXYGEN_GENERATING_OUTPUT /* it complains otherwise. why? don't know, don't want to know */
template<UnsignedInt dimensions, class T> template<UnsignedInt otherDimensions, class> ImageView<dimensions, T>::ImageView(const ImageView<otherDimensions, T>& other) noexcept: _storage{other._storage}, _format{other._format}, _formatExtra{other._formatExtra}, _pixelSize{other._pixelSize}, _size{Math::Vector<dimensions, Int>::pad(other._size, 1)}, _data{other._data} {}
#endif

template<UnsignedInt dimensions, class T> template<class U, class> ImageView<dimensions, T>::ImageView(const ImageView<dimensions, U>& other) noexcept: _storage{other._storage}, _format{other._format}, _formatExtra{other._formatExtra}, _pixelSize{other._pixelSize}, _size{other._size}, _data{other._data} {}

template<UnsignedInt dimensions, class T> template<class U> inline  CompressedImageView<dimensions, T>::CompressedImageView(const CompressedPixelStorage storage, const U format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data) noexcept: CompressedImageView{storage, UnsignedInt(format), size, data} {
    static_assert(sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

template<UnsignedInt dimensions, class T> template<class U> inline CompressedImageView<dimensions, T>::CompressedImageView(const CompressedPixelStorage storage, const U format, const VectorTypeFor<dimensions, Int>& size) noexcept: CompressedImageView{storage, UnsignedInt(format), size} {
    static_assert(sizeof(U) <= 4,
        "format types larger than 32bits are not supported");
}

#ifndef DOXYGEN_GENERATING_OUTPUT /* it complains otherwise. why? don't know, don't want to know */
template<UnsignedInt dimensions, class T> template<UnsignedInt otherDimensions, class> CompressedImageView<dimensions, T>::CompressedImageView(const CompressedImageView<otherDimensions, T>& other) noexcept: _storage{other._storage}, _format{other._format}, _size{Math::Vector<dimensions, Int>::pad(other._size, 1)}, _data{other._data} {}
#endif

template<UnsignedInt dimensions, class T> template<class U, class> CompressedImageView<dimensions, T>::CompressedImageView(const CompressedImageView<dimensions, U>& other) noexcept: _storage{other._storage}, _format{other._format}, _size{other._size}, _data{other._data} {}

}

#endif
