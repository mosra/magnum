/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "PixelFormat.h"

#include <Corrade/Containers/String.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Math/Vector3.h"

namespace Magnum {

UnsignedInt pixelFormatSize(const PixelFormat format) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "pixelFormatSize(): can't determine size of an implementation-specific format" << Debug::hex << pixelFormatUnwrap(format), {});

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case PixelFormat::R8Unorm:
        case PixelFormat::R8Snorm:
        case PixelFormat::R8Srgb:
        case PixelFormat::R8UI:
        case PixelFormat::R8I:
        case PixelFormat::Stencil8UI:
            return 1;
        case PixelFormat::RG8Unorm:
        case PixelFormat::RG8Snorm:
        case PixelFormat::RG8Srgb:
        case PixelFormat::RG8UI:
        case PixelFormat::RG8I:
        case PixelFormat::R16Unorm:
        case PixelFormat::R16Snorm:
        case PixelFormat::R16UI:
        case PixelFormat::R16I:
        case PixelFormat::R16F:
        case PixelFormat::Depth16Unorm:
            return 2;
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGB8Snorm:
        case PixelFormat::RGB8Srgb:
        case PixelFormat::RGB8UI:
        case PixelFormat::RGB8I:
            return 3;
        case PixelFormat::RGBA8Unorm:
        case PixelFormat::RGBA8Snorm:
        case PixelFormat::RGBA8Srgb:
        case PixelFormat::RGBA8UI:
        case PixelFormat::RGBA8I:
        case PixelFormat::RG16Unorm:
        case PixelFormat::RG16Snorm:
        case PixelFormat::RG16UI:
        case PixelFormat::RG16I:
        case PixelFormat::RG16F:
        case PixelFormat::R32UI:
        case PixelFormat::R32I:
        case PixelFormat::R32F:
        case PixelFormat::Depth24Unorm:
        case PixelFormat::Depth32F:
        case PixelFormat::Depth16UnormStencil8UI:
        case PixelFormat::Depth24UnormStencil8UI:
            return 4;
        case PixelFormat::RGB16Unorm:
        case PixelFormat::RGB16Snorm:
        case PixelFormat::RGB16UI:
        case PixelFormat::RGB16I:
        case PixelFormat::RGB16F:
            return 6;
        case PixelFormat::RGBA16Unorm:
        case PixelFormat::RGBA16Snorm:
        case PixelFormat::RGBA16UI:
        case PixelFormat::RGBA16I:
        case PixelFormat::RGBA16F:
        case PixelFormat::RG32UI:
        case PixelFormat::RG32I:
        case PixelFormat::RG32F:
        case PixelFormat::Depth32FStencil8UI:
            return 8;
        case PixelFormat::RGB32UI:
        case PixelFormat::RGB32I:
        case PixelFormat::RGB32F:
            return 12;
        case PixelFormat::RGBA32UI:
        case PixelFormat::RGBA32I:
        case PixelFormat::RGBA32F:
            return 16;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("pixelFormatSize(): invalid format" << format, {});
}

PixelFormat pixelFormatChannelFormat(const PixelFormat format) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "pixelFormatChannelFormat(): can't determine channel format of an implementation-specific format" << Debug::hex << pixelFormatUnwrap(format), {});

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case PixelFormat::R8Unorm:
        case PixelFormat::RG8Unorm:
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGBA8Unorm:
            return PixelFormat::R8Unorm;
        case PixelFormat::R8Snorm:
        case PixelFormat::RG8Snorm:
        case PixelFormat::RGB8Snorm:
        case PixelFormat::RGBA8Snorm:
            return PixelFormat::R8Snorm;
        case PixelFormat::R8Srgb:
        case PixelFormat::RG8Srgb:
        case PixelFormat::RGB8Srgb:
        case PixelFormat::RGBA8Srgb:
            return PixelFormat::R8Srgb;
        case PixelFormat::R8UI:
        case PixelFormat::RG8UI:
        case PixelFormat::RGB8UI:
        case PixelFormat::RGBA8UI:
            return PixelFormat::R8UI;
        case PixelFormat::R8I:
        case PixelFormat::RG8I:
        case PixelFormat::RGB8I:
        case PixelFormat::RGBA8I:
            return PixelFormat::R8I;
        case PixelFormat::R16Unorm:
        case PixelFormat::RG16Unorm:
        case PixelFormat::RGB16Unorm:
        case PixelFormat::RGBA16Unorm:
            return PixelFormat::R16Unorm;
        case PixelFormat::R16Snorm:
        case PixelFormat::RG16Snorm:
        case PixelFormat::RGB16Snorm:
        case PixelFormat::RGBA16Snorm:
            return PixelFormat::R16Snorm;
        case PixelFormat::R16UI:
        case PixelFormat::RG16UI:
        case PixelFormat::RGB16UI:
        case PixelFormat::RGBA16UI:
            return PixelFormat::R16UI;
        case PixelFormat::R16I:
        case PixelFormat::RG16I:
        case PixelFormat::RGB16I:
        case PixelFormat::RGBA16I:
            return PixelFormat::R16I;
        case PixelFormat::R32UI:
        case PixelFormat::RG32UI:
        case PixelFormat::RGB32UI:
        case PixelFormat::RGBA32UI:
            return PixelFormat::R32UI;
        case PixelFormat::R32I:
        case PixelFormat::RG32I:
        case PixelFormat::RGB32I:
        case PixelFormat::RGBA32I:
            return PixelFormat::R32I;
        case PixelFormat::R16F:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
        case PixelFormat::RGBA16F:
            return PixelFormat::R16F;
        case PixelFormat::R32F:
        case PixelFormat::RG32F:
        case PixelFormat::RGB32F:
        case PixelFormat::RGBA32F:
            return PixelFormat::R32F;

        case PixelFormat::Stencil8UI:
        case PixelFormat::Depth16Unorm:
        case PixelFormat::Depth24Unorm:
        case PixelFormat::Depth32F:
        case PixelFormat::Depth16UnormStencil8UI:
        case PixelFormat::Depth24UnormStencil8UI:
        case PixelFormat::Depth32FStencil8UI:
            CORRADE_ASSERT_UNREACHABLE("pixelFormatChannelFormat(): can't determine channel format of" << format, {});
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("pixelFormatChannelFormat(): invalid format" << format, {});
}

