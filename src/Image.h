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

#include "Math/Vector.h"
#include "AbstractImage.h"
#include "TypeTraits.h"

namespace Magnum {

/**
@brief %Image

Class for storing image data on client memory. Can be replaced with
ImageWrapper, BufferedImage, which stores image data in GPU memory, or for
example with Trade::ImageData.
@see Image1D, Image2D, Image3D
*/
template<size_t dimensions> class Image: public AbstractImage {
    public:
        const static size_t Dimensions = dimensions;   /**< @brief Image dimension count */

        /**
         * @brief Constructor
         * @param size              %Image size
         * @param components        Color components. Data type is detected
         *      from passed data array.
         * @param data              %Image data with proper size
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        template<class T> inline Image(const Math::Vector<Dimensions, GLsizei>& size, Components components, T* data): AbstractImage(components, TypeTraits<T>::imageType()), _size(size), _data(data) {}

        /**
         * @brief Constructor
         * @param size              %Image size
         * @param components        Color components
         * @param type              Data type
         * @param data              %Image data
         *
         * Note that the image data are not copied on construction, but they
         * are deleted on class destruction.
         */
        inline Image(const Math::Vector<Dimensions, GLsizei>& size, Components components, ComponentType type, GLvoid* data): AbstractImage(components, type), _size(size), _data(reinterpret_cast<char*>(data)) {}

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

        /** @brief %Image size */
        inline constexpr const Math::Vector<Dimensions, GLsizei>& size() const { return _size; }

        /** @brief Pointer to raw data */
        inline void* data() { return _data; }
        inline constexpr const void* data() const { return _data; } /**< @overload */

        /**
         * @brief Set image data
         * @param size              %Image size
         * @param components        Color components. Data type is detected
         *      from passed data array.
         * @param data              %Image data
         *
         * Deletes previous data and replaces them with new. Note that the
         * data are not copied, but they are deleted on destruction.
         */
        template<class T> inline void setData(const Math::Vector<Dimensions, GLsizei>& size, Components components, T* data) {
            setData(size, components, TypeTraits<T>::imageType(), data);
        }

        /**
         * @brief Set image data
         * @param size              %Image size
         * @param components        Color components
         * @param type              Data type
         * @param data              %Image data
         *
         * Deletes previous data and replaces them with new. Note that the
         * data are not copied, but they are deleted on destruction.
         */
        void setData(const Math::Vector<Dimensions, GLsizei>& size, Components components, ComponentType type, GLvoid* data);

    protected:
        Math::Vector<Dimensions, GLsizei> _size;    /**< @brief %Image size */
        char* _data;                                /**< @brief %Image data */
};

#ifndef DOXYGEN_GENERATING_OUTPUT
extern template class Image<1>;
extern template class Image<2>;
extern template class Image<3>;
#endif

/** @brief One-dimensional image */
typedef Image<1> Image1D;

/** @brief Two-dimensional image */
typedef Image<2> Image2D;

/** @brief Three-dimensional image */
typedef Image<3> Image3D;

}

#endif
