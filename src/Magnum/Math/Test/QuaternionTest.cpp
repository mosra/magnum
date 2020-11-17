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
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Quaternion.h"
#include "Magnum/Math/StrictWeakOrdering.h"

struct Quat {
    float x, y, z, w;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct QuaternionConverter<Float, Quat> {
    constexpr static Quaternion<Float> from(const Quat& other) {
        return {{other.x, other.y, other.z}, other.w};
    }

    constexpr static Quat to(const Quaternion<Float>& other) {
        return {other.vector().x(), other.vector().y(), other.vector().z(), other.scalar() };
    }
};

}

namespace Test { namespace {

struct QuaternionTest: Corrade::TestSuite::Tester {
    explicit QuaternionTest();

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
    void axisAngle();
    void axisAngleNotNormalized();

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
    void invertedNormalizedNotNormalized();

    void rotation();
    void rotationNotNormalized();
    void angle();
    void angleNormalizedButOver1();
    void angleNotNormalized();
    void matrix();
    void matrixNotOrthogonal();
    void euler();
    void eulerNotNormalized();

    void lerp();
    void lerp2D();
    void lerpNotNormalized();
    void lerpShortestPath();
    void lerpShortestPathNotNormalized();
    void slerp();
    void slerpLinearFallback();
    template<class T> void slerpLinearFallbackIsNormalized();
    void slerp2D();
    void slerpNormalizedButOver1();
    void slerpNotNormalized();
    void slerpShortestPath();
    void slerpShortestPathLinearFallback();
    template<class T> void slerpShortestPathLinearFallbackIsNormalized();
    void slerpShortestPathNotNormalized();

    void transformVector();
    void transformVectorNormalized();
    void transformVectorNormalizedNotNormalized();

    void strictWeakOrdering();

