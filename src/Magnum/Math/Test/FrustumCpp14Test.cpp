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

#include "Magnum/Math/Frustum.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct FrustumCpp14Test: TestSuite::Tester {
    explicit FrustumCpp14Test();

    void accessConstexpr();
};

/* What's a typedef and not a using differs from the typedefs in root Magnum
   namespace, or is not present there at all */
using Magnum::Vector4;
using Magnum::Frustum;

FrustumCpp14Test::FrustumCpp14Test() {
    addTests({&FrustumCpp14Test::accessConstexpr});
}

constexpr Frustum populate() {
    Frustum a;
    a.left() = {-2.0f, 2.0f, -3.0f, 0.1f}; /* gets 1 added to X */
    a.right() = { 1.0f, -4.0f, 3.0f, 0.2f}; /* gets Y divided by 2 */
    a.bottom() = {-4.0f, 5.0f, -6.0f, 0.3f};
    a.top() = { 4.0f, -5.0f, 3.0f, 0.4f}; /* gets Z multiplied by 2 */
    a.near() = {-7.0f, 8.0f, -9.0f, 0.5f};
    a.far() = { 7.0f, 8.0f, 9.0f, 0.6f};
    a.begin()->x() += 1.0f;
    a[1].y() /= 2.0f;
    (a.end() - 3)->z() *= 2.0f;
    return a;
}

void FrustumCpp14Test::accessConstexpr() {
    constexpr Frustum a = populate();
    CORRADE_COMPARE(a, (Frustum{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f,  8.0f,  9.0f, 0.6f}}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FrustumCpp14Test)
