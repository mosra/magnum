/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

namespace Magnum { namespace Math { namespace Test { namespace {

struct SwizzleTest: Corrade::TestSuite::Tester {
    explicit SwizzleTest();

    void gather();
    void gatherConstants();
    void gatherDifferentSize();
    void gatherFarComponents();

    void scatter();
    void scatterOneComponent();
    void scatterRepeatedComponents();
    void scatterOverwriteAllComponents();
    void scatterFarComponents();
};

typedef Vector<2, Int> Vector2i;
typedef Vector<3, Int> Vector3i;
typedef Vector<4, Int> Vector4i;

SwizzleTest::SwizzleTest() {
    addTests({&SwizzleTest::gather,
              &SwizzleTest::gatherConstants,
              &SwizzleTest::gatherDifferentSize,
              &SwizzleTest::gatherFarComponents,

              &SwizzleTest::scatter,
              &SwizzleTest::scatterOneComponent,
              &SwizzleTest::scatterRepeatedComponents,
              &SwizzleTest::scatterOverwriteAllComponents,
              &SwizzleTest::scatterFarComponents});
}

void SwizzleTest::gather() {
    constexpr auto a = Math::gather<'z', 'x', 'w', 'y'>(Vector4i{2, 4, 5, 7});
    constexpr auto b = Math::gather<'b', 'r', 'a', 'g'>(Vector4i{2, 4, 5, 7});
    constexpr auto c = Math::gather<2, 0, 3, 1>(Vector4i{2, 4, 5, 7});
    CORRADE_COMPARE(a, (Vector4i{5, 2, 7, 4}));
    CORRADE_COMPARE(b, (Vector4i{5, 2, 7, 4}));
    CORRADE_COMPARE(c, (Vector4i{5, 2, 7, 4}));
}

void SwizzleTest::gatherConstants() {
    constexpr auto a = Math::gather<'1', 'w', '0', 'y'>(Vector4i{2, 4, 5, 7});
    constexpr auto b = Math::gather<'1', 3, '0', 1>(Vector4i{2, 4, 5, 7});
    CORRADE_COMPARE(a, (Vector4i{1, 7, 0, 4}));
    CORRADE_COMPARE(b, (Vector4i{1, 7, 0, 4}));
}

void SwizzleTest::gatherDifferentSize() {
    constexpr auto a = Math::gather<'y', 'x', 'x'>(Vector<2, Int>{1, 2});
    CORRADE_COMPARE(a, (Vector3i{2, 1, 1}));

    constexpr auto b = Math::gather<'z'>(Vector4i{1, 2, 3, 4});
    CORRADE_COMPARE(b, (Vector<1, Int>{3}));

    constexpr auto c = Math::gather<'z', 'x', 'w', 'y', 'z', 'y', 'x'>(Vector4i{1, 2, 3, 4});
    CORRADE_COMPARE(c, (Vector<7, Int>{3, 1, 4, 2, 3, 2, 1}));
}

void SwizzleTest::gatherFarComponents() {
    constexpr auto a = Math::gather<5, 4, 6>(Vector<7, Int>{2, 4, 5, 7, 0, 3, 2});
    CORRADE_COMPARE(a, (Vector3i{3, 0, 2}));
}

void SwizzleTest::scatter() {
    constexpr auto a = Math::scatter<'w', 'y'>(Vector4i{2, 4, 5, 7}, Vector2i{1, 3});
    constexpr auto b = Math::scatter<'a', 'g'>(Vector4i{2, 4, 5, 7}, Vector2i{1, 3});
    constexpr auto c = Math::scatter<3, 1>(Vector4i{2, 4, 5, 7}, Vector2i{1, 3});
    CORRADE_COMPARE(a, (Vector4i{2, 3, 5, 1}));
    CORRADE_COMPARE(b, (Vector4i{2, 3, 5, 1}));
    CORRADE_COMPARE(c, (Vector4i{2, 3, 5, 1}));

    /* It's an inverse, so doing the same gather should result back in the
       original values */
    CORRADE_COMPARE((Math::gather<'w', 'y'>(a)), (Vector2i{1, 3}));
}

void SwizzleTest::scatterOneComponent() {
    constexpr auto a = Math::scatter<'w'>(Vector<7, Int>{2, 4, 5, 7, 0, 3, 2}, 1);
    constexpr auto b = Math::scatter<'a'>(Vector<7, Int>{2, 4, 5, 7, 0, 3, 2}, 1);
    constexpr auto c = Math::scatter<3>(Vector<7, Int>{2, 4, 5, 7, 0, 3, 2}, 1);
    CORRADE_COMPARE(a, (Vector<7, Int>{2, 4, 5, 1, 0, 3, 2}));
    CORRADE_COMPARE(b, (Vector<7, Int>{2, 4, 5, 1, 0, 3, 2}));
    CORRADE_COMPARE(c, (Vector<7, Int>{2, 4, 5, 1, 0, 3, 2}));
}

void SwizzleTest::scatterRepeatedComponents() {
    constexpr auto a = Math::scatter<'x', 'y', 'z', 'y', 'x'>(Vector3i{6, 12, 19}, Vector<5, Int>{1, 2, 3, 4, 5});
    constexpr auto b = Math::scatter<'r', 'g', 'b', 'g', 'r'>(Vector3i{6, 12, 19}, Vector<5, Int>{1, 2, 3, 4, 5});
    constexpr auto c = Math::scatter<0, 1, 2, 1, 0>(Vector3i{6, 12, 19}, Vector<5, Int>{1, 2, 3, 4, 5});
    CORRADE_COMPARE(a, (Vector3i{5, 4, 3}));
    CORRADE_COMPARE(b, (Vector3i{5, 4, 3}));
    CORRADE_COMPARE(c, (Vector3i{5, 4, 3}));
}

void SwizzleTest::scatterOverwriteAllComponents() {
    constexpr auto a = Math::scatter<'w', 'y', 'z', 'x'>(Vector4i{2, 4, 5, 7}, Vector4i{1, 3, 6, 9});
    constexpr auto b = Math::gather<'w', 'y', 'z', 'x'>(Vector4i{1, 3, 6, 9});
    CORRADE_COMPARE(a, (Vector4i{9, 3, 6, 1}));
    CORRADE_COMPARE(b, (Vector4i{9, 3, 6, 1}));
}

void SwizzleTest::scatterFarComponents() {
    constexpr auto a = Math::scatter<5, 4, 6>(Vector<7, Int>{2, 4, 5, 7, 0, 3, 2}, Vector3i{1, 6, 9});
    CORRADE_COMPARE(a, (Vector<7, Int>{2, 4, 5, 7, 6, 1, 9}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::SwizzleTest)
