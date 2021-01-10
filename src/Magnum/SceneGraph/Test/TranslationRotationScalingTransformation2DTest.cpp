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
#include "Magnum/SceneGraph/TranslationRotationScalingTransformation2D.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct TranslationRotationScalingTransformation2DTest: TestSuite::Tester {
    explicit TranslationRotationScalingTransformation2DTest();

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

TranslationRotationScalingTransformation2DTest::TranslationRotationScalingTransformation2DTest() {
    addTests<TranslationRotationScalingTransformation2DTest>({
        &TranslationRotationScalingTransformation2DTest::fromMatrix<Float>,
        &TranslationRotationScalingTransformation2DTest::fromMatrix<Double>,
        &TranslationRotationScalingTransformation2DTest::toMatrix<Float>,
        &TranslationRotationScalingTransformation2DTest::toMatrix<Double>,
        &TranslationRotationScalingTransformation2DTest::compose<Float>,
        &TranslationRotationScalingTransformation2DTest::compose<Double>,
        &TranslationRotationScalingTransformation2DTest::inverted<Float>,
        &TranslationRotationScalingTransformation2DTest::inverted<Double>,

        &TranslationRotationScalingTransformation2DTest::defaults<Float>,
        &TranslationRotationScalingTransformation2DTest::defaults<Double>,
        &TranslationRotationScalingTransformation2DTest::setTransformation<Float>,
        &TranslationRotationScalingTransformation2DTest::setTransformation<Double>,
        &TranslationRotationScalingTransformation2DTest::setTransformationRotateALot<Float>,
        &TranslationRotationScalingTransformation2DTest::setTransformationRotateALot<Double>,
        &TranslationRotationScalingTransformation2DTest::resetTransformation<Float>,
        &TranslationRotationScalingTransformation2DTest::resetTransformation<Double>,

        &TranslationRotationScalingTransformation2DTest::translate<Float>,
        &TranslationRotationScalingTransformation2DTest::translate<Double>,
        &TranslationRotationScalingTransformation2DTest::rotate<Float>,
        &TranslationRotationScalingTransformation2DTest::rotate<Double>,
        &TranslationRotationScalingTransformation2DTest::scale<Float>,
        &TranslationRotationScalingTransformation2DTest::scale<Double>});
}

using namespace Math::Literals;

template<class T> using Object2D = Object<BasicTranslationRotationScalingTransformation2D<T>>;
template<class T> using Scene2D = Scene<BasicTranslationRotationScalingTransformation2D<T>>;

template<class T> void TranslationRotationScalingTransformation2DTest::fromMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationRotationScalingTransformation2D<T>>::fromMatrix(m), m);
}

template<class T> void TranslationRotationScalingTransformation2DTest::toMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationRotationScalingTransformation2D<T>>::toMatrix(m), m);
}

template<class T> void TranslationRotationScalingTransformation2DTest::compose() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> parent = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)});
    Math::Matrix3<T> child = Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationRotationScalingTransformation2D<T>>::compose(parent, child), parent*child);
}

template<class T> void TranslationRotationScalingTransformation2DTest::inverted() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationRotationScalingTransformation2D<T>>::inverted(m)*m, Math::Matrix3<T>{});
}

template<class T> void TranslationRotationScalingTransformation2DTest::defaults() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    CORRADE_COMPARE(o.translation(), Math::Vector2<T>{});
    CORRADE_COMPARE(o.rotation(), Math::Complex<T>{});
    CORRADE_COMPARE(o.scaling(), Math::Vector2<T>{T(1.0)});
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>{});
}

template<class T> void TranslationRotationScalingTransformation2DTest::setTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Dirty after setting transformation */
    Object2D<T> o;
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(
        Math::Matrix3<T>::translation({T(7.0), T(-1.0)})*
        Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*
        Math::Matrix3<T>::scaling({T(1.5), T(0.5)}));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.translation(), (Math::Vector2<T>{T(7.0), T(-1.0)}));
    CORRADE_COMPARE(o.rotation(), Math::Complex<T>::rotation(Math::Deg<T>{T(17.0)}));
    CORRADE_COMPARE(o.scaling(), (Math::Vector2<T>{T(1.5), T(0.5)}));
    CORRADE_COMPARE(o.transformationMatrix(),
        Math::Matrix3<T>::translation({T(7.0), T(-1.0)})*
        Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*
        Math::Matrix3<T>::scaling({T(1.5), T(0.5)}));

    /* Scene cannot be transformed */
    Scene2D<T> s;
    s.setClean();
    CORRADE_VERIFY(!s.isDirty());
    s.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Math::Matrix3<T>{});
}

