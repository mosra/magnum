/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Complex.h"
#include "Magnum/SceneGraph/MatrixTransformation2D.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

typedef Object<MatrixTransformation2D> Object2D;
typedef Scene<MatrixTransformation2D> Scene2D;

struct MatrixTransformation2DTest: TestSuite::Tester {
    explicit MatrixTransformation2DTest();

    void fromMatrix();
    void toMatrix();
    void compose();
    void inverted();

    void setTransformation();
    void resetTransformation();
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
              &MatrixTransformation2DTest::resetTransformation,
              &MatrixTransformation2DTest::transform,
              &MatrixTransformation2DTest::translate,
              &MatrixTransformation2DTest::rotate,
              &MatrixTransformation2DTest::scale,
              &MatrixTransformation2DTest::reflect});
}

using namespace Math::Literals;

void MatrixTransformation2DTest::fromMatrix() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<MatrixTransformation2D>::fromMatrix(m), m);
}

void MatrixTransformation2DTest::toMatrix() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<MatrixTransformation2D>::toMatrix(m), m);
}

void MatrixTransformation2DTest::compose() {
    Matrix3 parent = Matrix3::rotation(Deg(17.0f));
    Matrix3 child = Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<MatrixTransformation2D>::compose(parent, child), parent*child);
}

void MatrixTransformation2DTest::inverted() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<MatrixTransformation2D>::inverted(m)*m, Matrix3());
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

void MatrixTransformation2DTest::resetTransformation() {
    Object2D o;
    o.rotate(Deg(17.0f));
    CORRADE_VERIFY(o.transformationMatrix() != Matrix3());
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3());
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
        o.transformLocal(Matrix3::translation({1.0f, -0.3f}));
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
        o.translateLocal({1.0f, -0.3f});
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f}));
    }
}

void MatrixTransformation2DTest::rotate() {
    {
        Object2D o;
        o.setTransformation(Matrix3::translation({1.0f, -0.3f}))
         .rotate(Complex::rotation(7.0_degf))
         .rotate(10.0_degf);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f}));
    } {
        Object2D o;
        o.setTransformation(Matrix3::translation({1.0f, -0.3f}))
         .rotateLocal(Complex::rotation(7.0_degf))
         .rotateLocal(10.0_degf);
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
        o.scaleLocal({1.0f, -0.3f});
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
        o.reflectLocal(Vector2(-1.0f/Constants::sqrt2()));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::reflection(Vector2(-1.0f/Constants::sqrt2())));
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::MatrixTransformation2DTest)
