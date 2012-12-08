#ifndef Magnum_SceneGraph_Animable_hpp
#define Magnum_SceneGraph_Animable_hpp
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief @ref compilation-speedup-hpp "Template implementation" for Animable.h and AnimableGroup.h
 */

#include "AnimableGroup.h"
#include "Animable.h"

#include "Timeline.h"

namespace Magnum { namespace SceneGraph {

template<std::uint8_t dimensions, class T> Animable<dimensions, T>::Animable(AbstractObject<dimensions, T>* object, GLfloat duration, AnimableGroup<dimensions, T>* group): AbstractGroupedFeature<dimensions, Animable<dimensions, T>, T>(object, group), _duration(duration), startTime(std::numeric_limits<GLfloat>::infinity()), pauseTime(-std::numeric_limits<GLfloat>::infinity()), previousState(AnimationState::Stopped), currentState(AnimationState::Stopped), _repeated(false), _repeatCount(0), repeats(0) {}

template<std::uint8_t dimensions, class T> Animable<dimensions, T>* Animable<dimensions, T>::setState(AnimationState state) {
    if(currentState == state) return this;

    /* Not allowed (for sanity) */
    if(previousState == AnimationState::Stopped && state == AnimationState::Paused)
        return this;

    /* Wake up the group in case no animations are running */
    group()->wakeUp = true;
    currentState = state;
    return this;
}

template<std::uint8_t dimensions, class T> AnimableGroup<dimensions, T>* Animable<dimensions, T>::group() {
    return static_cast<AnimableGroup<dimensions, T>*>(AbstractGroupedFeature<dimensions, Animable<dimensions, T>, T>::group());
}

template<std::uint8_t dimensions, class T> const AnimableGroup<dimensions, T>* Animable<dimensions, T>::group() const {
    return static_cast<const AnimableGroup<dimensions, T>*>(AbstractGroupedFeature<dimensions, Animable<dimensions, T>, T>::group());
}

template<std::uint8_t dimensions, class T> void AnimableGroup<dimensions, T>::step(const GLfloat time, const GLfloat delta) {
    if(!_runningCount && !wakeUp) return;
    wakeUp = false;

    for(std::size_t i = 0; i != this->size(); ++i) {
        Animable<dimensions, T>* animable = (*this)[i];

        /* The animation was stopped recently, just decrease count of running
           animations if the animation was running before */
        if(animable->previousState != AnimationState::Stopped && animable->currentState == AnimationState::Stopped) {
            if(animable->previousState == AnimationState::Running)
                --_runningCount;
            animable->previousState = AnimationState::Stopped;
            animable->animationStopped();
            continue;

        /* The animation was paused recently, set pause time to previous frame time */
        } else if(animable->previousState == AnimationState::Running && animable->currentState == AnimationState::Paused) {
            animable->previousState = AnimationState::Paused;
            animable->pauseTime = time;
            --_runningCount;
            animable->animationPaused();
            continue;

        /* Skip the rest for not running animations */
        } else if(animable->currentState != AnimationState::Running) {
            CORRADE_INTERNAL_ASSERT(animable->previousState == animable->currentState);
            continue;

        /* The animation was started recently, set start time to previous frame time */
        } else if(animable->previousState == AnimationState::Stopped) {
            animable->previousState = AnimationState::Running;
            animable->startTime = time;
            ++_runningCount;
            animable->animationStarted();

        /* The animation was resumed recently, add pause duration to start time */
        } else if(animable->previousState == AnimationState::Paused) {
            animable->previousState = AnimationState::Running;
            animable->startTime += time - animable->pauseTime;
            ++_runningCount;
            animable->animationResumed();
        }

        CORRADE_INTERNAL_ASSERT(animable->previousState == AnimationState::Running);

        /* Animation time exceeded duration */
        if(animable->_duration != 0.0f && time-animable->startTime > animable->_duration) {
            /* Not repeated or repeat count exceeded, stop */
            if(!animable->_repeated || animable->repeats+1 == animable->_repeatCount) {
                animable->previousState = AnimationState::Stopped;
                animable->currentState = AnimationState::Stopped;
                --_runningCount;
                continue;
            }

            /* Increase repeat count and add duration to startTime */
            ++animable->repeats;
            animable->startTime += animable->_duration;
        }

        /* Animation is still running, perform animation step */
        CORRADE_ASSERT(time-animable->startTime >= 0.0f,
            "SceneGraph::AnimableGroup::step(): animation was started in future - probably wrong time passed", );
        CORRADE_ASSERT(delta >= 0.0f,
            "SceneGraph::AnimableGroup::step(): negative delta passed", );
        animable->animationStep(time - animable->startTime, delta);
    }

    CORRADE_INTERNAL_ASSERT(_runningCount <= this->size());
}

}}

#endif
