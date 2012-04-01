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

#include "AbstractImage.h"

namespace Magnum {

size_t AbstractImage::pixelSize(ColorFormat format, Type type) {
    size_t size = TypeInfo::sizeOf(type);
    switch(format) {
        case ColorFormat::Red:
            return 1*size;
        case ColorFormat::RedGreen:
            return 2*size;
        case ColorFormat::RGB:
        case ColorFormat::BGR:
            return 3*size;
        case ColorFormat::RGBA:
        case ColorFormat::BGRA:
            return 4*size;
        default:
            return 0;
    }
}

}