    void debug();
};

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Matrix<3, Float> Matrix3x3;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::Quaternion<Float> Quaternion;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector4<Float> Vector4;

using namespace Math::Literals;

QuaternionTest::QuaternionTest() {
    addTests({&QuaternionTest::construct,
              &QuaternionTest::constructIdentity,
              &QuaternionTest::constructZero,
              &QuaternionTest::constructNoInit,
              &QuaternionTest::constructFromVector,
              &QuaternionTest::constructConversion,
              &QuaternionTest::constructCopy,
              &QuaternionTest::convert,

              &QuaternionTest::data,

              &QuaternionTest::compare,
              &QuaternionTest::isNormalized,
              &QuaternionTest::isNormalizedEpsilon<Float>,
              &QuaternionTest::isNormalizedEpsilon<Double>,
              &QuaternionTest::axisAngle,
              &QuaternionTest::axisAngleNotNormalized,

              &QuaternionTest::addSubtract,
              &QuaternionTest::negated,
              &QuaternionTest::multiplyDivideScalar,
              &QuaternionTest::multiply,

              &QuaternionTest::dot,
              &QuaternionTest::dotSelf,
              &QuaternionTest::length,
              &QuaternionTest::normalized});

    addRepeatedTests<QuaternionTest>({
        &QuaternionTest::normalizedIterative<Float>,
        &QuaternionTest::normalizedIterative<Double>}, 1000);

    addTests({&QuaternionTest::conjugated,
              &QuaternionTest::inverted,
              &QuaternionTest::invertedNormalized,
              &QuaternionTest::invertedNormalizedNotNormalized,

              &QuaternionTest::rotation,
              &QuaternionTest::rotationNotNormalized,
              &QuaternionTest::angle,
              &QuaternionTest::angleNormalizedButOver1,
              &QuaternionTest::angleNotNormalized,
              &QuaternionTest::matrix,
              &QuaternionTest::matrixNotOrthogonal,
              &QuaternionTest::euler,
              &QuaternionTest::eulerNotNormalized,

              &QuaternionTest::lerp,
              &QuaternionTest::lerp2D,
              &QuaternionTest::lerpNotNormalized,
              &QuaternionTest::lerpShortestPath,
              &QuaternionTest::lerpShortestPathNotNormalized,
              &QuaternionTest::slerp,
              &QuaternionTest::slerpLinearFallback,
              &QuaternionTest::slerpLinearFallbackIsNormalized<Float>,
              &QuaternionTest::slerpLinearFallbackIsNormalized<Double>,
              &QuaternionTest::slerp2D,
              &QuaternionTest::slerpNormalizedButOver1,
              &QuaternionTest::slerpNotNormalized,
              &QuaternionTest::slerpShortestPath,
              &QuaternionTest::slerpShortestPathLinearFallback,
              &QuaternionTest::slerpShortestPathLinearFallbackIsNormalized<Float>,
              &QuaternionTest::slerpShortestPathLinearFallbackIsNormalized<Double>,
              &QuaternionTest::slerpShortestPathNotNormalized,

              &QuaternionTest::transformVector,
              &QuaternionTest::transformVectorNormalized,
              &QuaternionTest::transformVectorNormalizedNotNormalized,

              &QuaternionTest::strictWeakOrdering,

              &QuaternionTest::debug});
}

void QuaternionTest::construct() {
    constexpr Quaternion a = {{1.0f, 2.0f, 3.0f}, -4.0f};
    CORRADE_COMPARE(a, Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    CORRADE_COMPARE(a.vector(), Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(a.scalar(), -4.0f);

    CORRADE_VERIFY((std::is_nothrow_constructible<Quaternion, Vector3, Float>::value));
}

void QuaternionTest::constructIdentity() {
    constexpr Quaternion a;
    constexpr Quaternion b{IdentityInit};
    CORRADE_COMPARE(a, Quaternion({0.0f, 0.0f, 0.0f}, 1.0f));
    CORRADE_COMPARE(b, Quaternion({0.0f, 0.0f, 0.0f}, 1.0f));
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(b.length(), 1.0f);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Quaternion>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Quaternion, IdentityInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<IdentityInitT, Quaternion>::value));
}

void QuaternionTest::constructZero() {
    constexpr Quaternion a{ZeroInit};
    CORRADE_COMPARE(a, Quaternion({0.0f, 0.0f, 0.0f}, 0.0f));

    CORRADE_VERIFY((std::is_nothrow_constructible<Quaternion, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Quaternion>::value));
}

void QuaternionTest::constructNoInit() {
    Quaternion a{{1.0f, 2.0f, 3.0f}, -4.0f};
    new(&a) Quaternion{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Quaternion, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Quaternion>::value));
}

void QuaternionTest::constructFromVector() {
    constexpr Quaternion a(Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(a, Quaternion({1.0f, 2.0f, 3.0f}, 0.0f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector3, Quaternion>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Quaternion, Vector3>::value));
}

void QuaternionTest::constructConversion() {
    typedef Math::Quaternion<Int> Quaternioni;

    constexpr Quaternion a{{1.3f, 2.7f, -15.0f}, 7.0f};
    constexpr Quaternioni b{a};

    CORRADE_COMPARE(b, (Quaternioni{{1, 2, -15}, 7}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Quaternion, Quaternioni>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Quaternion, Quaternioni>::value));
}

void QuaternionTest::constructCopy() {
    constexpr Quaternion a({1.0f, -3.0f, 7.0f}, 2.5f);
    constexpr Quaternion b(a);
    CORRADE_COMPARE(b, Quaternion({1.0f, -3.0f, 7.0f}, 2.5f));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Quaternion>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Quaternion>::value);
}

void QuaternionTest::convert() {
    constexpr Quat a{1.5f, -3.5f, 7.0f, -0.5f};
    constexpr Quaternion b{{1.5f, -3.5f, 7.0f}, -0.5f};

    /* GCC 5.1 had a bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66450
       Hopefully this does not reappear. */
    constexpr Quaternion c{a};
    CORRADE_COMPARE(c, b);

    constexpr Quat d(b);
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);
    CORRADE_COMPARE(d.w, a.w);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Quat, Quaternion>::value));
    CORRADE_VERIFY(!(std::is_convertible<Quaternion, Quat>::value));
}

void QuaternionTest::data() {
    constexpr Quaternion ca{{1.0f, 2.0f, 3.0f}, -4.0f};
    constexpr Vector3 vector = ca.vector();
    constexpr Float scalar = ca.scalar();
    CORRADE_COMPARE(vector, (Vector3{1.0f, 2.0f, 3.0f}));
    CORRADE_COMPARE(scalar, -4.0f);

    Quaternion a{{1.0f, 2.0f, 3.0f}, -4.0f};
    a.vector().y() = 4.3f;
    a.scalar() = 1.1f;
    CORRADE_COMPARE(a, (Quaternion{{1.0f, 4.3f, 3.0f}, 1.1f}));

    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Apparently dereferencing a pointer is verboten */
    constexpr
    #endif
    Float b = *ca.data();
    Float c = a.data()[3];
    CORRADE_COMPARE(b, 1.0f);
    CORRADE_COMPARE(c, 1.1f);
}

