#ifndef Magnum_ImageReference_h
#define Magnum_ImageReference_h
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

/** @file
 * @brief Class @ref Magnum::ImageReference, typedef @ref Magnum::ImageReference1D, @ref Magnum::ImageReference2D, @ref Magnum::ImageReference3D
 */

#include "Magnum/Math/Vector3.h"
#include "Magnum/AbstractImage.h"
#include "Magnum/DimensionTraits.h"

namespace Magnum {

/**
@brief Image reference

Adds information about dimensions, color components and component type to some
data in memory.

Unlike @ref Image, this class doesn't delete the data on destruction, so it is
targeted for wrapping data which are either stored in stack/constant memory
(and shouldn't be deleted) or they are managed by someone else and have the
same properties for each frame, such as video stream. Thus it is not possible
to change image properties, only data pointer.

Interchangeable with @ref Image, @ref BufferImage or @ref Trade::ImageData.
@see @ref ImageReference1D, @ref ImageReference2D, @ref ImageReference3D
*/
template<UnsignedInt dimensions> class ImageReference: public AbstractImage {
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
         */
        constexpr explicit ImageReference(ColorFormat format, ColorType type, const VectorTypeFor<dimensions, Int>& size, const void* data): _format{format}, _type{type}, _size{size}, _data{reinterpret_cast<const char*>(data)} {}

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         *
         * Data pointer is set to `nullptr`, call @ref setData() to fill the
         * image with data.
         */
        constexpr explicit ImageReference(ColorFormat format, ColorType type, const VectorTypeFor<dimensions, Int>& size): _format{format}, _type{type}, _size{size}, _data{nullptr} {}

        /** @brief Format of pixel data */
        ColorFormat format() const { return _format; }

        /** @brief Data type of pixel data */
        ColorType type() const { return _type; }

        /** @brief Pixel size (in bytes) */
        std::size_t pixelSize() const { return Implementation::imagePixelSize(_format, _type); }

        /** @brief Image size */
        constexpr VectorTypeFor<dimensions, Int> size() const { return _size; }

        /** @copydoc Image::dataSize() */
        std::size_t dataSize(const VectorTypeFor<dimensions, Int>& size) const {
            return Implementation::imageDataSize<dimensions>(*this, _format, _type, size);
        }

        /** @brief Pointer to raw data */
        constexpr const char* data() const { return _data; }

        /** @overload */
        template<class T = char> const T* data() const {
            return reinterpret_cast<const T*>(_data);
        }

        /**
         * @brief Set image data
         * @param data              Image data
         *
         * Dimensions, color compnents and data type remains the same as
         * passed in constructor. The data are not copied nor deleted on
         * destruction.
         */
        void setData(const void* data) {
            _data = reinterpret_cast<const char*>(data);
        }

    private:
        ColorFormat _format;
        ColorType _type;
        Math::Vector<Dimensions, Int> _size;
        const char* _data;
};

/** @brief One-dimensional image wrapper */
typedef ImageReference<1> ImageReference1D;

/** @brief Two-dimensional image wrapper */
typedef ImageReference<2> ImageReference2D;

/** @brief Three-dimensional image wrapper */
typedef ImageReference<3> ImageReference3D;

}

#endif
