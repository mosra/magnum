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

#include "Magnum/Sampler.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Vk/SamplerCreateInfo.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct SamplerTest: TestSuite::Tester {
    explicit SamplerTest();

    void mapFilter();
    void mapFilterInvalid();

    void mapMipmap();
    void mapMipmapInvalid();

    void mapWrapping();
    void mapWrappingVector();
    void mapWrappingInvalid();

    void createInfoConstruct();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();
    template<class T> void createInfoSetFilter();
    template<class T> void createInfoSetMipmap();
    template<class T> void createInfoSetWrapping();
    template<class T> void createInfoSetWrappingSingleValue();

    void constructNoCreate();
    void constructCopy();

    void debugFilter();
    void debugMipmap();
    void debugWrapping();
};

SamplerTest::SamplerTest() {
    addTests({&SamplerTest::mapFilter,
              &SamplerTest::mapFilterInvalid,

              &SamplerTest::mapMipmap,
              &SamplerTest::mapMipmapInvalid,

              &SamplerTest::mapWrapping,
              &SamplerTest::mapWrappingVector,
              &SamplerTest::mapWrappingInvalid,

              &SamplerTest::createInfoConstruct,
              &SamplerTest::createInfoConstructNoInit,
              &SamplerTest::createInfoConstructFromVk,
              &SamplerTest::createInfoSetFilter<SamplerFilter>,
              &SamplerTest::createInfoSetFilter<Magnum::SamplerFilter>,
              &SamplerTest::createInfoSetMipmap<SamplerMipmap>,
              &SamplerTest::createInfoSetMipmap<Magnum::SamplerMipmap>,
              &SamplerTest::createInfoSetWrapping<SamplerWrapping>,
              &SamplerTest::createInfoSetWrapping<Magnum::SamplerWrapping>,
              &SamplerTest::createInfoSetWrappingSingleValue<SamplerWrapping>,
              &SamplerTest::createInfoSetWrappingSingleValue<Magnum::SamplerWrapping>,

              &SamplerTest::constructNoCreate,
              &SamplerTest::constructCopy,

              &SamplerTest::debugFilter,
              &SamplerTest::debugMipmap,
              &SamplerTest::debugWrapping});
}

template<class> struct SamplerTypeTraits;
template<> struct SamplerTypeTraits<SamplerFilter> {
    static const char* name() { return "SamplerFilter"; }
};
template<> struct SamplerTypeTraits<Magnum::SamplerFilter> {
    static const char* name() { return "Magnum::SamplerFilter"; }
};
template<> struct SamplerTypeTraits<SamplerMipmap> {
    static const char* name() { return "SamplerMipmap"; }
};
template<> struct SamplerTypeTraits<Magnum::SamplerMipmap> {
    static const char* name() { return "Magnum::SamplerMipmap"; }
};
template<> struct SamplerTypeTraits<SamplerWrapping> {
    static const char* name() { return "SamplerWrapping"; }
};
template<> struct SamplerTypeTraits<Magnum::SamplerWrapping> {
    static const char* name() { return "Magnum::SamplerWrapping"; }
};

void SamplerTest::mapFilter() {
    CORRADE_COMPARE(samplerFilter(Magnum::SamplerFilter::Nearest), SamplerFilter::Nearest);
    CORRADE_COMPARE(samplerFilter(Magnum::SamplerFilter::Linear), SamplerFilter::Linear);
}

