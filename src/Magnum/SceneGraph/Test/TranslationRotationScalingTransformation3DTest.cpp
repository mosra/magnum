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

#include "Magnum/SceneGraph/TranslationRotationScalingTransformation3D.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

typedef Object<TranslationRotationScalingTransformation3D> Object3D;
typedef Scene<TranslationRotationScalingTransformation3D> Scene3D;

struct TranslationRotationScalingTransformation3DTest: TestSuite::Tester {
    explicit TranslationRotationScalingTransformation3DTest();

    void fromMatrix();
    void toMatrix();
    void compose();
    void inverted();

    void defaults();
    void setTransformation();
    void setTransformationRotateALot();
    void resetTransformation();

    void translate();
    void rotate();
    void scale();
};

TranslationRotationScalingTransformation3DTest::TranslationRotationScalingTransformation3DTest() {
    addTests({&TranslationRotationScalingTransformation3DTest::fromMatrix,
              &TranslationRotationScalingTransformation3DTest::toMatrix,
              &TranslationRotationScalingTransformation3DTest::compose,
              &TranslationRotationScalingTransformation3DTest::inverted,

              &TranslationRotationScalingTransformation3DTest::defaults,
              &TranslationRotationScalingTransformation3DTest::setTransformation,
              &TranslationRotationScalingTransformation3DTest::setTransformationRotateALot,
              &TranslationRotationScalingTransformation3DTest::resetTransformation,

              &TranslationRotationScalingTransformation3DTest::translate,
              &TranslationRotationScalingTransformation3DTest::rotate,
              &TranslationRotationScalingTransformation3DTest::scale});
}

using namespace Math::Literals;

void TranslationRotationScalingTransformation3DTest::fromMatrix() {
    Matrix4 m = Matrix4::rotationX(17.0_degf)*Matrix4::translation({1.0f, -0.3f, 2.3f})*Matrix4::scaling({2.0f, 1.4f, -2.1f});
    CORRADE_COMPARE(Implementation::Transformation<TranslationRotationScalingTransformation3D>::fromMatrix(m), m);
}

void TranslationRotationScalingTransformation3DTest::toMatrix() {
    Matrix4 m = Matrix4::rotationX(17.0_degf)*Matrix4::translation({1.0f, -0.3f, 2.3f})*Matrix4::scaling({2.0f, 1.4f, -2.1f});
    CORRADE_COMPARE(Implementation::Transformation<TranslationRotationScalingTransformation3D>::toMatrix(m), m);
}

void TranslationRotationScalingTransformation3DTest::compose() {
    Matrix4 parent = Matrix4::rotationX(17.0_degf);
    Matrix4 child = Matrix4::translation({1.0f, -0.3f, 2.3f});
    CORRADE_COMPARE(Implementation::Transformation<TranslationRotationScalingTransformation3D>::compose(parent, child), parent*child);
}

void TranslationRotationScalingTransformation3DTest::inverted() {
    Matrix4 m = Matrix4::rotationX(17.0_degf)*Matrix4::translation({1.0f, -0.3f, 2.3f})*Matrix4::scaling({2.0f, 1.4f, -2.1f});
    CORRADE_COMPARE(Implementation::Transformation<TranslationRotationScalingTransformation3D>::inverted(m)*m, Matrix4());
}

void TranslationRotationScalingTransformation3DTest::defaults() {
    Object3D o;
    CORRADE_COMPARE(o.translation(), Vector3{});
    CORRADE_COMPARE(o.rotation(), Quaternion{});
    CORRADE_COMPARE(o.scaling(), Vector3{1.0f});
    CORRADE_COMPARE(o.transformationMatrix(), Matrix4{});
}

