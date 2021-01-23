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

#include <new>
#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/BufferCreateInfo.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct BufferTest: TestSuite::Tester {
    explicit BufferTest();

    void createInfoConstruct();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();

    void constructNoCreate();
    void constructCopy();

    void dedicatedMemoryNotDedicated();

    /* While *ConstructFromVk() tests that going from VkFromThing -> Vk::Thing
       -> VkToThing doesn't result in information loss, the *ConvertToVk()
       tests additionally check that all calls both on our APIs and by editing
       the contained structure are correctly propagated to the resulting
       structures. */

    void bufferCopyConstruct();
    void bufferCopyConstructNoInit();
    template<class From, class To> void bufferCopyConstructFromVk();
    template<class T> void bufferCopyConvertToVk();
    void bufferCopyConvertDisallowed();

    void copyBufferInfoConstruct();
    void copyBufferInfoConstructNoInit();
    void copyBufferInfoConstructFromVk();
    void copyBufferInfoConvertToVk();
};

BufferTest::BufferTest() {
    addTests({&BufferTest::createInfoConstruct,
              &BufferTest::createInfoConstructNoInit,
              &BufferTest::createInfoConstructFromVk,

              &BufferTest::constructNoCreate,
              &BufferTest::constructCopy,

              &BufferTest::dedicatedMemoryNotDedicated,

              &BufferTest::bufferCopyConstruct,
              &BufferTest::bufferCopyConstructNoInit,
              &BufferTest::bufferCopyConstructFromVk<VkBufferCopy2KHR, VkBufferCopy2KHR>,
              &BufferTest::bufferCopyConstructFromVk<VkBufferCopy, VkBufferCopy2KHR>,
              &BufferTest::bufferCopyConstructFromVk<VkBufferCopy2KHR, VkBufferCopy>,
              &BufferTest::bufferCopyConstructFromVk<VkBufferCopy, VkBufferCopy>,
              &BufferTest::bufferCopyConvertToVk<VkBufferCopy2KHR>,
              &BufferTest::bufferCopyConvertToVk<VkBufferCopy>,
              &BufferTest::bufferCopyConvertDisallowed,

              &BufferTest::copyBufferInfoConstruct,
              &BufferTest::copyBufferInfoConstructNoInit,
              &BufferTest::copyBufferInfoConstructFromVk,
              &BufferTest::copyBufferInfoConvertToVk});
}

template<class> struct Traits;
#define _c(type)                                                            \
    template<> struct Traits<Vk ## type> {                                  \
        static const char* name() { return #type; }                         \
        static Vk ## type convert(const type& instance) {                   \
            return instance.vk ## type ();                                  \
        }                                                                   \
    };                                                                      \
    template<> struct Traits<Vk ## type ## 2KHR> {                          \
        static const char* name() { return #type "2KHR"; }                  \
        static Vk ## type ## 2KHR convert(const type& instance) {           \
            return instance;                                                \
        }                                                                   \
    };
_c(BufferCopy)
#undef _c

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
    CORRADE_VERIFY(!std::is_copy_constructible<Buffer>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Buffer>{});
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

void BufferTest::bufferCopyConstruct() {
    BufferCopy copy{3, 5, 7};
    CORRADE_COMPARE(copy->srcOffset, 3);
    CORRADE_COMPARE(copy->dstOffset, 5);
    CORRADE_COMPARE(copy->size, 7);
}

void BufferTest::bufferCopyConstructNoInit() {
    BufferCopy copy{NoInit};
    copy->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&copy) BufferCopy{NoInit};
    CORRADE_COMPARE(copy->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<BufferCopy, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, BufferCopy>::value));
}

template<class From, class To> void BufferTest::bufferCopyConstructFromVk() {
    setTestCaseTemplateName({Traits<From>::name(), Traits<To>::name()});

    From from{};
    from.srcOffset = 3;
    from.dstOffset = 5;
    from.size = 7;

    BufferCopy copy{from};
    To to = Traits<To>::convert(copy);
    CORRADE_COMPARE(to.srcOffset, 3);
    CORRADE_COMPARE(to.dstOffset, 5);
    CORRADE_COMPARE(to.size, 7);
}

template<class T> void BufferTest::bufferCopyConvertToVk() {
    BufferCopy copy{3, 5, 7};

    T out = Traits<T>::convert(copy);
    CORRADE_COMPARE(out.srcOffset, 3);
    CORRADE_COMPARE(out.dstOffset, 5);
    CORRADE_COMPARE(out.size, 7);
}

void BufferTest::bufferCopyConvertDisallowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    BufferCopy copy{0, 0, 0};
    copy->pNext = &copy;

    std::ostringstream out;
    Error redirectError{&out};
    copy.vkBufferCopy();
    CORRADE_COMPARE(out.str(), "Vk::BufferCopy: disallowing conversion to VkBufferCopy with non-empty pNext to prevent information loss\n");
}

void BufferTest::copyBufferInfoConstruct() {
    auto a = reinterpret_cast<VkBuffer>(0xdead);
    auto b = reinterpret_cast<VkBuffer>(0xcafe);

    CopyBufferInfo info{a, b, {
        {3, 0, 0},
        {0, 5, 0}
    }};
    CORRADE_COMPARE(info->srcBuffer, a);
    CORRADE_COMPARE(info->dstBuffer, b);
    CORRADE_COMPARE(info->regionCount, 2);
    CORRADE_VERIFY(info->pRegions);
    CORRADE_COMPARE(info->pRegions[0].srcOffset, 3);
    CORRADE_COMPARE(info->pRegions[1].dstOffset, 5);
}

void BufferTest::copyBufferInfoConstructNoInit() {
    CopyBufferInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) CopyBufferInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<CopyBufferInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, CopyBufferInfo>::value));
}

void BufferTest::copyBufferInfoConstructFromVk() {
    VkCopyBufferInfo2KHR vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    CopyBufferInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void BufferTest::copyBufferInfoConvertToVk() {
    CopyBufferInfo info{VkBuffer{}, VkBuffer{}, {
        {3, 0, 0},
        {0, 5, 0}
    }};

    Containers::Array<VkBufferCopy> copies = info.vkBufferCopies();
    CORRADE_COMPARE(copies.size(), 2);
    CORRADE_COMPARE(copies[0].srcOffset, 3);
    CORRADE_COMPARE(copies[1].dstOffset, 5);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::BufferTest)
