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
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Animation/Player.h"

namespace Magnum { namespace Animation { namespace Test { namespace {

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
    void advanceResume();
    void advanceStop();
    void advancePauseResume();
    void advancePauseStop();
    void advancePauseStopped();
    void advancePauseTooLate();
    void advancePlayCount();
    void advancePlayCountInfinite();
    void advanceChrono();
    void advanceList();
    void advanceZeroDurationStop();
    void advanceZeroDurationPause();
    void advanceZeroDurationInfinitePlayCount();
    void advanceZeroDurationInfinitePlayCountChrono();

    void seekByStopped();
    void seekByPlaying();
    void seekByPaused();
    void seekByPausedParked();

    void seekToStopped();
    void seekToPlaying();
    void seekToPaused();
    void seekToPausedParked();

    void setState();

    template<class T> const T& addTemplateTrack();
    template<class T> void add();
    template<class T> void addWithCallback();
    template<class T> void addWithCallbackTemplate();
    template<class T> void addWithCallbackOnChange();
    template<class T> void addWithCallbackOnChangeTemplate();
    template<class T> void addRawCallback();

    void runFor100YearsFloat();
    void runFor100YearsChrono();

    void debugState();

    Animation::Track<Float, Float> mutableTrack{{
        {1.0f, 1.5f},
        {2.5f, 3.0f},
        {3.0f, 5.0f},
        {4.0f, 2.0f}
    }, Math::lerp};
    Animation::TrackView<Float, Float> mutableView = mutableTrack;
};

const struct {
    const char* name;
    Float offsetFloat;
    std::chrono::minutes offsetChrono;
    bool failsFloat, failsFuzzyFloat;
} RunFor100YearsData[]{
    {"0", 0.0f, {}, false, false},
    {"1 minute", 60.0f, std::chrono::minutes(1), false, false},
    {"5 minutes", 5.0f*60.0f, std::chrono::minutes{5}, false, false},
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

PlayerTest::PlayerTest() {
    addTests({&PlayerTest::constructEmpty,
              &PlayerTest::construct,
              &PlayerTest::constructChrono,
              &PlayerTest::constructCopy,
              &PlayerTest::constructMove,

              &PlayerTest::setDurationExtend,
              &PlayerTest::setDurationReplace,

              &PlayerTest::trackInvalidIndex,

              &PlayerTest::advanceNotRunning,
              &PlayerTest::advancePlaying,
              &PlayerTest::advanceRestart,
              &PlayerTest::advanceResume,
              &PlayerTest::advanceStop,
              &PlayerTest::advancePauseResume,
              &PlayerTest::advancePauseStop,
              &PlayerTest::advancePauseStopped,
              &PlayerTest::advancePauseTooLate,
              &PlayerTest::advancePlayCount,
              &PlayerTest::advancePlayCountInfinite,
              &PlayerTest::advanceChrono,
              &PlayerTest::advanceList,
              &PlayerTest::advanceZeroDurationStop,
              &PlayerTest::advanceZeroDurationPause,
              &PlayerTest::advanceZeroDurationInfinitePlayCount,
              &PlayerTest::advanceZeroDurationInfinitePlayCountChrono,

              &PlayerTest::seekByStopped,
              &PlayerTest::seekByPlaying,
              &PlayerTest::seekByPaused,
              &PlayerTest::seekByPausedParked,

              &PlayerTest::seekToStopped,
              &PlayerTest::seekToPlaying,
              &PlayerTest::seekToPaused,
              &PlayerTest::seekToPausedParked,

              &PlayerTest::setState,

              &PlayerTest::add<Track<Float, Float>>,
              &PlayerTest::add<TrackView<Float, Float>>,
              &PlayerTest::addWithCallback<Track<Float, Float>>,
              &PlayerTest::addWithCallback<TrackView<Float, Float>>,
              &PlayerTest::addWithCallbackTemplate<Track<Float, Float>>,
              &PlayerTest::addWithCallbackTemplate<TrackView<Float, Float>>,
              &PlayerTest::addWithCallbackOnChange<Track<Float, Float>>,
              &PlayerTest::addWithCallbackOnChange<TrackView<Float, Float>>,
              &PlayerTest::addWithCallbackOnChangeTemplate<Track<Float, Float>>,
              &PlayerTest::addWithCallbackOnChangeTemplate<TrackView<Float, Float>>,
              &PlayerTest::addRawCallback<Track<Float, Float>>,
              &PlayerTest::addRawCallback<TrackView<Float, Float>>});

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

const Animation::Track<Float, Float> Track{{
    {1.0f, 1.5f},
    {2.5f, 3.0f},
    {3.0f, 5.0f},
    {4.0f, 2.0f}
}, Math::lerp};

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

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Player<Float>>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Player<Float>>::value);
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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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
    CORRADE_COMPARE(player.elapsed(0.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Asking for elapsed doesn't change anything */
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    player.advance(1.75f);

    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::advancePlaying() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 3.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(0.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Asking for elapsed will say it's playing already, but doesn't change
       anything */
    CORRADE_COMPARE(player.elapsed(3.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(3.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* 2.67 secs in */
    player.advance(4.666666667f);
    CORRADE_COMPARE(player.state(), State::Playing);
    /** @todo make std::pair/std::tuple comparisons respect fuzzyness inside */
    CORRADE_COMPARE(player.elapsed(4.666666667f).first, 0);
    CORRADE_COMPARE(player.elapsed(4.666666667f).second, 2.666666667f);
    CORRADE_COMPARE(value, 3.0f);

    /* Asking for elapsed will say the stop time, but again doesn't change the
       state */
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(5.5f), std::make_pair(0, 3.0f));
    CORRADE_COMPARE(value, 3.0f);

    /* When the player gets stopped, the value at the stop time is written.
       Elapsed time still shows that it stopped by itself. */
    player.advance(5.5f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(5.5f), std::make_pair(0, 3.0f));
    CORRADE_COMPARE(value, 2.0f);

    /* But further advancing will not write anything */
    value = -1.0f;
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(100.0f), std::make_pair(0, 3.0f));
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::advanceRestart() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(2.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(0.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(3.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* Call play again, will restart from the beginning... */
    value = -1.0f;
    player.play(4.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(4.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* ... but only after calling advance() again. Now at 1 sec in. */
    player.advance(5.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(5.0f), std::make_pair(0, 1.0f));
    CORRADE_COMPARE(value, 2.5f);
}

void PlayerTest::advanceResume() {
    /* A variant of advanceRestart() that doesn't restart */

    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .resume(2.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(0.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(3.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* Calling resume() will not restart from the beginning */
    value = -1.0f;
    player.resume(4.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(4.0f), std::make_pair(0, 2.0f));
    CORRADE_COMPARE(value, -1.0f);

    player.advance(4.5f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(4.5f), std::make_pair(0, 2.5f));
    CORRADE_COMPARE(value, 3.5f);
}

void PlayerTest::advanceStop() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(2.0f);

    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(3.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* Stop, should not update anything. Elapsed will report a time from the
       beginning again. */
    value = -1.0;
    player.stop();
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(5.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Advancing will update with a value from beginning of the duration.
       Elapsed shows the same. */
    player.advance(5.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(5.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, 1.5f);

    /* But further advancing will not write anything */
    value = -1.0f;
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(100.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::advancePauseResume() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(22.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, -1.0f);

    player.advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* Pausing should not update anything */
    value = -1.0f;
    player.pause(24.0f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(24.0f), std::make_pair(0, 2.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Pausing again should be a no-op */
    player.pause(24.1f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(24.1f), std::make_pair(0, 2.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* But advance() after should. No matter what time is passed to it, it
       should update with time of pause. */
    player.advance(24.5f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(24.5f), std::make_pair(0, 2.0f));
    CORRADE_COMPARE(value, 5.0f); /* value at 2.0f, not 2.5f */

    /* Advancing further should do nothing */
    value = -1.0f;
    player.advance(50.0f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(50.0f), std::make_pair(0, 2.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Resuming the animation, again should not update anything */
    player.play(100.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(100.0f), std::make_pair(0, 2.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Advancing the animation should update again. It was paused after two
       seconds, so continuing at 2.5 seconds now. */
    player.advance(100.5f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(100.5f), std::make_pair(0, 2.5f));
    CORRADE_COMPARE(value, 3.5f);
}

void PlayerTest::advancePauseStop() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(2.0f);

    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(3.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* Pause, get value from the pause time */
    player.pause(4.0f);
    player.advance(4.5f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(3.75f), std::make_pair(0, 2.0f));
    CORRADE_COMPARE(value, 5.0f);

    /* Stop, should not update anything */
    value = -1.0;
    player.stop();
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(5.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Advancing will update with a value from beginning of the duration */
    player.advance(5.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(5.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, 1.5f);

    /* But further advancing will not write anything */
    value = -1.0f;
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(100.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Pause while stopped is a no-op */
    player.pause(101.0f);
    player.advance(101.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(101.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::advancePauseStopped() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(22.0f);

    player.advance(50.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(50.0f), std::make_pair(0, 3.0f));
    CORRADE_COMPARE(value, 2.0f);

    /* Pausing a stopped animation should change nothing */
    value = -1.0f;
    player.pause(50.5f);
    player.advance(51.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(51.0f), std::make_pair(0, 3.0f));
    CORRADE_COMPARE(value, -1);
}

void PlayerTest::advancePauseTooLate() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(22.0f);

    player.advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* Pausing too late will set the state to paused */
    player.pause(50.0f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(50.0f), std::make_pair(0, 3.0f));

    /* And advancing will keep it paused, not transforming to stopped */
    player.advance(50.5f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(50.5f), std::make_pair(0, 3.0f));
}

void PlayerTest::advancePlayCount() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .setPlayCount(3)
        .play(2.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(3.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* 2 secs in, second round */
    player.advance(7.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(7.0f), std::make_pair(1, 2.0f));
    CORRADE_COMPARE(value, 5.0f);

    /* 1.75 secs in, third round */
    player.advance(9.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(9.75f), std::make_pair(2, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* When the player gets stopped, the value at the stop time is written */
    player.advance(11.5f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(11.5f), std::make_pair(2, 3.0f));
    CORRADE_COMPARE(value, 2.0f);

    /* But further advancing will not write anything */
    value = -1.0f;
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(100.0f), std::make_pair(2, 3.0f));
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::advancePlayCountInfinite() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .setPlayCount(0)
        .play(2.0f);

    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(3.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* 2 secs in, second round */
    player.advance(7.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(7.0f), std::make_pair(1, 2.0f));
    CORRADE_COMPARE(value, 5.0f);

    /* 1.75 secs in, 11th round */
    player.advance(33.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(33.75f), std::make_pair(10, 1.75f));
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::advanceChrono() {
    Float value = -1.0f;
    Player<std::chrono::nanoseconds, Float> player;
    player.add(Track, value)
        .play(std::chrono::seconds{2});

    CORRADE_COMPARE(player.duration().size(), 3.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(std::chrono::milliseconds{1750}),
        std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(std::chrono::milliseconds{1750});
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(std::chrono::milliseconds{1750}),
        std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(std::chrono::milliseconds{3750});
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(std::chrono::milliseconds{3750}),
        std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::advanceList() {
    Float valueA = -1.0f, valueB = -1.0f;
    Player<std::chrono::nanoseconds, Float> a, b;
    a.add(Track, valueA)
     .play(std::chrono::seconds{2});
    b.add(Track, valueB)
     .play(std::chrono::seconds{1});

    /* 1.75 secs in for A, 2.75 seconds in for B */
    Player<std::chrono::nanoseconds, Float>::advance(std::chrono::milliseconds{3750}, {a, b});
    CORRADE_COMPARE(a.state(), State::Playing);
    CORRADE_COMPARE(b.state(), State::Playing);
    CORRADE_COMPARE(a.elapsed(std::chrono::milliseconds{3750}),
        std::make_pair(0, 1.75f));
    CORRADE_COMPARE(b.elapsed(std::chrono::milliseconds{3750}),
        std::make_pair(0, 2.75f));
    CORRADE_COMPARE(valueA, 4.0f);
    CORRADE_COMPARE(valueB, 2.75f);
}

void PlayerTest::advanceZeroDurationStop() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        /* 1.75 secs since the start of the original duration */
        .setDuration(Range1D::fromSize(1.0f + 1.75f, 0.0f))
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 0.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* The value at 1.75 secs is returned independent of time, state is stopped */
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(100.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::advanceZeroDurationPause() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        /* 1.75 secs since the start of the original duration */
        .setDuration(Range1D::fromSize(1.0f + 1.75f, 0.0f))
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 0.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* The value at 1.75 secs is returned independent of time, state is paused
       (explicitly not stopped) */
    player.pause(100.0f);
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(100.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::advanceZeroDurationInfinitePlayCount() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        /* 1.75 secs since the start of the original duration */
        .setDuration(Range1D::fromSize(1.0f + 1.75f, 0.0f))
        .setPlayCount(0)
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 0.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(1.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(1.75f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* After that, the value at 1.75 secs is returned independent of time */
    player.advance(100.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(100.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::advanceZeroDurationInfinitePlayCountChrono() {
    Float value = -1.0f;
    Player<std::chrono::nanoseconds, Float> player;
    player.add(Track, value)
        /* 1.75 secs since the start of the original duration */
        .setDuration(Range1D::fromSize(1.0f + 1.75f, 0.0f))
        .setPlayCount(0)
        .play(std::chrono::seconds{2});

    CORRADE_COMPARE(player.duration().size(), 0.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(std::chrono::milliseconds{1750}),
        std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* Still before starting time, nothing is done */
    player.advance(std::chrono::milliseconds{1750});
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(std::chrono::milliseconds{1750}),
        std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    /* After that, the value at 1.75 seconds is returned independent of the
       time */
    player.advance(std::chrono::seconds{100});
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(std::chrono::seconds{100}),
        std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, 4.0f);
}

void PlayerTest::seekByStopped() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value);

    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(0.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    player.seekBy(1.5f);
    player.advance(1.75f);

    /* Nothing should change */
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(0.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::seekByPlaying() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(22.0f);

    /* 1.75 secs in */
    player.advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* Seek to 0.5 secs in, the value should not change after just a seek */
    value = -1.0f;
    player.seekBy(-1.25f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, -1.0f);

    /* Now it should be updated at 0.5 secs in */
    player.advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, 2.0f);
}

void PlayerTest::seekByPaused() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(22.0f);

    /* Pause at 1.75 secs in, no advance() yet so not parked yet */
    player.pause(23.75f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, -1.0f);

    /* Seek to 0.5 secs in, the value should not change after just a seek */
    player.seekBy(-1.25f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, -1.0f);

    /* Now it should be updated at 0.5 secs in */
    player.advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, 2.0f);

    /* Updating again should do nothing */
    value = -1.0f;
    player.advance(25.0f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(25.0f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::seekByPausedParked() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(22.0f);

    /* Pause at 1.75 secs in */
    player.pause(23.75f)
        .advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* Seek to 0.5 secs in, the value should not change after just a seek */
    value = -1.0f;
    player.seekBy(-1.25f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, -1.0f);

    /* Now it should be updated (re-parked) at 0.5 secs in */
    player.advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, 2.0f);

    /* Updating again should do nothing */
    value = -1.0f;
    player.advance(25.0f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(25.0f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::seekToStopped() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value);

    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(0.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);

    player.seekTo(1.75f, -0.5f);
    player.advance(1.75f);

    /* Nothing should change */
    CORRADE_COMPARE(player.state(), State::Stopped);
    CORRADE_COMPARE(player.elapsed(0.0f), std::make_pair(0, 0.0f));
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::seekToPlaying() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(22.0f);

    /* 1.75 secs in */
    player.advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* Seek to 0.5 secs in, the value should not change after just a seek */
    value = -1.0f;
    player.seekTo(23.75f, 0.5f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, -1.0f);

    /* Now it should be updated at 0.5 secs in */
    player.advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, 2.0f);
}

void PlayerTest::seekToPaused() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(22.0f);

    /* Pause at 1.75 secs in, no advance() yet so not parked yet */
    player.pause(23.75f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, -1.0f);

    /* Seek to 0.5 secs in, the value should not change after just a seek */
    player.seekTo(23.75f, 0.5f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, -1.0f);

    /* Now it should be updated at 0.5 secs in */
    player.advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, 2.0f);

    /* Updating again should do nothing */
    value = -1.0f;
    player.advance(25.0f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(25.0f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, -1.0f);
}

void PlayerTest::seekToPausedParked() {
    Float value = -1.0f;
    Player<Float> player;
    player.add(Track, value)
        .play(22.0f);

    /* Pause at 1.75 secs in */
    player.pause(23.75f)
        .advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 1.75f));
    CORRADE_COMPARE(value, 4.0f);

    /* Seek to 0.5 secs in, the value should not change after just a seek */
    value = -1.0f;
    player.seekTo(23.75f, 0.5f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, -1.0f);

    /* Now it should be updated (re-parked) at 0.5 secs in */
    player.advance(23.75f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(23.75f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, 2.0f);

    /* Updating again should do nothing */
    value = -1.0f;
    player.advance(25.0f);
    CORRADE_COMPARE(player.state(), State::Paused);
    CORRADE_COMPARE(player.elapsed(25.0f), std::make_pair(0, 0.5f));
    CORRADE_COMPARE(value, -1.0f);
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

/* So we don't need to duplicate the add*() tests by hand */
template<class T> struct AddTemplate;
template<> struct AddTemplate<Animation::Track<Float, Float>> {
    static const char* name() { return "Track<Float, Float>"; };
};
template<> struct AddTemplate<Animation::TrackView<Float, Float>> {
    static const char* name() { return "TrackView<Float, Float>"; };
};
template<> const Animation::Track<Float, Float>& PlayerTest::addTemplateTrack() {
    return Track;
}
template<> const Animation::TrackView<Float, Float>& PlayerTest::addTemplateTrack() {
    return mutableView;
}

template<class T> void PlayerTest::add() {
    setTestCaseTemplateName(AddTemplate<T>::name());

    Float value = -1.0f;
    Player<Float> player;
    player.add(addTemplateTrack<T>(), value)
        .play(2.0f);

    CORRADE_COMPARE(player.duration().size(), 3.0f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, -1.0f);

    /* 1.75 secs in */
    player.advance(3.75f);
    CORRADE_COMPARE(player.state(), State::Playing);
    CORRADE_COMPARE(value, 4.0f);
}

template<class T> void PlayerTest::addWithCallback() {
    setTestCaseTemplateName(AddTemplate<T>::name());

    struct Data {
        Float value = -1.0f;
        Int called = 0;
    } data;
    Player<Float> player;
    player.addWithCallback(addTemplateTrack<T>(), [](Float, const Float& value, void* userData) {
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

template<class T> void PlayerTest::addWithCallbackTemplate() {
    setTestCaseTemplateName(AddTemplate<T>::name());

    struct Data {
        Float value = -1.0f;
        Int called = 0;
    } data;
    Player<Float> player;
    player.addWithCallback(addTemplateTrack<T>(), [](Float, const Float& value, Data& userData) {
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

template<class T> void PlayerTest::addWithCallbackOnChange() {
    setTestCaseTemplateName(AddTemplate<T>::name());

    struct Data {
        Float value = -1.0f;
        Int called = 0;
    } data;
    Player<Float> player;
    player.addWithCallbackOnChange(addTemplateTrack<T>(), [](Float, const Float& value, void* userData) {
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

template<class T> void PlayerTest::addWithCallbackOnChangeTemplate() {
    setTestCaseTemplateName(AddTemplate<T>::name());

    struct Data {
        Float value = -1.0f;
        Int called = 0;
    } data;
    Player<Float> player;
    player.addWithCallbackOnChange(addTemplateTrack<T>(), [](Float, const Float& value, Data& userData) {
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

/* Can't use raw lambdas because MSVC 2015 */
void callback(Containers::Array<Int>& data, Int value) {
    arrayAppend(data, value);
}

template<class T> void PlayerTest::addRawCallback() {
    setTestCaseTemplateName(AddTemplate<T>::name());

    T track;

    Int result = -1;
    Containers::Array<Int> data;

    Animation::Player<Float> player;
    player.addRawCallback(track,
        [](const Animation::TrackViewStorage<const Float>& track, Float key,
        std::size_t& hint, void* destination, void(*callback)(), void* userData) {
            Int value = static_cast<const Animation::TrackView<const Float, const Int>&>(track).at(key, hint);
            if(value == *static_cast<Int*>(destination)) return;
            *static_cast<Int*>(destination) = value;
            reinterpret_cast<void(*)(Containers::Array<Int>&, Int)>(callback)(*static_cast<Containers::Array<Int>*>(userData), value);
        }, &result, reinterpret_cast<void(*)()>(callback), &data)
        .play({});

    /* Should add the default-constructed value into the vector, but only once */
    CORRADE_COMPARE_AS(data, Containers::arrayView<Int>({}),
        TestSuite::Compare::Container);
    player.advance({});
    CORRADE_COMPARE_AS(data, Containers::arrayView<Int>({0}),
        TestSuite::Compare::Container);
    player.advance(1.0f);
    CORRADE_COMPARE_AS(data, Containers::arrayView<Int>({0}),
        TestSuite::Compare::Container);
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
    CORRADE_COMPARE(player.duration().size(), 3.0f);

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
    CORRADE_COMPARE(player.duration().size(), 3.0f);

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

}}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::PlayerTest)
