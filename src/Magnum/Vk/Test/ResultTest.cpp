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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/Result.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ResultTest: TestSuite::Tester {
    explicit ResultTest();

    void assertResult();
    void assertVkResult();

    void debug();

    bool _failAssertResult, _failAssertVkResult;
};

ResultTest::ResultTest(): TestSuite::Tester{TesterConfiguration{}.setSkippedArgumentPrefixes({"fail-on"})} {
    addTests({&ResultTest::assertResult,
              &ResultTest::assertVkResult,

              &ResultTest::debug});

    Utility::Arguments args{"fail-on"};
    args.addOption("assert-result", "false").setHelp("assert-result", "fail on MAGNUM_VK_INTERNAL_ASSERT_RESULT() with Vk::Result", "BOOL")
        .addOption("assert-vk-result", "false").setHelp("assert-vk-result", "fail on MAGNUM_VK_INTERNAL_ASSERT_RESULT() with VkResult", "BOOL")
        .parse(arguments().first, arguments().second);

    _failAssertResult = args.value<bool>("assert-result");
    _failAssertVkResult = args.value<bool>("assert-vk-result");
}

void ResultTest::assertResult() {
    Result a = Result::ErrorUnknown;

    Result r = _failAssertResult ? Result::ErrorFragmentedPool : Result::Success;
    MAGNUM_VK_INTERNAL_ASSERT_RESULT(a = r);

    CORRADE_COMPARE(a, Result::Success);
}

void ResultTest::assertVkResult() {
    VkResult a = VK_ERROR_UNKNOWN;

    VkResult r = _failAssertVkResult ? VK_ERROR_FRAGMENTED_POOL : VK_SUCCESS;
    MAGNUM_VK_INTERNAL_ASSERT_RESULT(a = r);

    CORRADE_COMPARE(Result(a), Result::Success);
}

void ResultTest::debug() {
    std::ostringstream out;
    Debug{&out} << Result::ErrorExtensionNotPresent << Result(-10007655);
    CORRADE_COMPARE(out.str(), "Vk::Result::ErrorExtensionNotPresent Vk::Result(-10007655)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ResultTest)
