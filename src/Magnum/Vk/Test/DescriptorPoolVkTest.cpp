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
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/DescriptorPoolCreateInfo.h"
#include "Magnum/Vk/DescriptorSet.h"
#include "Magnum/Vk/DescriptorSetLayoutCreateInfo.h"
#include "Magnum/Vk/DescriptorType.h"
#include "Magnum/Vk/DeviceCreateInfo.h"
#include "Magnum/Vk/DeviceFeatures.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DescriptorPoolVkTest: VulkanTester {
    explicit DescriptorPoolVkTest();

    void setupVariableDescriptorCount();
    void teardown();

    void construct();
    void constructMove();

    void allocate();
    void allocateFreeDescriptorSet();
    void allocateFail();
    void allocateVariableCount();
    void allocateVariableCountFreeDescriptorSet();
    void allocateVariableCountFail();

    void reset();

    void wrap();

    Queue _queue{NoCreate};
    Device _deviceVariableDescriptorCount{NoCreate};
};

DescriptorPoolVkTest::DescriptorPoolVkTest() {
    addTests({&DescriptorPoolVkTest::construct,
              &DescriptorPoolVkTest::constructMove,

              &DescriptorPoolVkTest::allocate,
              &DescriptorPoolVkTest::allocateFreeDescriptorSet,
              &DescriptorPoolVkTest::allocateFail});

    addTests({&DescriptorPoolVkTest::allocateVariableCount,
              &DescriptorPoolVkTest::allocateVariableCountFreeDescriptorSet,
              &DescriptorPoolVkTest::allocateVariableCountFail},
            &DescriptorPoolVkTest::setupVariableDescriptorCount,
            &DescriptorPoolVkTest::teardown);

    addTests({&DescriptorPoolVkTest::reset,

              &DescriptorPoolVkTest::wrap});
}

void DescriptorPoolVkTest::setupVariableDescriptorCount() {
    DeviceProperties properties = pickDevice(instance());
    if(!properties.enumerateExtensionProperties().isSupported<Extensions::EXT::descriptor_indexing>() ||
        !(properties.features() & DeviceFeature::DescriptorBindingVariableDescriptorCount))
        return;

    /* Create the device only if not already, to avoid spamming the output */
    if(!_deviceVariableDescriptorCount.handle()) _deviceVariableDescriptorCount.create(instance(),
        DeviceCreateInfo{std::move(properties)}
            .addQueues(QueueFlag::Graphics, {0.0f}, {_queue})
            .addEnabledExtensions<
                /* Dependency of EXT_descriptor_indexing if 1.1 isn't
                   supported. For simpler handling we enable it always, instead
                   of only when we're on 1.0. */
                Extensions::KHR::maintenance3,
                Extensions::EXT::descriptor_indexing
             >()
            .setEnabledFeatures(DeviceFeature::DescriptorBindingVariableDescriptorCount)
    );
}

void DescriptorPoolVkTest::teardown() {
    /* Nothing, the device & queue created by setupVariableDescriptorCount()
       is created just once and so shouldn't be destroyed right after */
}

