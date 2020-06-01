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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/CubicHermite.h"
#include "Magnum/Math/Quaternion.h"
#include "Magnum/Trade/AnimationData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AnimationDataTest: TestSuite::Tester {
    explicit AnimationDataTest();

    void constructTrackData();
    void constructTrackDataResultType();
    void constructTrackDataTemplate();
    void constructTrackDataDefault();

    void construct();
    void constructNotOwned();
    void constructImplicitDuration();
    void constructImplicitDurationEmpty();
    void constructImplicitDurationNotOwned();
    void constructNotOwnedFlagOwned();
    void constructImplicitDurationNotOwnedFlagOwned();

    void constructCopy();
    void constructMove();

    void mutableAccessNotAllowed();

    void trackCustomResultType();
    void trackWrongIndex();
    void trackWrongType();
    void trackWrongResultType();

    void release();

    void debugAnimationTrackType();
    void debugAnimationTrackTargetType();
};

struct {
    const char* name;
    DataFlags dataFlags;
} NotOwnedData[] {
    {"", {}},
    {"mutable", DataFlag::Mutable},
};

AnimationDataTest::AnimationDataTest() {
    addTests({&AnimationDataTest::constructTrackData,
              &AnimationDataTest::constructTrackDataResultType,
              &AnimationDataTest::constructTrackDataTemplate,
              &AnimationDataTest::constructTrackDataDefault,

              &AnimationDataTest::construct,
              &AnimationDataTest::constructImplicitDuration,
              &AnimationDataTest::constructImplicitDurationEmpty});

    addInstancedTests({&AnimationDataTest::constructNotOwned,
                       &AnimationDataTest::constructImplicitDurationNotOwned},
        Containers::arraySize(NotOwnedData));

    addTests({&AnimationDataTest::constructNotOwnedFlagOwned,
              &AnimationDataTest::constructImplicitDurationNotOwnedFlagOwned,

              &AnimationDataTest::constructCopy,
              &AnimationDataTest::constructMove,

              &AnimationDataTest::mutableAccessNotAllowed,

              &AnimationDataTest::trackCustomResultType,
              &AnimationDataTest::trackWrongIndex,
              &AnimationDataTest::trackWrongType,
              &AnimationDataTest::trackWrongResultType,

              &AnimationDataTest::release,

              &AnimationDataTest::debugAnimationTrackType,
              &AnimationDataTest::debugAnimationTrackTargetType});
}

using namespace Math::Literals;

void AnimationDataTest::constructTrackData() {
    AnimationTrackData trackData{
         AnimationTrackType::Vector3,
         AnimationTrackTargetType::Translation3D, 42,
         Animation::TrackView<const Float, const Vector3>{
            nullptr,
            Animation::Interpolation::Linear,
            animationInterpolatorFor<Vector3>(Animation::Interpolation::Linear)}};
    AnimationData data{nullptr, Containers::Array<AnimationTrackData>{Containers::InPlaceInit, {trackData}}};
    CORRADE_COMPARE(data.trackType(0), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.trackTargetType(0), AnimationTrackTargetType::Translation3D);
    CORRADE_COMPARE(data.trackTarget(0), 42);
    CORRADE_COMPARE(data.track(0).interpolation(), Animation::Interpolation::Linear);
}

void AnimationDataTest::constructTrackDataResultType() {
    AnimationTrackData trackData{
         AnimationTrackType::CubicHermite3D,
         AnimationTrackType::Vector3,
         AnimationTrackTargetType::Translation3D, 42,
         Animation::TrackView<const Float, const CubicHermite3D>{
            nullptr,
            Animation::Interpolation::Linear,
            animationInterpolatorFor<CubicHermite3D>(Animation::Interpolation::Linear)}};
    AnimationData data{nullptr, Containers::Array<AnimationTrackData>{Containers::InPlaceInit, {trackData}}};
    CORRADE_COMPARE(data.trackType(0), AnimationTrackType::CubicHermite3D);
    CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.trackTargetType(0), AnimationTrackTargetType::Translation3D);
    CORRADE_COMPARE(data.trackTarget(0), 42);
    CORRADE_COMPARE(data.track(0).interpolation(), Animation::Interpolation::Linear);
}

