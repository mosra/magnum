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

#include "AxisAlignedBoxTest.h"

#include <QtTest/QTest>

#include "Physics/AxisAlignedBox.h"

QTEST_APPLESS_MAIN(Magnum::Physics::Test::AxisAlignedBoxTest)

namespace Magnum { namespace Physics { namespace Test {

void AxisAlignedBoxTest::applyTransformation() {
    Physics::AxisAlignedBox box({-1.0f, -2.0f, -3.0f}, {1.0f, 2.0f, 3.0f});

    box.applyTransformation(Matrix4::scaling({2.0f, -1.0f, 1.5f}));
    QVERIFY((box.transformedPosition() == Vector3(-2.0f, 2.0f, -4.5f)));
    QVERIFY((box.transformedSize() == Vector3(2.0f, -2.0f, 4.5f)));

    box.applyTransformation(Matrix4::translation(Vector3(1.0f))*Matrix4::rotation(deg(90.0f), Vector3::xAxis()));
    QVERIFY((box.transformedPosition() == Vector3(0.0f, 4.0f, -1.0f)));
    QVERIFY((box.transformedSize() == Vector3(1.0f, -3.0f, 2.0f)));
}

}}}
