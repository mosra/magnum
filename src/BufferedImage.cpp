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

#include "BufferedImage.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES2
template<std::uint8_t dimensions> void BufferedImage<dimensions>::setData(const typename DimensionTraits<Dimensions, GLsizei>::VectorType& size, Format format, Type type, const GLvoid* data, Buffer::Usage usage) {
    _format = format;
    _type = type;
    _size = size;
    _buffer.setData(pixelSize(format, type)*size.product(), data, usage);
}

template class BufferedImage<1>;
template class BufferedImage<2>;
template class BufferedImage<3>;
#endif

}