void QuaternionTest::compare() {
    CORRADE_VERIFY(Quaternion({1.0f+TypeTraits<Float>::epsilon()/2, 2.0f, 3.0f}, -4.0f) == Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    CORRADE_VERIFY(Quaternion({1.0f+TypeTraits<Float>::epsilon()*2, 2.0f, 3.0f}, -4.0f) != Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    CORRADE_VERIFY(Quaternion({4.0f, 2.0f, 3.0f}, -1.0f+TypeTraits<Float>::epsilon()/2) == Quaternion({4.0f, 2.0f, 3.0f}, -1.0f));
    CORRADE_VERIFY(Quaternion({4.0f, 2.0f, 3.0f}, -1.0f+TypeTraits<Float>::epsilon()*2) != Quaternion({4.0f, 2.0f, 3.0f}, -1.0f));
}

void QuaternionTest::isNormalized() {
    CORRADE_VERIFY(!Quaternion({1.0f, 2.0f, 3.0f}, 4.0f).isNormalized());
    CORRADE_VERIFY(Quaternion::rotation(23.0_degf, Vector3::xAxis()).isNormalized());
}

template<class T> void QuaternionTest::isNormalizedEpsilon() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_VERIFY((Math::Quaternion<T>{{T(0.0106550719778129), T(0.311128101752138), T(-0.0468823167023769)}, T(0.949151106053128) + TypeTraits<T>::epsilon()/T(2.0)}.isNormalized()));
    CORRADE_VERIFY(!(Math::Quaternion<T>{{T(0.0106550719778129), T(0.311128101752138), T(-0.0468823167023769)}, T(0.949151106053128) + TypeTraits<T>::epsilon()*T(2.0)}.isNormalized()));
}

void QuaternionTest::axisAngle() {
    Quaternion a = Quaternion::rotation(23.0_degf, {0.6f, -0.8f, 0.0f});
    CORRADE_COMPARE(a.angle(), 23.0_degf);
    CORRADE_COMPARE(a.axis(), (Vector3{0.6f, -0.8f, 0.0f}));
}

void QuaternionTest::axisAngleNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Quaternion a = Quaternion::rotation(23.0_degf, {0.6f, -0.8f, 0.0f})*2;
    a.angle();
    a.axis();
    CORRADE_COMPARE(out.str(),
        "Math::Quaternion::angle(): Quaternion({0.239242, -0.318989, 0}, 1.95985) is not normalized\n"
        "Math::Quaternion::axis(): Quaternion({0.239242, -0.318989, 0}, 1.95985) is not normalized\n");
}

void QuaternionTest::addSubtract() {
    Quaternion a({ 1.0f, 3.0f, -2.0f}, -4.0f);
    Quaternion b({-0.5f, 1.4f,  3.0f}, 12.0f);
    Quaternion c({ 0.5f, 4.4f,  1.0f},  8.0f);

    CORRADE_COMPARE(a + b, c);
    CORRADE_COMPARE(c - b, a);
}

void QuaternionTest::negated() {
    CORRADE_COMPARE(-Quaternion({1.0f, 2.0f, -3.0f}, -4.0f), Quaternion({-1.0f, -2.0f, 3.0f}, 4.0f));
}

void QuaternionTest::multiplyDivideScalar() {
    Quaternion a({ 1.0f,  3.0f, -2.0f}, -4.0f);
    Quaternion b({-1.5f, -4.5f,  3.0f},  6.0f);

    CORRADE_COMPARE(a*-1.5f, b);
    CORRADE_COMPARE(-1.5f*a, b);
    CORRADE_COMPARE(b/-1.5f, a);

    CORRADE_COMPARE(2.0f/a, Quaternion({2.0f, 0.666666f, -1.0f}, -0.5f));
}

void QuaternionTest::multiply() {
    CORRADE_COMPARE(Quaternion({-6.0f, -9.0f, 15.0f}, 0.5f)*Quaternion({2.0f, 3.0f, -5.0f}, 2.0f),
                    Quaternion({-11.0f, -16.5f, 27.5f}, 115.0f));
}

void QuaternionTest::dot() {
    Quaternion a({ 1.0f, 3.0f, -2.0f}, -4.0f);
    Quaternion b({-0.5f, 1.5f,  3.0f}, 12.0f);

    CORRADE_COMPARE(Math::dot(a, b), -50.0f);
}

void QuaternionTest::dotSelf() {
    CORRADE_COMPARE(Quaternion({1.0f, 2.0f, -3.0f}, -4.0f).dot(), 30.0f);
}

void QuaternionTest::length() {
    CORRADE_COMPARE(Quaternion({1.0f, 3.0f, -2.0f}, -4.0f).length(), std::sqrt(30.0f));
}

void QuaternionTest::normalized() {
    Quaternion normalized = Quaternion({1.0f, 3.0f, -2.0f}, -4.0f).normalized();
    CORRADE_COMPARE(normalized.length(), 1.0f);
    CORRADE_COMPARE(normalized, Quaternion({1.0f, 3.0f, -2.0f}, -4.0f)/std::sqrt(30.0f));
}

template<class T> void QuaternionTest::normalizedIterative() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    const auto axis = Math::Vector3<T>{T(0.5), T(7.9), T(0.1)}.normalized();
    auto a = Math::Quaternion<T>::rotation(Math::Deg<T>{T(36.7)}, Math::Vector3<T>{T(0.25), T(7.3), T(-1.1)}.normalized());
    for(std::size_t i = 0; i != testCaseRepeatId(); ++i) {
        a = Math::Quaternion<T>::rotation(Math::Deg<T>{T(87.1)}, axis)*a;
        a = a.normalized();
    }

    CORRADE_VERIFY(a.isNormalized());
}

