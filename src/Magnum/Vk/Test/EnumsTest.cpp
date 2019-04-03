/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Mesh.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Sampler.h"
#include "Magnum/Vk/Enums.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

/* Tests MeshView as well */

struct EnumsTest: TestSuite::Tester {
    explicit EnumsTest();

    void mapVkPrimitiveTopology();
    void mapVkPrimitiveTopologyUnsupported();
    void mapVkPrimitiveTopologyInvalid();

    void mapVkIndexType();
    void mapVkIndexTypeUnsupported();
    void mapVkIndexTypeInvalid();

    void mapVkFormat();
    void mapVkFormatImplementationSpecific();
    void mapVkFormatUnsupported();
    void mapVkFormatInvalid();

    void mapVkFormatCompressed();
    void mapVkFormatCompressedImplementationSpecific();
    void mapVkFormatCompressedUnsupported();
    void mapVkFormatCompressedInvalid();

    void mapVkFilter();
    void mapVkFilterInvalid();

    void mapVkSamplerMipmapMode();
    void mapVkSamplerMipmapModeInvalid();

    void mapVkSamplerAddressMode();
    void mapVkSamplerAddressModeArray();
    void mapVkSamplerAddressModeUnsupported();
    void mapVkSamplerAddressModeInvalid();
};

EnumsTest::EnumsTest() {
    addTests({&EnumsTest::mapVkPrimitiveTopology,
              &EnumsTest::mapVkPrimitiveTopologyUnsupported,
              &EnumsTest::mapVkPrimitiveTopologyInvalid,

              &EnumsTest::mapVkIndexType,
              &EnumsTest::mapVkIndexTypeUnsupported,
              &EnumsTest::mapVkIndexTypeInvalid,

              &EnumsTest::mapVkFormat,
              &EnumsTest::mapVkFormatImplementationSpecific,
              &EnumsTest::mapVkFormatUnsupported,
              &EnumsTest::mapVkFormatInvalid,

              &EnumsTest::mapVkFormatCompressed,
              &EnumsTest::mapVkFormatCompressedImplementationSpecific,
              &EnumsTest::mapVkFormatCompressedUnsupported,
              &EnumsTest::mapVkFormatCompressedInvalid,

              &EnumsTest::mapVkFilter,
              &EnumsTest::mapVkFilterInvalid,

              &EnumsTest::mapVkSamplerMipmapMode,
              &EnumsTest::mapVkSamplerMipmapModeInvalid,

              &EnumsTest::mapVkSamplerAddressMode,
              &EnumsTest::mapVkSamplerAddressModeArray,
              &EnumsTest::mapVkSamplerAddressModeUnsupported,
              &EnumsTest::mapVkSamplerAddressModeInvalid});
}

