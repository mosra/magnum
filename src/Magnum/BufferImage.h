#ifndef Magnum_BufferImage_h
#define Magnum_BufferImage_h
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

#ifndef MAGNUM_TARGET_GLES2
/** @file
 * @brief Class @ref Magnum::BufferImage, @ref Magnum::CompressedBufferImage, typedef @ref Magnum::BufferImage1D, @ref Magnum::BufferImage2D, @ref Magnum::BufferImage3D, @ref Magnum::CompressedBufferImage1D, @ref Magnum::CompressedBufferImage2D, @ref Magnum::CompressedBufferImage3D
 */
#endif

#include "Magnum/Buffer.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/PixelStorage.h"
#include "Magnum/Math/Vector4.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES2
/**
@brief Buffer image

Stores image data in GPU memory. Interchangeable with @ref Image,
@ref ImageView or @ref Trade::ImageData.
@see @ref BufferImage1D, @ref BufferImage2D, @ref BufferImage3D,
    @ref CompressedBufferImage, @ref Buffer
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
@requires_webgl20 Pixel buffer objects are not available in WebGL 1.0.
*/
template<UnsignedInt dimensions> class BufferImage {
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
         * @param usage             Image buffer usage
         *
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        explicit BufferImage(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        explicit BufferImage(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage): BufferImage{{}, format, type, size, data, usage} {}

        /**
         * @brief Construct from existing buffer
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param buffer            Buffer
         * @param dataSize          Buffer data size
         *
         * If @p dataSize is 0, the buffer is unconditionally reallocated on
         * the first call to @ref setData().
         */
        explicit BufferImage(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept;

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        explicit BufferImage(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept: BufferImage{{}, format, type, size, std::move(buffer), dataSize} {}

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief BufferImage(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         * @deprecated Use @ref BufferImage(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use BufferImage(PixelFormat, PixelType, const VectorTypeFor&, Containers::ArrayView, BufferUsage) instead") BufferImage(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const void* data, BufferUsage usage): BufferImage{{}, format, type, size, {data, Implementation::imageDataSizeFor(format, type, size)}, usage} {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* To avoid decay of sized arrays and nullptr to const void* and
           unwanted use of deprecated function */
        template<class T, std::size_t dataSize> explicit BufferImage(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const T(&data)[dataSize], BufferUsage usage): BufferImage{{}, format, type, size, Containers::ArrayView<const void>{data}, usage} {}
        /* To avoid ambiguous overload when passing Containers::Array[View] */
        template<class T> explicit BufferImage(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const Containers::Array<T>& data, BufferUsage usage): BufferImage{{}, format, type, size, Containers::ArrayView<const void>{data}, usage} {}
        template<class T> explicit BufferImage(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<T>& data, BufferUsage usage): BufferImage{{}, format, type, size, Containers::ArrayView<const void>{data}, usage} {}
        #endif
        #endif

        /**
         * @brief Constructor
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Size is zero and buffer are empty, call @ref setData() to fill the
         * image with data or use @ref Texture::image() "*Texture::image()"/
         * @ref Texture::subImage() "*Texture::subImage()"/
         * @ref AbstractFramebuffer::read() "*Framebuffer::read()" to fill the
         * image with data using @p storage settings.
         */
        /*implicit*/ BufferImage(PixelStorage storage, PixelFormat format, PixelType type);

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        /*implicit*/ BufferImage(PixelFormat format, PixelType type): BufferImage{{}, format, type} {}

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state with
         * @ref PixelFormat::RGBA and @ref PixelType::UnsignedByte. Useful in
         * cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * @see @ref BufferImage(), @ref Buffer::wrap()
         */
        explicit BufferImage(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        BufferImage(const BufferImage<dimensions>&) = delete;

        /** @brief Move constructor */
        BufferImage(BufferImage<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        BufferImage<dimensions>& operator=(const BufferImage<dimensions>&) = delete;

        /** @brief Move assignment */
        BufferImage<dimensions>& operator=(BufferImage<dimensions>&& other) noexcept;

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
        VectorTypeFor<Dimensions, Int> size() const { return _size; }

        /**
         * @brief Image data properties
         *
         * See @ref PixelStorage::dataProperties() for more information.
         */
        std::tuple<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>, std::size_t> dataProperties() const {
            return Implementation::imageDataProperties<dimensions>(*this);
        }

        /** @brief Currently allocated data size */
        std::size_t dataSize() const { return _dataSize; }

        /**
         * @brief Image buffer
         *
         * @see @ref release()
         */
        Buffer& buffer() { return _buffer; }

        /**
         * @brief Set image data
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Updates the image buffer with given data. Passing `nullptr`
         * zero-sized @p data will not reallocate current storage, but expects
         * that current data size is large enough for the new parameters.
         * @see @ref Buffer::setData()
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        void setData(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        void setData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage) {
            setData({}, format, type, size, data, usage);
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief setData(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         * @deprecated Use @ref setData(PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         *      instead.
         */
        void CORRADE_DEPRECATED("use setData(PixelFormat, PixelType, const VectorTypeFor&, Containers::ArrayView, BufferUsage) instead") setData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const void* data, BufferUsage usage) {
            setData({}, format, type, size, {data, Implementation::imageDataSizeFor(format, type, size)}, usage);
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* To avoid decay of sized char arrays and nullptr to const void* and
           unwanted use of deprecated function */
        template<class T, std::size_t dataSize> void setData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const T(&data)[dataSize], BufferUsage usage) {
            setData({}, format, type, size, Containers::ArrayView<const void>{data}, usage);
        }
        /* To avoid ambiguous overload when passing Containers::Array */
        template<class T> void setData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const Containers::Array<T>& data, BufferUsage usage) {
            setData({}, format, type, size, Containers::ArrayView<const void>(data), usage);
        }
        #endif
        #endif

        /**
         * @brief Release the image buffer
         *
         * Releases the ownership of the image buffer and resets internal state
         * to default.
         * @see @ref buffer()
         */
        Buffer release();

    private:
        PixelStorage _storage;
        PixelFormat _format;
        PixelType _type;
        Math::Vector<Dimensions, Int> _size;
        Buffer _buffer;
        std::size_t _dataSize;
};

/** @brief One-dimensional buffer image */
typedef BufferImage<1> BufferImage1D;

/** @brief Two-dimensional buffer image */
typedef BufferImage<2> BufferImage2D;

/** @brief Three-dimensional buffer image */
typedef BufferImage<3> BufferImage3D;

/**
@brief Compressed buffer image

Stores image data in GPU memory.

See @ref BufferImage for more information. Interchangeable with @ref CompressedImage,
@ref CompressedImageView or @ref Trade::ImageData.
@see @ref CompressedBufferImage1D, @ref CompressedBufferImage2D,
    @ref CompressedBufferImage3D
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
@requires_webgl20 Pixel buffer objects are not available in WebGL 1.0.
*/
template<UnsignedInt dimensions> class CompressedBufferImage {
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
         * @param usage             Image buffer usage
         *
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         *
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        explicit CompressedBufferImage(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);
        #endif

        /**
         * @brief Constructor
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Similar the above, but uses default @ref CompressedPixelStorage
         * parameters (or the hardcoded ones in OpenGL ES and WebGL).
         */
        explicit CompressedBufferImage(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Constructor
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param buffer            Image data
         * @param dataSize          Image buffer usage
         *
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        explicit CompressedBufferImage(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept;
        #endif

        /** @overload
         * Similar the above, but uses default @ref CompressedPixelStorage
         * parameters (or the hardcoded ones in OpenGL ES and WebGL).
         */
        explicit CompressedBufferImage(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept;

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Constructor
         * @param storage           Storage of compressed pixel data
         *
         * Format is undefined, size is zero and buffer is empty, call
         * @ref setData() to fill the image with data or use
         * @ref Texture::compressedImage() "*Texture::compressedImage()"/
         * @ref Texture::compressedSubImage() "*Texture::compressedSubImage()"
         * to fill the image with data using @p storage settings.
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         */
        /*implicit*/ CompressedBufferImage(CompressedPixelStorage storage);
        #endif

        /**
         * @brief Constructor
         *
         * Similar the above, but uses default @ref CompressedPixelStorage
         * parameters (or the hardcoded ones in OpenGL ES and WebGL).
         */
        /*implicit*/ CompressedBufferImage();

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         * @see @ref CompressedBufferImage(), @ref Buffer::wrap()
         */
        explicit CompressedBufferImage(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        CompressedBufferImage(const CompressedBufferImage<dimensions>&) = delete;

        /** @brief Move constructor */
        CompressedBufferImage(CompressedBufferImage<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        CompressedBufferImage<dimensions>& operator=(const CompressedBufferImage<dimensions>&) = delete;

        /** @brief Move assignment */
        CompressedBufferImage<dimensions>& operator=(CompressedBufferImage<dimensions>&& other) noexcept;

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
        VectorTypeFor<Dimensions, Int> size() const { return _size; }

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
         * @brief Image buffer
         *
         * @see @ref release()
         */
        Buffer& buffer() { return _buffer; }

        /** @brief Raw data size */
        std::size_t dataSize() const { return _dataSize; }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Updates the image buffer with given data. The data are *not* deleted
         * after filling the buffer.
         * @see @ref Buffer::setData()
         * @requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
         * @requires_gl Compressed pixel storage is hardcoded in OpenGL ES and
         *      WebGL.
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        void setData(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);
        #endif

        /**
         * @brief Set image data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Similar the above, but uses default @ref CompressedPixelStorage
         * parameters (or the hardcoded ones in OpenGL ES and WebGL).
         */
        void setData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /**
         * @brief Release the image buffer
         *
         * Releases the ownership of the image buffer and resets internal state
         * to default.
         * @see @ref buffer()
         */
        Buffer release();

    private:
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage _storage;
        #endif
        CompressedPixelFormat _format;
        Math::Vector<Dimensions, Int> _size;
        Buffer _buffer;
        std::size_t _dataSize;
};

/** @brief One-dimensional compressed buffer image */
typedef CompressedBufferImage<1> CompressedBufferImage1D;

/** @brief Two-dimensional compressed buffer image */
typedef CompressedBufferImage<2> CompressedBufferImage2D;

/** @brief Three-dimensional compressed buffer image */
typedef CompressedBufferImage<3> CompressedBufferImage3D;

template<UnsignedInt dimensions> inline BufferImage<dimensions>::BufferImage(BufferImage<dimensions>&& other) noexcept: _storage{std::move(other._storage)}, _format{std::move(other._format)}, _type{std::move(other._type)}, _size{std::move(other._size)}, _buffer{std::move(other._buffer)} {
    other._size = {};
}

template<UnsignedInt dimensions> inline CompressedBufferImage<dimensions>::CompressedBufferImage(CompressedBufferImage<dimensions>&& other) noexcept:
    #ifndef MAGNUM_TARGET_GLES
    _storage{std::move(other._storage)},
    #endif
    _format{std::move(other._format)}, _size{std::move(other._size)}, _buffer{std::move(other._buffer)}, _dataSize{std::move(other._dataSize)} {
    other._size = {};
    other._dataSize = {};
}

template<UnsignedInt dimensions> inline BufferImage<dimensions>& BufferImage<dimensions>::operator=(BufferImage<dimensions>&& other) noexcept {
    using std::swap;
    swap(_storage, other._storage);
    swap(_format, other._format);
    swap(_type, other._type);
    swap(_size, other._size);
    swap(_buffer, other._buffer);
    return *this;
}

template<UnsignedInt dimensions> inline CompressedBufferImage<dimensions>& CompressedBufferImage<dimensions>::operator=(CompressedBufferImage<dimensions>&& other) noexcept {
    using std::swap;
    #ifndef MAGNUM_TARGET_GLES
    swap(_storage, other._storage);
    #endif
    swap(_format, other._format);
    swap(_size, other._size);
    swap(_buffer, other._buffer);
    swap(_dataSize, other._dataSize);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> inline CompressedBufferImage<dimensions>::CompressedBufferImage(const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data, const BufferUsage usage): CompressedBufferImage{{}, format, size, data, usage} {}

template<UnsignedInt dimensions> inline CompressedBufferImage<dimensions>::CompressedBufferImage(const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, const std::size_t dataSize) noexcept: CompressedBufferImage{{}, format, size, std::move(buffer), dataSize} {}

template<UnsignedInt dimensions> inline CompressedBufferImage<dimensions>::CompressedBufferImage(): CompressedBufferImage{CompressedPixelStorage{}} {}

template<UnsignedInt dimensions> inline void CompressedBufferImage<dimensions>::setData(const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data, const BufferUsage usage) {
    setData({}, format, size, data, usage);
}
#endif
#else
#error this header is not available in OpenGL ES 2.0 build
#endif

}

#endif
