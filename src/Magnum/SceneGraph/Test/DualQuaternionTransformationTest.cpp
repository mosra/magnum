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

#include "Magnum/SceneGraph/DualQuaternionTransformation.h"
#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct DualQuaternionTransformationTest: TestSuite::Tester {
    explicit DualQuaternionTransformationTest();

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
    template<class T> void normalizeRotation();
};

DualQuaternionTransformationTest::DualQuaternionTransformationTest() {
    addTests<DualQuaternionTransformationTest>({
        &DualQuaternionTransformationTest::fromMatrix<Float>,
        &DualQuaternionTransformationTest::fromMatrix<Double>,
        &DualQuaternionTransformationTest::fromMatrixInvalid<Float>,
        &DualQuaternionTransformationTest::fromMatrixInvalid<Double>,
        &DualQuaternionTransformationTest::toMatrix<Float>,
        &DualQuaternionTransformationTest::toMatrix<Double>,
        &DualQuaternionTransformationTest::compose<Float>,
        &DualQuaternionTransformationTest::compose<Double>,
        &DualQuaternionTransformationTest::inverted<Float>,
        &DualQuaternionTransformationTest::inverted<Double>,

        &DualQuaternionTransformationTest::setTransformation<Float>,
        &DualQuaternionTransformationTest::setTransformation<Double>,
        &DualQuaternionTransformationTest::setTransformationInvalid<Float>,
        &DualQuaternionTransformationTest::setTransformationInvalid<Double>,
        &DualQuaternionTransformationTest::resetTransformation<Float>,
        &DualQuaternionTransformationTest::resetTransformation<Double>,
        &DualQuaternionTransformationTest::transform<Float>,
        &DualQuaternionTransformationTest::transform<Double>,
        &DualQuaternionTransformationTest::transformInvalid<Float>,
        &DualQuaternionTransformationTest::transformInvalid<Double>,
        &DualQuaternionTransformationTest::translate<Float>,
        &DualQuaternionTransformationTest::translate<Double>,
        &DualQuaternionTransformationTest::rotate<Float>,
        &DualQuaternionTransformationTest::rotate<Double>,
        &DualQuaternionTransformationTest::normalizeRotation<Float>,
        &DualQuaternionTransformationTest::normalizeRotation<Double>});
}

using namespace Math::Literals;

template<class T> using Object3D = Object<BasicDualQuaternionTransformation<T>>;
template<class T> using Scene3D = Scene<BasicDualQuaternionTransformation<T>>;

template<class T> void DualQuaternionTransformationTest::fromMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)});
    Math::DualQuaternion<T> q = Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis())*Math::DualQuaternion<T>::translation({T(1.0), T(-0.3), T(2.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicDualQuaternionTransformation<T>>::fromMatrix(m), q);
}

template<class T> void DualQuaternionTransformationTest::fromMatrixInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Implementation::Transformation<BasicDualQuaternionTransformation<T>>::fromMatrix(Math::Matrix4<T>::scaling(Math::Vector3<T>{T(4.0)}));
    CORRADE_COMPARE(out.str(), "SceneGraph::DualQuaternionTransformation: the matrix doesn't represent rigid transformation\n");
}

template<class T> void DualQuaternionTransformationTest::toMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::DualQuaternion<T> q = Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis())*Math::DualQuaternion<T>::translation({T(1.0), T(-0.3), T(2.3)});
    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicDualQuaternionTransformation<T>>::toMatrix(q), m);
}

template<class T> void DualQuaternionTransformationTest::compose() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::DualQuaternion<T> parent = Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis());
    Math::DualQuaternion<T> child = Math::DualQuaternion<T>::translation({T(1.0), T(-0.3), T(2.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicDualQuaternionTransformation<T>>::compose(parent, child), parent*child);
}

template<class T> void DualQuaternionTransformationTest::inverted() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::DualQuaternion<T> q = Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis())*Math::DualQuaternion<T>::translation({T(1.0), T(-0.3), T(2.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicDualQuaternionTransformation<T>>::inverted(q)*q, Math::DualQuaternion<T>{});
}

template<class T> void DualQuaternionTransformationTest::setTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;

    /* Dirty after setting transformation */
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));

    /* Scene cannot be transformed */
    Scene3D<T> s;
    s.setClean();
    CORRADE_VERIFY(!s.isDirty());
    s.setTransformation(Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Math::Matrix4<T>{});
}

