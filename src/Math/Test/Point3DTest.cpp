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

#include "Point3DTest.h"

#include <Utility/Configuration.h>

#include "Point3D.h"

CORRADE_TEST_MAIN(Magnum::Math::Test::Point3DTest)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Math::Point3D<float> Point3D;

Point3DTest::Point3DTest() {
    addTests(&Point3DTest::construct,
             &Point3DTest::debug,
             &Point3DTest::configuration);
}

void Point3DTest::construct() {
    CORRADE_COMPARE(Point3D(), Point3D(0.0f, 0.0f, 0.0f, 1.0f));
    CORRADE_COMPARE(Point3D(1, 2, 3, 4), (Vector<4, float>(1.0f, 2.0f, 3.0f, 4.0f)));
    CORRADE_COMPARE(Point3D(Vector<3, float>(1.0f, 2.0f, 3.0f), 4), (Vector<4, float>(1.0f, 2.0f, 3.0f, 4.0f)));
}

void Point3DTest::debug() {
    ostringstream o;
    Debug(&o) << Point3D(0.5f, 15.0f, 1.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1, 1)\n");
}

void Point3DTest::configuration() {
    Configuration c;

    Point3D vec(3.0f, 3.125f, 9.0f, 9.55f);
    string value("3 3.125 9 9.55");

    c.setValue("point", vec);
    CORRADE_COMPARE(c.value<std::string>("point"), value);
    CORRADE_COMPARE(c.value<Point3D>("point"), vec);
}

}}}
