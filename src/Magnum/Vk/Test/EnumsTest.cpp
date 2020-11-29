/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include "Magnum/VertexFormat.h"
#include "Magnum/Vk/Enums.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct EnumsTest: TestSuite::Tester {
    explicit EnumsTest();

    void mapVkPrimitiveTopology();
    void mapVkPrimitiveTopologyImplementationSpecific();
    void mapVkPrimitiveTopologyUnsupported();
    void mapVkPrimitiveTopologyInvalid();

    void mapVkIndexType();
    void mapVkIndexTypeUnsupported();
    void mapVkIndexTypeInvalid();

    void mapVkFormatVertexFormat();
    void mapVkFormatVertexFormatImplementationSpecific();
    void mapVkFormatVertexFormatUnsupported();
    void mapVkFormatVertexFormatInvalid();

    void mapVkFormatPixelFormat();
    void mapVkFormatPixelFormatImplementationSpecific();
    void mapVkFormatPixelFormatUnsupported();
    void mapVkFormatPixelFormatInvalid();

    void mapVkFormatCompressedPixelFormat();
    void mapVkFormatCompressedPixelFormatImplementationSpecific();
    void mapVkFormatCompressedPixelFormatUnsupported();
    void mapVkFormatCompressedPixelFormatInvalid();

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
              &EnumsTest::mapVkPrimitiveTopologyImplementationSpecific,
              &EnumsTest::mapVkPrimitiveTopologyUnsupported,
              &EnumsTest::mapVkPrimitiveTopologyInvalid,

              &EnumsTest::mapVkIndexType,
              &EnumsTest::mapVkIndexTypeUnsupported,
              &EnumsTest::mapVkIndexTypeInvalid,

              &EnumsTest::mapVkFormatVertexFormat,
              &EnumsTest::mapVkFormatVertexFormatImplementationSpecific,
              &EnumsTest::mapVkFormatVertexFormatUnsupported,
              &EnumsTest::mapVkFormatVertexFormatInvalid,

              &EnumsTest::mapVkFormatPixelFormat,
              &EnumsTest::mapVkFormatPixelFormatImplementationSpecific,
              &EnumsTest::mapVkFormatPixelFormatUnsupported,
              &EnumsTest::mapVkFormatPixelFormatInvalid,

              &EnumsTest::mapVkFormatCompressedPixelFormat,
              &EnumsTest::mapVkFormatCompressedPixelFormatImplementationSpecific,
              &EnumsTest::mapVkFormatCompressedPixelFormatUnsupported,
              &EnumsTest::mapVkFormatCompressedPixelFormatInvalid,

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

    /* Ensure all generic primitives are handled. This goes through the first
       16 bits, which should be enough. Going through 32 bits takes 8 seconds,
       too much. */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto primitive = Magnum::MeshPrimitive(i);
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(primitive) {
            #define _c(primitive) \
                case Magnum::MeshPrimitive::primitive: \
                    if(hasVkPrimitiveTopology(Magnum::MeshPrimitive::primitive))  \
                        CORRADE_VERIFY(UnsignedInt(vkPrimitiveTopology(Magnum::MeshPrimitive::primitive)) >= 0); \
                    break;
            #include "Magnum/Implementation/meshPrimitiveMapping.hpp"
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif
    }
}

void EnumsTest::mapVkPrimitiveTopologyImplementationSpecific() {
    CORRADE_VERIFY(hasVkPrimitiveTopology(meshPrimitiveWrap(VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY)));
    CORRADE_COMPARE(vkPrimitiveTopology(meshPrimitiveWrap(VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY)),
        VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY);
}

void EnumsTest::mapVkPrimitiveTopologyUnsupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    hasVkPrimitiveTopology(Magnum::MeshPrimitive{});
    hasVkPrimitiveTopology(Magnum::MeshPrimitive(0x12));
    vkPrimitiveTopology(Magnum::MeshPrimitive{});
    vkPrimitiveTopology(Magnum::MeshPrimitive(0x12));
    CORRADE_COMPARE(out.str(),
        "Vk::hasVkPrimitiveTopology(): invalid primitive MeshPrimitive(0x0)\n"
        "Vk::hasVkPrimitiveTopology(): invalid primitive MeshPrimitive(0x12)\n"
        "Vk::vkPrimitiveTopology(): invalid primitive MeshPrimitive(0x0)\n"
        "Vk::vkPrimitiveTopology(): invalid primitive MeshPrimitive(0x12)\n");
}

