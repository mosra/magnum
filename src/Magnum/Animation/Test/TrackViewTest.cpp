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

#include "Magnum/Animation/Track.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Animation { namespace Test { namespace {

struct TrackViewTest: TestSuite::Tester {
    explicit TrackViewTest();

    void constructEmpty();
    void constructInterpolator();
    void constructInterpolatorDefaults();
    void constructInterpolation();
    void constructInterpolationDefaults();
    void constructInterpolationInterpolator();
    void constructInterpolationInterpolatorDefaults();
    void constructSingleArrayInterpolator();
    void constructSingleArrayInterpolatorDefaults();
    void constructSingleArrayInterpolation();
    void constructSingleArrayInterpolationDefaults();
    void constructSingleArrayInterpolationInterpolator();
    void constructSingleArrayInterpolationInterpolatorDefaults();

    void constructCopyStorage();
    void convertToConstView();

    void at();
    void atStrict();
    void atDifferentResultType();
    void atDifferentResultTypeStrict();
};

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

TrackViewTest::TrackViewTest() {
    addTests({&TrackViewTest::constructEmpty,
              &TrackViewTest::constructInterpolator,
              &TrackViewTest::constructInterpolatorDefaults,
              &TrackViewTest::constructInterpolation,
              &TrackViewTest::constructInterpolationDefaults,
              &TrackViewTest::constructInterpolationInterpolator,
              &TrackViewTest::constructInterpolationInterpolatorDefaults,
              &TrackViewTest::constructSingleArrayInterpolator,
              &TrackViewTest::constructSingleArrayInterpolatorDefaults,
              &TrackViewTest::constructSingleArrayInterpolation,
              &TrackViewTest::constructSingleArrayInterpolationDefaults,
              &TrackViewTest::constructSingleArrayInterpolationInterpolator,
              &TrackViewTest::constructSingleArrayInterpolationInterpolatorDefaults,

              &TrackViewTest::constructCopyStorage,
              &TrackViewTest::convertToConstView});

    addInstancedTests({&TrackViewTest::at,
                       &TrackViewTest::atStrict}, Containers::arraySize(AtData));

    addTests({&TrackViewTest::atDifferentResultType,
              &TrackViewTest::atDifferentResultTypeStrict});
}

using namespace Math::Literals;

void TrackViewTest::constructEmpty() {
    const TrackView<Float, Vector3> a;
    const TrackView<const Float, const Vector3> ca;

    CORRADE_VERIFY(!a.interpolator());
    CORRADE_COMPARE(a.duration(), Range1D{});
    CORRADE_VERIFY(!a.size());
    CORRADE_VERIFY(a.keys().empty());
    CORRADE_VERIFY(a.values().empty());
    CORRADE_COMPARE(a.at(42.0f), Vector3{});

    CORRADE_VERIFY(!ca.interpolator());
    CORRADE_COMPARE(ca.duration(), Range1D{});
    CORRADE_VERIFY(!ca.size());
    CORRADE_VERIFY(ca.keys().empty());
    CORRADE_VERIFY(ca.values().empty());
    CORRADE_COMPARE(ca.at(42.0f), Vector3{});
}

