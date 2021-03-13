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

#include <new>
#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/DescriptorPoolCreateInfo.h"
#include "Magnum/Vk/DescriptorType.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DescriptorPoolTest: TestSuite::Tester {
    explicit DescriptorPoolTest();

    void createInfoConstruct();
    void createInfoConstructNoSets();
    void createInfoConstructNoPools();
    void createInfoConstructEmptyPool();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();
    void createInfoConstructCopy();
    void createInfoConstructMove();

    void constructNoCreate();
    void constructCopy();
};

DescriptorPoolTest::DescriptorPoolTest() {
    addTests({&DescriptorPoolTest::createInfoConstruct,
              &DescriptorPoolTest::createInfoConstructNoSets,
              &DescriptorPoolTest::createInfoConstructNoPools,
              &DescriptorPoolTest::createInfoConstructEmptyPool,
              &DescriptorPoolTest::createInfoConstructNoInit,
              &DescriptorPoolTest::createInfoConstructFromVk,
              &DescriptorPoolTest::createInfoConstructCopy,
              &DescriptorPoolTest::createInfoConstructMove,

              &DescriptorPoolTest::constructNoCreate,
              &DescriptorPoolTest::constructCopy});
}

void DescriptorPoolTest::createInfoConstruct() {
    DescriptorPoolCreateInfo info{5, {
        {DescriptorType::CombinedImageSampler, 7},
        {DescriptorType::UniformBuffer, 3}
    }, DescriptorPoolCreateInfo::Flag::FreeDescriptorSet|DescriptorPoolCreateInfo::Flag::UpdateAfterBind};
    CORRADE_COMPARE(info->flags, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT|VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT);
    CORRADE_COMPARE(info->maxSets, 5);
    CORRADE_COMPARE(info->poolSizeCount, 2);
    CORRADE_VERIFY(info->pPoolSizes);
    CORRADE_COMPARE(info->pPoolSizes[0].type, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    CORRADE_COMPARE(info->pPoolSizes[0].descriptorCount, 7);
    CORRADE_COMPARE(info->pPoolSizes[1].type, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    CORRADE_COMPARE(info->pPoolSizes[1].descriptorCount, 3);
}

void DescriptorPoolTest::createInfoConstructNoSets() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    DescriptorPoolCreateInfo{0, {}};
    CORRADE_COMPARE(out.str(), "Vk::DescriptorPoolCreateInfo: there has to be at least one set\n");
}

void DescriptorPoolTest::createInfoConstructNoPools() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    DescriptorPoolCreateInfo{5, {}};
    CORRADE_COMPARE(out.str(), "Vk::DescriptorPoolCreateInfo: there has to be at least one pool\n");
}

void DescriptorPoolTest::createInfoConstructEmptyPool() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    DescriptorPoolCreateInfo{5, {
        {DescriptorType::InputAttachment, 2},
        {DescriptorType::UniformBuffer, 0},
    }};
    CORRADE_COMPARE(out.str(), "Vk::DescriptorPoolCreateInfo: pool 1 of Vk::DescriptorType::UniformBuffer has no descriptors\n");
}

void DescriptorPoolTest::createInfoConstructNoInit() {
    DescriptorPoolCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) DescriptorPoolCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY(std::is_nothrow_constructible<DescriptorPoolCreateInfo, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, DescriptorPoolCreateInfo>::value);
}

void DescriptorPoolTest::createInfoConstructFromVk() {
    VkDescriptorPoolCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    DescriptorPoolCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void DescriptorPoolTest::createInfoConstructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DescriptorPoolCreateInfo>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DescriptorPoolCreateInfo>{});
}

void DescriptorPoolTest::createInfoConstructMove() {
    DescriptorPoolCreateInfo a{5, {
        {DescriptorType::CombinedImageSampler, 7},
        {DescriptorType::UniformBuffer, 3}
    }};
    CORRADE_COMPARE(a->poolSizeCount, 2);
    CORRADE_VERIFY(a->pPoolSizes);

    DescriptorPoolCreateInfo b = std::move(a);
    CORRADE_COMPARE(a->poolSizeCount, 0);
    CORRADE_VERIFY(!a->pPoolSizes);
    CORRADE_COMPARE(b->poolSizeCount, 2);
    CORRADE_VERIFY(b->pPoolSizes);
    CORRADE_COMPARE(b->pPoolSizes[1].type, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    DescriptorPoolCreateInfo c{VkDescriptorPoolCreateInfo{}};
    c = std::move(b);
    CORRADE_COMPARE(b->poolSizeCount, 0);
    CORRADE_VERIFY(!b->pPoolSizes);
    CORRADE_COMPARE(c->poolSizeCount, 2);
    CORRADE_VERIFY(c->pPoolSizes);
    CORRADE_COMPARE(c->pPoolSizes[1].type, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
}

void DescriptorPoolTest::constructNoCreate() {
    {
        DescriptorPool pool{NoCreate};
        CORRADE_VERIFY(!pool.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, DescriptorPool>::value);
}

void DescriptorPoolTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DescriptorPool>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DescriptorPool>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DescriptorPoolTest)