void AnimationDataTest::constructTrackDataTemplate() {
    AnimationTrackData trackData{
         AnimationTrackTargetType::Translation3D, 42,
         Animation::TrackView<const Float, const CubicHermite3D>{
            nullptr,
            Animation::Interpolation::Linear,
            animationInterpolatorFor<CubicHermite3D>(Animation::Interpolation::Linear)}};
    AnimationData data{nullptr, Containers::Array<AnimationTrackData>{Containers::InPlaceInit, {trackData}}};
    CORRADE_COMPARE(data.trackType(0), AnimationTrackType::CubicHermite3D);
    CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.trackTargetType(0), AnimationTrackTargetType::Translation3D);
    CORRADE_COMPARE(data.trackTarget(0), 42);
    CORRADE_COMPARE(data.track(0).interpolation(), Animation::Interpolation::Linear);
}

void AnimationDataTest::constructTrackDataDefault() {
    AnimationTrackData data;
    /* no public accessors here, so nothing to check -- and such a track
       shouldn't get added to AnimationData anyway */
    CORRADE_VERIFY(true);
}

void AnimationDataTest::construct() {
    struct Data {
        Float time;
        Vector3 position;
        Quaternion rotation;
    };
    Containers::Array<char> buffer{sizeof(Data)*3};
    auto view = Containers::arrayCast<Data>(buffer);
    view[0] = {0.0f, {3.0f, 1.0f, 0.1f}, Quaternion::rotation(45.0_degf, Vector3::yAxis())};
    view[1] = {5.0f, {0.3f, 0.6f, 1.0f}, Quaternion::rotation(20.0_degf, Vector3::yAxis())};
    view[2] = {7.5f, {1.0f, 0.3f, 2.1f}, Quaternion{}};

    const int state = 5;
    AnimationData data{std::move(buffer), {
        AnimationTrackData{AnimationTrackTargetType::Translation3D, 42,
            Animation::TrackView<const Float, const Vector3>{
                {view, &view[0].time, view.size(), sizeof(Data)},
                {view, &view[0].position, view.size(), sizeof(Data)},
                Animation::Interpolation::Constant,
                animationInterpolatorFor<Vector3>(Animation::Interpolation::Constant)}},
        AnimationTrackData{AnimationTrackTargetType::Rotation3D, 1337,
            Animation::TrackView<const Float, const Quaternion>{
                {view, &view[0].time, view.size(), sizeof(Data)},
                {view, &view[0].rotation, view.size(), sizeof(Data)},
                Animation::Interpolation::Linear,
                animationInterpolatorFor<Quaternion>(Animation::Interpolation::Linear)}}
        }, {-1.0f, 7.0f}, &state};

    CORRADE_COMPARE(data.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.duration(), (Range1D{-1.0f, 7.0f}));
    CORRADE_COMPARE(static_cast<const void*>(data.data().data()), view.data());
    CORRADE_COMPARE(static_cast<void*>(data.mutableData().data()), view.data());
    CORRADE_COMPARE(data.trackCount(), 2);
    CORRADE_COMPARE(data.importerState(), &state);

    {
        CORRADE_COMPARE(data.trackType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(data.trackTargetType(0), AnimationTrackTargetType::Translation3D);
        CORRADE_COMPARE(data.trackTarget(0), 42);

        Animation::TrackView<const Float, const Vector3> track = data.track<Vector3>(0);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));

        Animation::TrackView<Float, Vector3> mutableTrack = data.mutableTrack<Vector3>(0);
        CORRADE_COMPARE(mutableTrack.keys().size(), 3);
        CORRADE_COMPARE(mutableTrack.values().size(), 3);
        CORRADE_COMPARE(mutableTrack.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(mutableTrack.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));
    } {
        CORRADE_COMPARE(data.trackType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(data.trackResultType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(data.trackTargetType(1), AnimationTrackTargetType::Rotation3D);
        CORRADE_COMPARE(data.trackTarget(1), 1337);

        Animation::TrackView<const Float, const Quaternion> track = data.track<Quaternion>(1);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Linear);
        CORRADE_COMPARE(track.at(2.5f), Quaternion::rotation(32.5_degf, Vector3::yAxis()));

        /* Testing the mutable track just once is enough */
    }
}