void QuaternionTest::conjugated() {
    CORRADE_COMPARE(Quaternion({ 1.0f,  3.0f, -2.0f}, -4.0f).conjugated(),
                    Quaternion({-1.0f, -3.0f,  2.0f}, -4.0f));
}

void QuaternionTest::inverted() {
    Quaternion a = Quaternion({1.0f, 3.0f, -2.0f}, -4.0f);
    Quaternion inverted = a.inverted();

    CORRADE_COMPARE(a*inverted, Quaternion());
    CORRADE_COMPARE(inverted*a, Quaternion());
    CORRADE_COMPARE(inverted, Quaternion({-1.0f, -3.0f, 2.0f}, -4.0f)/30.0f);
}

void QuaternionTest::invertedNormalized() {
    Quaternion a = Quaternion{{1.0f, 3.0f, -2.0f}, -4.0f}.normalized();

    Quaternion inverted = a.invertedNormalized();
    CORRADE_COMPARE(a*inverted, Quaternion());
    CORRADE_COMPARE(inverted*a, Quaternion());
    CORRADE_COMPARE(inverted, Quaternion({-1.0f, -3.0f, 2.0f}, -4.0f)/std::sqrt(30.0f));
}

void QuaternionTest::invertedNormalizedNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Quaternion{{1.0f, 3.0f, -2.0f}, -4.0f}.invertedNormalized();
    CORRADE_COMPARE(out.str(), "Math::Quaternion::invertedNormalized(): Quaternion({1, 3, -2}, -4) is not normalized\n");
}

void QuaternionTest::rotation() {
    Vector3 axis(1.0f/Constants<Float>::sqrt3());
    Quaternion q = Quaternion::rotation(120.0_degf, axis);
    CORRADE_COMPARE(q.length(), 1.0f);
    CORRADE_COMPARE(q, Quaternion(Vector3(0.5f, 0.5f, 0.5f), 0.5f));
    CORRADE_COMPARE_AS(q.angle(), 120.0_degf, Deg);
    CORRADE_COMPARE(q.axis(), axis);
    CORRADE_COMPARE(q.axis().length(), 1.0f);

    /* Verify negative angle */
    Quaternion q2 = Quaternion::rotation(-120.0_degf, axis);
    CORRADE_COMPARE(q2, Quaternion(Vector3(-0.5f, -0.5f, -0.5f), 0.5f));
    CORRADE_COMPARE_AS(q2.angle(), 120.0_degf, Deg);
    CORRADE_COMPARE(q2.axis(), -axis);

    /* Default-constructed quaternion has zero angle and NaN axis */
    CORRADE_COMPARE_AS(Quaternion().angle(), 0.0_degf, Deg);
    CORRADE_VERIFY(Quaternion().axis() != Quaternion().axis());
}

void QuaternionTest::rotationNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Quaternion::rotation(-74.0_degf, {-1.0f, 2.0f, 2.0f});
    CORRADE_COMPARE(out.str(), "Math::Quaternion::rotation(): axis Vector(-1, 2, 2) is not normalized\n");
}

void QuaternionTest::angle() {
    auto a = Quaternion({1.0f, 2.0f, -3.0f}, -4.0f).normalized();
    auto b = Quaternion({4.0f, -3.0f, 2.0f}, -1.0f).normalized();

    /* Verify also that the angle is the same as angle between 4D vectors */
    CORRADE_COMPARE(Math::angle(a, b), Math::angle(
        Vector4{1.0f, 2.0f, -3.0f, -4.0f}.normalized(),
        Vector4{4.0f, -3.0f, 2.0f, -1.0f}.normalized()));
    CORRADE_COMPARE(Math::angle(a, b), 1.704528_radf);
    CORRADE_COMPARE(Math::angle(-a, -b), 1.704528_radf);
    CORRADE_COMPARE(Math::angle(-a, b), Rad(180.0_degf) - 1.704528_radf);
    CORRADE_COMPARE(Math::angle(a, -b), Rad(180.0_degf) - 1.704528_radf);

    /* Same / opposite. Well, almost. It's interesting how imprecise
       normalization can get. */
    CORRADE_COMPARE_WITH(Math::angle(a, a), 0.0_radf,
        Corrade::TestSuite::Compare::around(0.0005_radf));
    CORRADE_COMPARE_WITH(Math::angle(a, -a), 180.0_degf,
        Corrade::TestSuite::Compare::around(0.0005_radf));
}

