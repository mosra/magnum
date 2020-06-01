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

#include "Magnum/SceneGraph/TranslationRotationScalingTransformation2D.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

typedef Object<TranslationRotationScalingTransformation2D> Object2D;
typedef Scene<TranslationRotationScalingTransformation2D> Scene2D;

struct TranslationRotationScalingTransformation2DTest: TestSuite::Tester {
    explicit TranslationRotationScalingTransformation2DTest();

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

TranslationRotationScalingTransformation2DTest::TranslationRotationScalingTransformation2DTest() {
    addTests({&TranslationRotationScalingTransformation2DTest::fromMatrix,
              &TranslationRotationScalingTransformation2DTest::toMatrix,
              &TranslationRotationScalingTransformation2DTest::compose,
              &TranslationRotationScalingTransformation2DTest::inverted,

              &TranslationRotationScalingTransformation2DTest::defaults,
              &TranslationRotationScalingTransformation2DTest::setTransformation,
              &TranslationRotationScalingTransformation2DTest::setTransformationRotateALot,
              &TranslationRotationScalingTransformation2DTest::resetTransformation,

              &TranslationRotationScalingTransformation2DTest::translate,
              &TranslationRotationScalingTransformation2DTest::rotate,
              &TranslationRotationScalingTransformation2DTest::scale});
}

using namespace Math::Literals;

void TranslationRotationScalingTransformation2DTest::fromMatrix() {
    Matrix3 m = Matrix3::rotation(17.0_degf)*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<TranslationRotationScalingTransformation2D>::fromMatrix(m), m);
}

void TranslationRotationScalingTransformation2DTest::toMatrix() {
    Matrix3 m = Matrix3::rotation(17.0_degf)*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<TranslationRotationScalingTransformation2D>::toMatrix(m), m);
}

void TranslationRotationScalingTransformation2DTest::compose() {
    Matrix3 parent = Matrix3::rotation(17.0_degf);
    Matrix3 child = Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<TranslationRotationScalingTransformation2D>::compose(parent, child), parent*child);
}

void TranslationRotationScalingTransformation2DTest::inverted() {
    Matrix3 m = Matrix3::rotation(17.0_degf)*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<TranslationRotationScalingTransformation2D>::inverted(m)*m, Matrix3());
}

void TranslationRotationScalingTransformation2DTest::defaults() {
    Object2D o;
    CORRADE_COMPARE(o.translation(), Vector2{});
    CORRADE_COMPARE(o.rotation(), Complex{});
    CORRADE_COMPARE(o.scaling(), Vector2{1.0f});
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3{});
}

void TranslationRotationScalingTransformation2DTest::setTransformation() {
    /* Dirty after setting transformation */
    Object2D o;
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(
        Matrix3::translation({7.0f, -1.0f})*
        Matrix3::rotation(17.0_degf)*
        Matrix3::scaling({1.5f, 0.5f}));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.translation(), (Vector2{7.0f, -1.0f}));
    CORRADE_COMPARE(o.rotation(), Complex::rotation(17.0_degf));
    CORRADE_COMPARE(o.scaling(), (Vector2{1.5f, 0.5f}));
    CORRADE_COMPARE(o.transformationMatrix(),
        Matrix3::translation({7.0f, -1.0f})*
        Matrix3::rotation(17.0_degf)*
        Matrix3::scaling({1.5f, 0.5f}));

    /* Scene cannot be transformed */
    Scene2D s;
    s.setClean();
    CORRADE_VERIFY(!s.isDirty());
    s.setTransformation(Matrix3::rotation(17.0_degf));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Matrix3());
}

void TranslationRotationScalingTransformation2DTest::setTransformationRotateALot() {
    Object2D o;
    o.setTransformation(
        Matrix3::translation({7.0f, -1.0f})*
        Matrix3::rotation(225.0_degf)*
        Matrix3::scaling({1.5f, 0.5f}));
    CORRADE_COMPARE(o.translation(), (Vector2{7.0f, -1.0f}));
    /* Rotation of more than 180° causes either the rotation matrix or scaling
       to contain negative signs, verify we get a proper matrix back again */
    CORRADE_COMPARE(o.rotation(), Complex::rotation(225.0_degf));
    CORRADE_COMPARE(o.scaling(), (Vector2{1.5f, 0.5f}));
    CORRADE_COMPARE(o.transformationMatrix(),
        Matrix3::translation({7.0f, -1.0f})*
        Matrix3::rotation(225.0_degf)*
        Matrix3::scaling({1.5f, 0.5f}));
}

