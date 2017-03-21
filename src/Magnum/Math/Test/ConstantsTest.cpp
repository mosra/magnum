/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <cmath>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Constants.h"
#include "Magnum/Math/Functions.h"

namespace Magnum { namespace Math { namespace Test {

struct ConstantsTest: Corrade::TestSuite::Tester {
    explicit ConstantsTest();

    template<class T> void constants();
    template<class T> void specials();
};

ConstantsTest::ConstantsTest() {
    addTests<ConstantsTest>({&ConstantsTest::constants<Float>,
                             &ConstantsTest::constants<Double>,
                             &ConstantsTest::specials<Float>,
                             &ConstantsTest::specials<Double>});
}

template<class T> void ConstantsTest::constants() {
    setTestCaseName(std::is_same<T, Double>::value ? "constants<Double>" : "constants<Float>");

    constexpr T a = Constants<T>::sqrt2();
    constexpr T b = Constants<T>::sqrt3();
    CORRADE_COMPARE(Math::pow<2>(a), T(2));
    CORRADE_COMPARE(Math::pow<2>(b), T(3));

    constexpr T c = Constants<T>::pi();
    constexpr T d = Constants<T>::piHalf();
    constexpr T e = Constants<T>::tau();
    CORRADE_COMPARE(T(0.5)*c, d);
    CORRADE_COMPARE(T(2.0)*c, e);

    constexpr T f = Constants<T>::e();
    CORRADE_COMPARE(std::log(f), T(1));
}

template<class T> void ConstantsTest::specials() {
    setTestCaseName(std::is_same<T, Double>::value ? "specials<Double>" : "specials<Float>");

    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* NaN is not constexpr */
    constexpr
    #endif
    T a = Constants<T>::nan();
    CORRADE_VERIFY(std::isnan(a));
    CORRADE_VERIFY(a != a);

    constexpr T b = Constants<T>::inf();
    CORRADE_VERIFY(std::isinf(b));

    /* Clang complains that producing NaN is not constexpr */
    T h = Constants<T>::inf() - Constants<T>::inf();
    CORRADE_VERIFY(h != h);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::ConstantsTest)