void QuaternionTest::angleNormalizedButOver1() {
    /* This quaternion *is* normalized, but its length is larger than 1, which
       would cause acos() to return a NaN. Ensure it's clamped to correct range
       before passing it there. */
    Quaternion a{{1.0f + Math::TypeTraits<Float>::epsilon()/2,  0.0f, 0.0f}, 0.0f};
    CORRADE_VERIFY(a.isNormalized());

    CORRADE_COMPARE(Math::angle(a, a), 0.0_radf);
    CORRADE_COMPARE(Math::angle(a, -a), 180.0_degf);
}

void QuaternionTest::angleNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Math::angle(Quaternion{{1.0f, 2.0f, -3.0f}, -4.0f}.normalized(), {{4.0f, -3.0f, 2.0f}, -1.0f});
    Math::angle({{1.0f, 2.0f, -3.0f}, -4.0f}, Quaternion{{4.0f, -3.0f, 2.0f}, -1.0f}.normalized());

    CORRADE_COMPARE(out.str(),
        "Math::angle(): quaternions Quaternion({0.182574, 0.365148, -0.547723}, -0.730297) and Quaternion({4, -3, 2}, -1) are not normalized\n"
        "Math::angle(): quaternions Quaternion({1, 2, -3}, -4) and Quaternion({0.730297, -0.547723, 0.365148}, -0.182574) are not normalized\n");
}

void QuaternionTest::matrix() {
    Vector3 axis = Vector3(-3.0f, 1.0f, 5.0f).normalized();

    Quaternion q = Quaternion::rotation(37.0_degf, axis);
    Matrix3x3 m = Matrix4::rotation(37.0_degf, axis).rotationScaling();

    /* Verify that negated quaternion gives the same rotation */
    CORRADE_COMPARE(q.toMatrix(), m);
    CORRADE_COMPARE((-q).toMatrix(), m);

    /* Trace > 0 */
    CORRADE_COMPARE_AS(m.trace(), 0.0f, Corrade::TestSuite::Compare::Greater);
    CORRADE_COMPARE(Quaternion::fromMatrix(m), q);

    /* Trace < 0, max is diagonal[2] */
    Matrix3x3 m2 = Matrix4::rotation(130.0_degf, axis).rotationScaling();
    Quaternion q2 = Quaternion::rotation(130.0_degf, axis);
    CORRADE_COMPARE_AS(m2.trace(), 0.0f, Corrade::TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(m2.diagonal()[2],
        Math::max(m2.diagonal()[0], m2.diagonal()[1]),
        Corrade::TestSuite::Compare::Greater);
    CORRADE_COMPARE(Quaternion::fromMatrix(m2), q2);

    /* Trace < 0, max is diagonal[1] */
    Vector3 axis2 = Vector3(-3.0f, 5.0f, 1.0f).normalized();
    Matrix3x3 m3 = Matrix4::rotation(130.0_degf, axis2).rotationScaling();
    Quaternion q3 = Quaternion::rotation(130.0_degf, axis2);
    CORRADE_COMPARE_AS(m3.trace(), 0.0f, Corrade::TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(m3.diagonal()[1],
        Math::max(m3.diagonal()[0], m3.diagonal()[2]),
        Corrade::TestSuite::Compare::Greater);
    CORRADE_COMPARE(Quaternion::fromMatrix(m3), q3);

    /* Trace < 0, max is diagonal[0] */
    Vector3 axis3 = Vector3(5.0f, -3.0f, 1.0f).normalized();
    Matrix3x3 m4 = Matrix4::rotation(130.0_degf, axis3).rotationScaling();
    Quaternion q4 = Quaternion::rotation(130.0_degf, axis3);
    CORRADE_COMPARE_AS(m4.trace(), 0.0f, Corrade::TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(m4.diagonal()[0],
        Math::max(m4.diagonal()[1], m4.diagonal()[2]),
        Corrade::TestSuite::Compare::Greater);
    CORRADE_COMPARE(Quaternion::fromMatrix(m4), q4);
}

void QuaternionTest::matrixNotOrthogonal() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Vector3 axis = Vector3(-3.0f, 1.0f, 5.0f).normalized();
    Matrix3x3 m = Matrix4::rotation(37.0_degf, axis).rotationScaling();
    Quaternion::fromMatrix(m*2);
    CORRADE_COMPARE(out.str(),
        "Math::Quaternion::fromMatrix(): the matrix is not orthogonal:\n"
        "Matrix(1.70083, -1.05177, 0.0308525,\n"
        "       0.982733, 1.60878, 0.667885,\n"
        "       -0.376049, -0.552819, 1.88493)\n");
}

void QuaternionTest::euler() {
    Quaternion a = Quaternion{{0.35f, 0.134f, 0.37f}, 0.02f}.normalized();
    Math::Vector3<Rad> b{1.59867_radf, -1.15100_radf, 1.85697_radf};

    CORRADE_COMPARE(a.toEuler(), b);
    CORRADE_COMPARE(a,
        Quaternion::rotation(b.z(), Vector3::zAxis())*
        Quaternion::rotation(b.y(), Vector3::yAxis())*
        Quaternion::rotation(b.x(), Vector3::xAxis()));

    Quaternion a2{{-0.624252f, -0.331868f, -0.624468f}, 0.331983f};
    Math::Vector3<Rad> b2{0.0_radf, -1.57045_radf, -2.16434_radf};

    CORRADE_COMPARE(a2.toEuler(), b2);
    CORRADE_COMPARE(a2,
        Quaternion::rotation(b2.z(), Vector3::zAxis())*
        Quaternion::rotation(b2.y(), Vector3::yAxis())*
        Quaternion::rotation(b2.x(), Vector3::xAxis()));
}

void QuaternionTest::eulerNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Quaternion{{1.0f, 3.0f, -2.0f}, -4.0f}.toEuler();
    CORRADE_COMPARE(out.str(),
        "Math::Quaternion::toEuler(): Quaternion({1, 3, -2}, -4) is not normalized\n");
}

void QuaternionTest::lerp() {
    Quaternion a = Quaternion::rotation(15.0_degf, Vector3(1.0f/Constants<Float>::sqrt3()));
    Quaternion b = Quaternion::rotation(23.0_degf, Vector3::xAxis());

    Quaternion lerp = Math::lerp(a, b, 0.35f);
    Quaternion lerpShortestPath = Math::lerpShortestPath(a, b, 0.35f);
    Quaternion expected{{0.119127f, 0.049134f, 0.049134f}, 0.990445f};

    /* Both should give the same result */
    CORRADE_VERIFY(lerp.isNormalized());
    CORRADE_VERIFY(lerpShortestPath.isNormalized());
    CORRADE_COMPARE(lerp, expected);
    CORRADE_COMPARE(lerpShortestPath, expected);
}

void QuaternionTest::lerp2D() {
    /* Results should be consistent with ComplexTest::lerp() */
    Quaternion a = Quaternion::rotation(15.0_degf, Vector3::zAxis());
    Quaternion b = Quaternion::rotation(57.0_degf, Vector3::zAxis());
    Quaternion lerp = Math::lerp(a, b, 0.35f);

    CORRADE_VERIFY(lerp.isNormalized());
    CORRADE_COMPARE(lerp.angle(), 29.6351_degf); /* almost but not quite 29.7 */
    CORRADE_COMPARE(lerp, (Quaternion{{0.0f, 0.0f, 0.255742f}, 0.966745f}));
}

void QuaternionTest::lerpNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Quaternion a;
    Math::lerp(a*3.0f, a, 0.35f);
    Math::lerp(a, a*-3.0f, 0.35f);
    CORRADE_COMPARE(out.str(),
        "Math::lerp(): quaternions Quaternion({0, 0, 0}, 3) and Quaternion({0, 0, 0}, 1) are not normalized\n"
        "Math::lerp(): quaternions Quaternion({0, 0, 0}, 1) and Quaternion({-0, -0, -0}, -3) are not normalized\n");
}

