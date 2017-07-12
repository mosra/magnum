#ifndef Magnum_Image_h
#define Magnum_Image_h
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
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * The data are expected to be of proper size for given @p storage
         * parameters.
         */
        explicit Image(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data) noexcept;

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        explicit Image(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data) noexcept: Image{{}, format, type, size, std::move(data)} {}

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief Image(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&)
         * @deprecated Use @ref Image(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use Image(PixelFormat, PixelType, const VectorTypeFor&, Containers::Array&&) instead") Image(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, void* data) noexcept: Image{{}, format, type, size, Containers::Array<char>{reinterpret_cast<char*>(data), Implementation::imageDataSizeFor(format, type, size)}} {}
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* To avoid decay of nullptr to const void* and unwanted use of
           deprecated function */
        explicit Image(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, std::nullptr_t) noexcept: Image{{}, format, type, size, nullptr} {}
        #endif
        #endif

        /**
         * @brief Constructor
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Dimensions are set to zero and data pointer to `nullptr`, call
         * @ref setData() to fill the image with data or use
         * @ref Texture::image() "*Texture::image()"/
         * @ref Texture::subImage() "*Texture::subImage()"/
         * @ref AbstractFramebuffer::read() "*Framebuffer::read()" to fill the
         * image with data using @p storage settings.
         */
        /*implicit*/ Image(PixelStorage storage, PixelFormat format, PixelType type) noexcept: _storage{storage}, _format{format}, _type{type}, _data{} {}

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        /*implicit*/ Image(PixelFormat format, PixelType type) noexcept: Image{{}, format, type} {}

        /** @brief Copying is not allowed */
        Image(const Image<dimensions>&) = delete;

        /** @brief Move constructor */
        Image(Image<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        Image<dimensions>& operator=(const Image<dimensions>&) = delete;

        /** @brief Move assignment */
        Image<dimensions>& operator=(Image<dimensions>&& other) noexcept;

        /** @brief Conversion to view */
        /* Not restricted to const&, because we might want to pass the view to
           another function in an oneliner (e.g. saving screenshot) */
        /*implicit*/ operator ImageView<dimensions>() const;

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
        VectorTypeFor<dimensions, Int> size() const { return _size; }

        /**
         * @brief Image data properties
         *
         * See @ref PixelStorage::dataProperties() for more information.
         */
        std::tuple<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>, std::size_t> dataProperties() const {
            return Implementation::imageDataProperties<dimensions>(*this);
        }

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
        template<class T = char> T* data() {
            return reinterpret_cast<T*>(_data.data());
        }

        /** @overload */
        template<class T = char> const T* data() const {
            return reinterpret_cast<const T*>(_data.data());
        }

        /**
         * @brief Set image data
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Deletes previous data and replaces them with new. The data are
         * expected to be of proper size for given @p storage parameters.
         * @see @ref release()
         */
        void setData(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data);

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        void setData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data) {
            setData({}, format, type, size, std::move(data));
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief setData(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&)
         * @deprecated Use @ref setData(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::Array<char>&&)
         *      instead.
         */
        void CORRADE_DEPRECATED("use setData(PixelFormat, PixelType, const VectorTypeFor&, Containers::ArrayView) instead") setData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, void* data) {
            setData({}, format, type, size, Containers::Array<char>{reinterpret_cast<char*>(data), Implementation::imageDataSizeFor(format, type, size)});
        }
        #endif

        /**
         * @brief Release data storage
         *
         * Releases the ownership of the data array and resets internal state
         * to default.
         * @see @ref data()
         */
        Containers::Array<char> release();

    private:
        PixelStorage _storage;
        PixelFormat _format;
        PixelType _type;
        Math::Vector<Dimensions, Int> _size;
        Containers::Array<char> _data;
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
        explicit CompressedImage(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data);
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
        explicit CompressedImage(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Constructor
         * @param storage           Storage of compressed pixel data
         *
         * Format is undefined, size is zero and data are empty, call
         * @ref setData() to fill the image with data or use
         * @ref Texture::compressedImage() "*Texture::compressedImage()"/
         * @ref Texture::compressedSubImage() "*Texture::compressedSubImage()"
         * to fill the image with data using @p storage settings.
         *
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        /*implicit*/ CompressedImage(CompressedPixelStorage storage);
        #endif

        /**
         * @brief Constructor
         *
         * Similar the above, but uses default @ref CompressedPixelStorage
         * parameters (or the hardcoded ones in OpenGL ES).
         */
        /*implicit*/ CompressedImage();

        /** @brief Copying is not allowed */
        CompressedImage(const CompressedImage<dimensions>&) = delete;

        /** @brief Move constructor */
        CompressedImage(CompressedImage<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        CompressedImage<dimensions>& operator=(const CompressedImage<dimensions>&) = delete;

        /** @brief Move assignment */
        CompressedImage<dimensions>& operator=(CompressedImage<dimensions>&& other) noexcept;

        /** @brief Conversion to view */
        /*implicit*/ operator CompressedImageView<dimensions>() const;

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

        /** @brief Format of compressed pixel data */
        CompressedPixelFormat format() const { return _format; }

        /** @brief Image size */
        VectorTypeFor<dimensions, Int> size() const { return _size; }

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
        std::tuple<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>, std::size_t> dataProperties() const {
            return Implementation::compressedImageDataProperties<dimensions>(*this);
        }
        #endif

        /**
         * @brief Raw data
         *
         * @see @ref release()
         */
        Containers::ArrayView<char> data() { return _data; }

        /** @overload */
        Containers::ArrayView<const char> data() const { return _data; }

        /** @overload */
        template<class T> T* data() {
            return reinterpret_cast<T*>(_data.data());
        }

        /** @overload */
        template<class T> const T* data() const {
            return reinterpret_cast<const T*>(_data.data());
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Deletes previous data and replaces them with new. Note that the
         * data are not copied, but they are deleted on destruction.
         * @see @ref release()
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        void setData(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data);
        #endif

        /**
         * @brief Set image data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         *
         * Similar the above, but uses default @ref CompressedPixelStorage
         * parameters (or the hardcoded ones in OpenGL ES and WebGL).
         */
        void setData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data);

        /**
         * @brief Release data storage
         *
         * Releases the ownership of the data array and resets internal state
         * to default.
         * @see @ref setData()
         */
        Containers::Array<char> release();

    private:
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage _storage;
        #endif
        CompressedPixelFormat _format;
        Math::Vector<Dimensions, Int> _size;
        Containers::Array<char> _data;
};

/** @brief One-dimensional compressed image */
typedef CompressedImage<1> CompressedImage1D;

/** @brief Two-dimensional compressed image */
typedef CompressedImage<2> CompressedImage2D;

/** @brief Three-dimensional compressed image */
typedef CompressedImage<3> CompressedImage3D;

template<UnsignedInt dimensions> inline Image<dimensions>::Image(Image<dimensions>&& other) noexcept: _storage{std::move(other._storage)}, _format{std::move(other._format)}, _type{std::move(other._type)}, _size{std::move(other._size)}, _data{std::move(other._data)} {
    other._size = {};
}

template<UnsignedInt dimensions> inline CompressedImage<dimensions>::CompressedImage(CompressedImage<dimensions>&& other) noexcept:
    #ifndef MAGNUM_TARGET_GLES
    _storage{std::move(other._storage)},
    #endif
    _format{std::move(other._format)}, _size{std::move(other._size)}, _data{std::move(other._data)}
{
    other._size = {};
}

template<UnsignedInt dimensions> inline Image<dimensions>& Image<dimensions>::operator=(Image<dimensions>&& other) noexcept {
    using std::swap;
    swap(_storage, other._storage);
    swap(_format, other._format);
    swap(_type, other._type);
    swap(_size, other._size);
    swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> inline CompressedImage<dimensions>& CompressedImage<dimensions>::operator=(CompressedImage<dimensions>&& other) noexcept {
    using std::swap;
    #ifndef MAGNUM_TARGET_GLES
    swap(_storage, other._storage);
    #endif
    swap(_format, other._format);
    swap(_size, other._size);
    swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> inline Image<dimensions>::operator ImageView<dimensions>() const
{
    return ImageView<dimensions>{_storage, _format, _type, _size, _data};
}

template<UnsignedInt dimensions> inline CompressedImage<dimensions>::operator CompressedImageView<dimensions>() const
{
    return CompressedImageView<dimensions>{
        #ifndef MAGNUM_TARGET_GLES
        _storage,
        #endif
        _format, _size, _data};
}

template<UnsignedInt dimensions> inline Containers::Array<char> Image<dimensions>::release() {
    Containers::Array<char> data{std::move(_data)};
    _size = {};
    return data;
}

template<UnsignedInt dimensions> inline Containers::Array<char> CompressedImage<dimensions>::release() {
    Containers::Array<char> data{std::move(_data)};
    _size = {};
    return data;
}

template<UnsignedInt dimensions> inline CompressedImage<dimensions>::CompressedImage(
    #ifndef MAGNUM_TARGET_GLES
    const CompressedPixelStorage storage,
    #endif
    const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data):
    #ifndef MAGNUM_TARGET_GLES
    _storage{storage},
    #endif
    _format{format}, _size{size}, _data{std::move(data)} {}

template<UnsignedInt dimensions> inline CompressedImage<dimensions>::CompressedImage(
    #ifndef MAGNUM_TARGET_GLES
    const CompressedPixelStorage storage
    #endif
    )
    #ifndef MAGNUM_TARGET_GLES
    : _storage{storage}
    #endif
    {}

#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> inline CompressedImage<dimensions>::CompressedImage(const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data): CompressedImage{{}, format, size, std::move(data)} {}

template<UnsignedInt dimensions> inline CompressedImage<dimensions>::CompressedImage(): CompressedImage{CompressedPixelStorage{}} {}

template<UnsignedInt dimensions> inline void CompressedImage<dimensions>::setData(const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data) {
    setData({}, format, size, std::move(data));
}
#endif

}

#endif
