/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/Configuration.h>

#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Quaternion.h"

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

namespace Test {

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

    void rotation();
    void angle();
    void matrix();

    void lerp();
    void lerpShortestPath();
    void lerp2D();
    void lerpNotNormalized();
    void slerp();
    void slerpShortestPath();
    void slerp2D();
    void slerpNotNormalized();

    void transformVector();
    void transformVectorNormalized();

    void debug();
    void configuration();
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

              &QuaternionTest::rotation,
              &QuaternionTest::angle,
              &QuaternionTest::matrix,

              &QuaternionTest::lerp,
              &QuaternionTest::lerpShortestPath,
              &QuaternionTest::lerp2D,
              &QuaternionTest::lerpNotNormalized,
              &QuaternionTest::slerp,
              &QuaternionTest::slerpShortestPath,
              &QuaternionTest::slerp2D,
              &QuaternionTest::slerpNotNormalized,

              &QuaternionTest::transformVector,
              &QuaternionTest::transformVectorNormalized,

              &QuaternionTest::debug,
              &QuaternionTest::configuration});
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
}

void QuaternionTest::constructZero() {
    constexpr Quaternion a{ZeroInit};
    CORRADE_COMPARE(a, Quaternion({0.0f, 0.0f, 0.0f}, 0.0f));

    CORRADE_VERIFY((std::is_nothrow_constructible<Quaternion, ZeroInitT>::value));
}

void QuaternionTest::constructNoInit() {
    Quaternion a{{1.0f, 2.0f, 3.0f}, -4.0f};
    new(&a) Quaternion{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Quaternion, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, Quaternion>::value));
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
}

