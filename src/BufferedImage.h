#ifndef Magnum_BufferedImage_h
#define Magnum_BufferedImage_h
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
 * @brief Class Magnum::BufferedImage, typedef Magnum::BufferedImage1D, Magnum::BufferedImage2D, Magnum::BufferedImage3D
 */

#include "Math/Vector.h"
#include "AbstractImage.h"
#include "Buffer.h"
#include "TypeTraits.h"

namespace Magnum {

/**
@brief %Buffered image

Class for storing image data in GPU memory. Can be replaced with Image, which
stores image data in client memory, ImageWrapper, or for example with
Trade::ImageData.
@see BufferedImage1D, BufferedImage2D, BufferedImage3D, Buffer
@requires_gles30 (no extension providing this functionality)
*/
template<size_t dimensions> class BufferedImage: public AbstractImage {
    public:
        const static size_t Dimensions = dimensions;   /**< @brief Image dimension count */

        /**
         * @brief Constructor
         * @param components        Color components
         * @param type              Data type
         *
         * Dimensions and buffer are empty, call setData() to fill the image
         * with data.
         */
        inline BufferedImage(Components components, ComponentType type): AbstractImage(components, type), _buffer(Buffer::Target::PixelPack) {}

        /** @brief %Image size */
        inline constexpr Math::Vector<Dimensions, GLsizei> size() const { return _size; }

        /**
         * @brief Data
         *
         * Binds the buffer to @ref Buffer::Target "pixel unpack
         * target" and returns nullptr, so it can be used for texture updating
         * functions the same way as Image::data().
         *
         * @see Buffer::bind(Target)
         */
        inline void* data() {
            _buffer.bind(Buffer::Target::PixelUnpack);
            return nullptr;
        }

        /** @brief %Image buffer */
        inline Buffer* buffer() { return &_buffer; }

        /**
         * @brief Set image data
         * @param size              %Image size
         * @param components        Color components. Data type is detected
         *      from passed data array.
         * @param data              %Image data
         * @param usage             %Image buffer usage
         *
         * Updates the image buffer with given data. The data are not deleted
         * after filling the buffer.
         *
         * @see setData(const Math::Vector<Dimensions, GLsizei>&, Components, ComponentType, const GLvoid*, Buffer::Usage)
         */
        template<class T> inline void setData(const Math::Vector<Dimensions, GLsizei>& size, Components components, const T* data, Buffer::Usage usage) {
            setData(size, components, TypeTraits<T>::imageType(), data, usage);
        }

        /**
         * @brief Set image data
         * @param size              %Image size
         * @param components        Color components
         * @param type              Data type
         * @param data              %Image data
         * @param usage             %Image buffer usage
         *
         * Updates the image buffer with given data. The data are not deleted
         * after filling the buffer.
         *
         * @see Buffer::setData()
         */
        void setData(const Math::Vector<Dimensions, GLsizei>& size, Components components, ComponentType type, const GLvoid* data, Buffer::Usage usage);

    protected:
        Math::Vector<Dimensions, GLsizei> _size;    /**< @brief %Image size */
        Buffer _buffer;                             /**< @brief %Image buffer */
};

#ifndef DOXYGEN_GENERATING_OUTPUT
extern template class BufferedImage<1>;
extern template class BufferedImage<2>;
extern template class BufferedImage<3>;
#endif

/** @brief One-dimensional buffered image */
typedef BufferedImage<1> BufferedImage1D;

/** @brief Two-dimensional buffered image */
typedef BufferedImage<2> BufferedImage2D;

/** @brief Three-dimensional buffered image */
typedef BufferedImage<3> BufferedImage3D;

}

#endif
