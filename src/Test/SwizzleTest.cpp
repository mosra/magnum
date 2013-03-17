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

class SwizzleTest: public Corrade::TestSuite::Tester {
    public:
        SwizzleTest();

        void rgba();
        void type();
        void defaultType();
};

SwizzleTest::SwizzleTest() {
    addTests({&SwizzleTest::rgba,
              &SwizzleTest::type,
              &SwizzleTest::defaultType});
}

void SwizzleTest::rgba() {
    CORRADE_COMPARE((swizzle<'b', 'r', 'a', 'g'>(Vector4i(2, 4, 5, 7))), Vector4i(5, 2, 7, 4));
}

void SwizzleTest::type() {
    Vector4i orig;
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'a'>(orig)), Vector2i>::value));
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'z', 'a'>(orig)), Vector3i>::value));
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'a', 'y', 'x'>(orig)), Vector4i>::value));

    Color3<Float> origColor3;
    Color4<Double> origColor4;
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'z', 'r'>(origColor3)), Color3<Float>>::value));
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'z', 'a'>(origColor4)), Color3<Double>>::value));
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'z', 'y', 'x'>(origColor3)), Color4<Float>>::value));
    CORRADE_VERIFY((std::is_same<decltype(swizzle<'y', 'a', 'y', 'x'>(origColor4)), Color4<Double>>::value));
}

void SwizzleTest::defaultType() {
    Vector4i orig(1, 2, 3, 4);
    CORRADE_COMPARE(swizzle<'b'>(orig), (Math::Vector<1, Int>(3)));
    CORRADE_COMPARE((swizzle<'b', 'r', 'a', 'g', 'z', 'y', 'x'>(orig)), (Math::Vector<7, Int>(3, 1, 4, 2, 3, 2, 1)));
}

}}

CORRADE_TEST_MAIN(Magnum::Test::SwizzleTest)
