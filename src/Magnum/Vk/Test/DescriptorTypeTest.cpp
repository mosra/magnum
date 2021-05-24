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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/DescriptorType.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DescriptorTypeTest: TestSuite::Tester {
    explicit DescriptorTypeTest();

    void debug();
};

DescriptorTypeTest::DescriptorTypeTest() {
    addTests({&DescriptorTypeTest::debug});
}

void DescriptorTypeTest::debug() {
    std::ostringstream out;
    Debug{&out} << DescriptorType::InputAttachment << DescriptorType(-10007655);
    CORRADE_COMPARE(out.str(), "Vk::DescriptorType::InputAttachment Vk::DescriptorType(-10007655)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DescriptorTypeTest)
