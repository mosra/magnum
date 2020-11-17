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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/StrictWeakOrdering.h"

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

namespace Test { namespace {

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

    void data();

    void compare();
    void isNormalized();
    template<class T> void isNormalizedEpsilon();

    void addSubtract();
    void negated();
    void multiplyDivideScalar();
    void multiplyDivideVector();
    void multiply();

    void dot();
    void dotSelf();
    void length();
    void normalized();
    template<class T> void normalizedIterative();

    void conjugated();
    void inverted();
    void invertedNormalized();
    void invertedNormalizedNotNormalized();

    void angle();
    void angleNormalizedButOver1();
    void angleNotNormalized();
    void rotation();
    void matrix();
    void matrixNotOrthogonal();
    void lerp();
    void lerpNotNormalized();
    void slerp();
    void slerpNotNormalized();
    void transformVector();

    void strictWeakOrdering();

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

              &ComplexTest::data,

              &ComplexTest::compare,
              &ComplexTest::isNormalized,
              &ComplexTest::isNormalizedEpsilon<Float>,
              &ComplexTest::isNormalizedEpsilon<Double>,

              &ComplexTest::addSubtract,
              &ComplexTest::negated,
              &ComplexTest::multiplyDivideScalar,
              &ComplexTest::multiplyDivideVector,
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
              &ComplexTest::invertedNormalizedNotNormalized,

              &ComplexTest::angle,
              &ComplexTest::angleNormalizedButOver1,
              &ComplexTest::angleNotNormalized,
              &ComplexTest::rotation,
              &ComplexTest::matrix,
              &ComplexTest::matrixNotOrthogonal,
              &ComplexTest::lerp,
              &ComplexTest::lerpNotNormalized,
              &ComplexTest::slerp,
              &ComplexTest::slerpNotNormalized,
              &ComplexTest::transformVector,

              &ComplexTest::strictWeakOrdering,

              &ComplexTest::debug});
}

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Complex<Float> Complex;
typedef Math::Vector2<Float> Vector2;
typedef Math::Matrix3<Float> Matrix3;
typedef Math::Matrix2x2<Float> Matrix2x2;

using namespace Math::Literals;

void ComplexTest::construct() {
    constexpr Complex a = {0.5f, -3.7f};
    CORRADE_COMPARE(a, Complex(0.5f, -3.7f));
    CORRADE_COMPARE(a.real(), 0.5f);
    CORRADE_COMPARE(a.imaginary(), -3.7f);

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

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<IdentityInitT, Complex>::value));
}

void ComplexTest::constructZero() {
    constexpr Complex a{ZeroInit};
    CORRADE_COMPARE(a, Complex(0.0f, 0.0f));

    CORRADE_VERIFY((std::is_nothrow_constructible<Complex, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Complex>::value));
}

void ComplexTest::constructNoInit() {
    Complex a{0.5f, -3.7f};
    new(&a) Complex{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Complex(0.5f, -3.7f));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Complex, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Complex>::value));
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

void ComplexTest::data() {
    constexpr Complex ca{1.5f, -3.5f};
    constexpr Float real = ca.real();
    constexpr Float imaginary = ca.imaginary();
    CORRADE_COMPARE(real, 1.5f);
    CORRADE_COMPARE(imaginary, -3.5f);

    Complex a{1.5f, -3.5f};
    a.real() = 2.0f;
    a.imaginary() = -3.5f;
    CORRADE_COMPARE(a, (Complex{2.0f, -3.5f}));

    constexpr Float b = *ca.data();
    Float c = a.data()[1];
    CORRADE_COMPARE(b, 1.5f);
    CORRADE_COMPARE(c, -3.5f);
}

void ComplexTest::compare() {
    CORRADE_VERIFY(Complex(3.7f, -1.0f+TypeTraits<Float>::epsilon()/2) == Complex(3.7f, -1.0f));
    CORRADE_VERIFY(Complex(3.7f, -1.0f+TypeTraits<Float>::epsilon()*2) != Complex(3.7f, -1.0f));
    CORRADE_VERIFY(Complex(1.0f+TypeTraits<Float>::epsilon()/2, 3.7f) == Complex(1.0f, 3.7f));
    CORRADE_VERIFY(Complex(1.0f+TypeTraits<Float>::epsilon()*2, 3.7f) != Complex(1.0f, 3.7f));
}

