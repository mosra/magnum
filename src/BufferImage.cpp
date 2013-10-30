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

#include "BufferImage.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void BufferImage<dimensions>::setData(const typename DimensionTraits<Dimensions, Int>::VectorType& size, ColorFormat format, ColorType type, const void* data, Buffer::Usage usage) {
    _format = format;
    _type = type;
    _size = size;
    _buffer.setData({data, pixelSize()*size.product()}, usage);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_EXPORT BufferImage<1>;
template class MAGNUM_EXPORT BufferImage<2>;
template class MAGNUM_EXPORT BufferImage<3>;
#endif
#endif

}
