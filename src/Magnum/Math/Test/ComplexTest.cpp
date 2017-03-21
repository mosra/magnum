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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Matrix3.h"

struct Cmpl {
    float re, im;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct ComplexConverter<Float, Cmpl> {
    constexpr static Complex<Float> from(const Cmpl& other) {
        return {other.re, other.im};
    }

    constexpr static Cmpl to(const Complex<Float>& other) {
        return {other.real(), other.imaginary()};
    }
};

}

namespace Test {

struct ComplexTest: Corrade::TestSuite::Tester {
    explicit ComplexTest();

    void construct();
    void constructIdentity();
    void constructZero();
    void constructNoInit();
    void constructFromVector();
    void constructConversion();
    void constructCopy();
    void convert();

    void compare();
    void isNormalized();
    template<class T> void isNormalizedEpsilon();

    void addSubtract();
    void negated();
    void multiplyDivideScalar();
    void multiply();

    void dot();
    void dotSelf();
    void length();
    void normalized();
    template<class T> void normalizedIterative();

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
              &ComplexTest::constructIdentity,
              &ComplexTest::constructZero,
              &ComplexTest::constructNoInit,
              &ComplexTest::constructFromVector,
              &ComplexTest::constructConversion,
              &ComplexTest::constructCopy,
              &ComplexTest::convert,

              &ComplexTest::compare,
              &ComplexTest::isNormalized,
              &ComplexTest::isNormalizedEpsilon<Float>,
              &ComplexTest::isNormalizedEpsilon<Double>,

              &ComplexTest::addSubtract,
              &ComplexTest::negated,
              &ComplexTest::multiplyDivideScalar,
              &ComplexTest::multiply,

              &ComplexTest::dot,
              &ComplexTest::dotSelf,
              &ComplexTest::length,
              &ComplexTest::normalized});

    addRepeatedTests<ComplexTest>({
        &ComplexTest::normalizedIterative<Float>,
        &ComplexTest::normalizedIterative<Double>}, 1000);

    addTests({&ComplexTest::conjugated,
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
typedef Math::Matrix2x2<Float> Matrix2x2;

void ComplexTest::construct() {
    constexpr Complex a = {0.5f, -3.7f};
    CORRADE_COMPARE(a, Complex(0.5f, -3.7f));

    constexpr Float b = a.real();
    constexpr Float c = a.imaginary();
    CORRADE_COMPARE(b, 0.5f);
    CORRADE_COMPARE(c, -3.7f);

    CORRADE_VERIFY((std::is_nothrow_constructible<Complex, Float, Float>::value));
}

void ComplexTest::constructIdentity() {
    constexpr Complex a;
    constexpr Complex b{IdentityInit};
    CORRADE_COMPARE(a, Complex(1.0f, 0.0f));
    CORRADE_COMPARE(b, Complex(1.0f, 0.0f));
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(b.length(), 1.0f);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Complex>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Complex, IdentityInitT>::value));
}

void ComplexTest::constructZero() {
    constexpr Complex a{ZeroInit};
    CORRADE_COMPARE(a, Complex(0.0f, 0.0f));

    CORRADE_VERIFY((std::is_nothrow_constructible<Complex, ZeroInitT>::value));
}

void ComplexTest::constructNoInit() {
    Complex a{0.5f, -3.7f};
    new(&a) Complex{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Complex(0.5f, -3.7f));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Complex, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, Complex>::value));
}

void ComplexTest::constructFromVector() {
    constexpr Vector2 vec(1.5f, -3.0f);

    constexpr Complex a(vec);
    CORRADE_COMPARE(a, Complex(1.5f, -3.0f));

    constexpr Vector2 b(a);
    CORRADE_COMPARE(b, vec);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector2, Complex>::value));
    CORRADE_VERIFY(!(std::is_convertible<Complex, Vector2>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Complex, Vector2>::value));
}

void ComplexTest::constructConversion() {
    typedef Math::Complex<Int> Complexi;

    constexpr Complex a{1.3f, 2.7f};
    constexpr Complexi b{a};

    CORRADE_COMPARE(b, (Complexi{1, 2}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Complex, Complexi>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Complex, Complexi>::value));
}

void ComplexTest::constructCopy() {
    constexpr Complex a(2.5f, -5.0f);
    constexpr Complex b(a);
    CORRADE_COMPARE(b, Complex(2.5f, -5.0f));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Complex>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Complex>::value);
}

void ComplexTest::convert() {
    constexpr Cmpl a{1.5f, -3.5f};
    constexpr Complex b{1.5f, -3.5f};

    /* GCC 5.1 had a bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66450
       Hopefully this does not reappear. */
    constexpr Complex c(a);
    CORRADE_COMPARE(c, b);

    constexpr Cmpl d(b);
    CORRADE_COMPARE(d.re, a.re);
    CORRADE_COMPARE(d.im, a.im);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Cmpl, Complex>::value));
    CORRADE_VERIFY(!(std::is_convertible<Complex, Cmpl>::value));
}

