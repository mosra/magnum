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

#include "Magnum/Math/Complex.h"
#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/RigidMatrixTransformation2D.hpp"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct RigidMatrixTransformation2DTest: TestSuite::Tester {
    explicit RigidMatrixTransformation2DTest();

    template<class T> void fromMatrix();
    template<class T> void fromMatrixInvalid();
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
    template<class T> void reflect();
    template<class T> void normalizeRotation();
};

RigidMatrixTransformation2DTest::RigidMatrixTransformation2DTest() {
    addTests<RigidMatrixTransformation2DTest>({
        &RigidMatrixTransformation2DTest::fromMatrix<Float>,
        &RigidMatrixTransformation2DTest::fromMatrix<Double>,
        &RigidMatrixTransformation2DTest::fromMatrixInvalid<Float>,
        &RigidMatrixTransformation2DTest::fromMatrixInvalid<Double>,
        &RigidMatrixTransformation2DTest::toMatrix<Float>,
        &RigidMatrixTransformation2DTest::toMatrix<Double>,
        &RigidMatrixTransformation2DTest::compose<Float>,
        &RigidMatrixTransformation2DTest::compose<Double>,
        &RigidMatrixTransformation2DTest::inverted<Float>,
        &RigidMatrixTransformation2DTest::inverted<Double>,

        &RigidMatrixTransformation2DTest::setTransformation<Float>,
        &RigidMatrixTransformation2DTest::setTransformation<Double>,
        &RigidMatrixTransformation2DTest::setTransformationInvalid<Float>,
        &RigidMatrixTransformation2DTest::setTransformationInvalid<Double>,
        &RigidMatrixTransformation2DTest::resetTransformation<Float>,
        &RigidMatrixTransformation2DTest::resetTransformation<Double>,
        &RigidMatrixTransformation2DTest::transform<Float>,
        &RigidMatrixTransformation2DTest::transform<Double>,
        &RigidMatrixTransformation2DTest::transformInvalid<Float>,
        &RigidMatrixTransformation2DTest::transformInvalid<Double>,
        &RigidMatrixTransformation2DTest::translate<Float>,
        &RigidMatrixTransformation2DTest::translate<Double>,
        &RigidMatrixTransformation2DTest::rotate<Float>,
        &RigidMatrixTransformation2DTest::rotate<Double>,
        &RigidMatrixTransformation2DTest::reflect<Float>,
        &RigidMatrixTransformation2DTest::reflect<Double>,
        &RigidMatrixTransformation2DTest::normalizeRotation<Float>,
        &RigidMatrixTransformation2DTest::normalizeRotation<Double>});
}

using namespace Math::Literals;

template<class T> using Object2D = Object<BasicRigidMatrixTransformation2D<T>>;
template<class T> using Scene2D = Scene<BasicRigidMatrixTransformation2D<T>>;

template<class T> void RigidMatrixTransformation2DTest::fromMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicRigidMatrixTransformation2D<T>>::fromMatrix(m), m);
}

template<class T> void RigidMatrixTransformation2DTest::fromMatrixInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Implementation::Transformation<BasicRigidMatrixTransformation2D<T>>::fromMatrix(Math::Matrix3<T>::scaling(Math::Vector2<T>{T(4.0)}));
    CORRADE_COMPARE(out.str(), "SceneGraph::RigidMatrixTransformation2D: the matrix doesn't represent rigid transformation\n");
}

template<class T> void RigidMatrixTransformation2DTest::toMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicRigidMatrixTransformation2D<T>>::toMatrix(m), m);
}

template<class T> void RigidMatrixTransformation2DTest::compose() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> parent = Math::Matrix3<T>::rotation(Math::Deg<T>(T(17.0)));
    Math::Matrix3<T> child = Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicRigidMatrixTransformation2D<T>>::compose(parent, child), parent*child);
}

template<class T> void RigidMatrixTransformation2DTest::inverted() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix3<T> m = Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)})*Math::Matrix3<T>::translation({T(1.0), T(-0.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicRigidMatrixTransformation2D<T>>::inverted(m)*m, Math::Matrix3<T>{});
}

template<class T> void RigidMatrixTransformation2DTest::setTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;

    /* Dirty after setting transformation */
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));

    /* Scene cannot be transformed */
    Scene2D<T> s;
    s.setClean();
    s.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Math::Matrix3<T>());
}

template<class T> void RigidMatrixTransformation2DTest::setTransformationInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Object2D<T> o;

    /* Can't transform with non-rigid transformation */
    std::ostringstream out;
    Error redirectError{&out};
    o.setTransformation(Math::Matrix3<T>::scaling(Math::Vector2<T>{T(3.0)}));
    CORRADE_COMPARE(out.str(), "SceneGraph::RigidMatrixTransformation2D::setTransformation(): the matrix doesn't represent rigid transformation\n");
}

template<class T> void RigidMatrixTransformation2DTest::resetTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(o.transformationMatrix() != Math::Matrix3<T>());
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>());
}

template<class T> void RigidMatrixTransformation2DTest::transform() {
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

template<class T> void RigidMatrixTransformation2DTest::transformInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Can't transform with non-rigid transformation */
    Object2D<T> o;
    std::ostringstream out;
    Error redirectError{&out};
    o.transform(Math::Matrix3<T>::scaling(Math::Vector2<T>{T(3.0)}));
    CORRADE_COMPARE(out.str(), "SceneGraph::RigidMatrixTransformation2D::transform(): the matrix doesn't represent rigid transformation\n");
}

template<class T> void RigidMatrixTransformation2DTest::translate() {
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

template<class T> void RigidMatrixTransformation2DTest::rotate() {
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

template<class T> void RigidMatrixTransformation2DTest::reflect() {
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

template<class T> void RigidMatrixTransformation2DTest::normalizeRotation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    o.setTransformation(Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
    o.normalizeRotation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix3<T>::rotation(Math::Deg<T>{T(17.0)}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::RigidMatrixTransformation2DTest)
