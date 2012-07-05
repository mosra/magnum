#ifndef Magnum_ImageWrapper_h
#define Magnum_ImageWrapper_h
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
 * @brief Class Magnum::ImageWrapper
 */

#include "AbstractImage.h"
#include "TypeTraits.h"

namespace Magnum {

/** @addtogroup textures
 * @{
 */

/**
@brief %Image wrapper

Adds information about dimensions, color components and component type to some
data in memory.

Unlike Image, this class doesn't delete the data on destruction, so it is
targeted for wrapping data which are either stored in stack/constant memory
(and shouldn't be deleted) or they are managed by someone else and have the
same properties for each frame, such as video stream. Thus it is not possible
to change image properties, only data pointer.

See also Image, BufferedImage and Trade::ImageData.
*/
template<size_t imageDimensions> class ImageWrapper: public AbstractImage {
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
        template<class T> inline ImageWrapper(const Math::Vector<Dimensions, GLsizei>& dimensions, Components components, T* data): AbstractImage(components, TypeTraits<T>::imageType()), _dimensions(dimensions), _data(data) {}

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
        inline ImageWrapper(const Math::Vector<Dimensions, GLsizei>& dimensions, Components components, ComponentType type, GLvoid* data): AbstractImage(components, type), _dimensions(dimensions), _data(reinterpret_cast<char*>(data)) {}

        /**
         * @brief Constructor
         * @param dimensions        %Image dimensions
         * @param components        Color components
         * @param type              Data type
         *
         * Dimensions and data pointer are set to zero, call setData() to fill
         * the image with data.
         */
        inline ImageWrapper(const Math::Vector<Dimensions, GLsizei>& dimensions, Components components, ComponentType type): AbstractImage(components, type), _dimensions(dimensions), _data(nullptr) {}

        /** @brief %Image dimensions */
        inline constexpr const Math::Vector<Dimensions, GLsizei>& dimensions() const { return _dimensions; }

        /** @brief Pointer to raw data */
        inline constexpr const void* data() const { return _data; }

        /**
         * @brief Set image data
         * @param data              %Image data
         *
         * Dimensions, color compnents and data type remains the same as
         * passed in constructor. The data are not copied nor deleted on
         * destruction.
         */
        void setData(GLvoid* data) {
            _data = reinterpret_cast<char*>(data);
        }

    protected:
        Math::Vector<Dimensions, GLsizei> _dimensions;  /**< @brief %Image dimensions */
        char* _data;                                    /**< @brief %Image data */
};

/** @brief One-dimensional image wrapper */
typedef ImageWrapper<1> ImageWrapper1D;

/** @brief Two-dimensional image wrapper */
typedef ImageWrapper<2> ImageWrapper2D;

/** @brief Three-dimensional image wrapper */
typedef ImageWrapper<3> ImageWrapper3D;

/*@}*/

}

#endif
