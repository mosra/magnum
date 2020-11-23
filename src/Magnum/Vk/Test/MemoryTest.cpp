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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/Memory.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct MemoryTest: TestSuite::Tester {
    explicit MemoryTest();

    void requirementsConstructNoInit();
    void requirementsConstructFromVk();

    void requirementsAlignedSize();
    void requirementsAlignedSizeZeroAlignement();

    void allocateInfoConstruct();
    void allocateInfoConstructNoInit();
    void allocateInfoConstructFromVk();

    void constructNoCreate();
    void constructCopy();

    void debugMemoryFlag();
    void debugMemoryFlags();
};

MemoryTest::MemoryTest() {
    addTests({&MemoryTest::requirementsConstructNoInit,
              &MemoryTest::requirementsConstructFromVk,

              &MemoryTest::requirementsAlignedSize,
              &MemoryTest::requirementsAlignedSizeZeroAlignement,

              &MemoryTest::allocateInfoConstruct,
              &MemoryTest::allocateInfoConstructNoInit,
              &MemoryTest::allocateInfoConstructFromVk,

              &MemoryTest::constructNoCreate,
              &MemoryTest::constructCopy,

              &MemoryTest::debugMemoryFlag,
              &MemoryTest::debugMemoryFlags});
}

void MemoryTest::requirementsConstructNoInit() {
    MemoryRequirements requirements{NoInit};
    requirements->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    new(&requirements) MemoryRequirements{NoInit};
    CORRADE_COMPARE(requirements->sType, VK_STRUCTURE_TYPE_APPLICATION_INFO);

    CORRADE_VERIFY((std::is_nothrow_constructible<MemoryRequirements, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, MemoryRequirements>::value));
}

void MemoryTest::requirementsConstructFromVk() {
    VkMemoryRequirements2 vkRequirements;
    vkRequirements.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    MemoryRequirements requirements{vkRequirements};
    CORRADE_COMPARE(requirements->sType, VK_STRUCTURE_TYPE_APPLICATION_INFO);
}

void MemoryTest::requirementsAlignedSize() {
    /* Creating from a raw Vulkan structure because there's no other way */
    VkMemoryRequirements2 vkRequirements{};
    vkRequirements.memoryRequirements.size = 13765;
    CORRADE_COMPARE(MemoryRequirements{vkRequirements}.alignedSize(4096), 16384);

    vkRequirements.memoryRequirements.size = 16383;
    CORRADE_COMPARE(MemoryRequirements{vkRequirements}.alignedSize(4096), 16384);

    vkRequirements.memoryRequirements.size = 16384;
    CORRADE_COMPARE(MemoryRequirements{vkRequirements}.alignedSize(4096), 16384);

    vkRequirements.memoryRequirements.size = 0;
    CORRADE_COMPARE(MemoryRequirements{vkRequirements}.alignedSize(4096), 0);
}

void MemoryTest::requirementsAlignedSizeZeroAlignement() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    VkMemoryRequirements2 vkRequirements{};
    vkRequirements.memoryRequirements.size = 16384;

    std::ostringstream out;
    Error redirectError{&out};
    MemoryRequirements{vkRequirements}.alignedSize(0);
    CORRADE_COMPARE(out.str(), "Vk::MemoryRequirements::alignedSize(): alignment can't be zero\n");
}

void MemoryTest::allocateInfoConstruct() {
    MemoryAllocateInfo info{65536, 1};
    CORRADE_COMPARE(info->allocationSize, 65536);
    CORRADE_COMPARE(info->memoryTypeIndex, 1);
}

void MemoryTest::allocateInfoConstructNoInit() {
    MemoryAllocateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) MemoryAllocateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<MemoryAllocateInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, MemoryAllocateInfo>::value));
}

void MemoryTest::allocateInfoConstructFromVk() {
    VkMemoryAllocateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    MemoryAllocateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void MemoryTest::constructNoCreate() {
    {
        Memory memory{NoCreate};
        CORRADE_VERIFY(!memory.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Memory>::value));
}

void MemoryTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Memory, const Memory&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Memory, const Memory&>{}));
}

void MemoryTest::debugMemoryFlag() {
    std::ostringstream out;
    Debug{&out} << MemoryFlag::HostCached << MemoryFlag(0xdeadcafe);
    CORRADE_COMPARE(out.str(), "Vk::MemoryFlag::HostCached Vk::MemoryFlag(0xdeadcafe)\n");
}

void MemoryTest::debugMemoryFlags() {
    std::ostringstream out;
    Debug{&out} << (MemoryFlag::HostCached|MemoryFlag::LazilyAllocated) << MemoryFlags{};
    CORRADE_COMPARE(out.str(), "Vk::MemoryFlag::HostCached|Vk::MemoryFlag::LazilyAllocated Vk::MemoryFlags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::MemoryTest)
