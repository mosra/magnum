/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Jonathan Hale <squareys@googlemail.com>

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

#include "Magnum/Math/DualComplex.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/StrictWeakOrdering.h"

struct DualCmpl {
    float re, im, x, y;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct DualComplexConverter<Float, DualCmpl> {
    constexpr static DualComplex<Float> from(const DualCmpl& other) {
        return {{other.re, other.im}, {other.x, other.y}};
    }

    constexpr static DualCmpl to(const DualComplex<Float>& other) {
        return {other.real().real(), other.real().imaginary(), other.dual().real(), other.dual().imaginary() };
    }
};

}

namespace Test { namespace {

struct DualComplexTest: Corrade::TestSuite::Tester {
    explicit DualComplexTest();

    void construct();
    void constructIdentity();
    void constructZero();
    void constructNoInit();
    void constructFromVector();
    void constructConversion();
    void constructCopy();
    void convert();

    void data();

    void isNormalized();
    template<class T> void isNormalizedEpsilonRotation();
    template<class T> void isNormalizedEpsilonTranslation();

    void multiply();

    void lengthSquared();
    void length();
    void normalized();
    template<class T> void normalizedIterative();

    void complexConjugated();
    void dualConjugated();
    void conjugated();
    void inverted();
    void invertedNormalized();
    void invertedNormalizedNotNormalized();

    void rotation();
    void translation();
    void combinedTransformParts();

    void fromParts();
    void matrix();
    void matrixNotOrthogonal();
    void transformVector();
    void transformPoint();

    void strictWeakOrdering();

