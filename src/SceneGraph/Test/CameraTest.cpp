/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "CameraTest.h"

#include "Math/Constants.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/Camera.hpp" /* only for aspectRatioFix(), so it doesn't have to be exported */
#include "SceneGraph/Drawable.h"
#include "SceneGraph/MatrixTransformation2D.h"
#include "SceneGraph/MatrixTransformation3D.h"

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::CameraTest)

namespace Magnum { namespace SceneGraph { namespace Test {

typedef SceneGraph::Object<SceneGraph::MatrixTransformation2D<>> Object2D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D<>> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D<>> Scene3D;
typedef SceneGraph::Camera2D<> Camera2D;
typedef SceneGraph::Camera3D<> Camera3D;

CameraTest::CameraTest() {
    addTests(&CameraTest::fixAspectRatio,
             &CameraTest::defaultProjection2D,
             &CameraTest::defaultProjection3D,
             &CameraTest::projection2D,
             &CameraTest::orthographic,
             &CameraTest::perspective,
             &CameraTest::projectionSizeViewport,
             &CameraTest::draw);
}

void CameraTest::fixAspectRatio() {
    Vector2 projectionScale(0.5f, 1.0f/3.0f);
    Math::Vector2<GLsizei> size(400, 300);

    /* Division by zero */
    Vector2 projectionScaleZeroY(0.5f, 0.0f);
    Vector2 projectionScaleZeroX(0.0f, 0.5f);
    Math::Vector2<GLsizei> sizeZeroY(400, 0);
    Math::Vector2<GLsizei> sizeZeroX(0, 300);
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, GLfloat>(Implementation::AspectRatioPolicy::Clip, projectionScaleZeroX, size)), Matrix4());
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, GLfloat>(Implementation::AspectRatioPolicy::Clip, projectionScaleZeroY, size)), Matrix4());
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, GLfloat>(Implementation::AspectRatioPolicy::Clip, projectionScale, sizeZeroY)), Matrix4());
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, GLfloat>(Implementation::AspectRatioPolicy::Extend, projectionScale, sizeZeroX)), Matrix4());

    /* Not preserved */
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, GLfloat>(Implementation::AspectRatioPolicy::NotPreserved, projectionScale, size)), Matrix4());

    /* Clip */
    Matrix4 expectedClip(1.0f, 0.0f,      0.0f, 0.0f,
                         0.0f, 4.0f/3.0f, 0.0f, 0.0f,
                         0.0f, 0.0f,      1.0f, 0.0f,
                         0.0f, 0.0f,      0.0f, 1.0f);
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, GLfloat>(Implementation::AspectRatioPolicy::Clip, Vector2(0.5f), size)), expectedClip);
    Matrix4 expectedClipRectangle(1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 2.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f);
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, GLfloat>(Implementation::AspectRatioPolicy::Clip, projectionScale, size)), expectedClipRectangle);

    /* Extend */
    Matrix4 expectedExtend(3.0f/4.0f, 0.0f, 0.0f, 0.0f,
                           0.0f,      1.0f, 0.0f, 0.0f,
                           0.0f,      0.0f, 1.0f, 0.0f,
                           0.0f,      0.0f, 0.0f, 1.0f);
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, GLfloat>(Implementation::AspectRatioPolicy::Extend, Vector2(0.5f), size)), expectedExtend);
    Matrix4 expectedExtendRectangle(0.5f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f);
    CORRADE_COMPARE((Implementation::aspectRatioFix<3, GLfloat>(Implementation::AspectRatioPolicy::Extend, projectionScale, size)), expectedExtendRectangle);
}

void CameraTest::defaultProjection2D() {
    Object2D o;
    Camera2D camera(&o);
    CORRADE_COMPARE(camera.projectionMatrix(), Matrix3());
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f));
}

void CameraTest::defaultProjection3D() {
    Object3D o;
    Camera3D camera(&o);
    CORRADE_COMPARE(camera.projectionMatrix(), Matrix4());
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f));
}

