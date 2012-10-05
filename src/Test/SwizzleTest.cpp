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

CORRADE_TEST_MAIN(Magnum::Test::SwizzleTest)

namespace Magnum { namespace Test {

typedef Math::Vector2<int32_t> Vector2;
typedef Math::Vector3<int32_t> Vector3;
typedef Math::Vector4<int32_t> Vector4;

SwizzleTest::SwizzleTest() {
    addTests(&SwizzleTest::xyzw,
             &SwizzleTest::rgba,
             &SwizzleTest::constants,
             &SwizzleTest::fromSmall,
             &SwizzleTest::type,
             &SwizzleTest::defaultType);
}

void SwizzleTest::xyzw() {
    Vector4 orig(2, 4, 5, 7);
    Vector4 swizzled(5, 2, 7, 4);
    CORRADE_COMPARE(swizzle(orig, "zxwy"), swizzled);
    CORRADE_COMPARE((swizzle<'z', 'x', 'w', 'y'>(orig)), swizzled);
}

void SwizzleTest::rgba() {
    Vector4 orig(2, 4, 5, 7);
    Vector4 swizzled(5, 2, 7, 4);
    CORRADE_COMPARE(swizzle(orig, "brag"), swizzled);
    CORRADE_COMPARE((swizzle<'b', 'r', 'a', 'g'>(orig)), swizzled);
}

void SwizzleTest::constants() {
    Vector4 orig(2, 4, 5, 7);
    Vector4 swizzled(1, 7, 0, 4);
    CORRADE_COMPARE(swizzle(orig, "1w0g"), swizzled);
    CORRADE_COMPARE((swizzle<'1', 'w', '0', 'g'>(orig)), swizzled);
}

void SwizzleTest::fromSmall() {
    /* Force compile-time evaluation for both */
    constexpr Vector2 orig(1, 2);
    constexpr Vector3 swizzled(swizzle(orig, "gxr"));
    CORRADE_VERIFY((integral_constant<bool, swizzled.x() == 2>::value));
    CORRADE_COMPARE((swizzle<'g', 'x', 'r'>(orig)), Vector3(2, 1, 1));
}

void SwizzleTest::type() {
    Vector4 orig;
    CORRADE_VERIFY((is_same<decltype(swizzle<'y', 'a'>(orig)), Vector2>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle(orig, "ya")), Vector2>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle<'y', 'z', 'a'>(orig)), Vector3>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle(orig, "yza")), Vector3>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle<'y', 'a', 'y', 'x'>(orig)), Vector4>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle(orig, "yayx")), Vector4>::value));

    Color3<float> origColor3;
    Color4<double> origColor4;
    CORRADE_VERIFY((is_same<decltype(swizzle<'y', 'z', 'r'>(origColor3)), Color3<float>>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle<'y', 'z', 'a'>(origColor4)), Color3<double>>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle(origColor3, "yzr")), Color3<float>>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle(origColor4, "yza")), Color3<double>>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle<'y', 'z', 'y', 'x'>(origColor3)), Color4<float>>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle<'y', 'a', 'y', 'x'>(origColor4)), Color4<double>>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle(origColor3, "yzyx")), Color4<float>>::value));
    CORRADE_VERIFY((is_same<decltype(swizzle(origColor4, "yayx")), Color4<double>>::value));
}

void SwizzleTest::defaultType() {
    Vector4 orig(1, 2, 3, 4);
    CORRADE_COMPARE(swizzle<'b'>(orig), (Math::Vector<1, int32_t>(3)));
    CORRADE_COMPARE(swizzle(orig, "b"), (Math::Vector<1, int32_t>(3)));
    CORRADE_COMPARE((swizzle<'b', 'r', 'a', 'g', 'z', 'y', 'x'>(orig)), (Math::Vector<7, int32_t>(3, 1, 4, 2, 3, 2, 1)));
    CORRADE_COMPARE(swizzle(orig, "bragzyx"), (Math::Vector<7, int32_t>(3, 1, 4, 2, 3, 2, 1)));
}

}}