    void debug();
};

using namespace Math::Literals;

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Complex<Float> Complex;
typedef Math::Dual<Float> Dual;
typedef Math::DualComplex<Float> DualComplex;
typedef Math::Matrix3<Float> Matrix3;
typedef Math::Vector2<Float> Vector2;

DualComplexTest::DualComplexTest() {
    addTests({&DualComplexTest::construct,
              &DualComplexTest::constructIdentity,
              &DualComplexTest::constructZero,
              &DualComplexTest::constructNoInit,
              &DualComplexTest::constructFromVector,
              &DualComplexTest::constructConversion,
              &DualComplexTest::constructCopy,
              &DualComplexTest::convert,

              &DualComplexTest::data,

              &DualComplexTest::isNormalized,
              &DualComplexTest::isNormalizedEpsilonRotation<Float>,
              &DualComplexTest::isNormalizedEpsilonRotation<Double>,
              &DualComplexTest::isNormalizedEpsilonTranslation<Float>,
              &DualComplexTest::isNormalizedEpsilonTranslation<Double>,

              &DualComplexTest::multiply,

              &DualComplexTest::lengthSquared,
              &DualComplexTest::length,
              &DualComplexTest::normalized});

    addRepeatedTests<DualComplexTest>({
        &DualComplexTest::normalizedIterative<Float>,
        &DualComplexTest::normalizedIterative<Double>}, 1000);

    addTests({&DualComplexTest::complexConjugated,
              &DualComplexTest::dualConjugated,
              &DualComplexTest::conjugated,
              &DualComplexTest::inverted,
              &DualComplexTest::invertedNormalized,
              &DualComplexTest::invertedNormalizedNotNormalized,

              &DualComplexTest::rotation,
              &DualComplexTest::translation,
              &DualComplexTest::combinedTransformParts,

              &DualComplexTest::fromParts,
              &DualComplexTest::matrix,
              &DualComplexTest::matrixNotOrthogonal,
              &DualComplexTest::transformVector,
              &DualComplexTest::transformPoint,

              &DualComplexTest::strictWeakOrdering,

              &DualComplexTest::debug});
}

void DualComplexTest::construct() {
    constexpr DualComplex a = {{-1.0f, 2.5f}, {3.0f, -7.5f}};
    CORRADE_COMPARE(a, DualComplex({-1.0f, 2.5f}, {3.0f, -7.5f}));
    CORRADE_COMPARE(a.real(), Complex(-1.0f, 2.5f));
    CORRADE_COMPARE(a.dual(), Complex(3.0f, -7.5f));

    constexpr DualComplex b(Complex(-1.0f, 2.5f));
    CORRADE_COMPARE(b, DualComplex({-1.0f, 2.5f}, {0.0f, 0.0f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<DualComplex, Complex, Complex>::value));
}

void DualComplexTest::constructIdentity() {
    constexpr DualComplex a;
    constexpr DualComplex b{IdentityInit};
    CORRADE_COMPARE(a, DualComplex({1.0f, 0.0f}, {0.0f, 0.0f}));
    CORRADE_COMPARE(b, DualComplex({1.0f, 0.0f}, {0.0f, 0.0f}));
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(b.length(), 1.0f);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<DualComplex>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<DualComplex, IdentityInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<IdentityInitT, DualComplex>::value));
}

void DualComplexTest::constructZero() {
    constexpr DualComplex a{ZeroInit};
    CORRADE_COMPARE(a, DualComplex({0.0f, 0.0f}, {0.0f, 0.0f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<DualComplex, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, DualComplex>::value));
}

void DualComplexTest::constructNoInit() {
    DualComplex a{{-1.0f, 2.5f}, {3.0f, -7.5f}};
    new(&a) DualComplex{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, DualComplex({-1.0f, 2.5f}, {3.0f, -7.5f}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<DualComplex, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, DualComplex>::value));
}

void DualComplexTest::constructFromVector() {
    constexpr DualComplex a(Vector2(1.5f, -3.0f));
    CORRADE_COMPARE(a, DualComplex({1.0f, 0.0f}, {1.5f, -3.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector2, DualComplex>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<DualComplex, Vector2>::value));
}

void DualComplexTest::constructConversion() {
    typedef Math::DualComplex<Int> DualComplexi;

    constexpr DualComplex a{{1.3f, 2.7f}, {-15.0f, 7.0f}};
    constexpr DualComplexi b{a};

    CORRADE_COMPARE(b, (DualComplexi{{1, 2}, {-15, 7}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<DualComplex, DualComplexi>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<DualComplex, DualComplexi>::value));
}

void DualComplexTest::constructCopy() {
    constexpr Math::Dual<Complex> a({-1.0f, 2.5f}, {3.0f, -7.5f});
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    DualComplex b(a);
    CORRADE_COMPARE(b, DualComplex({-1.0f, 2.5f}, {3.0f, -7.5f}));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<DualComplex>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<DualComplex>::value);
}

void DualComplexTest::convert() {
    constexpr DualCmpl a{1.5f, -3.5f, 7.0f, -0.5f};
    constexpr DualComplex b{{1.5f, -3.5f}, {7.0f, -0.5f}};

    /* GCC 5.1 had a bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66450
       Hopefully this does not reappear. */
    constexpr DualComplex c{a};
    CORRADE_COMPARE(c, b);

    constexpr DualCmpl d(b);
    CORRADE_COMPARE(d.re, a.re);
    CORRADE_COMPARE(d.im, a.im);
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<DualCmpl, DualComplex>::value));
    CORRADE_VERIFY(!(std::is_convertible<DualComplex, DualCmpl>::value));
}

void DualComplexTest::data() {
    constexpr DualComplex ca{{-1.0f, 2.5f}, {3.0f, -7.5f}};

    constexpr Complex b = ca.real();
    constexpr Complex c = ca.dual();
    CORRADE_COMPARE(b, Complex(-1.0f, 2.5f));
    CORRADE_COMPARE(c, Complex(3.0f, -7.5f));

    DualComplex a{{-1.0f, 2.5f}, {3.0f, -7.5f}};

    constexpr Float d = *ca.data();
    Float e = a.data()[3];
    CORRADE_COMPARE(d, -1.0f);
    CORRADE_COMPARE(e, -7.5f);
}

void DualComplexTest::isNormalized() {
    CORRADE_VERIFY(!DualComplex({2.0f, 1.0f}, {}).isNormalized());
    CORRADE_VERIFY((DualComplex::rotation(23.0_degf)*DualComplex::translation({6.0f, 3.0f})).isNormalized());
}

template<class T> void DualComplexTest::isNormalizedEpsilonRotation() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_VERIFY((Math::DualComplex<T>{{T(0.801775644243754) + TypeTraits<T>::epsilon()/T(2.0), T(0.597625146975521)}, {T(8018055.25501103), T(5975850.58193309)}}.isNormalized()));
    CORRADE_VERIFY(!(Math::DualComplex<T>{{T(0.801775644243754) + TypeTraits<T>::epsilon()*T(2.0), T(0.597625146975521)}, {T(8018055.25501103), T(5975850.58193309)}}.isNormalized()));
}