UnsignedInt pixelFormatChannelCount(const PixelFormat format) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "pixelFormatChannelCount(): can't determine channel count of an implementation-specific format" << Debug::hex << pixelFormatUnwrap(format), {});

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case PixelFormat::R8Unorm:
        case PixelFormat::R8Snorm:
        case PixelFormat::R8Srgb:
        case PixelFormat::R8UI:
        case PixelFormat::R8I:
        case PixelFormat::R16Unorm:
        case PixelFormat::R16Snorm:
        case PixelFormat::R16UI:
        case PixelFormat::R16I:
        case PixelFormat::R32UI:
        case PixelFormat::R32I:
        case PixelFormat::R16F:
        case PixelFormat::R32F:
            return 1;
        case PixelFormat::RG8Unorm:
        case PixelFormat::RG8Snorm:
        case PixelFormat::RG8Srgb:
        case PixelFormat::RG8UI:
        case PixelFormat::RG8I:
        case PixelFormat::RG16Unorm:
        case PixelFormat::RG16Snorm:
        case PixelFormat::RG16UI:
        case PixelFormat::RG16I:
        case PixelFormat::RG32UI:
        case PixelFormat::RG32I:
        case PixelFormat::RG16F:
        case PixelFormat::RG32F:
            return 2;
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGB8Snorm:
        case PixelFormat::RGB8Srgb:
        case PixelFormat::RGB8UI:
        case PixelFormat::RGB8I:
        case PixelFormat::RGB16Unorm:
        case PixelFormat::RGB16Snorm:
        case PixelFormat::RGB16UI:
        case PixelFormat::RGB16I:
        case PixelFormat::RGB32UI:
        case PixelFormat::RGB32I:
        case PixelFormat::RGB16F:
        case PixelFormat::RGB32F:
            return 3;
        case PixelFormat::RGBA8Unorm:
        case PixelFormat::RGBA8Snorm:
        case PixelFormat::RGBA8Srgb:
        case PixelFormat::RGBA8UI:
        case PixelFormat::RGBA8I:
        case PixelFormat::RGBA16Unorm:
        case PixelFormat::RGBA16Snorm:
        case PixelFormat::RGBA16UI:
        case PixelFormat::RGBA16I:
        case PixelFormat::RGBA32UI:
        case PixelFormat::RGBA32I:
        case PixelFormat::RGBA16F:
        case PixelFormat::RGBA32F:
            return 4;
        case PixelFormat::Depth16Unorm:
        case PixelFormat::Depth24Unorm:
        case PixelFormat::Depth32F:
        case PixelFormat::Stencil8UI:
        case PixelFormat::Depth16UnormStencil8UI:
        case PixelFormat::Depth24UnormStencil8UI:
        case PixelFormat::Depth32FStencil8UI:
            CORRADE_ASSERT_UNREACHABLE("pixelFormatChannelCount(): can't determine channel count of" << format, {});
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("pixelFormatChannelCount(): invalid format" << format, {});
}

bool isPixelFormatNormalized(const PixelFormat format) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "isPixelFormatNormalized(): can't determine type of an implementation-specific format" << Debug::hex << pixelFormatUnwrap(format), {});

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case PixelFormat::R8Unorm:
        case PixelFormat::RG8Unorm:
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGBA8Unorm:
        case PixelFormat::R8Snorm:
        case PixelFormat::RG8Snorm:
        case PixelFormat::RGB8Snorm:
        case PixelFormat::RGBA8Snorm:
        case PixelFormat::R8Srgb:
        case PixelFormat::RG8Srgb:
        case PixelFormat::RGB8Srgb:
        case PixelFormat::RGBA8Srgb:
        case PixelFormat::R16Unorm:
        case PixelFormat::RG16Unorm:
        case PixelFormat::RGB16Unorm:
        case PixelFormat::RGBA16Unorm:
        case PixelFormat::R16Snorm:
        case PixelFormat::RG16Snorm:
        case PixelFormat::RGB16Snorm:
        case PixelFormat::RGBA16Snorm:
            return true;
        case PixelFormat::R8UI:
        case PixelFormat::RG8UI:
        case PixelFormat::RGB8UI:
        case PixelFormat::RGBA8UI:
        case PixelFormat::R8I:
        case PixelFormat::RG8I:
        case PixelFormat::RGB8I:
        case PixelFormat::RGBA8I:
        case PixelFormat::R16UI:
        case PixelFormat::RG16UI:
        case PixelFormat::RGB16UI:
        case PixelFormat::RGBA16UI:
        case PixelFormat::R16I:
        case PixelFormat::RG16I:
        case PixelFormat::RGB16I:
        case PixelFormat::RGBA16I:
        case PixelFormat::R32UI:
        case PixelFormat::RG32UI:
        case PixelFormat::RGB32UI:
        case PixelFormat::RGBA32UI:
        case PixelFormat::R32I:
        case PixelFormat::RG32I:
        case PixelFormat::RGB32I:
        case PixelFormat::RGBA32I:
        case PixelFormat::R16F:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
        case PixelFormat::RGBA16F:
        case PixelFormat::R32F:
        case PixelFormat::RG32F:
        case PixelFormat::RGB32F:
        case PixelFormat::RGBA32F:
            return false;
        case PixelFormat::Depth16Unorm:
        case PixelFormat::Depth24Unorm:
        case PixelFormat::Depth32F:
        case PixelFormat::Stencil8UI:
        case PixelFormat::Depth16UnormStencil8UI:
        case PixelFormat::Depth24UnormStencil8UI:
        case PixelFormat::Depth32FStencil8UI:
            CORRADE_ASSERT_UNREACHABLE("isPixelFormatNormalized(): can't determine type of" << format, {});
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("isPixelFormatNormalized(): invalid format" << format, {});
}

bool isPixelFormatIntegral(const PixelFormat format) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "isPixelFormatIntegral(): can't determine type of an implementation-specific format" << Debug::hex << pixelFormatUnwrap(format), {});

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case PixelFormat::R8UI:
        case PixelFormat::RG8UI:
        case PixelFormat::RGB8UI:
        case PixelFormat::RGBA8UI:
        case PixelFormat::R8I:
        case PixelFormat::RG8I:
        case PixelFormat::RGB8I:
        case PixelFormat::RGBA8I:
        case PixelFormat::R16UI:
        case PixelFormat::RG16UI:
        case PixelFormat::RGB16UI:
        case PixelFormat::RGBA16UI:
        case PixelFormat::R16I:
        case PixelFormat::RG16I:
        case PixelFormat::RGB16I:
        case PixelFormat::RGBA16I:
        case PixelFormat::R32UI:
        case PixelFormat::RG32UI:
        case PixelFormat::RGB32UI:
        case PixelFormat::RGBA32UI:
        case PixelFormat::R32I:
        case PixelFormat::RG32I:
        case PixelFormat::RGB32I:
        case PixelFormat::RGBA32I:
            return true;
        case PixelFormat::R8Unorm:
        case PixelFormat::RG8Unorm:
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGBA8Unorm:
        case PixelFormat::R8Snorm:
        case PixelFormat::RG8Snorm:
        case PixelFormat::RGB8Snorm:
        case PixelFormat::RGBA8Snorm:
        case PixelFormat::R8Srgb:
        case PixelFormat::RG8Srgb:
        case PixelFormat::RGB8Srgb:
        case PixelFormat::RGBA8Srgb:
        case PixelFormat::R16Unorm:
        case PixelFormat::RG16Unorm:
        case PixelFormat::RGB16Unorm:
        case PixelFormat::RGBA16Unorm:
        case PixelFormat::R16Snorm:
        case PixelFormat::RG16Snorm:
        case PixelFormat::RGB16Snorm:
        case PixelFormat::RGBA16Snorm:
        case PixelFormat::R16F:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
        case PixelFormat::RGBA16F:
        case PixelFormat::R32F:
        case PixelFormat::RG32F:
        case PixelFormat::RGB32F:
        case PixelFormat::RGBA32F:
            return false;
        case PixelFormat::Depth16Unorm:
        case PixelFormat::Depth24Unorm:
        case PixelFormat::Depth32F:
        case PixelFormat::Stencil8UI:
        case PixelFormat::Depth16UnormStencil8UI:
        case PixelFormat::Depth24UnormStencil8UI:
        case PixelFormat::Depth32FStencil8UI:
            CORRADE_ASSERT_UNREACHABLE("isPixelFormatIntegral(): can't determine type of" << format, {});
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("isPixelFormatIntegral(): invalid format" << format, {});
}

