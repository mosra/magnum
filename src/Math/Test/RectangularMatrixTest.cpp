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
#include <Utility/Configuration.h>

#include "Math/RectangularMatrix.h"

using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

class RectangularMatrixTest: public Corrade::TestSuite::Tester {
    public:
        RectangularMatrixTest();

        void constructFromData();
        void constructDefault();
        void constructConversion();
        void constructFromVectors();
        void data();

        void compare();

        void negative();
        void addSubtract();
        void multiplyDivide();
        void multiply();

        void transposed();

        void debug();
        void configuration();
};

typedef RectangularMatrix<4, 3, float> Matrix4x3;
typedef RectangularMatrix<3, 4, float> Matrix3x4;
typedef RectangularMatrix<2, 2, float> Matrix2;
typedef RectangularMatrix<2, 2, std::int32_t> Matrix2i;
typedef Vector<4, float> Vector4;
typedef Vector<3, float> Vector3;
typedef Vector<2, float> Vector2;
typedef Vector<2, std::int32_t> Vector2i;

RectangularMatrixTest::RectangularMatrixTest() {
    addTests(&RectangularMatrixTest::constructFromData,
             &RectangularMatrixTest::constructDefault,
             &RectangularMatrixTest::constructConversion,
             &RectangularMatrixTest::constructFromVectors,
             &RectangularMatrixTest::data,

             &RectangularMatrixTest::compare,

             &RectangularMatrixTest::negative,
             &RectangularMatrixTest::addSubtract,
             &RectangularMatrixTest::multiplyDivide,
             &RectangularMatrixTest::multiply,

             &RectangularMatrixTest::transposed,

             &RectangularMatrixTest::debug,
             &RectangularMatrixTest::configuration);
}

void RectangularMatrixTest::constructFromData() {
    float m[] = {
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f
    };

    Matrix3x4 expected(Vector4(3.0f, 5.0f, 8.0f, 4.0f),
                       Vector4(4.0f, 4.0f, 7.0f, 3.0f),
                       Vector4(7.0f, -1.0f, 8.0f, 0.0f));

    CORRADE_COMPARE(Matrix3x4::from(m), expected);
}

void RectangularMatrixTest::constructDefault() {
    Matrix4x3 zero;

    Matrix4x3 zeroExpected(Vector3(0.0f, 0.0f, 0.0f),
                           Vector3(0.0f, 0.0f, 0.0f),
                           Vector3(0.0f, 0.0f, 0.0f),
                           Vector3(0.0f, 0.0f, 0.0f));

    CORRADE_COMPARE(zero, zeroExpected);
}

void RectangularMatrixTest::constructConversion() {
    Matrix2 floatingPoint(Vector2(  1.3f, 2.7f),
                          Vector2(-15.0f, 7.0f));
    Matrix2 floatingPointRounded(Vector2(1.0f, 2.0f),
                                 Vector2(-15.0f, 7.0f));
    Matrix2i integral(Vector2i(  1, 2),
                      Vector2i(-15, 7));

    CORRADE_COMPARE(Matrix2i(floatingPoint), integral);
    CORRADE_COMPARE(Matrix2(integral), floatingPointRounded);
}

void RectangularMatrixTest::constructFromVectors() {
    Matrix3x4 actual(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                     Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                     Vector4(9.0f, 10.0f, 11.0f, 12.0f));

    Matrix3x4 expected(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                       Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                       Vector4(9.0f, 10.0f, 11.0f, 12.0f));

    CORRADE_COMPARE(actual, expected);
}

void RectangularMatrixTest::data() {
    Matrix3x4 m;
    Vector4 vector(4.0f, 5.0f, 6.0f, 7.0f);

    m[2] = vector;
    m[1][1] = 1.0f;
    m[0][2] = 1.5f;

    CORRADE_COMPARE(m[1][1], 1.0f);
    CORRADE_COMPARE(m[0][2], 1.5f);
    CORRADE_COMPARE(m[2], vector);

    Matrix3x4 expected(Vector4(0.0f, 0.0f, 1.5f, 0.0f),
                       Vector4(0.0f, 1.0f, 0.0f, 0.0f),
                       Vector4(4.0f, 5.0f, 6.0f, 7.0f));

    CORRADE_COMPARE(m, expected);
}

void RectangularMatrixTest::compare() {
    Matrix2 a(Vector2(1.0f, -3.0f),
              Vector2(5.0f, -10.0f));
    Matrix2 b(Vector2(1.0f + MathTypeTraits<float>::epsilon()/2, -3.0f),
              Vector2(5.0f, -10.0f));
    Matrix2 c(Vector2(1.0f, -1.0f + MathTypeTraits<float>::epsilon()*2),
              Vector2(5.0f, -10.0f));
    CORRADE_VERIFY(a == b);
    CORRADE_VERIFY(a != c);

    Matrix2i ai(Vector2i(1, -3),
                Vector2i(5, -10));
    Matrix2i bi(Vector2i(1, -2),
                Vector2i(5, -10));
    CORRADE_VERIFY(ai == ai);
    CORRADE_VERIFY(ai != bi);
}

void RectangularMatrixTest::negative() {
    Matrix2 matrix(Vector2(1.0f,  -3.0f),
                   Vector2(5.0f, -10.0f));
    Matrix2 negated(Vector2(-1.0f,  3.0f),
                    Vector2(-5.0f, 10.0f));
    CORRADE_COMPARE(-matrix, negated);
}

