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

#include "Math/Vector3.h"
#include "AbstractImage.h"
#include "DimensionTraits.h"
#include "TypeTraits.h"

namespace Magnum { namespace Trade {

/**
@brief %Image data

Access to image data provided by AbstractImporter subclasses. Interchangeable
with Image, ImageWrapper or BufferedImage.
@see ImageData1D, ImageData2D, ImageData3D
*/
template<std::uint8_t dimensions> class ImageData: public AbstractImage {
    public:
        const static std::uint8_t Dimensions = dimensions; /**< @brief %Image dimension count */

        /**
         * @brief Constructor
         * @param name              %Image name
         * @param size              %Image size
         * @param format            Format of pixel data. Data type is
         *      detected from passed data array.
         * @param data              %Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        template<class T> inline ImageData(const std::string& name, const typename DimensionTraits<Dimensions, GLsizei>::VectorType& size, Format format, T* data): AbstractImage(format, TypeTraits<T>::imageType()), _name(name), _size(size), _data(reinterpret_cast<char*>(data)) {}

        /**
         * @brief Constructor
         * @param name              %Image name
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param data              %Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        inline ImageData(const std::string& name, const typename DimensionTraits<Dimensions, GLsizei>::VectorType& size, Format format, Type type, GLvoid* data): AbstractImage(format, type), _name(name), _size(size), _data(reinterpret_cast<char*>(data)) {}

        /** @brief Destructor */
        inline ~ImageData() { delete[] _data; }

        /** @brief %Image name */
        inline std::string name() const { return _name; }

        /** @brief %Image size */
        inline typename DimensionTraits<Dimensions, GLsizei>::VectorType size() const { return _size; }

        /** @brief Pointer to raw data */
        inline void* data() { return _data; }
        inline const void* data() const { return _data; } /**< @overload */

    private:
        std::string _name;
        Math::Vector<Dimensions, GLsizei> _size;
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
