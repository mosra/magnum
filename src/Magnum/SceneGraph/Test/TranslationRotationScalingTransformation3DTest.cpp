/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/TranslationRotationScalingTransformation3D.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct TranslationRotationScalingTransformation3DTest: TestSuite::Tester {
    explicit TranslationRotationScalingTransformation3DTest();

    template<class T> void fromMatrix();
    template<class T> void toMatrix();
    template<class T> void compose();
    template<class T> void inverted();

    template<class T> void defaults();
    template<class T> void setTransformation();
    template<class T> void setTransformationRotateALot();
    template<class T> void resetTransformation();

    template<class T> void translate();
    template<class T> void rotate();
    template<class T> void scale();
};

TranslationRotationScalingTransformation3DTest::TranslationRotationScalingTransformation3DTest() {
    addTests<TranslationRotationScalingTransformation3DTest>({
        &TranslationRotationScalingTransformation3DTest::fromMatrix<Float>,
        &TranslationRotationScalingTransformation3DTest::fromMatrix<Double>,
        &TranslationRotationScalingTransformation3DTest::toMatrix<Float>,
        &TranslationRotationScalingTransformation3DTest::toMatrix<Double>,
        &TranslationRotationScalingTransformation3DTest::compose<Float>,
        &TranslationRotationScalingTransformation3DTest::compose<Double>,
        &TranslationRotationScalingTransformation3DTest::inverted<Float>,
        &TranslationRotationScalingTransformation3DTest::inverted<Double>,

        &TranslationRotationScalingTransformation3DTest::defaults<Float>,
        &TranslationRotationScalingTransformation3DTest::defaults<Double>,
        &TranslationRotationScalingTransformation3DTest::setTransformation<Float>,
        &TranslationRotationScalingTransformation3DTest::setTransformation<Double>,
        &TranslationRotationScalingTransformation3DTest::setTransformationRotateALot<Float>,
        &TranslationRotationScalingTransformation3DTest::setTransformationRotateALot<Double>,
        &TranslationRotationScalingTransformation3DTest::resetTransformation<Float>,
        &TranslationRotationScalingTransformation3DTest::resetTransformation<Double>,

        &TranslationRotationScalingTransformation3DTest::translate<Float>,
        &TranslationRotationScalingTransformation3DTest::translate<Double>,
        &TranslationRotationScalingTransformation3DTest::rotate<Float>,
        &TranslationRotationScalingTransformation3DTest::rotate<Double>,
        &TranslationRotationScalingTransformation3DTest::scale<Float>,
        &TranslationRotationScalingTransformation3DTest::scale<Double>});
}

using namespace Math::Literals;

template<class T> using Object3D = Object<BasicTranslationRotationScalingTransformation3D<T>>;
template<class T> using Scene3D = Scene<BasicTranslationRotationScalingTransformation3D<T>>;

template<class T> void TranslationRotationScalingTransformation3DTest::fromMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::scaling({T(2.0), T(1.4), T(-2.1)});
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationRotationScalingTransformation3D<T>>::fromMatrix(m), m);
}

template<class T> void TranslationRotationScalingTransformation3DTest::toMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::scaling({T(2.0), T(1.4), T(-2.1)});
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationRotationScalingTransformation3D<T>>::toMatrix(m), m);
}

template<class T> void TranslationRotationScalingTransformation3DTest::compose() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> parent = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)});
    Math::Matrix4<T> child = Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationRotationScalingTransformation3D<T>>::compose(parent, child), parent*child);
}

template<class T> void TranslationRotationScalingTransformation3DTest::inverted() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::scaling({T(2.0), T(1.4), T(-2.1)});
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationRotationScalingTransformation3D<T>>::inverted(Math::Matrix4<T>{m})*Math::Matrix4<T>{m}, Math::Matrix4<T>{});
}

template<class T> void TranslationRotationScalingTransformation3DTest::defaults() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    CORRADE_COMPARE(o.translation(), Math::Vector3<T>{});
    CORRADE_COMPARE(o.rotation(), Math::Quaternion<T>{});
    CORRADE_COMPARE(o.scaling(), Math::Vector3<T>{T(1.0)});
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>{});
}

