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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Animation/Player.h"

namespace Magnum { namespace Animation { namespace Test {

struct PlayerTest: TestSuite::Tester {
    explicit PlayerTest();

    void constructEmpty();
    void construct();
    void constructChrono();
    void constructCopy();
    void constructMove();

    void setDurationExtend();
    void setDurationReplace();

    void trackInvalidIndex();

    void advanceNotRunning();
    void advancePlaying();
    void advanceRestart();
    void advanceStop();
    void advancePauseResume();
    void advancePauseStop();
    void advancePlayCount();
    void advancePlayCountInfinite();
    void advanceChrono();
    void advanceZeroDuration();
    void advanceZeroDurationChrono();

    void setState();

    void add();
    void addWithCallback();
    void addWithCallbackTemplate();
    void addWithCallbackOnChange();
    void addWithCallbackOnChangeTemplate();
    void addRawCallback();

    void runFor100YearsFloat();
    void runFor100YearsChrono();

    void debugState();
};

namespace {
    const struct {
        const char* name;
        Float offsetFloat;
        std::chrono::minutes offsetChrono;
        bool failsFloat, failsFuzzyFloat;
    } RunFor100YearsData[]{
        {"0", 0.0f, {}, false, false},
        {"1 minute", 60.0f, std::chrono::minutes(1), false, false},
        {"5 minutes", 5.0f*60.0f, std::chrono::minutes{5}, true, false},
        {"30 minutes", 30.0f*60.0f, std::chrono::minutes{30}, true, false},
        {"1 hour", 60.0f*60.0f, std::chrono::minutes{60}, true, false},
        {"1 day", 24.0f*60.0f*60.0f, std::chrono::minutes{24*60}, true, true},
        {"100 days", 100.0f*24.0f*60.0f*60.0f, std::chrono::minutes{100*24*60}, true, true},
        {"100 years", 100.0f*365.0f*24.0f*60.0f*60.0f,
            /* MSVC 2017 (but not 2015) ICEs when assigning hours to minutes
               (or just any other two different chrono types) in a struct array
               initializer (not when there's just a struct and also not when
               the struct is only a chrono member itself). Keeping at least one
               instance here so I can monitor when this gets fixed. */
            #if !defined(CORRADE_MSVC2017_COMPATIBILITY) || defined(CORRADE_MSVC2015_COMPATIBLITY)
            std::chrono::hours{100*365*24},
            #else
            std::chrono::minutes{100*365*24*60},
            #endif
            true, true},
    };
}

PlayerTest::PlayerTest() {
    addTests({&PlayerTest::constructEmpty,
              &PlayerTest::construct,
              &PlayerTest::constructCopy,
              &PlayerTest::constructMove,

              &PlayerTest::setDurationExtend,
              &PlayerTest::setDurationReplace,

              &PlayerTest::trackInvalidIndex,

              &PlayerTest::advanceNotRunning,
              &PlayerTest::advancePlaying,
              &PlayerTest::advanceRestart,
              &PlayerTest::advanceStop,
              &PlayerTest::advancePauseResume,
              &PlayerTest::advancePauseStop,
              &PlayerTest::advancePlayCount,
              &PlayerTest::advancePlayCountInfinite,
              &PlayerTest::advanceChrono,
              &PlayerTest::advanceZeroDuration,
              &PlayerTest::advanceZeroDurationChrono,

              &PlayerTest::setState,

              &PlayerTest::add,
              &PlayerTest::addWithCallback,
              &PlayerTest::addWithCallbackTemplate,
              &PlayerTest::addWithCallbackOnChange,
              &PlayerTest::addWithCallbackOnChangeTemplate,
              &PlayerTest::addRawCallback});

    addInstancedTests({
        &PlayerTest::runFor100YearsFloat,
        &PlayerTest::runFor100YearsChrono},
        Containers::arraySize(RunFor100YearsData));

    addTests({&PlayerTest::debugState});
}

void PlayerTest::constructEmpty() {
    Player<Float> player;
    CORRADE_VERIFY(player.scaler());
    CORRADE_COMPARE(player.duration(), Range1D{});
    CORRADE_COMPARE(player.playCount(), 1);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_VERIFY(player.isEmpty());
    CORRADE_COMPARE(player.size(), 0);
}

namespace {
    const Animation::Track<Float, Float> Track{{
        {1.0f, 1.5f},
        {2.5f, 3.0f},
        {3.0f, 5.0f},
        {4.0f, 2.0f}
    }, Math::lerp};
}

void PlayerTest::construct() {
    Animation::Track<Float, Int> track2{{
        {0.5f, 42},
        {3.0f, 1337},
        {3.5f, -17}
    }, Math::select};

    Float value = -1.0f;
    Int value2 = -1;
    Player<Float> player;
    player.add(Track, value)
        .add(track2, value2)
        .setPlayCount(37);
    CORRADE_VERIFY(player.scaler());
    CORRADE_COMPARE(player.duration(), (Range1D{0.5f, 4.0f}));
    CORRADE_COMPARE(player.playCount(), 37);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_VERIFY(!player.isEmpty());
    CORRADE_COMPARE(player.size(), 2);
    CORRADE_COMPARE(player.track(0).keys().data(), Track.keys().data());
    CORRADE_COMPARE(player.track(1).keys().data(), track2.keys().data());
}

void PlayerTest::constructChrono() {
    Animation::Track<Float, Int> track2{{
        {0.5f, 42},
        {3.0f, 1337},
        {3.5f, -17}
    }, Math::select};

    Float value = -1.0f;
    Int value2 = -1;
    Player<std::chrono::nanoseconds, Float> player;
    player.add(Track, value)
        .add(track2, value2)
        .setPlayCount(37);
    CORRADE_VERIFY(player.scaler());
    CORRADE_COMPARE(player.duration(), (Range1D{0.5f, 4.0f}));
    CORRADE_COMPARE(player.playCount(), 37);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_VERIFY(!player.isEmpty());
    CORRADE_COMPARE(player.size(), 2);
    CORRADE_COMPARE(player.track(0).keys().data(), Track.keys().data());
    CORRADE_COMPARE(player.track(1).keys().data(), track2.keys().data());
}

void PlayerTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Player<Float>, const Player<Float>&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Player<Float>, const Player<Float>&>{}));
}

