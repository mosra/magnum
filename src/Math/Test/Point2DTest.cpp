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

#include <sstream>
#include <TestSuite/Tester.h>
#include <Utility/Configuration.h>

#include "Math/Point2D.h"

namespace Magnum { namespace Math { namespace Test {

class Point2DTest: public Corrade::TestSuite::Tester {
    public:
        Point2DTest();

        void construct();
        void debug();
        void configuration();
};

typedef Math::Point2D<float> Point2D;

Point2DTest::Point2DTest() {
    addTests(&Point2DTest::construct,
             &Point2DTest::debug,
             &Point2DTest::configuration);
}

void Point2DTest::construct() {
    CORRADE_COMPARE(Point2D(), (Vector<3, float>(0.0f, 0.0f, 1.0f)));
    CORRADE_COMPARE(Point2D(1, 2), (Vector<3, float>(1.0f, 2.0f, 1.0f)));
    CORRADE_COMPARE(Point2D(Vector<2, float>(1.0f, 2.0f), 3), (Vector<3, float>(1.0f, 2.0f, 3.0f)));
}

void Point2DTest::debug() {
    std::ostringstream o;
    Debug(&o) << Point2D(0.5f, 15.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1)\n");
}

void Point2DTest::configuration() {
    Corrade::Utility::Configuration c;

    Point2D vec(3.0f, 3.125f, 9.55f);
    std::string value("3 3.125 9.55");

    c.setValue("point", vec);
    CORRADE_COMPARE(c.value("point"), value);
    CORRADE_COMPARE(c.value<Point2D>("point"), vec);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Point2DTest)