void ComplexTest::compare() {
    CORRADE_VERIFY(Complex(3.7f, -1.0f+TypeTraits<Float>::epsilon()/2) == Complex(3.7f, -1.0f));
    CORRADE_VERIFY(Complex(3.7f, -1.0f+TypeTraits<Float>::epsilon()*2) != Complex(3.7f, -1.0f));
    CORRADE_VERIFY(Complex(1.0f+TypeTraits<Float>::epsilon()/2, 3.7f) == Complex(1.0f, 3.7f));
    CORRADE_VERIFY(Complex(1.0f+TypeTraits<Float>::epsilon()*2, 3.7f) != Complex(1.0f, 3.7f));
}

void ComplexTest::isNormalized() {
    CORRADE_VERIFY(!Complex(2.5f, -3.7f).isNormalized());
    CORRADE_VERIFY(Complex::rotation(Deg(23.0f)).isNormalized());
}

template<class T> void ComplexTest::isNormalizedEpsilon() {
    setTestCaseName(std::string{"isNormalizedEpsilon<"} + TypeTraits<T>::name() + ">");

    CORRADE_VERIFY((Math::Complex<T>{T(0.801775644243754) + TypeTraits<T>::epsilon()/T(2.0), T(0.597625146975521)}.isNormalized()));
    CORRADE_VERIFY(!(Math::Complex<T>{T(0.801775644243754) + TypeTraits<T>::epsilon()*T(2.0), T(0.597625146975521)}.isNormalized()));
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

    CORRADE_COMPARE(Math::dot(a, b), 9.0f);
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

template<class T> void ComplexTest::normalizedIterative() {
    setTestCaseName(std::string{"normalizedIterative<"} + TypeTraits<T>::name() + ">");

    auto a = Math::Complex<T>::rotation(Math::Deg<T>{T(36.7)});
    for(std::size_t i = 0; i != testCaseRepeatId(); ++i) {
        a = Math::Complex<T>::rotation(Math::Deg<T>{T(87.1)})*a;
        a = a.normalized();
    }

    CORRADE_VERIFY(a.isNormalized());
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
    Error redirectError{&o};

    Complex a(-0.6f, 0.8f);
    Complex b(-0.6f, -0.8f);

    (a*2).invertedNormalized();
    CORRADE_COMPARE(o.str(), "Math::Complex::invertedNormalized(): complex number must be normalized\n");

    Complex inverted = a.invertedNormalized();
    CORRADE_COMPARE(a*inverted, Complex());
    CORRADE_COMPARE(inverted*a, Complex());
    CORRADE_COMPARE(inverted, b);
}

void ComplexTest::angle() {
    std::ostringstream o;
    Error redirectError{&o};
    Math::angle(Complex(1.5f, -2.0f).normalized(), {-4.0f, 3.5f});
    CORRADE_COMPARE(o.str(), "Math::angle(): complex numbers must be normalized\n");

    o.str({});
    Math::angle({1.5f, -2.0f}, Complex(-4.0f, 3.5f).normalized());
    CORRADE_COMPARE(o.str(), "Math::angle(): complex numbers must be normalized\n");

    /* Verify also that the angle is the same as angle between 2D vectors */
    Rad angle = Math::angle(Complex( 1.5f, -2.0f).normalized(),
                            Complex(-4.0f,  3.5f).normalized());
    CORRADE_COMPARE(angle, Math::angle(Vector2( 1.5f, -2.0f).normalized(),
                                       Vector2(-4.0f,  3.5f).normalized()));
    CORRADE_COMPARE(angle, Rad(2.933128f));
}

void ComplexTest::rotation() {
    Complex a = Complex::rotation(Deg(120.0f));
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(a, Complex(-0.5f, 0.8660254f));
    CORRADE_COMPARE_AS(a.angle(), Deg(120.0f), Rad);

    /* Verify negative angle */
    Complex b = Complex::rotation(Deg(-240.0f));
    CORRADE_COMPARE(b, Complex(-0.5f, 0.8660254f));
    CORRADE_COMPARE_AS(b.angle(), Deg(120.0f), Rad);

    /* Default-constructed complex number has zero angle */
    CORRADE_COMPARE_AS(Complex().angle(), Deg(0.0f), Rad);
}

void ComplexTest::matrix() {
    Complex a = Complex::rotation(Deg(37.0f));
    Matrix2x2 m = Matrix3::rotation(Deg(37.0f)).rotationScaling();

    CORRADE_COMPARE(a.toMatrix(), m);

    std::ostringstream o;
    Error redirectError{&o};
    Complex::fromMatrix(m*2);
    CORRADE_COMPARE(o.str(), "Math::Complex::fromMatrix(): the matrix is not orthogonal\n");

    Complex b = Complex::fromMatrix(m);
    CORRADE_COMPARE(b, a);
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
