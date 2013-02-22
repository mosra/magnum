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

#include "Math/Complex.h"

namespace Magnum { namespace Math { namespace Test {

class ComplexTest: public Corrade::TestSuite::Tester {
    public:
        explicit ComplexTest();

        void construct();
        void constructDefault();
        void compare();

        void constExpressions();

        void addSubtract();
        void negated();
        void multiplyDivideScalar();
        void multiply();

        void dot();
        void dotSelf();
        void length();
        void normalized();

        void conjugated();
        void inverted();
        void invertedNormalized();

        void debug();
};

ComplexTest::ComplexTest() {
    addTests(&ComplexTest::construct,
             &ComplexTest::constructDefault,
             &ComplexTest::compare,

             &ComplexTest::constExpressions,

             &ComplexTest::addSubtract,
             &ComplexTest::negated,
             &ComplexTest::multiplyDivideScalar,
             &ComplexTest::multiply,

             &ComplexTest::dot,
             &ComplexTest::dotSelf,
             &ComplexTest::length,
             &ComplexTest::normalized,

             &ComplexTest::conjugated,
             &ComplexTest::inverted,
             &ComplexTest::invertedNormalized,

             &ComplexTest::debug);
}

typedef Math::Complex<float> Complex;

void ComplexTest::construct() {
    Complex c(0.5f, -3.7f);
    CORRADE_COMPARE(c.real(), 0.5f);
    CORRADE_COMPARE(c.imaginary(), -3.7f);

    CORRADE_COMPARE(Complex(2.0f), Complex(2.0f, 0.0f));
}

void ComplexTest::constructDefault() {
    CORRADE_COMPARE(Complex(), Complex(1.0f, 0.0f));
    CORRADE_COMPARE(Complex().length(), 1.0f);
}

void ComplexTest::compare() {
    CORRADE_VERIFY(Complex(3.7f, -1.0f+MathTypeTraits<float>::epsilon()/2) == Complex(3.7f, -1.0f));
    CORRADE_VERIFY(Complex(3.7f, -1.0f+MathTypeTraits<float>::epsilon()*2) != Complex(3.7f, -1.0f));
    CORRADE_VERIFY(Complex(1.0f+MathTypeTraits<float>::epsilon()/2, 3.7f) == Complex(1.0f, 3.7f));
    CORRADE_VERIFY(Complex(1.0f+MathTypeTraits<float>::epsilon()*2, 3.7f) != Complex(1.0f, 3.7f));
}

void ComplexTest::constExpressions() {
    /* Default constructor */
    constexpr Complex a;
    CORRADE_COMPARE(a, Complex(1.0f, 0.0f));

    /* Value constructor */
    constexpr Complex b(2.5f, -5.0f);
    CORRADE_COMPARE(b, Complex(2.5f, -5.0f));

    /* Copy constructor */
    constexpr Complex c(b);
    CORRADE_COMPARE(c, Complex(2.5f, -5.0f));

    /* Data access */
    constexpr float d = b.real();
    constexpr float e = c.imaginary();
    CORRADE_COMPARE(d, 2.5f);
    CORRADE_COMPARE(e, -5.0f);
}

void ComplexTest::addSubtract() {
    Complex a( 1.7f, -3.7f);
    Complex b(-3.6f,  0.2f);
    Complex c(-1.9f, -3.5f);

    CORRADE_COMPARE(a + b, c);
    CORRADE_COMPARE(c - b, a);
}

void ComplexTest::negated() {
    CORRADE_COMPARE(-Complex(2.5f, -7.4f), Complex(-2.5f, 7.4f));
}

void ComplexTest::multiplyDivideScalar() {
    Complex a( 2.5f, -0.5f);
    Complex b(-7.5f,  1.5f);

    CORRADE_COMPARE(a*-3.0f, b);
    CORRADE_COMPARE(-3.0f*a, b);
    CORRADE_COMPARE(b/-3.0f, a);

    Complex c(-0.8f, 4.0f);
    CORRADE_COMPARE(-2.0f/a, c);
}

void ComplexTest::multiply() {
    Complex a( 5.0f,   3.0f);
    Complex b( 6.0f,  -7.0f);
    Complex c(51.0f, -17.0f);

    CORRADE_COMPARE(a*b, c);
    CORRADE_COMPARE(b*a, c);
}

void ComplexTest::dot() {
    Complex a(5.0f,  3.0f);
    Complex b(6.0f, -7.0f);

    CORRADE_COMPARE(Complex::dot(a, b), Complex(9.0f, 53.0f));
}

void ComplexTest::dotSelf() {
    CORRADE_COMPARE(Complex(-4.0f, 3.0f).dot(), 25.0f);
}

void ComplexTest::length() {
    CORRADE_COMPARE(Complex(-4.0f, 3.0f).length(), 5.0f);
}

void ComplexTest::normalized() {
    Complex a(-3.0f, 4.0f);
    Complex b(-0.6f, 0.8f);

    CORRADE_COMPARE(a.normalized(), b);
    CORRADE_COMPARE(a.normalized().length(), 1.0f);
}

void ComplexTest::conjugated() {
    CORRADE_COMPARE(Complex(-3.0f, 4.5f).conjugated(), Complex(-3.0f, -4.5f));
}

void ComplexTest::inverted() {
    Complex a(-3.0f, 4.0f);
    Complex b(-0.12f, -0.16f);

    Complex inverted = a.inverted();
    CORRADE_COMPARE(a*inverted, Complex());
    CORRADE_COMPARE(inverted*a, Complex());
    CORRADE_COMPARE(inverted, b);
}

void ComplexTest::invertedNormalized() {
    std::ostringstream o;
    Error::setOutput(&o);

    Complex a(-0.6f, 0.8f);
    Complex b(-0.6f, -0.8f);

    Complex notInverted = (a*2).invertedNormalized();
    CORRADE_VERIFY(notInverted != notInverted);
    CORRADE_COMPARE(o.str(), "Math::Complex::invertedNormalized(): complex number must be normalized\n");

    Complex inverted = a.invertedNormalized();
    CORRADE_COMPARE(a*inverted, Complex());
    CORRADE_COMPARE(inverted*a, Complex());
    CORRADE_COMPARE(inverted, b);
}

void ComplexTest::debug() {
    std::ostringstream o;

    Debug(&o) << Complex(2.5f, -7.5f);
    CORRADE_COMPARE(o.str(), "Complex(2.5, -7.5)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::ComplexTest)