void SamplerTest::mapFilterInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    samplerFilter(Magnum::SamplerFilter(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::samplerFilter(): invalid filter SamplerFilter(0x123)\n");
}

void SamplerTest::mapMipmap() {
    CORRADE_COMPARE(samplerMipmap(Magnum::SamplerMipmap::Base), SamplerMipmap::Nearest); /* deliberate */
    CORRADE_COMPARE(samplerMipmap(Magnum::SamplerMipmap::Nearest), SamplerMipmap::Nearest);
    CORRADE_COMPARE(samplerMipmap(Magnum::SamplerMipmap::Linear), SamplerMipmap::Linear);
}

void SamplerTest::mapMipmapInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    samplerMipmap(Magnum::SamplerMipmap(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::samplerMipmap(): invalid mode SamplerMipmap(0x123)\n");
}

void SamplerTest::mapWrapping() {
    CORRADE_COMPARE(samplerWrapping(Magnum::SamplerWrapping::Repeat), SamplerWrapping::Repeat);

    CORRADE_COMPARE(samplerWrapping(Magnum::SamplerWrapping::MirroredRepeat), SamplerWrapping::MirroredRepeat);

    CORRADE_COMPARE(samplerWrapping(Magnum::SamplerWrapping::ClampToEdge), SamplerWrapping::ClampToEdge);

    CORRADE_COMPARE(samplerWrapping(Magnum::SamplerWrapping::ClampToBorder), SamplerWrapping::ClampToBorder);
}

void SamplerTest::mapWrappingVector() {
    CORRADE_COMPARE(samplerWrapping<2>({Magnum::SamplerWrapping::Repeat, Magnum::SamplerWrapping::ClampToBorder}), (Math::Vector2<SamplerWrapping>{SamplerWrapping::Repeat, SamplerWrapping::ClampToBorder}));
}

void SamplerTest::mapWrappingInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    samplerWrapping(Magnum::SamplerWrapping(0x123));
    CORRADE_COMPARE(out.str(),
        "Vk::samplerWrapping(): invalid wrapping SamplerWrapping(0x123)\n");
}

void SamplerTest::createInfoConstruct() {
    /** @todo use a real flag once it exists */
    SamplerCreateInfo info{SamplerCreateInfo::Flag(1237)};
    CORRADE_COMPARE(info->flags, 1237);
    CORRADE_COMPARE(info->minFilter, VK_FILTER_NEAREST);
    CORRADE_COMPARE(info->magFilter, VK_FILTER_NEAREST);
    CORRADE_COMPARE(info->mipmapMode, VK_SAMPLER_MIPMAP_MODE_NEAREST);
    CORRADE_COMPARE(info->addressModeU, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    CORRADE_COMPARE(info->addressModeV, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    CORRADE_COMPARE(info->addressModeW, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    CORRADE_COMPARE(info->minLod, -1000.0f);
    CORRADE_COMPARE(info->maxLod, 1000.0f);
}

void SamplerTest::createInfoConstructNoInit() {
    SamplerCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    new(&info) SamplerCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);

    CORRADE_VERIFY(std::is_nothrow_constructible<SamplerCreateInfo, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, SamplerCreateInfo>::value);
}

void SamplerTest::createInfoConstructFromVk() {
    VkSamplerCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    SamplerCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

template<class T> void SamplerTest::createInfoSetFilter() {
    setTestCaseTemplateName(SamplerTypeTraits<T>::name());

    SamplerCreateInfo info;
    info.setMinificationFilter(T::Linear, {})
        .setMagnificationFilter(T::Linear);
    CORRADE_COMPARE(info->minFilter, VK_FILTER_LINEAR);
    CORRADE_COMPARE(info->mipmapMode, 0);
    CORRADE_COMPARE(info->magFilter, VK_FILTER_LINEAR);
}

template<class T> void SamplerTest::createInfoSetMipmap() {
    setTestCaseTemplateName(SamplerTypeTraits<T>::name());

    SamplerCreateInfo info;
    info.setMinificationFilter({}, T::Linear);
    CORRADE_COMPARE(info->minFilter, 0);
    CORRADE_COMPARE(info->mipmapMode, VK_SAMPLER_MIPMAP_MODE_LINEAR);
}

template<class T> void SamplerTest::createInfoSetWrapping() {
    setTestCaseTemplateName(SamplerTypeTraits<T>::name());

    SamplerCreateInfo info;
    info.setWrapping({T::MirroredRepeat, T::ClampToEdge, T::MirrorClampToEdge});
    CORRADE_COMPARE(info->addressModeU, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
    CORRADE_COMPARE(info->addressModeV, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    CORRADE_COMPARE(info->addressModeW, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE);
}

template<class T> void SamplerTest::createInfoSetWrappingSingleValue() {
    setTestCaseTemplateName(SamplerTypeTraits<T>::name());

    SamplerCreateInfo info;
    info.setWrapping(T::ClampToBorder);
    CORRADE_COMPARE(info->addressModeU, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
    CORRADE_COMPARE(info->addressModeV, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
    CORRADE_COMPARE(info->addressModeW, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
}

void SamplerTest::constructNoCreate() {
    {
        Sampler sampler{NoCreate};
        CORRADE_VERIFY(!sampler.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, Sampler>::value);
}

void SamplerTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<Sampler>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Sampler>{});
}

void SamplerTest::debugFilter() {
    std::ostringstream out;
    Debug{&out} << SamplerFilter::Linear << SamplerFilter(-10007655);
    CORRADE_COMPARE(out.str(), "Vk::SamplerFilter::Linear Vk::SamplerFilter(-10007655)\n");
}

void SamplerTest::debugMipmap() {
    std::ostringstream out;
    Debug{&out} << SamplerMipmap::Linear << SamplerMipmap(-10007655);
    CORRADE_COMPARE(out.str(), "Vk::SamplerMipmap::Linear Vk::SamplerMipmap(-10007655)\n");
}

void SamplerTest::debugWrapping() {
    std::ostringstream out;
    Debug{&out} << SamplerWrapping::MirrorClampToEdge << SamplerWrapping(-10007655);
    CORRADE_COMPARE(out.str(), "Vk::SamplerWrapping::MirrorClampToEdge Vk::SamplerWrapping(-10007655)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::SamplerTest)
