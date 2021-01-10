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

#include <algorithm>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/SceneGraph/AbstractFeature.hpp"
#include "Magnum/SceneGraph/Camera.hpp"
#include "Magnum/SceneGraph/Camera.h"
#include "Magnum/SceneGraph/Drawable.hpp"
#include "Magnum/SceneGraph/FeatureGroup.hpp"
#include "Magnum/SceneGraph/MatrixTransformation2D.hpp"
#include "Magnum/SceneGraph/MatrixTransformation3D.hpp"
#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct CameraTest: TestSuite::Tester {
    explicit CameraTest();

    template<class T> void fixAspectRatio();
    template<class T> void defaultProjection2D();
    template<class T> void defaultProjection3D();
    template<class T> void projectionCorrectedInvertedY();
    template<class T> void projectionSize2D();
    template<class T> void projectionSizeOrthographic();
    template<class T> void projectionSizePerspective();
    template<class T> void projectionSizeViewport();

    template<class T> void draw();
    template<class T> void drawOrdered();
};

CameraTest::CameraTest() {
    addTests<CameraTest>({
        &CameraTest::fixAspectRatio<Float>,
        &CameraTest::fixAspectRatio<Double>,
        &CameraTest::defaultProjection2D<Float>,
        &CameraTest::defaultProjection2D<Double>,
        &CameraTest::defaultProjection3D<Float>,
        &CameraTest::defaultProjection3D<Double>,
        &CameraTest::projectionCorrectedInvertedY<Float>,
        &CameraTest::projectionCorrectedInvertedY<Double>,
        &CameraTest::projectionSize2D<Float>,
        &CameraTest::projectionSize2D<Double>,
        &CameraTest::projectionSizeOrthographic<Float>,
        &CameraTest::projectionSizeOrthographic<Double>,
        &CameraTest::projectionSizePerspective<Float>,
        &CameraTest::projectionSizePerspective<Double>,
        &CameraTest::projectionSizeViewport<Float>,
        &CameraTest::projectionSizeViewport<Double>,

        &CameraTest::draw<Float>,
        &CameraTest::draw<Double>,
        &CameraTest::drawOrdered<Float>,
        &CameraTest::drawOrdered<Double>});
}

template<class T> using Object2D = SceneGraph::Object<SceneGraph::BasicMatrixTransformation2D<T>>;
template<class T> using Object3D = SceneGraph::Object<SceneGraph::BasicMatrixTransformation3D<T>>;
template<class T> using Scene3D = SceneGraph::Scene<SceneGraph::BasicMatrixTransformation3D<T>>;

template<class T> void CameraTest::fixAspectRatio() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Vector2<T> projectionScale(T(0.5), T(1.0)/T(3.0));
    Vector2i size(400, 300);

    /* Division by zero */
    Math::Vector2<T> projectionScaleZeroY(T(0.5), T(0.0));
    Math::Vector2<T> projectionScaleZeroX(T(0.0), T(0.5));
    Vector2i sizeZeroY(400, 0);
    Vector2i sizeZeroX(0, 300);
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, T>(AspectRatioPolicy::Clip, projectionScaleZeroX, size)), Math::Matrix4<T>{});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, T>(AspectRatioPolicy::Clip, projectionScaleZeroY, size)), Math::Matrix4<T>{});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, T>(AspectRatioPolicy::Clip, projectionScale, sizeZeroY)), Math::Matrix4<T>{});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, T>(AspectRatioPolicy::Extend, projectionScale, sizeZeroX)), Math::Matrix4<T>{});

    /* Not preserved */
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, T>(AspectRatioPolicy::NotPreserved, projectionScale, size)), Math::Matrix4<T>{});

    /* Clip */
    Math::Matrix4<T> expectedClip(
        {T(1.0),        T(0.0), T(0.0), T(0.0)},
        {T(0.0), T(4.0)/T(3.0), T(0.0), T(0.0)},
        {T(0.0),        T(0.0), T(1.0), T(0.0)},
        {T(0.0),        T(0.0), T(0.0), T(1.0)});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, T>(AspectRatioPolicy::Clip, Math::Vector2<T>{T(0.5)}, size)), expectedClip);
    Matrix4 expectedClipRectangle({1.0f, 0.0f, 0.0f, 0.0f},
                                  {0.0f, 2.0f, 0.0f, 0.0f},
                                  {0.0f, 0.0f, 1.0f, 0.0f},
                                  {0.0f, 0.0f, 0.0f, 1.0f});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, T>(AspectRatioPolicy::Clip, projectionScale, size)), Math::Matrix4<T>{expectedClipRectangle});

    /* Extend */
    Matrix4 expectedExtend({3.0f/4.0f, 0.0f, 0.0f, 0.0f},
                           {     0.0f, 1.0f, 0.0f, 0.0f},
                           {     0.0f, 0.0f, 1.0f, 0.0f},
                           {     0.0f, 0.0f, 0.0f, 1.0f});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, T>(AspectRatioPolicy::Extend, Math::Vector2<T>{T(0.5)}, size)), Math::Matrix4<T>{expectedExtend});
    Matrix4 expectedExtendRectangle({0.5f, 0.0f, 0.0f, 0.0f},
                                    {0.0f, 1.0f, 0.0f, 0.0f},
                                    {0.0f, 0.0f, 1.0f, 0.0f},
                                    {0.0f, 0.0f, 0.0f, 1.0f});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, T>(AspectRatioPolicy::Extend, projectionScale, size)), Math::Matrix4<T>{expectedExtendRectangle});
}

