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

#include "ImageFormat.h"

namespace Magnum {

std::size_t AbstractImage::pixelSize(ImageFormat format, ImageType type) {
    std::size_t size = 0;
    switch(type) {
        case ImageType::UnsignedByte:
        #ifndef MAGNUM_TARGET_GLES2
        case ImageType::Byte:
        #endif
            size = 1; break;
        case ImageType::UnsignedShort:
        #ifndef MAGNUM_TARGET_GLES2
        case ImageType::Short:
        #endif
        case ImageType::HalfFloat:
            size = 2; break;
        case ImageType::UnsignedInt:
        #ifndef MAGNUM_TARGET_GLES2
        case ImageType::Int:
        #endif
        case ImageType::Float:
            size = 4; break;

        #ifndef MAGNUM_TARGET_GLES
        case ImageType::UnsignedByte332:
        case ImageType::UnsignedByte233Rev:
            return 1;
        #endif
        case ImageType::UnsignedShort565:
        #ifndef MAGNUM_TARGET_GLES
        case ImageType::UnsignedShort565Rev:
        #endif
        case ImageType::UnsignedShort4444:
        #ifndef MAGNUM_TARGET_GLES3
        case ImageType::UnsignedShort4444Rev:
        #endif
        case ImageType::UnsignedShort5551:
        #ifndef MAGNUM_TARGET_GLES3
        case ImageType::UnsignedShort1555Rev:
        #endif
            return 2;
        #ifndef MAGNUM_TARGET_GLES
        case ImageType::UnsignedInt8888:
        case ImageType::UnsignedInt8888Rev:
        case ImageType::UnsignedInt1010102:
        #endif
        case ImageType::UnsignedInt2101010Rev:
        #ifndef MAGNUM_TARGET_GLES2
        case ImageType::UnsignedInt10F11F11FRev:
        case ImageType::UnsignedInt5999Rev:
        #endif
        case ImageType::UnsignedInt248:
            return 4;
        #ifndef MAGNUM_TARGET_GLES2
        case ImageType::Float32UnsignedInt248Rev:
            return 8;
        #endif
    }

    switch(format) {
        case ImageFormat::Red:
        #ifndef MAGNUM_TARGET_GLES2
        case ImageFormat::RedInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case ImageFormat::Green:
        case ImageFormat::Blue:
        case ImageFormat::GreenInteger:
        case ImageFormat::BlueInteger:
        #endif
            return 1*size;
        case ImageFormat::RG:
        #ifndef MAGNUM_TARGET_GLES2
        case ImageFormat::RGInteger:
        #endif
            return 2*size;
        case ImageFormat::RGB:
        #ifndef MAGNUM_TARGET_GLES2
        case ImageFormat::RGBInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case ImageFormat::BGR:
        case ImageFormat::BGRInteger:
        #endif
            return 3*size;
        case ImageFormat::RGBA:
        #ifndef MAGNUM_TARGET_GLES2
        case ImageFormat::RGBAInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES3
        case ImageFormat::BGRA:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case ImageFormat::BGRAInteger:
        #endif
            return 4*size;

        /* Handled above */
        case ImageFormat::DepthComponent:
        #ifndef MAGNUM_TARGET_GLES3
        case ImageFormat::StencilIndex:
        #endif
        case ImageFormat::DepthStencil:
            CORRADE_ASSERT_UNREACHABLE();
    }

    CORRADE_ASSERT_UNREACHABLE();
    return 0;
}

}
