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

#include "Magnum/Vk/DescriptorSetLayoutCreateInfo.h"
#include "Magnum/Vk/DescriptorType.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DescriptorSetLayoutVkTest: VulkanTester {
    explicit DescriptorSetLayoutVkTest();

    void construct();
    void constructEmpty();
    void constructEmptyBinding();
    void constructMove();

    void wrap();
};

DescriptorSetLayoutVkTest::DescriptorSetLayoutVkTest() {
    addTests({&DescriptorSetLayoutVkTest::construct,
              &DescriptorSetLayoutVkTest::constructEmpty,
              &DescriptorSetLayoutVkTest::constructEmptyBinding,
              &DescriptorSetLayoutVkTest::constructMove,

              &DescriptorSetLayoutVkTest::wrap});
}

void DescriptorSetLayoutVkTest::construct() {
    {
        DescriptorSetLayout layout{device(), DescriptorSetLayoutCreateInfo{
            {{15, DescriptorType::UniformBuffer}}
        }};
        CORRADE_VERIFY(layout.handle());
        CORRADE_COMPARE(layout.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void DescriptorSetLayoutVkTest::constructEmpty() {
    {
        /* Although rather weird, the spec allows this */
        DescriptorSetLayout layout{device(), DescriptorSetLayoutCreateInfo{}};
        CORRADE_VERIFY(layout.handle());
        CORRADE_COMPARE(layout.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void DescriptorSetLayoutVkTest::constructEmptyBinding() {
    {
        /* Also weird, but the spec *also* allows this */
        DescriptorSetLayout layout{device(), DescriptorSetLayoutCreateInfo{
            {{15, DescriptorType::UniformBuffer, 0}}
        }};
        CORRADE_VERIFY(layout.handle());
        CORRADE_COMPARE(layout.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void DescriptorSetLayoutVkTest::constructMove() {
    DescriptorSetLayout a{device(), DescriptorSetLayoutCreateInfo{
        {{15, DescriptorType::UniformBuffer}}
    }};
    VkDescriptorSetLayout handle = a.handle();

    DescriptorSetLayout b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);

    DescriptorSetLayout c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DescriptorSetLayout>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DescriptorSetLayout>::value);
}

void DescriptorSetLayoutVkTest::wrap() {
    VkDescriptorSetLayout layout{};
    CORRADE_COMPARE(Result(device()->CreateDescriptorSetLayout(device(),
        DescriptorSetLayoutCreateInfo{
            {{15, DescriptorType::UniformBuffer}}
        },
        nullptr, &layout)), Result::Success);

    auto wrapped = DescriptorSetLayout::wrap(device(), layout, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), layout);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), layout);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyDescriptorSetLayout(device(), layout, nullptr);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DescriptorSetLayoutVkTest)
