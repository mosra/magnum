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
#include <TestSuite/Tester.h>

#include "Math/Complex.h"
#include "Math/Matrix3.h"

namespace Magnum { namespace Math { namespace Test {

class ComplexTest: public Corrade::TestSuite::Tester {
    public:
        explicit ComplexTest();

        void construct();
        void constructDefault();
        void constructFromVector();
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

        void angle();
        void rotation();
        void matrix();
        void transformVector();

        void debug();
};

ComplexTest::ComplexTest() {
    addTests({&ComplexTest::construct,
              &ComplexTest::constructDefault,
              &ComplexTest::constructFromVector,
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

              &ComplexTest::angle,
              &ComplexTest::rotation,
              &ComplexTest::matrix,
              &ComplexTest::transformVector,

              &ComplexTest::debug});
}

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Complex<Float> Complex;
typedef Math::Vector2<Float> Vector2;
typedef Math::Matrix3<Float> Matrix3;
typedef Math::Matrix<2, Float> Matrix2;

void ComplexTest::construct() {
    Complex c(0.5f, -3.7f);
    CORRADE_COMPARE(c.real(), 0.5f);
    CORRADE_COMPARE(c.imaginary(), -3.7f);
}

void ComplexTest::constructDefault() {
    CORRADE_COMPARE(Complex(), Complex(1.0f, 0.0f));
    CORRADE_COMPARE(Complex().length(), 1.0f);
}

void ComplexTest::constructFromVector() {
    Vector2 vec(1.5f, -3.0f);

    Complex a(vec);
    CORRADE_COMPARE(a, Complex(1.5f, -3.0f));
    CORRADE_COMPARE(Vector2(a), vec);
}

void ComplexTest::compare() {
    CORRADE_VERIFY(Complex(3.7f, -1.0f+MathTypeTraits<Float>::epsilon()/2) == Complex(3.7f, -1.0f));
    CORRADE_VERIFY(Complex(3.7f, -1.0f+MathTypeTraits<Float>::epsilon()*2) != Complex(3.7f, -1.0f));
    CORRADE_VERIFY(Complex(1.0f+MathTypeTraits<Float>::epsilon()/2, 3.7f) == Complex(1.0f, 3.7f));
    CORRADE_VERIFY(Complex(1.0f+MathTypeTraits<Float>::epsilon()*2, 3.7f) != Complex(1.0f, 3.7f));
}

void ComplexTest::constExpressions() {
    /* Default constructor */
    constexpr Complex a;
    CORRADE_COMPARE(a, Complex(1.0f, 0.0f));

    /* Value constructor */
    constexpr Complex b(2.5f, -5.0f);
    CORRADE_COMPARE(b, Complex(2.5f, -5.0f));

    /* Vector constructor */
    constexpr Complex c(Vector2(-1.0f, 2.2f));
    CORRADE_COMPARE(c, Complex(-1.0f, 2.2f));

    /* Copy constructor */
    constexpr Complex d(b);
    CORRADE_COMPARE(d, Complex(2.5f, -5.0f));

    /* Data access */
    constexpr Float e = b.real();
    constexpr Float f = b.imaginary();
    constexpr Vector2 g(b);
    CORRADE_COMPARE(e, 2.5f);
    CORRADE_COMPARE(f, -5.0f);
    CORRADE_COMPARE(g, Vector2(2.5f, -5.0f));
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

    CORRADE_COMPARE(Complex::dot(a, b), 9.0f);
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

void ComplexTest::angle() {
    std::ostringstream o;
    Error::setOutput(&o);
    auto angle = Complex::angle(Complex(1.5f, -2.0f).normalized(), {-4.0f, 3.5f});
    CORRADE_VERIFY(angle != angle);
    CORRADE_COMPARE(o.str(), "Math::Complex::angle(): complex numbers must be normalized\n");

    o.str({});
    angle = Complex::angle({1.5f, -2.0f}, Complex(-4.0f, 3.5f).normalized());
    CORRADE_VERIFY(angle != angle);
    CORRADE_COMPARE(o.str(), "Math::Complex::angle(): complex numbers must be normalized\n");

    /* Verify also that the angle is the same as angle between 2D vectors */
    angle = Complex::angle(Complex( 1.5f, -2.0f).normalized(),
                           Complex(-4.0f,  3.5f).normalized());
    CORRADE_COMPARE(angle, Vector2::angle(Vector2( 1.5f, -2.0f).normalized(),
                                          Vector2(-4.0f,  3.5f).normalized()));
    CORRADE_COMPARE(angle, Rad(2.933128f));
}

void ComplexTest::rotation() {
    Complex a = Complex::rotation(Deg(120.0f));
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(a, Complex(-0.5f, 0.8660254f));
    CORRADE_COMPARE_AS(a.rotationAngle(), Deg(120.0f), Rad);

    /* Verify negative angle */
    Complex b = Complex::rotation(Deg(-240.0f));
    CORRADE_COMPARE(b, Complex(-0.5f, 0.8660254f));
    CORRADE_COMPARE_AS(b.rotationAngle(), Deg(120.0f), Rad);

    /* Default-constructed complex number has zero angle */
    CORRADE_COMPARE_AS(Complex().rotationAngle(), Deg(0.0f), Rad);
}

void ComplexTest::matrix() {
    Complex a = Complex::rotation(Deg(37.0f));
    Matrix2 m = Matrix3::rotation(Deg(37.0f)).rotationScaling();

    CORRADE_COMPARE(a.toMatrix(), m);
}

void ComplexTest::transformVector() {
    Complex a = Complex::rotation(Deg(23.0f));
    Matrix3 m = Matrix3::rotation(Deg(23.0f));
    Vector2 v(-3.6f, 0.7f);

    Vector2 rotated = a.transformVector(v);
    CORRADE_COMPARE(rotated, m.transformVector(v));
    CORRADE_COMPARE(rotated, Vector2(-3.58733f, -0.762279f));
}

void ComplexTest::debug() {
    std::ostringstream o;

    Debug(&o) << Complex(2.5f, -7.5f);
    CORRADE_COMPARE(o.str(), "Complex(2.5, -7.5)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::ComplexTest)
