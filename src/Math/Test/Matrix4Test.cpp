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
typedef Math::Matrix3<float> Matrix3;

void Matrix4Test::constructIdentity() {
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

void Matrix4Test::rotationScalingPart() {
    Matrix4 m(
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f,
        9.0f, 4.0f, 5.0f, 9.0f
    );

    Matrix3 expected(
        3.0f, 5.0f, 8.0f,
        4.0f, 4.0f, 7.0f,
        7.0f, -1.0f, 8.0f
    );

    QVERIFY(m.rotationScaling() == expected);
}

void Matrix4Test::rotationPart() {
    Matrix3 expectedRotationPart(
        0.35612214f,  -0.80181062f, 0.47987163f,
        0.47987163f,  0.59757638f,  0.6423595f,
        -0.80181062f, 0.0015183985f, 0.59757638f
    );

    Matrix4 rotation = Matrix4::rotation(deg(-74.0f), {-1.0f, 2.0f, 2.0f});
    QVERIFY(rotation.rotation() == expectedRotationPart);

    Matrix4 rotationTransformed = Matrix4::translation({2.0f, 5.0f, -3.0f})*rotation*Matrix4::scaling(Vector3<float>(9.0f));
    QVERIFY(rotationTransformed.rotation() == expectedRotationPart);
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
