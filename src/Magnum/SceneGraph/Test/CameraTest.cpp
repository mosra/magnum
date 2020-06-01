/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/SceneGraph/Camera.hpp" /* only for aspectRatioFix(), so it doesn't have to be exported */
#include "Magnum/SceneGraph/Camera.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/SceneGraph/MatrixTransformation2D.h"
#include "Magnum/SceneGraph/MatrixTransformation3D.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct CameraTest: TestSuite::Tester {
    explicit CameraTest();

    void fixAspectRatio();
    void defaultProjection2D();
    void defaultProjection3D();
    void projectionCorrectedInvertedY();
    void projectionSize2D();
    void projectionSizeOrthographic();
    void projectionSizePerspective();
    void projectionSizeViewport();

    void draw();
    void drawOrdered();
};

typedef SceneGraph::Object<SceneGraph::MatrixTransformation2D> Object2D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

CameraTest::CameraTest() {
    addTests({&CameraTest::fixAspectRatio,
              &CameraTest::defaultProjection2D,
              &CameraTest::defaultProjection3D,
              &CameraTest::projectionCorrectedInvertedY,
              &CameraTest::projectionSize2D,
              &CameraTest::projectionSizeOrthographic,
              &CameraTest::projectionSizePerspective,
              &CameraTest::projectionSizeViewport,

              &CameraTest::draw,
              &CameraTest::drawOrdered});
}

void CameraTest::fixAspectRatio() {
    Vector2 projectionScale(0.5f, 1.0f/3.0f);
    Vector2i size(400, 300);

    /* Division by zero */
    Vector2 projectionScaleZeroY(0.5f, 0.0f);
    Vector2 projectionScaleZeroX(0.0f, 0.5f);
    Vector2i sizeZeroY(400, 0);
    Vector2i sizeZeroX(0, 300);
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, Float>(AspectRatioPolicy::Clip, projectionScaleZeroX, size)), Matrix4());
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, Float>(AspectRatioPolicy::Clip, projectionScaleZeroY, size)), Matrix4());
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, Float>(AspectRatioPolicy::Clip, projectionScale, sizeZeroY)), Matrix4());
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, Float>(AspectRatioPolicy::Extend, projectionScale, sizeZeroX)), Matrix4());

    /* Not preserved */
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, Float>(AspectRatioPolicy::NotPreserved, projectionScale, size)), Matrix4());

    /* Clip */
    Matrix4 expectedClip({1.0f,      0.0f, 0.0f, 0.0f},
                         {0.0f, 4.0f/3.0f, 0.0f, 0.0f},
                         {0.0f,      0.0f, 1.0f, 0.0f},
                         {0.0f,      0.0f, 0.0f, 1.0f});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, Float>(AspectRatioPolicy::Clip, Vector2(0.5f), size)), expectedClip);
    Matrix4 expectedClipRectangle({1.0f, 0.0f, 0.0f, 0.0f},
                                  {0.0f, 2.0f, 0.0f, 0.0f},
                                  {0.0f, 0.0f, 1.0f, 0.0f},
                                  {0.0f, 0.0f, 0.0f, 1.0f});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, Float>(AspectRatioPolicy::Clip, projectionScale, size)), expectedClipRectangle);

    /* Extend */
    Matrix4 expectedExtend({3.0f/4.0f, 0.0f, 0.0f, 0.0f},
                           {     0.0f, 1.0f, 0.0f, 0.0f},
                           {     0.0f, 0.0f, 1.0f, 0.0f},
                           {     0.0f, 0.0f, 0.0f, 1.0f});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, Float>(AspectRatioPolicy::Extend, Vector2(0.5f), size)), expectedExtend);
    Matrix4 expectedExtendRectangle({0.5f, 0.0f, 0.0f, 0.0f},
                                    {0.0f, 1.0f, 0.0f, 0.0f},
                                    {0.0f, 0.0f, 1.0f, 0.0f},
                                    {0.0f, 0.0f, 0.0f, 1.0f});
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, Float>(AspectRatioPolicy::Extend, projectionScale, size)), expectedExtendRectangle);
}

void CameraTest::defaultProjection2D() {
    Object2D o;
    Camera2D camera(o);
    CORRADE_COMPARE(camera.projectionMatrix(), Matrix3());
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f));
}

void CameraTest::defaultProjection3D() {
    Object3D o;
    Camera3D camera(o);
    CORRADE_COMPARE(camera.projectionMatrix(), Matrix4());
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f));
}

void CameraTest::projectionCorrectedInvertedY() {
    Object2D o;
    Camera2D camera(o);
    camera.setProjectionMatrix(Matrix3::projection({4.0f, -2.0f}))
        .setAspectRatioPolicy(AspectRatioPolicy::Extend)
        .setViewport({4, 4});

    /* Resulting matrix should have Y coordinate inverted */
    Matrix3 expected{{0.5f,  0.0f, 0.0f},
                     {0.0f, -0.5f, 0.0f},
                     {0.0f,  0.0f, 1.0f}};
    CORRADE_COMPARE(camera.projectionMatrix(), expected);
}

