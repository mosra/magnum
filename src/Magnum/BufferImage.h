#ifndef Magnum_BufferImage_h
#define Magnum_BufferImage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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
#include "Magnum/Math/Vector3.h"

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
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * The data are *not* deleted after filling the buffer.
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        explicit BufferImage(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const void* data, BufferUsage usage);

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Size is zero and buffer are empty, call @ref setData() to fill the
         * image with data.
         */
        /*implicit*/ BufferImage(PixelFormat format, PixelType type);

        /** @brief Copying is not allowed */
        BufferImage(const BufferImage<dimensions>&) = delete;

        /** @brief Move constructor */
        BufferImage(BufferImage<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        BufferImage<dimensions>& operator=(const BufferImage<dimensions>&) = delete;

        /** @brief Move assignment */
        BufferImage<dimensions>& operator=(BufferImage<dimensions>&& other) noexcept;

        /** @brief Format of pixel data */
        PixelFormat format() const { return _format; }

        /** @brief Data type of pixel data */
        PixelType type() const { return _type; }

        /** @brief Pixel size (in bytes) */
        std::size_t pixelSize() const { return Implementation::imagePixelSize(_format, _type); }

        /** @brief Image size */
        VectorTypeFor<Dimensions, Int> size() const { return _size; }

        /** @copydoc Image::dataSize() */
        std::size_t dataSize(const VectorTypeFor<dimensions, Int>& size) const {
            return Implementation::imageDataSize<dimensions>(_format, _type, size);
        }

        /** @brief Image buffer */
        Buffer& buffer() { return _buffer; }

        /**
         * @brief Set image data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Updates the image buffer with given data. The data are *not* deleted
         * after filling the buffer.
         * @see @ref Buffer::setData()
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        void setData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, const void* data, BufferUsage usage);

    private:
        PixelFormat _format;
        PixelType _type;
        Math::Vector<Dimensions, Int> _size;
        Buffer _buffer;
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

        /**
         * @brief Constructor
         * @param format            Format of compressed data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * The data are *not* deleted after filling the buffer.
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        explicit CompressedBufferImage(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /**
         * @brief Constructor
         *
         * Format is undefined, size is zero and buffer is empty, call
         * @ref setData() to fill the image with data.
         */
        /*implicit*/ CompressedBufferImage();

        /** @brief Copying is not allowed */
        CompressedBufferImage(const CompressedBufferImage<dimensions>&) = delete;

        /** @brief Move constructor */
        CompressedBufferImage(CompressedBufferImage<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        CompressedBufferImage<dimensions>& operator=(const CompressedBufferImage<dimensions>&) = delete;

        /** @brief Move assignment */
        CompressedBufferImage<dimensions>& operator=(CompressedBufferImage<dimensions>&& other) noexcept;

        /** @brief Format of compressed data */
        CompressedPixelFormat format() const { return _format; }

        /** @brief Image size */
        VectorTypeFor<Dimensions, Int> size() const { return _size; }

        /** @brief Image buffer */
        Buffer& buffer() { return _buffer; }

        /** @brief Raw data size */
        std::size_t dataSize() const { return _dataSize; }

        /**
         * @brief Set image data
         * @param format            Format of compressed data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Updates the image buffer with given data. The data are *not* deleted
         * after filling the buffer.
         * @see @ref Buffer::setData()
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        void setData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

    private:
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

template<UnsignedInt dimensions> inline BufferImage<dimensions>::BufferImage(BufferImage<dimensions>&& other) noexcept: _format{std::move(other._format)}, _type{std::move(other._type)}, _size{std::move(other._size)}, _buffer{std::move(other._buffer)} {
    other._size = {};
}

template<UnsignedInt dimensions> inline CompressedBufferImage<dimensions>::CompressedBufferImage(CompressedBufferImage<dimensions>&& other) noexcept: _format{std::move(other._format)}, _size{std::move(other._size)}, _buffer{std::move(other._buffer)}, _dataSize{std::move(other._dataSize)} {
    other._size = {};
    other._dataSize = {};
}

template<UnsignedInt dimensions> inline BufferImage<dimensions>& BufferImage<dimensions>::operator=(BufferImage<dimensions>&& other) noexcept {
    using std::swap;
    swap(_format, other._format);
    swap(_type, other._type);
    swap(_size, other._size);
    swap(_buffer, other._buffer);
    return *this;
}

template<UnsignedInt dimensions> inline CompressedBufferImage<dimensions>& CompressedBufferImage<dimensions>::operator=(CompressedBufferImage<dimensions>&& other) noexcept {
    using std::swap;
    swap(_format, other._format);
    swap(_size, other._size);
    swap(_buffer, other._buffer);
    swap(_dataSize, other._dataSize);
    return *this;
}
#else
#error this header is not available in OpenGL ES 2.0 build
#endif

}

#endif