void AnimationDataTest::constructImplicitDuration() {
    struct Data {
        Float time;
        bool value;
    };
    Containers::Array<char> buffer{sizeof(Data)*4};
    auto view = Containers::arrayCast<Data>(buffer);
    view[0] = {1.0f, true};
    view[1] = {5.0f, false};
    view[2] = {3.0f, true};
    view[3] = {7.0f, false};

    const int state = 5;
    AnimationData data{std::move(buffer), {
        AnimationTrackData{AnimationTrackTargetType(129), 0,
            Animation::TrackView<const Float, const bool>{
                {view, &view[0].time, 2, sizeof(Data)},
                {view, &view[0].value, 2, sizeof(Data)},
                Animation::Interpolation::Constant}},
        AnimationTrackData{AnimationTrackTargetType(130), 1,
            Animation::TrackView<const Float, const bool>{
                {view, &view[2].time, 2, sizeof(Data)},
                {view, &view[2].value, 2, sizeof(Data)},
                Animation::Interpolation::Linear}}
        }, &state};

    CORRADE_COMPARE(data.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.duration(), (Range1D{1.0f, 7.0f}));
    CORRADE_COMPARE(data.trackCount(), 2);
    CORRADE_COMPARE(data.importerState(), &state);
    {
        CORRADE_COMPARE(data.trackType(0), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackTargetType(0), AnimationTrackTargetType(129));
        CORRADE_COMPARE(data.trackTarget(0), 0);

        Animation::TrackView<const Float, const bool> track = data.track<bool>(0);
        CORRADE_COMPARE(track.duration(), (Range1D{1.0f, 5.0f}));
        CORRADE_COMPARE(track.keys().size(), 2);
        CORRADE_COMPARE(track.values().size(), 2);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(6.0f), false);

        Animation::TrackView<Float, bool> mutableTrack = data.mutableTrack<bool>(0);
        CORRADE_COMPARE(mutableTrack.duration(), (Range1D{1.0f, 5.0f}));
        CORRADE_COMPARE(mutableTrack.keys().size(), 2);
        CORRADE_COMPARE(mutableTrack.values().size(), 2);
        CORRADE_COMPARE(mutableTrack.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(mutableTrack.at(6.0f), false);
    } {
        CORRADE_COMPARE(data.trackType(1), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackResultType(1), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackTargetType(1), AnimationTrackTargetType(130));
        CORRADE_COMPARE(data.trackTarget(1), 1);

        Animation::TrackView<const Float, const bool> track = data.track<bool>(1);
        CORRADE_COMPARE(track.duration(), (Range1D{3.0f, 7.0f}));
        CORRADE_COMPARE(track.keys().size(), 2);
        CORRADE_COMPARE(track.values().size(), 2);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Linear);
        CORRADE_COMPARE(track.at(4.5f), true);

        /* Testing the mutable track just once is enough */
    }
}

void AnimationDataTest::constructImplicitDurationEmpty() {
    AnimationData data{nullptr, nullptr};
    CORRADE_COMPARE(data.duration(), Range1D{});
}

