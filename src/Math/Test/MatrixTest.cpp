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

#include "MatrixTest.h"

#include <sstream>
#include <QtTest/QTest>

#include "Matrix.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::MatrixTest)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Matrix<4, float> Matrix4;
typedef Matrix<3, float> Matrix3;
typedef Vector<4, float> Vector4;

void MatrixTest::construct() {
    float m[] = {
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f,
        9.0f, 4.0f, 5.0f, 9.0f
    };

    Matrix4 expected(
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f,
        9.0f, 4.0f, 5.0f, 9.0f
    );

    QVERIFY(Matrix4::from(m) == expected);
}

void MatrixTest::constructFromVectors() {
    Matrix4 actual = Matrix4::from(Vector4(1.0f, 2.0f, 3.0f, 4.0f),
                                   Vector4(5.0f, 6.0f, 7.0f, 8.0f),
                                   Vector4(9.0f, 10.0f, 11.0f, 12.0f),
                                   Vector4(13.0f, 14.0f, 15.0f, 16.0f));

    Matrix4 expected(1.0f, 2.0f, 3.0f, 4.0f,
                     5.0f, 6.0f, 7.0f, 8.0f,
                     9.0f, 10.0f, 11.0f, 12.0f,
                     13.0f, 14.0f, 15.0f, 16.0f);

    QVERIFY(actual == expected);
}

