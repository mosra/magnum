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

#include "Magnum/Vk/FenceCreateInfo.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct FenceTest: TestSuite::Tester {
    explicit FenceTest();

    void createInfoConstruct();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();

    void constructNoCreate();
    void constructCopy();
};

FenceTest::FenceTest() {
    addTests({&FenceTest::createInfoConstruct,
              &FenceTest::createInfoConstructNoInit,
              &FenceTest::createInfoConstructFromVk,

              &FenceTest::constructNoCreate,
              &FenceTest::constructCopy});
}

void FenceTest::createInfoConstruct() {
    FenceCreateInfo info{FenceCreateInfo::Flag::Signaled};
    CORRADE_COMPARE(info->flags, VK_FENCE_CREATE_SIGNALED_BIT);
}

void FenceTest::createInfoConstructNoInit() {
    FenceCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) FenceCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<FenceCreateInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, FenceCreateInfo>::value));
}

void FenceTest::createInfoConstructFromVk() {
    VkFenceCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    FenceCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void FenceTest::constructNoCreate() {
    {
        Fence fence{NoCreate};
        CORRADE_VERIFY(!fence.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Fence>::value));
}

void FenceTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<Fence>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Fence>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::FenceTest)
