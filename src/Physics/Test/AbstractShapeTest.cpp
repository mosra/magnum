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

#include "AbstractShapeTest.h"

#include <sstream>
#include <Utility/Debug.h>

#include "Physics/AbstractShape.h"

CORRADE_TEST_MAIN(Magnum::Physics::Test::AbstractShapeTest)

namespace Magnum { namespace Physics { namespace Test {

AbstractShapeTest::AbstractShapeTest() {
    addTests(&AbstractShapeTest::debug);
}

void AbstractShapeTest::debug() {
    std::ostringstream o;
    Debug(&o) << AbstractShape2D::Type::ShapeGroup;
    CORRADE_COMPARE(o.str(), "AbstractShape2D::Type::ShapeGroup\n");

    o.str("");
    Debug(&o) << AbstractShape3D::Type::Plane;
    CORRADE_COMPARE(o.str(), "AbstractShape3D::Type::Plane\n");
}

}}}
