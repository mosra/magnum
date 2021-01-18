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

#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPoolCreateInfo.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Fence.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct QueueVkTest: VulkanTester {
    explicit QueueVkTest();

    /* construction tested in DeviceVkTest as it's done implicitly on Device
       creation */
    /* move and wrap tested in QueueTest as there's no vkDestroyQueue() and
       thus we don't need a Vulkan-enabled test for that */

    void submit();
    void submitOne();
    void submitImplicitFence();
};

QueueVkTest::QueueVkTest() {
    addTests({&QueueVkTest::submit,
              &QueueVkTest::submitOne,
              &QueueVkTest::submitImplicitFence});
}

void QueueVkTest::submit() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    CommandBuffer a = pool.allocate();
    a.begin()
     .end();

    CommandBuffer b = pool.allocate();
    b.begin()
     .end();

    CommandBuffer c = pool.allocate();
    c.begin()
     .end();

    Fence fence{device()};
    CORRADE_VERIFY(!fence.status());

    queue().submit({
        SubmitInfo{}.setCommandBuffers({a, b}),
        SubmitInfo{}.setCommandBuffers({c})
    }, fence);

    CORRADE_VERIFY(fence.wait(std::chrono::milliseconds{1000}));
}

void QueueVkTest::submitOne() {
    /* Until DynamicArray is a thing, submit() has a special case for a single
       item that doesn't allocate but instead just point to it */
    /** @todo drop once DynamicArray handles that */

    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    CommandBuffer a = pool.allocate();
    a.begin()
     .end();

    Fence fence{device()};
    CORRADE_VERIFY(!fence.status());

    queue().submit({
        SubmitInfo{}.setCommandBuffers({a})
    }, fence);

    CORRADE_VERIFY(fence.wait(std::chrono::milliseconds{1000}));
}

void QueueVkTest::submitImplicitFence() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    CommandBuffer a = pool.allocate();
    a.begin()
     .end();

    CORRADE_VERIFY(queue().submit({SubmitInfo{}.setCommandBuffers({a})})
        .wait(std::chrono::milliseconds{1000}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::QueueVkTest)