void EnumsTest::mapVkIndexType() {
    CORRADE_VERIFY(hasVkIndexType(Magnum::MeshIndexType::UnsignedShort));
    CORRADE_COMPARE(vkIndexType(Magnum::MeshIndexType::UnsignedShort), VK_INDEX_TYPE_UINT16);

    CORRADE_VERIFY(hasVkIndexType(Magnum::MeshIndexType::UnsignedInt));
    CORRADE_COMPARE(vkIndexType(Magnum::MeshIndexType::UnsignedInt), VK_INDEX_TYPE_UINT32);

    /* Ensure all generic index types are handled. This goes through the first
       16 bits, which should be enough. Going through 32 bits takes 8 seconds,
       too much. */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto type = Magnum::MeshIndexType(i);
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(type) {
            #define _c(type) \
                case Magnum::MeshIndexType::type: \
                    CORRADE_VERIFY(UnsignedInt(vkIndexType(Magnum::MeshIndexType::type)) >= 0); \
                    break;
            #include "Magnum/Implementation/meshIndexTypeMapping.hpp"
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif
    }
}

void EnumsTest::mapVkIndexTypeUnsupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #if 1
    CORRADE_SKIP("All index formats are supported.");
    #else
    CORRADE_VERIFY(!hasVkIndexType(Magnum::MeshIndexType::UnsignedByte));
    std::ostringstream out;
    {
        Error redirectError{&out};
        vkIndexType(Magnum::MeshIndexType::UnsignedByte);
    }
    CORRADE_COMPARE(out.str(),
        "Vk::vkIndexType(): unsupported type MeshIndexType::UnsignedByte\n");
    #endif
}

void EnumsTest::mapVkIndexTypeInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    hasVkIndexType(Magnum::MeshIndexType(0x0));
    hasVkIndexType(Magnum::MeshIndexType(0x12));
    vkIndexType(Magnum::MeshIndexType(0x0));
    vkIndexType(Magnum::MeshIndexType(0x12));
    CORRADE_COMPARE(out.str(),
        "Vk::hasVkIndexType(): invalid type MeshIndexType(0x0)\n"
        "Vk::hasVkIndexType(): invalid type MeshIndexType(0x12)\n"
        "Vk::vkIndexType(): invalid type MeshIndexType(0x0)\n"
        "Vk::vkIndexType(): invalid type MeshIndexType(0x12)\n");
}

void EnumsTest::mapVkFormatVertexFormat() {
    /* Touchstone verification */
    CORRADE_VERIFY(hasVkFormat(Magnum::VertexFormat::Vector3us));
    CORRADE_COMPARE(vkFormat(Magnum::VertexFormat::Vector3us), VK_FORMAT_R16G16B16_UINT);
    CORRADE_COMPARE(vkFormat(Magnum::VertexFormat::Matrix2x3bNormalizedAligned), VK_FORMAT_R8G8B8_SNORM);

    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid format */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto format = Magnum::VertexFormat(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range)
           - that a particular vertex format maps to a particular VkFormat */
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(format) {
            #define _c(format, expectedFormat) \
                case Magnum::VertexFormat::format: \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(hasVkFormat(Magnum::VertexFormat::format)); \
                    CORRADE_COMPARE(vkFormat(Magnum::VertexFormat::format), VK_FORMAT_ ## expectedFormat); \
                    ++nextHandled; \
                    continue;
            #define _s(format) \
                case Magnum::VertexFormat::format: { \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasVkFormat(Magnum::VertexFormat::format)); \
                    std::ostringstream out; \
                    { /* Redirected otherwise graceful assert would abort */ \
                        Error redirectError{&out}; \
                        vkFormat(Magnum::VertexFormat::format); \
                    } \
                    Debug{Debug::Flag::NoNewlineAtTheEnd} << out.str(); \
                    ++nextHandled; \
                    continue; \
                }
            #include "Magnum/Vk/Implementation/vertexFormatMapping.hpp"
            #undef _s
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xffff);
}

