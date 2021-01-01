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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Queue.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct QueueTest: TestSuite::Tester {
    explicit QueueTest();

    void constructNoCreate();

    void wrap();
};

QueueTest::QueueTest() {
    addTests({&QueueTest::constructNoCreate,

              &QueueTest::wrap});
}

void QueueTest::constructNoCreate() {
    {
        Queue queue{NoCreate};
        CORRADE_VERIFY(!queue.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Queue>::value));
}

void QueueTest::wrap() {
    /* Queues are not getting destroyed in any way, so it's enough to do it in
       a non-Vulkan-enabled test */

    Device device{NoCreate};
    auto vkQueue = reinterpret_cast<VkQueue>(0xbadcafe);

    Queue queue = Queue::wrap(device, vkQueue);
    CORRADE_COMPARE(queue.handle(), vkQueue);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::QueueTest)