void EnumsTest::mapVkPrimitiveTopology() {
    CORRADE_VERIFY(hasVkPrimitiveTopology(Magnum::MeshPrimitive::Points));
    CORRADE_COMPARE(vkPrimitiveTopology(Magnum::MeshPrimitive::Points), VK_PRIMITIVE_TOPOLOGY_POINT_LIST);

    CORRADE_VERIFY(hasVkPrimitiveTopology(Magnum::MeshPrimitive::Lines));
    CORRADE_COMPARE(vkPrimitiveTopology(Magnum::MeshPrimitive::Lines), VK_PRIMITIVE_TOPOLOGY_LINE_LIST);

    CORRADE_VERIFY(hasVkPrimitiveTopology(Magnum::MeshPrimitive::LineStrip));
    CORRADE_COMPARE(vkPrimitiveTopology(Magnum::MeshPrimitive::LineStrip), VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);

    CORRADE_VERIFY(hasVkPrimitiveTopology(Magnum::MeshPrimitive::Triangles));
    CORRADE_COMPARE(vkPrimitiveTopology(Magnum::MeshPrimitive::Triangles), VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    CORRADE_VERIFY(hasVkPrimitiveTopology(Magnum::MeshPrimitive::TriangleStrip));
    CORRADE_COMPARE(vkPrimitiveTopology(Magnum::MeshPrimitive::TriangleStrip), VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);

    CORRADE_VERIFY(hasVkPrimitiveTopology(Magnum::MeshPrimitive::TriangleFan));
    CORRADE_COMPARE(vkPrimitiveTopology(Magnum::MeshPrimitive::TriangleFan), VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN);
}

void EnumsTest::mapVkPrimitiveTopologyUnsupported() {
    CORRADE_VERIFY(!hasVkPrimitiveTopology(Magnum::MeshPrimitive::LineLoop));

    std::ostringstream out;
    {
        Error redirectError{&out};
        vkPrimitiveTopology(Magnum::MeshPrimitive::LineLoop);
    }
    CORRADE_COMPARE(out.str(),
        "Vk::vkPrimitiveTopology(): unsupported primitive MeshPrimitive::LineLoop\n");
}

void EnumsTest::mapVkPrimitiveTopologyInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    hasVkPrimitiveTopology(Magnum::MeshPrimitive(0x123));
    vkPrimitiveTopology(Magnum::MeshPrimitive(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::hasVkPrimitiveTopology(): invalid primitive MeshPrimitive(0x123)\n"
        "Vk::vkPrimitiveTopology(): invalid primitive MeshPrimitive(0x123)\n");
}

void EnumsTest::mapVkIndexType() {
    CORRADE_VERIFY(hasVkIndexType(Magnum::MeshIndexType::UnsignedShort));
    CORRADE_COMPARE(vkIndexType(Magnum::MeshIndexType::UnsignedShort), VK_INDEX_TYPE_UINT16);

    CORRADE_VERIFY(hasVkIndexType(Magnum::MeshIndexType::UnsignedInt));
    CORRADE_COMPARE(vkIndexType(Magnum::MeshIndexType::UnsignedInt), VK_INDEX_TYPE_UINT32);
}

void EnumsTest::mapVkIndexTypeUnsupported() {
    CORRADE_VERIFY(!hasVkIndexType(Magnum::MeshIndexType::UnsignedByte));
    std::ostringstream out;
    {
        Error redirectError{&out};
        vkIndexType(Magnum::MeshIndexType::UnsignedByte);
    }
    CORRADE_COMPARE(out.str(),
        "Vk::vkIndexType(): unsupported type MeshIndexType::UnsignedByte\n");
}

void EnumsTest::mapVkIndexTypeInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    hasVkIndexType(Magnum::MeshIndexType(0x123));
    vkIndexType(Magnum::MeshIndexType(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::hasVkIndexType(): invalid type MeshIndexType(0x123)\n"
        "Vk::vkIndexType(): invalid type MeshIndexType(0x123)\n");
}

void EnumsTest::mapVkFormat() {
    /* Touchstone verification */
    CORRADE_VERIFY(hasVkFormat(Magnum::PixelFormat::RGBA8Unorm));
    CORRADE_COMPARE(vkFormat(Magnum::PixelFormat::RGBA8Unorm), VK_FORMAT_R8G8B8A8_UNORM);

    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 0;
    for(UnsignedInt i = 0; i <= 0xffff; ++i) {
        const auto format = Magnum::PixelFormat(i);
        /* Each case verifies:
           - that the cases are ordered by number (so insertion here is done in
             proper place)
           - that there was no gap (unhandled value inside the range)
           - that a particular pixel format maps to a particular GL format
           - that a particular pixel type maps to a particular GL type */
        switch(format) {
            #define _c(format, expectedFormat) \
                case Magnum::PixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasVkFormat(Magnum::PixelFormat::format)); \
                    CORRADE_COMPARE(vkFormat(Magnum::PixelFormat::format), VK_FORMAT_ ## expectedFormat); \
                    ++nextHandled; \
                    continue;
            #define _s(format) \
                case Magnum::PixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasVkFormat(Magnum::PixelFormat::format)); \
                    vkFormat(Magnum::PixelFormat::format); \
                    ++nextHandled; \
                    continue;
            #include "Magnum/Vk/Implementation/formatMapping.hpp"
            #undef _s
            #undef _c
        }

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xffff);
}

