/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Swizzle.h"

namespace Magnum { namespace Math { namespace Test {

struct SwizzleTest: Corrade::TestSuite::Tester {
    explicit SwizzleTest();

    void components();
    void constants();
    void rgba();
    void sizes();
};

typedef Vector<4, Int> Vector4i;

SwizzleTest::SwizzleTest() {
    addTests({&SwizzleTest::components,
              &SwizzleTest::constants,
              &SwizzleTest::rgba,
              &SwizzleTest::sizes});
}

void SwizzleTest::components() {
    constexpr auto a = swizzle<'z', 'x', 'w', 'y'>(Vector4i(2, 4, 5, 7));
    CORRADE_COMPARE(a, Vector4i(5, 2, 7, 4));
}

void SwizzleTest::constants() {
    constexpr auto a = swizzle<'1', 'w', '0', 'y'>(Vector4i(2, 4, 5, 7));
    CORRADE_COMPARE(a, Vector4i(1, 7, 0, 4));
}

void SwizzleTest::rgba() {
    constexpr auto a = swizzle<'b', 'r', 'a', 'g'>(Vector4i(2, 4, 5, 7));
    CORRADE_COMPARE(a, Vector4i(5, 2, 7, 4));
}

void SwizzleTest::sizes() {
    constexpr auto a = swizzle<'y', 'x', 'x'>(Vector<2, Int>(1, 2));
    CORRADE_COMPARE(a, (Vector<3, Int>(2, 1, 1)));

    constexpr auto b = swizzle<'z'>(Vector4i(1, 2, 3, 4));
    CORRADE_COMPARE(b, (Vector<1, Int>(3)));

    constexpr auto c = swizzle<'z', 'x', 'w', 'y', 'z', 'y', 'x'>(Vector4i(1, 2, 3, 4));
    CORRADE_COMPARE(c, (Vector<7, Int>(3, 1, 4, 2, 3, 2, 1)));
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::SwizzleTest)
