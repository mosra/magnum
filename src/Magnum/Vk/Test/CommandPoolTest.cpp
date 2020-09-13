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
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Vk/CommandPool.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct CommandPoolTest: TestSuite::Tester {
    explicit CommandPoolTest();

    void createInfoConstruct();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();

    void constructNoCreate();
    void constructCopy();
};

CommandPoolTest::CommandPoolTest() {
    addTests({&CommandPoolTest::createInfoConstruct,
              &CommandPoolTest::createInfoConstructNoInit,
              &CommandPoolTest::createInfoConstructFromVk,

              &CommandPoolTest::constructNoCreate,
              &CommandPoolTest::constructCopy});
}

void CommandPoolTest::createInfoConstruct() {
    CommandPoolCreateInfo info{37, CommandPoolCreateInfo::Flag::ResetCommandBuffer|CommandPoolCreateInfo::Flag::Transient};
    CORRADE_COMPARE(info->queueFamilyIndex, 37);
    CORRADE_COMPARE(info->flags, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT|VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

void CommandPoolTest::createInfoConstructNoInit() {
    CommandPoolCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) CommandPoolCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY((std::is_nothrow_constructible<CommandPoolCreateInfo, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, CommandPoolCreateInfo>::value));
}

void CommandPoolTest::createInfoConstructFromVk() {
    VkCommandPoolCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    CommandPoolCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void CommandPoolTest::constructNoCreate() {
    {
        CommandPool pool{NoCreate};
        CORRADE_VERIFY(!pool.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, CommandPool>::value));
}

void CommandPoolTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<CommandPool, const CommandPool&>{}));
    CORRADE_VERIFY(!(std::is_assignable<CommandPool, const CommandPool&>{}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::CommandPoolTest)
