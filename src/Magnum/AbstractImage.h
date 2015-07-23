#ifndef Magnum_AbstractImage_h
#define Magnum_AbstractImage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::AbstractImage, @ref Magnum::AbstractCompressedImage
 */

#include <cstddef>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

namespace Magnum {

namespace Implementation {
    std::size_t MAGNUM_EXPORT imagePixelSize(ColorFormat format, ColorType type);

    template<UnsignedInt dimensions> std::size_t imageDataSize(const AbstractImage& image, ColorFormat format, ColorType type, Math::Vector<dimensions, Int> size);
}

/**
@brief Non-templated base for one-, two- or three-dimensional uncompressed images

See @ref Image, @ref ImageView, @ref BufferImage and @ref Trade::ImageData
documentation for more information.
@see @ref AbstractCompressedImage
@todo Where to put glClampColor() and glPixelStore() encapsulation? It is
    needed in AbstractFramebuffer::read(), Texture::setImage() etc (i.e. all
    functions operating with images). It also possibly needs to be "stackable"
    to easily revert the state back.
*/
class AbstractImage {
    protected:
        ~AbstractImage() = default;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @todo remove this when the class has actual contents */
        /* Otherwise both (heh) GCC and Clang complain when move-constructing using {} */
        Int _dummy;
};

/**
@brief Non-templated base for one-, two- or three-dimensional compressed images

See @ref CompressedImage, @ref CompressedImageView, @ref CompressedBufferImage
and @ref Trade::ImageData documentation for more information.
@see @ref AbstractImage
*/
class AbstractCompressedImage: public AbstractImage {
    protected:
        ~AbstractCompressedImage() = default;
};

}

#endif
