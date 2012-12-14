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

#ifndef MAGNUM_TARGET_GLES2
/** @file
 * @brief Class Magnum::BufferedImage, typedef Magnum::BufferedImage1D, Magnum::BufferedImage2D, Magnum::BufferedImage3D
 */
#endif

#include "Math/Vector3.h"
#include "AbstractImage.h"
#include "Buffer.h"
#include "DimensionTraits.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES2
/**
@brief %Buffered image

Stores image data in GPU memory. Interchangeable with Image, ImageWrapper or
Trade::ImageData.
@see BufferedImage1D, BufferedImage2D, BufferedImage3D, Buffer
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
*/
template<std::uint8_t dimensions> class MAGNUM_EXPORT BufferedImage: public AbstractImage {
    public:
        const static std::uint8_t Dimensions = dimensions; /**< @brief %Image dimension count */

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Dimensions and buffer are empty, call setData() to fill the image
         * with data.
         */
        inline BufferedImage(Format format, Type type): AbstractImage(format, type) {
            _buffer.setTargetHint(Buffer::Target::PixelPack);
        }

        /** @brief %Image size */
        inline typename DimensionTraits<Dimensions, GLsizei>::VectorType size() const { return _size; }

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
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param data              %Image data
         * @param usage             %Image buffer usage
         *
         * Updates the image buffer with given data. The data are not deleted
         * after filling the buffer.
         *
         * @see Buffer::setData()
         */
        void setData(const typename DimensionTraits<Dimensions, GLsizei>::VectorType& size, Format format, Type type, const GLvoid* data, Buffer::Usage usage);

    private:
        Math::Vector<Dimensions, GLsizei> _size;
        Buffer _buffer;
};

/** @brief One-dimensional buffered image */
typedef BufferedImage<1> BufferedImage1D;

/** @brief Two-dimensional buffered image */
typedef BufferedImage<2> BufferedImage2D;

/** @brief Three-dimensional buffered image */
typedef BufferedImage<3> BufferedImage3D;
#endif

}

#endif