void ComplexTest::isNormalized() {
    CORRADE_VERIFY(!Complex(2.5f, -3.7f).isNormalized());
    CORRADE_VERIFY(Complex::rotation(23.0_degf).isNormalized());
}

template<class T> void ComplexTest::isNormalizedEpsilon() {
    setTestCaseTemplateName(TypeTraits<T>::name());

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

void ComplexTest::multiplyDivideVector() {
    Complex a{ 2.5f, -0.5f};
    Vector2 b{-3.0f,  0.8f};
    Complex c{-7.5f, -0.4f};

    CORRADE_COMPARE(a*b, c);
    CORRADE_COMPARE(b*a, c);
    CORRADE_COMPARE(c/b, a);

    Complex d(-0.8f, -3.2f);
    CORRADE_COMPARE((Vector2{-2.0f, 1.6f}/a), d);
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
    setTestCaseTemplateName(TypeTraits<T>::name());

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
    Complex a(-0.6f, 0.8f);
    Complex b(-0.6f, -0.8f);

    Complex inverted = a.invertedNormalized();
    CORRADE_COMPARE(a*inverted, Complex());
    CORRADE_COMPARE(inverted*a, Complex());
    CORRADE_COMPARE(inverted, b);
}

void ComplexTest::invertedNormalizedNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    (Complex(-0.6f, 0.8f)*2).invertedNormalized();
    CORRADE_COMPARE(out.str(), "Math::Complex::invertedNormalized(): Complex(-1.2, 1.6) is not normalized\n");
}

void ComplexTest::angle() {
    auto a = Complex{ 1.5f, -2.0f}.normalized();
    auto b = Complex{-4.0f,  3.5f}.normalized();

    /* Verify also that the angle is the same as angle between 2D vectors */
    CORRADE_COMPARE(Math::angle(a, b), Math::angle(
        Vector2{ 1.5f, -2.0f}.normalized(),
        Vector2{-4.0f,  3.5f}.normalized()));
    CORRADE_COMPARE(Math::angle(a, b), 2.933128_radf);
    CORRADE_COMPARE(Math::angle(-a, -b), 2.933128_radf);
    CORRADE_COMPARE(Math::angle(-a, b), Rad(180.0_degf) - 2.933128_radf);
    CORRADE_COMPARE(Math::angle(a, -b), Rad(180.0_degf) - 2.933128_radf);

    /* Same / opposite */
    CORRADE_COMPARE(Math::angle(a, a), 0.0_radf);
    CORRADE_COMPARE(Math::angle(a, -a), 180.0_degf);
}

void ComplexTest::angleNormalizedButOver1() {
    /* This complex *is* normalized, but its length is larger than 1, which
       would cause acos() to return a NaN. Ensure it's clamped to correct range
       before passing it there. */
    Complex a{1.0f + Math::TypeTraits<Float>::epsilon()/2,  0.0f};
    CORRADE_VERIFY(a.isNormalized());

    CORRADE_COMPARE(Math::angle(a, a), 0.0_radf);
    CORRADE_COMPARE(Math::angle(a, -a), 180.0_degf);
}

void ComplexTest::angleNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Math::angle(Complex(1.5f, -2.0f).normalized(), {-4.0f, 3.5f});
    Math::angle({1.5f, -2.0f}, Complex(-4.0f, 3.5f).normalized());
    CORRADE_COMPARE(out.str(),
        "Math::angle(): complex numbers Complex(0.6, -0.8) and Complex(-4, 3.5) are not normalized\n"
        "Math::angle(): complex numbers Complex(1.5, -2) and Complex(-0.752577, 0.658505) are not normalized\n");
}

void ComplexTest::rotation() {
    Complex a = Complex::rotation(120.0_degf);
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(a, Complex(-0.5f, 0.8660254f));
    CORRADE_COMPARE_AS(a.angle(), 120.0_degf, Rad);

    /* Verify negative angle */
    Complex b = Complex::rotation(-240.0_degf);
    CORRADE_COMPARE(b, Complex(-0.5f, 0.8660254f));
    CORRADE_COMPARE_AS(b.angle(), 120.0_degf, Rad);

    /* Default-constructed complex number has zero angle */
    CORRADE_COMPARE_AS(Complex().angle(), 0.0_degf, Rad);
}

