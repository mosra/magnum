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

#include "SceneGraph/MatrixTransformation2D.h"
#include "SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test {

typedef Object<MatrixTransformation2D<>> Object2D;
typedef Scene<MatrixTransformation2D<>> Scene2D;

class MatrixTransformation2DTest: public Corrade::TestSuite::Tester {
    public:
        explicit MatrixTransformation2DTest();

        void fromMatrix();
        void toMatrix();
        void compose();
        void inverted();

        void setTransformation();
        void transform();
        void translate();
        void rotate();
        void scale();
        void reflect();
};

MatrixTransformation2DTest::MatrixTransformation2DTest() {
    addTests({&MatrixTransformation2DTest::fromMatrix,
              &MatrixTransformation2DTest::toMatrix,
              &MatrixTransformation2DTest::compose,
              &MatrixTransformation2DTest::inverted,

              &MatrixTransformation2DTest::setTransformation,
              &MatrixTransformation2DTest::transform,
              &MatrixTransformation2DTest::translate,
              &MatrixTransformation2DTest::rotate,
              &MatrixTransformation2DTest::scale,
              &MatrixTransformation2DTest::reflect});
}

void MatrixTransformation2DTest::fromMatrix() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(MatrixTransformation2D<>::fromMatrix(m), m);
}

void MatrixTransformation2DTest::toMatrix() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(MatrixTransformation2D<>::toMatrix(m), m);
}

void MatrixTransformation2DTest::compose() {
    Matrix3 parent = Matrix3::rotation(Deg(17.0f));
    Matrix3 child = Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(MatrixTransformation2D<>::compose(parent, child), parent*child);
}

void MatrixTransformation2DTest::inverted() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(MatrixTransformation2D<>::inverted(m)*m, Matrix3());
}

void MatrixTransformation2DTest::setTransformation() {
    /* Dirty after setting transformation */
    Object2D o;
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(Matrix3::rotation(Deg(17.0f)));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f)));

    /* Scene cannot be transformed */
    Scene2D s;
    s.setClean();
    CORRADE_VERIFY(!s.isDirty());
    s.setTransformation(Matrix3::rotation(Deg(17.0f)));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Matrix3());
}

void MatrixTransformation2DTest::transform() {
    {
        Object2D o;
        o.setTransformation(Matrix3::rotation(Deg(17.0f)));
        o.transform(Matrix3::translation({1.0f, -0.3f}));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({1.0f, -0.3f})*Matrix3::rotation(Deg(17.0f)));
    } {
        Object2D o;
        o.setTransformation(Matrix3::rotation(Deg(17.0f)));
        o.transform(Matrix3::translation({1.0f, -0.3f}), TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f}));
    }
}

void MatrixTransformation2DTest::translate() {
    {
        Object2D o;
        o.setTransformation(Matrix3::rotation(Deg(17.0f)));
        o.translate({1.0f, -0.3f});
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({1.0f, -0.3f})*Matrix3::rotation(Deg(17.0f)));
    } {
        Object2D o;
        o.setTransformation(Matrix3::rotation(Deg(17.0f)));
        o.translate({1.0f, -0.3f}, TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f}));
    }
}

void MatrixTransformation2DTest::rotate() {
    {
        Object2D o;
        o.setTransformation(Matrix3::translation({1.0f, -0.3f}));
        o.rotate(Deg(17.0f));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f}));
    } {
        Object2D o;
        o.setTransformation(Matrix3::translation({1.0f, -0.3f}));
        o.rotate(Deg(17.0f), TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({1.0f, -0.3f})*Matrix3::rotation(Deg(17.0f)));
    }
}

void MatrixTransformation2DTest::scale() {
    {
        Object2D o;
        o.setTransformation(Matrix3::rotation(Deg(17.0f)));
        o.scale({1.0f, -0.3f});
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::scaling({1.0f, -0.3f})*Matrix3::rotation(Deg(17.0f)));
    } {
        Object2D o;
        o.setTransformation(Matrix3::rotation(Deg(17.0f)));
        o.scale({1.0f, -0.3f}, TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::scaling({1.0f, -0.3f}));
    }
}

void MatrixTransformation2DTest::reflect() {
    {
        Object2D o;
        o.setTransformation(Matrix3::rotation(Deg(17.0f)));
        o.reflect(Vector2(-1.0f/Constants::sqrt2()));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::reflection(Vector2(-1.0f/Constants::sqrt2()))*Matrix3::rotation(Deg(17.0f)));
    } {
        Object2D o;
        o.setTransformation(Matrix3::rotation(Deg(17.0f)));
        o.reflect(Vector2(-1.0f/Constants::sqrt2()), TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::reflection(Vector2(-1.0f/Constants::sqrt2())));
    }
}

}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::MatrixTransformation2DTest)
