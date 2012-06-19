#ifndef Magnum_Trade_ImageData_h
#define Magnum_Trade_ImageData_h
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
 * @brief Class Magnum::Trade::ImageData
 */

#include "AbstractImage.h"
#include "TypeTraits.h"

namespace Magnum { namespace Trade {

/**
@brief %Image data

Provides access to image data and additional information about data type and
dimensions. Can be used in the same situations as Image and BufferedImage.
*/
template<size_t imageDimensions> class ImageData: public AbstractImage {
    public:
        const static size_t Dimensions = imageDimensions;   /**< @brief %Image dimension count */

        /**
         * @brief Constructor
         * @param dimensions        %Image dimensions
         * @param components        Color components. Data type is detected
         *      from passed data array.
         * @param data              %Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        template<class T> inline ImageData(const Math::Vector<Dimensions, GLsizei>& dimensions, Components components, T* data): AbstractImage(components, TypeTraits<T>::imageType()), _dimensions(dimensions), _data(reinterpret_cast<char*>(data)) {}

        /**
         * @brief Constructor
         * @param dimensions        %Image dimensions
         * @param components        Color components
         * @param type              Data type
         * @param data              %Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        inline ImageData(const Math::Vector<Dimensions, GLsizei>& dimensions, Components components, ComponentType type, GLvoid* data): AbstractImage(components, type), _dimensions(dimensions), _data(reinterpret_cast<char*>(data)) {}

        /** @brief Destructor */
        inline ~ImageData() { delete[] _data; }

        /** @brief %Image dimensions */
        inline constexpr const Math::Vector<Dimensions, GLsizei>& dimensions() const { return _dimensions; }

        /** @brief Pointer to raw data */
        inline constexpr const void* data() const { return _data; }

    private:
        Math::Vector<Dimensions, GLsizei> _dimensions;
        char* _data;
};

/** @brief One-dimensional image */
typedef ImageData<1> ImageData1D;

/** @brief Two-dimensional image */
typedef ImageData<2> ImageData2D;

/** @brief Three-dimensional image */
typedef ImageData<3> ImageData3D;

}}

#endif
