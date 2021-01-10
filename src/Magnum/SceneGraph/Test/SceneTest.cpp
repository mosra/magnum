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

#include "Magnum/SceneGraph/MatrixTransformation3D.hpp"
#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct SceneTest: TestSuite::Tester {
    explicit SceneTest();

    template<class T> void transformation();
    template<class T> void parent();
};

SceneTest::SceneTest() {
    addTests<SceneTest>({
        &SceneTest::transformation<Float>,
        &SceneTest::transformation<Double>,
        &SceneTest::parent<Float>,
        &SceneTest::parent<Double>});
}

template<class T> using Object3D = SceneGraph::Object<SceneGraph::BasicMatrixTransformation3D<T>>;
template<class T> using Scene3D = SceneGraph::Scene<SceneGraph::BasicMatrixTransformation3D<T>>;

template<class T> void SceneTest::transformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Scene3D<T> scene;

    Object3D<T>* scenePointer = &scene;
    scenePointer->setTransformation(Math::Matrix4<T>::translation({T(1.0), T(1.0), T(1.0)}));
    CORRADE_COMPARE(scene.transformation(), Math::Matrix4<T>{});
}

template<class T> void SceneTest::parent() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Scene3D<T> scene;

    /* Scene parent cannot be changed */
    Object3D<T>* scenePointer = &scene;
    Object3D<T> object;
    scenePointer->setParent(&object);
    CORRADE_VERIFY(scene.parent() == nullptr);
    CORRADE_VERIFY(scene.children().isEmpty());
    CORRADE_VERIFY(object.children().isEmpty());
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::SceneTest)