template<class T> void TranslationRotationScalingTransformation3DTest::setTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Dirty after setting transformation */
    Object3D<T> o;
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(
        Math::Matrix4<T>::translation({T(7.0), T(-1.0), T(2.2)})*
        Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*
        Math::Matrix4<T>::scaling({T(1.5), T(0.5), T(3.0)}));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.translation(), (Math::Vector3<T>{T(7.0), T(-1.0), T(2.2)}));
    CORRADE_COMPARE(o.rotation(), Math::Quaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
    CORRADE_COMPARE(o.scaling(), (Math::Vector3<T>{T(1.5), T(0.5), T(3.0)}));
    CORRADE_COMPARE(o.transformationMatrix(),
        Math::Matrix4<T>::translation({T(7.0), T(-1.0), T(2.2)})*
        Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*
        Math::Matrix4<T>::scaling({T(1.5), T(0.5), T(3.0)}));

    /* Scene cannot be transformed */
    Scene3D<T> s;
    s.setClean();
    CORRADE_VERIFY(!s.isDirty());
    s.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{17.0}));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Math::Matrix4<T>{});
}

template<class T> void TranslationRotationScalingTransformation3DTest::setTransformationRotateALot() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    o.setTransformation(
        Math::Matrix4<T>::translation({T(7.0), T(-1.0), T(2.2)})*
        Math::Matrix4<T>::rotationX(Math::Deg<T>{T(225.0)})*
        Math::Matrix4<T>::scaling({T(1.5), T(0.5), T(3.0)}));
    CORRADE_COMPARE(o.translation(), (Math::Vector3<T>{T(7.0), T(-1.0), T(2.2)}));
    /* Rotation of more than 180° causes either the rotation matrix or scaling
       to contain negative signs, verify we get a proper matrix back again */
    CORRADE_COMPARE(o.rotation(), Math::Quaternion<T>::rotation(Math::Deg<T>{T(225.0)}, Math::Vector3<T>::xAxis()));
    CORRADE_COMPARE(o.scaling(), (Math::Vector3<T>{T(1.5), T(0.5), T(3.0)}));
    CORRADE_COMPARE(o.transformationMatrix(),
        Math::Matrix4<T>::translation({T(7.0), T(-1.0), T(2.2)})*
        Math::Matrix4<T>::rotationX(Math::Deg<T>{T(225.0)})*
        Math::Matrix4<T>::scaling({T(1.5), T(0.5), T(3.0)}));
}

template<class T> void TranslationRotationScalingTransformation3DTest::resetTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    o.rotateX(Math::Deg<T>{17.0});
    CORRADE_VERIFY(o.transformationMatrix() != Math::Matrix4<T>{});
    o.resetTransformation();
    CORRADE_COMPARE(o.translation(), Math::Vector3<T>{});
    CORRADE_COMPARE(o.rotation(), Math::Quaternion<T>{});
    CORRADE_COMPARE(o.scaling(), Math::Vector3<T>{T(1.0)});
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>());
}

template<class T> void TranslationRotationScalingTransformation3DTest::translate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{17.0}));
        o.translate({T(1.0), T(-0.3), T(2.3)})
         .translate({T(1.0), T(0.1), T(0.2)});
        CORRADE_COMPARE(o.translation(), (Math::Vector3<T>{T(2.0), T(-0.2), T(2.5)}));
        CORRADE_COMPARE(o.rotation(), Math::Quaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
        CORRADE_COMPARE(o.scaling(), Math::Vector3<T>{T(1.0)});
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix4<T>::translation({T(1.0), T(0.1), T(0.2)})*
            Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*
            Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{17.0}));
        o.translateLocal({T(1.0), T(-0.3), T(2.3)})
         .translateLocal({T(1.0), T(0.1), T(0.2)});
        CORRADE_COMPARE(o.translation(), (Math::Vector3<T>{T(2.0), T(-0.2), T(2.5)}));
        CORRADE_COMPARE(o.rotation(), Math::Quaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
        CORRADE_COMPARE(o.scaling(), Math::Vector3<T>{T(1.0)});
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*
            Math::Matrix4<T>::translation({T(1.0), T(0.1), T(0.2)})*
            Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    }
}

