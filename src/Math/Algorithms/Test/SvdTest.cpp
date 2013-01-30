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

#include <TestSuite/Tester.h>

#include "Math/Algorithms/Svd.h"

namespace Magnum { namespace Math { namespace Algorithms { namespace Test {

class SvdTest: public Corrade::TestSuite::Tester {
    public:
        explicit SvdTest();

        void testDouble();
        void testFloat();
};

typedef RectangularMatrix<5, 8, double> Matrix5x8d;
typedef Matrix<8, double> Matrix8d;
typedef Matrix<5, double> Matrix5d;
typedef Vector<8, double> Vector8d;
typedef Vector<5, double> Vector5d;

typedef RectangularMatrix<5, 8, float> Matrix5x8f;
typedef Matrix<8, float> Matrix8f;
typedef Matrix<5, float> Matrix5f;
typedef Vector<8, float> Vector8f;
typedef Vector<5, float> Vector5f;

constexpr static Matrix5x8d a(
    Vector8d(22.0, 14.0,  -1.0, -3.0,  9.0,  9.0,  2.0,  4.0),
    Vector8d(10.0,  7.0,  13.0, -2.0,  8.0,  1.0, -6.0,  5.0),
    Vector8d( 2.0, 10.0,  -1.0, 13.0,  1.0, -7.0,  6.0,  0.0),
    Vector8d( 3.0,  0.0, -11.0, -2.0, -2.0,  5.0,  5.0, -2.0),
    Vector8d( 7.0,  8.0,   3.0,  4.0,  4.0, -1.0,  1.0,  2.0)
);

static const Vector5d expected(std::sqrt(1248.0), 0.0, 20.0, std::sqrt(384.0), 0.0);

SvdTest::SvdTest() {
    addTests(&SvdTest::testDouble,
             &SvdTest::testFloat);
}

void SvdTest::testDouble() {
    Matrix5x8d u;
    Vector5d w;
    Matrix5d v;
    std::tie(u, w, v) = Algorithms::svd(a);

    /* Test composition */
    Matrix8d u2(u[0], u[1], u[2], u[3], u[4], Vector8d(), Vector8d(), Vector8d());
    Matrix5x8d w2 = Matrix5x8d::fromDiagonal(w);
    CORRADE_COMPARE(u2*w2*v.transposed(), a);

    /* Test that V is unitary */
    CORRADE_COMPARE(v*v.transposed(), Matrix5d(Matrix5d::Identity));
    CORRADE_COMPARE(v.transposed()*v, Matrix5d(Matrix5d::Identity));

    /* Test W */
    CORRADE_COMPARE(w, expected);
}

void SvdTest::testFloat() {
    Matrix5x8f u;
    Vector5f w;
    Matrix5f v;
    std::tie(u, w, v) = Algorithms::svd(Matrix5x8f(a));

    /* Test composition (single precision is not enough, test for similarity) */
    Matrix8f u2(u[0], u[1], u[2], u[3], u[4], Vector8f(), Vector8f(), Vector8f());
    Matrix5x8f w2 = Matrix5x8f::fromDiagonal(w);
    CORRADE_VERIFY((u2*w2*v.transposed()-Matrix5x8f(a)).maxAbs() < 1.0e-5f);

    /* Test that V is unitary */
    CORRADE_COMPARE(v*v.transposed(), Matrix5f(Matrix5f::Identity));
    CORRADE_COMPARE(v.transposed()*v, Matrix5f(Matrix5f::Identity));

    /* Test W (single precision is not enough, test for similarity) */
    CORRADE_VERIFY((w-Vector5f(expected)).maxAbs() < 1.0e-5f);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Algorithms::Test::SvdTest)