void AnimationDataTest::constructNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    std::pair<Float, Vector3> keyframes[] {
        {0.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}
    };

    const int state = 5;
    AnimationData data{instanceData.dataFlags, keyframes, {
        AnimationTrackData{AnimationTrackTargetType::Translation3D, 42,
            Animation::TrackView<const Float, const Vector3>{
                keyframes,
                Animation::Interpolation::Constant,
                animationInterpolatorFor<Vector3>(Animation::Interpolation::Constant)}}
        }, {-1.0f, 7.0f}, &state};

    CORRADE_COMPARE(data.dataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.duration(), (Range1D{-1.0f, 7.0f}));
    CORRADE_COMPARE(static_cast<const void*>(data.data().data()), keyframes);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableData().data()), keyframes);
    CORRADE_COMPARE(data.trackCount(), 1);
    CORRADE_COMPARE(data.importerState(), &state);

    {
        CORRADE_COMPARE(data.trackType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(data.trackTargetType(0), AnimationTrackTargetType::Translation3D);
        CORRADE_COMPARE(data.trackTarget(0), 42);

        Animation::TrackView<const Float, const Vector3> track = data.track<Vector3>(0);
        CORRADE_COMPARE(track.keys().size(), 2);
        CORRADE_COMPARE(track.values().size(), 2);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));

        if(instanceData.dataFlags & DataFlag::Mutable) {
            Animation::TrackView<Float, Vector3> mutableTrack = data.mutableTrack<Vector3>(0);
            CORRADE_COMPARE(mutableTrack.keys().size(), 2);
            CORRADE_COMPARE(mutableTrack.values().size(), 2);
            CORRADE_COMPARE(mutableTrack.interpolation(), Animation::Interpolation::Constant);
            CORRADE_COMPARE(mutableTrack.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));
        }
    }
}

void AnimationDataTest::constructImplicitDurationNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    std::pair<Float, bool> keyframes[] {
        {1.0f, true},
        {5.0f, false}
    };

    const int state = 5;
    AnimationData data{instanceData.dataFlags, keyframes, {
        AnimationTrackData{AnimationTrackTargetType(129), 0,
            Animation::TrackView<const Float, const bool>{keyframes, Animation::Interpolation::Constant}},
        }, &state};

    CORRADE_COMPARE(data.dataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(static_cast<const void*>(data.data().data()), keyframes);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableData().data()), keyframes);
    CORRADE_COMPARE(data.trackCount(), 1);
    CORRADE_COMPARE(data.importerState(), &state);

    {
        CORRADE_COMPARE(data.trackType(0), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackTargetType(0), AnimationTrackTargetType(129));
        CORRADE_COMPARE(data.trackTarget(0), 0);

        Animation::TrackView<const Float, const bool> track = data.track<bool>(0);
        CORRADE_COMPARE(track.duration(), (Range1D{1.0f, 5.0f}));
        CORRADE_COMPARE(track.keys().size(), 2);
        CORRADE_COMPARE(track.values().size(), 2);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(3.0f), true);

        if(instanceData.dataFlags & DataFlag::Mutable) {
            Animation::TrackView<Float, bool> mutableTrack = data.mutableTrack<bool>(0);
            CORRADE_COMPARE(mutableTrack.duration(), (Range1D{1.0f, 5.0f}));
            CORRADE_COMPARE(mutableTrack.keys().size(), 2);
            CORRADE_COMPARE(mutableTrack.values().size(), 2);
            CORRADE_COMPARE(mutableTrack.interpolation(), Animation::Interpolation::Constant);
            CORRADE_COMPARE(mutableTrack.at(3.0f), true);
        }
    }
}

