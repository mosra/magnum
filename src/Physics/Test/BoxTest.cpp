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

#include "BoxTest.h"

#include "Math/Matrix4.h"
#include "Physics/Box.h"

CORRADE_TEST_MAIN(Magnum::Physics::Test::BoxTest)

namespace Magnum { namespace Physics { namespace Test {

BoxTest::BoxTest() {
    addTests(&BoxTest::applyTransformation);
}

void BoxTest::applyTransformation() {
    Physics::Box3D box(Matrix4::translation({1.0f, 2.0f, -3.0f}));

    box.applyTransformation(Matrix4::scaling({2.0f, -1.0f, 1.5f}));
    CORRADE_COMPARE(box.transformedTransformation(), Matrix4::scaling({2.0f, -1.0f, 1.5f})*Matrix4::translation({1.0f, 2.0f, -3.0f}));
}

}}}