void ComplexTest::matrix() {
    Complex a = Complex::rotation(37.0_degf);
    Matrix2x2 m = Matrix3::rotation(37.0_degf).rotationScaling();

    CORRADE_COMPARE(a.toMatrix(), m);
    CORRADE_COMPARE(Complex::fromMatrix(m), a);
}

void ComplexTest::matrixNotOrthogonal() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Complex::fromMatrix(Matrix3::rotation(37.0_degf).rotationScaling()*2);
    CORRADE_COMPARE(out.str(),
        "Math::Complex::fromMatrix(): the matrix is not orthogonal:\n"
        "Matrix(1.59727, -1.20363,\n"
        "       1.20363, 1.59727)\n");
}

void ComplexTest::lerp() {
    /* Results should be consistent with QuaternionTest::lerp2D() (but not
       equivalent, probably because quaternions double cover and complex
       numbers not) */
    Complex a = Complex::rotation(15.0_degf);
    Complex b = Complex::rotation(57.0_degf);
    Complex lerp = Math::lerp(a, b, 0.35f);

    CORRADE_VERIFY(lerp.isNormalized());
    CORRADE_COMPARE(lerp.angle(), 29.4308_degf); /* almost but not quite 29.7 */
    CORRADE_COMPARE(lerp, (Complex{0.87095f, 0.491372f}));
}

void ComplexTest::lerpNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Complex a;
    Math::lerp(a*3.0f, a, 0.35f);
    Math::lerp(a, a*-3.0f, 0.35f);
    CORRADE_COMPARE(out.str(),
        "Math::lerp(): complex numbers Complex(3, 0) and Complex(1, 0) are not normalized\n"
        "Math::lerp(): complex numbers Complex(1, 0) and Complex(-3, -0) are not normalized\n");
}

void ComplexTest::slerp() {
    /* Result angle should be equivalent to QuaternionTest::slerp2D() */
    Complex a = Complex::rotation(15.0_degf);
    Complex b = Complex::rotation(57.0_degf);
    Complex slerp = Math::slerp(a, b, 0.35f);

    CORRADE_VERIFY(slerp.isNormalized());
    CORRADE_COMPARE(slerp.angle(), 29.7_degf); /* 15 + (57-15)*0.35 */
    CORRADE_COMPARE(slerp, (Complex{0.868632f, 0.495459f}));

    /* Avoid division by zero */
    CORRADE_COMPARE(Math::slerp(a, a, 0.25f), a);
}

void ComplexTest::slerpNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Complex a;
    Math::slerp(a*3.0f, a, 0.35f);
    Math::slerp(a, a*-3.0f, 0.35f);
    CORRADE_COMPARE(out.str(),
        "Math::slerp(): complex numbers Complex(3, 0) and Complex(1, 0) are not normalized\n"
        "Math::slerp(): complex numbers Complex(1, 0) and Complex(-3, -0) are not normalized\n");
}

void ComplexTest::transformVector() {
    Complex a = Complex::rotation(23.0_degf);
    Matrix3 m = Matrix3::rotation(23.0_degf);
    Vector2 v(-3.6f, 0.7f);

    Vector2 rotated = a.transformVector(v);
    CORRADE_COMPARE(rotated, m.transformVector(v));
    CORRADE_COMPARE(rotated, Vector2(-3.58733f, -0.762279f));
}

void ComplexTest::strictWeakOrdering() {
    StrictWeakOrdering o;
    const Complex a{1.0f, 2.0f};
    const Complex b{2.0f, 3.0f};
    const Complex c{1.0f, 3.0f};

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));
    CORRADE_VERIFY( o(c, b));
    CORRADE_VERIFY(!o(b, c));
    CORRADE_VERIFY(!o(a, a));
}

void ComplexTest::debug() {
    std::ostringstream o;

    Debug(&o) << Complex(2.5f, -7.5f);
    CORRADE_COMPARE(o.str(), "Complex(2.5, -7.5)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::ComplexTest)