template<class T> void DualQuaternionTransformationTest::setTransformationInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Object3D<T> o;

    /* Can't transform with non-rigid transformation */
    std::ostringstream out;
    Error redirectError{&out};
    o.setTransformation(Math::DualQuaternion<T>({{T(1.0), T(2.0), T(3.0)}, T(4.0)}, {}));
    CORRADE_COMPARE(out.str(), "SceneGraph::DualQuaternionTransformation::setTransformation(): the dual quaternion is not normalized\n");
}

template<class T> void DualQuaternionTransformationTest::resetTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    o.setTransformation(Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
    CORRADE_VERIFY(o.transformationMatrix() != Math::Matrix4<T>{});
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>{});
}

template<class T> void DualQuaternionTransformationTest::transform() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
        o.transform(Math::DualQuaternion<T>::translation({T(1.0), T(-0.3), T(2.3)}));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
        o.transformLocal(Math::DualQuaternion<T>::translation({T(1.0), T(-0.3), T(2.3)}));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}));
    }
}

template<class T> void DualQuaternionTransformationTest::transformInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Can't transform with non-rigid transformation */
    Object3D<T> o;
    std::ostringstream out;
    Error redirectError{&out};
    o.transform(Math::DualQuaternion<T>({{T(1.0), T(2.0), T(3.0)}, T(4.0)}, {}));
    CORRADE_COMPARE(out.str(), "SceneGraph::DualQuaternionTransformation::transform(): the dual quaternion is not normalized\n");
}

template<class T> void DualQuaternionTransformationTest::translate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
        o.translate({T(1.0), T(-0.3), T(2.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
        o.translateLocal({T(1.0), T(-0.3), T(2.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}));
    }
}

template<class T> void DualQuaternionTransformationTest::rotate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.transform(Math::DualQuaternion<T>::translation({T(1.0), T(-0.3), T(2.3)}))
         .rotateX(Math::Deg<T>{T(17.0)})
         .rotateY(Math::Deg<T>{T(25.0)})
         .rotateZ(-Math::Deg<T>{T(23.0)})
         .rotate(Math::Quaternion<T>::rotation(Math::Deg<T>{T(36.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()}))
         .rotate(Math::Deg<T>{T(60.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>{
            Math::Matrix4<T>::rotation(Math::Deg<T>{T(96.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()})*
            Math::Matrix4<T>::rotationZ(Math::Deg<T>{T(-23.0)})*
            Math::Matrix4<T>::rotationY(Math::Deg<T>{T(25.0)})*
            Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*
            Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})});
    } {
        Object3D<T> o;
        o.transform(Math::DualQuaternion<T>::translation({T(1.0), T(-0.3), T(2.3)}))
         .rotateXLocal(Math::Deg<T>{T(17.0)})
         .rotateYLocal(Math::Deg<T>{T(25.0)})
         .rotateZLocal(-Math::Deg<T>{T(23.0)})
         .rotateLocal(Math::Quaternion<T>::rotation(Math::Deg<T>{T(36.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()}))
         .rotateLocal(Math::Deg<T>{T(60.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>{
            Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*
            Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*
            Math::Matrix4<T>::rotationY(Math::Deg<T>{T(25.0)})*
            Math::Matrix4<T>::rotationZ(Math::Deg<T>{T(-23.0)})*
            Math::Matrix4<T>::rotation(Math::Deg<T>{T(96.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()})});
    }
}

template<class T> void DualQuaternionTransformationTest::normalizeRotation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    o.setTransformation(Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(17.0)}, Math::Vector3<T>::xAxis()));
    o.normalizeRotation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::DualQuaternionTransformationTest)