bool isPixelFormatFloatingPoint(const PixelFormat format) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "isPixelFormatFloatingPoint(): can't determine type of an implementation-specific format" << Debug::hex << pixelFormatUnwrap(format), {});

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case PixelFormat::R16F:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
        case PixelFormat::RGBA16F:
        case PixelFormat::R32F:
        case PixelFormat::RG32F:
        case PixelFormat::RGB32F:
        case PixelFormat::RGBA32F:
            return true;
        case PixelFormat::R8Unorm:
        case PixelFormat::RG8Unorm:
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGBA8Unorm:
        case PixelFormat::R8Snorm:
        case PixelFormat::RG8Snorm:
        case PixelFormat::RGB8Snorm:
        case PixelFormat::RGBA8Snorm:
        case PixelFormat::R8Srgb:
        case PixelFormat::RG8Srgb:
        case PixelFormat::RGB8Srgb:
        case PixelFormat::RGBA8Srgb:
        case PixelFormat::R8UI:
        case PixelFormat::RG8UI:
        case PixelFormat::RGB8UI:
        case PixelFormat::RGBA8UI:
        case PixelFormat::R8I:
        case PixelFormat::RG8I:
        case PixelFormat::RGB8I:
        case PixelFormat::RGBA8I:
        case PixelFormat::R16Unorm:
        case PixelFormat::RG16Unorm:
        case PixelFormat::RGB16Unorm:
        case PixelFormat::RGBA16Unorm:
        case PixelFormat::R16Snorm:
        case PixelFormat::RG16Snorm:
        case PixelFormat::RGB16Snorm:
        case PixelFormat::RGBA16Snorm:
        case PixelFormat::R16UI:
        case PixelFormat::RG16UI:
        case PixelFormat::RGB16UI:
        case PixelFormat::RGBA16UI:
        case PixelFormat::R16I:
        case PixelFormat::RG16I:
        case PixelFormat::RGB16I:
        case PixelFormat::RGBA16I:
        case PixelFormat::R32UI:
        case PixelFormat::RG32UI:
        case PixelFormat::RGB32UI:
        case PixelFormat::RGBA32UI:
        case PixelFormat::R32I:
        case PixelFormat::RG32I:
        case PixelFormat::RGB32I:
        case PixelFormat::RGBA32I:
            return false;
        case PixelFormat::Depth16Unorm:
        case PixelFormat::Depth24Unorm:
        case PixelFormat::Depth32F:
        case PixelFormat::Stencil8UI:
        case PixelFormat::Depth16UnormStencil8UI:
        case PixelFormat::Depth24UnormStencil8UI:
        case PixelFormat::Depth32FStencil8UI:
            CORRADE_ASSERT_UNREACHABLE("isPixelFormatFloatingPoint(): can't determine type of" << format, {});
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("isPixelFormatFloatingPoint(): invalid format" << format, {});
}

bool isPixelFormatSrgb(const PixelFormat format) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "isPixelFormatSrgb(): can't determine colorspace of an implementation-specific format" << Debug::hex << pixelFormatUnwrap(format), {});

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case PixelFormat::R8Srgb:
        case PixelFormat::RG8Srgb:
        case PixelFormat::RGB8Srgb:
        case PixelFormat::RGBA8Srgb:
            return true;
        case PixelFormat::R8Unorm:
        case PixelFormat::RG8Unorm:
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGBA8Unorm:
        case PixelFormat::R8Snorm:
        case PixelFormat::RG8Snorm:
        case PixelFormat::RGB8Snorm:
        case PixelFormat::RGBA8Snorm:
        case PixelFormat::R8UI:
        case PixelFormat::RG8UI:
        case PixelFormat::RGB8UI:
        case PixelFormat::RGBA8UI:
        case PixelFormat::R8I:
        case PixelFormat::RG8I:
        case PixelFormat::RGB8I:
        case PixelFormat::RGBA8I:
        case PixelFormat::R16Unorm:
        case PixelFormat::RG16Unorm:
        case PixelFormat::RGB16Unorm:
        case PixelFormat::RGBA16Unorm:
        case PixelFormat::R16Snorm:
        case PixelFormat::RG16Snorm:
        case PixelFormat::RGB16Snorm:
        case PixelFormat::RGBA16Snorm:
        case PixelFormat::R16UI:
        case PixelFormat::RG16UI:
        case PixelFormat::RGB16UI:
        case PixelFormat::RGBA16UI:
        case PixelFormat::R16I:
        case PixelFormat::RG16I:
        case PixelFormat::RGB16I:
        case PixelFormat::RGBA16I:
        case PixelFormat::R32UI:
        case PixelFormat::RG32UI:
        case PixelFormat::RGB32UI:
        case PixelFormat::RGBA32UI:
        case PixelFormat::R32I:
        case PixelFormat::RG32I:
        case PixelFormat::RGB32I:
        case PixelFormat::RGBA32I:
        case PixelFormat::R16F:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
        case PixelFormat::RGBA16F:
        case PixelFormat::R32F:
        case PixelFormat::RG32F:
        case PixelFormat::RGB32F:
        case PixelFormat::RGBA32F:
            return false;
        case PixelFormat::Depth16Unorm:
        case PixelFormat::Depth24Unorm:
        case PixelFormat::Depth32F:
        case PixelFormat::Stencil8UI:
        case PixelFormat::Depth16UnormStencil8UI:
        case PixelFormat::Depth24UnormStencil8UI:
        case PixelFormat::Depth32FStencil8UI:
            CORRADE_ASSERT_UNREACHABLE("isPixelFormatSrgb(): can't determine colorspace of" << format, {});
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("isPixelFormatSrgb(): invalid format" << format, {});
}

