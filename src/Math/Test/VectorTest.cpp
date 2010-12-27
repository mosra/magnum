/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

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

#include <QtTest/QTest>

#include "Vector.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::VectorTest)

using namespace std;

namespace Magnum { namespace Math { namespace Test {

typedef Vector<float, 4> Vector4;

void VectorTest::construct() {
    float zero[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    QVERIFY(Vector4() == Vector4(zero));
}

void VectorTest::data() {
    Vector4 v;
    v.set(2, 1.0f);
    v.add(2, 0.5f);

    v.set(0, 1.0f);

    float data[] = { 1.0f, 0.0f, 1.5f, 0.0f };
    QVERIFY(v == Vector4(data));
}

void VectorTest::bracketOperator() {
    Vector4 v1, v2;
    v1.set(0, 1.0f);
    v1.set(1, v1.at(0));
    v1.set(3, 0.5f);
    v1.add(3, 2.5f);

    v2[0] = 1.0f;
    v2[1] = v2[0];
    v2[3] = 0.5f;
    v2[3] += 2.5f;

    QVERIFY(v1 == v2);
}

void VectorTest::copy() {
    Vector4 v1;

    v1.set(3, 1.0f);

    Vector4 v2(v1);
    Vector4 v3;
    v3.set(0, 0.0f); /* this line is here so it's not optimized to Vector4 v3(v1) */
    v3 = v1;

    /* Change original */
    v1.set(2, 1.0f);

    /* Verify the copy is the same as original original */
    Vector4 original;
    original.set(3, 1.0f);

    QVERIFY(v2 == original);
    QVERIFY(v3 == original);
}

void VectorTest::dot() {
    float first[] = { 1.0f, 0.5f, 0.75f, 1.5f };
    float second[] = { 2.0f, 4.0f, 1.0f, 7.0f };

    QVERIFY(Vector4(first)*Vector4(second) == 15.25f);
}

void VectorTest::multiplyDivide() {
    float vec[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    float multiplied[] = { -1.5f, -3.0f, -4.5f, -6.0f };

    QVERIFY(Vector4(vec)*-1.5f == Vector4(multiplied));
    QVERIFY(Vector4(multiplied)/-1.5f == Vector4(vec));
}

void VectorTest::addSubstract() {
    float a[] = { 0.5f, -7.5f, 9.0f, -11.0f };
    float b[] = { -0.5, 1.0f, 0.0f, 7.5f };
    float expected[] = { 0.0f, -6.5f, 9.0f, -3.5f };

    QVERIFY(Vector4(a)+Vector4(b) == Vector4(expected));
    QVERIFY(Vector4(expected)-Vector4(b) == Vector4(a));
}

void VectorTest::length() {
    float vec[] = { 1.0f, 2.0f, 3.0f, 4.0f };

    QCOMPARE(Vector4(vec).length(), 5.4772256f);
}

void VectorTest::normalized() {
    float vec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float normalized[] = { 0.5f, 0.5f, 0.5f, 0.5f };

    QVERIFY(Vector4(vec).normalized() == Vector4(normalized));
}

}}}
