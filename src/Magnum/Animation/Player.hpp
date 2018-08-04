#ifndef Magnum_Animation_Player_hpp
#define Magnum_Animation_Player_hpp
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

/** @file
 * @brief @ref compilation-speedup-hpp "Template implementation" for @ref Animable.h and @ref AnimableGroup.h
 */

#include "Player.h"

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
    /*implicit*/ Track(const TrackViewStorage<K>& track, void (*advancer)(const TrackViewStorage<K>&, K, std::size_t&, void*, void(*)(), void*), void* destination, void(*userCallback)(), void* userCallbackData, std::size_t hint) noexcept: track{track}, advancer{advancer}, destination{destination}, userCallback{userCallback}, userCallbackData{userCallbackData}, hint{hint} {}

    TrackViewStorage<K> track;
    void (*advancer)(const TrackViewStorage<K>&, K, std::size_t&, void*, void(*)(), void*);
    void* destination;
    void(*userCallback)();
    void* userCallbackData;
    std::size_t hint;
};
#endif

template<class T, class K> Player<T, K>::Player(Player<T, K>&&) = default;

template<class T, class K> Player<T, K>& Player<T, K>::operator=(Player<T, K>&&) = default;

template<class T, class K> Player<T, K>::Player(): Player<T, K>{Implementation::DefaultScaler<T, K>::scale} {}

template<class T, class K> Player<T, K>::Player(Scaler scaler): _scaler{scaler} {}

template<class T, class K> Player<T, K>::~Player() = default;

template<class T, class K> bool Player<T, K>::isEmpty() const {
    return _tracks.empty();
}

template<class T, class K> std::size_t Player<T, K>::size() const {
    return _tracks.size();
}

template<class T, class K> const TrackViewStorage<K>& Player<T, K>::track(std::size_t i) const {
    CORRADE_ASSERT(i < _tracks.size(),
        "Animation::Player::track(): index out of range", _tracks[i].track);
    return _tracks[i].track;
}

template<class T, class K> Player<T, K>& Player<T, K>::addInternal(const TrackViewStorage<K>& track, void(*const advancer)(const TrackViewStorage<K>&, K, std::size_t&, void*, void(*)(), void*), void* const destination, void(*const userCallback)(), void* const userCallbackData) {
    if(_tracks.empty() && _duration == Math::Range1D<K>{})
        _duration = track.duration();
    else
        _duration = Math::join(track.duration(), _duration);
    _tracks.emplace_back(track, advancer, destination, userCallback, userCallbackData, 0);
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

template<class T, class K> Player<T, K>& Player<T, K>::pause(T pauseTime) {
    /* Avoid breaking the pause state when not playing */
    if(_state != State::Playing) return *this;

    _state = State::Paused;
    _pauseTime = pauseTime;
    return *this;
}

template<class T, class K> Player<T, K>& Player<T, K>::stop() {
    _state = State::Stopped;
    return *this;
}

template<class T, class K> Player<T, K>& Player<T, K>::setState(State state, T time) {
    switch(state) {
        case State::Playing: return play(time);
        case State::Paused: return pause(time);
        case State::Stopped: return stop();
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

template<class T, class K> Player<T, K>& Player<T, K>::advance(T time) {
    /* Time to use for advancing the animation */
    T timeToUse = time;

    /* The animation was paused right before this iteration, "park" the
       animation to the pause time. This time will be used by play() to offset
       the playback when the animation is resumed.

       std::chrono::duration doesn't have operator bool, so I need to compare
       to default-constructed value. Ugh. */
    if(_state == State::Paused && (_pauseTime != T{})) {
        timeToUse = _pauseTime;
        _startTime = _pauseTime - _startTime;
        _pauseTime = {};

    /* The animation was stopped by the user right before this iteration,
       "park" the animation to the initial time */
    } else if(_state == State::Stopped && (_startTime != T{})) {
        _startTime = {};
        timeToUse = {};

    /* Otherwise, if the player is not playing or scheduled to start playing in
       the future, do nothing */
    } else if(_state != State::Playing || time < _startTime) return *this;

    /* If the player duration is empty, we can't call the scaler. If play count
       is infinite, infinitely advance to a key at duration start. If not, stop
       the animation. */
    K key;
    const K duration = _duration.size()[0];
    if(duration == K{}) {
        key = K{};
        if(_playCount != 0) {
            _state = State::Stopped;
            _startTime = {};
        }

    /* Otherwise calculate current play iteration and key value in that
       iteration. If we exceeded play count, stop the animation and give out
       value at duration end. */
    } else {
        UnsignedInt playCount;
        std::tie(playCount, key) = _scaler(timeToUse - _startTime, duration);
        if(_playCount && playCount >= _playCount) {
            _state = State::Stopped;
            _startTime = {};
            key = duration;
        }
    }

    /* Advance all tracks. Properly handle durations that don't start at 0. */
    for(Track& t: _tracks)
        t.advancer(t.track, _duration.min()[0] + key, t.hint, t.destination, t.userCallback, t.userCallbackData);

    return *this;
}

}}

#endif
