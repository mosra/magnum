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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Complex.h"
#include "Magnum/SceneGraph/RigidMatrixTransformation2D.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

typedef Object<RigidMatrixTransformation2D> Object2D;
typedef Scene<RigidMatrixTransformation2D> Scene2D;

struct RigidMatrixTransformation2DTest: TestSuite::Tester {
    explicit RigidMatrixTransformation2DTest();

    void fromMatrix();
    void fromMatrixInvalid();
    void toMatrix();
    void compose();
    void inverted();

    void setTransformation();
    void setTransformationInvalid();
    void resetTransformation();
    void transform();
    void transformInvalid();
    void translate();
    void rotate();
    void reflect();
    void normalizeRotation();
};

RigidMatrixTransformation2DTest::RigidMatrixTransformation2DTest() {
    addTests({&RigidMatrixTransformation2DTest::fromMatrix,
              &RigidMatrixTransformation2DTest::fromMatrixInvalid,
              &RigidMatrixTransformation2DTest::toMatrix,
              &RigidMatrixTransformation2DTest::compose,
              &RigidMatrixTransformation2DTest::inverted,

              &RigidMatrixTransformation2DTest::setTransformation,
              &RigidMatrixTransformation2DTest::setTransformationInvalid,
              &RigidMatrixTransformation2DTest::resetTransformation,
              &RigidMatrixTransformation2DTest::transform,
              &RigidMatrixTransformation2DTest::transformInvalid,
              &RigidMatrixTransformation2DTest::translate,
              &RigidMatrixTransformation2DTest::rotate,
              &RigidMatrixTransformation2DTest::reflect,
              &RigidMatrixTransformation2DTest::normalizeRotation});
}

using namespace Math::Literals;

void RigidMatrixTransformation2DTest::fromMatrix() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<RigidMatrixTransformation2D>::fromMatrix(m), m);
}

void RigidMatrixTransformation2DTest::fromMatrixInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Implementation::Transformation<RigidMatrixTransformation2D>::fromMatrix(Matrix3::scaling(Vector2(4.0f)));
    CORRADE_COMPARE(out.str(), "SceneGraph::RigidMatrixTransformation2D: the matrix doesn't represent rigid transformation\n");
}

void RigidMatrixTransformation2DTest::toMatrix() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<RigidMatrixTransformation2D>::toMatrix(m), m);
}

void RigidMatrixTransformation2DTest::compose() {
    Matrix3 parent = Matrix3::rotation(Deg(17.0f));
    Matrix3 child = Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<RigidMatrixTransformation2D>::compose(parent, child), parent*child);
}

void RigidMatrixTransformation2DTest::inverted() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<RigidMatrixTransformation2D>::inverted(m)*m, Matrix3());
}

void RigidMatrixTransformation2DTest::setTransformation() {
    Object2D o;

    /* Dirty after setting transformation */
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(Matrix3::rotation(Deg(17.0f)));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f)));

    /* Scene cannot be transformed */
    Scene2D s;
    s.setClean();
    s.setTransformation(Matrix3::rotation(Deg(17.0f)));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Matrix3());
}

void RigidMatrixTransformation2DTest::setTransformationInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Object2D o;

    /* Can't transform with non-rigid transformation */
    std::ostringstream out;
    Error redirectError{&out};
    o.setTransformation(Matrix3::scaling(Vector2(3.0f)));
    CORRADE_COMPARE(out.str(), "SceneGraph::RigidMatrixTransformation2D::setTransformation(): the matrix doesn't represent rigid transformation\n");
}

void RigidMatrixTransformation2DTest::resetTransformation() {
    Object2D o;
    o.setTransformation(Matrix3::rotation(Deg(17.0f)));
    CORRADE_VERIFY(o.transformationMatrix() != Matrix3());
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3());
}

void RigidMatrixTransformation2DTest::transform() {
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

void RigidMatrixTransformation2DTest::transformInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Can't transform with non-rigid transformation */
    Object2D o;
    std::ostringstream out;
    Error redirectError{&out};
    o.transform(Matrix3::scaling(Vector2(3.0f)));
    CORRADE_COMPARE(out.str(), "SceneGraph::RigidMatrixTransformation2D::transform(): the matrix doesn't represent rigid transformation\n");
}

void RigidMatrixTransformation2DTest::translate() {
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

void RigidMatrixTransformation2DTest::rotate() {
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

void RigidMatrixTransformation2DTest::reflect() {
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

void RigidMatrixTransformation2DTest::normalizeRotation() {
    Object2D o;
    o.setTransformation(Matrix3::rotation(Deg(17.0f)));
    o.normalizeRotation();
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f)));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::RigidMatrixTransformation2DTest)
