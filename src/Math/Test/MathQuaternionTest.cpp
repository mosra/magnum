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

#include "Math/Quaternion.h"

namespace Magnum { namespace Math { namespace Test {

class QuaternionTest: public Corrade::TestSuite::Tester {
    public:
        explicit QuaternionTest();

        void construct();
        void multiplyDivideScalar();
        void multiply();
        void length();
        void normalized();
        void conjugated();
        void inverted();
        void invertedNormalized();

        void debug();
};

typedef Math::Quaternion<float> Quaternion;
typedef Math::Vector3<float> Vector3;

QuaternionTest::QuaternionTest() {
    addTests(&QuaternionTest::construct,
             &QuaternionTest::multiplyDivideScalar,
             &QuaternionTest::multiply,
             &QuaternionTest::length,
             &QuaternionTest::normalized,
             &QuaternionTest::conjugated,
             &QuaternionTest::inverted,
             &QuaternionTest::invertedNormalized,
             &QuaternionTest::debug);
}

void QuaternionTest::construct() {
    Quaternion q({1.0f, 2.0f, 3.0f}, -4.0f);
    CORRADE_COMPARE(q.vector(), Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(q.scalar(), -4.0f);

    CORRADE_COMPARE(Quaternion(), Quaternion({0.0f, 0.0f, 0.0f}, {1.0f}));
}

void QuaternionTest::multiplyDivideScalar() {
    Quaternion a({1.0f, 3.0f, -2.0f}, -4.0f);
    Quaternion b({-1.5f, -4.5f, 3.0f}, 6.0f);

    CORRADE_COMPARE(a*-1.5f, b);
    CORRADE_COMPARE(b/-1.5f, a);
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

void QuaternionTest::debug() {
    std::ostringstream o;

    Corrade::Utility::Debug(&o) << Quaternion({1.0f, 2.0f, 3.0f}, -4.0f);
    CORRADE_COMPARE(o.str(), "Quaternion({1, 2, 3}, -4)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::QuaternionTest)