void TranslationRotationScalingTransformation2DTest::resetTransformation() {
    Object2D o;
    o.rotate(17.0_degf);
    CORRADE_VERIFY(o.transformationMatrix() != Matrix3());
    o.resetTransformation();
    CORRADE_COMPARE(o.translation(), Vector2{});
    CORRADE_COMPARE(o.rotation(), Complex{});
    CORRADE_COMPARE(o.scaling(), Vector2{1.0f});
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3());
}

void TranslationRotationScalingTransformation2DTest::translate() {
    {
        Object2D o;
        o.setTransformation(Matrix3::rotation(17.0_degf));
        o.translate({1.0f, -0.3f})
         .translate({1.0f, 0.1f});
        CORRADE_COMPARE(o.translation(), (Vector2{2.0f, -0.2f}));
        CORRADE_COMPARE(o.rotation(), Complex::rotation(17.0_degf));
        CORRADE_COMPARE(o.scaling(), Vector2{1.0f});
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix3::translation({1.0f, 0.1f})*
            Matrix3::translation({1.0f, -0.3f})*
            Matrix3::rotation(17.0_degf));
    } {
        Object2D o;
        o.setTransformation(Matrix3::rotation(17.0_degf));
        o.translateLocal({1.0f, -0.3f})
         .translateLocal({1.0f, 0.1f});
        CORRADE_COMPARE(o.translation(), (Vector2{2.0f, -0.2f}));
        CORRADE_COMPARE(o.rotation(), Complex::rotation(17.0_degf));
        CORRADE_COMPARE(o.scaling(), Vector2{1.0f});
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix3::translation({1.0f, -0.3f})*
            Matrix3::translation({1.0f, 0.1f})*
            Matrix3::rotation(17.0_degf));
    }
}

void TranslationRotationScalingTransformation2DTest::rotate() {
    {
        Object2D o;
        o.setTransformation(Matrix3::translation({1.0f, -0.3f}));
        o.rotate(Complex::rotation(17.0_degf))
         .rotate(-96.0_degf);
        CORRADE_COMPARE(o.translation(), (Vector2{1.0f, -0.3f}));
        CORRADE_COMPARE(o.rotation(), Complex::rotation(-79.0_degf));
        CORRADE_COMPARE(o.scaling(), Vector2{1.0f});
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix3::translation({1.0f, -0.3f})*
            Matrix3::rotation(-96.0_degf)*
            Matrix3::rotation(17.0_degf));
    } {
        Object2D o;
        o.setTransformation(Matrix3::translation({1.0f, -0.3f}));
        o.rotateLocal(Complex::rotation(17.0_degf))
         .rotateLocal(-96.0_degf);
        CORRADE_COMPARE(o.translation(), (Vector2{1.0f, -0.3f}));
        CORRADE_COMPARE(o.rotation(), Complex::rotation(-79.0_degf));
        CORRADE_COMPARE(o.scaling(), Vector2{1.0f});
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix3::translation({1.0f, -0.3f})*
            Matrix3::rotation(17.0_degf)*
            Matrix3::rotation(-96.0_degf));
    }
}

void TranslationRotationScalingTransformation2DTest::scale() {
    {
        Object2D o;
        o.setTransformation(Matrix3::rotation(17.0_degf));
        o.scale({1.0f, -0.3f})
         .scale({0.5f, 1.1f});
        CORRADE_COMPARE(o.translation(), Vector2{});
        CORRADE_COMPARE(o.rotation(), Complex::rotation(17.0_degf));
        CORRADE_COMPARE(o.scaling(), (Vector2{0.5f, -0.33f}));
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix3::rotation(17.0_degf)*
            Matrix3::scaling({0.5f, 1.1f})*
            Matrix3::scaling({1.0f, -0.3f}));
    } {
        Object2D o;
        o.setTransformation(Matrix3::rotation(17.0_degf));
        o.scaleLocal({1.0f, -0.3f})
         .scaleLocal({0.5f, 1.1f});
        CORRADE_COMPARE(o.translation(), Vector2{});
        CORRADE_COMPARE(o.rotation(), Complex::rotation(17.0_degf));
        CORRADE_COMPARE(o.scaling(), (Vector2{0.5f, -0.33f}));
        CORRADE_COMPARE(o.transformationMatrix(),
            Matrix3::rotation(17.0_degf)*
            Matrix3::scaling({1.0f, -0.3f})*
            Matrix3::scaling({0.5f, 1.1f}));
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::TranslationRotationScalingTransformation2DTest)
