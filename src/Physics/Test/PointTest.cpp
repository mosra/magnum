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

#include <TestSuite/Tester.h>

#include "Math/Matrix4.h"
#include "Physics/Point.h"

namespace Magnum { namespace Physics { namespace Test {

class PointTest: public Corrade::TestSuite::Tester {
    public:
        PointTest();

        void applyTransformation();
};

PointTest::PointTest() {
    addTests({&PointTest::applyTransformation});
}

void PointTest::applyTransformation() {
    Physics::Point3D point({1.0f, 2.0f, 3.0f});
    point.applyTransformationMatrix(Matrix4::translation({5.0f, 6.0f, 7.0f}));
    CORRADE_COMPARE(point.transformedPosition(), Vector3(6.0f, 8.0f, 10.0f));
}

}}}

CORRADE_TEST_MAIN(Magnum::Physics::Test::PointTest)
