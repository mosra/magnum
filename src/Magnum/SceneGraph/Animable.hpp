#ifndef Magnum_SceneGraph_Animable_hpp
#define Magnum_SceneGraph_Animable_hpp
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

#include "Magnum/Timeline.h"
#include "Magnum/Math/Constants.h"
#include "Magnum/SceneGraph/AnimableGroup.h"
#include "Magnum/SceneGraph/Animable.h"

namespace Magnum { namespace SceneGraph {

template<UnsignedInt dimensions, class T> Animable<dimensions, T>::Animable(AbstractObject<dimensions, T>& object, AnimableGroup<dimensions, T>* group): AbstractGroupedFeature<dimensions, Animable<dimensions, T>, T>{object, group}, _duration{0.0f}, _startTime{Constants::inf()}, _pauseTime{-Constants::inf()}, _previousState{AnimationState::Stopped}, _currentState{AnimationState::Stopped}, _repeated{false}, _repeatCount{0}, _repeats{0} {}

template<UnsignedInt dimensions, class T> Animable<dimensions, T>::~Animable() {
    /* Update count of running animations when deleting an animable that's
       currently running */
    if(animables() && _currentState == AnimationState::Running)
        --animables()->_runningCount;
}

template<UnsignedInt dimensions, class T> Animable<dimensions, T>& Animable<dimensions, T>::setState(AnimationState state) {
    if(_currentState == state) return *this;

    /* Not allowed (for sanity) */
    if(_previousState == AnimationState::Stopped && state == AnimationState::Paused)
        return *this;

    /* Wake up the group in case no animations are running */
    animables()->wakeUp = true;
    _currentState = state;
    return *this;
}

template<UnsignedInt dimensions, class T> AnimableGroup<dimensions, T>* Animable<dimensions, T>::animables() {
    return static_cast<AnimableGroup<dimensions, T>*>(AbstractGroupedFeature<dimensions, Animable<dimensions, T>, T>::group());
}

template<UnsignedInt dimensions, class T> const AnimableGroup<dimensions, T>* Animable<dimensions, T>::animables() const {
    return static_cast<const AnimableGroup<dimensions, T>*>(AbstractGroupedFeature<dimensions, Animable<dimensions, T>, T>::group());
}

template<UnsignedInt dimensions, class T> void AnimableGroup<dimensions, T>::step(const Float time, const Float delta) {
    if(!_runningCount && !wakeUp) return;
    wakeUp = false;

    for(std::size_t i = 0; i != AnimableGroup<dimensions, T>::size(); ++i) {
        Animable<dimensions, T>& animable = (*this)[i];

        /* The animation was stopped recently, just decrease count of running
           animations if the animation was running before */
        if(animable._previousState != AnimationState::Stopped && animable._currentState == AnimationState::Stopped) {
            if(animable._previousState == AnimationState::Running)
                --_runningCount;
            animable._previousState = AnimationState::Stopped;
            animable.animationStopped();
            continue;

        /* The animation was paused recently, set pause time to previous frame time */
        } else if(animable._previousState == AnimationState::Running && animable._currentState == AnimationState::Paused) {
            animable._previousState = AnimationState::Paused;
            animable._pauseTime = time;
            --_runningCount;
            animable.animationPaused();
            continue;

        /* Skip the rest for not running animations */
        } else if(animable._currentState != AnimationState::Running) {
            CORRADE_INTERNAL_ASSERT(animable._previousState == animable._currentState);
            continue;

        /* The animation was started recently, set start time to previous frame
           time, reset repeat count */
        } else if(animable._previousState == AnimationState::Stopped) {
            animable._previousState = AnimationState::Running;
            animable._startTime = time;
            animable._repeats = 0;
            ++_runningCount;
            animable.animationStarted();

        /* The animation was resumed recently, add pause duration to start time */
        } else if(animable._previousState == AnimationState::Paused) {
            animable._previousState = AnimationState::Running;
            animable._startTime += time - animable._pauseTime;
            ++_runningCount;
            animable.animationResumed();
        }

        CORRADE_INTERNAL_ASSERT(animable._previousState == AnimationState::Running);

        /* Animation time exceeded duration */
        if(animable._duration != 0.0f && time-animable._startTime > animable._duration) {
            /* Not repeated or repeat count exceeded, stop */
            if(!animable._repeated || animable._repeats + 1 == animable._repeatCount) {
                animable._previousState = AnimationState::Stopped;
                animable._currentState = AnimationState::Stopped;
                --_runningCount;
                animable.animationStopped();
                continue;
            }

            /* Increase repeat count and add duration to startTime */
            ++animable._repeats;
            animable._startTime += animable._duration;
        }

        /* Animation is still running, perform animation step */
        CORRADE_ASSERT(time - animable._startTime >= 0.0f,
            "SceneGraph::AnimableGroup::step(): animation was started in future - probably wrong time passed", );
        CORRADE_ASSERT(delta >= 0.0f,
            "SceneGraph::AnimableGroup::step(): negative delta passed", );
        animable.animationStep(time - animable._startTime, delta);
    }

    CORRADE_INTERNAL_ASSERT((_runningCount <= AnimableGroup<dimensions, T>::size()));
}

}}

#endif
