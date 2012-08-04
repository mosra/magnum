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

#include "SwizzleTest.h"

#include "Swizzle.h"

using namespace std;

CORRADE_TEST_MAIN(Magnum::Math::Test::SwizzleTest)

namespace Magnum { namespace Math { namespace Test {

typedef Math::Vector2<int> Vector2;
typedef Math::Vector3<int> Vector3;
typedef Math::Vector4<int> Vector4;

SwizzleTest::SwizzleTest() {
    addTests(&SwizzleTest::xyzw,
             &SwizzleTest::rgba,
             &SwizzleTest::type,
             &SwizzleTest::defaultType);
}

void SwizzleTest::xyzw() {
    Vector4 orig(2, 4, 5, 7);
    CORRADE_COMPARE((swizzle<'z', 'x', 'w', 'y'>(orig)), Vector4(5, 2, 7, 4));
}

void SwizzleTest::rgba() {
    Vector4 orig(2, 4, 5, 7);
    CORRADE_COMPARE((swizzle<'b', 'r', 'a', 'g'>(orig)), Vector4(5, 2, 7, 4));
}

void SwizzleTest::type() {
    Vector4 orig;
    CORRADE_VERIFY((is_same<decltype(swizzle<'y', 'a'>(orig)), Vector2>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle<'y', 'z', 'a'>(orig)), Vector3>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle<'y', 'a', 'y', 'x'>(orig)), Vector4>::value));
}

void SwizzleTest::defaultType() {
    Vector4 orig(1, 2, 3, 4);
    CORRADE_COMPARE(swizzle<'b'>(orig), (Vector<1, int>(3)));
    CORRADE_COMPARE((swizzle<'b', 'r', 'a', 'g', 'z', 'y', 'x'>(orig)), (Vector<7, int>(3, 1, 4, 2, 3, 2, 1)));
}

}}}
