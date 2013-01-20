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

#include "Math/Constants.h"
#include "SceneGraph/MatrixTransformation2D.h"

namespace Magnum { namespace SceneGraph { namespace Test {

class MatrixTransformation2DTest: public Corrade::TestSuite::Tester {
    public:
        explicit MatrixTransformation2DTest();

        void fromMatrix();
        void toMatrix();
        void compose();
        void inverted();
};

MatrixTransformation2DTest::MatrixTransformation2DTest() {
    addTests(&MatrixTransformation2DTest::fromMatrix,
             &MatrixTransformation2DTest::toMatrix,
             &MatrixTransformation2DTest::compose,
             &MatrixTransformation2DTest::inverted);
}

void MatrixTransformation2DTest::fromMatrix() {
    Matrix3 m = Matrix3::rotation(deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(MatrixTransformation2D<>::fromMatrix(m), m);
}

void MatrixTransformation2DTest::toMatrix() {
    Matrix3 m = Matrix3::rotation(deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(MatrixTransformation2D<>::toMatrix(m), m);
}

void MatrixTransformation2DTest::compose() {
    Matrix3 parent = Matrix3::rotation(deg(17.0f));
    Matrix3 child = Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(MatrixTransformation2D<>::compose(parent, child), parent*child);
}

void MatrixTransformation2DTest::inverted() {
    Matrix3 m = Matrix3::rotation(deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(MatrixTransformation2D<>::inverted(m)*m, Matrix3());
}

}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::MatrixTransformation2DTest)
