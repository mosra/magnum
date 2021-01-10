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

#include "Magnum/Math/Quaternion.h"
#include "Magnum/SceneGraph/MatrixTransformation3D.hpp"
#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct MatrixTransformation3DTest: TestSuite::Tester {
    explicit MatrixTransformation3DTest();

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

MatrixTransformation3DTest::MatrixTransformation3DTest() {
    addTests<MatrixTransformation3DTest>({
        &MatrixTransformation3DTest::fromMatrix<Float>,
        &MatrixTransformation3DTest::fromMatrix<Double>,
        &MatrixTransformation3DTest::toMatrix<Float>,
        &MatrixTransformation3DTest::toMatrix<Double>,
        &MatrixTransformation3DTest::compose<Float>,
        &MatrixTransformation3DTest::compose<Double>,
        &MatrixTransformation3DTest::inverted<Float>,
        &MatrixTransformation3DTest::inverted<Double>,

        &MatrixTransformation3DTest::setTransformation<Float>,
        &MatrixTransformation3DTest::setTransformation<Double>,
        &MatrixTransformation3DTest::resetTransformation<Float>,
        &MatrixTransformation3DTest::resetTransformation<Double>,
        &MatrixTransformation3DTest::transform<Float>,
        &MatrixTransformation3DTest::transform<Double>,
        &MatrixTransformation3DTest::translate<Float>,
        &MatrixTransformation3DTest::translate<Double>,
        &MatrixTransformation3DTest::rotate<Float>,
        &MatrixTransformation3DTest::rotate<Double>,
        &MatrixTransformation3DTest::scale<Float>,
        &MatrixTransformation3DTest::scale<Double>,
        &MatrixTransformation3DTest::reflect<Float>,
        &MatrixTransformation3DTest::reflect<Double>});
}

using namespace Math::Literals;

template<class T> using Object3D = Object<BasicMatrixTransformation3D<T>>;
template<class T> using Scene3D = Scene<BasicMatrixTransformation3D<T>>;

template<class T> void MatrixTransformation3DTest::fromMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::scaling({T(2.0), T(1.4), T(-2.1)});
    CORRADE_COMPARE(Implementation::Transformation<BasicMatrixTransformation3D<T>>::fromMatrix(m), m);
}

template<class T> void MatrixTransformation3DTest::toMatrix() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::scaling({T(2.0), T(1.4), T(-2.1)});
    CORRADE_COMPARE(Implementation::Transformation<BasicMatrixTransformation3D<T>>::toMatrix(m), m);
}

template<class T> void MatrixTransformation3DTest::compose() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> parent = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)});
    Math::Matrix4<T> child = Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)});
    CORRADE_COMPARE(Implementation::Transformation<BasicMatrixTransformation3D<T>>::compose(parent, child), parent*child);
}

template<class T> void MatrixTransformation3DTest::inverted() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Matrix4<T> m = Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::translation({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::scaling({T(2.0), T(1.4), T(-2.1)});
    CORRADE_COMPARE(Implementation::Transformation<BasicMatrixTransformation3D<T>>::inverted(m)*m, Math::Matrix4<T>{});
}

template<class T> void MatrixTransformation3DTest::setTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Dirty after setting transformation */
    Object3D<T> o;
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

template<class T> void MatrixTransformation3DTest::resetTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    o.rotateX(Math::Deg<T>{T(17.0)});
    CORRADE_VERIFY(o.transformationMatrix() != Math::Matrix4<T>());
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>());
}

template<class T> void MatrixTransformation3DTest::transform() {
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

template<class T> void MatrixTransformation3DTest::translate() {
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

template<class T> void MatrixTransformation3DTest::rotate() {
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

template<class T> void MatrixTransformation3DTest::scale() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
        o.scale({T(1.0), T(-0.3), T(2.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::scaling({T(1.0), T(-0.3), T(2.3)})*Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
        o.scaleLocal({T(1.0), T(-0.3), T(2.3)});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::scaling({T(1.0), T(-0.3), T(2.3)}));
    }
}

template<class T> void MatrixTransformation3DTest::reflect() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
        o.reflect(Math::Vector3<T>{T(-1.0)/Math::Constants<T>::sqrt3()});
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::reflection(Math::Vector3<T>(T(-1.0)/Math::Constants<T>::sqrt3()))*Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
    } {
        Object3D<T> o;
        o.setTransformation(Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)}));
        o.reflectLocal(Math::Vector3<T>(T(-1.0)/Math::Constants<T>::sqrt3()));
        CORRADE_COMPARE(o.transformationMatrix(), Math::Matrix4<T>::rotationX(Math::Deg<T>{T(17.0)})*Math::Matrix4<T>::reflection(Math::Vector3<T>(T(-1.0)/Math::Constants<T>::sqrt3())));
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::MatrixTransformation3DTest)