bool isPixelFormatDepthOrStencil(const PixelFormat format) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "isPixelFormatDepthOrStencil(): can't determine type of an implementation-specific format" << Debug::hex << pixelFormatUnwrap(format), {});

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case PixelFormat::R8Unorm:
        case PixelFormat::RG8Unorm:
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGBA8Unorm:
        case PixelFormat::R8Snorm:
        case PixelFormat::RG8Snorm:
        case PixelFormat::RGB8Snorm:
        case PixelFormat::RGBA8Snorm:
        case PixelFormat::R8Srgb:
        case PixelFormat::RG8Srgb:
        case PixelFormat::RGB8Srgb:
        case PixelFormat::RGBA8Srgb:
        case PixelFormat::R8UI:
        case PixelFormat::RG8UI:
        case PixelFormat::RGB8UI:
        case PixelFormat::RGBA8UI:
        case PixelFormat::R8I:
        case PixelFormat::RG8I:
        case PixelFormat::RGB8I:
        case PixelFormat::RGBA8I:
        case PixelFormat::R16Unorm:
        case PixelFormat::RG16Unorm:
        case PixelFormat::RGB16Unorm:
        case PixelFormat::RGBA16Unorm:
        case PixelFormat::R16Snorm:
        case PixelFormat::RG16Snorm:
        case PixelFormat::RGB16Snorm:
        case PixelFormat::RGBA16Snorm:
        case PixelFormat::R16UI:
        case PixelFormat::RG16UI:
        case PixelFormat::RGB16UI:
        case PixelFormat::RGBA16UI:
        case PixelFormat::R16I:
        case PixelFormat::RG16I:
        case PixelFormat::RGB16I:
        case PixelFormat::RGBA16I:
        case PixelFormat::R32UI:
        case PixelFormat::RG32UI:
        case PixelFormat::RGB32UI:
        case PixelFormat::RGBA32UI:
        case PixelFormat::R32I:
        case PixelFormat::RG32I:
        case PixelFormat::RGB32I:
        case PixelFormat::RGBA32I:
        case PixelFormat::R16F:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
        case PixelFormat::RGBA16F:
        case PixelFormat::R32F:
        case PixelFormat::RG32F:
        case PixelFormat::RGB32F:
        case PixelFormat::RGBA32F:
            return false;
        case PixelFormat::Depth16Unorm:
        case PixelFormat::Depth24Unorm:
        case PixelFormat::Depth32F:
        case PixelFormat::Stencil8UI:
        case PixelFormat::Depth16UnormStencil8UI:
        case PixelFormat::Depth24UnormStencil8UI:
        case PixelFormat::Depth32FStencil8UI:
            return true;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("isPixelFormatDepthOrStencil(): invalid format" << format, {});
}

PixelFormat pixelFormat(const PixelFormat format, const UnsignedInt channelCount, const bool srgb) {
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(format),
        "pixelFormat(): can't assemble a format out of an implementation-specific format" << Debug::hex << pixelFormatUnwrap(format), {});
    CORRADE_ASSERT(!isPixelFormatDepthOrStencil(format),
        "pixelFormat(): can't assemble a format out of" << format, {});

    PixelFormat channelFormat = pixelFormatChannelFormat(format);

    /* First turn the format into a sRGB one or remove the sRGB property, if
       requested. The [RGBA]8Srgb formats follow [RGBA]8Unorm in the same order
       so it's just constant addition / subtraction for all four variants. */
    if(srgb && channelFormat != PixelFormat::R8Srgb) {
        CORRADE_ASSERT(channelFormat == PixelFormat::R8Unorm,
            "pixelFormat():" << format << "can't be made sRGB", {});
        channelFormat = PixelFormat(UnsignedInt(channelFormat) - UnsignedInt(PixelFormat::R8Unorm) + UnsignedInt(PixelFormat::R8Srgb));
    } else if(!srgb && channelFormat == PixelFormat::R8Srgb) {
        channelFormat = PixelFormat(UnsignedInt(channelFormat) - UnsignedInt(PixelFormat::R8Srgb) + UnsignedInt(PixelFormat::R8Unorm));
    }

    CORRADE_ASSERT(channelCount >= 1 && channelCount <= 4,
        "pixelFormat(): invalid component count" << channelCount, {});

    /* The two-, three- and four-channel variants follow each other, so it's
       just addition again. There may be packed formats in the future, so
       whitelist for the known set of single-channel formats. */
    if(channelFormat == PixelFormat::R8Unorm ||
       channelFormat == PixelFormat::R8Snorm ||
       channelFormat == PixelFormat::R8Srgb ||
       channelFormat == PixelFormat::R8UI ||
       channelFormat == PixelFormat::R8I ||
       channelFormat == PixelFormat::R16Unorm ||
       channelFormat == PixelFormat::R16Snorm ||
       channelFormat == PixelFormat::R16UI ||
       channelFormat == PixelFormat::R16I ||
       channelFormat == PixelFormat::R32UI ||
       channelFormat == PixelFormat::R32I ||
       channelFormat == PixelFormat::R16F ||
       channelFormat == PixelFormat::R32F)
        return PixelFormat(UnsignedInt(channelFormat) + channelCount - 1);

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

namespace {

constexpr const char* PixelFormatNames[] {
    #define _c(format) #format,
    #include "Magnum/Implementation/pixelFormatMapping.hpp"
    #undef _c
};

}

Debug& operator<<(Debug& debug, const PixelFormat value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "PixelFormat" << Debug::nospace;

    if(isPixelFormatImplementationSpecific(value)) {
        return debug << (packed ? "ImplementationSpecific(" : "::ImplementationSpecific(") << Debug::nospace << Debug::hex << pixelFormatUnwrap(value) << Debug::nospace << ")";
    }

    if(UnsignedInt(value) - 1 < Containers::arraySize(PixelFormatNames)) {
        return debug << (packed ? "" : "::") << Debug::nospace << PixelFormatNames[UnsignedInt(value) - 1];
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << (packed ? "" : ")");
}

namespace {

constexpr UnsignedShort CompressedBlockData[] {
    /* Assuming w/h/d/s is never larger than 16 (and never zero), each number
       has 1 subtracted and packed into four bits, 16 bits in total. The size
       is supplied in bits, so first divide by eight and then subtract 1. For
       the currently ~100 supported formats that makes this table to be about
       256 bytes.*/
    #define _c(format, width, height, depth, size) \
        ((width - 1) << 12) | \
        ((height - 1) << 8) | \
        ((depth - 1) << 4) | \
        ((size >> 3) - 1),
    #include "Magnum/Implementation/compressedPixelFormatMapping.hpp"
    #undef _c
};

}

Vector3i compressedPixelFormatBlockSize(const CompressedPixelFormat format) {
    CORRADE_ASSERT(!(UnsignedInt(format) & (1 << 31)),
        "compressedPixelFormatBlockSize(): can't determine size of an implementation-specific format" << Debug::hex << compressedPixelFormatUnwrap(format), {});

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(CompressedBlockData),
        "compressedPixelFormatBlockSize(): invalid format" << format, {});
    const UnsignedInt data = CompressedBlockData[UnsignedInt(format) - 1];
    return {
        (Int(data >> 12) & 0xf) + 1,
        (Int(data >>  8) & 0xf) + 1,
        (Int(data >>  4) & 0xf) + 1,
    };
}

