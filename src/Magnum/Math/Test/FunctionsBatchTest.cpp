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

#include <vector>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct FunctionsBatchTest: Corrade::TestSuite::Tester {
    explicit FunctionsBatchTest();

    void isInf();
    void isNan();

    void min();
    void max();
    void minmax();

    void nanIgnoring();
    void nanIgnoringVector();
};

using namespace Literals;

typedef Math::Constants<Float> Constants;
typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Int> Vector3i;
typedef Math::Vector3<Float> Vector3;

FunctionsBatchTest::FunctionsBatchTest() {
    addTests({&FunctionsBatchTest::isInf,
              &FunctionsBatchTest::isNan,

              &FunctionsBatchTest::min,
              &FunctionsBatchTest::max,
              &FunctionsBatchTest::minmax,

              &FunctionsBatchTest::nanIgnoring,
              &FunctionsBatchTest::nanIgnoringVector});
}

void FunctionsBatchTest::isInf() {
    CORRADE_VERIFY(!Math::isInf({5.0f, -2.0f, 9.0f}));
    CORRADE_VERIFY(Math::isInf({5.0f, Constants::inf(), 9.0f}));

    CORRADE_COMPARE(Math::isInf({Vector2{5.0f, -3.0f},
                                 Vector2{-2.0f, 14.0f},
                                 Vector2{9.0f, -5.0f}}), BoolVector<2>{0});
    CORRADE_COMPARE(Math::isInf({Vector2{5.0f, -3.0f},
                                 Vector2{-2.0f, 14.0f},
                                 Vector2{Constants::inf(), -5.0f}}), BoolVector<2>{1});

    CORRADE_VERIFY(!Math::isInf(std::initializer_list<Float>{}));
    CORRADE_COMPARE(Math::isInf(std::initializer_list<Vector3>{}), BoolVector<3>{0});

    const Float a[]{5.0f, -2.0f, -Constants::inf()};
    CORRADE_VERIFY(Math::isInf(a));

    const Float b[]{5.0f, -2.0f, -1.0};
    CORRADE_VERIFY(!Math::isInf(b));

    Vector2 c[]{{5.0f, -3.0f}, {-2.0f, 14.0f}, {Constants::inf(), -5.0f}};
    CORRADE_COMPARE(Math::isInf(c), BoolVector<2>{1});
    /* Mutable view */
    CORRADE_COMPARE(Math::isInf(Corrade::Containers::StridedArrayView1D<Vector2>{c}), BoolVector<2>{1});

    /* This should work without explicit casts or types */
    CORRADE_VERIFY(!Math::isInf(std::vector<Float>{5.0f, -2.0f, -1.0f}));
    CORRADE_COMPARE(Math::isInf(std::vector<Vector2>{
        {5.0f, -3.0f}, {-2.0f, 14.0f}, {Constants::inf(), -5.0f}
    }), BoolVector<2>{1});
}

void FunctionsBatchTest::isNan() {
    CORRADE_VERIFY(!Math::isNan({5.0f, -2.0f, 9.0f}));
    CORRADE_VERIFY(Math::isNan({5.0f, 9.0f, Constants::nan()}));

    CORRADE_COMPARE(Math::isNan({Vector2{5.0f, -3.0f},
                                 Vector2{-2.0f, 14.0f},
                                 Vector2{9.0f, -5.0f}}), BoolVector<2>{0});
    CORRADE_COMPARE(Math::isNan({Vector2{5.0f, -3.0f},
                                 Vector2{14.0f, Constants::nan()},
                                 Vector2{-2.0f, -5.0f}}), BoolVector<2>{2});

    CORRADE_VERIFY(!Math::isNan(std::initializer_list<Double>{}));
    CORRADE_COMPARE(Math::isNan(std::initializer_list<Vector3>{}), BoolVector<3>{0});

    const Float a[]{5.0f, -Constants::nan(), -2.0f};
    CORRADE_VERIFY(Math::isNan(a));

    const Float b[]{5.0f, -2.0f, -1.0};
    CORRADE_VERIFY(!Math::isNan(b));

    Vector2 c[]{{5.0f, -3.0f}, {14.0f, Constants::nan()}, {-2.0f, -5.0f}};
    CORRADE_COMPARE(Math::isNan(c), BoolVector<2>{2});
    /* Mutable view */
    CORRADE_COMPARE(Math::isNan(Corrade::Containers::StridedArrayView1D<Vector2>{c}), BoolVector<2>{2});

    /* This should work without explicit casts or types */
    CORRADE_VERIFY(!Math::isNan(std::vector<Float>{5.0f, -2.0f, -1.0f}));
    CORRADE_COMPARE(Math::isNan(std::vector<Vector2>{
        {5.0f, -3.0f}, {14.0f, Constants::nan()}, {-2.0f, -5.0f}
    }), BoolVector<2>{2});
}

