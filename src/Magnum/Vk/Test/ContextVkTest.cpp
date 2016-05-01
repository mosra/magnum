/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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
#include <algorithm>

#include "Magnum/Vk/Context.h"
#include "AbstractVulkanTester.h"


namespace Magnum { namespace Vk { namespace Test {

struct ContextVkTest: AbstractVulkanTester {
    explicit ContextVkTest();

    void constructCopyMove();

    void createInstance();
    void createWithValidation();
};

ContextVkTest::ContextVkTest() {
    addTests({&ContextVkTest::constructCopyMove,
              &ContextVkTest::createInstance,
              &ContextVkTest::createWithValidation});
}

void ContextVkTest::constructCopyMove() {
    /* Only move-construction allowed */
    CORRADE_VERIFY(!(std::is_constructible<Context, const Context&>{}));
    CORRADE_VERIFY((std::is_constructible<Context, Context&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Context, const Context&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Context, Context&&>{}));
}

void ContextVkTest::createInstance() {
    Vk::Context context;

    CORRADE_VERIFY(&Vk::Context::current() != nullptr);
    CORRADE_VERIFY(Vk::Context::hasCurrent());
    CORRADE_COMPARE(context.version(), Vk::Version::Vulkan_1_0);
}

void ContextVkTest::createWithValidation() {
    Vk::Context context{Vk::Context::Flag::EnableValidation};
    CORRADE_VERIFY(Vk::Context::hasCurrent());
    CORRADE_COMPARE(context.version(), Vk::Version::Vulkan_1_0);
}

}}}

MAGNUM_VK_TEST_MAIN(Magnum::Vk::Test::ContextVkTest)