#ifdef MAGNUM_BUILD_DEPRECATED
Vector3i compressedBlockSize(const CompressedPixelFormat format) {
    return compressedPixelFormatBlockSize(format);
}
#endif

UnsignedInt compressedPixelFormatBlockDataSize(const CompressedPixelFormat format) {
    CORRADE_ASSERT(!(UnsignedInt(format) & (1 << 31)),
        "compressedPixelFormatBlockDataSize(): can't determine size of an implementation-specific format" << Debug::hex << compressedPixelFormatUnwrap(format), {});

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(CompressedBlockData),
        "compressedPixelFormatBlockDataSize(): invalid format" << format, {});
    return (CompressedBlockData[UnsignedInt(format) - 1] & 0xf) + 1;
}

bool isCompressedPixelFormatNormalized(const CompressedPixelFormat format) {
    CORRADE_ASSERT(!isCompressedPixelFormatImplementationSpecific(format),
        "isCompressedPixelFormatNormalized(): can't determine type of an implementation-specific format" << Debug::hex << compressedPixelFormatUnwrap(format), {});

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case CompressedPixelFormat::Bc1RGBUnorm:
        case CompressedPixelFormat::Bc1RGBSrgb:
        case CompressedPixelFormat::Bc1RGBAUnorm:
        case CompressedPixelFormat::Bc1RGBASrgb:
        case CompressedPixelFormat::Bc2RGBAUnorm:
        case CompressedPixelFormat::Bc2RGBASrgb:
        case CompressedPixelFormat::Bc3RGBAUnorm:
        case CompressedPixelFormat::Bc3RGBASrgb:
        case CompressedPixelFormat::Bc4RUnorm:
        case CompressedPixelFormat::Bc4RSnorm:
        case CompressedPixelFormat::Bc5RGUnorm:
        case CompressedPixelFormat::Bc5RGSnorm:
        case CompressedPixelFormat::Bc7RGBAUnorm:
        case CompressedPixelFormat::Bc7RGBASrgb:
        case CompressedPixelFormat::EacR11Unorm:
        case CompressedPixelFormat::EacR11Snorm:
        case CompressedPixelFormat::EacRG11Unorm:
        case CompressedPixelFormat::EacRG11Snorm:
        case CompressedPixelFormat::Etc2RGB8Unorm:
        case CompressedPixelFormat::Etc2RGB8Srgb:
        case CompressedPixelFormat::Etc2RGB8A1Unorm:
        case CompressedPixelFormat::Etc2RGB8A1Srgb:
        case CompressedPixelFormat::Etc2RGBA8Unorm:
        case CompressedPixelFormat::Etc2RGBA8Srgb:
        case CompressedPixelFormat::Astc4x4RGBAUnorm:
        case CompressedPixelFormat::Astc4x4RGBASrgb:
        case CompressedPixelFormat::Astc5x4RGBAUnorm:
        case CompressedPixelFormat::Astc5x4RGBASrgb:
        case CompressedPixelFormat::Astc5x5RGBAUnorm:
        case CompressedPixelFormat::Astc5x5RGBASrgb:
        case CompressedPixelFormat::Astc6x5RGBAUnorm:
        case CompressedPixelFormat::Astc6x5RGBASrgb:
        case CompressedPixelFormat::Astc6x6RGBAUnorm:
        case CompressedPixelFormat::Astc6x6RGBASrgb:
        case CompressedPixelFormat::Astc8x5RGBAUnorm:
        case CompressedPixelFormat::Astc8x5RGBASrgb:
        case CompressedPixelFormat::Astc8x6RGBAUnorm:
        case CompressedPixelFormat::Astc8x6RGBASrgb:
        case CompressedPixelFormat::Astc8x8RGBAUnorm:
        case CompressedPixelFormat::Astc8x8RGBASrgb:
        case CompressedPixelFormat::Astc10x5RGBAUnorm:
        case CompressedPixelFormat::Astc10x5RGBASrgb:
        case CompressedPixelFormat::Astc10x6RGBAUnorm:
        case CompressedPixelFormat::Astc10x6RGBASrgb:
        case CompressedPixelFormat::Astc10x8RGBAUnorm:
        case CompressedPixelFormat::Astc10x8RGBASrgb:
        case CompressedPixelFormat::Astc10x10RGBAUnorm:
        case CompressedPixelFormat::Astc10x10RGBASrgb:
        case CompressedPixelFormat::Astc12x10RGBAUnorm:
        case CompressedPixelFormat::Astc12x10RGBASrgb:
        case CompressedPixelFormat::Astc12x12RGBAUnorm:
        case CompressedPixelFormat::Astc12x12RGBASrgb:
        case CompressedPixelFormat::Astc3x3x3RGBAUnorm:
        case CompressedPixelFormat::Astc3x3x3RGBASrgb:
        case CompressedPixelFormat::Astc4x3x3RGBAUnorm:
        case CompressedPixelFormat::Astc4x3x3RGBASrgb:
        case CompressedPixelFormat::Astc4x4x3RGBAUnorm:
        case CompressedPixelFormat::Astc4x4x3RGBASrgb:
        case CompressedPixelFormat::Astc4x4x4RGBAUnorm:
        case CompressedPixelFormat::Astc4x4x4RGBASrgb:
        case CompressedPixelFormat::Astc5x4x4RGBAUnorm:
        case CompressedPixelFormat::Astc5x4x4RGBASrgb:
        case CompressedPixelFormat::Astc5x5x4RGBAUnorm:
        case CompressedPixelFormat::Astc5x5x4RGBASrgb:
        case CompressedPixelFormat::Astc5x5x5RGBAUnorm:
        case CompressedPixelFormat::Astc5x5x5RGBASrgb:
        case CompressedPixelFormat::Astc6x5x5RGBAUnorm:
        case CompressedPixelFormat::Astc6x5x5RGBASrgb:
        case CompressedPixelFormat::Astc6x6x5RGBAUnorm:
        case CompressedPixelFormat::Astc6x6x5RGBASrgb:
        case CompressedPixelFormat::Astc6x6x6RGBAUnorm:
        case CompressedPixelFormat::Astc6x6x6RGBASrgb:
        case CompressedPixelFormat::PvrtcRGB2bppUnorm:
        case CompressedPixelFormat::PvrtcRGB2bppSrgb:
        case CompressedPixelFormat::PvrtcRGBA2bppUnorm:
        case CompressedPixelFormat::PvrtcRGBA2bppSrgb:
        case CompressedPixelFormat::PvrtcRGB4bppUnorm:
        case CompressedPixelFormat::PvrtcRGB4bppSrgb:
        case CompressedPixelFormat::PvrtcRGBA4bppUnorm:
        case CompressedPixelFormat::PvrtcRGBA4bppSrgb:
            return true;
        case CompressedPixelFormat::Bc6hRGBUfloat:
        case CompressedPixelFormat::Bc6hRGBSfloat:
        case CompressedPixelFormat::Astc4x4RGBAF:
        case CompressedPixelFormat::Astc5x4RGBAF:
        case CompressedPixelFormat::Astc5x5RGBAF:
        case CompressedPixelFormat::Astc6x5RGBAF:
        case CompressedPixelFormat::Astc6x6RGBAF:
        case CompressedPixelFormat::Astc8x5RGBAF:
        case CompressedPixelFormat::Astc8x6RGBAF:
        case CompressedPixelFormat::Astc8x8RGBAF:
        case CompressedPixelFormat::Astc10x5RGBAF:
        case CompressedPixelFormat::Astc10x6RGBAF:
        case CompressedPixelFormat::Astc10x8RGBAF:
        case CompressedPixelFormat::Astc10x10RGBAF:
        case CompressedPixelFormat::Astc12x10RGBAF:
        case CompressedPixelFormat::Astc12x12RGBAF:
        case CompressedPixelFormat::Astc3x3x3RGBAF:
        case CompressedPixelFormat::Astc4x3x3RGBAF:
        case CompressedPixelFormat::Astc4x4x3RGBAF:
        case CompressedPixelFormat::Astc4x4x4RGBAF:
        case CompressedPixelFormat::Astc5x4x4RGBAF:
        case CompressedPixelFormat::Astc5x5x4RGBAF:
        case CompressedPixelFormat::Astc5x5x5RGBAF:
        case CompressedPixelFormat::Astc6x5x5RGBAF:
        case CompressedPixelFormat::Astc6x6x5RGBAF:
        case CompressedPixelFormat::Astc6x6x6RGBAF:
            return false;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("isCompressedPixelFormatNormalized(): invalid format" << format, {});
}

