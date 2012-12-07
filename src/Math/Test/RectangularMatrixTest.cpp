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

#include "RectangularMatrixTest.h"

#include <Utility/Configuration.h>

#include "RectangularMatrix.h"

CORRADE_TEST_MAIN(Magnum::Math::Test::RectangularMatrixTest)

using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef RectangularMatrix<4, 3, float> Matrix4x3;
typedef RectangularMatrix<3, 4, float> Matrix3x4;
typedef RectangularMatrix<2, 2, float> Matrix2;
typedef RectangularMatrix<2, 2, std::int32_t> Matrix2i;
typedef Vector<4, float> Vector4;

RectangularMatrixTest::RectangularMatrixTest() {
    addTests(&RectangularMatrixTest::construct,
             &RectangularMatrixTest::constructFromVectors,
             &RectangularMatrixTest::constructFrom,
             &RectangularMatrixTest::constructZero,
             &RectangularMatrixTest::data,

             &RectangularMatrixTest::negative,
             &RectangularMatrixTest::addSubtract,
             &RectangularMatrixTest::multiplyDivide,
             &RectangularMatrixTest::multiply,

             &RectangularMatrixTest::transposed,

             &RectangularMatrixTest::debug,
             &RectangularMatrixTest::configuration);
}

void RectangularMatrixTest::construct() {
    float m[] = {
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f
    };

    Matrix3x4 expected(
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f
    );

    CORRADE_COMPARE(Matrix3x4::from(m), expected);
}

void RectangularMatrixTest::constructFromVectors() {
    Matrix3x4 actual = Matrix3x4::from(Vector4(1.0f, 2.0f, 3.0f, 4.0f),
                                       Vector4(5.0f, 6.0f, 7.0f, 8.0f),
                                       Vector4(9.0f, 10.0f, 11.0f, 12.0f));

    Matrix3x4 expected(1.0f, 2.0f, 3.0f, 4.0f,
                       5.0f, 6.0f, 7.0f, 8.0f,
                       9.0f, 10.0f, 11.0f, 12.0f);

    CORRADE_COMPARE(actual, expected);
}


void RectangularMatrixTest::constructFrom() {
    Matrix2 floatingPoint(1.3f, 2.7f, -15.0f, 7.0f);
    Matrix2 floatingPointRounded(1.0f, 2.0f, -15.0f, 7.0f);
    Matrix2i integral(1, 2, -15, 7);

    CORRADE_COMPARE(Matrix2i::from(floatingPoint), integral);
    CORRADE_COMPARE(Matrix2::from(integral), floatingPointRounded);
}

void RectangularMatrixTest::constructZero() {
    Matrix4x3 zero;

    Matrix4x3 zeroExpected(
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    );

    CORRADE_COMPARE(zero, zeroExpected);
}

void RectangularMatrixTest::data() {
    Matrix3x4 m;

    Vector4 vector(4.0f, 5.0f, 6.0f, 7.0f);

    m[2] = vector;
    m[1][1] = 1.0f;

    m(0, 2) = 1.5f;

    CORRADE_COMPARE(m(1, 1), 1.0f);
    CORRADE_COMPARE(m[0][2], 1.5f);
    CORRADE_COMPARE(m[2], vector);

    Matrix3x4 expected(
        0.0f, 0.0f, 1.5f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        4.0f, 5.0f, 6.0f, 7.0f
    );

    CORRADE_COMPARE(m, expected);
}

void RectangularMatrixTest::negative() {
    CORRADE_COMPARE(-Matrix2(1.0f, -3.0f, 5.0f, -10.0f), Matrix2(-1.0f, 3.0f, -5.0f, 10.0f));
}

void RectangularMatrixTest::addSubtract() {
    Matrix4x3 a(0.0f,   1.0f,   3.0f,
                4.0f,   5.0f,   7.0f,
                8.0f,   9.0f,   11.0f,
                12.0f, 13.0f,  15.0f);
    Matrix4x3 b(-4.0f,  0.5f,   9.0f,
                -9.0f, 11.0f,  0.25f,
                 0.0f, -8.0f,  19.0f,
                -3.0f, -5.0f,   2.0f);
    Matrix4x3 e(-4.0f,  1.5f,  12.0f,
                -5.0f, 16.0f,  7.25f,
                 8.0f,  1.0f,  30.0f,
                 9.0f,  8.0f,  17.0f);

    CORRADE_COMPARE(a + b, e);
    CORRADE_COMPARE(e - b, a);
}

