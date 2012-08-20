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

#include "Vector3Test.h"

#include <sstream>

#include "Vector3.h"

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector3Test)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Math::Vector3<float> Vector3;
typedef Math::Vector2<float> Vector2;

Vector3Test::Vector3Test() {
    addTests(&Vector3Test::construct,
             &Vector3Test::cross,
             &Vector3Test::axes,
             &Vector3Test::scales,
             &Vector3Test::twoComponent,
             &Vector3Test::debug);
}

void Vector3Test::construct() {
    CORRADE_COMPARE(Vector3(1, 2, 3), (Vector<3, float>(1.0f, 2.0f, 3.0f)));
    CORRADE_COMPARE(Vector3(Vector<2, float>(1.0f, 2.0f), 3), (Vector<3, float>(1.0f, 2.0f, 3.0f)));
}

void Vector3Test::cross() {
    Vector3 a(1, -1, 1);
    Vector3 b(4, 3, 7);

    CORRADE_COMPARE(Vector3::cross(a, b), Vector3(-10, -3, 7));
}

void Vector3Test::axes() {
    CORRADE_COMPARE(Vector3::xAxis(5.0f), Vector3(5.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(Vector3::yAxis(6.0f), Vector3(0.0f, 6.0f, 0.0f));
    CORRADE_COMPARE(Vector3::zAxis(7.0f), Vector3(0.0f, 0.0f, 7.0f));
}

void Vector3Test::scales() {
    CORRADE_COMPARE(Vector3::xScale(-5.0f), Vector3(-5.0f, 1.0f, 1.0f));
    CORRADE_COMPARE(Vector3::yScale(-0.2f), Vector3(1.0f, -0.2f, 1.0f));
    CORRADE_COMPARE(Vector3::zScale(71.0f), Vector3(1.0f, 1.0f, 71.0f));
}

void Vector3Test::twoComponent() {
    CORRADE_COMPARE(Vector3(1.0f, 2.0f, 3.0f).xy(), Vector2(1.0f, 2.0f));
}

void Vector3Test::debug() {
    ostringstream o;
    Debug(&o) << Vector3(0.5f, 15.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1)\n");
}

}}}
