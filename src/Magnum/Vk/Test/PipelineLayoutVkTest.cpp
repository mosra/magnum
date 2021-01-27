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

#include "Magnum/Vk/PipelineLayoutCreateInfo.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct PipelineLayoutVkTest: VulkanTester {
    explicit PipelineLayoutVkTest();

    void construct();
    void constructMove();

    void wrap();
};

PipelineLayoutVkTest::PipelineLayoutVkTest() {
    addTests({&PipelineLayoutVkTest::construct,
              &PipelineLayoutVkTest::constructMove,

              &PipelineLayoutVkTest::wrap});
}

void PipelineLayoutVkTest::construct() {
    {
        PipelineLayout layout{device(), PipelineLayoutCreateInfo{}};
        CORRADE_VERIFY(layout.handle());
        CORRADE_COMPARE(layout.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void PipelineLayoutVkTest::constructMove() {
    PipelineLayout a{device(), PipelineLayoutCreateInfo{}};
    VkPipelineLayout handle = a.handle();

    PipelineLayout b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);

    PipelineLayout c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<PipelineLayout>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<PipelineLayout>::value);
}

void PipelineLayoutVkTest::wrap() {
    VkPipelineLayout layout{};
    CORRADE_COMPARE(Result(device()->CreatePipelineLayout(device(),
        PipelineLayoutCreateInfo{},
        nullptr, &layout)), Result::Success);

    auto wrapped = PipelineLayout::wrap(device(), layout, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), layout);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), layout);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyPipelineLayout(device(), layout, nullptr);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::PipelineLayoutVkTest)