template<class T> void TranslationRotationScalingTransformation3DTest::rotate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}));
        o.rotateX(Math::Deg<T>{T(17.0)})
         .rotateY(Math::Deg<T>{T(25.0)})
         .rotateZ(Math::Deg<T>{T(-23.0)})
         .rotate(Math::Deg<T>{T(96.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()});
        CORRADE_COMPARE(o.translation(), (Math::Vector3<T>{T(1.0), T(-0.3), T(2.3)}));
        CORRADE_COMPARE(o.rotation(),
            Math::Quaternion<T>::rotation(Math::Deg<T>{T(96.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()})*
            Math::Quaternion<T>::rotation(Math::Deg<T>{T(-23.0)}, Math::Vector3<T>::zAxis())*
            Math::Quaternion<T>::rotation(Math::Deg<T>{T(25.0)}, Math::Vector3<T>::yAxis())*
            Math::Quaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
        CORRADE_COMPARE(o.scaling(), Math::Vector3<T>{T(1.0)});
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*
            Math::Matrix4<T>::rotation(Math::Deg<T>{T(96.0)},Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()})*
            Math::Matrix4<T>::rotationZ(Math::Deg<T>{T(-23.0)})*
            Math::Matrix4<T>::rotationY(Math::Deg<T>{T(25.0)})*
            Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}));
        o.rotateXLocal(Math::Deg<T>{T(17.0)})
         .rotateYLocal(Math::Deg<T>{T(25.0)})
         .rotateZLocal(Math::Deg<T>{T(-23.0)})
         .rotateLocal(Math::Deg<T>{T(96.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()});
        CORRADE_COMPARE(o.translation(), (Math::Vector3<T>{T(1.0), T(-0.3), T(2.3)}));
        CORRADE_COMPARE(o.rotation(),
            Math::Quaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis())*
            Math::Quaternion<T>::rotation(Math::Deg<T>{T(25.0)}, Math::Vector3<T>::yAxis())*
            Math::Quaternion<T>::rotation(Math::Deg<T>{T(-23.0)}, Math::Vector3<T>::zAxis())*
            Math::Quaternion<T>::rotation(Math::Deg<T>{T(96.0)}, Math::Vector3<T>(T(1.0)/Math::Constants<T>::sqrt3())));
        CORRADE_COMPARE(o.scaling(), Math::Vector3<T>{T(1.0)});
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*
            Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*
            Math::Matrix4<T>::rotationY(Math::Deg<T>{T(25.0)})*
            Math::Matrix4<T>::rotationZ(Math::Deg<T>{T(-23.0)})*
            Math::Matrix4<T>::rotation(Math::Deg<T>{T(96.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()}));
    }
}

template<class T> void TranslationRotationScalingTransformation3DTest::scale() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{17.0}));
        o.scale({T(1.0), T(-0.3), T(2.3)})
         .scale({T(0.5), T(1.1), T(2.0)});
        CORRADE_COMPARE(o.translation(), Math::Vector3<T>{});
        CORRADE_COMPARE(o.rotation(), Math::Quaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
        CORRADE_COMPARE(o.scaling(), (Math::Vector3<T>{T(0.5), T(-0.33), T(4.6)}));
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*
            Math::Matrix4<T>::scaling({T(0.5), T(1.1), T(2.0)})*
            Math::Matrix4<T>::scaling({T(1.0), T(-0.3), T(2.3)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{17.0}));
        o.scaleLocal({T(1.0), T(-0.3), T(2.3)})
         .scaleLocal({T(0.5), T(1.1), T(2.0)});
        CORRADE_COMPARE(o.translation(), Math::Vector3<T>{});
        CORRADE_COMPARE(o.rotation(), Math::Quaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
        CORRADE_COMPARE(o.scaling(), (Math::Vector3<T>{T(0.5), T(-0.33), T(4.6)}));
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*
            Math::Matrix4<T>::scaling({T(1.0), T(-0.3), T(2.3)})*
            Math::Matrix4<T>::scaling({T(0.5), T(1.1), T(2.0)}));
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::TranslationRotationScalingTransformation3DTest)
