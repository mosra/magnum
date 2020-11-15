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

#ifndef CORRADE_STANDARD_ASSERT
#define CORRADE_NO_ASSERT
#endif

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Result.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct AssertDisabledTest: TestSuite::Tester {
    explicit AssertDisabledTest();

    void success();
    void successOrIncomplete();
    void vkSuccess();
    void vkSuccessOrIncomplete();
};

AssertDisabledTest::AssertDisabledTest() {
    addTests({&AssertDisabledTest::success,
              &AssertDisabledTest::successOrIncomplete,
              &AssertDisabledTest::vkSuccess,
              &AssertDisabledTest::vkSuccessOrIncomplete});

    #ifdef CORRADE_STANDARD_ASSERT
    setTestName("Magum::Vk::Test::AssertStandardDisabledTest");
    #endif
}

void AssertDisabledTest::success() {
    std::ostringstream out;
    Error redirectError{&out};

    Result a = Result::ErrorUnknown;
    Result r = Result::ErrorFragmentedPool;
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(a = r);
    CORRADE_COMPARE(a, Result::ErrorFragmentedPool);

    CORRADE_COMPARE(out.str(), "");
}

void AssertDisabledTest::successOrIncomplete() {
    std::ostringstream out;
    Error redirectError{&out};

    Result a = Result::ErrorUnknown;
    Result r = Result::ErrorExtensionNotPresent;
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR_INCOMPLETE(a = r);
    CORRADE_COMPARE(a, Result::ErrorExtensionNotPresent);

    CORRADE_COMPARE(out.str(), "");
}

void AssertDisabledTest::vkSuccess() {
    std::ostringstream out;
    Error redirectError{&out};

    VkResult a = VK_ERROR_UNKNOWN;
    VkResult r = VK_ERROR_FRAGMENTED_POOL;
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(a = r);
    CORRADE_COMPARE(Result(a), Result::ErrorFragmentedPool);

    CORRADE_COMPARE(out.str(), "");
}

void AssertDisabledTest::vkSuccessOrIncomplete() {
    std::ostringstream out;
    Error redirectError{&out};

    VkResult b = VK_ERROR_UNKNOWN;
    VkResult s = VK_ERROR_EXTENSION_NOT_PRESENT;
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR_INCOMPLETE(b = s);
    CORRADE_COMPARE(Result(b), Result::ErrorExtensionNotPresent);

    CORRADE_COMPARE(out.str(), "");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::AssertDisabledTest)
