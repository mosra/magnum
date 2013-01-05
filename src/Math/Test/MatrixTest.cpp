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

#include "Math/Matrix.h"

using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

class MatrixTest: public Corrade::TestSuite::Tester {
    public:
        MatrixTest();

        void construct();
        void constructIdentity();
        void constructZero();
        void trace();
        void ij();
        void determinant();
        void inverted();

        void debug();
        void configuration();
};

typedef Matrix<4, float> Matrix4;
typedef Matrix<3, float> Matrix3;
typedef Vector<4, float> Vector4;

MatrixTest::MatrixTest() {
    addTests(&MatrixTest::construct,
             &MatrixTest::constructIdentity,
             &MatrixTest::constructZero,
             &MatrixTest::trace,
             &MatrixTest::ij,
             &MatrixTest::determinant,
             &MatrixTest::inverted,
             &MatrixTest::debug,
             &MatrixTest::configuration);
}

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

    CORRADE_COMPARE(Matrix4::from(m), expected);
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

    CORRADE_COMPARE(identity, identityExpected);
    CORRADE_COMPARE(identity2, identityExpected);
    CORRADE_COMPARE(identity3, identity3Expected);
}

void MatrixTest::constructZero() {
    Matrix4 zero(Matrix4::Zero);

    Matrix4 zeroExpected(
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    );

    CORRADE_COMPARE(zero, zeroExpected);
}

void MatrixTest::trace() {
    Matrix<5, std::int32_t> m(
        1, 2, 3, 0, 0,
        2, 3, 2, 1, -2,
        1, 1, -20, 1, 0,
        2, 0, 0, 10, 2,
        3, 1, 0, 1, -2
    );

    CORRADE_COMPARE(m.trace(), -8);
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

    CORRADE_COMPARE(original.ij(1, 2), skipped);
}

void MatrixTest::determinant() {
    Matrix<5, std::int32_t> m(
        1, 2, 2, 1, 0,
        2, 3, 2, 1, -2,
        1, 1, 1, 1, 0,
        2, 0, 0, 1, 2,
        3, 1, 0, 1, -2
    );

    CORRADE_COMPARE(m.determinant(), -2);
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

    CORRADE_COMPARE(_inverse, inverse);
    CORRADE_COMPARE(_inverse*m, Matrix4());
}

void MatrixTest::debug() {
    Matrix4 m(
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f,
        9.0f, 4.0f, 5.0f, 9.0f
    );

    std::ostringstream o;
    Debug(&o) << m;
    CORRADE_COMPARE(o.str(), "Matrix(3, 4, 7, 9,\n"
                             "       5, 4, -1, 4,\n"
                             "       8, 7, 8, 5,\n"
                             "       4, 3, 0, 9)\n");

    o.str("");
    Debug(&o) << "a" << Matrix4() << "b" << Matrix4();
    CORRADE_COMPARE(o.str(), "a Matrix(1, 0, 0, 0,\n"
                             "       0, 1, 0, 0,\n"
                             "       0, 0, 1, 0,\n"
                             "       0, 0, 0, 1) b Matrix(1, 0, 0, 0,\n"
                             "       0, 1, 0, 0,\n"
                             "       0, 0, 1, 0,\n"
                             "       0, 0, 0, 1)\n");
}

void MatrixTest::configuration() {
    Configuration c;

    Matrix4 m(
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.125f,
        7.0f, -1.0f, 8.0f, 0.0f,
        9.0f, 4.0f, 5.0f, 9.55f
    );
    std::string value("3 4 7 9 5 4 -1 4 8 7 8 5 4 3.125 0 9.55");

    c.setValue("matrix", m);
    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix4>("matrix"), m);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::MatrixTest)
