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
 * @brief Class @ref Magnum::BufferImage, typedef @ref Magnum::BufferImage1D, @ref Magnum::BufferImage2D, @ref Magnum::BufferImage3D
 */
#endif

#include "Magnum/AbstractImage.h"
#include "Magnum/Buffer.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES2
/**
@brief Buffer image

Stores image data in GPU memory. Interchangeable with @ref Image,
@ref ImageReference or @ref Trade::ImageData.
@see @ref BufferImage1D, @ref BufferImage2D, @ref BufferImage3D, @ref Buffer
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
@requires_webgl20 Pixel buffer objects are not available in WebGL 1.0.
*/
template<UnsignedInt dimensions> class BufferImage: public AbstractImage {
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
        explicit BufferImage(ColorFormat format, ColorType type, const VectorTypeFor<dimensions, Int>& size, const void* data, BufferUsage usage);

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Size is zero and buffer are empty, call @ref setData() to fill the
         * image with data.
         */
        /*implicit*/ BufferImage(ColorFormat format, ColorType type);

        /** @brief Copying is not allowed */
        BufferImage(const BufferImage<dimensions>&) = delete;

        /** @brief Move constructor */
        BufferImage(BufferImage<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        BufferImage<dimensions>& operator=(const BufferImage<dimensions>&) = delete;

        /** @brief Move assignment */
        BufferImage<dimensions>& operator=(BufferImage<dimensions>&& other) noexcept;

        /** @brief Format of pixel data */
        ColorFormat format() const { return _format; }

        /** @brief Data type of pixel data */
        ColorType type() const { return _type; }

        /** @brief Pixel size (in bytes) */
        std::size_t pixelSize() const { return Implementation::imagePixelSize(_format, _type); }

        /** @brief Image size */
        VectorTypeFor<Dimensions, Int> size() const { return _size; }

        /** @copydoc Image::dataSize() */
        std::size_t dataSize(const VectorTypeFor<dimensions, Int>& size) const {
            return Implementation::imageDataSize<dimensions>(*this, _format, _type, size);
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
        void setData(ColorFormat format, ColorType type, const VectorTypeFor<dimensions, Int>& size, const void* data, BufferUsage usage);

    private:
        ColorFormat _format;
        ColorType _type;
        Math::Vector<Dimensions, Int> _size;
        Buffer _buffer;
};

/** @brief One-dimensional buffer image */
typedef BufferImage<1> BufferImage1D;

/** @brief Two-dimensional buffer image */
typedef BufferImage<2> BufferImage2D;

/** @brief Three-dimensional buffer image */
typedef BufferImage<3> BufferImage3D;

template<UnsignedInt dimensions> inline BufferImage<dimensions>::BufferImage(BufferImage<dimensions>&& other) noexcept: AbstractImage{std::move(other)}, _format{std::move(other._format)}, _type{std::move(other._type)}, _size{std::move(other._size)}, _buffer{std::move(other._buffer)} {
    other._size = {};
}

template<UnsignedInt dimensions> inline BufferImage<dimensions>& BufferImage<dimensions>::operator=(BufferImage<dimensions>&& other) noexcept {
    AbstractImage::operator=(std::move(other));
    using std::swap;
    swap(_format, other._format);
    swap(_type, other._type);
    swap(_size, other._size);
    swap(_buffer, other._buffer);
    return *this;
}
#else
#error this header is not available in OpenGL ES 2.0 build
#endif

}

#endif