void FunctionsBatchTest::min() {
    CORRADE_COMPARE(Math::min({5, -2, 9}), -2);
    CORRADE_COMPARE(Math::min({Vector3i(5, -3, 2),
                               Vector3i(-2, 14, 7),
                               Vector3i(9, -5, 18)}), Vector3i(-2, -5, 2));

    CORRADE_COMPARE(Math::min(std::initializer_list<Vector3i>{}), Vector3i{});

    const Int array[]{5, -2, 9};
    CORRADE_COMPARE(Math::min(array), -2);
    /* Mutable view */
    Int marray[]{5, -2, 9};
    CORRADE_COMPARE(Math::min(Corrade::Containers::StridedArrayView1D<Int>{marray}), -2);

    /* This should work without explicit casts or types */
    CORRADE_COMPARE(Math::min(std::vector<Int>{5, -2, 9}), -2);

    /* Wrapped types */
    CORRADE_COMPARE(Math::min({5.0_degf, 2.0_degf, 9.0_degf}), 2.0_degf);
}

void FunctionsBatchTest::max() {
    CORRADE_COMPARE(Math::max({5, -2, 9}), 9);
    CORRADE_COMPARE(Math::max({Vector3i(5, -3, 2),
                               Vector3i(-2, 14, 7),
                               Vector3i(9, -5, 18)}), Vector3i(9, 14, 18));

    CORRADE_COMPARE(Math::max(std::initializer_list<Vector3i>{}), Vector3i{});

    const Int array[]{5, -2, 9};
    CORRADE_COMPARE(Math::max(array), 9);
    /* Mutable view */
    Int marray[]{5, -2, 9};
    CORRADE_COMPARE(Math::max(Corrade::Containers::StridedArrayView1D<Int>{marray}), 9);

    /* This should work without explicit casts or types */
    CORRADE_COMPARE(Math::max(std::vector<Int>{5, -2, 9}), 9);

    /* Wrapped types */
    CORRADE_COMPARE(Math::max({5.0_degf, 2.0_degf, 9.0_degf}), 9.0_degf);
}

void FunctionsBatchTest::minmax() {
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
    /* Mutable view */
    Float marray[]{-1.0f, 2.0f, -3.0f};
    CORRADE_COMPARE(Math::minmax(Corrade::Containers::StridedArrayView1D<Float>{marray}), expected);

    /* This should work without explicit casts or types */
    CORRADE_COMPARE(Math::minmax(std::vector<Float>{-1.0f, 2.0f, -3.0f}), expected);

    /* Wrapped types */
    CORRADE_COMPARE(Math::minmax({1.0_radf, 2.0_radf, 3.0_radf}), std::make_pair(1.0_radf, 3.0_radf));
}

void FunctionsBatchTest::nanIgnoring() {
    auto oneNan = {1.0f, Constants::nan(), -3.0f};
    auto firstNan = {Constants::nan(), 1.0f, -3.0f};
    auto allNan = {Constants::nan(), Constants::nan(), Constants::nan()};

    CORRADE_COMPARE(Math::min(oneNan), -3.0f);
    CORRADE_COMPARE(Math::min(firstNan), -3.0f);
    CORRADE_COMPARE(Math::min(allNan), Constants::nan());

    CORRADE_COMPARE(Math::max(oneNan), 1.0f);
    CORRADE_COMPARE(Math::max(firstNan), 1.0f);
    CORRADE_COMPARE(Math::max(allNan), Constants::nan());

    CORRADE_COMPARE(Math::minmax(oneNan), std::make_pair(-3.0f, 1.0f));
    CORRADE_COMPARE(Math::minmax(firstNan), std::make_pair(-3.0f, 1.0f));
    /* Need to compare this way because of NaNs */
    CORRADE_COMPARE(Math::minmax(allNan).first, Constants::nan());
    CORRADE_COMPARE(Math::minmax(allNan).second, Constants::nan());
}