void DescriptorPoolVkTest::construct() {
    {
        DescriptorPool pool{device(), DescriptorPoolCreateInfo{5, {
            {DescriptorType::UniformBuffer, 2}
        }}};
        CORRADE_VERIFY(pool.handle());
        CORRADE_COMPARE(pool.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void DescriptorPoolVkTest::constructMove() {
    DescriptorPool a{device(), DescriptorPoolCreateInfo{5, {
        {DescriptorType::UniformBuffer, 2}
    }}};
    VkDescriptorPool handle = a.handle();

    DescriptorPool b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);

    DescriptorPool c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DescriptorPool>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DescriptorPool>::value);
}

void DescriptorPoolVkTest::allocate() {
    DescriptorSetLayout layout{device(), DescriptorSetLayoutCreateInfo{
        {{0, DescriptorType::UniformBuffer}}
    }};

    /* We can allocate two sets at most, each with one uniform buffer */
    DescriptorPool pool{device(), DescriptorPoolCreateInfo{2, {
        {DescriptorType::UniformBuffer, 2}
    }}};

    {
        Containers::Optional<DescriptorSet> allocated = pool.tryAllocate(layout);
        CORRADE_VERIFY(allocated);
        CORRADE_VERIFY(allocated->handle());
        /* No DestroyOnDestruction, the sets get freed only on descriptor pool
           reset */
        CORRADE_COMPARE(allocated->handleFlags(), HandleFlags{});
    } {
        DescriptorSet allocated = pool.allocate(layout);
        CORRADE_VERIFY(allocated.handle());
        /* No DestroyOnDestruction, the sets get freed only on descriptor pool
           reset */
        CORRADE_COMPARE(allocated.handleFlags(), HandleFlags{});
    }
}

void DescriptorPoolVkTest::allocateFail() {
    DescriptorSetLayout layout{device(), DescriptorSetLayoutCreateInfo{
        {{0, DescriptorType::UniformBuffer, 2}},
    }};

    DescriptorPool pool{device(), DescriptorPoolCreateInfo{1, {
        {DescriptorType::UniformBuffer, 1}
    }}};

    {
        /* tryAllocate() should not assert, and should not print anything */
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!pool.tryAllocate(layout));
        CORRADE_COMPARE(out.str(), "");
    } {
        #ifdef CORRADE_NO_ASSERT
        CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
        #endif

        /* allocate() should assert with ErrorOutOfPoolMemory */
        std::ostringstream out;
        Error redirectError{&out};
        pool.allocate(layout);
        CORRADE_COMPARE(out.str(), "Vk::DescriptorPool::allocate(): allocation failed with Vk::Result::ErrorOutOfPoolMemory\n");
    }
}

void DescriptorPoolVkTest::allocateFreeDescriptorSet() {
    DescriptorSetLayout layout{device(), DescriptorSetLayoutCreateInfo{
        {{0, DescriptorType::UniformBuffer}}
    }};

    DescriptorPool pool{device(), DescriptorPoolCreateInfo{1, {
        {DescriptorType::UniformBuffer, 1}
    }, DescriptorPoolCreateInfo::Flag::FreeDescriptorSet}};

    DescriptorSet allocated = pool.allocate(layout);
    CORRADE_VERIFY(allocated.handle());
    /* vkFreeDescriptorSets() gets called on destruction */
    CORRADE_COMPARE(allocated.handleFlags(), HandleFlag::DestroyOnDestruction);
}

void DescriptorPoolVkTest::allocateVariableCount() {
    if(!(_deviceVariableDescriptorCount.enabledFeatures() & DeviceFeature::DescriptorBindingVariableDescriptorCount))
        CORRADE_SKIP("DeviceFeature::DescriptorBindingVariableDescriptorCount not supported, can't test.");

    DescriptorSetLayout layout{_deviceVariableDescriptorCount, DescriptorSetLayoutCreateInfo{
        {{0, DescriptorType::UniformBuffer, 8, ~ShaderStages{}, DescriptorSetLayoutBinding::Flag::VariableDescriptorCount}}
    }};

    /* We can allocate two sets at most and at most 12 uniform buffers */
    DescriptorPool pool{_deviceVariableDescriptorCount, DescriptorPoolCreateInfo{2, {
        {DescriptorType::UniformBuffer, 12}
    }}};

    {
        Containers::Optional<DescriptorSet> allocated = pool.tryAllocate(layout, 8);
        CORRADE_VERIFY(allocated);
        CORRADE_VERIFY(allocated->handle());
        /* No DestroyOnDestruction, the sets get freed only on descriptor pool
           reset */
        CORRADE_COMPARE(allocated->handleFlags(), HandleFlags{});
    } {
        DescriptorSet allocated = pool.allocate(layout, 4);
        CORRADE_VERIFY(allocated.handle());
        /* No DestroyOnDestruction, the sets get freed only on descriptor pool
           reset */
        CORRADE_COMPARE(allocated.handleFlags(), HandleFlags{});
    }
}