void TranslationRotationScalingTransformation3DTest::setTransformation() {
    /* Dirty after setting transformation */
    Object3D o;
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(
        Matrix4::translation({7.0f, -1.0f, 2.2f})*
        Matrix4::rotationX(17.0_degf)*
        Matrix4::scaling({1.5f, 0.5f, 3.0f}));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.translation(), (Vector3{7.0f, -1.0f, 2.2f}));
    CORRADE_COMPARE(o.rotation(), Quaternion::rotation(17.0_degf, Vector3::xAxis()));
    CORRADE_COMPARE(o.scaling(), (Vector3{1.5f, 0.5f, 3.0f}));
    CORRADE_COMPARE(o.transformationMatrix(),
        Matrix4::translation({7.0f, -1.0f, 2.2f})*
        Matrix4::rotationX(17.0_degf)*
        Matrix4::scaling({1.5f, 0.5f, 3.0f}));

    /* Scene cannot be transformed */
    Scene3D s;
    s.setClean();
    CORRADE_VERIFY(!s.isDirty());
    s.setTransformation(Matrix4::rotationX(17.0_degf));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Matrix4());
}

void TranslationRotationScalingTransformation3DTest::setTransformationRotateALot() {
    Object3D o;
    o.setTransformation(
        Matrix4::translation({7.0f, -1.0f, 2.2f})*
        Matrix4::rotationX(225.0_degf)*
        Matrix4::scaling({1.5f, 0.5f, 3.0f}));
    CORRADE_COMPARE(o.translation(), (Vector3{7.0f, -1.0f, 2.2f}));
    /* Rotation of more than 180° causes either the rotation matrix or scaling
       to contain negative signs, verify we get a proper matrix back again */
    CORRADE_COMPARE(o.rotation(), Quaternion::rotation(225.0_degf, Vector3::xAxis()));
    CORRADE_COMPARE(o.scaling(), (Vector3{1.5f, 0.5f, 3.0f}));
    CORRADE_COMPARE(o.transformationMatrix(),
        Matrix4::translation({7.0f, -1.0f, 2.2f})*
        Matrix4::rotationX(225.0_degf)*
        Matrix4::scaling({1.5f, 0.5f, 3.0f}));
}

void TranslationRotationScalingTransformation3DTest::resetTransformation() {
    Object3D o;
    o.rotateX(17.0_degf);
    CORRADE_VERIFY(o.transformationMatrix() != Matrix4());
    o.resetTransformation();
    CORRADE_COMPARE(o.translation(), Vector3{});
    CORRADE_COMPARE(o.rotation(), Quaternion{});
    CORRADE_COMPARE(o.scaling(), Vector3{1.0f});
    CORRADE_COMPARE(o.transformationMatrix(), Matrix4());
}

void TranslationRotationScalingTransformation3DTest::translate() {
    {
        Object3D o;
        o.setTransformation(Matrix4::rotationX(17.0_degf));
        o.translate({1.0f, -0.3f, 2.3f})
         .translate({1.0f, 0.1f, 0.2f});
        CORRADE_COMPARE(o.translation(), (Vector3{2.0f, -0.2f, 2.5f}));
        CORRADE_COMPARE(o.rotation(), Quaternion::rotation(17.0_degf, Vector3::xAxis()));
        CORRADE_COMPARE(o.scaling(), Vector3{1.0f});
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix4::translation({1.0f, 0.1f, 0.2f})*
            Matrix4::translation({1.0f, -0.3f, 2.3f})*
            Matrix4::rotationX(17.0_degf));
    } {
        Object3D o;
        o.setTransformation(Matrix4::rotationX(17.0_degf));
        o.translateLocal({1.0f, -0.3f, 2.3f})
         .translateLocal({1.0f, 0.1f, 0.2f});
        CORRADE_COMPARE(o.translation(), (Vector3{2.0f, -0.2f, 2.5f}));
        CORRADE_COMPARE(o.rotation(), Quaternion::rotation(17.0_degf, Vector3::xAxis()));
        CORRADE_COMPARE(o.scaling(), Vector3{1.0f});
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix4::translation({1.0f, -0.3f, 2.3f})*
            Matrix4::translation({1.0f, 0.1f, 0.2f})*
            Matrix4::rotationX(17.0_degf));
    }
}