template<class T> void CameraTest::defaultProjection2D() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    BasicCamera2D<T> camera{o};
    CORRADE_COMPARE(camera.projectionMatrix(), Math::Matrix3<T>{});
    CORRADE_COMPARE(camera.projectionSize(), Math::Vector2<T>{T(2.0)});
}

template<class T> void CameraTest::defaultProjection3D() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    BasicCamera3D<T> camera{o};
    CORRADE_COMPARE(camera.projectionMatrix(), Math::Matrix4<T>{});
    CORRADE_COMPARE(camera.projectionSize(), Math::Vector2<T>{T(2.0)});
}

template<class T> void CameraTest::projectionCorrectedInvertedY() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object2D<T> o;
    BasicCamera2D<T> camera{o};
    camera.setProjectionMatrix(Math::Matrix3<T>::projection({T(4.0), T(-2.0)}))
        .setAspectRatioPolicy(AspectRatioPolicy::Extend)
        .setViewport({4, 4});

    /* Resulting matrix should have Y coordinate inverted */
    Matrix3 expected{{0.5f,  0.0f, 0.0f},
                     {0.0f, -0.5f, 0.0f},
                     {0.0f,  0.0f, 1.0f}};
    CORRADE_COMPARE(camera.projectionMatrix(), Math::Matrix3<T>{expected});
}

template<class T> void CameraTest::projectionSize2D() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Vector2<T> projectionSize{T(4.0), T(3.0)};
    Object2D<T> o;
    BasicCamera2D<T> camera{o};
    camera.setProjectionMatrix(Math::Matrix3<T>::projection(projectionSize));
    CORRADE_COMPARE(camera.projectionSize(), projectionSize);
}

template<class T> void CameraTest::projectionSizeOrthographic() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Vector2<T> projectionSizeRectangle{T(5.0), T(4.0)};
    Object3D<T> o;
    BasicCamera3D<T> camera{o};
    camera.setProjectionMatrix(Math::Matrix4<T>::orthographicProjection(projectionSizeRectangle, 1, 9));
    CORRADE_COMPARE(camera.projectionSize(), projectionSizeRectangle);
}

template<class T> void CameraTest::projectionSizePerspective() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    BasicCamera3D<T> camera{o};
    camera.setProjectionMatrix(Math::Matrix4<T>::perspectiveProjection(Math::Deg<T>(T(27.0)), T(2.35), T(32.0), 100));
    CORRADE_COMPARE(camera.projectionSize(), (Math::Vector2<T>{T(0.480157518160232), T(0.20432234815329)}));
}

template<class T> void CameraTest::projectionSizeViewport() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> o;
    BasicCamera3D<T> camera(o);
    camera.setViewport({200, 300});
    CORRADE_COMPARE(camera.projectionSize(), (Math::Vector2<T>{T(2.0), T(2.0)}));

    camera.setAspectRatioPolicy(AspectRatioPolicy::Extend);
    CORRADE_COMPARE(camera.projectionSize(), (Math::Vector2<T>{T(2.0), T(3.0)}));

    camera.setAspectRatioPolicy(AspectRatioPolicy::Clip);
    CORRADE_COMPARE(camera.projectionSize(), (Math::Vector2<T>{T(4.0)/T(3.0), T(2.0)}));
}

