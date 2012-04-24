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

#include "ShapeGroupTest.h"

#include <QtTest/QTest>

#include "Physics/Point.h"
#include "Physics/Sphere.h"
#include "Physics/ShapeGroup.h"

QTEST_APPLESS_MAIN(Magnum::Physics::Test::ShapeGroupTest)

using namespace std;

namespace Magnum { namespace Physics { namespace Test {

void ShapeGroupTest::copy() {
    ShapeGroup group;
    {
        Physics::Point point({1.0f, 2.0f, 3.0f});
        Physics::Sphere sphere({2.0f, 1.0f, 30.0f}, 1.0f);

        group = ~(point|sphere);
    }

    /* Just to test that it doesn't crash */
    group.applyTransformation(Matrix4::translation(Vector3::xAxis(1.0f)));
}

void ShapeGroupTest::reference() {
    Physics::Point point({1.0f, 2.0f, 3.0f});
    Physics::Sphere sphere({2.0f, 1.0f, 30.0f}, 1.0f);

    ShapeGroup group = ~(ref(point)|ref(sphere));

    group.applyTransformation(Matrix4::translation(Vector3(1.0f)));

    QVERIFY((point.transformedPosition() == Vector3(2.0f, 3.0f, 4.0f)));
    QVERIFY((sphere.transformedPosition() == Vector3(3.0f, 2.0f, 31.0f)));
}

}}}
