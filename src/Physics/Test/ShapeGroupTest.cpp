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

#include "Math/Matrix4.h"
#include "Physics/Point.h"
#include "Physics/LineSegment.h"
#include "Physics/ShapeGroup.h"

using namespace std;

CORRADE_TEST_MAIN(Magnum::Physics::Test::ShapeGroupTest)

namespace Magnum { namespace Physics { namespace Test {

ShapeGroupTest::ShapeGroupTest() {
    addTests(&ShapeGroupTest::copy,
             &ShapeGroupTest::reference);
}

void ShapeGroupTest::copy() {
    ShapeGroup3D group;
    {
        Physics::Point3D point({1.0f, 2.0f, 3.0f});
        Physics::LineSegment3D segment({2.0f, 1.0f, 30.0f}, {1.0f, -20.0f, 3.0f});

        group = !(point || segment);
    }

    /* Just to test that it doesn't crash */
    group.applyTransformationMatrix(Matrix4::translation(Vector3::xAxis(1.0f)));

    CORRADE_VERIFY(true);
}

void ShapeGroupTest::reference() {
    Physics::Point3D point({1.0f, 2.0f, 3.0f});
    Physics::LineSegment3D segment({2.0f, 1.0f, 30.0f}, {1.0f, -20.0f, 3.0f});

    ShapeGroup3D group = !(ref(point) || ref(segment));

    group.applyTransformationMatrix(Matrix4::translation(Vector3(1.0f)));

    CORRADE_VERIFY((point.transformedPosition() == Vector3(2.0f, 3.0f, 4.0f)));
    CORRADE_VERIFY((segment.transformedA() == Vector3(3.0f, 2.0f, 31.0f)));
}

}}}
