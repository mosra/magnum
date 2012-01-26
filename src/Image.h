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
 * @brief Class Magnum::Image
 */

#include "Magnum.h"
#include "TypeTraits.h"
#include "AbstractTexture.h"

namespace Magnum {

/**
@brief %Image

Class for storing data, which are later fed to textures. It is just a
transparent envelope around the data, which holds additional information about
data type and dimensions.
*/
template<size_t imageDimensions> class Image {
    Image<imageDimensions>(const Image<imageDimensions>& other) = delete;
    Image<imageDimensions>(Image<imageDimensions>&& other) = delete;
    Image<imageDimensions>& operator=(const Image<imageDimensions>& other) = delete;
    Image<imageDimensions>& operator=(Image<imageDimensions>&& other) = delete;

    public:
        const static size_t Dimensions = imageDimensions;   /**< @brief Image dimension count */

        /**
         * @brief Constructor
         * @param dimensions        %Image dimensions
         * @param colorFormat       Color format of passed data. Data size
         *      per color channel is detected from format of passed data array.
         * @param data              %Image data
         *
         * @attention Note that the image data are not copied on construction,
         * but they are deleted on class destruction.
         */
        template<class T> Image(const Math::Vector<GLsizei, Dimensions>& dimensions, AbstractTexture::ColorFormat colorFormat, const T* data): _dimensions(dimensions), _colorFormat(colorFormat), _type(TypeTraits<typename TypeTraits<T>::TextureType>::glType()), _data(reinterpret_cast<const char*>(data)) {}

        /** @brief Destructor */
        virtual ~Image() { delete[] _data; }

        /** @brief %Image dimensions */
        inline const Math::Vector<GLsizei, Dimensions>& dimensions() const { return _dimensions; }

        /** @brief Color format */
        inline AbstractTexture::ColorFormat colorFormat() const { return _colorFormat; }

        /** @brief Data type */
        GLenum type() const { return _type; }

        /** @brief Pointer to raw data */
        inline const void* data() const { return _data; }

    private:
        Math::Vector<GLsizei, Dimensions> _dimensions;
        AbstractTexture::ColorFormat _colorFormat;
        GLenum _type;
        const char* _data;
};

/** @brief One-dimensional image */
typedef Image<1> Image1D;

/** @brief Two-dimensional image */
typedef Image<2> Image2D;

/** @brief Three-dimensional image */
typedef Image<3> Image3D;

}

#endif
