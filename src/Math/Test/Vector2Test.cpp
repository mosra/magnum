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

#include "Vector2Test.h"

#include <sstream>
#include <Utility/Configuration.h>

#include "Vector2.h"

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector2Test)

using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Math::Vector2<float> Vector2;

Vector2Test::Vector2Test() {
    addTests(&Vector2Test::construct,
             &Vector2Test::access,
             &Vector2Test::axes,
             &Vector2Test::scales,
             &Vector2Test::debug,
             &Vector2Test::configuration);
}

void Vector2Test::construct() {
    CORRADE_COMPARE(Vector2(1, 2), (Vector<2, float>(1.0f, 2.0f)));
}

void Vector2Test::access() {
    Vector2 vec(1.0f, -2.0f);
    const Vector2 cvec(1.0f, -2.0f);

    CORRADE_COMPARE(vec.x(), 1.0f);
    CORRADE_COMPARE(vec.y(), -2.0f);
    CORRADE_COMPARE(cvec.x(), 1.0f);
    CORRADE_COMPARE(cvec.y(), -2.0f);
}

void Vector2Test::axes() {
    CORRADE_COMPARE(Vector2::xAxis(5.0f), Vector2(5.0f, 0.0f));
    CORRADE_COMPARE(Vector2::yAxis(6.0f), Vector2(0.0f, 6.0f));
}

void Vector2Test::scales() {
    CORRADE_COMPARE(Vector2::xScale(-5.0f), Vector2(-5.0f, 1.0f));
    CORRADE_COMPARE(Vector2::yScale(-0.2f), Vector2(1.0f, -0.2f));
}

void Vector2Test::debug() {
    std::ostringstream o;
    Debug(&o) << Vector2(0.5f, 15.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15)\n");
}

void Vector2Test::configuration() {
    Configuration c;

    Vector2 vec(3.125f, 9.0f);
    std::string value("3.125 9");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector2>("vector"), vec);
}

}}}