void EnumsTest::mapVkFormatImplementationSpecific() {
    CORRADE_VERIFY(hasVkFormat(Magnum::pixelFormatWrap(VK_FORMAT_A8B8G8R8_SINT_PACK32)));
    CORRADE_COMPARE(vkFormat(Magnum::pixelFormatWrap(VK_FORMAT_A8B8G8R8_SINT_PACK32)),
        VK_FORMAT_A8B8G8R8_SINT_PACK32);
}

void EnumsTest::mapVkFormatUnsupported() {
    #if 1
    CORRADE_SKIP("All pixel formats are supported.");
    #else
    std::ostringstream out;
    Error redirectError{&out};

    vkFormat(Magnum::PixelFormat::RGB16UI);
    CORRADE_COMPARE(out.str(), "Vk::vkFormat(): unsupported format PixelFormat::RGB16UI\n");
    #endif
}

void EnumsTest::mapVkFormatInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    hasVkFormat(Magnum::PixelFormat(0x123));
    vkFormat(Magnum::PixelFormat(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::hasVkFormat(): invalid format PixelFormat(0x123)\n"
        "Vk::vkFormat(): invalid format PixelFormat(0x123)\n");
}

void EnumsTest::mapVkFormatCompressed() {
    /* Touchstone verification */
    CORRADE_VERIFY(hasVkFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm));
    CORRADE_COMPARE(vkFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm), VK_FORMAT_BC1_RGBA_UNORM_BLOCK);

    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 0;
    for(UnsignedInt i = 0; i <= 0xffff; ++i) {
        const auto format = Magnum::CompressedPixelFormat(i);
        /* Each case verifies:
           - that the cases are ordered by number (so insertion here is done in
             proper place)
           - that there was no gap (unhandled value inside the range)
           - that a particular pixel format maps to a particular GL format
           - that a particular pixel type maps to a particular GL type */
        switch(format) {
            #define _c(format, expectedFormat) \
                case Magnum::CompressedPixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasVkFormat(Magnum::CompressedPixelFormat::format)); \
                    CORRADE_COMPARE(vkFormat(Magnum::CompressedPixelFormat::format), VK_FORMAT_ ## expectedFormat); \
                    ++nextHandled; \
                    continue;
            #define _s(format) \
                case Magnum::CompressedPixelFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasVkFormat(Magnum::CompressedPixelFormat::format)); \
                    vkFormat(Magnum::CompressedPixelFormat::format); \
                    ++nextHandled; \
                    continue;
            #include "Magnum/Vk/Implementation/compressedFormatMapping.hpp"
            #undef _s
            #undef _c
        }

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xffff);
}

void EnumsTest::mapVkFormatCompressedImplementationSpecific() {
    CORRADE_VERIFY(hasVkFormat(Magnum::compressedPixelFormatWrap(VK_FORMAT_ASTC_10x6_UNORM_BLOCK)));
    CORRADE_COMPARE(vkFormat(Magnum::compressedPixelFormatWrap(VK_FORMAT_ASTC_10x6_UNORM_BLOCK)),
        VK_FORMAT_ASTC_10x6_UNORM_BLOCK);
}

void EnumsTest::mapVkFormatCompressedUnsupported() {
    #if 1
    CORRADE_SKIP("All compressed pixel formats are currently supported.");
    #else
    CORRADE_VERIFY(!hasVkFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm));

    std::ostringstream out;
    Error redirectError{&out};
    vkFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm);
    CORRADE_COMPARE(out.str(), "Vk::vkFormat(): unsupported format CompressedPixelFormat::Bc1RGBAUnorm\n");
    #endif
}

