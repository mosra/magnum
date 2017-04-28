#ifndef Magnum_ImageView_h
#define Magnum_ImageView_h
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
 * @brief Class @ref Magnum::ImageView, @ref Magnum::CompressedImageView, typedef @ref Magnum::ImageView1D, @ref Magnum::ImageView2D, @ref Magnum::ImageView3D, @ref Magnum::CompressedImageView1D, @ref Magnum::CompressedImageView2D, @ref Magnum::CompressedImageView3D
 */

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/PixelStorage.h"
#include "Magnum/Math/Vector4.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/Array.h>
#endif

namespace Magnum {

/**
@brief Image view

Adds information about dimensions, color components and component type to some
data in memory.

Unlike @ref Image, this class doesn't delete the data on destruction, so it is
targeted for wrapping data which are either stored in stack/constant memory
(and shouldn't be deleted) or they are managed by someone else and have the
same properties for each frame, such as video stream. Thus it is not possible
to change image properties, only data pointer.

Interchangeable with @ref Image, @ref BufferImage or @ref Trade::ImageData.
@see @ref ImageView1D, @ref ImageView2D, @ref ImageView3D,
    @ref CompressedImageView
*/
template<UnsignedInt dimensions> class ImageView {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Constructor
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * The data are expected to be of proper size for given @p storage
         * parameters.
         */
        explicit ImageView(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data) noexcept: _storage{storage}, _format{format}, _type{type}, _size{size}, _data{reinterpret_cast<const char*>(data.data()), data.size()} {
            CORRADE_ASSERT(!_data || Implementation::imageDataSize(*this) <= _data.size(), "ImageView::ImageView(): bad image data size, got" << _data.size() << "but expected at least" << Implementation::imageDataSize(*this), );
        }

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        explicit ImageView(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data) noexcept: ImageView{{}, format, type, size, data} {}

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief ImageView(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>)
         * @deprecated Use @ref ImageView(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>) instead.
         */
        explicit CORRADE_DEPRECATED("use ImageView(PixelFormat, PixelType, const VectorTypeFor&, Containers::ArrayView) instead") ImageView(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const void* data) noexcept: ImageView{{}, format, type, size, {reinterpret_cast<const char*>(data), Implementation::imageDataSizeFor(format, type, size)}} {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* To avoid ambiguous overload when passing ArrayView<T> to the
           constructor */
        template<class T> explicit ImageView(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<T> data): ImageView{{}, format, type, size, Containers::ArrayView<const void>{data}} {}
        template<class T> explicit ImageView(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const Containers::Array<T>& data): ImageView{{}, format, type, size, Containers::ArrayView<const void>(data)} {}
        /* To avoid decay of sized arrays and nullptr to const void* and
           unwanted use of deprecated function */
        template<class T, std::size_t dataSize> explicit ImageView(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const T(&data)[dataSize]): ImageView{{}, format, type, size, Containers::ArrayView<const void>{data}} {}
        explicit ImageView(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, std::nullptr_t): ImageView{{}, format, type, size, Containers::ArrayView<const void>{nullptr}} {}
        #endif
        #endif

        /**
         * @brief Constructor
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         *
         * Data pointer is set to `nullptr`, call @ref setData() to fill the
         * image with data.
         */
        constexpr explicit ImageView(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size) noexcept: _storage{storage}, _format{format}, _type{type}, _size{size}, _data{nullptr} {}

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        constexpr explicit ImageView(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size) noexcept: ImageView{{}, format, type, size} {}

        /** @brief Storage of pixel data */
        PixelStorage storage() const { return _storage; }

        /** @brief Format of pixel data */
        PixelFormat format() const { return _format; }

        /** @brief Data type of pixel data */
        PixelType type() const { return _type; }

        /**
         * @brief Pixel size (in bytes)
         *
         * @see @ref PixelStorage::pixelSize()
         */
        std::size_t pixelSize() const { return PixelStorage::pixelSize(_format, _type); }

        /** @brief Image size */
        constexpr VectorTypeFor<dimensions, Int> size() const { return _size; }

        /**
         * @brief Image data properties
         *
         * See @ref PixelStorage::dataProperties() for more information.
         */
        std::tuple<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>, std::size_t> dataProperties() const {
            return Implementation::imageDataProperties<dimensions>(*this);
        }

        /** @brief Raw data */
        constexpr Containers::ArrayView<const char> data() const { return _data; }

        /** @overload */
        template<class T> const T* data() const {
            return reinterpret_cast<const T*>(_data.data());
        }

        /**
         * @brief Set image data
         * @param data              Image data
         *
         * Storage parameters, pixel format, type and size remain the same as
         * passed in constructor. The data are expected to be of proper size
         * for given @p storage parameters.
         */
        void setData(Containers::ArrayView<const void> data) {
            CORRADE_ASSERT(Implementation::imageDataSize(*this) <= data.size(), "ImageView::setData(): bad image data size, got" << data.size() << "but expected at least" << Implementation::imageDataSize(*this), );
            _data = {reinterpret_cast<const char*>(data.data()), data.size()};
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief setData(Containers::ArrayView<const void>)
         * @deprecated Use @ref setData(Containers::ArrayView<const void>)
         *      instead.
         */
        void CORRADE_DEPRECATED("use setData(Containers::ArrayView) instead") setData(const void* data) {
            setData({reinterpret_cast<const char*>(data), Implementation::imageDataSize(*this)});
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* To avoid ambiguous overload when passing ArrayView<T> to the
           function */
        template<class T> void setData(Containers::ArrayView<const T> data) {
            setData(Containers::ArrayView<const void>{data});
        }
        template<class T> void setData(const Containers::Array<T>& data) {
            setData(Containers::ArrayView<const void>(data));
        }
        /* To avoid decay of sized arrays and nullptr to const void* and
           unwanted use of deprecated function */
        template<class T, std::size_t size> void setData(const T(&data)[size]) {
            setData(Containers::ArrayView<const void>{data});
        }
        void setData(std::nullptr_t) {
            setData(Containers::ArrayView<const void>{nullptr});
        }
        #endif
        #endif

    private:
        PixelStorage _storage;
        PixelFormat _format;
        PixelType _type;
        Math::Vector<Dimensions, Int> _size;
        Containers::ArrayView<const char> _data;
};

/** @brief One-dimensional image view */
typedef ImageView<1> ImageView1D;

/** @brief Two-dimensional image view */
typedef ImageView<2> ImageView2D;

/** @brief Three-dimensional image view */
typedef ImageView<3> ImageView3D;

/**
@brief Compressed image view

Adds information about dimensions and compression type to some data in memory.

See @ref ImageView for more information. Interchangeable with
@ref CompressedImage, @ref CompressedBufferImage or @ref Trade::ImageData.
@see @ref CompressedImageView1D, @ref CompressedImageView2D,
    @ref CompressedImageView3D
*/
template<UnsignedInt dimensions> class CompressedImageView {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Constructor
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        constexpr explicit CompressedImageView(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data) noexcept;
        #endif

        /**
         * @brief Constructor
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Similar the above, but uses default @ref CompressedPixelStorage
         * parameters (or the hardcoded ones in OpenGL ES and WebGL).
         */
        constexpr explicit CompressedImageView(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data) noexcept;

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Constructor
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         *
         * Data pointer is set to `nullptr`, call @ref setData() to fill the
         * image with data.
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        constexpr explicit CompressedImageView(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size) noexcept;
        #endif

        /**
         * @brief Constructor
         * @param format            Format of compressed pixel data
         * @param size              Image size
         *
         * Similar the above, but uses default @ref CompressedPixelStorage
         * parameters (or the hardcoded ones in OpenGL ES and WebGL).
         */
        constexpr explicit CompressedImageView(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size) noexcept;

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Storage of compressed pixel data
         *
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        CompressedPixelStorage storage() const { return _storage; }
        #endif

        /** @brief Format of compressed data */
        CompressedPixelFormat format() const { return _format; }

        /** @brief Image size */
        constexpr VectorTypeFor<dimensions, Int> size() const { return _size; }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Compressed image data properties
         *
         * See @ref CompressedPixelStorage::dataProperties() for more
         * information.
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        std::tuple<std::size_t, VectorTypeFor<dimensions, std::size_t>, std::size_t> dataProperties() const {
            return Implementation::compressedImageDataProperties<dimensions>(*this);
        }
        #endif

        /** @brief Raw data */
        constexpr Containers::ArrayView<const char> data() const { return _data; }

        /** @overload */
        template<class T> const T* data() const {
            return reinterpret_cast<const T*>(_data.data());
        }

        /**
         * @brief Set image data
         * @param data              Image data
         *
         * Dimensions, color compnents and data type remains the same as
         * passed in constructor. The data are not copied nor deleted on
         * destruction.
         */
        void setData(Containers::ArrayView<const void> data) {
            _data = {reinterpret_cast<const char*>(data.data()), data.size()};
        }

    private:
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage _storage;
        #endif
        CompressedPixelFormat _format;
        Math::Vector<Dimensions, Int> _size;
        Containers::ArrayView<const char> _data;
};

/** @brief One-dimensional compressed image view */
typedef CompressedImageView<1> CompressedImageView1D;

/** @brief Two-dimensional compressed image view */
typedef CompressedImageView<2> CompressedImageView2D;

/** @brief Three-dimensional compressed image view */
typedef CompressedImageView<3> CompressedImageView3D;

template<UnsignedInt dimensions> constexpr CompressedImageView<dimensions>::CompressedImageView(
    #ifndef MAGNUM_TARGET_GLES
    const CompressedPixelStorage storage,
    #endif
    const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data) noexcept:
    #ifndef MAGNUM_TARGET_GLES
    _storage{storage},
    #endif
    _format{format}, _size{size}, _data{reinterpret_cast<const char*>(data.data()), data.size()} {}

template<UnsignedInt dimensions> constexpr CompressedImageView<dimensions>::CompressedImageView(
    #ifndef MAGNUM_TARGET_GLES
    const CompressedPixelStorage storage,
    #endif
    const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size) noexcept:
    #ifndef MAGNUM_TARGET_GLES
    _storage{storage},
    #endif
    _format{format}, _size{size} {}

#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> constexpr CompressedImageView<dimensions>::CompressedImageView(const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data) noexcept: CompressedImageView{{}, format, size, data} {}

template<UnsignedInt dimensions> constexpr CompressedImageView<dimensions>::CompressedImageView(const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size) noexcept: CompressedImageView{{}, format, size} {}
#endif

}

#endif