void DescriptorPoolVkTest::allocateVariableCountFail() {
    if(!(_deviceVariableDescriptorCount.enabledFeatures() & DeviceFeature::DescriptorBindingVariableDescriptorCount))
        CORRADE_SKIP("DeviceFeature::DescriptorBindingVariableDescriptorCount not supported, can't test.");

    DescriptorSetLayout layout{_deviceVariableDescriptorCount, DescriptorSetLayoutCreateInfo{
        {{0, DescriptorType::UniformBuffer, 8, ~ShaderStages{}, DescriptorSetLayoutBinding::Flag::VariableDescriptorCount}}
    }};

    /* We can allocate two sets at most and at most 8 uniform buffers */
    DescriptorPool pool{_deviceVariableDescriptorCount, DescriptorPoolCreateInfo{2, {
        {DescriptorType::UniformBuffer, 7}
    }}};

    {
        /* tryAllocate() should not assert, and should not print anything */
        std::ostringstream out;
        Error redirectError{&out};
        CORRADE_VERIFY(!pool.tryAllocate(layout, 8));
        CORRADE_COMPARE(out.str(), "");
    } {
        #ifdef CORRADE_NO_ASSERT
        CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
        #endif

        /* allocate() should assert with ErrorOutOfPoolMemory */
        std::ostringstream out;
        Error redirectError{&out};
        pool.allocate(layout, 8);
        CORRADE_COMPARE(out.str(), "Vk::DescriptorPool::allocate(): allocation failed with Vk::Result::ErrorOutOfPoolMemory\n");
    }
}

void DescriptorPoolVkTest::allocateVariableCountFreeDescriptorSet() {
    if(!(_deviceVariableDescriptorCount.enabledFeatures() & DeviceFeature::DescriptorBindingVariableDescriptorCount))
        CORRADE_SKIP("DeviceFeature::DescriptorBindingVariableDescriptorCount not supported, can't test.");

    DescriptorSetLayout layout{_deviceVariableDescriptorCount, DescriptorSetLayoutCreateInfo{
        {{0, DescriptorType::UniformBuffer, 4, ~ShaderStages{}, DescriptorSetLayoutBinding::Flag::VariableDescriptorCount}}
    }};

    DescriptorPool pool{_deviceVariableDescriptorCount, DescriptorPoolCreateInfo{1, {
        {DescriptorType::UniformBuffer, 4}
    }, DescriptorPoolCreateInfo::Flag::FreeDescriptorSet}};

    DescriptorSet allocated = pool.allocate(layout, 4);
    CORRADE_VERIFY(allocated.handle());
    /* vkFreeDescriptorSets() gets called on destruction */
    CORRADE_COMPARE(allocated.handleFlags(), HandleFlag::DestroyOnDestruction);
}

void DescriptorPoolVkTest::reset() {
    DescriptorSetLayout layout{device(), DescriptorSetLayoutCreateInfo{
        {{0, DescriptorType::UniformBuffer}}
    }};

    /* Just one */
    DescriptorPool pool{device(), DescriptorPoolCreateInfo{1, {
        {DescriptorType::UniformBuffer, 1}
    }}};

    /* First allocation will work */
    CORRADE_VERIFY(pool.tryAllocate(layout));

    /* Second won't */
    CORRADE_VERIFY(!pool.tryAllocate(layout));

    pool.reset();

    /* Now it will again */
    CORRADE_VERIFY(pool.tryAllocate(layout));
}

void DescriptorPoolVkTest::wrap() {
    VkDescriptorPool pool{};
    CORRADE_COMPARE(Result(device()->CreateDescriptorPool(device(),
        DescriptorPoolCreateInfo{5, {
            {DescriptorType::UniformBuffer, 2}
        }},
        nullptr, &pool)), Result::Success);

    auto wrapped = DescriptorPool::wrap(device(), pool, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), pool);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), pool);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyDescriptorPool(device(), pool, nullptr);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DescriptorPoolVkTest)