void EnumsTest::mapVkFormatVertexFormatImplementationSpecific() {
    CORRADE_VERIFY(hasVkFormat(Magnum::vertexFormatWrap(VK_FORMAT_A8B8G8R8_SINT_PACK32)));
    CORRADE_COMPARE(vkFormat(Magnum::vertexFormatWrap(VK_FORMAT_A8B8G8R8_SINT_PACK32)),
        VK_FORMAT_A8B8G8R8_SINT_PACK32);
}

void EnumsTest::mapVkFormatVertexFormatUnsupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #if 1
    CORRADE_SKIP("All vertex formats are supported.");
    #else
    std::ostringstream out;
    Error redirectError{&out};

    vkFormat(Magnum::VertexFormat::Vector3d);
    CORRADE_COMPARE(out.str(), "Vk::vkFormat(): unsupported format VertexFormat::Vector3d\n");
    #endif
}

void EnumsTest::mapVkFormatVertexFormatInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    hasVkFormat(Magnum::VertexFormat{});
    hasVkFormat(Magnum::VertexFormat(0x123));
    vkFormat(Magnum::VertexFormat{});
    vkFormat(Magnum::VertexFormat(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::hasVkFormat(): invalid format VertexFormat(0x0)\n"
        "Vk::hasVkFormat(): invalid format VertexFormat(0x123)\n"
        "Vk::vkFormat(): invalid format VertexFormat(0x0)\n"
        "Vk::vkFormat(): invalid format VertexFormat(0x123)\n");
}

void EnumsTest::mapVkFormatPixelFormat() {
    /* Touchstone verification */
    CORRADE_VERIFY(hasVkFormat(Magnum::PixelFormat::RGBA8Unorm));
    CORRADE_COMPARE(vkFormat(Magnum::PixelFormat::RGBA8Unorm), VK_FORMAT_R8G8B8A8_UNORM);

    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid format */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto format = Magnum::PixelFormat(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range)
           - that a particular pixel format maps to a particular vkFormat */
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
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
                case Magnum::PixelFormat::format: { \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasVkFormat(Magnum::PixelFormat::format)); \
                    std::ostringstream out; \
                    { /* Redirected otherwise graceful assert would abort */ \
                        Error redirectError{&out}; \
                        vkFormat(Magnum::PixelFormat::format); \
                    } \
                    Debug{Debug::Flag::NoNewlineAtTheEnd} << out.str(); \
                    ++nextHandled; \
                    continue; \
                }
            #include "Magnum/Vk/Implementation/pixelFormatMapping.hpp"
            #undef _s
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xffff);
}

void EnumsTest::mapVkFormatPixelFormatImplementationSpecific() {
    CORRADE_VERIFY(hasVkFormat(Magnum::pixelFormatWrap(VK_FORMAT_A8B8G8R8_SINT_PACK32)));
    CORRADE_COMPARE(vkFormat(Magnum::pixelFormatWrap(VK_FORMAT_A8B8G8R8_SINT_PACK32)),
        VK_FORMAT_A8B8G8R8_SINT_PACK32);
}

void EnumsTest::mapVkFormatPixelFormatUnsupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #if 1
    CORRADE_SKIP("All pixel formats are supported.");
    #else
    std::ostringstream out;
    Error redirectError{&out};

    vkFormat(Magnum::PixelFormat::RGB16UI);
    CORRADE_COMPARE(out.str(), "Vk::vkFormat(): unsupported format PixelFormat::RGB16UI\n");
    #endif
}

void EnumsTest::mapVkFormatPixelFormatInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    hasVkFormat(Magnum::PixelFormat{});
    hasVkFormat(Magnum::PixelFormat(0x123));
    vkFormat(Magnum::PixelFormat{});
    vkFormat(Magnum::PixelFormat(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::hasVkFormat(): invalid format PixelFormat(0x0)\n"
        "Vk::hasVkFormat(): invalid format PixelFormat(0x123)\n"
        "Vk::vkFormat(): invalid format PixelFormat(0x0)\n"
        "Vk::vkFormat(): invalid format PixelFormat(0x123)\n");
}