void FunctionsBatchTest::nanIgnoringVector() {
    auto oneNan = {Vector2{1.0f, 0.5f},
                   Vector2{Constants::nan(), -3.0f},
                   Vector2{0.4f, -1.0f}};
    auto firstNan = {Vector2{1.0f, -Constants::nan()},
                     Vector2{2.2f, -1.0f},
                     Vector2{0.4f, -3.0f}};
    auto nanEveryComponent = {Vector2{0.4f, -Constants::nan()},
                              Vector2{Constants::nan(), -1.0f},
                              Vector2{2.2f, -3.0f}};
    auto oneComponentNan = {Vector2{Constants::nan(), 1.5f},
                            Vector2{Constants::nan(), Constants::nan()},
                            Vector2{Constants::nan(), 0.3f}};
    auto firstFullNan = {Vector2{Constants::nan(), Constants::nan()},
                         Vector2{1.5f, Constants::nan()},
                         Vector2{0.3f, Constants::nan()}};
    auto allNan = {Vector2{Constants::nan(), Constants::nan()},
                   Vector2{Constants::nan(), Constants::nan()},
                   Vector2{Constants::nan(), Constants::nan()}};

    CORRADE_COMPARE(Math::min(oneNan), (Vector2{0.4f, -3.0f}));
    CORRADE_COMPARE(Math::min(firstNan), (Vector2{0.4f, -3.0f}));
    CORRADE_COMPARE(Math::min(nanEveryComponent), (Vector2{0.4f, -3.0f}));
    /* Need to compare this way because of NaNs */
    CORRADE_COMPARE(Math::min(oneComponentNan)[0], Constants::nan());
    CORRADE_COMPARE(Math::min(oneComponentNan)[1], 0.3f);
    CORRADE_COMPARE(Math::min(firstFullNan)[0], 0.3f);
    CORRADE_COMPARE(Math::min(firstFullNan)[1], Constants::nan());
    CORRADE_COMPARE(Math::min(allNan)[0], Constants::nan());
    CORRADE_COMPARE(Math::min(allNan)[1], Constants::nan());

    CORRADE_COMPARE(Math::max(oneNan), (Vector2{1.0f, 0.5f}));
    CORRADE_COMPARE(Math::max(firstNan), (Vector2{2.2f, -1.0f}));
    CORRADE_COMPARE(Math::max(nanEveryComponent), (Vector2{2.2f, -1.0f}));
    /* Need to compare this way because of NaNs */
    CORRADE_COMPARE(Math::max(oneComponentNan)[0], Constants::nan());
    CORRADE_COMPARE(Math::max(oneComponentNan)[1], 1.5f);
    CORRADE_COMPARE(Math::max(firstFullNan)[0], 1.5f);
    CORRADE_COMPARE(Math::max(firstFullNan)[1], Constants::nan());
    CORRADE_COMPARE(Math::max(allNan)[0], Constants::nan());
    CORRADE_COMPARE(Math::max(allNan)[1], Constants::nan());

    CORRADE_COMPARE(Math::minmax(oneNan), std::make_pair(
        Vector2{0.4f, -3.0f}, Vector2{1.0f, 0.5f}));
    CORRADE_COMPARE(Math::minmax(firstNan), std::make_pair(
        Vector2{0.4f, -3.0f}, Vector2{2.2f, -1.0f}));
    CORRADE_COMPARE(Math::minmax(nanEveryComponent), std::make_pair(
        Vector2{0.4f, -3.0f}, Vector2{2.2f, -1.0f}));
    /* Need to compare this way because of NaNs */
    CORRADE_COMPARE(Math::minmax(oneComponentNan).first[0], Constants::nan());
    CORRADE_COMPARE(Math::minmax(oneComponentNan).first[1], 0.3f);
    CORRADE_COMPARE(Math::minmax(oneComponentNan).second[0], Constants::nan());
    CORRADE_COMPARE(Math::minmax(oneComponentNan).second[1], 1.5f);
    CORRADE_COMPARE(Math::minmax(firstFullNan).first[0], 0.3f);
    CORRADE_COMPARE(Math::minmax(firstFullNan).first[1], Constants::nan());
    CORRADE_COMPARE(Math::minmax(firstFullNan).second[0], 1.5f);
    CORRADE_COMPARE(Math::minmax(firstFullNan).second[1], Constants::nan());
    CORRADE_COMPARE(Math::minmax(allNan).first[0], Constants::nan());
    CORRADE_COMPARE(Math::minmax(allNan).first[1], Constants::nan());
    CORRADE_COMPARE(Math::minmax(allNan).second[0], Constants::nan());
    CORRADE_COMPARE(Math::minmax(allNan).second[1], Constants::nan());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FunctionsBatchTest)
