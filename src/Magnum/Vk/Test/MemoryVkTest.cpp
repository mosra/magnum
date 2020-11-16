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

#include <Corrade/Containers/Array.h>

#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Memory.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct MemoryVkTest: VulkanTester {
    explicit MemoryVkTest();

    void construct();
    void constructMove();

    void wrap();

    void map();
    void mapRead();
};

MemoryVkTest::MemoryVkTest() {
    addTests({&MemoryVkTest::construct,
              &MemoryVkTest::constructMove,

              &MemoryVkTest::wrap,

              &MemoryVkTest::map,
              &MemoryVkTest::mapRead});
}

void MemoryVkTest::construct() {
    Memory memory{device(), MemoryAllocateInfo{1024*1024, device().properties().pickMemory(MemoryFlag::DeviceLocal)}};
    CORRADE_VERIFY(memory.handle());
    CORRADE_COMPARE(memory.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(memory.size(), 1024*1024);
}

void MemoryVkTest::constructMove() {
    Memory a{device(), MemoryAllocateInfo{1024*1024, device().properties().pickMemory(MemoryFlag::DeviceLocal)}};
    VkDeviceMemory handle = a.handle();

    Memory b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(b.size(), 1024*1024);

    Memory c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(c.size(), 1024*1024);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Memory>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Memory>::value);
}

void MemoryVkTest::wrap() {
    VkDeviceMemory memory{};
    CORRADE_COMPARE(Result(device()->AllocateMemory(device(),
        MemoryAllocateInfo{1024*1024, device().properties().pickMemory(MemoryFlag::DeviceLocal)},
        nullptr, &memory)), Result::Success);
    CORRADE_VERIFY(memory);

    auto wrapped = Memory::wrap(device(), memory, 1024*1024, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), memory);
    CORRADE_COMPARE(wrapped.size(), 1024*1024);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), memory);
    CORRADE_VERIFY(!wrapped.handle());
    CORRADE_COMPARE(wrapped.size(), 0);
    device()->FreeMemory(device(), memory, nullptr);
}

void MemoryVkTest::map() {
    Memory a{device(), MemoryAllocateInfo{1024*1024, device().properties().pickMemory(MemoryFlag::HostVisible)}};

    /* Map and write */
    {
        Containers::Array<char, MemoryMapDeleter> mapped = a.map();
        CORRADE_COMPARE(mapped.size(), 1024*1024);
        mapped[1024 + 37] = 'c';
    }

    /* Map a subrange again -- shouldn't fail since we unmapped implicitly
       above */
    {
        Containers::Array<char, MemoryMapDeleter> mapped = a.map(1024, 100);
        CORRADE_COMPARE(mapped.size(), 100);
        CORRADE_COMPARE(mapped[37], 'c');
    }
}

void MemoryVkTest::mapRead() {
    Memory a{device(), MemoryAllocateInfo{1024*1024, device().properties().pickMemory(MemoryFlag::HostVisible)}};

    /* Map and write, unmap should be implicit */
    {
        Containers::Array<const char, MemoryMapDeleter> mapped = a.mapRead();
        CORRADE_COMPARE(mapped.size(), 1024*1024);
    }

    /* Map a subrange again */
    {
        Containers::Array<const char, MemoryMapDeleter> mapped = a.mapRead(1024, 100);
        CORRADE_COMPARE(mapped.size(), 100);
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::MemoryVkTest)
