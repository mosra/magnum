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
#include "SceneGraph/MatrixTransformation3D.h"

namespace Magnum { namespace SceneGraph { namespace Test {

class MatrixTransformation3DTest: public Corrade::TestSuite::Tester {
    public:
        explicit MatrixTransformation3DTest();

        void fromMatrix();
        void toMatrix();
        void compose();
        void inverted();
};

MatrixTransformation3DTest::MatrixTransformation3DTest() {
    addTests(&MatrixTransformation3DTest::fromMatrix,
             &MatrixTransformation3DTest::toMatrix,
             &MatrixTransformation3DTest::compose,
             &MatrixTransformation3DTest::inverted);
}

void MatrixTransformation3DTest::fromMatrix() {
    Matrix4 m = Matrix4::rotationX(deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f})*Matrix4::scaling({2.0f, 1.4f, -2.1f});
    CORRADE_COMPARE(MatrixTransformation3D<>::fromMatrix(m), m);
}

void MatrixTransformation3DTest::toMatrix() {
    Matrix4 m = Matrix4::rotationX(deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f})*Matrix4::scaling({2.0f, 1.4f, -2.1f});
    CORRADE_COMPARE(MatrixTransformation3D<>::toMatrix(m), m);
}

void MatrixTransformation3DTest::compose() {
    Matrix4 parent = Matrix4::rotationX(deg(17.0f));
    Matrix4 child = Matrix4::translation({1.0f, -0.3f, 2.3f});
    CORRADE_COMPARE(MatrixTransformation3D<>::compose(parent, child), parent*child);
}

void MatrixTransformation3DTest::inverted() {
    Matrix4 m = Matrix4::rotationX(deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f})*Matrix4::scaling({2.0f, 1.4f, -2.1f});
    CORRADE_COMPARE(MatrixTransformation3D<>::inverted(m)*m, Matrix4());
}

}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::MatrixTransformation3DTest)
