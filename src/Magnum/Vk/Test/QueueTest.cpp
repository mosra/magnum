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
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Queue.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct QueueTest: TestSuite::Tester {
    explicit QueueTest();

    void constructNoCreate();
    void constructCopy();
    void constructMove();

    void wrap();

    void submitInfoConstruct();
    void submitInfoConstructNoInit();
    void submitInfoConstructCommandBuffers();
    void submitInfoConstructFromVk();
    void submitInfoConstructCopy();
    void submitInfoConstructMove();
};

QueueTest::QueueTest() {
    addTests({&QueueTest::constructNoCreate,
              &QueueTest::constructCopy,
              &QueueTest::constructMove,

              &QueueTest::wrap,

              &QueueTest::submitInfoConstruct,
              &QueueTest::submitInfoConstructNoInit,
              &QueueTest::submitInfoConstructCommandBuffers,
              &QueueTest::submitInfoConstructFromVk,
              &QueueTest::submitInfoConstructCopy,
              &QueueTest::submitInfoConstructMove});
}

void QueueTest::constructNoCreate() {
    {
        Queue queue{NoCreate};
        CORRADE_VERIFY(!queue.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Queue>::value));
}

void QueueTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<Queue>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Queue>{});
}

void QueueTest::constructMove() {
    Device device{NoCreate};
    Queue a = Queue::wrap(device, reinterpret_cast<VkQueue>(0xbadcafe));
    VkQueue handle = a.handle();
    CORRADE_VERIFY(a.handle());

    Queue b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);

    Queue c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(c.handle(), handle);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Queue>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Queue>::value);
}

void QueueTest::wrap() {
    /* Queues are not getting destroyed in any way, so it's enough to do it in
       a non-Vulkan-enabled test */

    Device device{NoCreate};
    auto vkQueue = reinterpret_cast<VkQueue>(0xbadcafe);

    Queue queue = Queue::wrap(device, vkQueue);
    CORRADE_COMPARE(queue.handle(), vkQueue);
}

void QueueTest::submitInfoConstruct() {
    SubmitInfo info;
    CORRADE_COMPARE(info->commandBufferCount, 0);
    CORRADE_VERIFY(!info->pCommandBuffers);
}

void QueueTest::submitInfoConstructNoInit() {
    SubmitInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) SubmitInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<SubmitInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, SubmitInfo>::value));
}

void QueueTest::submitInfoConstructCommandBuffers() {
    SubmitInfo info;
    info.setCommandBuffers({
        reinterpret_cast<VkCommandBuffer>(0xbadbeef),
        reinterpret_cast<VkCommandBuffer>(0xcafecafe)
    });

    CORRADE_COMPARE(info->commandBufferCount, 2);
    CORRADE_VERIFY(info->pCommandBuffers);
    CORRADE_COMPARE(info->pCommandBuffers[0], reinterpret_cast<VkCommandBuffer>(0xbadbeef));
    CORRADE_COMPARE(info->pCommandBuffers[1], reinterpret_cast<VkCommandBuffer>(0xcafecafe));
}

void QueueTest::submitInfoConstructFromVk() {
    VkSubmitInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    SubmitInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void QueueTest::submitInfoConstructCopy() {
    CORRADE_VERIFY(!(std::is_copy_constructible<SubmitInfo>{}));
    CORRADE_VERIFY(!(std::is_copy_assignable<SubmitInfo>{}));
}

void QueueTest::submitInfoConstructMove() {
    SubmitInfo a;
    a.setCommandBuffers({{}, reinterpret_cast<VkCommandBuffer>(0xcafecafe)});

    SubmitInfo b = std::move(a);
    CORRADE_COMPARE(a->commandBufferCount, 0);
    CORRADE_VERIFY(!a->pCommandBuffers);
    CORRADE_COMPARE(b->commandBufferCount, 2);
    CORRADE_VERIFY(b->pCommandBuffers);
    CORRADE_COMPARE(b->pCommandBuffers[1], reinterpret_cast<VkCommandBuffer>(0xcafecafe));

    SubmitInfo c{VkSubmitInfo{}};
    c = std::move(b);
    CORRADE_COMPARE(b->commandBufferCount, 0);
    CORRADE_VERIFY(!b->pCommandBuffers);
    CORRADE_COMPARE(c->commandBufferCount, 2);
    CORRADE_VERIFY(c->pCommandBuffers);
    CORRADE_COMPARE(c->pCommandBuffers[1], reinterpret_cast<VkCommandBuffer>(0xcafecafe));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::QueueTest)
