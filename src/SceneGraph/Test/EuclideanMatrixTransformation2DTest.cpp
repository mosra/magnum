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
#include "SceneGraph/EuclideanMatrixTransformation2D.h"

namespace Magnum { namespace SceneGraph { namespace Test {

class EuclideanMatrixTransformation2DTest: public Corrade::TestSuite::Tester {
    public:
        explicit EuclideanMatrixTransformation2DTest();

        void fromMatrix();
        void toMatrix();
        void compose();
        void inverted();
};

EuclideanMatrixTransformation2DTest::EuclideanMatrixTransformation2DTest() {
    addTests(&EuclideanMatrixTransformation2DTest::fromMatrix,
             &EuclideanMatrixTransformation2DTest::toMatrix,
             &EuclideanMatrixTransformation2DTest::compose,
             &EuclideanMatrixTransformation2DTest::inverted);
}

void EuclideanMatrixTransformation2DTest::fromMatrix() {
    Matrix3 m = Matrix3::rotation(deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation2D<>::fromMatrix(m), m);
}

void EuclideanMatrixTransformation2DTest::toMatrix() {
    Matrix3 m = Matrix3::rotation(deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation2D<>::toMatrix(m), m);
}

void EuclideanMatrixTransformation2DTest::compose() {
    Matrix3 parent = Matrix3::rotation(deg(17.0f));
    Matrix3 child = Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation2D<>::compose(parent, child), parent*child);
}

void EuclideanMatrixTransformation2DTest::inverted() {
    Matrix3 m = Matrix3::rotation(deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation2D<>::inverted(m)*m, Matrix3());
}

}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::EuclideanMatrixTransformation2DTest)
