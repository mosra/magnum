#ifndef Magnum_AbstractImage_h
#define Magnum_AbstractImage_h
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
 * @brief Class Magnum::AbstractImage
 */

#include <cstddef>

#include "Magnum.h"
#include "magnumVisibility.h"

namespace Magnum {

/**
@brief Non-templated base for one-, two- or three-dimensional images

See Image, ImageReference, BufferImage, Trade::ImageData documentation for
more information.
@todo Where to put glClampColor() and glPixelStore() encapsulation? It is
needed in AbstractFramebuffer::read(), Texture::setImage() etc (i.e. all
functions operating with images). It also possibly needs to be "stackable" to
easily revert the state back.
*/
class MAGNUM_EXPORT AbstractImage {
    public:
        /**
         * @brief Pixel size (in bytes)
         * @param format            Format of the pixel
         * @param type              Data type of the pixel
         *
         * @see pixelSize() const
         */
        static std::size_t pixelSize(ColorFormat format, ColorType type);

        /** @brief Format of pixel data */
        constexpr ColorFormat format() const { return _format; }

        /** @brief Data type of pixel data */
        constexpr ColorType type() const { return _type; }

        /**
         * @brief Pixel size (in bytes)
         *
         * Convenience member alternative for pixelSize(Format, Type).
         */
        std::size_t pixelSize() const { return pixelSize(_format, _type); }

    protected:
        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         */
        constexpr explicit AbstractImage(ColorFormat format, ColorType type): _format(format), _type(type) {}

        ~AbstractImage() = default;

        template<UnsignedInt dimensions> std::size_t dataSize(Math::Vector<dimensions, Int> size) const;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        ColorFormat _format;
        ColorType _type;
};

}

#endif
