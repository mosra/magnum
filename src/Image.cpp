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

#include "Image.h"

namespace Magnum {

template<UnsignedInt dimensions> void Image<dimensions>::setData(const typename DimensionTraits<Dimensions, GLsizei>::VectorType& size, Format format, Type type, GLvoid* data) {
    delete[] _data;
    _format = format;
    _type = type;
    _size = size;
    _data = reinterpret_cast<char*>(data);
}

template class Image<1>;
template class Image<2>;
template class Image<3>;

}
