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

#include "Magnum/SceneGraph/DualComplexTransformation.h"
#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct DualComplexTransformationTest: TestSuite::Tester {
    explicit DualComplexTransformationTest();

    template<class T> void fromMatrix();
    template<class T> void toMatrix();
    template<class T> void compose();
    template<class T> void inverted();

    template<class T> void setTransformation();
    template<class T> void setTransformationInvalid();
    template<class T> void resetTransformation();
    template<class T> void transform();
    template<class T> void transformInvalid();
    template<class T> void translate();
    template<class T> void rotate();
    template<class T> void normalizeRotation();
};

DualComplexTransformationTest::DualComplexTransformationTest() {
    addTests<DualComplexTransformationTest>({
        &DualComplexTransformationTest::fromMatrix<Float>,
        &DualComplexTransformationTest::fromMatrix<Double>,
        &DualComplexTransformationTest::toMatrix<Float>,
        &DualComplexTransformationTest::toMatrix<Double>,
        &DualComplexTransformationTest::compose<Float>,
        &DualComplexTransformationTest::compose<Double>,
        &DualComplexTransformationTest::inverted<Float>,
        &DualComplexTransformationTest::inverted<Double>,

        &DualComplexTransformationTest::setTransformation<Float>,
        &DualComplexTransformationTest::setTransformation<Double>,
        &DualComplexTransformationTest::setTransformationInvalid<Float>,
        &DualComplexTransformationTest::setTransformationInvalid<Double>,
        &DualComplexTransformationTest::resetTransformation<Float>,
        &DualComplexTransformationTest::resetTransformation<Double>,
        &DualComplexTransformationTest::transform<Float>,
        &DualComplexTransformationTest::transform<Double>,
        &DualComplexTransformationTest::transformInvalid<Float>,
        &DualComplexTransformationTest::transformInvalid<Double>,
        &DualComplexTransformationTest::translate<Float>,
        &DualComplexTransformationTest::translate<Double>,
        &DualComplexTransformationTest::rotate<Float>,
        &DualComplexTransformationTest::rotate<Double>,
        &DualComplexTransformationTest::normalizeRotation<Float>,
        &DualComplexTransformationTest::normalizeRotation<Double>});
}

using namespace Math::Literals;

template<class T> using Object2D = Object<BasicDualComplexTransformation<T>>;
template<class T> using Scene2D = Scene<BasicDualComplexTransformation<T>>;

template<class T> void DualComplexTransformationTest::fromMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    Math::DualComplex<T> c = Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)})*Math::DualComplex<T>::translation({1.0, T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicDualComplexTransformation<T>>::fromMatrix(m), c);
}

template<class T> void DualComplexTransformationTest::toMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::DualComplex<T> c = Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)})*Math::DualComplex<T>::translation({T(1.0), T(-0.3)});
    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicDualComplexTransformation<T>>::toMatrix(c), m);
}

template<class T> void DualComplexTransformationTest::compose() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::DualComplex<T> parent = Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)});
    Math::DualComplex<T> child = Math::DualComplex<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicDualComplexTransformation<T>>::compose(parent, child), parent*child);
}

template<class T> void DualComplexTransformationTest::inverted() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::DualComplex<T> c = Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)})*Math::DualComplex<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicDualComplexTransformation<T>>::inverted(c)*c, Math::DualComplex<T>{});
}

template<class T> void DualComplexTransformationTest::setTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;

    /* Dirty after setting transformation */
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));

    /* Scene cannot be transformed */
    Scene2D<T> s;
    s.setClean();
    s.setTransformation(Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Math::Matrix3<T>{});
}

template<class T> void DualComplexTransformationTest::setTransformationInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Object2D<T> o;

    /* Can't transform with non-rigid transformation */
    std::ostringstream out;
    Error redirectError{&out};
    o.setTransformation(Math::DualComplex<T>({T(1.0), T(2.0)}, {}));
    CORRADE_COMPARE(out.str(), "SceneGraph::DualComplexTransformation::setTransformation(): the dual complex number is not normalized\n");
}

template<class T> void DualComplexTransformationTest::resetTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    o.setTransformation(Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(o.transformationMatrix() != Math::Matrix3<T>{});
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>{});
}

template<class T> void DualComplexTransformationTest::transform() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.transform(Math::DualComplex<T>::translation({T(1.0), T(-0.3)}));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.transformLocal(Math::DualComplex<T>::translation({T(1.0), T(-0.3)}));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));
    }
}

template<class T> void DualComplexTransformationTest::transformInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Can't transform with non-rigid transformation */
    Object2D<T> o;
    std::ostringstream out;
    Error redirectError{&out};
    o.transform(Math::DualComplex<T>{{T(1.0), T(2.0)}, {}});
    CORRADE_COMPARE(out.str(), "SceneGraph::DualComplexTransformation::transform(): the dual complex number is not normalized\n");
}

template<class T> void DualComplexTransformationTest::translate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.translate({T(1.0), T(-0.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.translateLocal({T(1.0), T(-0.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));
    }
}

template<class T> void DualComplexTransformationTest::rotate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::DualComplex<T>::translation({T(1.0), T(-0.3)}))
         .rotate(Math::Complex<T>::rotation(Math::Deg<T>{T(7.0)}))
         .rotate(Math::Deg<T>{T(10.0)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::DualComplex<T>::translation({T(1.0), T(-0.3)}))
         .rotateLocal(Math::Complex<T>::rotation(Math::Deg<T>{T(7.0)}))
         .rotateLocal(Math::Deg<T>{T(10.0)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    }
}

template<class T> void DualComplexTransformationTest::normalizeRotation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    o.setTransformation(Math::DualComplex<T>::rotation(Math::Deg<T>{T(17.0)}));
    o.normalizeRotation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::DualComplexTransformationTest)
