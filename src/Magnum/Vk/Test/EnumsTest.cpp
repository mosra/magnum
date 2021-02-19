/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
    addTests({&EnumsTest::mapVkFilter,
              &EnumsTest::mapVkFilterInvalid,

              &EnumsTest::mapVkSamplerMipmapMode,
              &EnumsTest::mapVkSamplerMipmapModeInvalid,

              &EnumsTest::mapVkSamplerAddressMode,
              &EnumsTest::mapVkSamplerAddressModeArray,
              &EnumsTest::mapVkSamplerAddressModeUnsupported,
              &EnumsTest::mapVkSamplerAddressModeInvalid});
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
