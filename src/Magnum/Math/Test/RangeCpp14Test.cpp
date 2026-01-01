/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include "Magnum/Math/Range.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct RangeCpp14Test: TestSuite::Tester {
    explicit RangeCpp14Test();

    void accessConstexpr();
    /* Range1D doesn't have any dedicated getters on top of the generic base */
    void accessConstexpr2D();
    void accessConstexpr3D();
};

/* What's a typedef and not a using differs from the typedefs in root Magnum
   namespace, or is not present there at all */
using Magnum::Range1D;
using Magnum::Range2D;
using Magnum::Range3D;

RangeCpp14Test::RangeCpp14Test() {
    addTests({&RangeCpp14Test::accessConstexpr,
              &RangeCpp14Test::accessConstexpr2D,
              &RangeCpp14Test::accessConstexpr3D});
}

constexpr Range2D populate() {
    Range2D a;
    a.min() = {1.0f, 0.0f};
    a.max() = {2.0f, 3.0f};
    return a;
}

void RangeCpp14Test::accessConstexpr() {
    constexpr Range2D a = populate();
    CORRADE_COMPARE(a, (Range2D{{1.0f, 0.0f}, {2.0f, 3.0f}}));
}

constexpr Range2D populate2D() {
    Range2D a;
    a.bottomLeft() = {0.0f, -2.0f};
    a.topRight() = {4.0f, 1.5f};
    a.bottom() += 2.0f;
    a.left() += 1.0f;
    a.top() *= 2.0f;
    a.right() /= 2.0f;
    return a;
}

void RangeCpp14Test::accessConstexpr2D() {
    constexpr Range2D a = populate2D();
    CORRADE_COMPARE(a, (Range2D{{1.0f, 0.0f}, {2.0f, 3.0f}}));
}

constexpr Range3D populate3D() {
    Range3D a;
    a.backBottomLeft() = {0.0f, -2.0f, 5.0f};
    a.frontTopRight() = {4.0f, 1.5f, 4.5f};
    a.bottom() += 2.0f;
    a.left() += 1.0f;
    a.top() *= 2.0f;
    a.right() /= 2.0f;
    a.back() -= 1.0f;
    a.front() += 0.5f;
    return a;
}

void RangeCpp14Test::accessConstexpr3D() {
    constexpr Range3D a = populate3D();
    CORRADE_COMPARE(a, (Range3D{{1.0f, 0.0f, 4.0f}, {2.0f, 3.0f, 5.0f}}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::RangeCpp14Test)
