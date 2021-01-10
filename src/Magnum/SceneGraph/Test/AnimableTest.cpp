/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/SceneGraph/AbstractFeature.hpp"
#include "Magnum/SceneGraph/Animable.hpp"
#include "Magnum/SceneGraph/AnimableGroup.h"
#include "Magnum/SceneGraph/FeatureGroup.hpp"
#include "Magnum/SceneGraph/MatrixTransformation3D.hpp"
#include "Magnum/SceneGraph/Object.hpp"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct AnimableTest: TestSuite::Tester {
    explicit AnimableTest();

    template<class T> void state();
    template<class T> void step();
    template<class T> void duration();
    template<class T> void repeat();
    template<class T> void stop();
    template<class T> void pause();

    void deleteWhileRunning();

    void debug();
};

AnimableTest::AnimableTest() {
    addTests({&AnimableTest::state<Float>,
              &AnimableTest::state<Double>,
              &AnimableTest::step<Float>,
              &AnimableTest::step<Double>,
              &AnimableTest::duration<Float>,
              &AnimableTest::duration<Double>,
              &AnimableTest::repeat<Float>,
              &AnimableTest::repeat<Double>,
              &AnimableTest::stop<Float>,
              &AnimableTest::stop<Double>,
              &AnimableTest::pause<Float>,
              &AnimableTest::pause<Double>,

              &AnimableTest::deleteWhileRunning,

              &AnimableTest::debug});
}

template<class T> using Object3D = SceneGraph::Object<SceneGraph::BasicMatrixTransformation3D<T>>;

template<class T> void AnimableTest::state() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    class StateTrackingAnimable: public SceneGraph::BasicAnimable3D<T> {
        public:
            StateTrackingAnimable(AbstractBasicObject3D<T>& object, BasicAnimableGroup3D<T>* group = nullptr): SceneGraph::BasicAnimable3D<T>{object, group} {
                this->setDuration(1.0f);
            }

            std::string trackedState;

        protected:
            void animationStep(Float, Float) override {}

            void animationStarted() override { trackedState += "started"; }
            void animationPaused() override { trackedState += "paused"; }
            void animationResumed() override { trackedState += "resumed"; }
            void animationStopped() override { trackedState += "stopped"; }
    };

    Object3D<T> object;
    BasicAnimableGroup3D<T> group;
    CORRADE_COMPARE(group.runningCount(), 0);

    /* Verify initial state */
    StateTrackingAnimable animable(object, &group);
    CORRADE_COMPARE(animable.state(), AnimationState::Stopped);
    CORRADE_VERIFY(animable.trackedState.empty());
    group.step(1.0f, 1.0f);
    CORRADE_VERIFY(animable.trackedState.empty());
    CORRADE_COMPARE(group.runningCount(), 0);

    /* Stopped -> paused is not supported */
    CORRADE_COMPARE(animable.state(), AnimationState::Stopped);
    animable.setState(AnimationState::Paused);
    CORRADE_COMPARE(animable.state(), AnimationState::Stopped);

    /* Stopped -> running */
    CORRADE_COMPARE(animable.state(), AnimationState::Stopped);
    animable.trackedState.clear();
    animable.setState(AnimationState::Running);
    CORRADE_VERIFY(animable.trackedState.empty());
    group.step(1.0f, 1.0f);
    CORRADE_COMPARE(animable.trackedState, "started");
    CORRADE_COMPARE(group.runningCount(), 1);

    /* Running -> paused */
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    animable.trackedState.clear();
    animable.setState(AnimationState::Paused);
    CORRADE_VERIFY(animable.trackedState.empty());
    group.step(1.0f, 1.0f);
    CORRADE_COMPARE(animable.trackedState, "paused");
    CORRADE_COMPARE(group.runningCount(), 0);

    /* Paused -> running */
    CORRADE_COMPARE(animable.state(), AnimationState::Paused);
    animable.trackedState.clear();
    animable.setState(AnimationState::Running);
    CORRADE_VERIFY(animable.trackedState.empty());
    group.step(1.0f, 1.0f);
    CORRADE_COMPARE(animable.trackedState, "resumed");
    CORRADE_COMPARE(group.runningCount(), 1);

    /* Running -> stopped */
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    animable.trackedState.clear();
    animable.setState(AnimationState::Stopped);
    CORRADE_VERIFY(animable.trackedState.empty());
    group.step(1.0f, 1.0f);
    CORRADE_COMPARE(animable.trackedState, "stopped");
    CORRADE_COMPARE(group.runningCount(), 0);

    animable.setState(AnimationState::Running);
    group.step(1.0f, 1.0f);
    animable.setState(AnimationState::Paused);

    /* Paused -> stopped */
    CORRADE_COMPARE(animable.state(), AnimationState::Paused);
    animable.trackedState.clear();
    animable.setState(AnimationState::Stopped);
    CORRADE_VERIFY(animable.trackedState.empty());
    group.step(1.0f, 1.0f);
    CORRADE_COMPARE(animable.trackedState, "stopped");
    CORRADE_COMPARE(group.runningCount(), 0);

    /* Verify running count can go past 0/1 */
    auto a = new StateTrackingAnimable(object, &group);
    auto b = new StateTrackingAnimable(object, &group);
    a->setState(AnimationState::Running);
    b->setState(AnimationState::Running);
    group.add(*a).add(*b);
    group.step(1.0f, 1.0f);
    CORRADE_COMPARE(group.runningCount(), 2);
}

