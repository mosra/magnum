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

#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

namespace Magnum { namespace Math { namespace Test {

class QuaternionTest: public Corrade::TestSuite::Tester {
    public:
        explicit QuaternionTest();

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

        void rotation();
        void angle();
        void matrix();
        void lerp();
        void slerp();
        void rotateVector();
        void rotateVectorNormalized();

        void debug();
};

typedef Math::Deg<float> Deg;
typedef Math::Rad<float> Rad;
typedef Math::Matrix<3, float> Matrix3;
typedef Math::Matrix4<float> Matrix4;
typedef Math::Quaternion<float> Quaternion;
typedef Math::Vector3<float> Vector3;
typedef Math::Vector4<float> Vector4;

QuaternionTest::QuaternionTest() {
    addTests(&QuaternionTest::construct,
             &QuaternionTest::constructDefault,
             &QuaternionTest::constructFromVector,
             &QuaternionTest::compare,

             &QuaternionTest::constExpressions,

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
             &QuaternionTest::rotateVector,
             &QuaternionTest::rotateVectorNormalized,

             &QuaternionTest::debug);
}

void QuaternionTest::construct() {
    Quaternion q({1.0f, 2.0f, 3.0f}, -4.0f);
    CORRADE_COMPARE(q.vector(), Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(q.scalar(), -4.0f);
}

void QuaternionTest::constructDefault() {
    CORRADE_COMPARE(Quaternion(), Quaternion({0.0f, 0.0f, 0.0f}, 1.0f));
}

void QuaternionTest::constructFromVector() {
    CORRADE_COMPARE(Quaternion({1.0f, 2.0f, 3.0f}), Quaternion({1.0f, 2.0f, 3.0f}, 0.0f));
}

void QuaternionTest::compare() {
    CORRADE_VERIFY(Quaternion({1.0f+MathTypeTraits<float>::epsilon()/2, 2.0f, 3.0f}, -4.0f) == Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    CORRADE_VERIFY(Quaternion({1.0f+MathTypeTraits<float>::epsilon()*2, 2.0f, 3.0f}, -4.0f) != Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    CORRADE_VERIFY(Quaternion({4.0f, 2.0f, 3.0f}, -1.0f+MathTypeTraits<float>::epsilon()/2) == Quaternion({4.0f, 2.0f, 3.0f}, -1.0f));
    CORRADE_VERIFY(Quaternion({4.0f, 2.0f, 3.0f}, -1.0f+MathTypeTraits<float>::epsilon()*2) != Quaternion({4.0f, 2.0f, 3.0f}, -1.0f));
}

void QuaternionTest::constExpressions() {
    /* Default constructor */
    constexpr Quaternion a;
    CORRADE_COMPARE(a, Quaternion({0.0f, 0.0f, 0.0f}, 1.0f));

    /* Value constructor */
    constexpr Quaternion b({1.0f, -3.0f, 7.0f}, 2.5f);
    CORRADE_COMPARE(b, Quaternion({1.0f, -3.0f, 7.0f}, 2.5f));

    /* Construct from vector */
    constexpr Quaternion c({2.0f, -3.0f, 1.5f});
    CORRADE_COMPARE(c, Quaternion({2.0f, -3.0f, 1.5f}, 0.0f));

    /* Copy constructor */
    constexpr Quaternion d(b);
    CORRADE_COMPARE(d, Quaternion({1.0f, -3.0f, 7.0f}, 2.5f));

    /* Data access */
    constexpr Vector3 e = b.vector();
    constexpr float f = b.scalar();
    CORRADE_COMPARE(e, Vector3(1.0f, -3.0f, 7.0f));
    CORRADE_COMPARE(f, 2.5f);
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
    Corrade::Utility::Error::setOutput(&o);
    Quaternion notInverted = a.invertedNormalized();
    CORRADE_COMPARE(notInverted.vector(), Vector3());
    CORRADE_COMPARE(notInverted.scalar(), std::numeric_limits<float>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::invertedNormalized(): quaternion must be normalized\n");

    Quaternion aNormalized = a.normalized();
    Quaternion inverted = aNormalized.invertedNormalized();
    CORRADE_COMPARE(aNormalized*inverted, Quaternion());
    CORRADE_COMPARE(inverted*aNormalized.normalized(), Quaternion());
    CORRADE_COMPARE(inverted, Quaternion({-1.0f, -3.0f, 2.0f}, -4.0f)/std::sqrt(30.0f));
}

void QuaternionTest::rotation() {
    std::ostringstream o;
    Error::setOutput(&o);

    Vector3 axis(1.0f/Constants<float>::sqrt3());

    CORRADE_COMPARE(Quaternion::rotation(Deg(-74.0f), {-1.0f, 2.0f, 2.0f}), Quaternion());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::rotation(): axis must be normalized\n");

    Quaternion q = Quaternion::rotation(Deg(120.0f), axis);
    CORRADE_COMPARE(q, Quaternion(Vector3(0.5f, 0.5f, 0.5f), 0.5f));
    CORRADE_COMPARE_AS(q.rotationAngle(), Deg(120.0f), Deg);
    CORRADE_COMPARE(q.rotationAxis(), axis);
    CORRADE_COMPARE(q.rotationAxis().length(), 1.0f);

    /* Verify negative angle */
    Quaternion q2 = Quaternion::rotation(Deg(-120.0f), axis);
    CORRADE_COMPARE(q2, Quaternion(Vector3(-0.5f, -0.5f, -0.5f), 0.5f));
    CORRADE_COMPARE_AS(q2.rotationAngle(), Deg(120.0f), Deg);
    CORRADE_COMPARE(q2.rotationAxis(), -axis);

    /* Default-constructed quaternion has zero angle and NaN axis */
    CORRADE_COMPARE_AS(Quaternion().rotationAngle(), Deg(0.0f), Deg);
    CORRADE_VERIFY(Quaternion().rotationAxis() != Quaternion().rotationAxis());
}

void QuaternionTest::angle() {
    std::ostringstream o;
    Corrade::Utility::Error::setOutput(&o);
    auto angle = Quaternion::angle(Quaternion({1.0f, 2.0f, -3.0f}, -4.0f).normalized(), {{4.0f, -3.0f, 2.0f}, -1.0f});
    CORRADE_VERIFY(angle != angle);
    CORRADE_COMPARE(o.str(), "Math::Quaternion::angle(): quaternions must be normalized\n");

    o.str("");
    angle = Quaternion::angle({{1.0f, 2.0f, -3.0f}, -4.0f}, Quaternion({4.0f, -3.0f, 2.0f}, -1.0f).normalized());
    CORRADE_VERIFY(angle != angle);
    CORRADE_COMPARE(o.str(), "Math::Quaternion::angle(): quaternions must be normalized\n");

    CORRADE_COMPARE(Quaternion::angle(Quaternion({1.0f, 2.0f, -3.0f}, -4.0f).normalized(),
                                      Quaternion({4.0f, -3.0f, 2.0f}, -1.0f).normalized()),
                    Rad(1.704528f));
}

void QuaternionTest::matrix() {
    Quaternion q = Quaternion::rotation(Deg(37.0f), Vector3(1.0f/Constants<float>::sqrt3()));
    Matrix3 m = Matrix4::rotation(Deg(37.0f), Vector3(1.0f/Constants<float>::sqrt3())).rotationScaling();

    /* Verify that negated quaternion gives the same rotation */
    CORRADE_COMPARE(q.matrix(), m);
    CORRADE_COMPARE((-q).matrix(), m);
}

void QuaternionTest::lerp() {
    Quaternion a = Quaternion::rotation(Deg(15.0f), Vector3(1.0f/Constants<float>::sqrt3()));
    Quaternion b = Quaternion::rotation(Deg(23.0f), Vector3::xAxis());

    std::ostringstream o;
    Corrade::Utility::Error::setOutput(&o);

    Quaternion notLerpA = Quaternion::lerp(a*3.0f, b, 0.35f);
    CORRADE_COMPARE(notLerpA.vector(), Vector3());
    CORRADE_COMPARE(notLerpA.scalar(), std::numeric_limits<float>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::lerp(): quaternions must be normalized\n");

    o.str("");
    Quaternion notLerpB = Quaternion::lerp(a, b*-3.0f, 0.35f);
    CORRADE_COMPARE(notLerpB.vector(), Vector3());
    CORRADE_COMPARE(notLerpB.scalar(), std::numeric_limits<float>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::lerp(): quaternions must be normalized\n");

    Quaternion lerp = Quaternion::lerp(a, b, 0.35f);
    CORRADE_COMPARE(lerp, Quaternion({0.119127f, 0.049134f, 0.049134f}, 0.990445f));
}

void QuaternionTest::slerp() {
    Quaternion a = Quaternion::rotation(Deg(15.0f), Vector3(1.0f/Constants<float>::sqrt3()));
    Quaternion b = Quaternion::rotation(Deg(23.0f), Vector3::xAxis());

    std::ostringstream o;
    Corrade::Utility::Error::setOutput(&o);

    Quaternion notSlerpA = Quaternion::slerp(a*3.0f, b, 0.35f);
    CORRADE_COMPARE(notSlerpA.vector(), Vector3());
    CORRADE_COMPARE(notSlerpA.scalar(), std::numeric_limits<float>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::slerp(): quaternions must be normalized\n");

    o.str("");
    Quaternion notSlerpB = Quaternion::slerp(a, b*-3.0f, 0.35f);
    CORRADE_COMPARE(notSlerpB.vector(), Vector3());
    CORRADE_COMPARE(notSlerpB.scalar(), std::numeric_limits<float>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::slerp(): quaternions must be normalized\n");

    Quaternion slerp = Quaternion::slerp(a, b, 0.35f);
    CORRADE_COMPARE(slerp, Quaternion({0.119165f, 0.0491109f, 0.0491109f}, 0.990442f));
}

void QuaternionTest::rotateVector() {
    Quaternion a = Quaternion::rotation(Deg(23.0f), Vector3::xAxis());
    Matrix4 m = Matrix4::rotationX(Deg(23.0f));
    Vector3 v(5.0f, -3.6f, 0.7f);

    Vector3 rotated = a.rotateVector(v);
    CORRADE_COMPARE(rotated, m.transformVector(v));
    CORRADE_COMPARE(rotated, Vector3(5.0f, -3.58733f, -0.762279f));
}

void QuaternionTest::rotateVectorNormalized() {
    Quaternion a = Quaternion::rotation(Deg(23.0f), Vector3::xAxis());
    Matrix4 m = Matrix4::rotationX(Deg(23.0f));
    Vector3 v(5.0f, -3.6f, 0.7f);

    std::ostringstream o;
    Corrade::Utility::Error::setOutput(&o);
    Vector3 notRotated = (a*2).rotateVectorNormalized(v);
    CORRADE_VERIFY(notRotated != notRotated);
    CORRADE_COMPARE(o.str(), "Math::Quaternion::rotateVectorNormalized(): quaternion must be normalized\n");

    Vector3 rotated = a.rotateVectorNormalized(v);
    CORRADE_COMPARE(rotated, m.transformVector(v));
    CORRADE_COMPARE(rotated, a.rotateVector(v));
}

void QuaternionTest::debug() {
    std::ostringstream o;

    Debug(&o) << Quaternion({1.0f, 2.0f, 3.0f}, -4.0f);
    CORRADE_COMPARE(o.str(), "Quaternion({1, 2, 3}, -4)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::QuaternionTest)
