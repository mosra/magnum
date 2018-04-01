/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>

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

/* Needs to be here otherwise the deprecation warning inside OptionalConverter
   is not ignored */
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

#include <memory>
#include <Corrade/Containers/Optional.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Magnum.h"
#include "MagnumExternal/Optional/OptionalWrapper.h"

namespace Magnum { namespace Test {

struct StdOptionalTest: TestSuite::Tester {
    explicit StdOptionalTest();

    void conversion();
};

StdOptionalTest::StdOptionalTest() {
    addTests({&StdOptionalTest::conversion});
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
void StdOptionalTest::conversion() {
    Debug{} << "Using C++ version" << CORRADE_CXX_STANDARD;
    #ifdef __has_include
    Debug{} << "__has_include is supported";
    #if __has_include(<optional>)
    Debug{} << "<optional> header is present";
    #else
    Debug{} << "<optional> header is not present";
    #endif
    #if __has_include(<experimental/optional>)
    Debug{} << "<experimental/optional> header is present";
    #else
    Debug{} << "<experimental/optional> header is not present";
    #endif
    #else
    Debug{} << "__has_include is not supported";
    #endif
    #ifdef _MAGNUM_HAS_STD_OPTIONAL
    Debug{} << "Using a conversion to std::optional, C++17 should be present";
    CORRADE_VERIFY(CORRADE_CXX_STANDARD >= 201703L);
    #else
    Debug{} << "Using a typedef to std::optional, C++17 should not be present";
    CORRADE_VERIFY(CORRADE_CXX_STANDARD < 201703L);
    #endif

    Containers::Optional<int> a{5};
    Containers::Optional<int> b;

    std::optional<int> sa = a;
    CORRADE_COMPARE(*sa, 5);
    std::optional<int> sb = b;
    CORRADE_VERIFY(!sb);
    std::optional<int> empty = std::nullopt;
    CORRADE_VERIFY(!empty);

    Containers::Optional<std::unique_ptr<int>> c{std::unique_ptr<int>{new int{7}}};
    std::optional<std::unique_ptr<int>> sc = std::move(c);
    CORRADE_COMPARE(**sc, 7);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

}}

CORRADE_TEST_MAIN(Magnum::Test::StdOptionalTest)

#ifdef _MSC_VER
#pragma warning(pop)
#endif
