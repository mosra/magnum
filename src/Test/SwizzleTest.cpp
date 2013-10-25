/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <TestSuite/Tester.h>

#include "Swizzle.h"

namespace Magnum { namespace Test {

class SwizzleTest: public TestSuite::Tester {
    public:
        SwizzleTest();

        void type();
        void defaultType();
};

SwizzleTest::SwizzleTest() {
    addTests({&SwizzleTest::type,
              &SwizzleTest::defaultType});
}

void SwizzleTest::type() {
    constexpr Vector4i orig;
    constexpr Color3 origColor3;
    constexpr BasicColor4<UnsignedByte> origColor4;

    constexpr auto a = swizzle<'y', 'a'>(orig);
    CORRADE_VERIFY((std::is_same<decltype(a), const Vector2i>::value));

    constexpr auto b = swizzle<'y', 'z', 'a'>(orig);
    CORRADE_VERIFY((std::is_same<decltype(b), const Vector3i>::value));

    constexpr auto c = swizzle<'y', 'a', 'y', 'x'>(orig);
    CORRADE_VERIFY((std::is_same<decltype(c), const Vector4i>::value));

    constexpr auto d = swizzle<'y', 'z', 'r'>(origColor3);
    CORRADE_VERIFY((std::is_same<decltype(d), const Color3>::value));

    constexpr auto e = swizzle<'y', 'z', 'a'>(origColor4);
    CORRADE_VERIFY((std::is_same<decltype(e), const BasicColor3<UnsignedByte>>::value));

    constexpr auto f = swizzle<'y', 'z', 'y', 'x'>(origColor3);
    CORRADE_VERIFY((std::is_same<decltype(f), const Color4>::value));

    constexpr auto g = swizzle<'y', 'a', 'y', 'x'>(origColor4);
    CORRADE_VERIFY((std::is_same<decltype(g), const BasicColor4<UnsignedByte>>::value));
}

void SwizzleTest::defaultType() {
    constexpr Vector4i orig(1, 2, 3, 4);

    constexpr auto a = swizzle<'b'>(orig);
    CORRADE_COMPARE(a, (Math::Vector<1, Int>(3)));

    constexpr auto b = swizzle<'b', 'r', 'a', 'g', 'z', 'y', 'x'>(orig);
    CORRADE_COMPARE(b, (Math::Vector<7, Int>(3, 1, 4, 2, 3, 2, 1)));
}

}}

CORRADE_TEST_MAIN(Magnum::Test::SwizzleTest)
