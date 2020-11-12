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

    void debugMemoryFlag();
    void debugMemoryFlags();
};

MemoryTest::MemoryTest() {
    addTests({&MemoryTest::requirementsConstructNoInit,
              &MemoryTest::requirementsConstructFromVk,

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
