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
#include "SceneGraph/EuclideanMatrixTransformation3D.h"

namespace Magnum { namespace SceneGraph { namespace Test {

class EuclideanMatrixTransformation3DTest: public Corrade::TestSuite::Tester {
    public:
        explicit EuclideanMatrixTransformation3DTest();

        void fromMatrix();
        void toMatrix();
        void compose();
        void inverted();
};

EuclideanMatrixTransformation3DTest::EuclideanMatrixTransformation3DTest() {
    addTests(&EuclideanMatrixTransformation3DTest::fromMatrix,
             &EuclideanMatrixTransformation3DTest::toMatrix,
             &EuclideanMatrixTransformation3DTest::compose,
             &EuclideanMatrixTransformation3DTest::inverted);
}

void EuclideanMatrixTransformation3DTest::fromMatrix() {
    Matrix4 m = Matrix4::rotationX(deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation3D<>::fromMatrix(m), m);
}

void EuclideanMatrixTransformation3DTest::toMatrix() {
    Matrix4 m = Matrix4::rotationX(deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation3D<>::toMatrix(m), m);
}

void EuclideanMatrixTransformation3DTest::compose() {
    Matrix4 parent = Matrix4::rotationX(deg(17.0f));
    Matrix4 child = Matrix4::translation({1.0f, -0.3f, 2.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation3D<>::compose(parent, child), parent*child);
}

void EuclideanMatrixTransformation3DTest::inverted() {
    Matrix4 m = Matrix4::rotationX(deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation3D<>::inverted(m)*m, Matrix4());
}

}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::EuclideanMatrixTransformation3DTest)
