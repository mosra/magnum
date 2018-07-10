/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/Animation/Track.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Animation { namespace Test {

struct TrackViewTest: TestSuite::Tester {
    explicit TrackViewTest();

    void construct();
    void constructDefaults();
    void constructSingleArray();
    void constructSingleArrayDefaults();

    void at();
    void atStrict();
    void atDifferentResultType();
    void atDifferentResultTypeStrict();
};

namespace {

/* Reduced version from InterpolateTest, keep in sync with TrackTest */
const struct {
    const char* name;
    Extrapolation extrapolationBefore;
    Extrapolation extrapolationAfter;
    Float time;
    Float expectedValue, expectedValueStrict;
    std::size_t expectedHint;
} AtData[] {
    {"before default-constructed",
        Extrapolation::DefaultConstructed, Extrapolation::Extrapolated,
        -1.0f, 0.0f, 4.0f, 0},
    {"before constant",
        Extrapolation::Constant, Extrapolation::Extrapolated,
        -1.0f, 3.0f, 4.0f, 0},
    {"before extrapolated",
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed,
        -1.0f, 4.0f, 4.0f, 0},
    {"during first",
        Extrapolation::DefaultConstructed, Extrapolation::DefaultConstructed,
        1.5f, 1.5f, 1.5f, 0},
    {"during second",
        Extrapolation::DefaultConstructed, Extrapolation::DefaultConstructed,
        4.75f, 1.0f, 1.0f, 2},
    {"after default-constructed",
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed,
        6.0f, 0.0f, -1.5f, 2},
    {"after constant",
        Extrapolation::Extrapolated, Extrapolation::Constant,
        6.0f, 0.5f, -1.5f, 2},
    {"after extrapolated",
        Extrapolation::DefaultConstructed, Extrapolation::Extrapolated,
        6.0f, -1.5f, -1.5f, 2}
};

}

TrackViewTest::TrackViewTest() {
    addTests({&TrackViewTest::construct,
              &TrackViewTest::constructDefaults,
              &TrackViewTest::constructSingleArray,
              &TrackViewTest::constructSingleArrayDefaults});

    addInstancedTests({&TrackViewTest::at,
                       &TrackViewTest::atStrict}, Containers::arraySize(AtData));

    addTests({&TrackViewTest::atDifferentResultType,
              &TrackViewTest::atDifferentResultTypeStrict});
}

using namespace Math::Literals;

void TrackViewTest::construct() {
    constexpr Float keys[]{0.0f, 5.0f};
    constexpr Vector3 values[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<Float, Vector3> a{keys, values, Math::lerp,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructDefaults() {
    constexpr Float keys[]{0.0f, 5.0f};
    constexpr Vector3 values[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<Float, Vector3> a{keys, values, Math::lerp,
        Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructSingleArray() {
    const std::pair<Float, Vector3> data[]{
        {0.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<Float, Vector3> a{data, Math::lerp,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructSingleArrayDefaults() {
    const std::pair<Float, Vector3> data[]{
        {0.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<Float, Vector3> a{data, Math::lerp,
        Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

namespace {
    const std::pair<Float, Float> Keyframes[]{
        {0.0f, 3.0f},
        {2.0f, 1.0f},
        {4.0f, 2.5f},
        {5.0f, 0.5f}};
}

void TrackViewTest::at() {
    const auto& data = AtData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const TrackView<Float, Float> a{Keyframes, Math::lerp,
        data.extrapolationBefore, data.extrapolationAfter};

    std::size_t hint{};
    CORRADE_COMPARE(a.at(data.time, hint), data.expectedValue);
    CORRADE_COMPARE(a.at(data.time), data.expectedValue);
    CORRADE_COMPARE(hint, data.expectedHint);
}

void TrackViewTest::atStrict() {
    const auto& data = AtData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const TrackView<Float, Float> a{Keyframes, Math::lerp,
        data.extrapolationBefore, data.extrapolationAfter};

    std::size_t hint{};
    CORRADE_COMPARE(a.atStrict(data.time, hint), data.expectedValueStrict);
    CORRADE_COMPARE(hint, data.expectedHint);
}

namespace {
    using namespace Math::Literals;

    const Half HalfValues[]{3.0_h, 1.0_h, 2.5_h, 0.5_h};

    Float lerpHalf(const Half& a, const Half& b, Float t) {
        return Math::lerp(Float(a), Float(b), t);
    }
}

void TrackViewTest::atDifferentResultType() {
    const TrackView<Float, Half, Float> a{
        {&Keyframes[0].first, Containers::arraySize(Keyframes), sizeof(Keyframes[0])},
        HalfValues, lerpHalf};

    std::size_t hint{};
    CORRADE_COMPARE(a.at(4.75f, hint), 1.0f);
    CORRADE_COMPARE(a.at(4.75f), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

void TrackViewTest::atDifferentResultTypeStrict() {
    const TrackView<Float, Half, Float> a{
        {&Keyframes[0].first, Containers::arraySize(Keyframes), sizeof(Keyframes[0])},
        HalfValues, lerpHalf};

    std::size_t hint{};
    CORRADE_COMPARE(a.atStrict(4.75f, hint), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::TrackViewTest)
