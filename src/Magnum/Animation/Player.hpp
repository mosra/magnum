#ifndef Magnum_Animation_Player_hpp
#define Magnum_Animation_Player_hpp
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

/** @file
 * @brief @ref compilation-speedup-hpp "Template implementation" for @ref Animable.h and @ref AnimableGroup.h
 */

#include "Player.h"

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Reference.h>

namespace Magnum { namespace Animation {

namespace Implementation {
    template<class T, class K> struct DefaultScaler {
        static std::pair<UnsignedInt, K> scale(T time, K duration) {
            const UnsignedInt playCount = time/duration;
            const K factor = std::fmod(Double(time), Double(duration));
            return {playCount, factor};
        }
    };

    template<> struct DefaultScaler<std::chrono::nanoseconds, Float> {
        static std::pair<UnsignedInt, Float> scale(std::chrono::nanoseconds time, Float duration) {
            /* I hope I'm not having any catastrophic cancellation here :/ */
            const std::chrono::nanoseconds durationNs{std::chrono::nanoseconds::rep(Double(duration)*1000000000.0)};
            const UnsignedInt playCount = time/durationNs;
            const Float factor = Float((time - playCount*durationNs).count()/1000000000.0);
            return {playCount, factor};
        }
    };
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T, class K> struct Player<T, K>::Track  {
    /* Not sure why is this still needed for emplace_back(). It's 2018,
       COME ON  ¯\_(ツ)_/¯ */
    /*implicit*/ Track(const TrackViewStorage<const K>& track, void (*advancer)(const TrackViewStorage<const K>&, K, std::size_t&, void*, void(*)(), void*), void* destination, void(*userCallback)(), void* userCallbackData, std::size_t hint) noexcept: track{track}, advancer{advancer}, destination{destination}, userCallback{userCallback}, userCallbackData{userCallbackData}, hint{hint} {}

    TrackViewStorage<const K> track;
    void (*advancer)(const TrackViewStorage<const K>&, K, std::size_t&, void*, void(*)(), void*);
    void* destination;
    void(*userCallback)();
    void* userCallbackData;
    std::size_t hint;
};
#endif

template<class T, class K> void Player<T, K>::advance(const T time, const std::initializer_list<Containers::Reference<Player<T, K>>> players) {
    for(Player<T, K>& p: players) p.advance(time);
}

template<class T, class K> Player<T, K>::Player(Player<T, K>&&) noexcept = default;

template<class T, class K> Player<T, K>& Player<T, K>::operator=(Player<T, K>&&) noexcept = default;

template<class T, class K> Player<T, K>::Player(): Player<T, K>{Implementation::DefaultScaler<T, K>::scale} {}

template<class T, class K> Player<T, K>::Player(Scaler scaler): _scaler{scaler} {}

template<class T, class K> Player<T, K>::~Player() = default;

template<class T, class K> bool Player<T, K>::isEmpty() const {
    return _tracks.empty();
}

template<class T, class K> std::size_t Player<T, K>::size() const {
    return _tracks.size();
}

template<class T, class K> const TrackViewStorage<const K>& Player<T, K>::track(std::size_t i) const {
    CORRADE_ASSERT(i < _tracks.size(),
        /* Returning track 0 so we can test this w/ MSVC debug iterators */
        "Animation::Player::track(): index out of range", _tracks[0].track);
    return _tracks[i].track;
}

template<class T, class K> Player<T, K>& Player<T, K>::addInternal(const TrackViewStorage<const K>& track, void(*const advancer)(const TrackViewStorage<const K>&, K, std::size_t&, void*, void(*)(), void*), void* const destination, void(*const userCallback)(), void* const userCallbackData) {
    if(_tracks.empty() && _duration == Math::Range1D<K>{})
        _duration = track.duration();
    else
        _duration = Math::join(track.duration(), _duration);
    arrayAppend(_tracks, Containers::InPlaceInit, track, advancer, destination, userCallback, userCallbackData, 0u);
    return *this;
}

template<class T, class K> Player<T, K>& Player<T, K>::play(T startTime) {
    /* In case we were paused, move start time backwards by the duration that
       was already played back */
    if(_state == State::Paused) {
        _startTime = startTime - _startTime;
        _state = State::Playing;
        return *this;
    }

    _state = State::Playing;
    _startTime = startTime;
    return *this;
}

template<class T, class K> Player<T, K>& Player<T, K>::resume(T startTime) {
    if(_state == State::Playing) return *this;
    return play(startTime);
}

template<class T, class K> Player<T, K>& Player<T, K>::pause(T pauseTime) {
    /* Avoid breaking the pause state when not playing */
    if(_state != State::Playing) return *this;

    _state = State::Paused;
    _stopPauseTime = pauseTime;
    return *this;
}

template<class T, class K> Player<T, K>& Player<T, K>::seekBy(T timeDelta) {
    /* Animation is stopped, nothing to do */
    if(_state == State::Stopped) return *this;

    /* If the animation is paused and parked already, trigger a "park" again in
       order to have the values updated on the next call to advance(). The
       value is simply the new elapsed animation time. */
    if(_state == State::Paused && _stopPauseTime == T{}) {
        _stopPauseTime = _startTime + timeDelta;
        _startTime = {};
        return *this;
    }

    /* Otherwise, the animation is either playing or not yet parked, simply
       patch the start time to make the seek */
    _startTime -= timeDelta;
    return *this;
}

template<class T, class K> Player<T, K>& Player<T, K>::seekTo(T seekTime, T animationTime) {
    /* Animation is stopped, nothing to do */
    if(_state == State::Stopped) return *this;

    /* If the animation is paused and parked already, trigger a "park" again in
       order to have the values updated on the next call to advance(). The
       value is simply the new elapsed animation time. */
    if(_state == State::Paused && _stopPauseTime == T{}) {
        _stopPauseTime = animationTime;
        _startTime = {};
        return *this;
    }

    /* Otherwise, the animation is either playing or not yet parked, simply
       patch the start time to make the seek */
    _startTime = seekTime - animationTime;
    return *this;
}

template<class T, class K> Player<T, K>& Player<T, K>::stop() {
    _state = State::Stopped;
    /* Anything, just not a default-constructed value */
    /** @todo might be problematic for some types */
    _stopPauseTime = T{1};
    return *this;
}

template<class T, class K> Player<T, K>& Player<T, K>::setState(State state, T time) {
    switch(state) {
        case State::Playing: return play(time);
        case State::Paused: return pause(time);
        case State::Stopped: return stop();
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

namespace Implementation {

template<class T, class K> Containers::Optional<std::pair<UnsignedInt, K>> playerElapsed(const K duration, const UnsignedInt playCount, const typename Player<T, K>::Scaler scaler, const T time, T& startTime, T& stopPauseTime, State& state) {
    /* Time to use for advancing the animation */
    T timeToUse = time - startTime;

    /* The animation was paused right before this iteration, "park" the
       animation to the pause time. This time will be used by play() to offset
       the playback when the animation is resumed.

       std::chrono::duration doesn't have operator bool, so I need to compare
       to default-constructed value. Ugh. */
    if(state == State::Paused && stopPauseTime != T{}) {
        startTime = stopPauseTime - startTime;
        timeToUse = startTime;
        stopPauseTime = {};

    /* The animation was stopped by the user right before this iteration,
       "park" the animation to the initial time */
    } else if(state == State::Stopped && stopPauseTime != T{}) {
        timeToUse = {};
        startTime = {};
        stopPauseTime = {};

    /* Otherwise, if the player is not playing or scheduled to start playing in
       the future, do nothing */
    } else if(state != State::Playing || time < startTime)
        return Containers::NullOpt;

    /* If the player duration is empty, we can't call the scaler. If play count
       is infinite, infinitely advance to a key at duration start. If not, stop
       the animation. */
    UnsignedInt playIteration;
    K key;
    if(duration == K{}) {
        key = K{};
        playIteration = 0;
        if(playCount != 0) {
            if(state != State::Paused) state = State::Stopped;
            startTime = {};
        }

    /* Otherwise calculate current play iteration and key value in that
       iteration. If we exceeded play count, stop the animation and give out
       value at duration end. */
    } else {
        const std::pair<UnsignedInt, K> scaled = scaler(timeToUse, duration);
        playIteration = scaled.first;
        key = scaled.second;
        if(playCount && playIteration >= playCount) {
            if(state != State::Paused) state = State::Stopped;
            /* Don't reset the startTime to disambiguate between explicitly
               stopped and "time run out" animation */
            playIteration = playCount - 1;
            key = duration;
        }
    }

    return {Containers::InPlaceInit, playIteration, key};
}

}

template<class T, class K> std::pair<UnsignedInt, K> Player<T, K>::elapsed(const T time) const {
    const K duration = _duration.size();

    /* Get the elapsed time. This is an immutable query, so make copies of the
       (otherwise to be modified) internal state. */
    {
        T startTime = _startTime;
        T pauseTime = _stopPauseTime;
        State state = _state;
        const Containers::Optional<std::pair<UnsignedInt, K>> elapsed = Implementation::playerElapsed(duration, _playCount, _scaler, time, startTime, pauseTime, state);
        if(elapsed) return *elapsed;
    }

    /* If not advancing, the animation can be paused -- calculate the iteration
       index and keyframe at which it was paused if the duration is nonzero. If
       the paused animation ran out, return the last iteration index and the
       duration, otherwise just the calculated value. */
    if(_state == State::Paused && duration) {
        const std::pair<UnsignedInt, K> elapsed = _scaler(_startTime, duration);
        if(_playCount && elapsed.first >= _playCount)
            return {_playCount - 1, duration};
        return elapsed;
    }

    /* It can be also stopped by running out, in that case return the last
       iteration index and the duration. Again have to use comparison to
       default-constructed value because std::chrono::nanoseconds doesn't have
       operator bool. */
    if(_state == State::Stopped && _startTime != T{}) {
        CORRADE_INTERNAL_ASSERT(_playCount);
        return {_playCount - 1, duration};
    }

    /* Otherwise (zero duration, explicitly stopped, not yet started) return
       zero */
    return {0, K{}};
}

template<class T, class K> Player<T, K>& Player<T, K>::advance(const T time) {
    /* Get the elapsed time. If we shouldn't advance anything (player already
       stopped / not yet playing, quit */
    Containers::Optional<std::pair<UnsignedInt, K>> elapsed = Implementation::playerElapsed(_duration.size(), _playCount, _scaler, time, _startTime, _stopPauseTime, _state);
    if(!elapsed) return *this;

    /* Advance all tracks. Properly handle durations that don't start at 0. */
    for(Track& t: _tracks)
        t.advancer(t.track, _duration.min() + elapsed->second, t.hint, t.destination, t.userCallback, t.userCallbackData);

    return *this;
}

}}

#endif
