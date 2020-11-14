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

#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPool.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct CommandBufferVkTest: VulkanTester {
    explicit CommandBufferVkTest();

    void construct();
    void constructMove();
    void wrap();

    void reset();
};

CommandBufferVkTest::CommandBufferVkTest() {
    addTests({&CommandBufferVkTest::construct,
              &CommandBufferVkTest::constructMove,
              &CommandBufferVkTest::wrap,

              &CommandBufferVkTest::reset});
}

void CommandBufferVkTest::construct() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    {
        CommandBuffer buffer = pool.allocate();
        CORRADE_VERIFY(buffer.handle());
        CORRADE_COMPARE(buffer.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void CommandBufferVkTest::constructMove() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    CommandBuffer a = pool.allocate();
    VkCommandBuffer handle = a.handle();

    CommandBuffer b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);

    CommandBuffer c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<CommandBuffer>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<CommandBuffer>::value);
}

void CommandBufferVkTest::wrap() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    VkCommandBuffer buffer{};
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = pool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = 1;
    CORRADE_COMPARE(Result(device()->AllocateCommandBuffers(device(), &info, &buffer)), Result::Success);
    CORRADE_VERIFY(buffer);

    auto wrapped = CommandBuffer::wrap(device(), pool, buffer, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), buffer);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), buffer);
    CORRADE_VERIFY(!wrapped.handle());
    device()->FreeCommandBuffers(device(), pool, 1, &buffer);
}

void CommandBufferVkTest::reset() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics),
        CommandPoolCreateInfo::Flag::ResetCommandBuffer}};

    CommandBuffer a = pool.allocate();

    a.reset(CommandBufferResetFlag::ReleaseResources);

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::CommandBufferVkTest)
