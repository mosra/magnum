/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct FunctionsBatchTest: Corrade::TestSuite::Tester {
    explicit FunctionsBatchTest();

    void minList();
    void maxList();
    void minmaxList();
};

typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Int> Vector3i;

FunctionsBatchTest::FunctionsBatchTest() {
    addTests({&FunctionsBatchTest::minList,
              &FunctionsBatchTest::maxList,
              &FunctionsBatchTest::minmaxList});
}

void FunctionsBatchTest::minList() {
    CORRADE_COMPARE(Math::min({5, -2, 9}), -2);
    CORRADE_COMPARE(Math::min({Vector3i(5, -3, 2),
                               Vector3i(-2, 14, 7),
                               Vector3i(9, -5, 18)}), Vector3i(-2, -5, 2));

    CORRADE_COMPARE(Math::min(std::initializer_list<Vector3i>{}), Vector3i{});

    const Int array[]{5, -2, 9};
    CORRADE_COMPARE(Math::min(array), -2);
}

void FunctionsBatchTest::maxList() {
    CORRADE_COMPARE(Math::max({5, -2, 9}), 9);
    CORRADE_COMPARE(Math::max({Vector3i(5, -3, 2),
                               Vector3i(-2, 14, 7),
                               Vector3i(9, -5, 18)}), Vector3i(9, 14, 18));

    CORRADE_COMPARE(Math::max(std::initializer_list<Vector3i>{}), Vector3i{});

    const Int array[]{5, -2, 9};
    CORRADE_COMPARE(Math::max(array), 9);
}

void FunctionsBatchTest::minmaxList() {
    const auto expected = std::make_pair(-3.0f, 2.0f);
    CORRADE_COMPARE(Math::minmax({-1.0f, 2.0f, -3.0f}), expected);
    CORRADE_COMPARE(Math::minmax({-1.0f, -3.0f, 2.0f}), expected);
    CORRADE_COMPARE(Math::minmax({2.0f, -1.0f, -3.0f}), expected);
    CORRADE_COMPARE(Math::minmax({2.0f, -3.0f, -1.0f}), expected);
    CORRADE_COMPARE(Math::minmax({-3.0f, 2.0f, -1.0f}), expected);
    CORRADE_COMPARE(Math::minmax({-3.0f, -1.0f, 2.0f}), expected);

    const std::pair<Vector2, Vector2> expectedVec{Vector2{-3.0f, -2.0f}, Vector2{2.0f, 3.0f}};
    CORRADE_COMPARE(Math::minmax({Vector2{-1.0f, 3.0f}, Vector2{2.0f, 1.0f}, Vector2{-3.0f, -2.0f}}), expectedVec);
    CORRADE_COMPARE(Math::minmax({Vector2{-1.0f, 1.0f}, Vector2{-3.0f, 3.0f}, Vector2{2.0f, -2.0f}}), expectedVec);
    CORRADE_COMPARE(Math::minmax({Vector2{2.0f, -2.0f}, Vector2{-1.0f, 1.0f}, Vector2{-3.0f, 3.0f}}), expectedVec);
    CORRADE_COMPARE(Math::minmax({Vector2{2.0f, 1.0f}, Vector2{-3.0f, -2.0f}, Vector2{-1.0f, 3.0f}}), expectedVec);
    CORRADE_COMPARE(Math::minmax({Vector2{-3.0f, 3.0f}, Vector2{2.0f, -2.0f}, Vector2{-1.0f, 1.0f}}), expectedVec);
    CORRADE_COMPARE(Math::minmax({Vector2{-3.0f, -2.0f}, Vector2{-1.0f, 3.0f}, Vector2{2.0f, 1.0f}}), expectedVec);

    const Float array[]{-1.0f, 2.0f, -3.0f};
    CORRADE_COMPARE(Math::minmax(array), expected);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FunctionsBatchTest)
