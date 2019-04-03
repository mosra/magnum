/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/SceneGraph/RigidMatrixTransformation3D.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

typedef Object<RigidMatrixTransformation3D> Object3D;
typedef Scene<RigidMatrixTransformation3D> Scene3D;

struct RigidMatrixTransformation3DTest: TestSuite::Tester {
    explicit RigidMatrixTransformation3DTest();

    void fromMatrix();
    void toMatrix();
    void compose();
    void inverted();

    void setTransformation();
    void resetTransformation();
    void transform();
    void translate();
    void rotate();
    void reflect();
    void normalizeRotation();
};

RigidMatrixTransformation3DTest::RigidMatrixTransformation3DTest() {
    addTests({&RigidMatrixTransformation3DTest::fromMatrix,
              &RigidMatrixTransformation3DTest::toMatrix,
              &RigidMatrixTransformation3DTest::compose,
              &RigidMatrixTransformation3DTest::inverted,

              &RigidMatrixTransformation3DTest::setTransformation,
              &RigidMatrixTransformation3DTest::resetTransformation,
              &RigidMatrixTransformation3DTest::transform,
              &RigidMatrixTransformation3DTest::translate,
              &RigidMatrixTransformation3DTest::rotate,
              &RigidMatrixTransformation3DTest::reflect,
              &RigidMatrixTransformation3DTest::normalizeRotation});
}

void RigidMatrixTransformation3DTest::fromMatrix() {
    std::ostringstream o;
    Error redirectError{&o};
    Implementation::Transformation<RigidMatrixTransformation3D>::fromMatrix(Matrix4::scaling(Vector3(4.0f)));
    CORRADE_COMPARE(o.str(), "SceneGraph::RigidMatrixTransformation3D: the matrix doesn't represent rigid transformation\n");

    Matrix4 m = Matrix4::rotationX(Deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f});
    CORRADE_COMPARE(Implementation::Transformation<RigidMatrixTransformation3D>::fromMatrix(m), m);
}

void RigidMatrixTransformation3DTest::toMatrix() {
    Matrix4 m = Matrix4::rotationX(Deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f});
    CORRADE_COMPARE(Implementation::Transformation<RigidMatrixTransformation3D>::toMatrix(m), m);
}

void RigidMatrixTransformation3DTest::compose() {
    Matrix4 parent = Matrix4::rotationX(Deg(17.0f));
    Matrix4 child = Matrix4::translation({1.0f, -0.3f, 2.3f});
    CORRADE_COMPARE(Implementation::Transformation<RigidMatrixTransformation3D>::compose(parent, child), parent*child);
}

void RigidMatrixTransformation3DTest::inverted() {
    Matrix4 m = Matrix4::rotationX(Deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f});
    CORRADE_COMPARE(Implementation::Transformation<RigidMatrixTransformation3D>::inverted(m)*m, Matrix4());
}

void RigidMatrixTransformation3DTest::setTransformation() {
    Object3D o;

    /* Can't transform with non-rigid transformation */
    std::ostringstream out;
    Error redirectError{&out};
    o.setTransformation(Matrix4::scaling(Vector3(3.0f)));
    CORRADE_COMPARE(out.str(), "SceneGraph::RigidMatrixTransformation3D::setTransformation(): the matrix doesn't represent rigid transformation\n");

    /* Dirty after setting transformation */
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(Matrix4::rotationX(Deg(17.0f)));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Matrix4::rotationX(Deg(17.0f)));

    /* Scene cannot be transformed */
    Scene3D s;
    s.setClean();
    CORRADE_VERIFY(!s.isDirty());
    s.setTransformation(Matrix4::rotationX(Deg(17.0f)));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Matrix4());
}

void RigidMatrixTransformation3DTest::resetTransformation() {
    Object3D o;
    o.setTransformation(Matrix4::rotationX(Deg(17.0f)));
    CORRADE_VERIFY(o.transformationMatrix() != Matrix4());
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Matrix4());
}

