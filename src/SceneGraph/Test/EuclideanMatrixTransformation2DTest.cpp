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

#include "SceneGraph/EuclideanMatrixTransformation2D.h"
#include "SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test {

typedef Object<EuclideanMatrixTransformation2D<>> Object2D;
typedef Scene<EuclideanMatrixTransformation2D<>> Scene2D;

class EuclideanMatrixTransformation2DTest: public Corrade::TestSuite::Tester {
    public:
        explicit EuclideanMatrixTransformation2DTest();

        void fromMatrix();
        void toMatrix();
        void compose();
        void inverted();

        void setTransformation();
        void translate();
        void rotate();
        void reflect();
        void normalizeRotation();
};

EuclideanMatrixTransformation2DTest::EuclideanMatrixTransformation2DTest() {
    addTests(&EuclideanMatrixTransformation2DTest::fromMatrix,
             &EuclideanMatrixTransformation2DTest::toMatrix,
             &EuclideanMatrixTransformation2DTest::compose,
             &EuclideanMatrixTransformation2DTest::inverted,

             &EuclideanMatrixTransformation2DTest::setTransformation,
             &EuclideanMatrixTransformation2DTest::translate,
             &EuclideanMatrixTransformation2DTest::rotate,
             &EuclideanMatrixTransformation2DTest::reflect,
             &EuclideanMatrixTransformation2DTest::normalizeRotation);
}

void EuclideanMatrixTransformation2DTest::fromMatrix() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation2D<>::fromMatrix(m), m);
}

void EuclideanMatrixTransformation2DTest::toMatrix() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation2D<>::toMatrix(m), m);
}

void EuclideanMatrixTransformation2DTest::compose() {
    Matrix3 parent = Matrix3::rotation(Deg(17.0f));
    Matrix3 child = Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation2D<>::compose(parent, child), parent*child);
}

void EuclideanMatrixTransformation2DTest::inverted() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(EuclideanMatrixTransformation2D<>::inverted(m)*m, Matrix3());
}

void EuclideanMatrixTransformation2DTest::setTransformation() {
    /* Dirty after setting transformation */
    Object2D o;
    o.setClean();
    o.rotate(Deg(17.0f));
    CORRADE_VERIFY(o.isDirty());

    /* Scene cannot be transformed */
    Scene2D s;
    s.setClean();
    s.rotate(Deg(17.0f));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Matrix3());
}

void EuclideanMatrixTransformation2DTest::translate() {
    {
        Object2D o;
        o.rotate(Deg(17.0f));
        o.translate({1.0f, -0.3f});
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({1.0f, -0.3f})*Matrix3::rotation(Deg(17.0f)));
    } {
        Object2D o;
        o.rotate(Deg(17.0f));
        o.translate({1.0f, -0.3f}, TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f}));
    }
}

void EuclideanMatrixTransformation2DTest::rotate() {
    {
        Object2D o;
        o.translate({1.0f, -0.3f});
        o.rotate(Deg(17.0f));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f}));
    } {
        Object2D o;
        o.translate({1.0f, -0.3f});
        o.rotate(Deg(17.0f), TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({1.0f, -0.3f})*Matrix3::rotation(Deg(17.0f)));
    }
}

void EuclideanMatrixTransformation2DTest::reflect() {
    {
        Object2D o;
        o.rotate(Deg(17.0f));
        o.reflect(Vector2(-1.0f/Constants::sqrt2()));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::reflection(Vector2(-1.0f/Constants::sqrt2()))*Matrix3::rotation(Deg(17.0f)));
    } {
        Object2D o;
        o.rotate(Deg(17.0f));
        o.reflect(Vector2(-1.0f/Constants::sqrt2()), TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::reflection(Vector2(-1.0f/Constants::sqrt2())));
    }
}

void EuclideanMatrixTransformation2DTest::normalizeRotation() {
    Object2D o;
    o.rotate(Deg(17.0f));
    o.normalizeRotation();
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f)));
}

}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::EuclideanMatrixTransformation2DTest)