void EnumsTest::mapVkFormatCompressedPixelFormat() {
    /* Touchstone verification */
    CORRADE_VERIFY(hasVkFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm));
    CORRADE_COMPARE(vkFormat(Magnum::CompressedPixelFormat::Bc1RGBAUnorm), VK_FORMAT_BC1_RGBA_UNORM_BLOCK);

    /* This goes through the first 16 bits, which should be enough. Going
       through 32 bits takes 8 seconds, too much. */
    UnsignedInt firstUnhandled = 0xffff;
    UnsignedInt nextHandled = 1; /* 0 is an invalid format */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto format = Magnum::CompressedPixelFormat(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range)
           - that a particular pixel format maps to a particular VkFormat */
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
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
                case Magnum::CompressedPixelFormat::format: { \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xffff); \
                    CORRADE_VERIFY(!hasVkFormat(Magnum::CompressedPixelFormat::format)); \
                    std::ostringstream out; \
                    { /* Redirected otherwise graceful assert would abort */ \
                        Error redirectError{&out}; \
                        vkFormat(Magnum::CompressedPixelFormat::format); \
                    } \
                    Debug{Debug::Flag::NoNewlineAtTheEnd} << out.str(); \
                    ++nextHandled; \
                    continue; \
                }
            #include "Magnum/Vk/Implementation/compressedPixelFormatMapping.hpp"
            #undef _s
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xffff);
}

void EnumsTest::mapVkFormatCompressedPixelFormatImplementationSpecific() {
    CORRADE_VERIFY(hasVkFormat(Magnum::compressedPixelFormatWrap(VK_FORMAT_ASTC_10x6_UNORM_BLOCK)));
    CORRADE_COMPARE(vkFormat(Magnum::compressedPixelFormatWrap(VK_FORMAT_ASTC_10x6_UNORM_BLOCK)),
        VK_FORMAT_ASTC_10x6_UNORM_BLOCK);
}

void EnumsTest::mapVkFormatCompressedPixelFormatUnsupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    CORRADE_VERIFY(!hasVkFormat(Magnum::CompressedPixelFormat::Astc3x3x3RGBAUnorm));

    std::ostringstream out;
    Error redirectError{&out};
    vkFormat(Magnum::CompressedPixelFormat::Astc3x3x3RGBAUnorm);
    CORRADE_COMPARE(out.str(), "Vk::vkFormat(): unsupported format CompressedPixelFormat::Astc3x3x3RGBAUnorm\n");
}

void EnumsTest::mapVkFormatCompressedPixelFormatInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    hasVkFormat(Magnum::CompressedPixelFormat{});
    hasVkFormat(Magnum::CompressedPixelFormat(0x123));
    vkFormat(Magnum::CompressedPixelFormat{});
    vkFormat(Magnum::CompressedPixelFormat(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::hasVkFormat(): invalid format CompressedPixelFormat(0x0)\n"
        "Vk::hasVkFormat(): invalid format CompressedPixelFormat(0x123)\n"
        "Vk::vkFormat(): invalid format CompressedPixelFormat(0x0)\n"
        "Vk::vkFormat(): invalid format CompressedPixelFormat(0x123)\n");
}

void EnumsTest::mapVkFilter() {
    CORRADE_COMPARE(vkFilter(SamplerFilter::Nearest), VK_FILTER_NEAREST);
    CORRADE_COMPARE(vkFilter(SamplerFilter::Linear), VK_FILTER_LINEAR);
}

void EnumsTest::mapVkFilterInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    #if 1
    CORRADE_SKIP("All sampler address modes are supported.");
    #else
    CORRADE_VERIFY(!hasVkSamplerAddressMode(Magnum::SamplerWrapping::MirrorClampToEdge));
    std::ostringstream out;
    Error redirectError{&out};
    vkSamplerAddressMode(Magnum::SamplerWrapping::MirrorClampToEdge);
    CORRADE_COMPARE(out.str(),
        "Vk::vkSamplerAddressMode(): unsupported wrapping SamplerWrapping::MirrorClampToEdge\n");
    #endif
}

void EnumsTest::mapVkSamplerAddressModeInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    vkSamplerAddressMode(Magnum::SamplerWrapping(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::vkSamplerAddressMode(): invalid wrapping SamplerWrapping(0x123)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::EnumsTest)
