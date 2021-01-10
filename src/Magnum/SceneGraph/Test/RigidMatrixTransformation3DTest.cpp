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

#include "Magnum/Math/Quaternion.h"
#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/RigidMatrixTransformation3D.hpp"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct RigidMatrixTransformation3DTest: TestSuite::Tester {
    explicit RigidMatrixTransformation3DTest();

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

RigidMatrixTransformation3DTest::RigidMatrixTransformation3DTest() {
    addTests<RigidMatrixTransformation3DTest>({
        &RigidMatrixTransformation3DTest::fromMatrix<Float>,
        &RigidMatrixTransformation3DTest::fromMatrix<Double>,
        &RigidMatrixTransformation3DTest::fromMatrixInvalid<Float>,
        &RigidMatrixTransformation3DTest::fromMatrixInvalid<Double>,
        &RigidMatrixTransformation3DTest::toMatrix<Float>,
        &RigidMatrixTransformation3DTest::toMatrix<Double>,
        &RigidMatrixTransformation3DTest::compose<Float>,
        &RigidMatrixTransformation3DTest::compose<Double>,
        &RigidMatrixTransformation3DTest::inverted<Float>,
        &RigidMatrixTransformation3DTest::inverted<Double>,

        &RigidMatrixTransformation3DTest::setTransformation<Float>,
        &RigidMatrixTransformation3DTest::setTransformation<Double>,
        &RigidMatrixTransformation3DTest::setTransformationInvalid<Float>,
        &RigidMatrixTransformation3DTest::setTransformationInvalid<Double>,
        &RigidMatrixTransformation3DTest::resetTransformation<Float>,
        &RigidMatrixTransformation3DTest::resetTransformation<Double>,
        &RigidMatrixTransformation3DTest::transform<Float>,
        &RigidMatrixTransformation3DTest::transform<Double>,
        &RigidMatrixTransformation3DTest::transformInvalid<Float>,
        &RigidMatrixTransformation3DTest::transformInvalid<Double>,
        &RigidMatrixTransformation3DTest::translate<Float>,
        &RigidMatrixTransformation3DTest::translate<Double>,
        &RigidMatrixTransformation3DTest::rotate<Float>,
        &RigidMatrixTransformation3DTest::rotate<Double>,
        &RigidMatrixTransformation3DTest::reflect<Float>,
        &RigidMatrixTransformation3DTest::reflect<Double>,
        &RigidMatrixTransformation3DTest::normalizeRotation<Float>,
        &RigidMatrixTransformation3DTest::normalizeRotation<Double>});
}

using namespace Math::Literals;

template<class T> using Object3D = Object<BasicRigidMatrixTransformation3D<T>>;
template<class T> using Scene3D = Scene<BasicRigidMatrixTransformation3D<T>>;

template<class T> void RigidMatrixTransformation3DTest::fromMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicRigidMatrixTransformation3D<T>>::fromMatrix(m), m);
}

template<class T> void RigidMatrixTransformation3DTest::fromMatrixInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream o;
    Error redirectError{&o};
    Implementation::Transformation<BasicRigidMatrixTransformation3D<T>>::fromMatrix(Math::Matrix4<T>::scaling(Math::Vector3<T>(T(4.0))));
    CORRADE_COMPARE(o.str(), "SceneGraph::RigidMatrixTransformation3D: the matrix doesn't represent rigid transformation\n");
}

template<class T> void RigidMatrixTransformation3DTest::toMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicRigidMatrixTransformation3D<T>>::toMatrix(Math::Matrix4<T>{m}), Math::Matrix4<T>{m});
}

template<class T> void RigidMatrixTransformation3DTest::compose() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> parent = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)});
    Math::Matrix4<T> child = Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicRigidMatrixTransformation3D<T>>::compose(parent, child), parent*child);
}

template<class T> void RigidMatrixTransformation3DTest::inverted() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicRigidMatrixTransformation3D<T>>::inverted(m)*m, Math::Matrix4<T>{});
}

