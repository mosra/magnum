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
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/BufferCreateInfo.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPoolCreateInfo.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/Fence.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/MemoryAllocateInfo.h"
#include "Magnum/Vk/Pipeline.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct BufferVkTest: VulkanTester {
    explicit BufferVkTest();

    void construct();
    void constructMove();

    void wrap();

    void memoryRequirements();

    void bindMemory();
    void bindDedicatedMemory();

    void directAllocation();

    void cmdFillBuffer();
    void cmdCopyBuffer();
    void cmdCopyBufferDisallowedConversion();
};

BufferVkTest::BufferVkTest() {
    addTests({&BufferVkTest::construct,
              &BufferVkTest::constructMove,

              &BufferVkTest::wrap,

              &BufferVkTest::memoryRequirements,

              &BufferVkTest::bindMemory,
              &BufferVkTest::bindDedicatedMemory,

              &BufferVkTest::directAllocation,

              &BufferVkTest::cmdFillBuffer,
              &BufferVkTest::cmdCopyBuffer,
              &BufferVkTest::cmdCopyBufferDisallowedConversion});
}

using namespace Containers::Literals;

void BufferVkTest::construct() {
    {
        Buffer buffer{device(), BufferCreateInfo{BufferUsage::StorageBuffer, 1024}, NoAllocate};
        CORRADE_VERIFY(buffer.handle());
        CORRADE_COMPARE(buffer.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void BufferVkTest::constructMove() {
    /* Verify that also the dedicated memory gets moved */
    Buffer a{device(), BufferCreateInfo{BufferUsage::StorageBuffer, 1024}, MemoryFlag::DeviceLocal};
    VkBuffer handle = a.handle();
    VkDeviceMemory memoryHandle = a.dedicatedMemory().handle();

    Buffer b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_VERIFY(!a.hasDedicatedMemory());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_VERIFY(b.hasDedicatedMemory());
    CORRADE_COMPARE(b.dedicatedMemory().handle(), memoryHandle);

    Buffer c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_VERIFY(!b.hasDedicatedMemory());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_VERIFY(c.hasDedicatedMemory());
    CORRADE_COMPARE(c.dedicatedMemory().handle(), memoryHandle);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Buffer>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Buffer>::value);
}

void BufferVkTest::wrap() {
    VkBuffer buffer{};
    CORRADE_COMPARE(Result(device()->CreateBuffer(device(),
        BufferCreateInfo{BufferUsage::StorageTexelBuffer, 4096},
        nullptr, &buffer)), Result::Success);

    auto wrapped = Buffer::wrap(device(), buffer, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), buffer);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), buffer);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyBuffer(device(), buffer, nullptr);
}

void BufferVkTest::memoryRequirements() {
    Buffer buffer{device(), BufferCreateInfo{BufferUsage::StorageBuffer, 16384}, NoAllocate};

    MemoryRequirements requirements = buffer.memoryRequirements();
    CORRADE_COMPARE(requirements.size(), 16384);
}

void BufferVkTest::bindMemory() {
    Buffer buffer{device(), BufferCreateInfo{BufferUsage::StorageBuffer, 16384}, NoAllocate};
    MemoryRequirements requirements = buffer.memoryRequirements();

    /* Similarly to the Image bindMemory() test, use a 128 kB offset */
    constexpr UnsignedLong offset = 128*1024;
    CORRADE_COMPARE_AS(offset, requirements.alignment(),
        TestSuite::Compare::Divisible);

    Memory memory{device(), MemoryAllocateInfo{
        requirements.size() + offset,
        device().properties().pickMemory(MemoryFlag::DeviceLocal, requirements.memories())}};

    buffer.bindMemory(memory, offset);
    CORRADE_VERIFY(!buffer.hasDedicatedMemory());
}

void BufferVkTest::bindDedicatedMemory() {
    Buffer buffer{device(), BufferCreateInfo{BufferUsage::StorageBuffer, 16384}, NoAllocate};
    MemoryRequirements requirements = buffer.memoryRequirements();

    /** @todo expand once KHR_dedicated_allocation is implemented */

    Memory memory{device(), MemoryAllocateInfo{
        requirements.size(),
        device().properties().pickMemory(MemoryFlag::DeviceLocal, requirements.memories())}};
    VkDeviceMemory handle = memory.handle();
    CORRADE_VERIFY(handle);

    buffer.bindDedicatedMemory(std::move(memory));
    CORRADE_VERIFY(buffer.hasDedicatedMemory());
    CORRADE_COMPARE(buffer.dedicatedMemory().handle(), handle);
}

void BufferVkTest::directAllocation() {
    Buffer buffer{device(),
        BufferCreateInfo{BufferUsage::StorageBuffer, 16384},
        MemoryFlag::DeviceLocal};

    /* Not sure what else to test here */
    CORRADE_VERIFY(buffer.hasDedicatedMemory());
    CORRADE_VERIFY(buffer.dedicatedMemory().handle());
}

void BufferVkTest::cmdFillBuffer() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    Buffer a{device(), BufferCreateInfo{
        BufferUsage::TransferSource|BufferUsage::TransferDestination, 16
    }, MemoryFlag::HostVisible};
    Utility::copy("0123456789abcdef"_s, a.dedicatedMemory().map());

    cmd.begin()
       .fillBuffer(a, 4, 8, 0x2e2e2e2e)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead}
        }, {}, {})
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();
    CORRADE_COMPARE(arrayView(a.dedicatedMemory().mapRead()), "0123........cdef"_s);

    /* Test the full fill as well */
    pool.reset();
    cmd.begin()
       .fillBuffer(a, 0x2e2e2e2e)
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead}
        }, {}, {})
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();
    CORRADE_COMPARE(arrayView(a.dedicatedMemory().mapRead()), "................"_s);
}

void BufferVkTest::cmdCopyBuffer() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Source buffer */
    Buffer a{device(), BufferCreateInfo{BufferUsage::TransferSource, 7}, MemoryFlag::HostVisible};
    Utility::copy("__ABCD_"_s, a.dedicatedMemory().map());

    /* Destination buffer, clear it to have predictable output */
    Buffer b{device(), BufferCreateInfo{BufferUsage::TransferDestination, 10}, MemoryFlag::HostVisible};
    Utility::copy(".........."_s, b.dedicatedMemory().map());

    cmd.begin()
       .copyBuffer({a, b, {{2, 5, 4}}})
       .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
           {Access::TransferWrite, Access::HostRead}
        }, {}, {})
       .end();
    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    CORRADE_COMPARE(arrayView(b.dedicatedMemory().mapRead()),
        ".....ABCD."_s);
}

void BufferVkTest::cmdCopyBufferDisallowedConversion() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(device().isExtensionEnabled<Extensions::KHR::copy_commands2>())
        CORRADE_SKIP("KHR_copy_commands2 enabled on the device, can't test");

    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    CopyBufferInfo a{{}, {}, {}};
    a->pNext = &a;

    /* The commands shouldn't do anything, so it should be fine to just call
       them without any render pass set up */
    std::ostringstream out;
    Error redirectError{&out};
    cmd.copyBuffer(a);
    CORRADE_COMPARE(out.str(),
        "Vk::CommandBuffer::copyBuffer(): disallowing extraction of CopyBufferInfo with non-empty pNext to prevent information loss\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::BufferVkTest)
