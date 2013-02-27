#ifndef Magnum_Image_h
#define Magnum_Image_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
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
        inline explicit Image(const typename DimensionTraits<Dimensions, Int>::VectorType& size, Format format, Type type, GLvoid* data): AbstractImage(format, type), _size(size), _data(reinterpret_cast<char*>(data)) {}

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
        inline void* data() { return _data; }
        inline const void* data() const { return _data; } /**< @overload */

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
        char* _data;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
extern template class MAGNUM_EXPORT Image<1>;
extern template class MAGNUM_EXPORT Image<2>;
extern template class MAGNUM_EXPORT Image<3>;
#endif

/** @brief One-dimensional image */
typedef Image<1> Image1D;

/** @brief Two-dimensional image */
typedef Image<2> Image2D;

/** @brief Three-dimensional image */
typedef Image<3> Image3D;

}

#endif
