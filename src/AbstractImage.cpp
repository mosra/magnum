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
#include "TypeTraits.h"

namespace Magnum {

size_t AbstractImage::pixelSize(Components format, ComponentType type) {
    size_t size;
    switch(type) {
        case ComponentType::RGB332:
        case ComponentType::BGR233:
            return 1;
        case ComponentType::RGB565:
        case ComponentType::BGR565:
        case ComponentType::RGBA4:
        case ComponentType::ABGR4:
        case ComponentType::RGB5Alpha1:
        case ComponentType::Alpha1BGR5:
            return 2;
        case ComponentType::RGBA8:
        case ComponentType::ABGR8:
        case ComponentType::RGB10Alpha2:
        case ComponentType::Alpha2RGB10:
        case ComponentType::Depth24Stencil8:
        case ComponentType::B10GR11Float:
        case ComponentType::Exponent5RGB9:
            return 4;
        case ComponentType::Depth32FloatStencil8:
            return 8;
        case ComponentType::UnsignedByte:
        case ComponentType::Byte:
            size = 1; break;
        case ComponentType::UnsignedShort:
        case ComponentType::Short:
        case ComponentType::HalfFloat:
            size = 2; break;
        case ComponentType::UnsignedInt:
        case ComponentType::Int:
        case ComponentType::Float:
            size = 4; break;
    }

    switch(format) {
        case Components::Red:
            return 1*size;
        case Components::RedGreen:
            return 2*size;
        case Components::RGB:
        case Components::BGR:
            return 3*size;
        case Components::RGBA:
        case Components::BGRA:
            return 4*size;
        default:
            return 0;
    }
}

}
