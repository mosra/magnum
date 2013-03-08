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

#include "Math/Vector3.h"

namespace Magnum { namespace Math { namespace Test {

class Vector3Test: public Corrade::TestSuite::Tester {
    public:
        Vector3Test();

        void construct();
        void constructDefault();
        void constructOneValue();
        void constructParts();
        void constructConversion();
        void constructCopy();

        void access();
        void cross();
        void axes();
        void scales();
        void twoComponent();

        void debug();
        void configuration();
};

typedef Math::Vector3<Float> Vector3;
typedef Math::Vector3<Int> Vector3i;
typedef Math::Vector2<Float> Vector2;

Vector3Test::Vector3Test() {
    addTests({&Vector3Test::construct,
              &Vector3Test::constructDefault,
              &Vector3Test::constructOneValue,
              &Vector3Test::constructParts,
              &Vector3Test::constructConversion,
              &Vector3Test::constructCopy,

              &Vector3Test::access,
              &Vector3Test::cross,
              &Vector3Test::axes,
              &Vector3Test::scales,
              &Vector3Test::twoComponent,

              &Vector3Test::debug,
              &Vector3Test::configuration});
}

void Vector3Test::construct() {
    constexpr Vector3 a(1.0f, 2.5f, -3.0f);
    CORRADE_COMPARE(a, (Vector<3, Float>(1.0f, 2.5f, -3.0f)));
}

void Vector3Test::constructDefault() {
    constexpr Vector3 a;
    CORRADE_COMPARE(a, Vector3(0.0f, 0.0f, 0.0f));
}

void Vector3Test::constructOneValue() {
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Vector3 a(-3.0f);
    #else
    Vector3 a(-3.0f); /* Not constexpr under GCC < 4.7 */
    #endif
    CORRADE_COMPARE(a, Vector3(-3.0f, -3.0f, -3.0f));
}

void Vector3Test::constructParts() {
    constexpr Vector2 a(1.0f, 2.0f);
    constexpr Vector3 b(a, 3.0f);
    CORRADE_COMPARE(b, Vector3(1.0f, 2.0f, 3.0f));
}

void Vector3Test::constructConversion() {
    constexpr Vector3 a(1.0f, 2.5f, -3.0f);
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Vector3i b(a);
    #else
    Vector3i b(a); /* Not constexpr under GCC < 4.7 */
    #endif
    CORRADE_COMPARE(b, Vector3i(1, 2, -3));
}

void Vector3Test::constructCopy() {
    constexpr Vector3 a(1.0f, 2.5f, -3.0f);
    constexpr Vector3 b(a);
    CORRADE_COMPARE(b, Vector3(1.0f, 2.5f, -3.0f));
}

void Vector3Test::access() {
    Vector3 vec(1.0f, -2.0f, 5.0f);
    CORRADE_COMPARE(vec.x(), 1.0f);
    CORRADE_COMPARE(vec.y(), -2.0f);
    CORRADE_COMPARE(vec.z(), 5.0f);

    constexpr Vector3 cvec(1.0f, -2.0f, 5.0f);
    constexpr Float x = cvec.x();
    constexpr Float y = cvec.y();
    constexpr Float z = cvec.z();
    CORRADE_COMPARE(x, 1.0f);
    CORRADE_COMPARE(y, -2.0f);
    CORRADE_COMPARE(z, 5.0f);
}

void Vector3Test::cross() {
    Vector3 a(1, -1, 1);
    Vector3 b(4, 3, 7);

    CORRADE_COMPARE(Vector3::cross(a, b), Vector3(-10, -3, 7));
}

void Vector3Test::axes() {
    constexpr Vector3 x = Vector3::xAxis(5.0f);
    constexpr Vector3 y = Vector3::yAxis(6.0f);
    constexpr Vector3 z = Vector3::zAxis(7.0f);
    CORRADE_COMPARE(x, Vector3(5.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(y, Vector3(0.0f, 6.0f, 0.0f));
    CORRADE_COMPARE(z, Vector3(0.0f, 0.0f, 7.0f));
}

void Vector3Test::scales() {
    constexpr Vector3 x = Vector3::xScale(-5.0f);
    constexpr Vector3 y = Vector3::yScale(-0.2f);
    constexpr Vector3 z = Vector3::zScale(71.0f);
    CORRADE_COMPARE(x, Vector3(-5.0f, 1.0f, 1.0f));
    CORRADE_COMPARE(y, Vector3(1.0f, -0.2f, 1.0f));
    CORRADE_COMPARE(z, Vector3(1.0f, 1.0f, 71.0f));
}

void Vector3Test::twoComponent() {
    Vector3 a(1.0f, 2.0f, 3.0f);
    CORRADE_COMPARE(a.xy(), Vector2(1.0f, 2.0f));

    constexpr Vector3 b(1.0f, 2.0f, 3.0f);
    constexpr Vector2 c = b.xy();
    constexpr Float d = b.xy().y();
    CORRADE_COMPARE(c, Vector2(1.0f, 2.0f));
    CORRADE_COMPARE(d, 2.0f);
}

void Vector3Test::debug() {
    std::ostringstream o;
    Debug(&o) << Vector3(0.5f, 15.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1)\n");
}

void Vector3Test::configuration() {
    Corrade::Utility::Configuration c;

    Vector3 vec(3.0f, 3.125f, 9.55f);
    std::string value("3 3.125 9.55");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector3>("vector"), vec);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector3Test)