template<class T> void DualComplexTest::isNormalizedEpsilonTranslation() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    /* Translation does not affect normalization */
    CORRADE_VERIFY((Math::DualComplex<T>{{T(0.801775644243754), T(0.597625146975521)}, {T(8018055.25501103), T(20.5)}}.isNormalized()));
    CORRADE_VERIFY((Math::DualComplex<T>{{T(0.801775644243754), T(0.597625146975521)}, {T(8018055.25501103), T(-200000000.0)}}.isNormalized()));
}

void DualComplexTest::multiply() {
    DualComplex a({-1.5f,  2.0f}, { 3.0f, -6.5f});
    DualComplex b({ 2.0f, -7.5f}, {-0.5f,  1.0f});;
    CORRADE_COMPARE(a*b, DualComplex({12.0f, 15.25f}, {1.75f, -9.0f}));
}

void DualComplexTest::lengthSquared() {
    DualComplex a({-1.0f, 3.0f}, {0.5f, -2.0f});
    CORRADE_COMPARE(a.lengthSquared(), 10.0f);
}

void DualComplexTest::length() {
    DualComplex a({-1.0f, 3.0f}, {0.5f, -2.0f});
    CORRADE_COMPARE(a.length(), 3.162278f);
}

void DualComplexTest::normalized() {
    DualComplex a({-1.0f, 3.0f}, {0.5f, -2.0f});
    DualComplex b({-0.316228f, 0.948683f}, {0.5f, -2.0f});
    CORRADE_COMPARE(a.normalized().length(), 1.0f);
    CORRADE_COMPARE(a.normalized(), b);
}

template<class> struct NormalizedIterativeData;
template<> struct NormalizedIterativeData<Float> {
    static Math::Vector2<Float> translation() { return {10000.0f, -50.0f}; }
};
template<> struct NormalizedIterativeData<Double> {
    static Math::Vector2<Double> translation() { return {10000000.0, -500.0}; }
};

template<class T> void DualComplexTest::normalizedIterative() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    auto a = Math::DualComplex<T>::rotation(Math::Deg<T>{T(36.7)})*Math::DualComplex<T>::translation(NormalizedIterativeData<T>::translation());
    for(std::size_t i = 0; i != testCaseRepeatId(); ++i) {
        a = Math::DualComplex<T>::rotation(Math::Deg<T>{T(87.1)})*a;
        a = a.normalized();
    }

    CORRADE_VERIFY(a.isNormalized());
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
    DualComplex a({-1.0f, 1.5f}, {3.0f, -7.5f});
    DualComplex b({-0.307692f, -0.461538f}, {4.384616f, -0.923077f});
    CORRADE_COMPARE(a*a.inverted(), DualComplex());
    CORRADE_COMPARE(a.inverted(), b);
}

void DualComplexTest::invertedNormalized() {
    DualComplex a({-0.316228f,  0.9486831f}, {     3.0f,    -2.5f});
    DualComplex b({-0.316228f, -0.9486831f}, {3.320391f, 2.05548f});

    DualComplex inverted = a.invertedNormalized();
    CORRADE_COMPARE(a*inverted, DualComplex());
    CORRADE_COMPARE(inverted*a, DualComplex());
    CORRADE_COMPARE(inverted, b);
}

void DualComplexTest::invertedNormalizedNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    DualComplex({-1.0f, -2.5f}, {}).invertedNormalized();
    CORRADE_COMPARE(out.str(), "Math::Complex::invertedNormalized(): Complex(-1, -2.5) is not normalized\n");
}

void DualComplexTest::rotation() {
    DualComplex a = DualComplex::rotation(120.0_degf);
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(a, DualComplex({-0.5f, 0.8660254f}, {0.0f, 0.0f}));
    CORRADE_COMPARE_AS(a.rotation().angle(), 120.0_degf, Rad);

    /* Constexpr access to rotation */
    constexpr DualComplex b({-1.0f, 2.0f}, {});
    constexpr Complex c = b.rotation();
    CORRADE_COMPARE(c, Complex(-1.0f, 2.0f));

    /* Conversion from a rotation complex should give the same result */
    CORRADE_COMPARE(DualComplex{Complex::rotation(120.0_degf)}, a);
}

void DualComplexTest::translation() {
    Vector2 vec(1.5f, -3.5f);
    DualComplex a = DualComplex::translation(vec);
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(a, DualComplex({}, {1.5f, -3.5f}));
    CORRADE_COMPARE(a.translation(), vec);
}