bool isCompressedPixelFormatFloatingPoint(const CompressedPixelFormat format) {
    CORRADE_ASSERT(!isCompressedPixelFormatImplementationSpecific(format),
        "isCompressedPixelFormatFloatingPoint(): can't determine type of an implementation-specific format" << Debug::hex << compressedPixelFormatUnwrap(format), {});

    /* Yes, this is currently the exact inverse of
       isCompressedPixelFormatNormalized(), so one function could call the
       other and negate the result. But keeping it this way in case there's
       some future integer CompressedPixelFormat, which would be false in both
       and which would need a new isCompressedPixelFormatIntegral(). */

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case CompressedPixelFormat::Bc6hRGBUfloat:
        case CompressedPixelFormat::Bc6hRGBSfloat:
        case CompressedPixelFormat::Astc4x4RGBAF:
        case CompressedPixelFormat::Astc5x4RGBAF:
        case CompressedPixelFormat::Astc5x5RGBAF:
        case CompressedPixelFormat::Astc6x5RGBAF:
        case CompressedPixelFormat::Astc6x6RGBAF:
        case CompressedPixelFormat::Astc8x5RGBAF:
        case CompressedPixelFormat::Astc8x6RGBAF:
        case CompressedPixelFormat::Astc8x8RGBAF:
        case CompressedPixelFormat::Astc10x5RGBAF:
        case CompressedPixelFormat::Astc10x6RGBAF:
        case CompressedPixelFormat::Astc10x8RGBAF:
        case CompressedPixelFormat::Astc10x10RGBAF:
        case CompressedPixelFormat::Astc12x10RGBAF:
        case CompressedPixelFormat::Astc12x12RGBAF:
        case CompressedPixelFormat::Astc3x3x3RGBAF:
        case CompressedPixelFormat::Astc4x3x3RGBAF:
        case CompressedPixelFormat::Astc4x4x3RGBAF:
        case CompressedPixelFormat::Astc4x4x4RGBAF:
        case CompressedPixelFormat::Astc5x4x4RGBAF:
        case CompressedPixelFormat::Astc5x5x4RGBAF:
        case CompressedPixelFormat::Astc5x5x5RGBAF:
        case CompressedPixelFormat::Astc6x5x5RGBAF:
        case CompressedPixelFormat::Astc6x6x5RGBAF:
        case CompressedPixelFormat::Astc6x6x6RGBAF:
            return true;
        case CompressedPixelFormat::Bc1RGBUnorm:
        case CompressedPixelFormat::Bc1RGBSrgb:
        case CompressedPixelFormat::Bc1RGBAUnorm:
        case CompressedPixelFormat::Bc1RGBASrgb:
        case CompressedPixelFormat::Bc2RGBAUnorm:
        case CompressedPixelFormat::Bc2RGBASrgb:
        case CompressedPixelFormat::Bc3RGBAUnorm:
        case CompressedPixelFormat::Bc3RGBASrgb:
        case CompressedPixelFormat::Bc4RUnorm:
        case CompressedPixelFormat::Bc4RSnorm:
        case CompressedPixelFormat::Bc5RGUnorm:
        case CompressedPixelFormat::Bc5RGSnorm:
        case CompressedPixelFormat::Bc7RGBAUnorm:
        case CompressedPixelFormat::Bc7RGBASrgb:
        case CompressedPixelFormat::EacR11Unorm:
        case CompressedPixelFormat::EacR11Snorm:
        case CompressedPixelFormat::EacRG11Unorm:
        case CompressedPixelFormat::EacRG11Snorm:
        case CompressedPixelFormat::Etc2RGB8Unorm:
        case CompressedPixelFormat::Etc2RGB8Srgb:
        case CompressedPixelFormat::Etc2RGB8A1Unorm:
        case CompressedPixelFormat::Etc2RGB8A1Srgb:
        case CompressedPixelFormat::Etc2RGBA8Unorm:
        case CompressedPixelFormat::Etc2RGBA8Srgb:
        case CompressedPixelFormat::Astc4x4RGBAUnorm:
        case CompressedPixelFormat::Astc4x4RGBASrgb:
        case CompressedPixelFormat::Astc5x4RGBAUnorm:
        case CompressedPixelFormat::Astc5x4RGBASrgb:
        case CompressedPixelFormat::Astc5x5RGBAUnorm:
        case CompressedPixelFormat::Astc5x5RGBASrgb:
        case CompressedPixelFormat::Astc6x5RGBAUnorm:
        case CompressedPixelFormat::Astc6x5RGBASrgb:
        case CompressedPixelFormat::Astc6x6RGBAUnorm:
        case CompressedPixelFormat::Astc6x6RGBASrgb:
        case CompressedPixelFormat::Astc8x5RGBAUnorm:
        case CompressedPixelFormat::Astc8x5RGBASrgb:
        case CompressedPixelFormat::Astc8x6RGBAUnorm:
        case CompressedPixelFormat::Astc8x6RGBASrgb:
        case CompressedPixelFormat::Astc8x8RGBAUnorm:
        case CompressedPixelFormat::Astc8x8RGBASrgb:
        case CompressedPixelFormat::Astc10x5RGBAUnorm:
        case CompressedPixelFormat::Astc10x5RGBASrgb:
        case CompressedPixelFormat::Astc10x6RGBAUnorm:
        case CompressedPixelFormat::Astc10x6RGBASrgb:
        case CompressedPixelFormat::Astc10x8RGBAUnorm:
        case CompressedPixelFormat::Astc10x8RGBASrgb:
        case CompressedPixelFormat::Astc10x10RGBAUnorm:
        case CompressedPixelFormat::Astc10x10RGBASrgb:
        case CompressedPixelFormat::Astc12x10RGBAUnorm:
        case CompressedPixelFormat::Astc12x10RGBASrgb:
        case CompressedPixelFormat::Astc12x12RGBAUnorm:
        case CompressedPixelFormat::Astc12x12RGBASrgb:
        case CompressedPixelFormat::Astc3x3x3RGBAUnorm:
        case CompressedPixelFormat::Astc3x3x3RGBASrgb:
        case CompressedPixelFormat::Astc4x3x3RGBAUnorm:
        case CompressedPixelFormat::Astc4x3x3RGBASrgb:
        case CompressedPixelFormat::Astc4x4x3RGBAUnorm:
        case CompressedPixelFormat::Astc4x4x3RGBASrgb:
        case CompressedPixelFormat::Astc4x4x4RGBAUnorm:
        case CompressedPixelFormat::Astc4x4x4RGBASrgb:
        case CompressedPixelFormat::Astc5x4x4RGBAUnorm:
        case CompressedPixelFormat::Astc5x4x4RGBASrgb:
        case CompressedPixelFormat::Astc5x5x4RGBAUnorm:
        case CompressedPixelFormat::Astc5x5x4RGBASrgb:
        case CompressedPixelFormat::Astc5x5x5RGBAUnorm:
        case CompressedPixelFormat::Astc5x5x5RGBASrgb:
        case CompressedPixelFormat::Astc6x5x5RGBAUnorm:
        case CompressedPixelFormat::Astc6x5x5RGBASrgb:
        case CompressedPixelFormat::Astc6x6x5RGBAUnorm:
        case CompressedPixelFormat::Astc6x6x5RGBASrgb:
        case CompressedPixelFormat::Astc6x6x6RGBAUnorm:
        case CompressedPixelFormat::Astc6x6x6RGBASrgb:
        case CompressedPixelFormat::PvrtcRGB2bppUnorm:
        case CompressedPixelFormat::PvrtcRGB2bppSrgb:
        case CompressedPixelFormat::PvrtcRGBA2bppUnorm:
        case CompressedPixelFormat::PvrtcRGBA2bppSrgb:
        case CompressedPixelFormat::PvrtcRGB4bppUnorm:
        case CompressedPixelFormat::PvrtcRGB4bppSrgb:
        case CompressedPixelFormat::PvrtcRGBA4bppUnorm:
        case CompressedPixelFormat::PvrtcRGBA4bppSrgb:
            return false;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("isCompressedPixelFormatFloatingPoint(): invalid format" << format, {});
}

