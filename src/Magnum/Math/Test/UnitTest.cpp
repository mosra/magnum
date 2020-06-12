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

#include "Magnum/Math/Constants.h"
#include "Magnum/Math/Unit.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct UnitTest: Corrade::TestSuite::Tester {
    explicit UnitTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructConversion();
    void compare();
    void compareNaN();

    void negated();
    void addSubtract();
    void multiplyDivide();
};

UnitTest::UnitTest() {
    addTests({&UnitTest::construct,
              &UnitTest::constructDefault,
              &UnitTest::constructNoInit,
              &UnitTest::constructConversion,
              &UnitTest::compare,
              &UnitTest::compareNaN,

              &UnitTest::negated,
              &UnitTest::addSubtract,
              &UnitTest::multiplyDivide});
}

template<class> struct Sec_;
typedef Unit<Sec_, Float> Sec;
typedef Unit<Sec_, Int> Seci;
typedef Constants<Float> Constants;

inline Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, Sec value) {
    return debug << Float(value);
}

void UnitTest::construct() {
    constexpr Sec a(25.0f);
    CORRADE_COMPARE(Float(a), 25.0f);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Sec>::value));
    CORRADE_VERIFY(!(std::is_convertible<Sec, Float>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Sec, Float>::value));
}

void UnitTest::constructDefault() {
    constexpr Sec a;
    constexpr Sec b{ZeroInit};
    CORRADE_COMPARE(a, Sec(0.0f));
    CORRADE_COMPARE(b, Sec(0.0f));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Sec>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Sec, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Sec>::value));
}

void UnitTest::constructNoInit() {
    Sec a{25.0f};
    new(&a) Sec{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Sec{25.0f});
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Sec, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Sec>::value));
}

void UnitTest::constructConversion() {
    constexpr Seci a(25);
    constexpr Sec b(a);
    CORRADE_COMPARE(b, Sec(25.0f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Sec, Seci>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Sec, Seci>::value));
}

void UnitTest::compare() {
    CORRADE_VERIFY(Sec(25.0f + TypeTraits<Float>::epsilon()/2.0f) == Sec(25.0f));
    CORRADE_VERIFY(Sec(25.0f + TypeTraits<Float>::epsilon()*75.0f) != Sec(25.0f));

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

void UnitTest::compareNaN() {
    CORRADE_VERIFY(Sec{Constants::nan()} != Sec{Constants::nan()});
    CORRADE_VERIFY(!(Sec{Constants::nan()} == Sec{Constants::nan()}));
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

    Sec f = a;
    f += b;
    Sec g = c;
    g -= a;
    CORRADE_COMPARE(f, c);
    CORRADE_COMPARE(g, b);
}

void UnitTest::multiplyDivide() {
    constexpr Sec a(3.0f);
    constexpr Sec b(-4.5f);

    constexpr Sec c = a*-1.5f;
    constexpr Sec d = -1.5f*a;
    constexpr Sec e = b/-1.5f;
    CORRADE_COMPARE(c, b);
    CORRADE_COMPARE(d, b);
    CORRADE_COMPARE(e, a);

    constexpr Float f = b/a;
    CORRADE_COMPARE(f, -1.5f);

    Sec g = a;
    g *= -1.5f;
    Sec h = b;
    h /= -1.5f;
    CORRADE_COMPARE(g, b);
    CORRADE_COMPARE(h, a);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::UnitTest)
