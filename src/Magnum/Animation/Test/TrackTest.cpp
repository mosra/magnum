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

struct TrackTest: TestSuite::Tester {
    explicit TrackTest();

    void constructEmpty();
    void constructArrayInterpolator();
    void constructArrayInterpolatorDefaults();
    void constructArrayInterpolation();
    void constructArrayInterpolationDefaults();
    void constructArrayInterpolationInterpolator();
    void constructArrayInterpolationInterpolationDefaults();
    void constructInitializerListInterpolation();
    void constructInitializerListInterpolationDefaults();
    void constructInitializerListInterpolator();
    void constructInitializerListInterpolatorDefaults();
    void constructInitializerListInterpolationInterpolator();
    void constructInitializerListInterpolationInterpolatorDefaults();

    void convertView();

    void at();
    void atStrict();
    void atDifferentResultType();
    void atDifferentResultTypeStrict();
};

/* Reduced version from InterpolateTest, keep in sync with TrackViewTest */
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

TrackTest::TrackTest() {
    addTests({&TrackTest::constructEmpty,
              &TrackTest::constructArrayInterpolator,
              &TrackTest::constructArrayInterpolatorDefaults,
              &TrackTest::constructArrayInterpolation,
              &TrackTest::constructArrayInterpolationDefaults,
              &TrackTest::constructArrayInterpolationInterpolator,
              &TrackTest::constructArrayInterpolationInterpolationDefaults,
              &TrackTest::constructInitializerListInterpolator,
              &TrackTest::constructInitializerListInterpolatorDefaults,
              &TrackTest::constructInitializerListInterpolation,
              &TrackTest::constructInitializerListInterpolationDefaults,
              &TrackTest::constructInitializerListInterpolationInterpolator,
              &TrackTest::constructInitializerListInterpolationInterpolatorDefaults,

              &TrackTest::convertView});

    addInstancedTests({&TrackTest::at,
                       &TrackTest::atStrict}, Containers::arraySize(AtData));

    addTests({&TrackTest::atDifferentResultType,
              &TrackTest::atDifferentResultTypeStrict});
}

using namespace Math::Literals;

void TrackTest::constructEmpty() {
    Track<Float, Vector3> a;
    const Track<Float, Vector3>& ca = a;

    CORRADE_VERIFY(!ca.interpolator());
    CORRADE_VERIFY(!ca.size());
    CORRADE_VERIFY(ca.keys().empty());
    CORRADE_VERIFY(ca.keys().empty());
    CORRADE_VERIFY(a.values().empty());
    CORRADE_VERIFY(ca.values().empty());
    CORRADE_COMPARE(ca.at(42.0f), Vector3{});
}