void QuaternionTest::lerpShortestPath() {
    Quaternion a = Quaternion::rotation(0.0_degf, Vector3::zAxis());
    Quaternion b = Quaternion::rotation(225.0_degf, Vector3::zAxis());

    Quaternion lerp = Math::lerp(a, b, 0.25f);
    Quaternion lerpShortestPath = Math::lerpShortestPath(a, b, 0.25f);

    CORRADE_VERIFY(lerp.isNormalized());
    CORRADE_VERIFY(lerpShortestPath.isNormalized());
    CORRADE_COMPARE(lerp.axis(), Vector3::zAxis());
    CORRADE_COMPARE(lerpShortestPath.axis(), Vector3::zAxis());
    CORRADE_COMPARE(lerp.angle(), 38.8848_degf);
    CORRADE_COMPARE(lerpShortestPath.angle(), 329.448_degf);

    CORRADE_COMPARE(lerp, (Quaternion{{0.0f, 0.0f, 0.332859f}, 0.942977f}));
    CORRADE_COMPARE(lerpShortestPath, (Quaternion{{0.0f, 0.0f, 0.26347f}, -0.964667f}));
}

void QuaternionTest::lerpShortestPathNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Quaternion a;
    Math::lerpShortestPath(a*3.0f, a, 0.35f);
    Math::lerpShortestPath(a, a*-3.0f, 0.35f);
    /* lerpShortestPath() is calling lerp(), so the message is from there */
    CORRADE_COMPARE(out.str(),
        "Math::lerp(): quaternions Quaternion({0, 0, 0}, 3) and Quaternion({0, 0, 0}, 1) are not normalized\n"
        "Math::lerp(): quaternions Quaternion({-0, -0, -0}, -1) and Quaternion({-0, -0, -0}, -3) are not normalized\n");
}

