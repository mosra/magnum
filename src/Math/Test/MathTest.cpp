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

#include "MathTest.h"

#include "Math.h"

CORRADE_TEST_MAIN(Magnum::Math::Test::MathTest)

namespace Magnum { namespace Math { namespace Test {

MathTest::MathTest() {
    addTests(&MathTest::degrad,
             &MathTest::pow,
             &MathTest::log);
}

void MathTest::degrad() {
    CORRADE_COMPARE(deg(90.0), Constants<double>::pi()/2);
    CORRADE_COMPARE(deg(90.0f), Constants<float>::pi()/2);
    CORRADE_COMPARE(rad(Constants<double>::pi()/2), Constants<double>::pi()/2);
}

void MathTest::pow() {
    CORRADE_COMPARE(Math::pow<10>(2ul), 1024ul);
    CORRADE_COMPARE(Math::pow<0>(3ul), 1ul);
    CORRADE_COMPARE(Math::pow<2>(2.0f), 4.0f);
}

void MathTest::log() {
    CORRADE_COMPARE(Math::log(2, 256), 8ul);
    CORRADE_COMPARE(Math::log(256, 2), 0ul);
}

}}}
