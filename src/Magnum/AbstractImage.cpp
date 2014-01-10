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

#include <Corrade/Utility/Assert.h>

#include "Magnum/ColorFormat.h"
#include "Magnum/Math/Vector.h"

namespace Magnum {

std::size_t AbstractImage::pixelSize(ColorFormat format, ColorType type) {
    std::size_t size = 0;
    switch(type) {
        case ColorType::UnsignedByte:
        #ifndef MAGNUM_TARGET_GLES2
        case ColorType::Byte:
        #endif
            size = 1; break;
        case ColorType::UnsignedShort:
        #ifndef MAGNUM_TARGET_GLES2
        case ColorType::Short:
        #endif
        case ColorType::HalfFloat:
            size = 2; break;
        case ColorType::UnsignedInt:
        #ifndef MAGNUM_TARGET_GLES2
        case ColorType::Int:
        #endif
        case ColorType::Float:
            size = 4; break;

        #ifndef MAGNUM_TARGET_GLES
        case ColorType::UnsignedByte332:
        case ColorType::UnsignedByte233Rev:
            return 1;
        #endif
        case ColorType::UnsignedShort565:
        #ifndef MAGNUM_TARGET_GLES
        case ColorType::UnsignedShort565Rev:
        #endif
        case ColorType::UnsignedShort4444:
        case ColorType::UnsignedShort4444Rev:
        case ColorType::UnsignedShort5551:
        case ColorType::UnsignedShort1555Rev:
            return 2;
        #ifndef MAGNUM_TARGET_GLES
        case ColorType::UnsignedInt8888:
        case ColorType::UnsignedInt8888Rev:
        case ColorType::UnsignedInt1010102:
        #endif
        case ColorType::UnsignedInt2101010Rev:
        #ifndef MAGNUM_TARGET_GLES2
        case ColorType::UnsignedInt10F11F11FRev:
        case ColorType::UnsignedInt5999Rev:
        #endif
        case ColorType::UnsignedInt248:
            return 4;
        #ifndef MAGNUM_TARGET_GLES2
        case ColorType::Float32UnsignedInt248Rev:
            return 8;
        #endif
    }

    switch(format) {
        case ColorFormat::Red:
        #ifndef MAGNUM_TARGET_GLES2
        case ColorFormat::RedInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case ColorFormat::Green:
        case ColorFormat::Blue:
        case ColorFormat::GreenInteger:
        case ColorFormat::BlueInteger:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case ColorFormat::Luminance:
        #endif
        case ColorFormat::DepthComponent:
        case ColorFormat::StencilIndex:
            return 1*size;
        case ColorFormat::RG:
        #ifndef MAGNUM_TARGET_GLES2
        case ColorFormat::RGInteger:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case ColorFormat::LuminanceAlpha:
        #endif
            return 2*size;
        case ColorFormat::RGB:
        #ifndef MAGNUM_TARGET_GLES2
        case ColorFormat::RGBInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case ColorFormat::BGR:
        case ColorFormat::BGRInteger:
        #endif
            return 3*size;
        case ColorFormat::RGBA:
        #ifndef MAGNUM_TARGET_GLES2
        case ColorFormat::RGBAInteger:
        #endif
        case ColorFormat::BGRA:
        #ifndef MAGNUM_TARGET_GLES
        case ColorFormat::BGRAInteger:
        #endif
            return 4*size;

        /* Handled above */
        case ColorFormat::DepthStencil:
            CORRADE_ASSERT(false, "AbstractImage::pixelSize(): invalid ColorType specified for depth/stencil ColorFormat", 0);
    }

    CORRADE_ASSERT_UNREACHABLE();
}

template<UnsignedInt dimensions> std::size_t AbstractImage::dataSize(Math::Vector<dimensions, Int> size) const {
    /** @todo Code this properly when all @fn_gl{PixelStore} parameters are implemented */
    /* Row size, rounded to multiple of 4 bytes */
    const std::size_t rowSize = ((size[0]*pixelSize() + 3)/4)*4;

    /** @todo Can't this be done somewhat nicer? */
    size[0] = 1;
    return rowSize*size.product();
}

template std::size_t AbstractImage::dataSize<1>(Math::Vector<1, Int>) const;
template std::size_t AbstractImage::dataSize<2>(Math::Vector<2, Int>) const;
template std::size_t AbstractImage::dataSize<3>(Math::Vector<3, Int>) const;

}