void AnimationDataTest::constructNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    AnimationData data{DataFlag::Owned, nullptr, {}, {-1.0f, 7.0f}};
    CORRADE_COMPARE(out.str(),
        "Trade::AnimationData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void AnimationDataTest::constructImplicitDurationNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    AnimationData data{DataFlag::Owned, nullptr, {}};
    CORRADE_COMPARE(out.str(),
        "Trade::AnimationData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void AnimationDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<AnimationData, const AnimationData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<AnimationData, const AnimationData&>{}));
}

void AnimationDataTest::constructMove() {
    /* Ugh and here we go again! */
    struct Data {
        Float time;
        Vector3 position;
        Quaternion rotation;
    };
    Containers::Array<char> buffer{sizeof(Data)*3};
    auto view = Containers::arrayCast<Data>(buffer);
    view[0] = {0.0f, {3.0f, 1.0f, 0.1f}, Quaternion::rotation(45.0_degf, Vector3::yAxis())};
    view[1] = {5.0f, {0.3f, 0.6f, 1.0f}, Quaternion::rotation(20.0_degf, Vector3::yAxis())};
    view[2] = {7.5f, {1.0f, 0.3f, 2.1f}, Quaternion{}};

    const int state = 5;
    AnimationData a{std::move(buffer), {
        AnimationTrackData{AnimationTrackTargetType::Translation3D, 42,
            Animation::TrackView<const Float, const Vector3>{
                {view, &view[0].time, view.size(), sizeof(Data)},
                {view, &view[0].position, view.size(), sizeof(Data)},
                Animation::Interpolation::Constant,
                animationInterpolatorFor<Vector3>(Animation::Interpolation::Constant)}},
        AnimationTrackData{AnimationTrackTargetType::Rotation3D, 1337,
            Animation::TrackView<const Float, const Quaternion>{
                {view, &view[0].time, view.size(), sizeof(Data)},
                {view, &view[0].rotation, view.size(), sizeof(Data)},
                Animation::Interpolation::Linear,
                animationInterpolatorFor<Quaternion>(Animation::Interpolation::Linear)}}
        }, {-1.0f, 7.0f}, &state};

    AnimationData b{std::move(a)};

    CORRADE_COMPARE(b.duration(), (Range1D{-1.0f, 7.0f}));
    CORRADE_COMPARE(b.data().size(), sizeof(Data)*3);
    CORRADE_COMPARE(b.trackCount(), 2);
    CORRADE_COMPARE(b.importerState(), &state);

    {
        CORRADE_COMPARE(b.trackType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(b.trackResultType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(b.trackTargetType(0), AnimationTrackTargetType::Translation3D);
        CORRADE_COMPARE(b.trackTarget(0), 42);

        Animation::TrackView<const Float, const Vector3> track = b.track<Vector3>(0);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));
    } {
        CORRADE_COMPARE(b.trackType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(b.trackResultType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(b.trackTargetType(1), AnimationTrackTargetType::Rotation3D);
        CORRADE_COMPARE(b.trackTarget(1), 1337);

        Animation::TrackView<const Float, const Quaternion> track = b.track<Quaternion>(1);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Linear);
        CORRADE_COMPARE(track.at(2.5f), Quaternion::rotation(32.5_degf, Vector3::yAxis()));
    }

    int other;
    AnimationData c{nullptr, nullptr, &other};
    c = std::move(b);

    CORRADE_COMPARE(c.duration(), (Range1D{-1.0f, 7.0f}));
    CORRADE_COMPARE(c.data().size(), sizeof(Data)*3);
    CORRADE_COMPARE(c.trackCount(), 2);
    CORRADE_COMPARE(c.importerState(), &state);

    {
        CORRADE_COMPARE(c.trackType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(c.trackResultType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(c.trackTargetType(0), AnimationTrackTargetType::Translation3D);
        CORRADE_COMPARE(c.trackTarget(0), 42);

        Animation::TrackView<const Float, const Vector3> track = c.track<Vector3>(0);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));
    } {
        CORRADE_COMPARE(c.trackType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(c.trackResultType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(c.trackTargetType(1), AnimationTrackTargetType::Rotation3D);
        CORRADE_COMPARE(c.trackTarget(1), 1337);

        Animation::TrackView<const Float, const Quaternion> track = c.track<Quaternion>(1);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Linear);
        CORRADE_COMPARE(track.at(2.5f), Quaternion::rotation(32.5_degf, Vector3::yAxis()));
    }

    CORRADE_VERIFY(std::is_nothrow_move_constructible<AnimationData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<AnimationData>::value);
}

void AnimationDataTest::mutableAccessNotAllowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const std::pair<Float, bool> keyframes[] {
        {1.0f, true},
        {5.0f, false}
    };

    AnimationData data{{}, keyframes, {
        AnimationTrackData{AnimationTrackTargetType(129), 0,
            Animation::TrackView<const Float, const bool>{keyframes, Animation::Interpolation::Constant}},
        }};
    CORRADE_COMPARE(data.dataFlags(), DataFlags{});

    std::ostringstream out;
    Error redirectError{&out};
    data.mutableData();
    data.mutableTrack(0);
    data.mutableTrack<bool>(0);
    CORRADE_COMPARE(out.str(),
        "Trade::AnimationData::mutableData(): the animation is not mutable\n"
        "Trade::AnimationData::mutableTrack(): the animation is not mutable\n"
        "Trade::AnimationData::mutableTrack(): the animation is not mutable\n");
}

void AnimationDataTest::trackCustomResultType() {
    using namespace Math::Literals;

    struct Data {
        Float time;
        Vector3i position;
    };
    Containers::Array<char> buffer{sizeof(Data)*3};
    auto view = Containers::arrayCast<Data>(buffer);
    view[0] = {0.0f, {300, 100, 10}};
    view[1] = {5.0f, {30, 60, 100}};

    AnimationData data{std::move(buffer), {
        AnimationTrackData{AnimationTrackTargetType::Scaling3D, 0,
            Animation::TrackView<const Float, const Vector3i, Vector3>{
                {view, &view[0].time, view.size(), sizeof(Data)},
                {view, &view[0].position, view.size(), sizeof(Data)},
                [](const Vector3i& a, const Vector3i& b, Float t) -> Vector3 {
                    return Math::lerp(Vector3{a}*0.01f, Vector3{b}*0.01f, t);
                }}}
        }};

    CORRADE_COMPARE((data.track<Vector3i, Vector3>(0).at(2.5f)), (Vector3{1.65f, 0.8f, 0.55f}));
}

void AnimationDataTest::trackWrongIndex() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    AnimationData data{nullptr, nullptr};
    data.trackType(0);
    data.trackResultType(0);
    data.trackTargetType(0);
    data.trackTarget(0);
    data.track(0);

    CORRADE_COMPARE(out.str(),
        "Trade::AnimationData::trackType(): index out of range\n"
        "Trade::AnimationData::trackResultType(): index out of range\n"
        "Trade::AnimationData::trackTargetType(): index out of range\n"
        "Trade::AnimationData::trackTarget(): index out of range\n"
        "Trade::AnimationData::track(): index out of range\n");
}

void AnimationDataTest::trackWrongType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    AnimationData data{nullptr, {
        AnimationTrackData{AnimationTrackType::Vector3i,
            AnimationTrackType::Vector3,
            AnimationTrackTargetType::Scaling3D, 0, {}}
        }};

    data.track<Vector3>(0);

    CORRADE_COMPARE(out.str(), "Trade::AnimationData::track(): improper type requested for Trade::AnimationTrackType::Vector3i\n");
}