void RectangularMatrixTest::multiplyDivide() {
    Matrix2 vec(1.0f, 2.0f, 3.0f, 4.0f);
    Matrix2 multiplied(-1.5f, -3.0f, -4.5f, -6.0f);

    CORRADE_COMPARE(vec*-1.5f, multiplied);
    CORRADE_COMPARE(-1.5f*vec, multiplied);
    CORRADE_COMPARE(multiplied/-1.5f, vec);

    Math::RectangularMatrix<1, 1, std::int8_t> vecChar(32);
    Math::RectangularMatrix<1, 1, std::int8_t> multipliedChar(-48);
    CORRADE_COMPARE(vecChar*-1.5f, multipliedChar);
    CORRADE_COMPARE(multipliedChar/-1.5f, vecChar);
    CORRADE_COMPARE(-1.5f*vecChar, multipliedChar);

    /* Divide vector with number and inverse */
    Matrix2 divisor(1.0f, 2.0f, -4.0f, 8.0f);
    Matrix2 result(1.0f, 0.5f, -0.25f, 0.125f);
    CORRADE_COMPARE(1.0f/divisor, result);
    CORRADE_COMPARE(-1550.0f/multipliedChar, vecChar);
}

void RectangularMatrixTest::multiply() {
    RectangularMatrix<4, 6, std::int32_t> left(
        -5,     27,     10,     33,     0,     -15,
         7,     56,     66,      1,     0,     -24,
         4,     41,      4,      0,     1,      -4,
         9,   -100,     19,    -49,     1,       9
    );

    RectangularMatrix<5, 4, std::int32_t> right(
         1,     -7,      0,    158,
         2,     24,     -3,     40,
         3,    -15,     -2,    -50,
         4,     17,     -1,   -284,
         5,     30,      4,     18
    );

    RectangularMatrix<5, 6, std::int32_t> expected(
        1368, -16165,   2550,  -7716,    158,   1575,
         506,  -2725,   2352,  -1870,     37,   -234,
        -578,   4159,  -1918,   2534,    -52,   -127,
       -2461,  29419,  -4238,  14065,   -285,  -3020,
         363,    179,   2388,   -687,     22,   -649
    );

    CORRADE_COMPARE(left*right, expected);
}

void RectangularMatrixTest::transposed() {
    Matrix4x3 original(
        0.0f,   1.0f,   3.0f,
        4.0f,   5.0f,   7.0f,
        8.0f,   9.0f,   11.0f,
        12.0f,  13.0f,  15.0f
    );

    Matrix3x4 transposed(
        0.0f,   4.0f,   8.0f,   12.0f,
        1.0f,   5.0f,   9.0f,   13.0f,
        3.0f,   7.0f,   11.0f,  15.0f
    );

    CORRADE_COMPARE(original.transposed(), transposed);
}

void RectangularMatrixTest::debug() {
    Matrix3x4 m(
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f
    );

    std::ostringstream o;
    Debug(&o) << m;
    CORRADE_COMPARE(o.str(), "Matrix(3, 4, 7,\n"
                             "       5, 4, -1,\n"
                             "       8, 7, 8,\n"
                             "       4, 3, 0)\n");

    o.str("");
    Debug(&o) << "a" << Matrix3x4() << "b" << RectangularMatrix<4, 3, std::int8_t>();
    CORRADE_COMPARE(o.str(), "a Matrix(0, 0, 0,\n"
                             "       0, 0, 0,\n"
                             "       0, 0, 0,\n"
                             "       0, 0, 0) b Matrix(0, 0, 0, 0,\n"
                             "       0, 0, 0, 0,\n"
                             "       0, 0, 0, 0)\n");
}

void RectangularMatrixTest::configuration() {
    Matrix3x4 m(
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.125f,
        7.0f, -1.0f, 8.0f, 9.55f
    );
    std::string value("3 4 7 5 4 -1 8 7 8 4 3.125 9.55");

    Configuration c;
    c.setValue<Matrix3x4>("matrix", m);

    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix3x4>("matrix"), m);
}

}}}
