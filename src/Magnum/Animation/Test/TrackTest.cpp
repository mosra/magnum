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
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Animation { namespace Test {

struct TrackTest: TestSuite::Tester {
    explicit TrackTest();

    void constructArray();
    void constructArrayDefaults();
    void constructInitializerList();
    void constructInitializerListDefaults();

    void convertView();

    void at();
    void atStrict();
};

namespace {

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

}

TrackTest::TrackTest() {
    addTests({&TrackTest::constructArray,
              &TrackTest::constructArrayDefaults,
              &TrackTest::constructInitializerList,
              &TrackTest::constructInitializerListDefaults,

              &TrackTest::convertView});

    addInstancedTests({&TrackTest::at,
                       &TrackTest::atStrict}, Containers::arraySize(AtData));
}

using namespace Math::Literals;

void TrackTest::constructArray() {
    const Track<Float, Vector3> a{
        Containers::Array<std::pair<Float, Vector3>>{Containers::InPlaceInit,
            {{0.0f, {3.0f, 1.0f, 0.1f}},
             {5.0f, {0.3f, 0.6f, 1.0f}}}},
        Math::select, Extrapolation::Extrapolated, Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolator(), Math::select);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.data().size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.data()[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.keys()[1], 5.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructArrayDefaults() {
    const Track<Float, Vector3> a{
        Containers::Array<std::pair<Float, Vector3>>{Containers::InPlaceInit,
            {{0.0f, {3.0f, 1.0f, 0.1f}}}},
        Math::lerp, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.data().size(), 1);
    CORRADE_COMPARE(a.keys().size(), 1);
    CORRADE_COMPARE(a.values().size(), 1);
    CORRADE_COMPARE(a[0], (std::pair<Float, Vector3>{0.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.data()[0], (std::pair<Float, Vector3>{0.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.keys()[0], 0.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructInitializerList() {
    const Track<Float, Vector3> a{
            {{0.0f, {3.0f, 1.0f, 0.1f}},
             {5.0f, {0.3f, 0.6f, 1.0f}}},
        Math::select, Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};

    CORRADE_COMPARE(a.interpolator(), Math::select);
    CORRADE_COMPARE(a.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(a.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(a.data().size(), 2);
    CORRADE_COMPARE(a.keys().size(), 2);
    CORRADE_COMPARE(a.values().size(), 2);
    CORRADE_COMPARE(a[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.data()[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(a.keys()[1], 5.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::constructInitializerListDefaults() {
    const Track<Float, Vector3> a{{{0.0f, {3.0f, 1.0f, 0.1f}}},
        Math::lerp, Extrapolation::Constant};

    CORRADE_COMPARE(a.interpolator(), Math::lerp);
    CORRADE_COMPARE(a.before(), Extrapolation::Constant);
    CORRADE_COMPARE(a.after(), Extrapolation::Constant);
    CORRADE_COMPARE(a.data().size(), 1);
    CORRADE_COMPARE(a.keys().size(), 1);
    CORRADE_COMPARE(a.values().size(), 1);
    CORRADE_COMPARE(a[0], (std::pair<Float, Vector3>{0.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.data()[0], (std::pair<Float, Vector3>{0.0f, {3.0f, 1.0f, 0.1f}}));
    CORRADE_COMPARE(a.keys()[0], 0.0f);
    CORRADE_COMPARE(a.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
}

void TrackTest::convertView() {
    const Track<Float, Vector3> a{
        {{0.0f, {3.0f, 1.0f, 0.1f}},
         {5.0f, {0.3f, 0.6f, 1.0f}}},
        Math::select, Extrapolation::Extrapolated, Extrapolation::DefaultConstructed};
    const TrackView<Float, Vector3> av = a;

    CORRADE_COMPARE(av.interpolator(), Math::select);
    CORRADE_COMPARE(av.before(), Extrapolation::Extrapolated);
    CORRADE_COMPARE(av.after(), Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(av.keys().size(), 2);
    CORRADE_COMPARE(av.values().size(), 2);
    CORRADE_COMPARE(av[1], (std::pair<Float, Vector3>{5.0f, {0.3f, 0.6f, 1.0f}}));
    CORRADE_COMPARE(av.keys()[1], 5.0f);
    CORRADE_COMPARE(av.values()[0], (Vector3{3.0f, 1.0f, 0.1f}));
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

}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::TrackTest)