void QuaternionTest::slerp() {
    Quaternion a = Quaternion::rotation(15.0_degf, Vector3(1.0f/Constants<Float>::sqrt3()));
    Quaternion b = Quaternion::rotation(23.0_degf, Vector3::xAxis());

    Quaternion slerp = Math::slerp(a, b, 0.35f);
    Quaternion slerpShortestPath = Math::slerpShortestPath(a, b, 0.35f);
    Quaternion expected{{0.1191653f, 0.0491109f, 0.0491109f}, 0.9904423f};

    /* Both should give the same result */
    CORRADE_VERIFY(slerp.isNormalized());
    CORRADE_COMPARE(slerp, expected);
    CORRADE_VERIFY(slerpShortestPath.isNormalized());
    CORRADE_COMPARE(slerpShortestPath, expected);
}

void QuaternionTest::slerpLinearFallback() {
    Quaternion a = Quaternion::rotation(23.0_degf, Vector3::xAxis());

    /* Returning the same */
    CORRADE_COMPARE(Math::slerp(a, a, 0.25f), a);

    /* Returning the second when negated */
    CORRADE_COMPARE(Math::slerp(a, -a, 0.0f), -a);
    CORRADE_COMPARE(Math::slerp(a, -a, 0.5f), -a);
    CORRADE_COMPARE(Math::slerp(a, -a, 1.0f), -a);
}

template<class T> void QuaternionTest::slerpLinearFallbackIsNormalized() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    Math::Quaternion<T> a = Math::Quaternion<T>::rotation({}, Math::Vector3<T>::xAxis());
    Math::Quaternion<T> b = Math::Quaternion<T>::rotation(Math::acos(T(1) - T(0.49999)*TypeTraits<T>::epsilon()), Math::Vector3<T>::xAxis());

    /* Ensure we're in the special case */
    CORRADE_VERIFY(std::abs(Math::dot(a, b)) > T(1) - T(0.5)*TypeTraits<T>::epsilon());

    /* Edges */
    CORRADE_COMPARE(Math::slerp(a, b, T(0.0)), a);
    CORRADE_COMPARE(Math::slerp(a, b, T(1.0)), b);

    /* Midpoint should still be normalized */
    CORRADE_VERIFY(Math::slerp(a, b, T(0.5)).isNormalized());
}

void QuaternionTest::slerp2D() {
    /* Result angle should be equivalent to ComplexTest::slerp() */
    Quaternion a = Quaternion::rotation(15.0_degf, Vector3::zAxis());
    Quaternion b = Quaternion::rotation(57.0_degf, Vector3::zAxis());
    Quaternion slerp = Math::slerp(a, b, 0.35f);

    CORRADE_VERIFY(slerp.isNormalized());
    CORRADE_COMPARE(slerp.angle(), 29.7_degf); /* 15 + (57-15)*0.35 */
    CORRADE_COMPARE(slerp, (Quaternion{{0.0f, 0.0f, 0.256289f}, 0.9666f}));
}

void QuaternionTest::slerpNormalizedButOver1() {
    /* This quaternion *is* normalized, but its length is larger than 1, which
       would cause acos() to return a NaN. Ensure it's clamped to correct range
       before passing it there. */
    Quaternion a{{1.0f + Math::TypeTraits<Float>::epsilon()/2,  0.0f, 0.0f}, 0.0f};

    /* Returning the same */
    CORRADE_COMPARE(Math::slerp(a, a, 0.25f), a);

    /* Returning the second when negated */
    CORRADE_COMPARE(Math::slerp(a, -a, 0.0f), -a);
    CORRADE_COMPARE(Math::slerp(a, -a, 0.5f), -a);
    CORRADE_COMPARE(Math::slerp(a, -a, 1.0f), -a);
}

void QuaternionTest::slerpNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Quaternion a;
    Math::slerp(a*3.0f, a, 0.35f);
    Math::slerp(a, a*-3.0f, 0.35f);
    CORRADE_COMPARE(out.str(),
        "Math::slerp(): quaternions Quaternion({0, 0, 0}, 3) and Quaternion({0, 0, 0}, 1) are not normalized\n"
        "Math::slerp(): quaternions Quaternion({0, 0, 0}, 1) and Quaternion({-0, -0, -0}, -3) are not normalized\n");
}

void QuaternionTest::slerpShortestPath() {
    Quaternion a = Quaternion::rotation(0.0_degf, Vector3::zAxis());
    Quaternion b = Quaternion::rotation(225.0_degf, Vector3::zAxis());

    Quaternion slerp = Math::slerp(a, b, 0.25f);
    Quaternion slerpShortestPath = Math::slerpShortestPath(a, b, 0.25f);

    CORRADE_VERIFY(slerp.isNormalized());
    CORRADE_VERIFY(slerpShortestPath.isNormalized());
    CORRADE_COMPARE(slerp.axis(), Vector3::zAxis());
    CORRADE_COMPARE(slerpShortestPath.axis(), Vector3::zAxis());
    CORRADE_COMPARE(slerp.angle(), 56.25_degf);
    CORRADE_COMPARE(slerpShortestPath.angle(), 326.25_degf);

    CORRADE_COMPARE(slerp, (Quaternion{{0.0f, 0.0f, 0.471397f}, 0.881921f}));
    CORRADE_COMPARE(slerpShortestPath, (Quaternion{{0.0f, 0.0f, 0.290285f}, -0.95694f}));
}

