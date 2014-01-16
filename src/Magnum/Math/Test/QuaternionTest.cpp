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
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Quaternion.h"

namespace Magnum { namespace Math { namespace Test {

class QuaternionTest: public Corrade::TestSuite::Tester {
    public:
        explicit QuaternionTest();

        void construct();
        void constructDefault();
        void constructFromVector();
        void constructCopy();

        void compare();
        void isNormalized();

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

        void rotation();
        void angle();
        void matrix();
        void lerp();
        void slerp();
        void transformVector();
        void transformVectorNormalized();

        void debug();
};

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Matrix<3, Float> Matrix3x3;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::Quaternion<Float> Quaternion;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector4<Float> Vector4;

QuaternionTest::QuaternionTest() {
    addTests<QuaternionTest>({&QuaternionTest::construct,
              &QuaternionTest::constructDefault,
              &QuaternionTest::constructFromVector,
              &QuaternionTest::constructCopy,

              &QuaternionTest::compare,
              &QuaternionTest::isNormalized,

              &QuaternionTest::addSubtract,
              &QuaternionTest::negated,
              &QuaternionTest::multiplyDivideScalar,
              &QuaternionTest::multiply,

              &QuaternionTest::dot,
              &QuaternionTest::dotSelf,
              &QuaternionTest::length,
              &QuaternionTest::normalized,

              &QuaternionTest::conjugated,
              &QuaternionTest::inverted,
              &QuaternionTest::invertedNormalized,

              &QuaternionTest::rotation,
              &QuaternionTest::angle,
              &QuaternionTest::matrix,
              &QuaternionTest::lerp,
              &QuaternionTest::slerp,
              &QuaternionTest::transformVector,
              &QuaternionTest::transformVectorNormalized,

              &QuaternionTest::debug});
}

void QuaternionTest::construct() {
    constexpr Quaternion a({1.0f, 2.0f, 3.0f}, -4.0f);
    CORRADE_COMPARE(a, Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));