void PlayerTest::constructMove() {
    Animation::Track<Float, Int> track2{{
        {0.5f, 42},
        {3.0f, 1337},
        {3.5f, -17}
    }, Math::select};

    Float value = -1.0f;
    Int value2 = -1;
    Player<Float> a;
    a.add(Track, value)
     .add(track2, value2)
     .setPlayCount(37)
     .play({});
    CORRADE_COMPARE(a.duration(), (Range1D{0.5f, 4.0f}));
    CORRADE_COMPARE(a.playCount(), 37);
    CORRADE_COMPARE(a.state(), State::Playing);
    CORRADE_VERIFY(!a.isEmpty());
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.track(0).keys().data(), Track.keys().data());
    CORRADE_COMPARE(a.track(1).keys().data(), track2.keys().data());

    Player<Float> b{std::move(a)};
    CORRADE_COMPARE(b.duration(), (Range1D{0.5f, 4.0f}));
    CORRADE_COMPARE(b.playCount(), 37);
    CORRADE_COMPARE(b.state(), State::Playing);
    CORRADE_VERIFY(!b.isEmpty());
    CORRADE_COMPARE(b.size(), 2);
    CORRADE_COMPARE(b.track(0).keys().data(), Track.keys().data());
    CORRADE_COMPARE(b.track(1).keys().data(), track2.keys().data());

    Player<Float> c;
    c.setDuration({1.2f, 1.3f});
    c = std::move(b);
    CORRADE_COMPARE(c.duration(), (Range1D{0.5f, 4.0f}));
    CORRADE_COMPARE(c.playCount(), 37);
    CORRADE_COMPARE(c.state(), State::Playing);
    CORRADE_VERIFY(!c.isEmpty());
    CORRADE_COMPARE(c.size(), 2);
    CORRADE_COMPARE(c.track(0).keys().data(), Track.keys().data());
    CORRADE_COMPARE(c.track(1).keys().data(), track2.keys().data());
}