bool isCompressedPixelFormatSrgb(const CompressedPixelFormat format) {
    CORRADE_ASSERT(!isCompressedPixelFormatImplementationSpecific(format),
        "isCompressedPixelFormatSrgb(): can't determine colorspace of an implementation-specific format" << Debug::hex << compressedPixelFormatUnwrap(format), {});

    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        case CompressedPixelFormat::Bc1RGBSrgb:
        case CompressedPixelFormat::Bc1RGBASrgb:
        case CompressedPixelFormat::Bc2RGBASrgb:
        case CompressedPixelFormat::Bc3RGBASrgb:
        case CompressedPixelFormat::Bc7RGBASrgb:
        case CompressedPixelFormat::Etc2RGB8Srgb:
        case CompressedPixelFormat::Etc2RGB8A1Srgb:
        case CompressedPixelFormat::Etc2RGBA8Srgb:
        case CompressedPixelFormat::Astc4x4RGBASrgb:
        case CompressedPixelFormat::Astc5x4RGBASrgb:
        case CompressedPixelFormat::Astc5x5RGBASrgb:
        case CompressedPixelFormat::Astc6x5RGBASrgb:
        case CompressedPixelFormat::Astc6x6RGBASrgb:
        case CompressedPixelFormat::Astc8x5RGBASrgb:
        case CompressedPixelFormat::Astc8x6RGBASrgb:
        case CompressedPixelFormat::Astc8x8RGBASrgb:
        case CompressedPixelFormat::Astc10x5RGBASrgb:
        case CompressedPixelFormat::Astc10x6RGBASrgb:
        case CompressedPixelFormat::Astc10x8RGBASrgb:
        case CompressedPixelFormat::Astc10x10RGBASrgb:
        case CompressedPixelFormat::Astc12x10RGBASrgb:
        case CompressedPixelFormat::Astc12x12RGBASrgb:
        case CompressedPixelFormat::Astc3x3x3RGBASrgb:
        case CompressedPixelFormat::Astc4x3x3RGBASrgb:
        case CompressedPixelFormat::Astc4x4x3RGBASrgb:
        case CompressedPixelFormat::Astc4x4x4RGBASrgb:
        case CompressedPixelFormat::Astc5x4x4RGBASrgb:
        case CompressedPixelFormat::Astc5x5x4RGBASrgb:
        case CompressedPixelFormat::Astc5x5x5RGBASrgb:
        case CompressedPixelFormat::Astc6x5x5RGBASrgb:
        case CompressedPixelFormat::Astc6x6x5RGBASrgb:
        case CompressedPixelFormat::Astc6x6x6RGBASrgb:
        case CompressedPixelFormat::PvrtcRGB2bppSrgb:
        case CompressedPixelFormat::PvrtcRGBA2bppSrgb:
        case CompressedPixelFormat::PvrtcRGB4bppSrgb:
        case CompressedPixelFormat::PvrtcRGBA4bppSrgb:
            return true;
        case CompressedPixelFormat::Bc1RGBUnorm:
        case CompressedPixelFormat::Bc1RGBAUnorm:
        case CompressedPixelFormat::Bc2RGBAUnorm:
        case CompressedPixelFormat::Bc3RGBAUnorm:
        case CompressedPixelFormat::Bc4RUnorm:
        case CompressedPixelFormat::Bc4RSnorm:
        case CompressedPixelFormat::Bc5RGUnorm:
        case CompressedPixelFormat::Bc5RGSnorm:
        case CompressedPixelFormat::Bc6hRGBUfloat:
        case CompressedPixelFormat::Bc6hRGBSfloat:
        case CompressedPixelFormat::Bc7RGBAUnorm:
        case CompressedPixelFormat::EacR11Unorm:
        case CompressedPixelFormat::EacR11Snorm:
        case CompressedPixelFormat::EacRG11Unorm:
        case CompressedPixelFormat::EacRG11Snorm:
        case CompressedPixelFormat::Etc2RGB8Unorm:
        case CompressedPixelFormat::Etc2RGB8A1Unorm:
        case CompressedPixelFormat::Etc2RGBA8Unorm:
        case CompressedPixelFormat::Astc4x4RGBAUnorm:
        case CompressedPixelFormat::Astc4x4RGBAF:
        case CompressedPixelFormat::Astc5x4RGBAUnorm:
        case CompressedPixelFormat::Astc5x4RGBAF:
        case CompressedPixelFormat::Astc5x5RGBAUnorm:
        case CompressedPixelFormat::Astc5x5RGBAF:
        case CompressedPixelFormat::Astc6x5RGBAUnorm:
        case CompressedPixelFormat::Astc6x5RGBAF:
        case CompressedPixelFormat::Astc6x6RGBAUnorm:
        case CompressedPixelFormat::Astc6x6RGBAF:
        case CompressedPixelFormat::Astc8x5RGBAUnorm:
        case CompressedPixelFormat::Astc8x5RGBAF:
        case CompressedPixelFormat::Astc8x6RGBAUnorm:
        case CompressedPixelFormat::Astc8x6RGBAF:
        case CompressedPixelFormat::Astc8x8RGBAUnorm:
        case CompressedPixelFormat::Astc8x8RGBAF:
        case CompressedPixelFormat::Astc10x5RGBAUnorm:
        case CompressedPixelFormat::Astc10x5RGBAF:
        case CompressedPixelFormat::Astc10x6RGBAUnorm:
        case CompressedPixelFormat::Astc10x6RGBAF:
        case CompressedPixelFormat::Astc10x8RGBAUnorm:
        case CompressedPixelFormat::Astc10x8RGBAF:
        case CompressedPixelFormat::Astc10x10RGBAUnorm:
        case CompressedPixelFormat::Astc10x10RGBAF:
        case CompressedPixelFormat::Astc12x10RGBAUnorm:
        case CompressedPixelFormat::Astc12x10RGBAF:
        case CompressedPixelFormat::Astc12x12RGBAUnorm:
        case CompressedPixelFormat::Astc12x12RGBAF:
        case CompressedPixelFormat::Astc3x3x3RGBAUnorm:
        case CompressedPixelFormat::Astc3x3x3RGBAF:
        case CompressedPixelFormat::Astc4x3x3RGBAUnorm:
        case CompressedPixelFormat::Astc4x3x3RGBAF:
        case CompressedPixelFormat::Astc4x4x3RGBAUnorm:
        case CompressedPixelFormat::Astc4x4x3RGBAF:
        case CompressedPixelFormat::Astc4x4x4RGBAUnorm:
        case CompressedPixelFormat::Astc4x4x4RGBAF:
        case CompressedPixelFormat::Astc5x4x4RGBAUnorm:
        case CompressedPixelFormat::Astc5x4x4RGBAF:
        case CompressedPixelFormat::Astc5x5x4RGBAUnorm:
        case CompressedPixelFormat::Astc5x5x4RGBAF:
        case CompressedPixelFormat::Astc5x5x5RGBAUnorm:
        case CompressedPixelFormat::Astc5x5x5RGBAF:
        case CompressedPixelFormat::Astc6x5x5RGBAUnorm:
        case CompressedPixelFormat::Astc6x5x5RGBAF:
        case CompressedPixelFormat::Astc6x6x5RGBAUnorm:
        case CompressedPixelFormat::Astc6x6x5RGBAF:
        case CompressedPixelFormat::Astc6x6x6RGBAUnorm:
        case CompressedPixelFormat::Astc6x6x6RGBAF:
        case CompressedPixelFormat::PvrtcRGB2bppUnorm:
        case CompressedPixelFormat::PvrtcRGBA2bppUnorm:
        case CompressedPixelFormat::PvrtcRGB4bppUnorm:
        case CompressedPixelFormat::PvrtcRGBA4bppUnorm:
            return false;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("isCompressedPixelFormatSrgb(): invalid format" << format, {});
}

