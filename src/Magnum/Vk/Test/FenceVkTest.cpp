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

#include <Corrade/Containers/Reference.h>

#include "Magnum/Vk/FenceCreateInfo.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct FenceVkTest: VulkanTester {
    explicit FenceVkTest();

    void construct();
    void constructMove();

    void wrap();

    void status();
    void reset();
    void wait100ms();
    void wait();
};

FenceVkTest::FenceVkTest() {
    addTests({&FenceVkTest::construct,
              &FenceVkTest::constructMove,

              &FenceVkTest::wrap,

              &FenceVkTest::status,
              &FenceVkTest::reset});

    addBenchmarks({&FenceVkTest::wait100ms}, 1);

    addTests({&FenceVkTest::wait});
}

void FenceVkTest::construct() {
    {
        Fence fence{device(), FenceCreateInfo{FenceCreateInfo::Flag::Signaled}};
        CORRADE_VERIFY(fence.handle());
        CORRADE_COMPARE(fence.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void FenceVkTest::constructMove() {
    Fence a{device()};
    VkFence handle = a.handle();

    Fence b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);

    Fence c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Fence>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Fence>::value);
}

void FenceVkTest::wrap() {
    VkFence fence{};
    CORRADE_COMPARE(Result(device()->CreateFence(device(),
        FenceCreateInfo{},
        nullptr, &fence)), Result::Success);

    auto wrapped = Fence::wrap(device(), fence, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), fence);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), fence);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyFence(device(), fence, nullptr);
}

void FenceVkTest::status() {
    Fence a{device()};
    CORRADE_VERIFY(!a.status());

    Fence b{device(), FenceCreateInfo{FenceCreateInfo::Flag::Signaled}};
    CORRADE_VERIFY(b.status());
}

void FenceVkTest::reset() {
    Fence a{device(), FenceCreateInfo{FenceCreateInfo::Flag::Signaled}};
    CORRADE_VERIFY(a.status());

    a.reset();
    CORRADE_VERIFY(!a.status());
}

void FenceVkTest::wait100ms() {
    Fence a{device()};
    CORRADE_VERIFY(!a.status());

    /* A benchmark so we have at least some verification we're not terribly off
       with the units */
    CORRADE_BENCHMARK(1)
        CORRADE_VERIFY(!a.wait(std::chrono::milliseconds{100}));

    CORRADE_VERIFY(!a.status());
}

void FenceVkTest::wait() {
    Fence a{device(), FenceCreateInfo{FenceCreateInfo::Flag::Signaled}};
    CORRADE_VERIFY(a.status());
    a.wait();
    CORRADE_VERIFY(a.status());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::FenceVkTest)