void RectangularMatrixTest::addSubtract() {
    Matrix4x3 a(Vector3(0.0f,   1.0f,   3.0f),
                Vector3(4.0f,   5.0f,   7.0f),
                Vector3(8.0f,   9.0f,   11.0f),
                Vector3(12.0f, 13.0f,  15.0f));
    Matrix4x3 b(Vector3(-4.0f,  0.5f,   9.0f),
                Vector3(-9.0f, 11.0f,  0.25f),
                Vector3( 0.0f, -8.0f,  19.0f),
                Vector3(-3.0f, -5.0f,   2.0f));
    Matrix4x3 c(Vector3(-4.0f,  1.5f,  12.0f),
                Vector3(-5.0f, 16.0f,  7.25f),
                Vector3( 8.0f,  1.0f,  30.0f),
                Vector3( 9.0f,  8.0f,  17.0f));

    CORRADE_COMPARE(a + b, c);
    CORRADE_COMPARE(c - b, a);
}

void RectangularMatrixTest::multiplyDivide() {
    Matrix2 matrix(Vector2(1.0f, 2.0f),
                   Vector2(3.0f, 4.0f));
    Matrix2 multiplied(Vector2(-1.5f, -3.0f),
                       Vector2(-4.5f, -6.0f));

    CORRADE_COMPARE(matrix*-1.5f, multiplied);
    CORRADE_COMPARE(-1.5f*matrix, multiplied);
    CORRADE_COMPARE(multiplied/-1.5f, matrix);

    Math::RectangularMatrix<1, 1, std::int8_t> matrixChar(32);
    Math::RectangularMatrix<1, 1, std::int8_t> multipliedChar(-48);
    CORRADE_COMPARE(matrixChar*-1.5f, multipliedChar);
    CORRADE_COMPARE(multipliedChar/-1.5f, matrixChar);
    CORRADE_COMPARE(-1.5f*matrixChar, multipliedChar);

    /* Divide vector with number and inverse */
    Matrix2 divisor(Vector2( 1.0f, 2.0f),
                    Vector2(-4.0f, 8.0f));
    Matrix2 result(Vector2(  1.0f,   0.5f),
                   Vector2(-0.25f, 0.125f));
    CORRADE_COMPARE(1.0f/divisor, result);
    CORRADE_COMPARE(-1550.0f/multipliedChar, matrixChar);
}

void RectangularMatrixTest::multiply() {
    RectangularMatrix<4, 6, std::int32_t> left(
        Vector<6, std::int32_t>(-5,   27, 10,  33, 0, -15),
        Vector<6, std::int32_t>( 7,   56, 66,   1, 0, -24),
        Vector<6, std::int32_t>( 4,   41,  4,   0, 1,  -4),
        Vector<6, std::int32_t>( 9, -100, 19, -49, 1,   9)
    );

    RectangularMatrix<5, 4, std::int32_t> right(
        Vector<4, std::int32_t>(1,  -7,  0,  158),
        Vector<4, std::int32_t>(2,  24, -3,   40),
        Vector<4, std::int32_t>(3, -15, -2,  -50),
        Vector<4, std::int32_t>(4,  17, -1, -284),
        Vector<4, std::int32_t>(5,  30,  4,   18)
    );

    RectangularMatrix<5, 6, std::int32_t> expected(
       Vector<6, std::int32_t>( 1368, -16165,  2550, -7716,  158,  1575),
       Vector<6, std::int32_t>(  506,  -2725,  2352, -1870,   37,  -234),
       Vector<6, std::int32_t>( -578,   4159, -1918,  2534,  -52,  -127),
       Vector<6, std::int32_t>(-2461,  29419, -4238, 14065, -285, -3020),
       Vector<6, std::int32_t>(  363,    179,  2388,  -687,   22,  -649)
    );

    CORRADE_COMPARE(left*right, expected);
}

void RectangularMatrixTest::transposed() {
    Matrix4x3 original(Vector3( 0.0f,  1.0f,  3.0f),
                       Vector3( 4.0f,  5.0f,  7.0f),
                       Vector3( 8.0f,  9.0f, 11.0f),
                       Vector3(12.0f, 13.0f, 15.0f));

    Matrix3x4 transposed(Vector4(0.0f, 4.0f,  8.0f, 12.0f),
                         Vector4(1.0f, 5.0f,  9.0f, 13.0f),
                         Vector4(3.0f, 7.0f, 11.0f, 15.0f));

    CORRADE_COMPARE(original.transposed(), transposed);
}

void RectangularMatrixTest::debug() {
    Matrix3x4 m(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
                Vector4(4.0f,  4.0f, 7.0f, 3.0f),
                Vector4(7.0f, -1.0f, 8.0f, 0.0f));

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
    Matrix3x4 m(Vector4(3.0f,  5.0f, 8.0f,   4.0f),
                Vector4(4.0f,  4.0f, 7.0f, 3.125f),
                Vector4(7.0f, -1.0f, 8.0f,  9.55f));
    std::string value("3 4 7 5 4 -1 8 7 8 4 3.125 9.55");

    Configuration c;
    c.setValue<Matrix3x4>("matrix", m);

    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix3x4>("matrix"), m);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::RectangularMatrixTest)
