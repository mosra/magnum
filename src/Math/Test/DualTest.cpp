/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <sstream>
#include <TestSuite/Tester.h>

#include "Math/Dual.h"

namespace Magnum { namespace Math { namespace Test {

class DualTest: public Corrade::TestSuite::Tester {
    public:
        explicit DualTest();

        void construct();
        void constructDefault();
        void compare();

        void addSubtract();
        void negated();
        void multiplyDivide();

        void conjugated();

        void debug();
};

typedef Math::Dual<float> Dual;

DualTest::DualTest() {
    addTests(&DualTest::construct,
             &DualTest::constructDefault,
             &DualTest::compare,

             &DualTest::addSubtract,
             &DualTest::negated,
             &DualTest::multiplyDivide,

             &DualTest::conjugated,

             &DualTest::debug);
}

void DualTest::construct() {
    Dual a(2.0f, -7.5f);
    CORRADE_COMPARE(a.real(), 2.0f);
    CORRADE_COMPARE(a.dual(), -7.5f);

    Dual b(3.0f);
    CORRADE_COMPARE(b.real(), 3.0f);
    CORRADE_COMPARE(b.dual(), 0.0f);
}

void DualTest::constructDefault() {
    CORRADE_COMPARE(Dual(), Dual(0.0f, 0.0f));
}

void DualTest::compare() {
    CORRADE_VERIFY(Dual(1.0f, 1.0f+MathTypeTraits<float>::epsilon()/2) == Dual(1.0f, 1.0f));
    CORRADE_VERIFY(Dual(1.0f, 1.0f+MathTypeTraits<float>::epsilon()*2) != Dual(1.0f, 1.0f));
    CORRADE_VERIFY(Dual(1.0f+MathTypeTraits<float>::epsilon()/2, 1.0f) == Dual(1.0f, 1.0f));
    CORRADE_VERIFY(Dual(1.0f+MathTypeTraits<float>::epsilon()*2, 1.0f) != Dual(1.0f, 1.0f));

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

void DualTest::debug() {
    std::ostringstream o;

    Debug(&o) << Dual(2.5f, -0.3f);
    CORRADE_COMPARE(o.str(), "Dual(2.5, -0.3)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DualTest)
