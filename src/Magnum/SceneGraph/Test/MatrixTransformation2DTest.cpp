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

#include "Magnum/Math/Complex.h"
#include "Magnum/SceneGraph/MatrixTransformation2D.hpp"
#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct MatrixTransformation2DTest: TestSuite::Tester {
    explicit MatrixTransformation2DTest();

    template<class T> void fromMatrix();
    template<class T> void toMatrix();
    template<class T> void compose();
    template<class T> void inverted();

    template<class T> void setTransformation();
    template<class T> void resetTransformation();
    template<class T> void transform();
    template<class T> void translate();
    template<class T> void rotate();
    template<class T> void scale();
    template<class T> void reflect();
};

MatrixTransformation2DTest::MatrixTransformation2DTest() {
    addTests<MatrixTransformation2DTest>({
        &MatrixTransformation2DTest::fromMatrix<Float>,
        &MatrixTransformation2DTest::fromMatrix<Double>,
        &MatrixTransformation2DTest::toMatrix<Float>,
        &MatrixTransformation2DTest::toMatrix<Double>,
        &MatrixTransformation2DTest::compose<Float>,
        &MatrixTransformation2DTest::compose<Double>,
        &MatrixTransformation2DTest::inverted<Float>,
        &MatrixTransformation2DTest::inverted<Double>,

        &MatrixTransformation2DTest::setTransformation<Float>,
        &MatrixTransformation2DTest::setTransformation<Double>,
        &MatrixTransformation2DTest::resetTransformation<Float>,
        &MatrixTransformation2DTest::resetTransformation<Double>,
        &MatrixTransformation2DTest::transform<Float>,
        &MatrixTransformation2DTest::transform<Double>,
        &MatrixTransformation2DTest::translate<Float>,
        &MatrixTransformation2DTest::translate<Double>,
        &MatrixTransformation2DTest::rotate<Float>,
        &MatrixTransformation2DTest::rotate<Double>,
        &MatrixTransformation2DTest::scale<Float>,
        &MatrixTransformation2DTest::scale<Double>,
        &MatrixTransformation2DTest::reflect<Float>,
        &MatrixTransformation2DTest::reflect<Double>});
}

using namespace Math::Literals;

template<class T> using Object2D = Object<BasicMatrixTransformation2D<T>>;
template<class T> using Scene2D = Scene<BasicMatrixTransformation2D<T>>;

template<class T> void MatrixTransformation2DTest::fromMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicMatrixTransformation2D<T>>::fromMatrix(m), m);
}

template<class T> void MatrixTransformation2DTest::toMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicMatrixTransformation2D<T>>::toMatrix(m), m);
}

template<class T> void MatrixTransformation2DTest::compose() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> parent = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)});
    Math::Matrix3<T> child = Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicMatrixTransformation2D<T>>::compose(parent, child), parent*child);
}

template<class T> void MatrixTransformation2DTest::inverted() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicMatrixTransformation2D<T>>::inverted(m)*m, Math::Matrix3<T>{});
}

template<class T> void MatrixTransformation2DTest::setTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Dirty after setting transformation */
    Object2D<T> o;
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));

    /* Scene cannot be transformed */
    Scene2D<T> s;
    s.setClean();
    CORRADE_VERIFY(!s.isDirty());
    s.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Math::Matrix3<T>{});
}

template<class T> void MatrixTransformation2DTest::resetTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    o.rotate(Math::Deg<T>{T(17.0)});
    CORRADE_VERIFY(o.transformationMatrix() != Math::Matrix3<T>{});
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>{});
}

template<class T> void MatrixTransformation2DTest::transform() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.transform(Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.transformLocal(Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));
    }
}

template<class T> void MatrixTransformation2DTest::translate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.translate({T(1.0), T(-0.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.translateLocal({T(1.0), T(-0.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));
    }
}

template<class T> void MatrixTransformation2DTest::rotate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::translation({T(1.0), T(-0.3)}))
         .rotate(Math::Complex<T>::rotation(Math::Deg<T>{T(7.0)}))
         .rotate(Math::Deg<T>{T(10.0)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::translation({T(1.0), T(-0.3)}))
         .rotateLocal(Math::Complex<T>::rotation(Math::Deg<T>{T(7.0)}))
         .rotateLocal(Math::Deg<T>{T(10.0)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::translation({T(1.0), T(-0.3)})*Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    }
}

template<class T> void MatrixTransformation2DTest::scale() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.scale({T(1.0), T(-0.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::scaling({T(1.0), T(-0.3)})*Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.scaleLocal({T(1.0), T(-0.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::scaling({T(1.0), T(-0.3)}));
    }
}

template<class T> void MatrixTransformation2DTest::reflect() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.reflect(Math::Vector2<T>{T(-1.0)/Math::Constants<T>::sqrt2()});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::reflection(Math::Vector2<T>{T(-1.0)/Math::Constants<T>::sqrt2()})*Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    } {
        Object2D<T> o;
        o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
        o.reflectLocal(Math::Vector2<T>{T(-1.0)/Math::Constants<T>::sqrt2()});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::reflection(Math::Vector2<T>{T(-1.0)/Math::Constants<T>::sqrt2()}));
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::MatrixTransformation2DTest)
