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

#include "Math/Angle.h"

namespace Magnum { namespace Math { namespace Test {

class AngleTest: public Corrade::TestSuite::Tester {
    public:
        explicit AngleTest();

        void construct();
        void literals();
        void conversion();

        void debugDeg();
        void debugRad();
};

typedef Math::Deg<float> Deg;
typedef Math::Rad<float> Rad;
typedef Math::Deg<double> Degd;
typedef Math::Rad<double> Radd;

AngleTest::AngleTest() {
    addTests(&AngleTest::construct,
             &AngleTest::literals,
             &AngleTest::conversion,

             &AngleTest::debugDeg,
             &AngleTest::debugRad);
}

void AngleTest::construct() {
    /* Default constructor */
    constexpr Degd a;
    constexpr Deg m;
    CORRADE_COMPARE(double(a), 0.0f);
    CORRADE_COMPARE(float(m), 0.0f);

    /* Value constructor */
    constexpr Deg b(25.0);
    constexpr Radd n(3.14);
    CORRADE_COMPARE(float(b), 25.0);
    CORRADE_COMPARE(double(n), 3.14);

    /* Copy constructor */
    constexpr Deg c(b);
    constexpr Radd o(n);
    CORRADE_COMPARE(c, b);
    CORRADE_COMPARE(o, n);

    /* Conversion operator */
    constexpr Degd d(b);
    constexpr Rad p(n);
    CORRADE_COMPARE(double(d), 25.0);
    CORRADE_COMPARE(float(p), 3.14f);
}

void AngleTest::literals() {
    constexpr auto a = 25.0_deg;
    constexpr auto b = 25.0_degf;
    CORRADE_VERIFY((std::is_same<decltype(a), const Degd>::value));
    CORRADE_VERIFY((std::is_same<decltype(b), const Deg>::value));
    CORRADE_COMPARE(double(a), 25.0);
    CORRADE_COMPARE(float(b), 25.0f);

    constexpr auto m = 3.14_rad;
    constexpr auto n = 3.14_radf;
    CORRADE_VERIFY((std::is_same<decltype(m), const Radd>::value));
    CORRADE_VERIFY((std::is_same<decltype(n), const Rad>::value));
    CORRADE_COMPARE(double(m), 3.14);
    CORRADE_COMPARE(float(n), 3.14f);
}

void AngleTest::conversion() {
    constexpr Deg a(Rad(1.57079633f));
    CORRADE_COMPARE(float(a), 90.0f);

    constexpr Rad b(Deg(90.0));
    CORRADE_COMPARE(float(b), 1.57079633f);
}

void AngleTest::debugDeg() {
    std::ostringstream o;

    Debug(&o) << Deg(90.0);
    CORRADE_COMPARE(o.str(), "Deg(90)\n");
}

void AngleTest::debugRad() {
    std::ostringstream o;

    Debug(&o) << Rad(1.5708);
    CORRADE_COMPARE(o.str(), "Rad(1.5708)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::AngleTest)
