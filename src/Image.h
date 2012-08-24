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

#include "AbstractImage.h"
#include "TypeTraits.h"

namespace Magnum {

/**
@brief %Image

Class for storing image data on client memory. Can be replaced with
ImageWrapper, BufferedImage, which stores image data in GPU memory, or for
example with Trade::ImageData.
*/
template<size_t imageDimensions> class Image: public AbstractImage {
    public:
        const static size_t Dimensions = imageDimensions;   /**< @brief Image dimension count */

        /**
         * @brief Constructor
         * @param dimensions        %Image dimensions
         * @param components        Color components. Data type is detected
         *      from passed data array.
         * @param data              %Image data with proper size
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        template<class T> inline Image(const Math::Vector<Dimensions, GLsizei>& dimensions, Components components, T* data): AbstractImage(components, TypeTraits<T>::imageType()), _dimensions(dimensions), _data(data) {}

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
        inline Image(const Math::Vector<Dimensions, GLsizei>& dimensions, Components components, ComponentType type, GLvoid* data): AbstractImage(components, type), _dimensions(dimensions), _data(reinterpret_cast<char*>(data)) {}

        /**
         * @brief Constructor
         * @param components        Color components
         * @param type              Data type
         *
         * Dimensions and data pointer are set to zero, call setData() to fill
         * the image with data.
         */
        inline Image(Components components, ComponentType type): AbstractImage(components, type), _data(nullptr) {}

        /** @brief Destructor */
        inline ~Image() { delete[] _data; }

        /** @brief %Image dimensions */
        inline constexpr const Math::Vector<Dimensions, GLsizei>& dimensions() const { return _dimensions; }

        /** @brief Pointer to raw data */
        inline void* data() { return _data; }
        inline constexpr const void* data() const { return _data; } /**< @overload */

        /**
         * @brief Set image data
         * @param dimensions        %Image dimensions
         * @param components        Color components. Data type is detected
         *      from passed data array.
         * @param data              %Image data
         *
         * Deletes previous data and replaces them with new. Note that the
         * data are not copied, but they are deleted on destruction.
         */
        template<class T> inline void setData(const Math::Vector<Dimensions, GLsizei>& dimensions, Components components, T* data) {
            setData(dimensions, components, TypeTraits<T>::imageType(), data);
        }

        /**
         * @brief Set image data
         * @param dimensions        %Image dimensions
         * @param components        Color components
         * @param type              Data type
         * @param data              %Image data
         *
         * Deletes previous data and replaces them with new. Note that the
         * data are not copied, but they are deleted on destruction.
         */
        void setData(const Math::Vector<Dimensions, GLsizei>& dimensions, Components components, ComponentType type, GLvoid* data) {
            delete[] _data;
            _components = components;
            _type = type;
            _dimensions = dimensions;
            _data = reinterpret_cast<char*>(data);
        }

    protected:
        Math::Vector<Dimensions, GLsizei> _dimensions;  /**< @brief %Image dimensions */
        char* _data;                                    /**< @brief %Image data */
};

/** @brief One-dimensional image */
typedef Image<1> Image1D;

/** @brief Two-dimensional image */
typedef Image<2> Image2D;

/** @brief Three-dimensional image */
typedef Image<3> Image3D;

}

#endif
