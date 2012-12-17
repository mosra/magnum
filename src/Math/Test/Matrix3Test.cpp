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
#include <Utility/Configuration.h>

#include "Constants.h"
#include "Matrix3.h"

CORRADE_TEST_MAIN(Magnum::Math::Test::Matrix3Test)

using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Math::Matrix3<float> Matrix3;
typedef Math::Matrix<2, float> Matrix2;
typedef Math::Vector2<float> Vector2;

Matrix3Test::Matrix3Test() {
    addTests(&Matrix3Test::constructIdentity,
             &Matrix3Test::translation,
             &Matrix3Test::scaling,
             &Matrix3Test::rotation,
             &Matrix3Test::fromParts,
             &Matrix3Test::rotationScalingPart,
             &Matrix3Test::rotationPart,
             &Matrix3Test::vectorParts,
             &Matrix3Test::debug,
             &Matrix3Test::configuration);
}

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

    CORRADE_COMPARE(identity, identityExpected);
    CORRADE_COMPARE(identity2, identityExpected);
    CORRADE_COMPARE(identity3, identity3Expected);
}

void Matrix3Test::translation() {
    Matrix3 matrix(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        3.0f, 1.0f, 1.0f
    );

    CORRADE_COMPARE(Matrix3::translation({3.0f, 1.0f}), matrix);
}

void Matrix3Test::scaling() {
    Matrix3 matrix(
        3.0f, 0.0f, 0.0f,
        0.0f, 1.5f, 0.0f,
        0.0f, 0.0f, 1.0f
    );

    CORRADE_COMPARE(Matrix3::scaling({3.0f, 1.5f}), matrix);
}

void Matrix3Test::rotation() {
    Matrix3 matrix(
        0.965926f, 0.258819f, 0.0f,
        -0.258819f, 0.965926f, 0.0f,
        0.0f, 0.0f, 1.0f
    );

    CORRADE_COMPARE(Matrix3::rotation(deg(15.0f)), matrix);
}

void Matrix3Test::fromParts() {
    Matrix2 rotationScaling(
        3.0f, 5.0f,
        4.0f, 4.0f
    );

    Vector2 translation(7.0f, -1.0f);

    Matrix3 expected(
        3.0f, 5.0f, 0.0f,
        4.0f, 4.0f, 0.0f,
        7.0f, -1.0f, 1.0f
    );
    CORRADE_COMPARE(Matrix3::from(rotationScaling, translation), expected);

    Matrix3 expectedNoRotation(
        3.0f, 5.0f, 0.0f,
        4.0f, 4.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    );
    CORRADE_COMPARE(Matrix3::from(rotationScaling), expectedNoRotation);
}

void Matrix3Test::rotationScalingPart() {
    Matrix3 m(
        3.0f, 5.0f, 8.0f,
        4.0f, 4.0f, 7.0f,
        7.0f, -1.0f, 8.0f
    );

    Matrix2 expected(
        3.0f, 5.0f,
        4.0f, 4.0f
    );

    CORRADE_COMPARE(m.rotationScaling(), expected);
}

void Matrix3Test::rotationPart() {
    Matrix2 expectedRotationPart(
        0.965926f, 0.258819f,
        -0.258819f, 0.965926f
    );

    Matrix3 rotation = Matrix3::rotation(deg(15.0f));
    CORRADE_COMPARE(rotation.rotation(), expectedRotationPart);

    Matrix3 rotationTransformed = Matrix3::translation({2.0f, 5.0f})*rotation*Matrix3::scaling(Vector2(9.0f));
    CORRADE_COMPARE(rotationTransformed.rotation(), expectedRotationPart);
}

void Matrix3Test::vectorParts() {
    Matrix3 m(15.0f, 0.0f, 0.0f,
              0.0f, -3.0f, 0.0f,
              -5.0f, 12.0f, 1.0f);

    CORRADE_COMPARE(m.right(), Vector2::xAxis(15.0f));
    CORRADE_COMPARE(m.up(), Vector2::yAxis(-3.0f));
    CORRADE_COMPARE(m.translation(), Vector2(-5.0f, 12.0f));
}

void Matrix3Test::debug() {
    Matrix3 m(
        3.0f, 5.0f, 8.0f,
        4.0f, 4.0f, 7.0f,
        7.0f, -1.0f, 8.0f
    );

    std::ostringstream o;
    Debug(&o) << m;
    CORRADE_COMPARE(o.str(), "Matrix(3, 4, 7,\n"
                             "       5, 4, -1,\n"
                             "       8, 7, 8)\n");
}

void Matrix3Test::configuration() {
    Configuration c;

    Matrix3 m(
        5.0f, 8.0f, 4.0f,
        4.0f, 7.0f, 3.125f,
        4.0f, 5.0f, 9.55f
    );
    std::string value("5 4 4 8 7 5 4 3.125 9.55");

    c.setValue("matrix", m);
    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix3>("matrix"), m);
}

}}}
