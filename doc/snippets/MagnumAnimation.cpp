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

#include "Magnum/Timeline.h"
#include "Magnum/Math/Bezier.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Quaternion.h"
#include "Magnum/Math/Packing.h"
#include "Magnum/Animation/Easing.h"
#include "Magnum/Animation/Player.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {

{
Float t{};
{
Vector3 a, b;
/* [ease] */
auto lerpBounceIn =
    Animation::ease<Vector3, Math::lerp, Animation::Easing::bounceIn>();

Vector3 result1 = Math::lerp(a, b, Animation::Easing::bounceIn(t));
Vector3 result2 = lerpBounceIn (a, b, t);
/* [ease] */
static_cast<void>(result1);
static_cast<void>(result2);
}

{
/* [unpack] */
UnsignedShort a, b;
auto lerpPacked =
    Animation::unpack<UnsignedShort, Float, Math::lerp, Math::unpack<Float>>();

Float result1 = Math::lerp(Math::unpack<Float>(a), Math::unpack<Float>(b), t);
Float result2 = lerpPacked(a, b, t);
/* [unpack] */
static_cast<void>(result1);
static_cast<void>(result2);
}

{
/* [unpackEase] */
UnsignedShort a, b;
auto lerpPackedBounceIn = Animation::unpackEase<UnsignedShort, Float,
    Math::lerp, Math::unpack<Float>, Animation::Easing::bounceIn>();

Float result1 = Math::lerp(Math::unpack<Float>(a), Math::unpack<Float>(b),
    Animation::Easing::bounceIn(t));
Float result2 = lerpPackedBounceIn(a, b, t);
/* [unpackEase] */
static_cast<void>(result1);
static_cast<void>(result2);
}
}

{
Vector3 a, b;
Float t{};
{
/* [Easing-factor] */
Vector3 result = Math::lerp(a, b, Animation::Easing::quadraticInOut(t));
/* [Easing-factor] */
static_cast<void>(result);
}

{
/* [Easing-ease] */
auto lerpQuadraticInOut =
    Animation::ease<Vector3, Math::lerp, Animation::Easing::quadraticInOut>();

Vector3 result = lerpQuadraticInOut(a, b, t);
/* [Easing-ease] */
static_cast<void>(result);
}

{
/* [Easing-clamp] */
auto lerpCircularOutClamped = Animation::easeClamped<
    Vector3, Math::lerp, Animation::Easing::quadraticInOut>();

Vector3 result1 = Math::lerp(a, b,
    Math::clamp(0.0f, 1.0f, Animation::Easing::circularOut(t)));
Vector3 result2 = lerpCircularOutClamped(a, b, t);
/* [Easing-clamp] */
static_cast<void>(result1);
static_cast<void>(result2);
}

{
/* [Easing-bezier-transform] */
Matrix3 transformation;
CubicBezier2D easing;
CubicBezier2D transformed{
    transformation.transformPoint(easing[0]),
    transformation.transformPoint(easing[1]),
    transformation.transformPoint(easing[2]),
    transformation.transformPoint(easing[3])};
/* [Easing-bezier-transform] */
}

{
/* [Easing-smoothstep] */
Math::lerp(a, b, Animation::Easing::smoothstep(t));
/* [Easing-smoothstep] */
}
}

{
/* [Player-usage] */
const Animation::TrackView<Float, Vector3> translation;
const Animation::TrackView<Float, Quaternion> rotation;
const Animation::TrackView<Float, Vector3> scaling;

Vector3 objectScaling;
Quaternion objectRotation;
Vector3 objectTranslation;

Animation::Player<Float> player;
player.add(scaling, objectScaling)
      .add(rotation, objectRotation)
      .add(translation, objectTranslation);
/* [Player-usage] */
}

/* WinRT has warnings-as-errors and fails on the unitialized object var */
#ifndef CORRADE_TARGET_WINDOWS_RT
{
const Animation::TrackView<Float, Vector3> translation;
const Animation::TrackView<Float, Quaternion> rotation;
const Animation::TrackView<Float, Vector3> scaling;
struct Object3D {
    Object3D& setTranslation(const Vector3&) { return *this; }
    Object3D& setRotation(const Quaternion&) { return *this; }
    Object3D& setScaling(const Vector3&) { return *this; }
};
#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
/* [Player-usage-callback] */
Object3D* object;

Animation::Player<Float> player;
player.addWithCallback(scaling,
    [](Float, const Vector3& scaling, Object3D& object) {
        object.setScaling(scaling);
    }, *object);
player.addWithCallback(rotation,
    [](Float, const Quaternion& rotation, Object3D& object) {
        object.setRotation(rotation);
    }, *object);
player.addWithCallback(translation,
    [](Float, const Vector3& translation, Object3D& object) {
        object.setTranslation(translation);
    }, *object);
/* [Player-usage-callback] */
#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
}
#endif

{
/* [Player-usage-playback] */
Animation::Player<Float> player;
Timeline timeline;

// during initialization
timeline.start();
player.play(timeline.previousFrameTime());

// every frame
player.advance(timeline.previousFrameTime());
/* [Player-usage-playback] */
}

{
/* [Player-usage-chrono] */
Animation::Player<std::chrono::nanoseconds, Float> player;
// add tracks…

// start the animation
player.play(std::chrono::system_clock::now().time_since_epoch());

// call every frame
player.advance(std::chrono::system_clock::now().time_since_epoch());
/* [Player-usage-chrono] */
}