void PlayerTest::setDurationExtend() {
    Float value;
    Player<Float> player;
    player.setDuration({-1.0f, 2.0f});
    CORRADE_COMPARE(player.duration(), (Range1D{-1.0f, 2.0f}));

    player.add(Track, value);
    CORRADE_COMPARE(player.duration(), (Range1D{-1.0f, 4.0f}));
}

void PlayerTest::setDurationReplace() {
    Float value;
    Player<Float> player;
    player.add(Track, value);
    CORRADE_COMPARE(player.duration(), (Range1D{1.0f, 4.0f}));

    player.setDuration({-1.0f, 2.0f});
    CORRADE_COMPARE(player.duration(), (Range1D{-1.0f, 2.0f}));
}

void PlayerTest::trackInvalidIndex() {
    std::ostringstream out;
    Error redirectError{&out};

    Float value;
    Player<Float> player;
    /* Adding at least one track so the return in the graceful assert can
       return the first value and not trigger MSVC debug iterator abort */
    player.add(TrackView<Float, Float>{}, value);

    player.track(1);

    CORRADE_COMPARE(out.str(), "Animation::Player::track(): index out of range\n");
}

void PlayerTest::advanceNotRunning() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value);

    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, -1.0f);

    player.advance(1.75f);

    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::advancePlaying() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 3.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);

    /* 2.67 secs in */
    player.advance(4.6666667f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 3.0f);

    /* When the player gets stopped, the value at the stop time is written */
    player.advance(5.5f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, 2.0f);

    /* But further advancing will not write anything */
    value = -1.0f;
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::advanceRestart() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(2.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);

    /* Call play again, will restart from the beginning... */
    value = -1.0f;
    player.play(4.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* ... but only after calling advance() again. Now at 1 sec in. */
    player.advance(5.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 2.5f);
}

