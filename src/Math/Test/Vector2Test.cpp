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

#include "Math/Vector2.h"

namespace Magnum { namespace Math { namespace Test {

class Vector2Test: public Corrade::TestSuite::Tester {
    public:
        Vector2Test();

        void construct();
        void constructDefault();
        void constructOneValue();
        void constructConversion();
        void constructCopy();

        void access();
        void axes();
        void scales();

        void debug();
        void configuration();
};

typedef Math::Vector2<Float> Vector2;
typedef Math::Vector2<Int> Vector2i;

Vector2Test::Vector2Test() {
    addTests({&Vector2Test::construct,
              &Vector2Test::constructDefault,
              &Vector2Test::constructOneValue,
              &Vector2Test::constructConversion,
              &Vector2Test::constructCopy,

              &Vector2Test::access,
              &Vector2Test::axes,
              &Vector2Test::scales,

              &Vector2Test::debug,
              &Vector2Test::configuration});
}

void Vector2Test::construct() {
    constexpr Vector2 a(1.5f, 2.5f);
    CORRADE_COMPARE(a, (Vector<2, Float>(1.5f, 2.5f)));
}

void Vector2Test::constructDefault() {
    constexpr Vector2 a;
    CORRADE_COMPARE(a, Vector2(0.0f, 0.0f));
}

void Vector2Test::constructOneValue() {
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Vector2 a(3.0f);
    #else
    Vector2 a(3.0f); /* Not constexpr under GCC < 4.7 */
    #endif
    CORRADE_COMPARE(a, Vector2(3.0f, 3.0f));
}

void Vector2Test::constructConversion() {
    constexpr Vector2 a(1.5f, 2.5f);
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Vector2i b(a);
    #else
    Vector2i b(a); /* Not constexpr under GCC < 4.7 */
    #endif
    CORRADE_COMPARE(b, Vector2i(1, 2));
}

void Vector2Test::constructCopy() {
    constexpr Vector2 a(1.5f, 2.5f);
    constexpr Vector2 b(a);
    CORRADE_COMPARE(b, Vector2(1.5f, 2.5f));
}

void Vector2Test::access() {
    Vector2 vec(1.0f, -2.0f);
    CORRADE_COMPARE(vec.x(), 1.0f);
    CORRADE_COMPARE(vec.y(), -2.0f);

    constexpr Vector2 cvec(1.0f, -2.0f);
    constexpr Float x = cvec.x();
    constexpr Float y = cvec.y();
    CORRADE_COMPARE(x, 1.0f);
    CORRADE_COMPARE(y, -2.0f);
}

void Vector2Test::axes() {
    constexpr Vector2 x = Vector2::xAxis(5.0f);
    constexpr Vector2 y = Vector2::yAxis(6.0f);
    CORRADE_COMPARE(x, Vector2(5.0f, 0.0f));
    CORRADE_COMPARE(y, Vector2(0.0f, 6.0f));
}

void Vector2Test::scales() {
    constexpr Vector2 x = Vector2::xScale(-5.0f);
    constexpr Vector2 y = Vector2::yScale(-0.2f);
    CORRADE_COMPARE(x, Vector2(-5.0f, 1.0f));
    CORRADE_COMPARE(y, Vector2(1.0f, -0.2f));
}

void Vector2Test::debug() {
    std::ostringstream o;
    Debug(&o) << Vector2(0.5f, 15.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15)\n");
}

void Vector2Test::configuration() {
    Corrade::Utility::Configuration c;

    Vector2 vec(3.125f, 9.0f);
    std::string value("3.125 9");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector2>("vector"), vec);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector2Test)
