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

#include "Magnum/Vk/DescriptorPoolCreateInfo.h"
#include "Magnum/Vk/DescriptorSet.h"
#include "Magnum/Vk/DescriptorSetLayoutCreateInfo.h"
#include "Magnum/Vk/DescriptorType.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DescriptorSetVkTest: VulkanTester {
    explicit DescriptorSetVkTest();

    void constructMove();

    void wrap();
};

DescriptorSetVkTest::DescriptorSetVkTest() {
    addTests({&DescriptorSetVkTest::constructMove,

              &DescriptorSetVkTest::wrap});
}

void DescriptorSetVkTest::constructMove() {
    DescriptorSetLayout layout{device(), DescriptorSetLayoutCreateInfo{
        {{0, DescriptorType::UniformBuffer}}
    }};

    /* Use the FreeDescriptorSet flag so the allocated descriptor set has
       DestroyOnDestruction set */
    DescriptorPool pool{device(), DescriptorPoolCreateInfo{1, {
        {DescriptorType::UniformBuffer, 1}
    }, DescriptorPoolCreateInfo::Flag::FreeDescriptorSet}};

    DescriptorSet a = pool.allocate(layout);
    VkDescriptorSet handle = a.handle();

    DescriptorSet b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);

    DescriptorSet c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DescriptorSet>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DescriptorSet>::value);
}

void DescriptorSetVkTest::wrap() {
    DescriptorSetLayout layout{device(), DescriptorSetLayoutCreateInfo{
        {{0, DescriptorType::UniformBuffer}}
    }};

    /* Use the FreeDescriptorSet flag so we can explicitly free the thing */
    DescriptorPool pool{device(), DescriptorPoolCreateInfo{1, {
        {DescriptorType::UniformBuffer, 1}
    }, DescriptorPoolCreateInfo::Flag::FreeDescriptorSet}};

    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = pool;
    info.descriptorSetCount = 1;
    const VkDescriptorSetLayout handle = layout;
    info.pSetLayouts = &handle; /* ew */
    VkDescriptorSet set{};
    CORRADE_COMPARE(Result(device()->AllocateDescriptorSets(device(),
        &info, &set)), Result::Success);

    auto wrapped = DescriptorSet::wrap(device(), pool, set, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), set);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), set);
    CORRADE_VERIFY(!wrapped.handle());
    device()->FreeDescriptorSets(device(), pool, 1, &set);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DescriptorSetVkTest)