void QuaternionTest::compare() {
    CORRADE_VERIFY(Quaternion({1.0f+TypeTraits<Float>::epsilon()/2, 2.0f, 3.0f}, -4.0f) == Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    CORRADE_VERIFY(Quaternion({1.0f+TypeTraits<Float>::epsilon()*2, 2.0f, 3.0f}, -4.0f) != Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    CORRADE_VERIFY(Quaternion({4.0f, 2.0f, 3.0f}, -1.0f+TypeTraits<Float>::epsilon()/2) == Quaternion({4.0f, 2.0f, 3.0f}, -1.0f));
    CORRADE_VERIFY(Quaternion({4.0f, 2.0f, 3.0f}, -1.0f+TypeTraits<Float>::epsilon()*2) != Quaternion({4.0f, 2.0f, 3.0f}, -1.0f));
}

void QuaternionTest::isNormalized() {
    CORRADE_VERIFY(!Quaternion({1.0f, 2.0f, 3.0f}, 4.0f).isNormalized());
    CORRADE_VERIFY(Quaternion::rotation(Deg(23.0f), Vector3::xAxis()).isNormalized());
}

template<class T> void QuaternionTest::isNormalizedEpsilon() {
    setTestCaseName(std::string{"isNormalizedEpsilon<"} + TypeTraits<T>::name() + ">");

    CORRADE_VERIFY((Math::Quaternion<T>{{T(0.0106550719778129), T(0.311128101752138), T(-0.0468823167023769)}, T(0.949151106053128) + TypeTraits<T>::epsilon()/T(2.0)}.isNormalized()));
    CORRADE_VERIFY(!(Math::Quaternion<T>{{T(0.0106550719778129), T(0.311128101752138), T(-0.0468823167023769)}, T(0.949151106053128) + TypeTraits<T>::epsilon()*T(2.0)}.isNormalized()));
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
    setTestCaseName(std::string{"normalizedIterative<"} + TypeTraits<T>::name() + ">");

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
    Quaternion a = Quaternion({1.0f, 3.0f, -2.0f}, -4.0f);

    std::ostringstream o;
    Error redirectError{&o};
    a.invertedNormalized();
    CORRADE_COMPARE(o.str(), "Math::Quaternion::invertedNormalized(): quaternion must be normalized\n");

    Quaternion aNormalized = a.normalized();
    Quaternion inverted = aNormalized.invertedNormalized();
    CORRADE_COMPARE(aNormalized*inverted, Quaternion());
    CORRADE_COMPARE(inverted*aNormalized, Quaternion());
    CORRADE_COMPARE(inverted, Quaternion({-1.0f, -3.0f, 2.0f}, -4.0f)/std::sqrt(30.0f));
}

void QuaternionTest::rotation() {
    std::ostringstream o;
    Error redirectError{&o};

    Vector3 axis(1.0f/Constants<Float>::sqrt3());

    CORRADE_COMPARE(Quaternion::rotation(Deg(-74.0f), {-1.0f, 2.0f, 2.0f}), Quaternion());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::rotation(): axis must be normalized\n");

    Quaternion q = Quaternion::rotation(Deg(120.0f), axis);
    CORRADE_COMPARE(q.length(), 1.0f);
    CORRADE_COMPARE(q, Quaternion(Vector3(0.5f, 0.5f, 0.5f), 0.5f));
    CORRADE_COMPARE_AS(q.angle(), Deg(120.0f), Deg);
    CORRADE_COMPARE(q.axis(), axis);
    CORRADE_COMPARE(q.axis().length(), 1.0f);

    /* Verify negative angle */
    Quaternion q2 = Quaternion::rotation(Deg(-120.0f), axis);
    CORRADE_COMPARE(q2, Quaternion(Vector3(-0.5f, -0.5f, -0.5f), 0.5f));
    CORRADE_COMPARE_AS(q2.angle(), Deg(120.0f), Deg);
    CORRADE_COMPARE(q2.axis(), -axis);

    /* Default-constructed quaternion has zero angle and NaN axis */
    CORRADE_COMPARE_AS(Quaternion().angle(), Deg(0.0f), Deg);
    CORRADE_VERIFY(Quaternion().axis() != Quaternion().axis());
}

void QuaternionTest::angle() {
    std::ostringstream o;
    Error redirectError{&o};
    Math::angle(Quaternion({1.0f, 2.0f, -3.0f}, -4.0f).normalized(), {{4.0f, -3.0f, 2.0f}, -1.0f});
    CORRADE_COMPARE(o.str(), "Math::angle(): quaternions must be normalized\n");

    o.str({});
    Math::angle({{1.0f, 2.0f, -3.0f}, -4.0f}, Quaternion({4.0f, -3.0f, 2.0f}, -1.0f).normalized());
    CORRADE_COMPARE(o.str(), "Math::angle(): quaternions must be normalized\n");

    /* Verify also that the angle is the same as angle between 4D vectors */
    Rad angle = Math::angle(Quaternion({1.0f, 2.0f, -3.0f}, -4.0f).normalized(),
                            Quaternion({4.0f, -3.0f, 2.0f}, -1.0f).normalized());
    CORRADE_COMPARE(angle, Math::angle(Vector4(1.0f, 2.0f, -3.0f, -4.0f).normalized(),
                                 Vector4(4.0f, -3.0f, 2.0f, -1.0f).normalized()));
    CORRADE_COMPARE(angle, Rad(1.704528f));
}

void QuaternionTest::matrix() {
    Vector3 axis = Vector3(-3.0f, 1.0f, 5.0f).normalized();

    Quaternion q = Quaternion::rotation(Deg(37.0f), axis);
    Matrix3x3 m = Matrix4::rotation(Deg(37.0f), axis).rotationScaling();

    /* Verify that negated quaternion gives the same rotation */
    CORRADE_COMPARE(q.toMatrix(), m);
    CORRADE_COMPARE((-q).toMatrix(), m);

    std::ostringstream o;
    Error redirectError{&o};
    Quaternion::fromMatrix(m*2);
    CORRADE_COMPARE(o.str(), "Math::Quaternion::fromMatrix(): the matrix is not orthogonal\n");

    /* Trace > 0 */
    CORRADE_COMPARE_AS(m.trace(), 0.0f, Corrade::TestSuite::Compare::Greater);
    CORRADE_COMPARE(Quaternion::fromMatrix(m), q);

    /* Trace < 0, max is diagonal[2] */
    Matrix3x3 m2 = Matrix4::rotation(Deg(130.0f), axis).rotationScaling();
    Quaternion q2 = Quaternion::rotation(Deg(130.0f), axis);
    CORRADE_COMPARE_AS(m2.trace(), 0.0f, Corrade::TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(m2.diagonal()[2],
        std::max(m2.diagonal()[0], m2.diagonal()[1]),
        Corrade::TestSuite::Compare::Greater);
    CORRADE_COMPARE(Quaternion::fromMatrix(m2), q2);

    /* Trace < 0, max is diagonal[1] */
    Vector3 axis2 = Vector3(-3.0f, 5.0f, 1.0f).normalized();
    Matrix3x3 m3 = Matrix4::rotation(Deg(130.0f), axis2).rotationScaling();
    Quaternion q3 = Quaternion::rotation(Deg(130.0f), axis2);
    CORRADE_COMPARE_AS(m3.trace(), 0.0f, Corrade::TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(m3.diagonal()[1],
        std::max(m3.diagonal()[0], m3.diagonal()[2]),
        Corrade::TestSuite::Compare::Greater);
    CORRADE_COMPARE(Quaternion::fromMatrix(m3), q3);

    /* Trace < 0, max is diagonal[0] */
    Vector3 axis3 = Vector3(5.0f, -3.0f, 1.0f).normalized();
    Matrix3x3 m4 = Matrix4::rotation(Deg(130.0f), axis3).rotationScaling();
    Quaternion q4 = Quaternion::rotation(Deg(130.0f), axis3);
    CORRADE_COMPARE_AS(m4.trace(), 0.0f, Corrade::TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(m4.diagonal()[0],
        std::max(m4.diagonal()[1], m4.diagonal()[2]),
        Corrade::TestSuite::Compare::Greater);
    CORRADE_COMPARE(Quaternion::fromMatrix(m4), q4);
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

void QuaternionTest::lerpShortestPath() {
    Quaternion a = Quaternion::rotation(0.0_degf, Vector3::zAxis());
    Quaternion b = Quaternion::rotation(225.0_degf, Vector3::zAxis());

    Quaternion slerp = Math::lerp(a, b, 0.25f);
    Quaternion slerpShortestPath = Math::lerpShortestPath(a, b, 0.25f);

    CORRADE_VERIFY(slerp.isNormalized());
    CORRADE_VERIFY(slerpShortestPath.isNormalized());
    CORRADE_COMPARE(slerp.axis(), Vector3::zAxis());
    CORRADE_COMPARE(slerpShortestPath.axis(), Vector3::zAxis());
    CORRADE_COMPARE(slerp.angle(), 38.8848_degf);
    CORRADE_COMPARE(slerpShortestPath.angle(), 329.448_degf);

    CORRADE_COMPARE(slerp, (Quaternion{{0.0f, 0.0f, 0.332859f}, 0.942977f}));
    CORRADE_COMPARE(slerpShortestPath, (Quaternion{{0.0f, 0.0f, 0.26347f}, -0.964667f}));
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
    std::ostringstream out;
    Error redirectError{&out};

    Quaternion a;
    Math::lerp(a*3.0f, a, 0.35f);
    Math::lerp(a, a*-3.0f, 0.35f);
    CORRADE_COMPARE(out.str(),
        "Math::lerp(): quaternions must be normalized\n"
        "Math::lerp(): quaternions must be normalized\n");
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

    /* Avoid division by zero */
    CORRADE_COMPARE(Math::slerp(a, a, 0.25f), a);
    CORRADE_COMPARE(Math::slerp(a, -a, 0.42f), a);
    CORRADE_COMPARE(Math::slerpShortestPath(a, a, 0.25f), a);
    CORRADE_COMPARE(Math::slerpShortestPath(a, -a, 0.25f), a);
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

void QuaternionTest::slerp2D() {
    /* Result angle should be equivalent to ComplexTest::slerp() */
    Quaternion a = Quaternion::rotation(15.0_degf, Vector3::zAxis());
    Quaternion b = Quaternion::rotation(57.0_degf, Vector3::zAxis());
    Quaternion slerp = Math::slerp(a, b, 0.35f);

    CORRADE_VERIFY(slerp.isNormalized());
    CORRADE_COMPARE(slerp.angle(), 29.7_degf); /* 15 + (57-15)*0.35 */
    CORRADE_COMPARE(slerp, (Quaternion{{0.0f, 0.0f, 0.256289f}, 0.9666f}));
}

void QuaternionTest::slerpNotNormalized() {
    std::ostringstream out;
    Error redirectError{&out};

    Quaternion a;
    Math::slerp(a*3.0f, a, 0.35f);
    Math::slerp(a, a*-3.0f, 0.35f);
    CORRADE_COMPARE(out.str(),
        "Math::slerp(): quaternions must be normalized\n"
        "Math::slerp(): quaternions must be normalized\n");
}

void QuaternionTest::transformVector() {
    Quaternion a = Quaternion::rotation(Deg(23.0f), Vector3::xAxis());
    Matrix4 m = Matrix4::rotationX(Deg(23.0f));
    Vector3 v(5.0f, -3.6f, 0.7f);

    Vector3 rotated = a.transformVector(v);
    CORRADE_COMPARE(rotated, m.transformVector(v));
    CORRADE_COMPARE(rotated, Vector3(5.0f, -3.58733f, -0.762279f));
}

void QuaternionTest::transformVectorNormalized() {
    Quaternion a = Quaternion::rotation(Deg(23.0f), Vector3::xAxis());
    Matrix4 m = Matrix4::rotationX(Deg(23.0f));
    Vector3 v(5.0f, -3.6f, 0.7f);

    std::ostringstream o;
    Error redirectError{&o};
    (a*2).transformVectorNormalized(v);
    CORRADE_COMPARE(o.str(), "Math::Quaternion::transformVectorNormalized(): quaternion must be normalized\n");

    Vector3 rotated = a.transformVectorNormalized(v);
    CORRADE_COMPARE(rotated, m.transformVector(v));
    CORRADE_COMPARE(rotated, a.transformVector(v));
}

void QuaternionTest::debug() {
    std::ostringstream o;

    Debug(&o) << Quaternion({1.0f, 2.0f, 3.0f}, -4.0f);
    CORRADE_COMPARE(o.str(), "Quaternion({1, 2, 3}, -4)\n");
}

void QuaternionTest::configuration() {
    Corrade::Utility::Configuration c;

    Quaternion q{{3.0f, 3.125f, 9.0f}, 9.55f};
    std::string value{"3 3.125 9 9.55"};

    c.setValue("quat", q);
    CORRADE_COMPARE(c.value("quat"), value);
    CORRADE_COMPARE(c.value<Quaternion>("quat"), q);

    /* Underflow */
    c.setValue("underflow", "2.1 8.9");
    CORRADE_COMPARE(c.value<Quaternion>("underflow"), (Quaternion{{2.1f, 8.9f, 0.0f}, 0.0f}));

    /* Overflow */
    c.setValue("overflow", "2 1 8 9 16 33");
    CORRADE_COMPARE(c.value<Quaternion>("overflow"), (Quaternion{{2.0f, 1.0f, 8.0f}, 9.0f}));
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::QuaternionTest)