void MatrixTest::constructIdentity() {
    Matrix4 identity;
    Matrix4 identity2(Matrix4::Identity);
    Matrix4 identity3(Matrix4::Identity, 4.0f);

    Matrix4 identityExpected(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    Matrix4 identity3Expected(
        4.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 4.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 4.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 4.0f
    );

    QVERIFY(identity == identityExpected);
    QVERIFY(identity2 == identityExpected);
    QVERIFY(identity3 == identity3Expected);
}

void MatrixTest::constructZero() {
    Matrix4 zero(Matrix4::Zero);

    Matrix4 zeroExpected(
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    );

    QVERIFY(zero == zeroExpected);
}

void MatrixTest::data() {
    Matrix4 m(Matrix4::Zero);

    Vector4 vector(4.0f, 5.0f, 6.0f, 7.0f);

    m[3] = vector;
    m[2][1] = 1.0f;

    m[1][2] = 1.5f;

    QVERIFY(m[2][1] == 1.0f);
    QVERIFY(m[3] == vector);

    Matrix4 expected(
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.5f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        4.0f, 5.0f, 6.0f, 7.0f
    );

    QVERIFY(m == expected);
}

void MatrixTest::copy() {
    Matrix4 m1(Matrix4::Zero);

    m1[2][3] = 1.0f;

    /* Copy */
    Matrix4 m2(m1);
    Matrix4 m3;
    m3[0][0] = 1.0f; /* this line is here so it's not optimized to Matrix4 m3(m1) */
    m3 = m1;

    /* Change original */
    m1[3][2] = 1.0f;

    /* Verify the copy is the same as original */
    Matrix4 original(Matrix4::Zero);
    original[2][3] = 1.0f;

    QVERIFY(m2 == original);
    QVERIFY(m3 == original);
}

void MatrixTest::multiplyIdentity() {
    Matrix4 values(
        0.0f,   1.0f,   2.0f,   3.0f,
        4.0f,   5.0f,   6.0f,   7.0f,
        8.0f,   9.0f,   10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );

    QVERIFY(Matrix4()*values == values);
    QVERIFY(values*Matrix4() == values);
}

void MatrixTest::multiply() {
    Matrix<5, int> left(
         -3,  -3,  -1,   3,  -5,
         -1,  -3,  -5,   2,   3,
         -1,  -4,   3,  -1,  -2,
         -5,  -5,  -1,  -4,  -1,
          1,   3,  -3,  -4,  -1
    );

    Matrix<5, int> right(
          0,   5,   3,   4,   4,
          5,   5,   0,   0,  -2,
          3,   2,  -4,  -3,   0,
         -3,   0,  -1,   2,  -1,
          0,  -1,  -4,   4,   3
    );

    Matrix<5, int> expected(
        -24, -35, -32, -25,   1,
        -22, -36, -24,  33,  -8,
          8,  16, -22,  29,   2,
         -1,   0,   1, -12,  16,
        -12,   8, -20, -26,  -2
    );

    QVERIFY((left *= right) == expected);
}

void MatrixTest::multiplyVector() {
    Matrix<5, int> matrix(
         -3,  -3,  -1,   3,  -5,
         -1,  -3,  -5,   2,   3,
         -1,  -4,   3,  -1,  -2,
         -5,  -5,  -1,  -4,  -1,
          1,   3,  -3,  -4,  -1
    );

    bool is = (matrix*Vector<5, int>(0, 5, 3, 4, 4) == Vector<5, int>(-24, -35, -32, -25, 1));

    QVERIFY(is);
}

void MatrixTest::transposed() {
    Matrix4 original(
        0.0f,   1.0f,   2.0f,   3.0f,
        4.0f,   5.0f,   6.0f,   7.0f,
        8.0f,   9.0f,   10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );

    Matrix4 transposed(
        0.0f,   4.0f,   8.0f,   12.0f,
        1.0f,   5.0f,   9.0f,   13.0f,
        2.0f,   6.0f,   10.0f,  14.0f,
        3.0f,   7.0f,   11.0f,  15.0f
    );

    QVERIFY(original.transposed() == transposed);
}

void MatrixTest::ij() {
    Matrix4 original(
        0.0f,   1.0f,   2.0f,   3.0f,
        4.0f,   5.0f,   6.0f,   7.0f,
        8.0f,   9.0f,   10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );

    Matrix3 skipped(
        0.0f,   1.0f,   3.0f,
        8.0f,   9.0f,   11.0f,
        12.0f,  13.0f,  15.0f
    );

    QVERIFY(original.ij(1, 2) == skipped);
}

void MatrixTest::determinant() {
    Matrix<5, int> m(
        1, 2, 2, 1, 0,
        2, 3, 2, 1, -2,
        1, 1, 1, 1, 0,
        2, 0, 0, 1, 2,
        3, 1, 0, 1, -2
    );

    QVERIFY(m.determinant() == -2);
}

void MatrixTest::inverted() {
    Matrix4 m(
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f,
        9.0f, 4.0f, 5.0f, 9.0f
    );

    Matrix4 inverse(
        -60/103.0f,      71/103.0f,     -4/103.0f,      3/103.0f,
        -66/103.0f,     109/103.0f,     -25/103.0f,     -7/103.0f,
        177/412.0f,     -97/206.0f,     53/412.0f,      -7/206.0f,
        259/412.0f,     -185/206.0f,    31/412.0f,      27/206.0f
    );

    Matrix4 _inverse = m.inverted();

    QVERIFY(_inverse == inverse);
    QVERIFY(_inverse*m == Matrix4());
}

void MatrixTest::debug() {
    Matrix4 m(
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f,
        9.0f, 4.0f, 5.0f, 9.0f
    );

    ostringstream o;
    Debug(&o) << m;
    QCOMPARE(QString::fromStdString(o.str()), QString("Matrix(3, 4, 7, 9,\n"
                                                      "       5, 4, -1, 4,\n"
                                                      "       8, 7, 8, 5,\n"
                                                      "       4, 3, 0, 9)\n"));

    o.str("");
    Debug(&o) << "a" << Matrix4() << "b" << Matrix4();
    QCOMPARE(QString::fromStdString(o.str()), QString("a Matrix(1, 0, 0, 0,\n"
                                                        "       0, 1, 0, 0,\n"
                                                        "       0, 0, 1, 0,\n"
                                                        "       0, 0, 0, 1) b Matrix(1, 0, 0, 0,\n"
                                                        "       0, 1, 0, 0,\n"
                                                        "       0, 0, 1, 0,\n"
                                                        "       0, 0, 0, 1)\n"));
}

}}}
