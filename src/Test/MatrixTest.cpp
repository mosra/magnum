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

#include "MatrixTest.h"

#include <QtTest/QTest>

#include "Matrix.h"

QTEST_APPLESS_MAIN(Magnum::Test::MatrixTest)

namespace Magnum { namespace Test {

typedef Matrix<float, 4> Matrix4;

void MatrixTest::constructIdentity() {
    Matrix4 identity;

    float identityExpected[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    QVERIFY(identity == Matrix4(identityExpected));
}

void MatrixTest::constructZero() {
    Matrix4 zero(false);

    float zeroExpected[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };

    QVERIFY(zero == Matrix4(zeroExpected));
}

void MatrixTest::data() {
    Matrix4 m(false);

    m.set(2, 1, 1.0f);

    m.set(1, 2, 1.0f);
    m.add(1, 2, 0.5f);

    QVERIFY(m.at(2, 1) == 1.0f);

    float expected[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.5f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };

    QVERIFY(m == Matrix4(expected));
}

void MatrixTest::copy() {
    Matrix4 m1(false);

    m1.set(2, 3, 1.0f);

    /* Copy */
    Matrix4 m2(m1);
    Matrix4 m3;
    m3.set(0, 0, 1.0f); /* this line is here so it's not optimized to Matrix4 m3(m1) */
    m3 = m1;

    /* Change original */
    m1.set(3, 2, 1.0f);

    /* Verify the copy is the same as original */
    Matrix4 original(false);
    original.set(2, 3, 1.0f);

    QVERIFY(m2 == original);
    QVERIFY(m3 == original);
}

void MatrixTest::multiplyIdentity() {
    float values[] = {
        0.0f,   1.0f,   2.0f,   3.0f,
        4.0f,   5.0f,   6.0f,   7.0f,
        8.0f,   9.0f,   10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    };

    QVERIFY(Matrix4()*Matrix4(values) == Matrix4(values));
    QVERIFY(Matrix4(values)*Matrix4() == Matrix4(values));
}

void MatrixTest::multiply() {
    int left[] = {
         -3,  -3,  -1,   3,  -5,
         -1,  -3,  -5,   2,   3,
         -1,  -4,   3,  -1,  -2,
         -5,  -5,  -1,  -4,  -1,
          1,   3,  -3,  -4,  -1
    };

    int right[] = {
          0,   5,   3,   4,   4,
          5,   5,   0,   0,  -2,
          3,   2,  -4,  -3,   0,
         -3,   0,  -1,   2,  -1,
          0,  -1,  -4,   4,   3
    };

    int expected[] = {
        -24, -35, -32, -25,   1,
        -22, -36, -24,  33,  -8,
          8,  16, -22,  29,   2,
         -1,   0,   1, -12,  16,
        -12,   8, -20, -26,  -2
    };

    bool is = (Matrix<int, 5>(left)*Matrix<int, 5>(right) == Matrix<int, 5>(expected));

    QVERIFY(is);
}

}}