void TranslationRotationScalingTransformation3DTest::rotate() {
    {
        Object3D o;
        o.setTransformation(Matrix4::translation({1.0f, -0.3f, 2.3f}));
        o.rotateX(17.0_degf)
         .rotateY(25.0_degf)
         .rotateZ(-23.0_degf)
         .rotate(96.0_degf, Vector3{1.0f/Constants::sqrt3()});
        CORRADE_COMPARE(o.translation(), (Vector3{1.0f, -0.3f, 2.3f}));
        CORRADE_COMPARE(o.rotation(),
            Quaternion::rotation(96.0_degf, Vector3{1.0f/Constants::sqrt3()})*
            Quaternion::rotation(-23.0_degf, Vector3::zAxis())*
            Quaternion::rotation(25.0_degf, Vector3::yAxis())*
            Quaternion::rotation(17.0_degf, Vector3::xAxis()));
        CORRADE_COMPARE(o.scaling(), Vector3{1.0f});
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix4::translation({1.0f, -0.3f, 2.3f})*
            Matrix4::rotation(96.0_degf, Vector3{1.0f/Constants::sqrt3()})*
            Matrix4::rotationZ(-23.0_degf)*
            Matrix4::rotationY(25.0_degf)*
            Matrix4::rotationX(17.0_degf));
    } {
        Object3D o;
        o.setTransformation(Matrix4::translation({1.0f, -0.3f, 2.3f}));
        o.rotateXLocal(17.0_degf)
         .rotateYLocal(25.0_degf)
         .rotateZLocal(-23.0_degf)
         .rotateLocal(96.0_degf, Vector3{1.0f/Constants::sqrt3()});
        CORRADE_COMPARE(o.translation(), (Vector3{1.0f, -0.3f, 2.3f}));
        CORRADE_COMPARE(o.rotation(),
            Quaternion::rotation(17.0_degf, Vector3::xAxis())*
            Quaternion::rotation(25.0_degf, Vector3::yAxis())*
            Quaternion::rotation(-23.0_degf, Vector3::zAxis())*
            Quaternion::rotation(96.0_degf, Vector3(1.0f/Constants::sqrt3())));
        CORRADE_COMPARE(o.scaling(), Vector3{1.0f});
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix4::translation({1.0f, -0.3f, 2.3f})*
            Matrix4::rotationX(17.0_degf)*
            Matrix4::rotationY(25.0_degf)*
            Matrix4::rotationZ(-23.0_degf)*
            Matrix4::rotation(96.0_degf, Vector3{1.0f/Constants::sqrt3()}));
    }
}

void TranslationRotationScalingTransformation3DTest::scale() {
    {
        Object3D o;
        o.setTransformation(Matrix4::rotationX(17.0_degf));
        o.scale({1.0f, -0.3f, 2.3f})
         .scale({0.5f, 1.1f, 2.0f});
        CORRADE_COMPARE(o.translation(), Vector3{});
        CORRADE_COMPARE(o.rotation(), Quaternion::rotation(17.0_degf, Vector3::xAxis()));
        CORRADE_COMPARE(o.scaling(), (Vector3{0.5f, -0.33f, 4.6f}));
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix4::rotationX(17.0_degf)*
            Matrix4::scaling({0.5f, 1.1f, 2.0f})*
            Matrix4::scaling({1.0f, -0.3f, 2.3f}));
    } {
        Object3D o;
        o.setTransformation(Matrix4::rotationX(17.0_degf));
        o.scaleLocal({1.0f, -0.3f, 2.3f})
         .scaleLocal({0.5f, 1.1f, 2.0f});
        CORRADE_COMPARE(o.translation(), Vector3{});
        CORRADE_COMPARE(o.rotation(), Quaternion::rotation(17.0_degf, Vector3::xAxis()));
        CORRADE_COMPARE(o.scaling(), (Vector3{0.5f, -0.33f, 4.6f}));
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix4::rotationX(17.0_degf)*
            Matrix4::scaling({1.0f, -0.3f, 2.3f})*
            Matrix4::scaling({0.5f, 1.1f, 2.0f}));
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::TranslationRotationScalingTransformation3DTest)