void DualComplexTest::fromParts() {
    Complex r = Complex::rotation(120.0_degf);

    Vector2 vec{1.0f, -3.5f};
    DualComplex t = DualComplex::translation(vec);

    DualComplex rt = t*DualComplex{r};
    CORRADE_COMPARE(DualComplex::from(r, vec), rt);
}

void DualComplexTest::combinedTransformParts() {
    Vector2 translation = Vector2(-1.5f, 2.75f);
    DualComplex a = DualComplex::translation(translation)*DualComplex::rotation(23.0_degf);
    DualComplex b = DualComplex::rotation(23.0_degf)*DualComplex::translation(translation);

    CORRADE_COMPARE_AS(a.rotation().angle(), 23.0_degf, Rad);
    CORRADE_COMPARE_AS(b.rotation().angle(), 23.0_degf, Rad);
    CORRADE_COMPARE(a.translation(), translation);
    CORRADE_COMPARE(b.translation(), Complex::rotation(23.0_degf).transformVector(translation));
}

void DualComplexTest::matrix() {
    DualComplex a = DualComplex::rotation(23.0_degf)*DualComplex::translation({2.0f, 3.0f});
    Matrix3 m = Matrix3::rotation(23.0_degf)*Matrix3::translation({2.0f, 3.0f});

    CORRADE_COMPARE(a.toMatrix(), m);
    CORRADE_COMPARE(DualComplex::fromMatrix(m), a);
}

void DualComplexTest::matrixNotOrthogonal() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream o;
    Error redirectError{&o};

    DualComplex::fromMatrix(Matrix3::rotation(23.0_degf)*Matrix3::translation({2.0f, 3.0f})*2);
    CORRADE_COMPARE(o.str(),
        "Math::DualComplex::fromMatrix(): the matrix doesn't represent rigid transformation:\n"
        "Matrix(1.84101, -0.781462, 1.33763,\n"
        "       0.781462, 1.84101, 7.08595,\n"
        "       0, 0, 2)\n");
}

void DualComplexTest::transformVector() {
    DualComplex a = Complex::rotation(23.0_degf);
    Complex c = Complex::rotation(23.0_degf);
    Vector2 v{-3.6f, 0.7f};

    Vector2 rotated = a.transformVector(v);
    /* Delegates to Complex, so should give the same result */
    CORRADE_COMPARE(rotated, c.transformVector(v));
    CORRADE_COMPARE(rotated, (Vector2{-3.58733f, -0.762279f}));
}

void DualComplexTest::transformPoint() {
    DualComplex a = DualComplex::translation({2.0f, 3.0f})*DualComplex::rotation(23.0_degf);
    DualComplex b = DualComplex::rotation(23.0_degf)*DualComplex::translation({2.0f, 3.0f});
    Matrix3 m = Matrix3::translation({2.0f, 3.0f})*Matrix3::rotation(23.0_degf);
    Matrix3 n = Matrix3::rotation(23.0_degf)*Matrix3::translation({2.0f, 3.0f});
    Vector2 v(-3.6f, 0.7f);

    Vector2 transformedA = a.transformPoint(v);
    CORRADE_COMPARE(transformedA, m.transformPoint(v));
    CORRADE_COMPARE(transformedA, Vector2(-1.58733f, 2.237721f));

    Vector2 transformedB = b.transformPoint(v);
    CORRADE_COMPARE(transformedB, n.transformPoint(v));
    CORRADE_COMPARE(transformedB, Vector2(-2.918512f, 2.780698f));
}

void DualComplexTest::strictWeakOrdering() {
    StrictWeakOrdering o;
    const DualComplex a{{1.0f, 0.0f}, {1.0f, 3.0f}};
    const DualComplex b{{1.0f, 2.0f}, {3.0f, 4.0f}};
    const DualComplex c{{1.0f, 0.0f}, {1.0f, 4.0f}};

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));
    CORRADE_VERIFY( o(c, b));
    CORRADE_VERIFY(!o(b, c));
    CORRADE_VERIFY(!o(a, a));
}

void DualComplexTest::debug() {
    std::ostringstream o;

    Debug(&o) << DualComplex({-1.0f, -2.5f}, {-3.0f, -7.5f});
    CORRADE_COMPARE(o.str(), "DualComplex({-1, -2.5}, {-3, -7.5})\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DualComplexTest)