{
/* [Player-higher-order] */
struct Data {
    Animation::Player<Float> player; // player we want to control
    Timeline timeline;
} data;

Animation::Track<Float, Animation::State> stateTrack{{
    {3.0f, Animation::State::Playing},
    {3.0f, Animation::State::Paused},
    {3.5f, Animation::State::Playing},
    {5.0f, Animation::State::Stopped}
}, Math::select};
Animation::State state;

Animation::Player<Float> controller;
controller.addWithCallbackOnChange(stateTrack,
    [](Float, const Animation::State& state, Data& data) {
        data.player.setState(state, data.timeline.previousFrameTime());
    }, state, data);
/* [Player-higher-order] */
}

{
Timeline timeline;
/* [Player-higher-order-animated-time] */
Animation::Player<Float> player; // player we want to control

Animation::Track<Float, Float> timeTrack{{
    {0.0f, 0.0f}, /* Start normal */
    {1.0f, 1.0f}, /* Then speed up */
    {2.0f, 3.0f}, /* Pause for a bit */
    {5.0f, 3.0f}, /* And normal again */
    {6.0f, 4.0f}
}, Animation::Interpolation::Linear};

Animation::Player<Float> timer;
timer.addWithCallback(timeTrack,
    [](Float, const Float& time, Animation::Player<Float>& player) {
        player.advance(time);
    }, player);

/* Calls player.advance() with the animated time */
timer.advance(timeline.previousFrameTime());
/* [Player-higher-order-animated-time] */
}

{
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Can't call + on lambdas */
/* [Player-addRawCallback] */
Animation::Track<Float, Int> track;

Int result;
std::vector<Int> data;
auto callback = [](std::vector<Int>& data, Int value) {
    data.push_back(value);
};

Animation::Player<Float> player;
player.addRawCallback(track,
    [](const Animation::TrackViewStorage<const Float>& track, Float key,
    std::size_t& hint, void* destination, void(*callback)(), void* userData) {
        Int value = static_cast<const Animation::TrackView<const Float, const Int>&>(track)
            .atStrict(key, hint);
        if(value == *static_cast<Int*>(destination)) return;
        *static_cast<Int*>(destination) = value;
        reinterpret_cast<void(*)(std::vector<Int>&, Int)>(callback)
            (*static_cast<std::vector<Int>*>(userData), value);
    }, &result, reinterpret_cast<void(*)()>(+callback), &data);
/* [Player-addRawCallback] */
#endif
}

{
/* [Track-usage] */
const Animation::Track<Float, Vector2> jump{{
    {0.0f, Vector2::yAxis(0.0f)},
    {1.0f, Vector2::yAxis(0.5f)},
    {2.0f, Vector2::yAxis(0.75f)},
    {3.0f, Vector2::yAxis(0.875f)},
    {4.0f, Vector2::yAxis(0.75f)},
    {5.0f, Vector2::yAxis(0.5f)},
    {6.0f, Vector2::yAxis(0.0f)}
}, Math::lerp, Animation::Extrapolation::Constant};

Vector2 position = jump.at(2.2f);               // y = 0.775
/* [Track-usage] */
static_cast<void>(position);
}

{
const Animation::Track<Float, Vector2> jump;
/* [Track-performance-hint] */
std::size_t hint = 0;
Vector2 position = jump.at(2.2f, hint);         // y = 0.775, hint = 2
/* [Track-performance-hint] */
static_cast<void>(position);
}

{
const Animation::Track<Float, Vector2> jump;
/* [Track-performance-strict] */
std::size_t hint = 0;
Vector2 position = jump.atStrict(2.2f, hint);   // y = 0.775, hint = 2
/* [Track-performance-strict] */
static_cast<void>(position);
}

{
/* [Track-performance-cache] */
struct Keyframe {
    Float time;
    Vector2 position;
    Deg rotation;
};
const Keyframe data[]{
    {0.0f, Vector2::yAxis(0.0f), 0.0_degf},
    {1.0f, Vector2::yAxis(0.5f), 60.0_degf},
    {2.0f, Vector2::yAxis(0.75f), 80.0_degf},
    {3.0f, Vector2::yAxis(0.875f), 90.0_degf},
    {4.0f, Vector2::yAxis(0.75f), 100.0_degf},
    {5.0f, Vector2::yAxis(0.5f), 120.0_degf},
    {6.0f, Vector2::yAxis(0.0f), 180.0_degf}
};

Animation::TrackView<const Float, const Vector2> positions{
    {data, &data[0].time, Containers::arraySize(data), sizeof(Keyframe)},
    {data, &data[0].position, Containers::arraySize(data), sizeof(Keyframe)},
    Math::lerp};
Animation::TrackView<const Float, const Deg> rotations{
    {data, &data[0].time, Containers::arraySize(data), sizeof(Keyframe)},
    {data, &data[0].rotation, Containers::arraySize(data), sizeof(Keyframe)},
    Math::lerp};

Float time = 2.2f;
std::size_t hint = 0;
Vector2 position = positions.atStrict(time, hint);  // y = 0.775f
Deg rotation = rotations.atStrict(time, hint);      // φ = 82°
/* [Track-performance-cache] */
static_cast<void>(position);
static_cast<void>(rotation);
}

}
