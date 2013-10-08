#ifndef Magnum_BufferImage_h
#define Magnum_BufferImage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::BufferImage, typedef Magnum::BufferImage1D, Magnum::BufferImage2D, Magnum::BufferImage3D
 */
#endif

#include "Math/Vector3.h"
#include "AbstractImage.h"
#include "Buffer.h"
#include "DimensionTraits.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES2
/**
@brief %Buffer image

Stores image data in GPU memory. Interchangeable with Image, ImageReference or
Trade::ImageData.
@see BufferImage1D, BufferImage2D, BufferImage3D, Buffer
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
*/
template<UnsignedInt dimensions> class MAGNUM_EXPORT BufferImage: public AbstractImage {
    public:
        const static UnsignedInt Dimensions = dimensions; /**< @brief %Image dimension count */

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Dimensions and buffer are empty, call setData() to fill the image
         * with data.
         */
        explicit BufferImage(ColorFormat format, ColorType type): AbstractImage(format, type) {
            _buffer.setTargetHint(Buffer::Target::PixelPack);
        }

        /** @brief %Image size */
        typename DimensionTraits<Dimensions, Int>::VectorType size() const { return _size; }

        /** @brief %Image buffer */
        Buffer& buffer() { return _buffer; }

        /**
         * @brief Set image data
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param data              %Image data
         * @param usage             %Image buffer usage
         *
         * Updates the image buffer with given data. The data are not deleted
         * after filling the buffer.
         *
         * @see Buffer::setData()
         */
        void setData(const typename DimensionTraits<Dimensions, Int>::VectorType& size, ColorFormat format, ColorType type, const void* data, Buffer::Usage usage);

    private:
        Math::Vector<Dimensions, Int> _size;
        Buffer _buffer;
};

/** @brief One-dimensional buffer image */
typedef BufferImage<1> BufferImage1D;

/** @brief Two-dimensional buffer image */
typedef BufferImage<2> BufferImage2D;

/** @brief Three-dimensional buffer image */
typedef BufferImage<3> BufferImage3D;
#endif

}

#endif
