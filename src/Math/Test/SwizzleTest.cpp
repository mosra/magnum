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

#include "Math/Swizzle.h"

namespace Magnum { namespace Math { namespace Test {

class SwizzleTest: public Corrade::TestSuite::Tester {
    public:
        SwizzleTest();

        void components();
        void constants();
        void sizes();
        void constExpressions();
};

typedef Vector<4, std::int32_t> Vector4i;

SwizzleTest::SwizzleTest() {
    addTests(&SwizzleTest::components,
             &SwizzleTest::constants,
             &SwizzleTest::sizes,
             &SwizzleTest::constExpressions);
}

void SwizzleTest::components() {
    CORRADE_COMPARE((swizzle<'z', 'x', 'w', 'y'>(Vector4i(2, 4, 5, 7))), Vector4i(5, 2, 7, 4));
}

void SwizzleTest::constants() {
    CORRADE_COMPARE((swizzle<'1', 'w', '0', 'y'>(Vector4i(2, 4, 5, 7))), Vector4i(1, 7, 0, 4));
}

void SwizzleTest::sizes() {
    CORRADE_COMPARE((swizzle<'y', 'x', 'x'>(Math::Vector<2, std::int32_t>(1, 2))),
        (Math::Vector<3, std::int32_t>(2, 1, 1)));
    CORRADE_COMPARE(swizzle<'z'>(Vector4i(1, 2, 3, 4)),
        (Math::Vector<1, std::int32_t>(3)));
    CORRADE_COMPARE((swizzle<'z', 'x', 'w', 'y', 'z', 'y', 'x'>(Vector4i(1, 2, 3, 4))),
        (Math::Vector<7, std::int32_t>(3, 1, 4, 2, 3, 2, 1)));
}

void SwizzleTest::constExpressions() {
    constexpr auto a = swizzle<'z', 'x', 'w', 'y'>(Vector4i(2, 4, 5, 7));
    CORRADE_COMPARE(a, Vector4i(5, 2, 7, 4));

    constexpr auto b = swizzle<'1', 'w', '0', 'y'>(Vector4i(2, 4, 5, 7));
    CORRADE_COMPARE(b, Vector4i(1, 7, 0, 4));
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::SwizzleTest)