void AnimationDataTest::trackWrongResultType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    AnimationData data{nullptr, {
        AnimationTrackData{AnimationTrackType::Vector3i,
            AnimationTrackType::Vector3,
            AnimationTrackTargetType::Scaling3D, 0, {}}
        }};

    data.track<Vector3i, Vector2>(0);

    CORRADE_COMPARE(out.str(), "Trade::AnimationData::track(): improper result type requested for Trade::AnimationTrackType::Vector3\n");
}

void AnimationDataTest::release() {
    const std::pair<Float, bool> keyframes[] {
        {1.0f, true},
        {5.0f, false}
    };

    AnimationData data{{}, keyframes, {
        AnimationTrackData{AnimationTrackTargetType(129), 0,
            Animation::TrackView<const Float, const bool>{keyframes, Animation::Interpolation::Constant}},
        }};
    CORRADE_COMPARE(data.trackCount(), 1);

    Containers::Array<char> released = data.release();
    CORRADE_COMPARE(data.data(), nullptr);
    CORRADE_COMPARE(data.trackCount(), 0);
    CORRADE_COMPARE(static_cast<const void*>(released.data()), keyframes);
}

void AnimationDataTest::debugAnimationTrackType() {
    std::ostringstream out;

    Debug{&out} << AnimationTrackType::DualQuaternion << AnimationTrackType(0xde);
    CORRADE_COMPARE(out.str(), "Trade::AnimationTrackType::DualQuaternion Trade::AnimationTrackType(0xde)\n");
}

void AnimationDataTest::debugAnimationTrackTargetType() {
    std::ostringstream out;

    Debug{&out} << AnimationTrackTargetType::Rotation3D << AnimationTrackTargetType(135) << AnimationTrackTargetType(0x42);
    CORRADE_COMPARE(out.str(), "Trade::AnimationTrackTargetType::Rotation3D Trade::AnimationTrackTargetType::Custom(135) Trade::AnimationTrackTargetType(0x42)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnimationDataTest)
