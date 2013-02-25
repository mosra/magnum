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

        void lengthSquared();
        void length();
        void normalized();

        void complexConjugated();
        void dualConjugated();
        void conjugated();
        void inverted();
        void invertedNormalized();

        void rotation();
        void translation();

        void debug();
};

typedef Math::Deg<float> Deg;
typedef Math::Rad<float> Rad;
typedef Math::Complex<float> Complex;
typedef Math::Dual<float> Dual;
typedef Math::DualComplex<float> DualComplex;
typedef Math::Vector2<float> Vector2;

DualComplexTest::DualComplexTest() {
    addTests(&DualComplexTest::construct,
             &DualComplexTest::constructDefault,

             &DualComplexTest::constExpressions,

             &DualComplexTest::lengthSquared,
             &DualComplexTest::length,
             &DualComplexTest::normalized,

             &DualComplexTest::complexConjugated,
             &DualComplexTest::dualConjugated,
             &DualComplexTest::conjugated,
             &DualComplexTest::inverted,
             &DualComplexTest::invertedNormalized,

             &DualComplexTest::rotation,
             &DualComplexTest::translation,

             &DualComplexTest::debug);
}

void DualComplexTest::construct() {
    DualComplex a({-1.0f, 2.5f}, {3.0f, -7.5f});
    CORRADE_COMPARE(a.real(), Complex(-1.0f, 2.5f));
    CORRADE_COMPARE(a.dual(), Complex(3.0f, -7.5f));
}

void DualComplexTest::constructDefault() {
    CORRADE_COMPARE(DualComplex(), DualComplex({1.0f, 0.0f}, {0.0f, 0.0f}));
    CORRADE_COMPARE(DualComplex().length(), 1.0f);
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

void DualComplexTest::lengthSquared() {
    DualComplex a({-1.0f, 3.0f}, {0.5f, -2.0f});
    CORRADE_COMPARE(a.lengthSquared(), Dual(10.0f, -13.0f));
}

void DualComplexTest::length() {
    DualComplex a({-1.0f, 3.0f}, {0.5f, -2.0f});
    CORRADE_COMPARE(a.length(), Dual(3.162278f, -2.05548f));
}

void DualComplexTest::normalized() {
    DualComplex a({-1.0f, 3.0f}, {0.5f, -2.0f});
    DualComplex b({-0.316228f, 0.948683f}, {-0.0474342f, -0.0158114f});
    CORRADE_COMPARE(a.normalized().length(), 1.0f);
    CORRADE_COMPARE(a.normalized(), b);
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

void DualComplexTest::inverted() {
    DualComplex a({-1.0f,  2.5f}, { 3.0f, -7.5f});
    DualComplex b({-1.0f, -2.5f}, { 3.0f,  7.5f});

    CORRADE_COMPARE(a*a.inverted(), DualComplex());
    CORRADE_COMPARE(a.inverted(), b/Dual(7.25f, -43.5f));
}

void DualComplexTest::invertedNormalized() {
    DualComplex a({-1.0f,  2.5f}, { 3.0f, -7.5f});
    DualComplex b({-1.0f, -2.5f}, { 3.0f,  7.5f});

    std::ostringstream o;
    Error::setOutput(&o);
    CORRADE_COMPARE(a.invertedNormalized(), DualComplex());
    CORRADE_COMPARE(o.str(), "Math::DualComplex::invertedNormalized(): dual complex number must be normalized\n");

    DualComplex normalized = a.normalized();
    DualComplex inverted = normalized.invertedNormalized();
    CORRADE_COMPARE(normalized*inverted, DualComplex());
    CORRADE_COMPARE(inverted*normalized, DualComplex());
    CORRADE_COMPARE(inverted, b/Math::sqrt(Dual(7.25f, -43.5f)));
}

void DualComplexTest::rotation() {
    DualComplex a = DualComplex::rotation(Deg(120.0f));
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(a, DualComplex({-0.5f, 0.8660254f}, {0.0f, 0.0f}));
    CORRADE_COMPARE_AS(a.rotationAngle(), Deg(120.0f), Rad);
}

void DualComplexTest::translation() {
    Vector2 vec(1.5f, -3.5f);
    DualComplex a = DualComplex::translation(vec);
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(a, DualComplex({}, {1.5f, -3.5f}));
    CORRADE_COMPARE(a.translation(), vec);
}

void DualComplexTest::debug() {
    std::ostringstream o;

    Debug(&o) << DualComplex({-1.0f, -2.5f}, {-3.0f, -7.5f});
    CORRADE_COMPARE(o.str(), "DualComplex({-1, -2.5}, {-3, -7.5})\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DualComplexTest)