template<class T> class OneShotAnimable: public SceneGraph::BasicAnimable3D<T> {
    public:
        explicit OneShotAnimable(AbstractBasicObject3D<T>& object, BasicAnimableGroup3D<T>* group = nullptr): SceneGraph::BasicAnimable3D<T>{object, group}, time{-1.0f} {
            this->setDuration(10.0f);
            this->setState(AnimationState::Running);
        }

        Float time;
        std::string stateChanges;

    protected:
        void animationStep(Float t, Float) override {
            time = t;
        }

        void animationStarted() override {
            stateChanges += "started;";
        }

        void animationStopped() override {
            stateChanges += "stopped;";
        }
};

template<class T> void AnimableTest::step() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    class InifiniteAnimable: public SceneGraph::BasicAnimable3D<T> {
        public:
            InifiniteAnimable(AbstractBasicObject3D<T>& object, BasicAnimableGroup3D<T>* group = nullptr): SceneGraph::BasicAnimable3D<T>{object, group}, time{-1.0f}, delta{0.0f} {}

            Float time, delta;

        protected:
            void animationStep(Float t, Float d) override {
                time = t;
                delta = d;
            }
    };

    Object3D<T> object;
    BasicAnimableGroup3D<T> group;
    InifiniteAnimable animable{object, &group};

    /* Calling step() if no object is running should do nothing */
    group.step(5.0f, 0.5f);
    CORRADE_COMPARE(group.runningCount(), 0);
    CORRADE_COMPARE(animable.time, -1.0f);
    CORRADE_COMPARE(animable.delta, 0.0f);

    /* Calling step() with running animation should start it with zero
       absolute time */
    animable.setState(AnimationState::Running);
    group.step(5.0f, 0.5f);
    CORRADE_COMPARE(group.runningCount(), 1);
    CORRADE_COMPARE(animable.time, 0.0f);
    CORRADE_COMPARE(animable.delta, 0.5f);

    /* Repeated call to step() will add to absolute animation time */
    group.step(8.0f, 0.75f);
    CORRADE_COMPARE(animable.time, 3.0f);
    CORRADE_COMPARE(animable.delta, 0.75f);
}

template<class T> void AnimableTest::duration() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> object;
    BasicAnimableGroup3D<T> group;
    OneShotAnimable<T> animable{object, &group};
    CORRADE_VERIFY(!animable.isRepeated());

    /* First animation step is in duration, verify that animation is still
       running and animationStep() is called */
    group.step(1.0f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    CORRADE_COMPARE(animable.stateChanges, "started;");
    CORRADE_COMPARE(animable.time, 0.0f);

    /* Next animation step is out of duration and repeat is not enabled,
       animationStep() shouldn't be called and animation should be stopped */
    group.step(12.75f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Stopped);
    CORRADE_COMPARE(animable.stateChanges, "started;stopped;");
    CORRADE_COMPARE(animable.time, 0.0f);
}

