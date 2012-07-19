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

#include "Matrix3Test.h"

#include <sstream>
#include <QtTest/QTest>

#include "Matrix3.h"
#include "Math.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::Matrix3Test)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Math::Matrix3<float> Matrix3;

void Matrix3Test::constructIdentity() {
    Matrix3 identity;
    Matrix3 identity2(Matrix3::Identity);
    Matrix3 identity3(Matrix3::Identity, 4.0f);

    Matrix3 identityExpected(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    );

    Matrix3 identity3Expected(
        4.0f, 0.0f, 0.0f,
        0.0f, 4.0f, 0.0f,
        0.0f, 0.0f, 4.0f
    );

    QVERIFY(identity == identityExpected);
    QVERIFY(identity2 == identityExpected);
    QVERIFY(identity3 == identity3Expected);
}

void Matrix3Test::translation() {
    Matrix3 matrix(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        3.0f, 1.0f, 1.0f
    );

    QVERIFY(Matrix3::translation({3.0f, 1.0f}) == matrix);
}

void Matrix3Test::scaling() {
    Matrix3 matrix(
        3.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f,
        0.0f, 0.0f, 1.0f
    );

    QVERIFY(Matrix3::scaling({3.0f, 1.5f}) == matrix);
}

void Matrix3Test::rotation() {
    Matrix3 matrix(
        0.965926f, 0.258819f, 0.0f,
        -0.258819f, 0.965926f, 0.0f,
        0.0f, 0.0f, 1.0f
    );

    QVERIFY(Matrix3::rotation(deg(15.0f)) == matrix);
}

void Matrix3Test::debug() {
    Matrix3 m(
        3.0f, 5.0f, 8.0f,
        4.0f, 4.0f, 7.0f,
        7.0f, -1.0f, 8.0f
    );

    ostringstream o;
    Debug(&o) << m;
    QCOMPARE(QString::fromStdString(o.str()), QString("Matrix(3, 4, 7,\n"
                                                      "       5, 4, -1,\n"
                                                      "       8, 7, 8)\n"));
}

}}}
