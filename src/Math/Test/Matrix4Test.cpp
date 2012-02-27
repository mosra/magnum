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

#include "Matrix4Test.h"

#include <sstream>
#include <QtTest/QTest>

#include "Matrix4.h"
#include "Math.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::Matrix4Test)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Math::Matrix4<float> Matrix4;

void Matrix4Test::translation() {
    Matrix4 matrix(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        3.0f, 1.0f, 2.0f, 1.0f
    );

    QVERIFY(Matrix4::translation({3.0f, 1.0f, 2.0f}) == matrix);
}

void Matrix4Test::scaling() {
    Matrix4 matrix(
        3.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    QVERIFY(Matrix4::scaling({3.0f, 1.5f, 2.0f}) == matrix);
}

void Matrix4Test::rotation() {
    Matrix4 matrix(
        0.35612214f,  -0.80181062f, 0.47987163f,  0.0f,
        0.47987163f,  0.59757638f,  0.6423595f,  0.0f,
        -0.80181062f, 0.0015183985f, 0.59757638f,  0.0f,
        0.0f,       0.0f,       0.0f,       1.0f
    );

    QVERIFY(Matrix4::rotation(deg(-74.0f), {-1.0f, 2.0f, 2.0f}) == matrix);
}

void Matrix4Test::debug() {
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
}

}}}
