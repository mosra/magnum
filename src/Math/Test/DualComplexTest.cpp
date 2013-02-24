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

#include "Math/DualComplex.h"

namespace Magnum { namespace Math { namespace Test {

class DualComplexTest: public Corrade::TestSuite::Tester {
    public:
        explicit DualComplexTest();

        void construct();
        void constructDefault();

        void constExpressions();

        void complexConjugated();
        void dualConjugated();
        void conjugated();

        void debug();
};

typedef Math::Complex<float> Complex;
typedef Math::DualComplex<float> DualComplex;

DualComplexTest::DualComplexTest() {
    addTests(&DualComplexTest::construct,
             &DualComplexTest::constructDefault,

             &DualComplexTest::constExpressions,

             &DualComplexTest::complexConjugated,
             &DualComplexTest::dualConjugated,
             &DualComplexTest::conjugated,

             &DualComplexTest::debug);
}

void DualComplexTest::construct() {
    DualComplex a({-1.0f, 2.5f}, {3.0f, -7.5f});
    CORRADE_COMPARE(a.real(), Complex(-1.0f, 2.5f));
    CORRADE_COMPARE(a.dual(), Complex(3.0f, -7.5f));
}

void DualComplexTest::constructDefault() {
    CORRADE_COMPARE(DualComplex(), DualComplex({1.0f, 0.0f}, {0.0f, 0.0f}));
}

void DualComplexTest::constExpressions() {
    /* Default constructor */
    constexpr DualComplex a;
    CORRADE_COMPARE(a, DualComplex({1.0f, 0.0f}, {0.0f, 0.0f}));

    /* Value constructor */
    constexpr DualComplex b({-1.0f, 2.5f}, {3.0f, -7.5f});
    CORRADE_COMPARE(b, DualComplex({-1.0f, 2.5f}, {3.0f, -7.5f}));

    /* Copy constructor */
    constexpr DualComplex d(b);
    CORRADE_COMPARE(d, DualComplex({-1.0f, 2.5f}, {3.0f, -7.5f}));
}

void DualComplexTest::complexConjugated() {
    DualComplex a({-1.0f,  2.5f}, {3.0f, -7.5f});
    DualComplex b({-1.0f, -2.5f}, {3.0f,  7.5f});
    CORRADE_COMPARE(a.complexConjugated(), b);
}

void DualComplexTest::dualConjugated() {
    DualComplex a({-1.0f, 2.5f}, { 3.0f, -7.5f});
    DualComplex b({-1.0f, 2.5f}, {-3.0f,  7.5f});
    CORRADE_COMPARE(a.dualConjugated(), b);
}

void DualComplexTest::conjugated() {
    DualComplex a({-1.0f,  2.5f}, { 3.0f, -7.5f});
    DualComplex b({-1.0f, -2.5f}, {-3.0f, -7.5f});
    CORRADE_COMPARE(a.conjugated(), b);
}

void DualComplexTest::debug() {
    std::ostringstream o;

    Debug(&o) << DualComplex({-1.0f, -2.5f}, {-3.0f, -7.5f});
    CORRADE_COMPARE(o.str(), "DualComplex({-1, -2.5}, {-3, -7.5})\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DualComplexTest)
