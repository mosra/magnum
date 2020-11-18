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

#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Vk/Buffer.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Memory.h"
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
};

BufferVkTest::BufferVkTest() {
    addTests({&BufferVkTest::construct,
              &BufferVkTest::constructMove,

              &BufferVkTest::wrap,

              &BufferVkTest::memoryRequirements,

              &BufferVkTest::bindMemory,
              &BufferVkTest::bindDedicatedMemory,

              &BufferVkTest::directAllocation});
}

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
    Buffer a{device(), BufferCreateInfo{BufferUsage::StorageBuffer, 1024}, Vk::MemoryFlag::DeviceLocal};
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

    Vk::Memory memory{device(), Vk::MemoryAllocateInfo{
        requirements.size() + offset,
        device().properties().pickMemory(Vk::MemoryFlag::DeviceLocal, requirements.memories())}};

    buffer.bindMemory(memory, offset);
    CORRADE_VERIFY(!buffer.hasDedicatedMemory());
}

void BufferVkTest::bindDedicatedMemory() {
    Buffer buffer{device(), BufferCreateInfo{BufferUsage::StorageBuffer, 16384}, NoAllocate};
    MemoryRequirements requirements = buffer.memoryRequirements();

    /** @todo expand once KHR_dedicated_allocation is implemented */

    Vk::Memory memory{device(), Vk::MemoryAllocateInfo{
        requirements.size(),
        device().properties().pickMemory(Vk::MemoryFlag::DeviceLocal, requirements.memories())}};
    VkDeviceMemory handle = memory.handle();
    CORRADE_VERIFY(handle);

    buffer.bindDedicatedMemory(std::move(memory));
    CORRADE_VERIFY(buffer.hasDedicatedMemory());
    CORRADE_COMPARE(buffer.dedicatedMemory().handle(), handle);
}

void BufferVkTest::directAllocation() {
    Buffer buffer{device(),
        BufferCreateInfo{BufferUsage::StorageBuffer, 16384},
        Vk::MemoryFlag::DeviceLocal};

    /* Not sure what else to test here */
    CORRADE_VERIFY(buffer.hasDedicatedMemory());
    CORRADE_VERIFY(buffer.dedicatedMemory().handle());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::BufferVkTest)