void TrackTest::constructArrayInterpolator() {
    Track<Float, Vector3> a{
        Containers::Array<std::pair<Float, Vector3>>{Containers::InPlaceInit,
            {{1.0f, {3.0f, 1.0f, 0.1f}},
             {5.0f, {0.3f, 0.6f, 1.0f}}}},
        Math::select, Extrapolation::Extrapolated, Extrapolation::Constant};
    const Track<Float, Vector3>& ca = a;

    CORRADE_COMPARE(ca.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(ca.interpolator(), Math::select);
    CORRADE_COMPARE(ca.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(ca.after(), Extrapolation::Constant);
    CORRADE_COMPARE(ca.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(ca.size(), 2);
    CORRADE_COMPARE(a.data().size(), 2);
    CORRADE_COMPARE(ca.data().size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(ca.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(ca.values().size(), 2);
    CORRADE_COMPARE(ca[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(ca.data()[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.data()[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(ca.keys()[1], 5.0f);
    CORRADE_COMPARE(a.keys()[1], 5.0f);
    CORRADE_COMPARE(ca.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructArrayInterpolatorDefaults() {
    const Track<Float, Vector3> a{
        Containers::Array<std::pair<Float, Vector3>>{Containers::InPlaceInit,
            {{1.0f, {3.0f, 1.0f, 0.1f}}}},
        Math::lerp, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 1.0f}));
    CORRADE_COMPARE(a.size(), 1);
    CORRADE_COMPARE(a.data().size(), 1);
    CORRADE_COMPARE(a.keys().size(), 1);
    CORRADE_COMPARE(a.values().size(), 1);
    CORRADE_COMPARE(a[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.data()[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.keys()[0], 1.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructArrayInterpolation() {
    const Track<Float, Vector3> a{
        Containers::Array<std::pair<Float, Vector3>>{Containers::InPlaceInit,
            {{1.0f, {3.0f, 1.0f, 0.1f}},
             {5.0f, {0.3f, 0.6f, 1.0f}}}},
        Interpolation::Linear, Extrapolation::Extrapolated, Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.data().size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.data()[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.keys()[1], 5.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructArrayInterpolationDefaults() {
    const Track<Float, Vector3> a{
        Containers::Array<std::pair<Float, Vector3>>{Containers::InPlaceInit,
            {{1.0f, {3.0f, 1.0f, 0.1f}}}},
        Interpolation::Constant, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(a.interpolator(), Math::select);
    CORRADE_COMPARE(a.before(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 1.0f}));
    CORRADE_COMPARE(a.size(), 1);
    CORRADE_COMPARE(a.data().size(), 1);
    CORRADE_COMPARE(a.keys().size(), 1);
    CORRADE_COMPARE(a.values().size(), 1);
    CORRADE_COMPARE(a[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.data()[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.keys()[0], 1.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

Vector3 customLerp(const Vector3&, const Vector3&, Float) { return {}; }

void TrackTest::constructArrayInterpolationInterpolator() {
    const Track<Float, Vector3> a{
        Containers::Array<std::pair<Float, Vector3>>{Containers::InPlaceInit,
            {{1.0f, {3.0f, 1.0f, 0.1f}},
             {5.0f, {0.3f, 0.6f, 1.0f}}}},
        Interpolation::Linear, customLerp, Extrapolation::Extrapolated, Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(a.interpolator(), customLerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.data().size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.data()[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.keys()[1], 5.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructArrayInterpolationInterpolationDefaults() {
    const Track<Float, Vector3> a{
        Containers::Array<std::pair<Float, Vector3>>{Containers::InPlaceInit,
            {{1.0f, {3.0f, 1.0f, 0.1f}}}},
        Interpolation::Constant, customLerp, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(a.interpolator(), customLerp);
    CORRADE_COMPARE(a.before(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 1.0f}));
    CORRADE_COMPARE(a.size(), 1);
    CORRADE_COMPARE(a.data().size(), 1);
    CORRADE_COMPARE(a.keys().size(), 1);
    CORRADE_COMPARE(a.values().size(), 1);
    CORRADE_COMPARE(a[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.data()[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.keys()[0], 1.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructInitializerListInterpolator() {
    const Track<Float, Vector3> a{
            {{1.0f, {3.0f, 1.0f, 0.1f}},
             {5.0f, {0.3f, 0.6f, 1.0f}}},
        Math::select, Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(a.interpolator(), Math::select);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.data().size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.data()[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.keys()[1], 5.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructInitializerListInterpolatorDefaults() {
    const Track<Float, Vector3> a{{{1.0f, {3.0f, 1.0f, 0.1f}}},
        Math::lerp, Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Custom);
    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 1.0f}));
    CORRADE_COMPARE(a.size(), 1);
    CORRADE_COMPARE(a.data().size(), 1);
    CORRADE_COMPARE(a.keys().size(), 1);
    CORRADE_COMPARE(a.values().size(), 1);
    CORRADE_COMPARE(a[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.data()[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.keys()[0], 1.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructInitializerListInterpolation() {
    const Track<Float, Vector3> a{
            {{1.0f, {3.0f, 1.0f, 0.1f}},
             {5.0f, {0.3f, 0.6f, 1.0f}}},
        Interpolation::Linear, Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.data().size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.data()[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.keys()[1], 5.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructInitializerListInterpolationDefaults() {
    const Track<Float, Vector3> a{{{1.0f, {3.0f, 1.0f, 0.1f}}},
        Interpolation::Constant, Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(a.interpolator(), Math::select);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 1.0f}));
    CORRADE_COMPARE(a.size(), 1);
    CORRADE_COMPARE(a.data().size(), 1);
    CORRADE_COMPARE(a.keys().size(), 1);
    CORRADE_COMPARE(a.values().size(), 1);
    CORRADE_COMPARE(a[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.data()[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.keys()[0], 1.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructInitializerListInterpolationInterpolator() {
    const Track<Float, Vector3> a{
            {{1.0f, {3.0f, 1.0f, 0.1f}},
             {5.0f, {0.3f, 0.6f, 1.0f}}},
        Interpolation::Linear, customLerp, Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(a.interpolator(), customLerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.data().size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.data()[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.keys()[1], 5.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructInitializerListInterpolationInterpolatorDefaults() {
    const Track<Float, Vector3> a{{{1.0f, {3.0f, 1.0f, 0.1f}}},
        Interpolation::Constant, customLerp, Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolation(), Interpolation::Constant);
    CORRADE_COMPARE(a.interpolator(), customLerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.duration(), (Range1D{1.0f, 1.0f}));
    CORRADE_COMPARE(a.size(), 1);
    CORRADE_COMPARE(a.data().size(), 1);
    CORRADE_COMPARE(a.keys().size(), 1);
    CORRADE_COMPARE(a.values().size(), 1);
    CORRADE_COMPARE(a[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.data()[0], (std::pair<Float, Vector3>{1.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.keys()[0], 1.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::convertView() {
    Track<Float, Vector3> a{
        {{1.0f, {3.0f, 1.0f, 0.1f}},
         {5.0f, {0.3f, 0.6f, 1.0f}}},
        Interpolation::Linear, customLerp, Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};
    const Track<Float, Vector3>& ca = a;

    const TrackView<Float, Vector3> av = a;
    CORRADE_COMPARE(av.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(av.interpolator(), customLerp);
    CORRADE_COMPARE(av.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(av.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(av.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(av.size(), 2);
    CORRADE_COMPARE(av.keys().size(), 2);
    CORRADE_COMPARE(av.values().size(), 2);
    CORRADE_COMPARE(av[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(av.keys()[1], 5.0f);
    CORRADE_COMPARE(av.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));

    const TrackView<const Float, const Vector3> cav = ca;
    CORRADE_COMPARE(cav.interpolation(), Interpolation::Linear);
    CORRADE_COMPARE(cav.interpolator(), customLerp);
    CORRADE_COMPARE(cav.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(cav.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(cav.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(cav.size(), 2);
    CORRADE_COMPARE(cav.keys().size(), 2);
    CORRADE_COMPARE(cav.values().size(), 2);
    CORRADE_COMPARE(cav[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(cav.keys()[1], 5.0f);
    CORRADE_COMPARE(cav.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::at() {
    const auto& data = AtData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const Track<Float, Float> a{
        {{0.0f, 3.0f},
         {2.0f, 1.0f},
         {4.0f, 2.5f},
         {5.0f, 0.5f}}, Math::lerp,
        data.extrapolationBefore, data.extrapolationAfter};

    std::size_t hint{};
    CORRADE_COMPARE(a.at(data.time, hint), data.expectedValue);
    CORRADE_COMPARE(a.at(data.time), data.expectedValue);
    CORRADE_COMPARE(hint, data.expectedHint);
}

void TrackTest::atStrict() {
    const auto& data = AtData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const Track<Float, Float> a{
        {{0.0f, 3.0f},
         {2.0f, 1.0f},
         {4.0f, 2.5f},
         {5.0f, 0.5f}}, Math::lerp,
        data.extrapolationBefore, data.extrapolationAfter};

    std::size_t hint{};
    CORRADE_COMPARE(a.atStrict(data.time, hint), data.expectedValueStrict);
    CORRADE_COMPARE(hint, data.expectedHint);
}

Float lerpHalf(const Half& a, const Half& b, Float t) {
    return Math::lerp(Float(a), Float(b), t);
}

void TrackTest::atDifferentResultType() {
    using namespace Math::Literals;

    const Track<Float, Half, Float> a{
        {{0.0f, 3.0_h},
         {2.0f, 1.0_h},
         {4.0f, 2.5_h},
         {5.0f, 0.5_h}}, lerpHalf};

    std::size_t hint{};
    CORRADE_COMPARE(a.at(4.75f, hint), 1.0f);
    CORRADE_COMPARE(a.at(4.75f), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

void TrackTest::atDifferentResultTypeStrict() {
    using namespace Math::Literals;

    const Track<Float, Half, Float> a{
        {{0.0f, 3.0_h},
         {2.0f, 1.0_h},
         {4.0f, 2.5_h},
         {5.0f, 0.5_h}}, lerpHalf};

    std::size_t hint{};
    CORRADE_COMPARE(a.atStrict(4.75f, hint), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::TrackTest)