void TrackViewTest::constructInterpolator() {
    Float keys[]{1.0f, 5.0f};
    Vector3 values[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<Float, Vector3> a{keys, values, Math::select,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(a.interpolator(), Math::select);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    constexpr Float ckeys[]{1.0f, 5.0f};
    constexpr Vector3 cvalues[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<const Float, const Vector3> ca{ckeys, cvalues, Math::select,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(ca.interpolator(), Math::select);
    CORRADE_COMPARE(ca.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(ca.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructInterpolatorDefaults() {
    Float keys[]{1.0f, 5.0f};
    Vector3 values[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<Float, Vector3> a{keys, values, Math::lerp,
        Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    constexpr Float ckeys[]{1.0f, 5.0f};
    constexpr Vector3 cvalues[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<const Float, const Vector3> ca{ckeys, cvalues, Math::lerp,
        Extrapolation::Constant};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(ca.interpolator(), Math::lerp);
    CORRADE_COMPARE(ca.before(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.after(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructInterpolation() {
    Float keys[]{1.0f, 5.0f};
    Vector3 values[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<Float, Vector3> a{keys, values, Interpolation::Linear,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    constexpr Float ckeys[]{1.0f, 5.0f};
    constexpr Vector3 cvalues[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<const Float, const Vector3> ca{ckeys, cvalues, Interpolation::Linear,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(ca.interpolator(), Math::lerp);
    CORRADE_COMPARE(ca.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(ca.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructInterpolationDefaults() {
    Float keys[]{1.0f, 5.0f};
    Vector3 values[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<Float, Vector3> a{keys, values, Interpolation::Constant,
        Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(a.interpolator(), Math::select);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    constexpr Float ckeys[]{1.0f, 5.0f};
    constexpr Vector3 cvalues[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<const Float, const Vector3> ca{ckeys, cvalues, Interpolation::Constant,
        Extrapolation::Constant};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(ca.interpolator(), Math::select);
    CORRADE_COMPARE(ca.before(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.after(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

Vector3 customLerp(const Vector3&, const Vector3&, Float) { return {}; }

void TrackViewTest::constructInterpolationInterpolator() {
    Float keys[]{1.0f, 5.0f};
    Vector3 values[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<Float, Vector3> a{keys, values, Interpolation::Linear,
        customLerp, Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(a.interpolator(), customLerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    constexpr Float ckeys[]{1.0f, 5.0f};
    constexpr Vector3 cvalues[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<const Float, const Vector3> ca{ckeys, cvalues,
        Interpolation::Linear, customLerp, Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(ca.interpolator(), customLerp);
    CORRADE_COMPARE(ca.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(ca.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructInterpolationInterpolatorDefaults() {
    Float keys[]{1.0f, 5.0f};
    Vector3 values[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<Float, Vector3> a{keys, values, Interpolation::Constant,
        customLerp, Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(a.interpolator(), customLerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    constexpr Float ckeys[]{1.0f, 5.0f};
    constexpr Vector3 cvalues[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<const Float, const Vector3> ca{ckeys, cvalues,
        Interpolation::Constant, customLerp, Extrapolation::Constant};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(ca.interpolator(), customLerp);
    CORRADE_COMPARE(ca.before(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.after(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructSingleArrayInterpolator() {
    std::pair<Float, Vector3> data[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<Float, Vector3> a{data, Math::select,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(a.interpolator(), Math::select);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    const std::pair<Float, Vector3> cdata[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<const Float, const Vector3> ca{cdata, Math::select,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(ca.interpolator(), Math::select);
    CORRADE_COMPARE(ca.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(ca.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructSingleArrayInterpolatorDefaults() {
    std::pair<Float, Vector3> data[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<Float, Vector3> a{data, Math::lerp,
        Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    const std::pair<Float, Vector3> cdata[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<const Float, const Vector3> ca{cdata, Math::lerp,
        Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructSingleArrayInterpolation() {
    std::pair<Float, Vector3> data[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<Float, Vector3> a{data, Interpolation::Linear,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    const std::pair<Float, Vector3> cdata[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<const Float, const Vector3> ca{cdata, Interpolation::Linear,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(ca.interpolator(), Math::lerp);
    CORRADE_COMPARE(ca.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(ca.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructSingleArrayInterpolationDefaults() {
    std::pair<Float, Vector3> data[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<Float, Vector3> a{data, Interpolation::Constant,
        Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(a.interpolator(), Math::select);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    const std::pair<Float, Vector3> cdata[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<const Float, const Vector3> ca{cdata, Interpolation::Constant,
        Extrapolation::Constant};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(ca.interpolator(), Math::select);
    CORRADE_COMPARE(ca.before(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.after(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructSingleArrayInterpolationInterpolator() {
    std::pair<Float, Vector3> data[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<Float, Vector3> a{data, Interpolation::Linear, customLerp,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(a.interpolator(), customLerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    const std::pair<Float, Vector3> cdata[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<const Float, const Vector3> ca{cdata, Interpolation::Linear, customLerp,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(ca.interpolator(), customLerp);
    CORRADE_COMPARE(ca.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(ca.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructSingleArrayInterpolationInterpolatorDefaults() {
    std::pair<Float, Vector3> data[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<Float, Vector3> a{data, Interpolation::Constant,
        customLerp, Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(a.interpolator(), customLerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    const std::pair<Float, Vector3> cdata[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<const Float, const Vector3> ca{cdata, Interpolation::Constant,
        customLerp, Extrapolation::Constant};

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(ca.interpolator(), customLerp);
    CORRADE_COMPARE(ca.before(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.after(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::constructCopyStorage() {
    std::pair<Float, Vector3> data[]{
        {1.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}};

    const TrackView<Float, Vector3> a{data, Interpolation::Constant, customLerp,
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    const TrackViewStorage<Float> b = a;
    CORRADE_COMPARE(b.keys().size(), 2);
    CORRADE_COMPARE(b.values().size(), 2);
    CORRADE_COMPARE(Containers::arrayCast<Float>(b.keys())[1], 5.0f);
    CORRADE_COMPARE(Containers::arrayCast<Vector3>(b.values())[1], (Vector3{0.3f, 0.6f, 1.0f}));

    auto& bv = *static_cast<const TrackView<Float, Vector3>*>(&b);

    CORRADE_COMPARE(bv.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(bv.interpolator(), customLerp);
    CORRADE_COMPARE(bv.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(bv.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(bv.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(bv.size(), 2);
    CORRADE_COMPARE(bv.keys().size(), 2);
    CORRADE_COMPARE(bv.values().size(), 2);
    CORRADE_COMPARE(bv[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
}

void TrackViewTest::convertToConstView() {
    Float keys[]{1.0f, 5.0f};
    Vector3 values[]{{3.0f, 1.0f, 0.1f}, {0.3f, 0.6f, 1.0f}};

    const TrackView<Float, Vector3> a{keys, values, Interpolation::Linear,
        customLerp, Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    const TrackView<const Float, const Vector3> ca = a;

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(ca.interpolator(), customLerp);
    CORRADE_COMPARE(ca.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(ca.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));

    /* Conversion back is not possible */
    CORRADE_VERIFY((std::is_convertible<TrackView<Float, Vector3>,  TrackView<const Float, const Vector3>>::value));
    CORRADE_VERIFY(!(std::is_convertible<TrackView<const Float, const Vector3>,  TrackView<Float, Vector3>>::value));
}

const std::pair<Float, Float> Keyframes[]{
    {0.0f, 3.0f},
    {2.0f, 1.0f},
    {4.0f, 2.5f},
    {5.0f, 0.5f}
};

void TrackViewTest::at() {
    const auto& data = AtData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const TrackView<const Float, const Float> a{Keyframes, Math::lerp,
        data.extrapolationBefore, data.extrapolationAfter};

    std::size_t hint{};
    CORRADE_COMPARE(a.at(data.time, hint), data.expectedValue);
    CORRADE_COMPARE(a.at(data.time), data.expectedValue);
    CORRADE_COMPARE(hint, data.expectedHint);
}

void TrackViewTest::atStrict() {
    const auto& data = AtData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const TrackView<const Float, const Float> a{Keyframes, Math::lerp,
        data.extrapolationBefore, data.extrapolationAfter};

    std::size_t hint{};
    CORRADE_COMPARE(a.atStrict(data.time, hint), data.expectedValueStrict);
    CORRADE_COMPARE(hint, data.expectedHint);
}

using namespace Math::Literals;

const Half HalfValues[]{3.0_h, 1.0_h, 2.5_h, 0.5_h};

Float lerpHalf(const Half& a, const Half& b, Float t) {
    return Math::lerp(Float(a), Float(b), t);
}

void TrackViewTest::atDifferentResultType() {
    const TrackView<const Float, const Half, Float> a{
        {Keyframes, &Keyframes[0].first, Containers::arraySize(Keyframes), sizeof(Keyframes[0])},
        HalfValues, lerpHalf};

    std::size_t hint{};
    CORRADE_COMPARE(a.at(4.75f, hint), 1.0f);
    CORRADE_COMPARE(a.at(4.75f), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

void TrackViewTest::atDifferentResultTypeStrict() {
    const TrackView<const Float, const Half, Float> a{
        {Keyframes, &Keyframes[0].first, Containers::arraySize(Keyframes), sizeof(Keyframes[0])},
        HalfValues, lerpHalf};

    std::size_t hint{};
    CORRADE_COMPARE(a.atStrict(4.75f, hint), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::TrackViewTest)
