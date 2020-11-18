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

#include <new>
#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/Buffer.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct BufferTest: TestSuite::Tester {
    explicit BufferTest();

    void createInfoConstruct();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();

    void constructNoCreate();
    void constructCopy();

    void dedicatedMemoryNotDedicated();
};

BufferTest::BufferTest() {
    addTests({&BufferTest::createInfoConstruct,
              &BufferTest::createInfoConstructNoInit,
              &BufferTest::createInfoConstructFromVk,

              &BufferTest::constructNoCreate,
              &BufferTest::constructCopy,

              &BufferTest::dedicatedMemoryNotDedicated});
}

void BufferTest::createInfoConstruct() {
    /** @todo use a real flag once at least one is exposed */
    BufferCreateInfo info{BufferUsage::UniformBuffer, 1024, BufferCreateInfo::Flag(VK_BUFFER_CREATE_PROTECTED_BIT)};
    CORRADE_COMPARE(info->flags, VK_BUFFER_CREATE_PROTECTED_BIT);
    CORRADE_COMPARE(info->size, 1024);
    CORRADE_COMPARE(info->usage, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

void BufferTest::createInfoConstructNoInit() {
    BufferCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) BufferCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<BufferCreateInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, BufferCreateInfo>::value));
}

void BufferTest::createInfoConstructFromVk() {
    VkBufferCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    BufferCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void BufferTest::constructNoCreate() {
    {
        Buffer buffer{NoCreate};
        CORRADE_VERIFY(!buffer.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Buffer>::value));
}

void BufferTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Buffer, const Buffer&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Buffer, const Buffer&>{}));
}

void BufferTest::dedicatedMemoryNotDedicated() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Buffer buffer{NoCreate};
    CORRADE_VERIFY(!buffer.hasDedicatedMemory());

    std::ostringstream out;
    Error redirectError{&out};
    buffer.dedicatedMemory();
    CORRADE_COMPARE(out.str(), "Vk::Buffer::dedicatedMemory(): buffer doesn't have a dedicated memory\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::BufferTest)