void PlayerTest::advanceStop() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(2.0f);

    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);

    /* Stop, should not update anything */
    value = -1.0;
    player.stop();
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, -1.0f);

    /* Advancing will update with a value from beginning of the duration */
    player.advance(5.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, 1.5f);

    /* But further advancing will not write anything */
    value = -1.0f;
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::advancePauseResume() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(2.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);

    /* Pausing should not update anything */
    value = -1.0f;
    player.pause(4.0f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(value, -1.0f);

    /* Pausing again should be a no-op */
    player.pause(4.1f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(value, -1.0f);

    /* But advance() after should. No matter what time is passed to it, it
       should update with time of pause. */
    player.advance(4.5f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(value, 5.0f); /* value at 2.0f, not 2.5f */

    /* Advancing further should do nothing */
    value = -1.0f;
    player.advance(50.0f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(value, -1.0f);

    /* Resuming the animation, again should not update anything */
    player.play(100.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* Advancing the animation should update again. It was paused after two
       seconds, so continuing at 2.5 seconds now. */
    player.advance(100.5f);
    CORRADE_COMPARE(value, 3.5f);
}

void PlayerTest::advancePauseStop() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(2.0f);

    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);

    /* Pause, get value from the pause time */
    player.pause(4.0f);
    player.advance(4.5f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(value, 5.0f);

    /* Stop, should not update anything */
    value = -1.0;
    player.stop();
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, -1.0f);

    /* Advancing will update with a value from beginning of the duration */
    player.advance(5.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, 1.5f);

    /* But further advancing will not write anything */
    value = -1.0f;
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, -1.0f);

    /* Pause while stopped is a no-op */
    player.pause(101.0f);
    player.advance(101.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::advancePlayCount() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .setPlayCount(3)
        .play(2.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);

    /* 2 secs in, second round */
    player.advance(7.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 5.0f);

    /* 1.75 secs in, third round */
    player.advance(9.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);

    /* When the player gets stopped, the value at the stop time is written */
    player.advance(11.5f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, 2.0f);

    /* But further advancing will not write anything */
    value = -1.0f;
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::advancePlayCountInfinite() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .setPlayCount(0)
        .play(2.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);

    /* 2 secs in, second round */
    player.advance(7.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 5.0f);

    /* 1.75 secs in, 10th round */
    player.advance(33.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::advanceChrono() {
    Float value = -1.0f;
    Player<std::chrono::nanoseconds, Float> player;
    player.add(Track, value)
        .play(std::chrono::seconds{2});

    CORRADE_COMPARE(player.duration().size(), 3.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(std::chrono::milliseconds{1750});
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(std::chrono::milliseconds{3750});
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::advanceZeroDuration() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        /* 1.75 secs since the start of the original duration */
        .setDuration(Range1D::fromSize(1.0f + 1.75f, 0.0f))
        .setPlayCount(0)
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 0.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* After that, the value at 1.75 secs is returned independent of time */
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::advanceZeroDurationChrono() {
    Float value = -1.0f;
    Player<std::chrono::nanoseconds, Float> player;
    player.add(Track, value)
        /* 1.75 secs since the start of the original duration */
        .setDuration(Range1D::fromSize(1.0f + 1.75f, 0.0f))
        .setPlayCount(0)
        .play(std::chrono::seconds{2});

    CORRADE_COMPARE(player.duration().size(), 0.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(std::chrono::milliseconds{1750});
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* After that, the value at 1.75 seconds is returned independent of the
       time */
    player.advance(std::chrono::seconds{100});
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::setState() {
    Player<Float> player;
    CORRADE_COMPARE(player.state(), State::Stopped);

    player.setState(State::Playing, {});
    CORRADE_COMPARE(player.state(), State::Playing);

    player.setState(State::Paused, {});
    CORRADE_COMPARE(player.state(), State::Paused);

    player.setState(State::Stopped, {});
    CORRADE_COMPARE(player.state(), State::Stopped);
}

void PlayerTest::add() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 3.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::addWithCallback() {
    struct Data {
        Float value = -1.0f;
        Int called = 0;
    } data;
    Player<Float> player;
    player.addWithCallback(Track, [](const Float&, const Float& value, void* userData) {
        static_cast<Data*>(userData)->value = value;
        ++static_cast<Data*>(userData)->called;
    }, &data)
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 3.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(data.value, -1.0f);
    CORRADE_COMPARE(data.called, 0);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(data.value, 4.0f);
    CORRADE_COMPARE(data.called, 1);
}

void PlayerTest::addWithCallbackTemplate() {
    struct Data {
        Float value = -1.0f;
        Int called = 0;
    } data;
    Player<Float> player;
    player.addWithCallback(Track, [](const Float&, const Float& value, Data& userData) {
        userData.value = value;
        ++userData.called;
    }, data)
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 3.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(data.value, -1.0f);
    CORRADE_COMPARE(data.called, 0);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(data.value, 4.0f);
    CORRADE_COMPARE(data.called, 1);
}

void PlayerTest::addWithCallbackOnChange() {
    struct Data {
        Float value = -1.0f;
        Int called = 0;
    } data;
    Player<Float> player;
    player.addWithCallbackOnChange(Track, [](const Float&, const Float& value, void* userData) {
        static_cast<Data*>(userData)->value = value;
        ++static_cast<Data*>(userData)->called;
    }, data.value, &data)
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 3.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(data.value, -1.0f);
    CORRADE_COMPARE(data.called, 0);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(data.value, 4.0f);
    CORRADE_COMPARE(data.called, 1);

    /* At the same time, same value, should not be called again */
    player.advance(3.75f);
    CORRADE_COMPARE(data.value, 4.0f);
    CORRADE_COMPARE(data.called, 1);

    /* Different time, different value, called again */
    player.advance(4.0f);
    CORRADE_COMPARE(data.value, 5.0f);
    CORRADE_COMPARE(data.called, 2);
}

void PlayerTest::addWithCallbackOnChangeTemplate() {
    struct Data {
        Float value = -1.0f;
        Int called = 0;
    } data;
    Player<Float> player;
    player.addWithCallbackOnChange(Track, [](const Float&, const Float& value, Data& userData) {
        userData.value = value;
        ++userData.called;
    }, data.value, data)
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 3.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(data.value, -1.0f);
    CORRADE_COMPARE(data.called, 0);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(data.value, 4.0f);
    CORRADE_COMPARE(data.called, 1);

    /* At the same time, same value, should not be called again */
    player.advance(3.75f);
    CORRADE_COMPARE(data.value, 4.0f);
    CORRADE_COMPARE(data.called, 1);

    /* Different time, different value, called again */
    player.advance(4.0f);
    CORRADE_COMPARE(data.value, 5.0f);
    CORRADE_COMPARE(data.called, 2);
}

namespace {
    /* Can't use raw lambdas because MSVC 2015 */
    void callback(std::vector<Int>& data, Int value) {
        data.push_back(value);
    }
}

void PlayerTest::addRawCallback() {
    Animation::Track<Float, Int> track;

    Int result = -1;
    std::vector<Int> data;

    Animation::Player<Float> player;
    player.addRawCallback(track,
        [](const Animation::TrackViewStorage<Float>& track, Float key,
        std::size_t& hint, void* destination, void(*callback)(), void* userData) {
            Int value = static_cast<const Animation::TrackView<Float, Int>&>(track).at(key, hint);
            if(value == *static_cast<Int*>(destination)) return;
            *static_cast<Int*>(destination) = value;
            reinterpret_cast<void(*)(std::vector<Int>&, Int)>(callback)(*static_cast<std::vector<Int>*>(userData), value);
        }, &result, reinterpret_cast<void(*)()>(callback), &data)
        .play({});

    /* Should add the default-constructed value into the vector, but only once */
    CORRADE_COMPARE(data, std::vector<Int>{});
    player.advance({});
    CORRADE_COMPARE(data, std::vector<Int>{0});
    player.advance(1.0f);
    CORRADE_COMPARE(data, std::vector<Int>{0});
}

void PlayerTest::runFor100YearsFloat() {
    auto&& data = RunFor100YearsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .setPlayCount(0)
        .play({});

    /* The track must fit an integer number of times into the day for this test
       to work (3 seconds do fit) */
    CORRADE_COMPARE(player.duration().size()[0], 3.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* 2.67 secs in given iteration. Comparing with a slightly larger epsilon,
       because it fails right after five minutes otherwise. */
    player.advance(data.offsetFloat + 2.6666666666667f);

    if(data.failsFloat || data.failsFuzzyFloat)
        Debug{} << "Calculated value:" << value;

    CORRADE_COMPARE(player.state(), State::Playing);
    {
        /* Asm.js uses doubles for all floating-point calculations, so we don't
           lose any precision and thus even the "run for 100 years" test passes
           there. Unfortunately it's not possible to detect if this is asm.js
           so the XFAIL is done like this. */
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        CORRADE_EXPECT_FAIL_IF(data.failsFuzzyFloat, "Imprecision larger than 2.5e-4f.");
        #else
        CORRADE_EXPECT_FAIL_IF(data.failsFuzzyFloat && !Math::TypeTraits<Float>::equals(value, 3.0f), "Imprecision larger than 2.5e-4f.");
        #endif
        CORRADE_COMPARE_WITH(value, 3.0f, TestSuite::Compare::around(0.00025f));
    }
    if(!data.failsFuzzyFloat) {
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        CORRADE_EXPECT_FAIL_IF(data.failsFloat, "Imprecision larger than 1e-6f.");
        #else
        CORRADE_EXPECT_FAIL_IF(data.failsFloat && !Math::TypeTraits<Float>::equals(value, 3.0f), "Imprecision larger than 2.5e-4f.");
        #endif
        CORRADE_COMPARE(value, 3.0f);
    }
}

void PlayerTest::runFor100YearsChrono() {
    auto&& data = RunFor100YearsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Float value = -1.0f;
    Player<std::chrono::nanoseconds, Float> player;
    player.add(Track, value)
        .setPlayCount(0)
        .play({});

    /* The track must fit an integer number of times into the day for this test
       to work (3 seconds do fit) */
    CORRADE_COMPARE(player.duration().size()[0], 3.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* 2.67 secs in */
    player.advance(data.offsetChrono + std::chrono::nanoseconds{2666666667ull});
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 3.0f);
}

void PlayerTest::debugState() {
    std::ostringstream out;

    Debug{&out} << State::Playing << State(0xde);
    CORRADE_COMPARE(out.str(), "Animation::State::Playing Animation::State(0xde)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::PlayerTest)