namespace {

constexpr const char* CompressedPixelFormatNames[] {
    #define _c(format, width, height, depth, size) #format,
    #include "Magnum/Implementation/compressedPixelFormatMapping.hpp"
    #undef _c
};

}

Debug& operator<<(Debug& debug, const CompressedPixelFormat value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "CompressedPixelFormat" << Debug::nospace;

    if(isCompressedPixelFormatImplementationSpecific(value)) {
        return debug << (packed ? "ImplementationSpecific(" : "::ImplementationSpecific(") << Debug::nospace << Debug::hex << compressedPixelFormatUnwrap(value) << Debug::nospace << ")";
    }

    if(UnsignedInt(value) - 1 < Containers::arraySize(CompressedPixelFormatNames)) {
        return debug << (packed ? "" : "::") << Debug::nospace << CompressedPixelFormatNames[UnsignedInt(value) - 1];
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << (packed ? "" : ")");
}

}

namespace Corrade { namespace Utility {

Containers::String ConfigurationValue<Magnum::PixelFormat>::toString(Magnum::PixelFormat value, ConfigurationValueFlags) {
    if(Magnum::UnsignedInt(value) - 1 < Containers::arraySize(Magnum::PixelFormatNames))
        return Magnum::PixelFormatNames[Magnum::UnsignedInt(value) - 1];

    return {};
}

Magnum::PixelFormat ConfigurationValue<Magnum::PixelFormat>::fromString(Containers::StringView stringValue, ConfigurationValueFlags) {
    /** @todo This is extremely slow with >100 values. Do a binary search on a
        sorted index list instead (extracted into a common utility) */
    for(std::size_t i = 0; i != Containers::arraySize(Magnum::PixelFormatNames); ++i)
        if(stringValue == Magnum::PixelFormatNames[i]) return Magnum::PixelFormat(i + 1);

    return {};
}

Containers::String ConfigurationValue<Magnum::CompressedPixelFormat>::toString(Magnum::CompressedPixelFormat value, ConfigurationValueFlags) {
    if(Magnum::UnsignedInt(value) - 1 < Containers::arraySize(Magnum::CompressedPixelFormatNames))
        return Magnum::CompressedPixelFormatNames[Magnum::UnsignedInt(value) - 1];

    return {};
}

Magnum::CompressedPixelFormat ConfigurationValue<Magnum::CompressedPixelFormat>::fromString(Containers::StringView stringValue, ConfigurationValueFlags) {
    /** @todo This is extremely slow with >100 values. Do a binary search on a
        sorted index list instead (extracted into a common utility) */
    for(std::size_t i = 0; i != Containers::arraySize(Magnum::CompressedPixelFormatNames); ++i)
        if(stringValue == Magnum::CompressedPixelFormatNames[i]) return Magnum::CompressedPixelFormat(i + 1);

    return {};
}

}}
