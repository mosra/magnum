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

#include <TestSuite/Tester.h>

#include "Math/Constants.h"
#include "Math/Functions.h"

namespace Magnum { namespace Math { namespace Test {

class ConstantsTest: public Corrade::TestSuite::Tester {
    public:
        ConstantsTest();

        void constants();
};

ConstantsTest::ConstantsTest() {
    addTests(&ConstantsTest::constants);
}

void ConstantsTest::constants() {
    CORRADE_COMPARE(Math::pow<2>(Constants<float>::sqrt2()), 2.0f);
    CORRADE_COMPARE(Math::pow<2>(Constants<float>::sqrt3()), 3.0f);

    CORRADE_COMPARE(Math::pow<2>(Constants<double>::sqrt2()), 2.0);
    CORRADE_COMPARE(Math::pow<2>(Constants<double>::sqrt3()), 3.0);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::ConstantsTest)