void CameraTest::projectionSize2D() {
    Vector2 projectionSize(4.0f, 3.0f);
    Object2D o;
    Camera2D camera(o);
    camera.setProjectionMatrix(Matrix3::projection(projectionSize));
    CORRADE_COMPARE(camera.projectionSize(), projectionSize);
}

void CameraTest::projectionSizeOrthographic() {
    Vector2 projectionSizeRectangle(5.0f, 4.0f);
    Object3D o;
    Camera3D camera(o);
    camera.setProjectionMatrix(Matrix4::orthographicProjection(projectionSizeRectangle, 1, 9));
    CORRADE_COMPARE(camera.projectionSize(), projectionSizeRectangle);
}

void CameraTest::projectionSizePerspective() {
    Object3D o;
    Camera3D camera(o);
    camera.setProjectionMatrix(Matrix4::perspectiveProjection(Deg(27.0f), 2.35f, 32.0f, 100));
    CORRADE_COMPARE(camera.projectionSize(), Vector2(0.48015756f, 0.204322f));
}

void CameraTest::projectionSizeViewport() {
    Object3D o;
    Camera3D camera(o);
    camera.setViewport({200, 300});
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f, 2.0f));

    camera.setAspectRatioPolicy(AspectRatioPolicy::Extend);
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f, 3.0f));

    camera.setAspectRatioPolicy(AspectRatioPolicy::Clip);
    CORRADE_COMPARE(camera.projectionSize(), Vector2(4.0f/3.0f, 2.0f));
}

void CameraTest::draw() {
    class Drawable: public SceneGraph::Drawable3D {
        public:
            Drawable(AbstractObject3D& object, DrawableGroup3D* group, Matrix4& result): SceneGraph::Drawable3D(object, group), result(result) {}

        protected:
            void draw(const Matrix4& transformationMatrix, Camera3D&) override {
                result = transformationMatrix;
            }

        private:
            Matrix4& result;
    };

    DrawableGroup3D group;
    Scene3D scene;

    Object3D first(&scene);
    Matrix4 firstTransformation;
    first.scale(Vector3(5.0f));
    new Drawable(first, &group, firstTransformation);

    Object3D second(&scene);
    Matrix4 secondTransformation;
    second.translate(Vector3::yAxis(3.0f));
    new Drawable(second, &group, secondTransformation);

    Object3D third(&second);
    Matrix4 thirdTransformation;
    third.translate(Vector3::zAxis(-1.5f));
    new Drawable(third, &group, thirdTransformation);

    Camera3D camera(third);
    camera.draw(group);

    CORRADE_COMPARE(firstTransformation, Matrix4::translation({0.0f, -3.0f, 1.5f})*Matrix4::scaling(Vector3(5.0f)));
    CORRADE_COMPARE(secondTransformation, Matrix4::translation(Vector3::zAxis(1.5f)));
    CORRADE_COMPARE(thirdTransformation, Matrix4());
}

void CameraTest::drawOrdered() {
    class Drawable: public SceneGraph::Drawable3D {
        public:
            Drawable(AbstractObject3D& object, DrawableGroup3D* group, std::vector<Matrix4>& result): SceneGraph::Drawable3D(object, group), _result(result) {}

        protected:
            void draw(const Matrix4& transformationMatrix, Camera3D&) override {
                _result.push_back(transformationMatrix);
            }

        private:
            std::vector<Matrix4>& _result;
    };

    DrawableGroup3D group;
    Scene3D scene;

    std::vector<Matrix4> transformations;

    Object3D first(&scene);
    first.scale(Vector3(5.0f))
        .translate(Vector3::zAxis(-1.0f));
    new Drawable{first, &group, transformations};

    Object3D second(&scene);
    second.translate(Vector3::zAxis(3.0f));
    new Drawable{second, &group, transformations};

    Object3D third(&second);
    third.translate(Vector3::zAxis(-1.5f));
    new Drawable{third, &group, transformations};

    Camera3D camera(third);

    std::vector<std::pair<std::reference_wrapper<SceneGraph::Drawable3D>, Matrix4>> drawableTransformations = camera.drawableTransformations(group);
    std::sort(drawableTransformations.begin(), drawableTransformations.end(),
        [](const std::pair<std::reference_wrapper<SceneGraph::Drawable3D>, Matrix4>& a,
           const std::pair<std::reference_wrapper<SceneGraph::Drawable3D>, Matrix4>& b) {
            return a.second.translation().z() < b.second.translation().z();
        });

    camera.draw(drawableTransformations);

    /* Should be ordered front to back, most negative Z first */
    CORRADE_COMPARE_AS(transformations, (std::vector<Matrix4>{
        Matrix4::translation(Vector3::zAxis(-2.5f))*Matrix4::scaling(Vector3{5.0f}), /* first */
        Matrix4{}, /* third */
        Matrix4::translation(Vector3::zAxis(1.5f)) /* second */
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::CameraTest)