void EnumsTest::mapVkFormatCompressedInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    hasVkFormat(Magnum::CompressedPixelFormat(0x123));
    vkFormat(Magnum::CompressedPixelFormat(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::hasVkFormat(): invalid format CompressedPixelFormat(0x123)\n"
        "Vk::vkFormat(): invalid format CompressedPixelFormat(0x123)\n");
}

void EnumsTest::mapVkFilter() {
    CORRADE_COMPARE(vkFilter(SamplerFilter::Nearest), VK_FILTER_NEAREST);
    CORRADE_COMPARE(vkFilter(SamplerFilter::Linear), VK_FILTER_LINEAR);
}

void EnumsTest::mapVkFilterInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    vkFilter(Magnum::SamplerFilter(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::vkFilter(): invalid filter SamplerFilter(0x123)\n");
}

void EnumsTest::mapVkSamplerMipmapMode() {
    CORRADE_COMPARE(vkSamplerMipmapMode(SamplerMipmap::Base), VK_SAMPLER_MIPMAP_MODE_NEAREST); /* deliberate */
    CORRADE_COMPARE(vkSamplerMipmapMode(SamplerMipmap::Nearest), VK_SAMPLER_MIPMAP_MODE_NEAREST);
    CORRADE_COMPARE(vkSamplerMipmapMode(SamplerMipmap::Linear), VK_SAMPLER_MIPMAP_MODE_LINEAR);
}

void EnumsTest::mapVkSamplerMipmapModeInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    vkSamplerMipmapMode(Magnum::SamplerMipmap(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::vkSamplerMipmapMode(): invalid mode SamplerMipmap(0x123)\n");
}

void EnumsTest::mapVkSamplerAddressMode() {
    CORRADE_VERIFY(hasVkSamplerAddressMode(SamplerWrapping::Repeat));
    CORRADE_COMPARE(vkSamplerAddressMode(SamplerWrapping::Repeat), VK_SAMPLER_ADDRESS_MODE_REPEAT);

    CORRADE_VERIFY(hasVkSamplerAddressMode(SamplerWrapping::MirroredRepeat));
    CORRADE_COMPARE(vkSamplerAddressMode(SamplerWrapping::MirroredRepeat), VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);

    CORRADE_VERIFY(hasVkSamplerAddressMode(SamplerWrapping::ClampToEdge));
    CORRADE_COMPARE(vkSamplerAddressMode(SamplerWrapping::ClampToEdge), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

    CORRADE_VERIFY(hasVkSamplerAddressMode(SamplerWrapping::ClampToBorder));
    CORRADE_COMPARE(vkSamplerAddressMode(SamplerWrapping::ClampToBorder), VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
}

void EnumsTest::mapVkSamplerAddressModeArray() {
    CORRADE_COMPARE(vkSamplerAddressMode<2>({SamplerWrapping::Repeat, SamplerWrapping::ClampToBorder}), (Array2D<VkSamplerAddressMode>{VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER}));
}

void EnumsTest::mapVkSamplerAddressModeUnsupported() {
    CORRADE_VERIFY(!hasVkSamplerAddressMode(Magnum::SamplerWrapping::MirrorClampToEdge));
    std::ostringstream out;
    {
        Error redirectError{&out};
        vkSamplerAddressMode(Magnum::SamplerWrapping::MirrorClampToEdge);
    }
    CORRADE_COMPARE(out.str(),
        "Vk::vkSamplerAddressMode(): unsupported wrapping SamplerWrapping::MirrorClampToEdge\n");
}

void EnumsTest::mapVkSamplerAddressModeInvalid() {
    std::ostringstream out;
    Error redirectError{&out};

    vkSamplerAddressMode(Magnum::SamplerWrapping(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::vkSamplerAddressMode(): invalid wrapping SamplerWrapping(0x123)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::EnumsTest)
