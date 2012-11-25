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

#include "Math/Constants.h"
#include "Math/Matrix4.h"
#include "Physics/AxisAlignedBox.h"

CORRADE_TEST_MAIN(Magnum::Physics::Test::AxisAlignedBoxTest)

namespace Magnum { namespace Physics { namespace Test {

AxisAlignedBoxTest::AxisAlignedBoxTest() {
    addTests(&AxisAlignedBoxTest::applyTransformation);
}

void AxisAlignedBoxTest::applyTransformation() {
    Physics::AxisAlignedBox3D box({-1.0f, -2.0f, -3.0f}, {1.0f, 2.0f, 3.0f});

    box.applyTransformationMatrix(Matrix4::scaling({2.0f, -1.0f, 1.5f}));
    CORRADE_COMPARE(box.transformedPosition(), Vector3(-2.0f, 2.0f, -4.5f));
    CORRADE_COMPARE(box.transformedSize(), Vector3(2.0f, -2.0f, 4.5f));

    box.applyTransformationMatrix(Matrix4::translation(Vector3(1.0f))*Matrix4::rotation(deg(90.0f), Vector3::xAxis()));
    CORRADE_COMPARE(box.transformedPosition(), Vector3(0.0f, 4.0f, -1.0f));
    CORRADE_COMPARE(box.transformedSize(), Vector3(1.0f, -3.0f, 2.0f));
}

}}}
