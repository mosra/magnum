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

#include <Utility/Assert.h>

#include "TypeTraits.h"

namespace Magnum {

std::size_t AbstractImage::pixelSize(Format format, Type type) {
    std::size_t size = 0;
    switch(type) {
        case Type::UnsignedByte:
        #ifndef MAGNUM_TARGET_GLES2
        case Type::Byte:
        #endif
            size = 1; break;
        case Type::UnsignedShort:
        #ifndef MAGNUM_TARGET_GLES2
        case Type::Short:
        #endif
        case Type::HalfFloat:
            size = 2; break;
        case Type::UnsignedInt:
        #ifndef MAGNUM_TARGET_GLES2
        case Type::Int:
        #endif
        case Type::Float:
            size = 4; break;

        #ifndef MAGNUM_TARGET_GLES
        case Type::UnsignedByte332:
        case Type::UnsignedByte233Rev:
            return 1;
        #endif
        case Type::UnsignedShort565:
        #ifndef MAGNUM_TARGET_GLES
        case Type::UnsignedShort565Rev:
        #endif
        case Type::UnsignedShort4444:
        case Type::UnsignedShort4444Rev:
        case Type::UnsignedShort5551:
        case Type::UnsignedShort1555Rev:
            return 2;
        #ifndef MAGNUM_TARGET_GLES
        case Type::UnsignedInt8888:
        case Type::UnsignedInt8888Rev:
        case Type::UnsignedInt1010102:
        #endif
        case Type::UnsignedInt2101010Rev:
        #ifndef MAGNUM_TARGET_GLES2
        case Type::UnsignedInt10F11F11FRev:
        case Type::UnsignedInt5999Rev:
        #endif
        case Type::UnsignedInt248:
            return 4;
        #ifndef MAGNUM_TARGET_GLES2
        case Type::Float32UnsignedInt248Rev:
            return 8;
        #endif
    }

    switch(format) {
        case Format::Red:
        #ifndef MAGNUM_TARGET_GLES2
        case Format::RedInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case Format::Green:
        case Format::Blue:
        case Format::GreenInteger:
        case Format::BlueInteger:
        #endif
            return 1*size;
        case Format::RG:
        #ifndef MAGNUM_TARGET_GLES2
        case Format::RGInteger:
        #endif
            return 2*size;
        case Format::RGB:
        #ifndef MAGNUM_TARGET_GLES2
        case Format::RGBInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case Format::BGR:
        case Format::BGRInteger:
        #endif
            return 3*size;
        case Format::RGBA:
        #ifndef MAGNUM_TARGET_GLES2
        case Format::RGBAInteger:
        #endif
        case Format::BGRA:
        #ifndef MAGNUM_TARGET_GLES2
        case Format::BGRAInteger:
        #endif
            return 4*size;

        /* Handled above */
        case Format::DepthComponent:
        case Format::StencilIndex:
        case Format::DepthStencil:
            CORRADE_INTERNAL_ASSERT(false);
    }

    return 0;
}

}
