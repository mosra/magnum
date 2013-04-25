#ifndef Magnum_Image_h
#define Magnum_Image_h
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

/** @file
 * @brief Class Magnum::Image, typedef Magnum::Image1D, Magnum::Image2D, Magnum::Image3D
 */

#include "Math/Vector3.h"
#include "AbstractImage.h"
#include "DimensionTraits.h"

namespace Magnum {

/**
@brief %Image

Stores image data on client memory. Interchangeable with ImageWrapper,
BufferImage or Trade::ImageData.
@see Image1D, Image2D, Image3D
*/
template<UnsignedInt dimensions> class Image: public AbstractImage {
    public:
        const static UnsignedInt Dimensions = dimensions; /**< @brief %Image dimension count */

        /**
         * @brief Constructor
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param data              %Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        inline explicit Image(const typename DimensionTraits<Dimensions, Int>::VectorType& size, Format format, Type type, GLvoid* data): AbstractImage(format, type), _size(size), _data(reinterpret_cast<unsigned char*>(data)) {}

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Dimensions and data pointer are set to zero, call setData() to fill
         * the image with data.
         */
        inline explicit Image(Format format, Type type): AbstractImage(format, type), _data(nullptr) {}

        /** @brief Destructor */
        inline ~Image() { delete[] _data; }

        /** @brief %Image size */
        inline typename DimensionTraits<Dimensions, Int>::VectorType size() const { return _size; }

        /** @brief Pointer to raw data */
        inline unsigned char* data() { return _data; }
        inline const unsigned char* data() const { return _data; } /**< @overload */

        /**
         * @brief Set image data
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param data              %Image data
         *
         * Deletes previous data and replaces them with new. Note that the
         * data are not copied, but they are deleted on destruction.
         */
        void setData(const typename DimensionTraits<Dimensions, Int>::VectorType& size, Format format, Type type, GLvoid* data);

    private:
        Math::Vector<Dimensions, Int> _size;
        unsigned char* _data;
};

/** @brief One-dimensional image */
typedef Image<1> Image1D;

/** @brief Two-dimensional image */
typedef Image<2> Image2D;

/** @brief Three-dimensional image */
typedef Image<3> Image3D;

}

#endif
