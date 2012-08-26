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

#include "VectorTest.h"

#include <sstream>

#include "Vector.h"
#include "Math.h"

CORRADE_TEST_MAIN(Magnum::Math::Test::VectorTest)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Vector<4, float> Vector4;
typedef Vector<4, int> Vector4i;
typedef Vector<3, float> Vector3;

VectorTest::VectorTest() {
    addTests(&VectorTest::construct,
             &VectorTest::constructFrom,
             &VectorTest::dot,
             &VectorTest::multiplyDivideComponentWise,
             &VectorTest::dotSelf,
             &VectorTest::length,
             &VectorTest::normalized,
             &VectorTest::sum,
             &VectorTest::product,
             &VectorTest::min,
             &VectorTest::max,
             &VectorTest::angle,
             &VectorTest::debug,
             &VectorTest::configuration);
}

void VectorTest::construct() {
    CORRADE_COMPARE(Vector4(), Vector4(0.0f, 0.0f, 0.0f, 0.0f));

    float data[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    CORRADE_COMPARE(Vector4::from(data), Vector4(1.0f, 2.0f, 3.0f, 4.0f));
}

void VectorTest::constructFrom() {
    Vector4 floatingPoint(1.3f, 2.7f, -15.0f, 7.0f);
    Vector4 floatingPointRounded(1.0f, 2.0f, -15.0f, 7.0f);
    Vector4i integral(1, 2, -15, 7);

    CORRADE_COMPARE(Vector4i::from(floatingPoint), integral);
    CORRADE_COMPARE(Vector4::from(integral), floatingPointRounded);
}

void VectorTest::dot() {
    CORRADE_COMPARE(Vector4::dot({1.0f, 0.5f, 0.75f, 1.5f}, {2.0f, 4.0f, 1.0f, 7.0f}), 15.25f);
}

void VectorTest::multiplyDivideComponentWise() {
    Vector4 vec(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4 multiplier(7.0f, -4.0f, -1.5f, 1.0f);
    Vector4 multiplied(7.0f, -8.0f, -4.5f, 4.0f);

    CORRADE_COMPARE(vec*multiplier, multiplied);
    CORRADE_COMPARE(multiplied/multiplier, vec);
}

void VectorTest::dotSelf() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).dot(), 30.0f);
}

void VectorTest::length() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).length(), 5.4772256f);
}

void VectorTest::normalized() {
    CORRADE_COMPARE(Vector4(1.0f, 1.0f, 1.0f, 1.0f).normalized(), Vector4(0.5f, 0.5f, 0.5f, 0.5f));
}

void VectorTest::sum() {
    CORRADE_COMPARE(Vector3(1.0f, 2.0f, 4.0f).sum(), 7.0f);
}

void VectorTest::product() {
    CORRADE_COMPARE(Vector3(1.0f, 2.0f, 3.0f).product(), 6.0f);
}

void VectorTest::min() {
    CORRADE_COMPARE(Vector3(1.0f, -2.0f, 3.0f).min(), -2.0f);
}

void VectorTest::max() {
    CORRADE_COMPARE(Vector3(1.0f, -2.0f, 3.0f).max(), 3.0f);
}

void VectorTest::angle() {
    ostringstream o;
    Error::setOutput(&o);
    /* Both vectors must be normalized, otherwise NaN is returned */
    CORRADE_COMPARE(Vector3::angle(Vector3(2.0f, 3.0f, 4.0f).normalized(), {1.0f, -2.0f, 3.0f}), numeric_limits<Vector3::Type>::quiet_NaN());
    CORRADE_COMPARE(o.str(), "Math::Vector::angle(): vectors must be normalized!\n");
    CORRADE_COMPARE(Vector3::angle({2.0f, 3.0f, 4.0f}, Vector3(1.0f, -2.0f, 3.0f).normalized()), numeric_limits<Vector3::Type>::quiet_NaN());

    CORRADE_COMPARE(Vector3::angle(Vector3(2.0f, 3.0f, 4.0f).normalized(), Vector3(1.0f, -2.0f, 3.0f).normalized()), rad(1.162514f));
}

void VectorTest::debug() {
    ostringstream o;
    Debug(&o) << Vector4(0.5f, 15.0f, 1.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1, 1)\n");

    o.str("");
    Debug(&o) << "a" << Vector4() << "b" << Vector4();
    CORRADE_COMPARE(o.str(), "a Vector(0, 0, 0, 0) b Vector(0, 0, 0, 0)\n");
}

void VectorTest::configuration() {
    Vector4 vec(3.0f, 3.125f, 9.0f, 9.55f);
    string value("3 3.125 9 9.55");
    CORRADE_COMPARE(ConfigurationValue<Vector4>::toString(vec), value);
    CORRADE_COMPARE(ConfigurationValue<Vector4>::fromString(value), vec);
}

}}}