    constexpr Vector3 b = a.vector();
    constexpr Float c = a.scalar();
    CORRADE_COMPARE(b, Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(c, -4.0f);
}

void QuaternionTest::constructDefault() {
    constexpr Quaternion a;
    CORRADE_COMPARE(a, Quaternion({0.0f, 0.0f, 0.0f}, 1.0f));
    CORRADE_COMPARE(a.length(), 1.0f);
}

void QuaternionTest::constructFromVector() {
    constexpr Quaternion a(Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(a, Quaternion({1.0f, 2.0f, 3.0f}, 0.0f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector3, Quaternion>::value));
}

void QuaternionTest::constructCopy() {
    constexpr Quaternion a({1.0f, -3.0f, 7.0f}, 2.5f);
    constexpr Quaternion b(a);
    CORRADE_COMPARE(b, Quaternion({1.0f, -3.0f, 7.0f}, 2.5f));
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

    CORRADE_COMPARE(Quaternion::dot(a, b), -50.0f);
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
    Error::setOutput(&o);
    Quaternion notInverted = a.invertedNormalized();
    CORRADE_COMPARE(notInverted.vector(), Vector3());
    CORRADE_COMPARE(notInverted.scalar(), std::numeric_limits<Float>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::invertedNormalized(): quaternion must be normalized\n");

    Quaternion aNormalized = a.normalized();
    Quaternion inverted = aNormalized.invertedNormalized();
    CORRADE_COMPARE(aNormalized*inverted, Quaternion());
    CORRADE_COMPARE(inverted*aNormalized, Quaternion());
    CORRADE_COMPARE(inverted, Quaternion({-1.0f, -3.0f, 2.0f}, -4.0f)/std::sqrt(30.0f));
}

void QuaternionTest::rotation() {
    std::ostringstream o;
    Error::setOutput(&o);

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
    Error::setOutput(&o);
    auto angle = Quaternion::angle(Quaternion({1.0f, 2.0f, -3.0f}, -4.0f).normalized(), {{4.0f, -3.0f, 2.0f}, -1.0f});
    CORRADE_VERIFY(angle != angle);
    CORRADE_COMPARE(o.str(), "Math::Quaternion::angle(): quaternions must be normalized\n");

    o.str({});
    angle = Quaternion::angle({{1.0f, 2.0f, -3.0f}, -4.0f}, Quaternion({4.0f, -3.0f, 2.0f}, -1.0f).normalized());
    CORRADE_VERIFY(angle != angle);
    CORRADE_COMPARE(o.str(), "Math::Quaternion::angle(): quaternions must be normalized\n");

    /* Verify also that the angle is the same as angle between 4D vectors */
    angle = Quaternion::angle(Quaternion({1.0f, 2.0f, -3.0f}, -4.0f).normalized(),
                              Quaternion({4.0f, -3.0f, 2.0f}, -1.0f).normalized());
    CORRADE_COMPARE(angle, Vector4::angle(Vector4(1.0f, 2.0f, -3.0f, -4.0f).normalized(),
                                          Vector4(4.0f, -3.0f, 2.0f, -1.0f).normalized()));
    CORRADE_COMPARE(angle, Rad(1.704528f));
}

void QuaternionTest::matrix() {
    Vector3 axis = Vector3(1.0f, -3.0f, 5.0f).normalized();

    Quaternion q = Quaternion::rotation(Deg(37.0f), axis);
    Matrix3x3 m = Matrix4::rotation(Deg(37.0f), axis).rotationScaling();

    /* Verify that negated quaternion gives the same rotation */
    CORRADE_COMPARE(q.toMatrix(), m);
    CORRADE_COMPARE((-q).toMatrix(), m);

    std::ostringstream o;
    Error::setOutput(&o);
    Quaternion::fromMatrix(m*2);
    CORRADE_COMPARE(o.str(), "Math::Quaternion::fromMatrix(): the matrix is not orthogonal\n");

    /* Trace > 0 */
    CORRADE_VERIFY(m.trace() > 0.0f);
    CORRADE_COMPARE(Quaternion::fromMatrix(m), q);

    /* Trace < 0 */
    Matrix3x3 m2 = Matrix4::rotation(Deg(130.0f), axis).rotationScaling();
    Quaternion q2 = Quaternion::rotation(Deg(130.0f), axis);
    CORRADE_VERIFY(m2.trace() < 0.0f);
    CORRADE_COMPARE(Quaternion::fromMatrix(m2), q2);
}

void QuaternionTest::lerp() {
    Quaternion a = Quaternion::rotation(Deg(15.0f), Vector3(1.0f/Constants<Float>::sqrt3()));
    Quaternion b = Quaternion::rotation(Deg(23.0f), Vector3::xAxis());

    std::ostringstream o;
    Corrade::Utility::Error::setOutput(&o);

    Quaternion notLerpA = Quaternion::lerp(a*3.0f, b, 0.35f);
    CORRADE_COMPARE(notLerpA.vector(), Vector3());
    CORRADE_COMPARE(notLerpA.scalar(), std::numeric_limits<Float>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::lerp(): quaternions must be normalized\n");

    o.str({});
    Quaternion notLerpB = Quaternion::lerp(a, b*-3.0f, 0.35f);
    CORRADE_COMPARE(notLerpB.vector(), Vector3());
    CORRADE_COMPARE(notLerpB.scalar(), std::numeric_limits<Float>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::lerp(): quaternions must be normalized\n");

    Quaternion lerp = Quaternion::lerp(a, b, 0.35f);
    CORRADE_COMPARE(lerp, Quaternion({0.119127f, 0.049134f, 0.049134f}, 0.990445f));
}

void QuaternionTest::slerp() {
    Quaternion a = Quaternion::rotation(Deg(15.0f), Vector3(1.0f/Constants<Float>::sqrt3()));
    Quaternion b = Quaternion::rotation(Deg(23.0f), Vector3::xAxis());

    std::ostringstream o;
    Corrade::Utility::Error::setOutput(&o);

    Quaternion notSlerpA = Quaternion::slerp(a*3.0f, b, 0.35f);
    CORRADE_COMPARE(notSlerpA.vector(), Vector3());
    CORRADE_COMPARE(notSlerpA.scalar(), std::numeric_limits<Float>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::slerp(): quaternions must be normalized\n");

    o.str({});
    Quaternion notSlerpB = Quaternion::slerp(a, b*-3.0f, 0.35f);
    CORRADE_COMPARE(notSlerpB.vector(), Vector3());
    CORRADE_COMPARE(notSlerpB.scalar(), std::numeric_limits<Float>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::slerp(): quaternions must be normalized\n");

    Quaternion slerp = Quaternion::slerp(a, b, 0.35f);
    CORRADE_COMPARE(slerp, Quaternion({0.119165f, 0.0491109f, 0.0491109f}, 0.990442f));
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
    Error::setOutput(&o);
    Vector3 notRotated = (a*2).transformVectorNormalized(v);
    CORRADE_VERIFY(notRotated != notRotated);
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

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::QuaternionTest)