void QuaternionTest::slerpShortestPathLinearFallback() {
    Quaternion a = Quaternion::rotation(23.0_degf, Vector3::xAxis());

    /* Returning the same */
    CORRADE_COMPARE(Math::slerpShortestPath(a, a, 0.25f), a);

    /* Returning the second when negated */
    CORRADE_COMPARE(Math::slerpShortestPath(a, -a, 0.0f), -a);
    CORRADE_COMPARE(Math::slerpShortestPath(a, -a, 0.5f), -a);
    CORRADE_COMPARE(Math::slerpShortestPath(a, -a, 1.0f), -a);
}

template<class T> void QuaternionTest::slerpShortestPathLinearFallbackIsNormalized() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    Math::Quaternion<T> a = Math::Quaternion<T>::rotation({}, Math::Vector3<T>::xAxis());
    Math::Quaternion<T> b = Math::Quaternion<T>::rotation(Math::acos(T(1) - T(0.49999)*TypeTraits<T>::epsilon()), Math::Vector3<T>::xAxis());

    /* Ensure we're in the special case */
    CORRADE_VERIFY(std::abs(Math::dot(a, b)) > T(1) - T(0.5)*TypeTraits<T>::epsilon());

    /* Edges */
    CORRADE_COMPARE(Math::slerpShortestPath(a, b, T(0.0)), a);
    CORRADE_COMPARE(Math::slerpShortestPath(a, b, T(1.0)), b);

    /* Midpoint should still be normalized */
    CORRADE_VERIFY(Math::slerpShortestPath(a, b, T(0.5)).isNormalized());
}

void QuaternionTest::slerpShortestPathNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Quaternion a;
    Math::slerpShortestPath(a*3.0f, a, 0.35f);
    Math::slerpShortestPath(a, a*-3.0f, 0.35f);
    CORRADE_COMPARE(out.str(),
        "Math::slerpShortestPath(): quaternions Quaternion({0, 0, 0}, 3) and Quaternion({0, 0, 0}, 1) are not normalized\n"
        "Math::slerpShortestPath(): quaternions Quaternion({0, 0, 0}, 1) and Quaternion({-0, -0, -0}, -3) are not normalized\n");
}

void QuaternionTest::transformVector() {
    Quaternion a = Quaternion::rotation(23.0_degf, Vector3::xAxis());
    Matrix4 m = Matrix4::rotationX(23.0_degf);
    Vector3 v(5.0f, -3.6f, 0.7f);

    Vector3 rotated = a.transformVector(v);
    CORRADE_COMPARE(rotated, m.transformVector(v));
    CORRADE_COMPARE(rotated, Vector3(5.0f, -3.58733f, -0.762279f));
}

void QuaternionTest::transformVectorNormalized() {
    Quaternion a = Quaternion::rotation(23.0_degf, Vector3::xAxis());
    Matrix4 m = Matrix4::rotationX(23.0_degf);
    Vector3 v(5.0f, -3.6f, 0.7f);

    Vector3 rotated = a.transformVectorNormalized(v);
    CORRADE_COMPARE(rotated, m.transformVector(v));
    CORRADE_COMPARE(rotated, a.transformVector(v));
}

void QuaternionTest::transformVectorNormalizedNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Quaternion a = Quaternion::rotation(23.0_degf, Vector3::xAxis());
    (a*2).transformVectorNormalized({});
    CORRADE_COMPARE(out.str(), "Math::Quaternion::transformVectorNormalized(): Quaternion({0.398736, 0, 0}, 1.95985) is not normalized\n");
}

void QuaternionTest::strictWeakOrdering() {
    StrictWeakOrdering o;
    const Quaternion a{{1.0f, 2.0f, 3.0f}, 4.0f};
    const Quaternion b{{2.0f, 3.0f, 4.0f}, 5.0f};
    const Quaternion c{{1.0f, 2.0f, 3.0f}, 5.0f};

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));
    CORRADE_VERIFY( o(c, b));
    CORRADE_VERIFY(!o(b, c));

    CORRADE_VERIFY(!o(a, a));
}

void QuaternionTest::debug() {
    std::ostringstream o;

    Debug(&o) << Quaternion({1.0f, 2.0f, 3.0f}, -4.0f);
    CORRADE_COMPARE(o.str(), "Quaternion({1, 2, 3}, -4)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::QuaternionTest)