template<class T> void RigidMatrixTransformation3DTest::setTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;

    /* Dirty after setting transformation */
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));

    /* Scene cannot be transformed */
    Scene3D<T> s;
    s.setClean();
    CORRADE_VERIFY(!s.isDirty());
    s.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Math::Matrix4<T>());
}

template<class T> void RigidMatrixTransformation3DTest::setTransformationInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Object3D<T> o;

    /* Can't transform with non-rigid transformation */
    std::ostringstream out;
    Error redirectError{&out};
    o.setTransformation(Math::Matrix4<T>::scaling(Math::Vector3<T>(T(3.0))));
    CORRADE_COMPARE(out.str(), "SceneGraph::RigidMatrixTransformation3D::setTransformation(): the matrix doesn't represent rigid transformation\n");
}

template<class T> void RigidMatrixTransformation3DTest::resetTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    CORRADE_VERIFY(o.transformationMatrix() != Math::Matrix4<T>());
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>());
}

template<class T> void RigidMatrixTransformation3DTest::transform() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
        o.transform(Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
        o.transformLocal(Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}));
    }
}

template<class T> void RigidMatrixTransformation3DTest::transformInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Can't transform with non-rigid transformation */
    Object3D<T> o;
    std::ostringstream out;
    Error redirectError{&out};
    o.transform(Math::Matrix4<T>::scaling(Math::Vector3<T>(T(3.0))));
    CORRADE_COMPARE(out.str(), "SceneGraph::RigidMatrixTransformation3D::transform(): the matrix doesn't represent rigid transformation\n");
}

template<class T> void RigidMatrixTransformation3DTest::translate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
        o.translate({T(1.0), T(-0.3), T(2.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
        o.translateLocal({T(1.0), T(-0.3), T(2.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}));
    }
}

template<class T> void RigidMatrixTransformation3DTest::rotate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}))
         .rotateX(Math::Deg<T>{T(17.0)})
         .rotateY(Math::Deg<T>{T(25.0)})
         .rotateZ(Math::Deg<T>{T(-23.0)})
         .rotate(Math::Quaternion<T>::rotation(Math::Deg<T>{T(36.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()}))
         .rotate(Math::Deg<T>{T(60.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()});
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix4<T>::rotation(Math::Deg<T>{T(96.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()})*
            Math::Matrix4<T>::rotationZ(Math::Deg<T>{T(-23.0)})*
            Math::Matrix4<T>::rotationY(Math::Deg<T>{T(25.0)})*
            Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*
            Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)}))
         .rotateXLocal(Math::Deg<T>{T(17.0)})
         .rotateYLocal(Math::Deg<T>{T(25.0)})
         .rotateZLocal(Math::Deg<T>{T(-23.0)})
         .rotateLocal(Math::Quaternion<T>::rotation(Math::Deg<T>{T(36.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()}))
         .rotateLocal(Math::Deg<T>{T(60.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()});
        CORRADE_COMPARE(o.transformationMatrix(),
            Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*
            Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*
            Math::Matrix4<T>::rotationY(Math::Deg<T>{T(25.0)})*
            Math::Matrix4<T>::rotationZ(Math::Deg<T>{T(-23.0)})*
            Math::Matrix4<T>::rotation(Math::Deg<T>{T(96.0)}, Math::Vector3<T>{T(1.0)/Math::Constants<T>::sqrt3()}));
    }
}

template<class T> void RigidMatrixTransformation3DTest::reflect() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
        o.reflect(Math::Vector3<T>(T(-1.0)/Math::Constants<T>::sqrt3()));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::reflection(Math::Vector3<T>(T(-1.0)/Math::Constants<T>::sqrt3()))*Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
        o.reflectLocal(Math::Vector3<T>(T(-1.0)/Math::Constants<T>::sqrt3()));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::reflection(Math::Vector3<T>(T(-1.0)/Math::Constants<T>::sqrt3())));
    }
}

template<class T> void RigidMatrixTransformation3DTest::normalizeRotation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    o.normalizeRotation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::RigidMatrixTransformation3DTest)
