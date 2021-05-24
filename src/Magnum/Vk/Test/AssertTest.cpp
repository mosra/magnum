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

#ifdef CORRADE_STANDARD_ASSERT
#undef NDEBUG /* So we can test them */
#endif

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Arguments.h>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Result.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct AssertTest: TestSuite::Tester {
    explicit AssertTest();

    void success();
    void successOr();
    void vkSuccess();
    void vkSuccessOr();

    bool _failAssertSuccess, _failAssertSuccessOr,
        _failAssertVkSuccess, _failAssertVkSuccessOr;
};

AssertTest::AssertTest(): TestSuite::Tester{TesterConfiguration{}.setSkippedArgumentPrefixes({"fail-on"})} {
    addTests({&AssertTest::success,
              &AssertTest::successOr,
              &AssertTest::vkSuccess,
              &AssertTest::vkSuccessOr});

    Utility::Arguments args{"fail-on"};
    args.addOption("assert-success", "false").setHelp("assert-success", "fail on MAGNUM_VK_INTERNAL_ASSERT_SUCCESS() with Vk::Result", "BOOL")
        .addOption("assert-success-or", "false").setHelp("assert-success-or", "fail on MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR() with Vk::Result", "BOOL")
        .addOption("assert-vk-success", "false").setHelp("assert-vk-success", "fail on MAGNUM_VK_INTERNAL_ASSERT_SUCCESS() with VkResult", "BOOL")
        .addOption("assert-vk-success-or", "false").setHelp("assert-vk-success-or", "fail on MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR() with VkResult", "BOOL")
        .parse(arguments().first, arguments().second);

    _failAssertSuccess = args.value<bool>("assert-success");
    _failAssertSuccessOr = args.value<bool>("assert-success-or");
    _failAssertVkSuccess = args.value<bool>("assert-vk-success");
    _failAssertVkSuccessOr = args.value<bool>("assert-vk-success-or");

    #ifdef CORRADE_STANDARD_ASSERT
    setTestName("Magum::Vk::Test::AssertStandardTest");
    #endif
}

void AssertTest::success() {
    Result a = Result::ErrorUnknown;

    Result r = _failAssertSuccess ? Result::ErrorFragmentedPool : Result::Success;
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(a = r);

    CORRADE_COMPARE(a, Result::Success);
}

void AssertTest::successOr() {
    Result a = Result::ErrorUnknown;
    Result a2 = MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR(a = Result::Success, Result::Incomplete);
    CORRADE_COMPARE(a2, a);

    Result r = _failAssertSuccessOr ? Result::ErrorExtensionNotPresent : Result::Incomplete;
    /* Verify that multiple results work too and all get checked */
    Result a3 = MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR(a = r, Result::ErrorOutOfDeviceMemory, Result::Incomplete);

    CORRADE_COMPARE(a, Result::Incomplete);
    CORRADE_COMPARE(a3, a);

    /* Test also that a standalone macro won't cause warnings about unused
       expression results */
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR(Result::ErrorDeviceLost, Result::ErrorDeviceLost);
}

void AssertTest::vkSuccess() {
    VkResult a = VK_ERROR_UNKNOWN;

    VkResult s = _failAssertVkSuccess ? VK_ERROR_FRAGMENTED_POOL : VK_SUCCESS;
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(a = s);

    CORRADE_COMPARE(Result(a), Result::Success);
}

void AssertTest::vkSuccessOr() {
    VkResult a = VK_ERROR_UNKNOWN;
    Result a2 = MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR(a = VK_SUCCESS, Result::Incomplete);
    CORRADE_COMPARE(a2, Result(a));

    VkResult s = _failAssertVkSuccessOr ? VK_ERROR_EXTENSION_NOT_PRESENT : VK_INCOMPLETE;
    /* Verify that multiple results work too and all get checked */
    Result a3 = MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR(a = s, Result::ErrorOutOfDeviceMemory, Result::Incomplete);

    CORRADE_COMPARE(Result(a), Result::Incomplete);
    CORRADE_COMPARE(a3, Result(a));

    /* Test also that a standalone macro won't cause warnings about unused
       expression results */
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR(VK_ERROR_DEVICE_LOST, Result::ErrorDeviceLost);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::AssertTest)