void RigidMatrixTransformation3DTest::transform() {
    {
        /* Can't transform with non-rigid transformation */
        Object3D o;
        std::ostringstream out;
        Error redirectError{&out};
        o.transform(Matrix4::scaling(Vector3(3.0f)));
        CORRADE_COMPARE(out.str(), "SceneGraph::RigidMatrixTransformation3D::transform(): the matrix doesn't represent rigid transformation\n");
    } {
        Object3D o;
        o.setTransformation(Matrix4::rotationX(Deg(17.0f)));
        o.transform(Matrix4::translation({1.0f, -0.3f, 2.3f}));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix4::translation({1.0f, -0.3f, 2.3f})*Matrix4::rotationX(Deg(17.0f)));
    } {
        Object3D o;
        o.setTransformation(Matrix4::rotationX(Deg(17.0f)));
        o.transformLocal(Matrix4::translation({1.0f, -0.3f, 2.3f}));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix4::rotationX(Deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f}));
    }
}

void RigidMatrixTransformation3DTest::translate() {
    {
        Object3D o;
        o.setTransformation(Matrix4::rotationX(Deg(17.0f)));
        o.translate({1.0f, -0.3f, 2.3f});
        CORRADE_COMPARE(o.transformationMatrix(), Matrix4::translation({1.0f, -0.3f, 2.3f})*Matrix4::rotationX(Deg(17.0f)));
    } {
        Object3D o;
        o.setTransformation(Matrix4::rotationX(Deg(17.0f)));
        o.translateLocal({1.0f, -0.3f, 2.3f});
        CORRADE_COMPARE(o.transformationMatrix(), Matrix4::rotationX(Deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f}));
    }
}

void RigidMatrixTransformation3DTest::rotate() {
    {
        Object3D o;
        o.setTransformation(Matrix4::translation({1.0f, -0.3f, 2.3f}));
        o.rotateX(Deg(17.0f))
            .rotateY(Deg(25.0f))
            .rotateZ(Deg(-23.0f))
            .rotate(Deg(96.0f), Vector3(1.0f/Constants::sqrt3()));
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix4::rotation(Deg(96.0f), Vector3(1.0f/Constants::sqrt3()))*
            Matrix4::rotationZ(Deg(-23.0f))*
            Matrix4::rotationY(Deg(25.0f))*
            Matrix4::rotationX(Deg(17.0f))*
            Matrix4::translation({1.0f, -0.3f, 2.3f}));
    } {
        Object3D o;
        o.setTransformation(Matrix4::translation({1.0f, -0.3f, 2.3f}));
        o.rotateXLocal(Deg(17.0f))
            .rotateYLocal(Deg(25.0f))
            .rotateZLocal(Deg(-23.0f))
            .rotateLocal(Deg(96.0f), Vector3(1.0f/Constants::sqrt3()));
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix4::translation({1.0f, -0.3f, 2.3f})*
            Matrix4::rotationX(Deg(17.0f))*
            Matrix4::rotationY(Deg(25.0f))*
            Matrix4::rotationZ(Deg(-23.0f))*
            Matrix4::rotation(Deg(96.0f), Vector3(1.0f/Constants::sqrt3())));
    }
}

void RigidMatrixTransformation3DTest::reflect() {
    {
        Object3D o;
        o.setTransformation(Matrix4::rotationX(Deg(17.0f)));
        o.reflect(Vector3(-1.0f/Constants::sqrt3()));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix4::reflection(Vector3(-1.0f/Constants::sqrt3()))*Matrix4::rotationX(Deg(17.0f)));
    } {
        Object3D o;
        o.setTransformation(Matrix4::rotationX(Deg(17.0f)));
        o.reflectLocal(Vector3(-1.0f/Constants::sqrt3()));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix4::rotationX(Deg(17.0f))*Matrix4::reflection(Vector3(-1.0f/Constants::sqrt3())));
    }
}

void RigidMatrixTransformation3DTest::normalizeRotation() {
    Object3D o;
    o.setTransformation(Matrix4::rotationX(Deg(17.0f)));
    o.normalizeRotation();
    CORRADE_COMPARE(o.transformationMatrix(), Matrix4::rotationX(Deg(17.0f)));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::RigidMatrixTransformation3DTest)
