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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Dual.h"

namespace Magnum { namespace Math { namespace Test {

class DualTest: public Corrade::TestSuite::Tester {
    public:
        explicit DualTest();

        void construct();
        void constructDefault();
        void constructCopy();

        void compare();

        void addSubtract();
        void negated();
        void multiplyDivide();

        void conjugated();
        void sqrt();

        void debug();
};

typedef Math::Dual<Float> Dual;

DualTest::DualTest() {
    addTests({&DualTest::construct,
              &DualTest::constructDefault,
              &DualTest::constructCopy,

              &DualTest::compare,

              &DualTest::addSubtract,
              &DualTest::negated,
              &DualTest::multiplyDivide,

              &DualTest::conjugated,
              &DualTest::sqrt,

              &DualTest::debug});
}

void DualTest::construct() {
    constexpr Dual a(2.0f, -7.5f);
    constexpr Float b = a.real();
    constexpr Float c = a.dual();
    CORRADE_COMPARE(b, 2.0f);
    CORRADE_COMPARE(c, -7.5f);

    constexpr Dual d(3.0f);
    CORRADE_COMPARE(d.real(), 3.0f);
    CORRADE_COMPARE(d.dual(), 0.0f);
}

void DualTest::constructDefault() {
    constexpr Dual a;
    CORRADE_COMPARE(a, Dual(0.0f, 0.0f));
}

void DualTest::constructCopy() {
    constexpr Dual a(2.0f, 3.0f);
    constexpr Dual b(a);
    CORRADE_COMPARE(b, Dual(2.0f, 3.0f));
}

void DualTest::compare() {
    CORRADE_VERIFY(Dual(1.0f, 1.0f+TypeTraits<Float>::epsilon()/2) == Dual(1.0f, 1.0f));
    CORRADE_VERIFY(Dual(1.0f, 1.0f+TypeTraits<Float>::epsilon()*2) != Dual(1.0f, 1.0f));
    CORRADE_VERIFY(Dual(1.0f+TypeTraits<Float>::epsilon()/2, 1.0f) == Dual(1.0f, 1.0f));
    CORRADE_VERIFY(Dual(1.0f+TypeTraits<Float>::epsilon()*2, 1.0f) != Dual(1.0f, 1.0f));

    /* Compare to real part only */
    CORRADE_VERIFY(Dual(1.0f, 0.0f) == 1.0f);
    CORRADE_VERIFY(Dual(1.0f, 3.0f) != 1.0f);
}

void DualTest::addSubtract() {
    Dual a(2.0f, -7.5f);
    Dual b(-3.3f, 0.2f);
    Dual c(-1.3f, -7.3f);

    CORRADE_COMPARE(a + b, c);
    CORRADE_COMPARE(c - b, a);
}

void DualTest::negated() {
    CORRADE_COMPARE(-Dual(1.0f, -6.5f), Dual(-1.0f, 6.5f));
}

void DualTest::multiplyDivide() {
    Dual a(1.5f, -4.0f);
    Dual b(-2.0f, 0.5f);
    Dual c(-3.0f, 8.75f);

    CORRADE_COMPARE(a*b, c);
    CORRADE_COMPARE(c/b, a);
}

void DualTest::conjugated() {
    CORRADE_COMPARE(Dual(1.0f, -6.5f).conjugated(), Dual(1.0f, 6.5f));
}

void DualTest::sqrt() {
    CORRADE_COMPARE(Math::sqrt(Dual(16.0f, 2.0f)), Dual(4.0f, 0.25f));
}

void DualTest::debug() {
    std::ostringstream o;

    Debug(&o) << Dual(2.5f, -0.3f);
    CORRADE_COMPARE(o.str(), "Dual(2.5, -0.3)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DualTest)
