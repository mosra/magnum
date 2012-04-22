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

#include "LightTest.h"

#include <sstream>
#include <QtTest/QTest>

#include "Camera.h"
#include "Light.h"
#include "Scene.h"

QTEST_APPLESS_MAIN(Magnum::Test::LightTest)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Test {

void LightTest::positionWrongCamera() {
    stringstream ss;
    Error::setOutput(&ss);

    Camera c;
    Light l;
    QVERIFY(l.position(&c) == Vector3());
    QVERIFY(ss.str() == "Light: camera and light aren't in the same scene!\n");
}

void LightTest::position() {
    Scene s;

    Object lightParent(&s);
    lightParent.translate(Vector3::zAxis(3));
    Light light(&lightParent);

    Object cameraParent(&s);
    cameraParent.rotate(deg(90.0f), Vector3::xAxis());
    Camera camera(&cameraParent);

    QVERIFY(light.position(&camera) == (Matrix4::translation(Vector3::zAxis(3))*
        Matrix4::rotation(deg(90.0f), Vector3::xAxis()).inverted())[3].xyz());

    /* Set another camera */
    Camera another(&cameraParent);
    another.scale(Vector3(3.0f));
    QVERIFY(light.position(&another) == (Matrix4::translation(Vector3::zAxis(3))*
        (Matrix4::scaling(Vector3(3.0f))*Matrix4::rotation(deg(90.0f), Vector3::xAxis())).inverted())[3].xyz());
}

}}
