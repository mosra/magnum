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

#include "PointTest.h"

#include <QtTest/QTest>

#include "Physics/Point.h"

QTEST_APPLESS_MAIN(Magnum::Physics::Test::PointTest)

namespace Magnum { namespace Physics { namespace Test {

void PointTest::applyTransformation() {
    Physics::Point point({1.0f, 2.0f, 3.0f});
    point.applyTransformation(Matrix4::translation({5.0f, 6.0f, 7.0f}));
    QVERIFY((point.transformedPosition() == Vector3(6.0f, 8.0f, 10.0f)));
}

}}}
