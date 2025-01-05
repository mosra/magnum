/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <new>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Constants.h"
#include "Magnum/Math/Unit.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct UnitTest: TestSuite::Tester {
    explicit UnitTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructConversion();
    void constructCopy();

    void compare();
    void compareNaN();

    void promotedNegated();
    void addSubtract();
    void multiplyDivide();
    void multiplyDivideIntegral();
    void modulo();
};

UnitTest::UnitTest() {
    addTests({&UnitTest::construct,
              &UnitTest::constructDefault,
              &UnitTest::constructNoInit,
              &UnitTest::constructConversion,
              &UnitTest::constructCopy,

              &UnitTest::compare,
              &UnitTest::compareNaN,

              &UnitTest::promotedNegated,
              &UnitTest::addSubtract,
              &UnitTest::multiplyDivide,
              &UnitTest::multiplyDivideIntegral,
              &UnitTest::modulo});
}

/* What's a typedef and not a using differs from the typedefs in root Magnum
   namespace, or is not present there at all */
template<class> struct Sec_;
typedef Unit<Sec_, Float> Sec;
typedef Unit<Sec_, Int> Seci;
using Magnum::Constants;

inline Debug& operator<<(Debug& debug, Sec value) {
    return debug << Float(value);
}
inline Debug& operator<<(Debug& debug, Seci value) {
    return debug << Int(value);
}

void UnitTest::construct() {
    Sec a{25.0f};
    CORRADE_COMPARE(Float(a), 25.0f);

    constexpr Sec ca(25.0f);
    CORRADE_COMPARE(Float(ca), 25.0f);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Float, Sec>::value);
    CORRADE_VERIFY(!std::is_convertible<Sec, Float>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Sec, Float>::value);
}

void UnitTest::constructDefault() {
    constexpr Sec a;
    constexpr Sec b{ZeroInit};
    CORRADE_COMPARE(a, Sec{0.0f});
    CORRADE_COMPARE(b, Sec{0.0f});

    Sec ca;
    Sec cb{ZeroInit};
    CORRADE_COMPARE(ca, Sec{0.0f});
    CORRADE_COMPARE(cb, Sec{0.0f});

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Sec>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Sec, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Sec>::value);
}

void UnitTest::constructNoInit() {
    Sec a{25.0f};
    new(&a) Sec{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Sec{25.0f});
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<Sec, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Sec>::value);
}

void UnitTest::constructConversion() {
    Seci a{25};
    Sec b{a};
    CORRADE_COMPARE(b, Sec{25.0f});

    constexpr Seci ca{25};
    constexpr Sec cb{ca};
    CORRADE_COMPARE(cb, Sec{25.0f});

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Sec, Seci>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Sec, Seci>::value);
}

void UnitTest::constructCopy() {
    Sec a{25.0f};
    Sec b{a};
    CORRADE_COMPARE(b, a);

    constexpr Sec ca{25.0f};
    constexpr Sec cb{ca};
    CORRADE_COMPARE(cb, ca);

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Sec>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Sec>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Sec>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Sec>::value);
}

void UnitTest::compare() {
    CORRADE_VERIFY(Sec(25.0f + TypeTraits<Float>::epsilon()/2.0f) == Sec(25.0f));
    CORRADE_VERIFY(Sec(25.0f + TypeTraits<Float>::epsilon()*75.0f) != Sec(25.0f));

    CORRADE_VERIFY(!(Sec{3.0f} < Sec{3.0f}));
    CORRADE_VERIFY(Sec{3.0f} <= Sec{3.0f});
    CORRADE_VERIFY(Sec{3.0f} >= Sec{3.0f});
    CORRADE_VERIFY(!(Sec{3.0f} > Sec{3.0f}));

    CORRADE_VERIFY(Sec{2.0f} < Sec{3.0f});
    CORRADE_VERIFY(Sec{2.0f} <= Sec{3.0f});
    CORRADE_VERIFY(Sec{3.0f} >= Sec{2.0f});
    CORRADE_VERIFY(Sec{3.0f} > Sec{2.0f});

    CORRADE_VERIFY(!(Sec{3.0f} < Sec{2.0f}));
    CORRADE_VERIFY(!(Sec{3.0f} <= Sec{2.0f}));
    CORRADE_VERIFY(!(Sec{2.0f} >= Sec{3.0f}));
    CORRADE_VERIFY(!(Sec{2.0f} > Sec{3.0f}));

    constexpr bool cc = Sec{3.0f} < Sec{3.0f};
    constexpr bool cd = Sec{3.0f} <= Sec{3.0f};
    constexpr bool ce = Sec{3.0f} >= Sec{3.0f};
    constexpr bool cf = Sec{3.0f} > Sec{3.0f};
    CORRADE_VERIFY(!cc);
    CORRADE_VERIFY(cd);
    CORRADE_VERIFY(ce);
    CORRADE_VERIFY(!cf);

    constexpr bool ch = Sec{2.0f} < Sec{3.0f};
    constexpr bool ci = Sec{2.0f} <= Sec{3.0f};
    constexpr bool cj = Sec{3.0f} >= Sec{2.0f};
    constexpr bool ck = Sec{3.0f} > Sec{2.0f};
    CORRADE_VERIFY(ch);
    CORRADE_VERIFY(ci);
    CORRADE_VERIFY(cj);
    CORRADE_VERIFY(ck);

    constexpr bool cl = Sec{3.0f} < Sec{2.0f};
    constexpr bool cm = Sec{3.0f} <= Sec{2.0f};
    constexpr bool cn = Sec{2.0f} >= Sec{3.0f};
    constexpr bool co = Sec{2.0f} > Sec{3.0f};
    CORRADE_VERIFY(!cl);
    CORRADE_VERIFY(!cm);
    CORRADE_VERIFY(!cn);
    CORRADE_VERIFY(!co);
}