template<class T> void AnimableTest::repeat() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    class RepeatingAnimable: public SceneGraph::BasicAnimable3D<T> {
        public:
            RepeatingAnimable(AbstractBasicObject3D<T>& object, BasicAnimableGroup3D<T>* group = nullptr): SceneGraph::BasicAnimable3D<T>{object, group}, time(-1.0f) {
                this->setDuration(10.0f);
                this->setState(AnimationState::Running);
                this->setRepeated(true);
            }

            Float time;

        protected:
            void animationStep(Float t, Float) override {
                time = t;
            }
    };

    Object3D<T> object;
    BasicAnimableGroup3D<T> group;
    RepeatingAnimable animable{object, &group};
    CORRADE_COMPARE(animable.repeatCount(), 0);

    /* First animation steps is in first loop iteration */
    group.step(1.0f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    CORRADE_COMPARE(animable.time, 0.0f);

    /* Next animation step is in second loop iteration, animation should be
       still running with time shifted by animation duration */
    group.step(11.5f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    CORRADE_COMPARE(animable.time, 0.5f);

    /* Third loop iteration (just to be sure) */
    group.step(25.5f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    CORRADE_COMPARE(animable.time, 4.5f);

    /* Cap repeat count to 3, the animation should be stopped now (and
       animationStep() shouldn't be called)*/
    animable.setRepeatCount(3);
    group.step(33.0f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Stopped);
    CORRADE_COMPARE(animable.time, 4.5f);

    /* Starting the animation again, should be repeatable again */
    animable.setState(AnimationState::Running);

    /* Three animation repeats */
    group.step(1.0f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    group.step(11.5f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    group.step(25.5f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);

    /* Should be stopped now */
    group.step(33.0f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Stopped);
}

template<class T> void AnimableTest::stop() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> object;
    BasicAnimableGroup3D<T> group;
    OneShotAnimable<T> animable(object, &group);
    CORRADE_COMPARE(animable.repeatCount(), 0);

    /* Eat up some absolute time */
    group.step(1.0f, 0.5f);
    group.step(1.5f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    CORRADE_COMPARE(animable.time, 0.5f);

    /* Stop the animable, nothing should be done */
    animable.setState(AnimationState::Stopped);
    group.step(1.5f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Stopped);
    CORRADE_COMPARE(animable.time, 0.5f);

    /* Restarting the animation should start with zero absolute time */
    animable.setState(AnimationState::Running);
    group.step(2.5f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    CORRADE_COMPARE(animable.time, 0.0f);
}

template<class T> void AnimableTest::pause() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> object;
    BasicAnimableGroup3D<T> group;
    OneShotAnimable<T> animable(object, &group);

    /* First two steps, animation is running */
    group.step(1.0f, 0.5f);
    group.step(2.5f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    CORRADE_COMPARE(animable.time, 1.5f);

    /* Pausing the animation, first step should decrease count of running
       animations and save paused time, next steps shouldn't affect anything */
    CORRADE_COMPARE(group.runningCount(), 1);
    animable.setState(AnimationState::Paused);
    CORRADE_COMPARE(group.runningCount(), 1);
    group.step(3.0f, 0.5f);
    CORRADE_COMPARE(group.runningCount(), 0);
    group.step(4.5f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Paused);
    CORRADE_COMPARE(animable.time, 1.5f);

    /* Unpausing, next step should continue from absolute time when pause
       occured */
    animable.setState(AnimationState::Running);
    group.step(5.0f, 0.5f);
    CORRADE_COMPARE(animable.state(), AnimationState::Running);
    CORRADE_COMPARE(animable.time, 2.0f);
}

void AnimableTest::deleteWhileRunning() {
    Object3D<Float> object;
    AnimableGroup3D group;
    CORRADE_COMPARE(group.runningCount(), 0);

    {
        OneShotAnimable<Float> animable(object, &group);

        /* Eat up some absolute time */
        group.step(1.0f, 0.5f);
        group.step(1.5f, 0.5f);
        CORRADE_COMPARE(group.runningCount(), 1);

        CORRADE_COMPARE(animable.state(), AnimationState::Running);
        CORRADE_COMPARE(animable.time, 0.5f);
    }

    /* Animable got deleted, stepping further should not crash and burn (tm) */
    group.step(1.5f, 0.5f);
    CORRADE_COMPARE(group.runningCount(), 0);
}

void AnimableTest::debug() {
    std::ostringstream o;
    Debug(&o) << AnimationState::Running << AnimationState(0xbe);
    CORRADE_COMPARE(o.str(), "SceneGraph::AnimationState::Running SceneGraph::AnimationState(0xbe)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::AnimableTest)
