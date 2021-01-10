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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/TranslationTransformation.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct TranslationTransformationTest: TestSuite::Tester {
    explicit TranslationTransformationTest();

    template<class T> void fromMatrix();
    template<class T> void fromMatrixInvalid();
    template<class T> void toMatrix();
    template<class T> void compose();
    template<class T> void inverted();

    template<class T> void setTransformation();
    template<class T> void resetTransformation();
    template<class T> void transform();
    template<class T> void translate();

    template<class T> void integral();
};

TranslationTransformationTest::TranslationTransformationTest() {
    addTests<TranslationTransformationTest>({
        &TranslationTransformationTest::fromMatrix<Float>,
        &TranslationTransformationTest::fromMatrix<Double>,
        &TranslationTransformationTest::fromMatrixInvalid<Float>,
        &TranslationTransformationTest::fromMatrixInvalid<Double>,
        &TranslationTransformationTest::toMatrix<Float>,
        &TranslationTransformationTest::toMatrix<Double>,
        &TranslationTransformationTest::compose<Float>,
        &TranslationTransformationTest::compose<Double>,
        &TranslationTransformationTest::inverted<Float>,
        &TranslationTransformationTest::inverted<Double>,

        &TranslationTransformationTest::setTransformation<Float>,
        &TranslationTransformationTest::setTransformation<Double>,
        &TranslationTransformationTest::resetTransformation<Float>,
        &TranslationTransformationTest::resetTransformation<Double>,
        &TranslationTransformationTest::transform<Float>,
        &TranslationTransformationTest::transform<Double>,
        &TranslationTransformationTest::translate<Float>,
        &TranslationTransformationTest::translate<Double>,

        &TranslationTransformationTest::integral<Float>,
        &TranslationTransformationTest::integral<Double>});
}

template<class T> using Object2D = Object<BasicTranslationTransformation2D<T>>;
template<class T> using Scene2D = Scene<BasicTranslationTransformation2D<T>>;

template<class T> void TranslationTransformationTest::fromMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const Math::Vector2<T> v{T(1.0), T(-0.3)};
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationTransformation2D<T>>::fromMatrix(Math::Matrix3<T>::translation(v)), v);
}

template<class T> void TranslationTransformationTest::fromMatrixInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream o;
    Error redirectError{&o};
    Implementation::Transformation<BasicTranslationTransformation2D<T>>::fromMatrix(Math::Matrix3<T>::scaling(Math::Vector2<T>{T(4.0)}));
    CORRADE_COMPARE(o.str(), "SceneGraph::TranslationTransformation: the matrix doesn't represent pure translation\n");
}

template<class T> void TranslationTransformationTest::toMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const Math::Vector2<T> v{T(1.0), T(-0.3)};
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationTransformation2D<T>>::toMatrix(v), Math::Matrix3<T>::translation(v));
}

template<class T> void TranslationTransformationTest::compose() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const Math::Vector2<T> parent{T(-0.5), T(2.0)};
    const Math::Vector2<T> child{T(1.0), T(-0.3)};
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationTransformation2D<T>>::compose(parent, child), (Math::Vector2<T>{T(0.5), T(1.7)}));
}

template<class T> void TranslationTransformationTest::inverted() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const Math::Vector2<T> v{T(1.0), T(-0.3)};
    CORRADE_COMPARE(Implementation::Transformation<BasicTranslationTransformation2D<T>>::inverted(v), (Math::Vector2<T>{T(-1.0), T(0.3)}));
}

template<class T> void TranslationTransformationTest::setTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;

    /* Dirty after setting transformation */
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation({T(1.0), T(-0.3)});
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));

    /* Scene cannot be transformed */
    Scene2D<T> s;
    s.setClean();
    s.setTransformation({T(1.0), T(-0.3)});
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Math::Matrix3<T>{});
}

template<class T> void TranslationTransformationTest::resetTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    o.setTransformation({T(1.0), T(-0.3)});
    CORRADE_VERIFY(o.transformationMatrix() != Math::Matrix3<T>{});
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>{});
}

template<class T> void TranslationTransformationTest::transform() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    o.setTransformation({T(1.0), T(-0.3)})
        .transform({T(-0.5), T(2.0)});
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::translation({T(0.5), T(1.7)}));
}

template<class T> void TranslationTransformationTest::translate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    o.setTransformation({T(1.0), T(-0.3)})
        .translate({T(-0.5), T(2.0)});
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*Math::Matrix3<T>::translation({T(-0.5), T(2.0)}));
}

template<class T> void TranslationTransformationTest::integral() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    typedef Object<BasicTranslationTransformation2D<Float, Short>> Object2Di;

    Object2Di o;
    o.translate({3, -7});
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({3.0f, -7.0f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::TranslationTransformationTest)
