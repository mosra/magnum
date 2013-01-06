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

#include "Math/Constants.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

namespace Magnum { namespace Math { namespace Test {

class QuaternionTest: public Corrade::TestSuite::Tester {
    public:
        explicit QuaternionTest();

        void construct();
        void addSubtract();
        void negated();
        void multiplyDivideScalar();
        void multiply();
        void length();
        void normalized();
        void conjugated();
        void inverted();
        void invertedNormalized();
        void rotation();
        void matrix();
        void lerp();

        void debug();
};

typedef Math::Quaternion<float> Quaternion;
typedef Math::Vector3<float> Vector3;

QuaternionTest::QuaternionTest() {
    addTests(&QuaternionTest::construct,
             &QuaternionTest::addSubtract,
             &QuaternionTest::negated,
             &QuaternionTest::multiplyDivideScalar,
             &QuaternionTest::multiply,
             &QuaternionTest::length,
             &QuaternionTest::normalized,
             &QuaternionTest::conjugated,
             &QuaternionTest::inverted,
             &QuaternionTest::invertedNormalized,
             &QuaternionTest::rotation,
             &QuaternionTest::matrix,
             &QuaternionTest::lerp,
             &QuaternionTest::debug);
}

void QuaternionTest::construct() {
    Quaternion q({1.0f, 2.0f, 3.0f}, -4.0f);
    CORRADE_COMPARE(q.vector(), Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(q.scalar(), -4.0f);

    CORRADE_COMPARE(Quaternion(), Quaternion({0.0f, 0.0f, 0.0f}, {1.0f}));
}

void QuaternionTest::addSubtract() {
    Quaternion a({1.0f, 3.0f, -2.0f}, -4.0f);
    Quaternion b({-0.5f, 1.4f, 3.0f}, 12.0f);
    Quaternion c({0.5f, 4.4f, 1.0f}, 8.0f);

    CORRADE_COMPARE(a+b, c);
    CORRADE_COMPARE(c-b, a);
}

void QuaternionTest::negated() {
    CORRADE_COMPARE(-Quaternion({1.0f, 2.0f, -3.0f}, -4.0f), Quaternion({-1.0f, -2.0f, 3.0f}, 4.0f));
}

void QuaternionTest::multiplyDivideScalar() {
    Quaternion a({1.0f, 3.0f, -2.0f}, -4.0f);
    Quaternion b({-1.5f, -4.5f, 3.0f}, 6.0f);

    CORRADE_COMPARE(a*-1.5f, b);
    CORRADE_COMPARE(-1.5f*a, b);
    CORRADE_COMPARE(b/-1.5f, a);

    CORRADE_COMPARE(2.0f/a, Quaternion({2.0f, 0.666666f, -1.0f}, -0.5f));
}

void QuaternionTest::multiply() {
    CORRADE_COMPARE(Quaternion({-6.0f, -9.0f, 15.0f}, 0.5f)*Quaternion({2.0f, 3.0f, -5.0f}, 2.0f),
                    Quaternion({-11.0f, -16.5f, 27.5f}, 115.0f));
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
    CORRADE_COMPARE(Quaternion({1.0f, 3.0f, -2.0f}, -4.0f).conjugated(),
                    Quaternion({-1.0f, -3.0f, 2.0f}, -4.0f));
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
    float angle = deg(120.0f);
    Vector3 axis(1.0f/Constants<float>::sqrt3());
    Quaternion q = Quaternion::fromRotation(angle, axis);
    CORRADE_COMPARE(q, Quaternion(Vector3(0.5f, 0.5f, 0.5f), 0.5f));
    CORRADE_COMPARE(q.rotationAngle(), angle);
    CORRADE_COMPARE(q.rotationAxis(), axis);
    CORRADE_COMPARE(q.rotationAxis().length(), 1.0f);

    /* Verify negative angle */
    Quaternion q2 = Quaternion::fromRotation(deg(-120.0f), axis);
    CORRADE_COMPARE(q2, Quaternion(Vector3(-0.5f, -0.5f, -0.5f), 0.5f));
    CORRADE_COMPARE(q2.rotationAngle(), deg(120.0f));
    CORRADE_COMPARE(q2.rotationAxis(), -axis);
}

void QuaternionTest::matrix() {
    float angle = deg(37.0f);
    Vector3 axis(1.0f/Constants<float>::sqrt3());
    Quaternion q = Quaternion::fromRotation(angle, axis);
    Matrix<3, float> expected = Matrix4<float>::rotation(angle, axis).rotationScaling();
    CORRADE_COMPARE(q.matrix(), expected);

    /* Verify that negated quaternion gives the same rotation */
    CORRADE_COMPARE((-q).matrix(), expected);
}

void QuaternionTest::lerp() {
    Quaternion a = Quaternion::fromRotation(deg(15.0f), Vector3(1.0f/Constants<float>::sqrt3()));
    Quaternion b = Quaternion::fromRotation(deg(23.0f), Vector3::xAxis());

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

void QuaternionTest::debug() {
    std::ostringstream o;

    Corrade::Utility::Debug(&o) << Quaternion({1.0f, 2.0f, 3.0f}, -4.0f);
    CORRADE_COMPARE(o.str(), "Quaternion({1, 2, 3}, -4)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::QuaternionTest)
