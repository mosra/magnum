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

#include "Magnum/Vk/Instance.h"
#include <Corrade/TestSuite/Tester.h>


using namespace Corrade;

namespace Magnum { namespace Vk { namespace Test {

struct InstanceVkTest: TestSuite::Tester {
    explicit InstanceVkTest();

    void constructCopyMove();

    void createInstance();
    void createWithValidation();
};

InstanceVkTest::InstanceVkTest() {
    addTests({&InstanceVkTest::constructCopyMove,
              &InstanceVkTest::createInstance,
              &InstanceVkTest::createWithValidation});
}

void InstanceVkTest::constructCopyMove() {
    /* Only move-construction allowed */
    CORRADE_VERIFY(!(std::is_constructible<Instance, const Instance&>{}));
    CORRADE_VERIFY((std::is_constructible<Instance, Instance&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Instance, const Instance&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Instance, Instance&&>{}));
}

void InstanceVkTest::createInstance() {
    Vk::Instance Instance;

    CORRADE_VERIFY(&Vk::Instance::current() != nullptr);
    CORRADE_VERIFY(Vk::Instance::hasCurrent());
    CORRADE_COMPARE(Instance.version(), Vk::Version::Vulkan_1_0);
}

void InstanceVkTest::createWithValidation() {
    Vk::Instance Instance{Vk::Instance::Flag::EnableValidation};
    CORRADE_VERIFY(Vk::Instance::hasCurrent());
    CORRADE_COMPARE(Instance.version(), Vk::Version::Vulkan_1_0);
}

}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::InstanceVkTest)
