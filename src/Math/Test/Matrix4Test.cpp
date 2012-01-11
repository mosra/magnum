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

QTEST_APPLESS_MAIN(Magnum::Math::Test::Matrix4Test)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Math::Matrix4<float> Matrix4;

void Matrix4Test::translation() {
    float matrix[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        3.0f, 1.0f, 2.0f, 1.0f
    };

    QVERIFY(Matrix4::translation(3.0f, 1.0f, 2.0f) == Matrix4(matrix));
}

void Matrix4Test::scaling() {
    float matrix[] = {
        3.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    QVERIFY(Matrix4::scaling(3.0f, 1.5f, 2.0f) == Matrix4(matrix));
}

void Matrix4Test::rotation() {
    float matrix[] = {
        0.35612214f,  -0.80181062f, 0.47987163f,  0.0f,
        0.47987163f,  0.59757638f,  0.6423595f,  0.0f,
        -0.80181062f, 0.0015183985f, 0.59757638f,  0.0f,
        0.0f,       0.0f,       0.0f,       1.0f
    };

    QVERIFY(Matrix4::rotation(deg(-74.0f), -1.0f, 2.0f, 2.0f) == Matrix4(matrix));
}

void Matrix4Test::debug() {
    float m[] = {
        3, 5, 8, 4,
        4, 4, 7, 3,
        7, -1, 8, 0,
        9, 4, 5, 9
    };

    ostringstream o;
    Debug(&o) << Matrix4(m);
    QCOMPARE(QString::fromStdString(o.str()), QString("Matrix(3, 4, 7, 9,\n"
                                                      "       5, 4, -1, 4,\n"
                                                      "       8, 7, 8, 5,\n"
                                                      "       4, 3, 0, 9)\n"));
}

}}}