template<class T> void TranslationRotationScalingTransformation2DTest::setTransformationRotateALot() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    o.setTransformation(
        Math::Matrix3<T>::translation({T(7.0), T(-1.0)})*
        Math::Matrix3<T>::rotation(Math::Deg<T>{T(225.0)})*
        Math::Matrix3<T>::scaling({T(1.5), T(0.5)}));
    CORRADE_COMPARE(o.translation(), (Math::Vector2<T>{T(7.0), T(-1.0)}));
    /* Rotation of more than 180° causes either the rotation matrix or scaling
       to contain negative signs, verify we get a proper matrix back again */
    CORRADE_COMPARE(o.rotation(), Math::Complex<T>::rotation(Math::Deg<T>{T(225.0)}));
    CORRADE_COMPARE(o.scaling(), (Math::Vector2<T>{T(1.5), T(0.5)}));
    CORRADE_COMPARE(o.transformationMatrix(),
        Math::Matrix3<T>::translation({T(7.0), T(-1.0)})*
        Math::Matrix3<T>::rotation(Math::Deg<T>{T(225.0)})*
        Math::Matrix3<T>::scaling({T(1.5), T(0.5)}));
}

template<class T> void TranslationRotationScalingTransformation2DTest::resetTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    o.rotate(Math::Deg<T>{T(17.0)});
    CORRADE_VERIFY(o.transformationMatrix() != Math::Matrix3<T>{});
    o.resetTransformation();
    CORRADE_COMPARE(o.translation(), Math::Vector2<T>{});
    CORRADE_COMPARE(o.rotation(), Math::Complex<T>{});
    CORRADE_COMPARE(o.scaling(), Math::Vector2<T>{T(1.0)});
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>{});
}

template<class T> void TranslationRotationScalingTransformation2DTest::translate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.translate({T(1.0), T(-0.3)})
         .translate({T(1.0), T(0.1)});
        CORRADE_COMPARE(o.translation(), (Math::Vector2<T>{T(2.0), T(-0.2)}));
        CORRADE_COMPARE(o.rotation(), Math::Complex<T>::rotation(Math::Deg<T>{T(17.0)}));
        CORRADE_COMPARE(o.scaling(), Math::Vector2<T>{T(1.0)});
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix3<T>::translation({T(1.0), T(0.1)})*
            Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*
            Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.translateLocal({T(1.0), T(-0.3)})
         .translateLocal({T(1.0), T(0.1)});
        CORRADE_COMPARE(o.translation(), (Math::Vector2<T>{T(2.0), T(-0.2)}));
        CORRADE_COMPARE(o.rotation(), Math::Complex<T>::rotation(Math::Deg<T>{T(17.0)}));
        CORRADE_COMPARE(o.scaling(), Math::Vector2<T>{T(1.0)});
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*
            Math::Matrix3<T>::translation({T(1.0), T(0.1)})*
            Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    }
}

template<class T> void TranslationRotationScalingTransformation2DTest::rotate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));
        o.rotate(Math::Complex<T>::rotation(Math::Deg<T>{T(17.0)}))
         .rotate(Math::Deg<T>{T(-96.0)});
        CORRADE_COMPARE(o.translation(), (Math::Vector2<T>{T(1.0), T(-0.3)}));
        CORRADE_COMPARE(o.rotation(), Math::Complex<T>::rotation(Math::Deg<T>{T(-79.0)}));
        CORRADE_COMPARE(o.scaling(), Math::Vector2<T>{T(1.0)});
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*
            Math::Matrix3<T>::rotation(Math::Deg<T>{T(-96.0)})*
            Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));
        o.rotateLocal(Math::Complex<T>::rotation(Math::Deg<T>{T(17.0)}))
         .rotateLocal(Math::Deg<T>{T(-96.0)});
        CORRADE_COMPARE(o.translation(), (Math::Vector2<T>{T(1.0), T(-0.3)}));
        CORRADE_COMPARE(o.rotation(), Math::Complex<T>::rotation(Math::Deg<T>{T(-79.0)}));
        CORRADE_COMPARE(o.scaling(), Math::Vector2<T>{T(1.0)});
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*
            Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*
            Math::Matrix3<T>::rotation(Math::Deg<T>{T(-96.0)}));
    }
}

template<class T> void TranslationRotationScalingTransformation2DTest::scale() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.scale({T(1.0), T(-0.3)})
         .scale({T(0.5), T(1.1)});
        CORRADE_COMPARE(o.translation(), Math::Vector2<T>{});
        CORRADE_COMPARE(o.rotation(), Math::Complex<T>::rotation(Math::Deg<T>{T(17.0)}));
        CORRADE_COMPARE(o.scaling(), (Math::Vector2<T>{T(0.5), T(-0.33)}));
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*
            Math::Matrix3<T>::scaling({T(0.5), T(1.1)})*
            Math::Matrix3<T>::scaling({T(1.0), T(-0.3)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.scaleLocal({T(1.0), T(-0.3)})
         .scaleLocal({T(0.5), T(1.1)});
        CORRADE_COMPARE(o.translation(), Math::Vector2<T>{});
        CORRADE_COMPARE(o.rotation(), Math::Complex<T>::rotation(Math::Deg<T>{T(17.0)}));
        CORRADE_COMPARE(o.scaling(), (Math::Vector2<T>{T(0.5), T(-0.33)}));
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*
            Math::Matrix3<T>::scaling({T(1.0), T(-0.3)})*
            Math::Matrix3<T>::scaling({T(0.5), T(1.1)}));
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::TranslationRotationScalingTransformation2DTest)
