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
#include <QtTest/QTest>

#include "Vector.h"
#include "Math.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::VectorTest)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Vector<4, float> Vector4;
typedef Vector<3, float> Vector3;

void VectorTest::construct() {
    QVERIFY((Vector4() == Vector4(0.0f, 0.0f, 0.0f, 0.0f)));

    float data[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    QVERIFY((Vector4::from(data) == Vector4(1.0f, 2.0f, 3.0f, 4.0f)));
}

void VectorTest::data() {
    Vector4 v;
    v[2] = 1.5f;

    v[0] = 1.0f;

    QVERIFY(v == Vector4(1.0f, 0.0f, 1.5f, 0.0f));
}

void VectorTest::copy() {
    Vector4 v1;

    v1[3] = 1.0f;

    Vector4 v2(v1);
    Vector4 v3;
    v3[0] = 0.0f; /* this line is here so it's not optimized to Vector4 v3(v1) */
    v3 = v1;

    /* Change original */
    v1[2] = 1.0f;

    /* Verify the copy is the same as original original */
    Vector4 original;
    original[3] = 1.0f;

    QVERIFY(v2 == original);
    QVERIFY(v3 == original);
}

void VectorTest::dot() {
    QVERIFY(Vector4::dot({1.0f, 0.5f, 0.75f, 1.5f}, {2.0f, 4.0f, 1.0f, 7.0f}) == 15.25f);
}

void VectorTest::multiplyDivide() {
    Vector4 vec(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4 multiplied(-1.5f, -3.0f, -4.5f, -6.0f);

    QVERIFY(vec*-1.5f == multiplied);
    QVERIFY(multiplied/-1.5f == vec);
}

void VectorTest::addSubstract() {
    Vector4 a(0.5f, -7.5f, 9.0f, -11.0f);
    Vector4 b(-0.5, 1.0f, 0.0f, 7.5f);
    Vector4 expected(0.0f, -6.5f, 9.0f, -3.5f);

    QVERIFY(a + b == expected);
    QVERIFY(expected - b == a);
}

void VectorTest::length() {
    QCOMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).length(), 5.4772256f);
}

void VectorTest::lengthSquared() {
    QCOMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).lengthSquared(), 30.0f);
}

void VectorTest::normalized() {
    QVERIFY(Vector4(1.0f, 1.0f, 1.0f, 1.0f).normalized() == Vector4(0.5f, 0.5f, 0.5f, 0.5f));
}

void VectorTest::product() {
    QCOMPARE(Vector3(1.0f, 2.0f, 3.0f).product(), 6.0f);
}

void VectorTest::angle() {
    QCOMPARE(Vector3::angle({2.0f, 3.0f, 4.0f}, {1.0f, -2.0f, 3.0f}), rad(1.16251f));
}

void VectorTest::negative() {
    QVERIFY(-Vector4(1.0f, -3.0f, 5.0f, -10.0f) == Vector4(-1.0f, 3.0f, -5.0f, 10.0f));
}

void VectorTest::debug() {
    ostringstream o;
    Debug(&o) << Vector4(0.5f, 15.0f, 1.0f, 1.0f);
    QCOMPARE(QString::fromStdString(o.str()), QString("Vector(0.5, 15, 1, 1)\n"));

    o.str("");
    Debug(&o) << "a" << Vector4() << "b" << Vector4();
    QCOMPARE(QString::fromStdString(o.str()), QString("a Vector(0, 0, 0, 0) b Vector(0, 0, 0, 0)\n"));
}

}}}
