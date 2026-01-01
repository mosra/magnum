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

#include "Magnum/Math/CubicHermite.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct CubicHermiteCpp14Test: TestSuite::Tester {
    explicit CubicHermiteCpp14Test();

    void accessConstexpr();
};

/* What's a typedef and not a using differs from the typedefs in root Magnum
   namespace, or is not present there at all */
using Magnum::CubicHermite2D;

CubicHermiteCpp14Test::CubicHermiteCpp14Test() {
    addTests({&CubicHermiteCpp14Test::accessConstexpr});
}

constexpr CubicHermite2D populate() {
    CubicHermite2D a;
    a.inTangent() = {1.0f, 2.0f};
    a.point() = {3.0f, 4.0f};
    a.outTangent() = {6.0f, 5.0f};
    return a;
}

void CubicHermiteCpp14Test::accessConstexpr() {
    constexpr CubicHermite2D a = populate();
    CORRADE_COMPARE(a, (CubicHermite2D{{1.0f, 2.0f}, {3.0f, 4.0f}, {6.0f, 5.0f}}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::CubicHermiteCpp14Test)
