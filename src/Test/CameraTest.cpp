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

#include "Camera.h"
#include "Scene.h"

CORRADE_TEST_MAIN(Magnum::Test::CameraTest)

namespace Magnum { namespace Test {

CameraTest::CameraTest() {
    addTests(&CameraTest::orthographic,
             &CameraTest::perspective);
}

void CameraTest::orthographic() {
    Camera camera;
    camera.setOrthographic(5, 1, 9);

    Matrix4 a(0.4f,   0.0f,   0.0f,       0.0f,
              0.0f,   0.4f,   0.0f,       0.0f,
              0.0f,   0.0f,   -0.25f,     0.0f,
              0.0f,   0.0f,   -1.25f,     1.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), a);
}

void CameraTest::perspective() {
    Camera camera;
    camera.setPerspective(deg(27.0f), 32.0f, 100);

    Matrix4 a(4.1652994f, 0.0f,       0.0f,        0.0f,
              0.0f,       4.1652994f, 0.0f,        0.0f,
              0.0f,       0.0f,      -1.9411764f, -1.0f,
              0.0f,       0.0f,      -94.1176452f, 0.0f);

    CORRADE_COMPARE(camera.projectionMatrix(), a);
}

}}