template<class T> void CameraTest::draw() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    class Drawable: public SceneGraph::BasicDrawable3D<T> {
        public:
            Drawable(AbstractBasicObject3D<T>& object, BasicDrawableGroup3D<T>* group, Math::Matrix4<T>& result): SceneGraph::BasicDrawable3D<T>{object, group}, result(result) {}

        protected:
            void draw(const Math::Matrix4<T>& transformationMatrix, BasicCamera3D<T>&) override {
                result = transformationMatrix;
            }

        private:
            Math::Matrix4<T>& result;
    };

    BasicDrawableGroup3D<T> group;
    Scene3D<T> scene;

    Object3D<T> first(&scene);
    Math::Matrix4<T> firstTransformation;
    first.scale(Math::Vector3<T>{T(5.0)});
    new Drawable{first, &group, firstTransformation};

    Object3D<T> second(&scene);
    Math::Matrix4<T> secondTransformation;
    second.translate(Math::Vector3<T>::yAxis(T(3.0)));
    new Drawable{second, &group, secondTransformation};

    Object3D<T> third(&second);
    Math::Matrix4<T> thirdTransformation;
    third.translate(Math::Vector3<T>::zAxis(T(-1.5)));
    new Drawable{third, &group, thirdTransformation};

    BasicCamera3D<T> camera{third};
    camera.draw(group);

    CORRADE_COMPARE(firstTransformation, Math::Matrix4<T>::translation({T(0.0), T(-3.0), T(1.5)})*Math::Matrix4<T>::scaling(Math::Vector3<T>(T(5.0))));
    CORRADE_COMPARE(secondTransformation, Math::Matrix4<T>::translation(Math::Vector3<T>::zAxis(T(1.5))));
    CORRADE_COMPARE(thirdTransformation, Math::Matrix4<T>{});
}

template<class T> void CameraTest::drawOrdered() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    class Drawable: public SceneGraph::BasicDrawable3D<T> {
        public:
            Drawable(AbstractBasicObject3D<T>& object, BasicDrawableGroup3D<T>* group, std::vector<Math::Matrix4<T>>& result): SceneGraph::BasicDrawable3D<T>{object, group}, _result(result) {}

        protected:
            void draw(const Math::Matrix4<T>& transformationMatrix, BasicCamera3D<T>&) override {
                _result.push_back(transformationMatrix);
            }

        private:
            std::vector<Math::Matrix4<T>>& _result;
    };

    BasicDrawableGroup3D<T> group;
    Scene3D<T> scene;

    std::vector<Math::Matrix4<T>> transformations;

    Object3D<T> first{&scene};
    first.scale(Math::Vector3<T>{T(5.0)})
        .translate(Math::Vector3<T>::zAxis(T(-1.0)));
    new Drawable{first, &group, transformations};

    Object3D<T> second{&scene};
    second.translate(Math::Vector3<T>::zAxis(T(3.0)));
    new Drawable{second, &group, transformations};

    Object3D<T> third{&second};
    third.translate(Math::Vector3<T>::zAxis(T(-1.5)));
    new Drawable{third, &group, transformations};

    BasicCamera3D<T> camera{third};

    std::vector<std::pair<std::reference_wrapper<SceneGraph::BasicDrawable3D<T>>, Math::Matrix4<T>>> drawableTransformations = camera.drawableTransformations(group);
    std::sort(drawableTransformations.begin(), drawableTransformations.end(),
        [](const std::pair<std::reference_wrapper<SceneGraph::BasicDrawable3D<T>>, Math::Matrix4<T>>& a,
           const std::pair<std::reference_wrapper<SceneGraph::BasicDrawable3D<T>>, Math::Matrix4<T>>& b) {
            return a.second.translation().z() < b.second.translation().z();
        });

    camera.draw(drawableTransformations);

    /* Should be ordered front to back, most negative Z first */
    CORRADE_COMPARE_AS(transformations, (std::vector<Math::Matrix4<T>>{
        Math::Matrix4<T>::translation(Math::Vector3<T>::zAxis(T(-2.5)))*Math::Matrix4<T>::scaling(Math::Vector3<T>{T(5.0)}), /* first */
        Math::Matrix4<T>{}, /* third */
        Math::Matrix4<T>::translation(Math::Vector3<T>::zAxis(T(1.5))) /* second */
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::CameraTest)
