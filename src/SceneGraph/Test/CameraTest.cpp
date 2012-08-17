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

#include "SceneGraph/Camera.h"

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::CameraTest)

namespace Magnum { namespace SceneGraph { namespace Test {

CameraTest::CameraTest() {
    addTests(&CameraTest::fixAspectRatio,
             &CameraTest::orthographic,
             &CameraTest::perspective);
}

void CameraTest::fixAspectRatio() {
    Vector2 projectionSize(2, 3);
    Math::Vector2<GLsizei> size(400, 300);

    /* Division by zero */
    Vector2 projectionSizeZeroY(2, 0);
    Vector2 projectionSizeZeroX(0, 2);
    Math::Vector2<GLsizei> sizeZeroY(400, 0);
    Math::Vector2<GLsizei> sizeZeroX(0, 300);
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Clip, projectionSizeZeroX, size), Matrix4());
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Clip, projectionSizeZeroY, size), Matrix4());
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Clip, projectionSize, sizeZeroY), Matrix4());
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Extend, projectionSize, sizeZeroX), Matrix4());

    /* Not preserved */
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::NotPreserved, projectionSize, size), Matrix4());

    /* Clip */
    Matrix4 expectedClip(1.0f, 0.0f,      0.0f, 0.0f,
                         0.0f, 4.0f/3.0f, 0.0f, 0.0f,
                         0.0f, 0.0f,      1.0f, 0.0f,
                         0.0f, 0.0f,      0.0f, 1.0f);
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Clip, Vector2(2.0f), size), expectedClip);
    Matrix4 expectedClipRectangle(1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 2.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f);
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Clip, projectionSize, size), expectedClipRectangle);

    /* Extend */
    Matrix4 expectedExtend(3.0f/4.0f, 0.0f, 0.0f, 0.0f,
                           0.0f,      1.0f, 0.0f, 0.0f,
                           0.0f,      0.0f, 1.0f, 0.0f,
                           0.0f,      0.0f, 0.0f, 1.0f);
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Extend, Vector2(2.0f), size), expectedExtend);
    Matrix4 expectedExtendRectangle(0.5f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f);
    CORRADE_COMPARE(Implementation::aspectRatioFix<Matrix4>(Implementation::AspectRatioPolicy::Extend, projectionSize, size), expectedExtendRectangle);
}

void CameraTest::orthographic() {
    Camera3D camera;
    camera.setOrthographic(Vector2(5), 1, 9);

    Matrix4 a(0.4f,   0.0f,   0.0f,       0.0f,
              0.0f,   0.4f,   0.0f,       0.0f,
              0.0f,   0.0f,   -0.25f,     0.0f,
              0.0f,   0.0f,   -1.25f,     1.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), a);

    camera.setOrthographic(Vector2(5.0f, 4.0f), 1, 9);

    Matrix4 rectangle(0.4f,   0.0f,   0.0f,       0.0f,
                      0.0f,   0.5f,   0.0f,       0.0f,
                      0.0f,   0.0f,   -0.25f,     0.0f,
                      0.0f,   0.0f,   -1.25f,     1.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), rectangle);
}

void CameraTest::perspective() {
    Camera3D camera;
    camera.setPerspective(deg(27.0f), 32.0f, 100);

    Matrix4 a(4.1652994f, 0.0f,       0.0f,        0.0f,
              0.0f,       4.1652994f, 0.0f,        0.0f,
              0.0f,       0.0f,      -1.9411764f, -1.0f,
              0.0f,       0.0f,      -94.1176452f, 0.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), a);
}

}}}