void UnitTest::compareNaN() {
    CORRADE_VERIFY(Sec{Constants::nan()} != Sec{Constants::nan()});
    CORRADE_VERIFY(!(Sec{Constants::nan()} == Sec{Constants::nan()}));
}

void UnitTest::promotedNegated() {
    Sec a{25.0f};
    CORRADE_COMPARE(+a, Sec{+25.0f});
    CORRADE_COMPARE(-a, Sec{-25.0f});

    constexpr Sec ca{25.0f};
    constexpr Sec cb{+ca};
    constexpr Sec cc{-ca};
    CORRADE_COMPARE(cb, Sec{+25.0f});
    CORRADE_COMPARE(cc, Sec{-25.0f});
}

void UnitTest::addSubtract() {
    Sec a{3.0f};
    Sec b{-4.0f};
    Sec c{-1.0f};
    CORRADE_COMPARE(a + b, c);
    CORRADE_COMPARE(c - a, b);

    Sec f = a;
    f += b;
    Sec g = c;
    g -= a;
    CORRADE_COMPARE(f, c);
    CORRADE_COMPARE(g, b);

    constexpr Sec ca{3.0f};
    constexpr Sec cb{-4.0f};
    constexpr Sec cc{-1.0f};

    constexpr Sec cd = ca + cb;
    constexpr Sec ce = cc - ca;
    CORRADE_COMPARE(cd, cc);
    CORRADE_COMPARE(ce, cb);
}

void UnitTest::multiplyDivide() {
    Sec a{3.0f};
    Sec b{-4.5f};
    CORRADE_COMPARE(a*-1.5f, b);
    CORRADE_COMPARE(-1.5f*a, b);
    CORRADE_COMPARE(b/-1.5f, a);
    CORRADE_COMPARE(b/a, -1.5f);

    Sec g = a;
    g *= -1.5f;
    Sec h = b;
    h /= -1.5f;
    CORRADE_COMPARE(g, b);
    CORRADE_COMPARE(h, a);

    constexpr Sec ca{3.0f};
    constexpr Sec cb{-4.5f};
    constexpr Sec cc = ca*-1.5f;
    constexpr Sec cd = -1.5f*ca;
    constexpr Sec ce = cb/-1.5f;
    CORRADE_COMPARE(cc, cb);
    CORRADE_COMPARE(cd, cb);
    CORRADE_COMPARE(ce, ca);

    constexpr Float cf = cb/ca;
    CORRADE_COMPARE(cf, -1.5f);
}

void UnitTest::multiplyDivideIntegral() {
    CORRADE_COMPARE(Seci{100}*1.25f, Seci{125});
    CORRADE_COMPARE(Seci{100}/0.8f, Seci{125});

    Seci a{100};
    Seci b{125};
    a *= 1.25f;
    b /= 1.25f;
    CORRADE_COMPARE(a, Seci{125});
    CORRADE_COMPARE(b, Seci{100});

    constexpr Seci ca{100};
    constexpr Seci cb = ca*1.25f;
    constexpr Seci cc = ca/0.8f;
    CORRADE_COMPARE(cb, Seci{125});
    CORRADE_COMPARE(cc, Seci{125});
}

void UnitTest::modulo() {
    CORRADE_COMPARE(Seci{255}%Seci{64}, Seci{63});
    CORRADE_COMPARE(Seci{-6}%Seci{-4}, Seci{-2});

    Seci a{255};
    Seci b{-6};
    a %= Seci{64};
    b %= Seci{-4};
    CORRADE_COMPARE(a, Seci{63});
    CORRADE_COMPARE(b, Seci{-2});

    constexpr Seci ca{255};
    constexpr Seci cb{-6};
    constexpr Seci cc = ca % Seci{64};
    constexpr Seci cd = cb % Seci{-4};
    CORRADE_COMPARE(cc, Seci{63});
    CORRADE_COMPARE(cd, Seci{-2});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::UnitTest)
