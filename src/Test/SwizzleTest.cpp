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

#include "Swizzle.h"

namespace Magnum { namespace Test {

class SwizzleTest: public Corrade::TestSuite::Tester {
    public:
        SwizzleTest();

        void rgba();
        void type();
        void defaultType();
};

SwizzleTest::SwizzleTest() {
    addTests(&SwizzleTest::rgba,
             &SwizzleTest::type,
             &SwizzleTest::defaultType);
}

void SwizzleTest::rgba() {
    CORRADE_COMPARE((swizzle<'b', 'r', 'a', 'g'>(Vector4i(2, 4, 5, 7))), Vector4i(5, 2, 7, 4));
}

void SwizzleTest::type() {
    Vector4i orig;
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'a'>(orig)), Vector2i>::value));
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'z', 'a'>(orig)), Vector3i>::value));
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'a', 'y', 'x'>(orig)), Vector4i>::value));

    Color3<float> origColor3;
    Color4<double> origColor4;
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'z', 'r'>(origColor3)), Color3<>>::value));
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'z', 'a'>(origColor4)), Color3<double>>::value));
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'z', 'y', 'x'>(origColor3)), Color4<>>::value));
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'a', 'y', 'x'>(origColor4)), Color4<double>>::value));
}

void SwizzleTest::defaultType() {
    Vector4i orig(1, 2, 3, 4);
    CORRADE_COMPARE(swizzle<'b'>(orig), (Math::Vector<1, std::int32_t>(3)));
    CORRADE_COMPARE((swizzle<'b', 'r', 'a', 'g', 'z', 'y', 'x'>(orig)), (Math::Vector<7, std::int32_t>(3, 1, 4, 2, 3, 2, 1)));
}

}}

CORRADE_TEST_MAIN(Magnum::Test::SwizzleTest)
