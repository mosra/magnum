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
#include "SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test {

typedef Object<EuclideanMatrixTransformation3D<>> Object3D;
typedef Scene<EuclideanMatrixTransformation3D<>> Scene3D;

class EuclideanMatrixTransformation3DTest: public Corrade::TestSuite::Tester {
    public:
        explicit EuclideanMatrixTransformation3DTest();

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

EuclideanMatrixTransformation3DTest::EuclideanMatrixTransformation3DTest() {
    addTests(&EuclideanMatrixTransformation3DTest::fromMatrix,
             &EuclideanMatrixTransformation3DTest::toMatrix,
             &EuclideanMatrixTransformation3DTest::compose,
             &EuclideanMatrixTransformation3DTest::inverted,

             &EuclideanMatrixTransformation3DTest::setTransformation,
             &EuclideanMatrixTransformation3DTest::translate,
             &EuclideanMatrixTransformation3DTest::rotate,
             &EuclideanMatrixTransformation3DTest::reflect,
             &EuclideanMatrixTransformation3DTest::normalizeRotation);
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

void EuclideanMatrixTransformation3DTest::setTransformation() {
    /* Dirty after setting transformation */
    Object3D o;
    o.setClean();
    o.rotateX(deg(17.0f));
    CORRADE_VERIFY(o.isDirty());

    /* Scene cannot be transformed */
    Scene3D s;
    s.setClean();
    s.rotateX(deg(17.0f));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Matrix4());
}

void EuclideanMatrixTransformation3DTest::translate() {
    {
        Object3D o;
        o.rotateX(deg(17.0f));
        o.translate({1.0f, -0.3f, 2.3f});
        CORRADE_COMPARE(o.transformationMatrix(), Matrix4::translation({1.0f, -0.3f, 2.3f})*Matrix4::rotationX(deg(17.0f)));
    } {
        Object3D o;
        o.rotateX(deg(17.0f));
        o.translate({1.0f, -0.3f, 2.3f}, TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix4::rotationX(deg(17.0f))*Matrix4::translation({1.0f, -0.3f, 2.3f}));
    }
}

void EuclideanMatrixTransformation3DTest::rotate() {
    {
        Object3D o;
        o.translate({1.0f, -0.3f, 2.3f});
        o.rotateX(deg(17.0f))
            ->rotateY(deg(25.0f))
            ->rotateZ(deg(-23.0f))
            ->rotate(deg(96.0f), Vector3(1.0f/Constants::sqrt3()));
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix4::rotation(deg(96.0f), Vector3(1.0f/Constants::sqrt3()))*
            Matrix4::rotationZ(deg(-23.0f))*
            Matrix4::rotationY(deg(25.0f))*
            Matrix4::rotationX(deg(17.0f))*
            Matrix4::translation({1.0f, -0.3f, 2.3f}));
    } {
        Object3D o;
        o.translate({1.0f, -0.3f, 2.3f});
        o.rotateX(deg(17.0f), TransformationType::Local)
            ->rotateY(deg(25.0f), TransformationType::Local)
            ->rotateZ(deg(-23.0f), TransformationType::Local)
            ->rotate(deg(96.0f), Vector3(1.0f/Constants::sqrt3()), TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix4::translation({1.0f, -0.3f, 2.3f})*
            Matrix4::rotationX(deg(17.0f))*
            Matrix4::rotationY(deg(25.0f))*
            Matrix4::rotationZ(deg(-23.0f))*
            Matrix4::rotation(deg(96.0f), Vector3(1.0f/Constants::sqrt3())));
    }
}

void EuclideanMatrixTransformation3DTest::reflect() {
    {
        Object3D o;
        o.rotateX(deg(17.0f));
        o.reflect(Vector3(-1.0f/Constants::sqrt3()));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix4::reflection(Vector3(-1.0f/Constants::sqrt3()))*Matrix4::rotationX(deg(17.0f)));
    } {
        Object3D o;
        o.rotateX(deg(17.0f));
        o.reflect(Vector3(-1.0f/Constants::sqrt3()), TransformationType::Local);
        CORRADE_COMPARE(o.transformationMatrix(), Matrix4::rotationX(deg(17.0f))*Matrix4::reflection(Vector3(-1.0f/Constants::sqrt3())));
    }
}

void EuclideanMatrixTransformation3DTest::normalizeRotation() {
    Object3D o;
    o.rotateX(deg(17.0f));
    o.normalizeRotation();
    CORRADE_COMPARE(o.transformationMatrix(), Matrix4::rotationX(deg(17.0f)));
}

}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::EuclideanMatrixTransformation3DTest)
