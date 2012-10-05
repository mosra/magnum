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

using namespace std;

namespace Magnum {

size_t AbstractImage::pixelSize(Components format, ComponentType type) {
    size_t size = 0;
    switch(type) {
        #ifndef MAGNUM_TARGET_GLES
        case ComponentType::RGB332:
        case ComponentType::BGR233:
            return 1;
        #endif
        case ComponentType::RGB565:
        #ifndef MAGNUM_TARGET_GLES
        case ComponentType::BGR565:
        #endif
        case ComponentType::RGBA4:
        #ifndef MAGNUM_TARGET_GLES
        case ComponentType::ABGR4:
        #endif
        case ComponentType::RGB5Alpha1:
        #ifndef MAGNUM_TARGET_GLES
        case ComponentType::Alpha1BGR5:
        #endif
            return 2;
        #ifndef MAGNUM_TARGET_GLES
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
        #endif
        case ComponentType::UnsignedByte:
        case ComponentType::Byte:
            size = 1; break;
        case ComponentType::UnsignedShort:
        case ComponentType::Short:
        #ifndef MAGNUM_TARGET_GLES
        case ComponentType::HalfFloat:
            size = 2; break;
        #endif
        case ComponentType::UnsignedInt:
        case ComponentType::Int:
        case ComponentType::Float:
            size = 4; break;
    }

    switch(format) {
        #ifndef MAGNUM_TARGET_GLES
        case Components::Red:
            return 1*size;
        case Components::RedGreen:
            return 2*size;
        #endif
        case Components::RGB:
        #ifndef MAGNUM_TARGET_GLES
        case Components::BGR:
        #endif
            return 3*size;
        case Components::RGBA:
        #ifndef MAGNUM_TARGET_GLES
        case Components::BGRA:
        #endif
            return 4*size;
        default:
            return 0;
    }
}

}
