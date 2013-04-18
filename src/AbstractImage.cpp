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

#include "AbstractImage.h"

#include <Utility/Assert.h>

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
        #ifndef MAGNUM_TARGET_GLES3
        case Type::UnsignedShort4444Rev:
        #endif
        case Type::UnsignedShort5551:
        #ifndef MAGNUM_TARGET_GLES3
        case Type::UnsignedShort1555Rev:
        #endif
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
        #ifndef MAGNUM_TARGET_GLES3
        case Format::BGRA:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case Format::BGRAInteger:
        #endif
            return 4*size;

        /* Handled above */
        case Format::DepthComponent:
        #ifndef MAGNUM_TARGET_GLES3
        case Format::StencilIndex:
        #endif
        case Format::DepthStencil:
            CORRADE_ASSERT_UNREACHABLE();
    }

    CORRADE_ASSERT_UNREACHABLE();
    return 0;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, AbstractImage::Format value) {
    switch(value) {
        #define _c(value) case AbstractImage::Format::value: return debug << "AbstractImage::Format::" #value;
        _c(Red)
        #ifndef MAGNUM_TARGET_GLES
        _c(Green)
        _c(Blue)
        #endif
        _c(RG)
        _c(RGB)
        _c(RGBA)
        #ifndef MAGNUM_TARGET_GLES
        _c(BGR)
        #endif
        #ifndef MAGNUM_TARGET_GLES3
        _c(BGRA)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(RedInteger)
        #ifndef MAGNUM_TARGET_GLES
        _c(GreenInteger)
        _c(BlueInteger)
        #endif
        _c(RGInteger)
        _c(RGBInteger)
        _c(RGBAInteger)
        #ifndef MAGNUM_TARGET_GLES
        _c(BGRInteger)
        _c(BGRAInteger)
        #endif
        #endif
        _c(DepthComponent)
        #ifndef MAGNUM_TARGET_GLES3
        _c(StencilIndex)
        #endif
        _c(DepthStencil)
        #undef _c
    }

    return debug << "AbstractImage::Format::(invalid)";
}

Debug operator<<(Debug debug, AbstractImage::Type value) {
    switch(value) {
        #define _c(value) case AbstractImage::Type::value: return debug << "AbstractImage::Type::" #value;
        _c(UnsignedByte)
        #ifndef MAGNUM_TARGET_GLES2
        _c(Byte)
        #endif
        _c(UnsignedShort)
        #ifndef MAGNUM_TARGET_GLES2
        _c(Short)
        #endif
        _c(UnsignedInt)
        #ifndef MAGNUM_TARGET_GLES2
        _c(Int)
        #endif
        _c(HalfFloat)
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(UnsignedByte332)
        _c(UnsignedByte233Rev)
        #endif
        _c(UnsignedShort565)
        #ifndef MAGNUM_TARGET_GLES
        _c(UnsignedShort565Rev)
        #endif
        _c(UnsignedShort4444)
        #ifndef MAGNUM_TARGET_GLES3
        _c(UnsignedShort4444Rev)
        #endif
        _c(UnsignedShort5551)
        #ifndef MAGNUM_TARGET_GLES3
        _c(UnsignedShort1555Rev)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(UnsignedInt8888)
        _c(UnsignedInt8888Rev)
        _c(UnsignedInt1010102)
        #endif
        _c(UnsignedInt2101010Rev)
        #ifndef MAGNUM_TARGET_GLES2
        _c(UnsignedInt10F11F11FRev)
        _c(UnsignedInt5999Rev)
        #endif
        _c(UnsignedInt248)
        #ifndef MAGNUM_TARGET_GLES2
        _c(Float32UnsignedInt248Rev)
        #endif
        #undef _c
    }

    return debug << "AbstractImage::Type::(invalid)";
}
#endif

}