void CameraTest::projection2D() {
    Vector2 projectionSize(4.0f, 3.0f);
    Object2D o;
    Camera2D camera(&o);
    camera.setProjection(projectionSize);

    Matrix3 a(2.0f/4.0f,    0.0f,       0.0f,
              0.0f,         2.0f/3.0f,  0.0f,
              0.0f,         0.0f,       1.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), a);
    CORRADE_COMPARE(camera.projectionSize(), projectionSize);
}

void CameraTest::orthographic() {
    Vector2 projectionSize(5);
    Object3D o;
    Camera3D camera(&o);
    camera.setOrthographic(projectionSize, 1, 9);

    Matrix4 a(0.4f,   0.0f,   0.0f,       0.0f,
              0.0f,   0.4f,   0.0f,       0.0f,
              0.0f,   0.0f,   -0.25f,     0.0f,
              0.0f,   0.0f,   -1.25f,     1.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), a);
    CORRADE_COMPARE(camera.projectionSize(), projectionSize);

    Vector2 projectionSizeRectangle(5.0f, 4.0f);
    camera.setOrthographic(projectionSizeRectangle, 1, 9);

    Matrix4 rectangle(0.4f,   0.0f,   0.0f,       0.0f,
                      0.0f,   0.5f,   0.0f,       0.0f,
                      0.0f,   0.0f,   -0.25f,     0.0f,
                      0.0f,   0.0f,   -1.25f,     1.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), rectangle);
    CORRADE_COMPARE(camera.projectionSize(), projectionSizeRectangle);
}

void CameraTest::perspective() {
    Object3D o;
    Camera3D camera(&o);
    camera.setPerspective(deg(27.0f), 32.0f, 100);

    Matrix4 a(4.1652994f, 0.0f,       0.0f,        0.0f,
              0.0f,       4.1652994f, 0.0f,        0.0f,
              0.0f,       0.0f,      -1.9411764f, -1.0f,
              0.0f,       0.0f,      -94.1176452f, 0.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), a);
    CORRADE_COMPARE(camera.projectionSize(), Vector2(0.48015756f));
}

void CameraTest::projectionSizeViewport() {
    Object3D o;
    Camera3D camera(&o);
    camera.setViewport({200, 300});
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f, 2.0f));

    camera.setAspectRatioPolicy(Camera3D::AspectRatioPolicy::Extend);
    CORRADE_COMPARE(camera.projectionSize(), Vector2(2.0f, 3.0f));

    camera.setAspectRatioPolicy(Camera3D::AspectRatioPolicy::Clip);
    CORRADE_COMPARE(camera.projectionSize(), Vector2(4.0f/3.0f, 2.0f));
}

void CameraTest::draw() {
    class Drawable: public SceneGraph::Drawable<3> {
        public:
            inline Drawable(AbstractObject<3>* object, DrawableGroup<3>* group, Matrix4& result): SceneGraph::Drawable<3>(object, group), result(result) {}

        protected:
            void draw(const Matrix4& transformationMatrix, AbstractCamera<3>*) {
                result = transformationMatrix;
            }

        private:
            Matrix4& result;
    };

    DrawableGroup<3> group;
    Scene3D scene;

    Object3D first(&scene);
    Matrix4 firstTransformation;
    first.scale(Vector3(5.0f));
    new Drawable(&first, &group, firstTransformation);

    Object3D second(&scene);
    Matrix4 secondTransformation;
    second.translate(Vector3::yAxis(3.0f));
    new Drawable(&second, &group, secondTransformation);

    Object3D third(&second);
    Matrix4 thirdTransformation;
    third.translate(Vector3::zAxis(-1.5f));
    new Drawable(&third, &group, thirdTransformation);

    Camera3D camera(&third);
    camera.draw(group);

    CORRADE_COMPARE(firstTransformation, Matrix4::translation({0.0f, -3.0f, 1.5f})*Matrix4::scaling(Vector3(5.0f)));
    CORRADE_COMPARE(secondTransformation, Matrix4::translation(Vector3::zAxis(1.5f)));
    CORRADE_COMPARE(thirdTransformation, Matrix4());
}

}}}
