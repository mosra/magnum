/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <TestSuite/Tester.h>

#include "Math/Unit.h"

namespace Magnum { namespace Math { namespace Test {

class UnitTest: public Corrade::TestSuite::Tester {
    public:
        explicit UnitTest();

        void construct();
        void constructDefault();
        void constructConversion();
        void compare();

        void negated();
        void addSubtract();
        void multiplyDivide();
};

UnitTest::UnitTest() {
    addTests({&UnitTest::construct,
              &UnitTest::constructDefault,
              &UnitTest::constructConversion,
              &UnitTest::compare,

              &UnitTest::negated,
              &UnitTest::addSubtract,
              &UnitTest::multiplyDivide});
}

template<class> struct Sec_;
typedef Unit<Sec_, Float> Sec;
typedef Unit<Sec_, Double> Secd;

inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, Sec value) {
    return debug << Float(value);
}

void UnitTest::construct() {
    constexpr Sec a(25.0f);
    CORRADE_COMPARE(Float(a), 25.0f);
}

void UnitTest::constructDefault() {
    constexpr Sec b;
    CORRADE_COMPARE(b, Sec(0.0f));
}

void UnitTest::constructConversion() {
    constexpr Secd a(25.0);
    constexpr Sec b(a);
    CORRADE_COMPARE(b, Sec(25.0f));
}

void UnitTest::compare() {
    CORRADE_VERIFY(Sec(25.0f + TypeTraits<Float>::epsilon()/2) == Sec(25.0f));
    CORRADE_VERIFY(Sec(25.0f + TypeTraits<Float>::epsilon()*2) != Sec(25.0f));

    constexpr bool c = Sec(3.0f) < Sec(3.0f);
    constexpr bool d = Sec(3.0f) <= Sec(3.0f);
    constexpr bool e = Sec(3.0f) >= Sec(3.0f);
    constexpr bool f = Sec(3.0f) > Sec(3.0f);
    CORRADE_VERIFY(!c);
    CORRADE_VERIFY(d);
    CORRADE_VERIFY(e);
    CORRADE_VERIFY(!f);

    constexpr bool h = Sec(2.0f) < Sec(3.0f);
    constexpr bool i = Sec(2.0f) <= Sec(3.0f);
    constexpr bool j = Sec(3.0f) >= Sec(2.0f);
    constexpr bool k = Sec(3.0f) > Sec(2.0f);
    CORRADE_VERIFY(h);
    CORRADE_VERIFY(i);
    CORRADE_VERIFY(j);
    CORRADE_VERIFY(k);

    constexpr bool l = Sec(3.0f) < Sec(2.0f);
    constexpr bool m = Sec(3.0f) <= Sec(2.0f);
    constexpr bool n = Sec(2.0f) >= Sec(3.0f);
    constexpr bool o = Sec(2.0f) > Sec(3.0f);
    CORRADE_VERIFY(!l);
    CORRADE_VERIFY(!m);
    CORRADE_VERIFY(!n);
    CORRADE_VERIFY(!o);
}

void UnitTest::negated() {
    constexpr Sec a(25.0f);
    constexpr Sec b(-a);
    CORRADE_COMPARE(b, Sec(-25.0f));
}

void UnitTest::addSubtract() {
    constexpr Sec a(3.0f);
    constexpr Sec b(-4.0f);
    constexpr Sec c(-1.0f);

    constexpr Sec d = a + b;
    constexpr Sec e = c - a;
    CORRADE_COMPARE(d, c);
    CORRADE_COMPARE(e, b);
}

void UnitTest::multiplyDivide() {
    constexpr Sec a(3.0f);
    constexpr Sec b(-4.5f);
    constexpr Sec c(5.0f);

    constexpr Sec d = a*-1.5f;
    constexpr Sec e = -1.5f*a;
    constexpr Sec f = b/-1.5f;
    CORRADE_COMPARE(d, b);
    CORRADE_COMPARE(e, b);
    CORRADE_COMPARE(f, a);

    constexpr Float g = b/a;
    CORRADE_COMPARE(g, -1.5f);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::UnitTest)
