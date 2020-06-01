#ifndef Magnum_SceneGraph_Animable_h
#define Magnum_SceneGraph_Animable_h
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
 * @brief Class @ref Magnum::SceneGraph::Animable, alias @ref Magnum::SceneGraph::BasicAnimable2D, @ref Magnum::SceneGraph::BasicAnimable3D, typedef @ref Magnum::SceneGraph::Animable2D, @ref Magnum::SceneGraph::Animable3D, enum @ref Magnum::SceneGraph::AnimationState
 */

#include "Magnum/SceneGraph/AbstractGroupedFeature.h"
#include "Magnum/SceneGraph/visibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Animation state

@see @ref Animable::setState()
*/
enum class AnimationState: UnsignedByte {
    /**
     * The animation is stopped. The animation will be started from the
     * beginning when state is changed to @ref AnimationState::Running.
     */
    Stopped,

    /**
     * The animation is stopped. The animation will continue from paused
     * position when state is changed to @ref AnimationState::Running.
     */
    Paused,

    /** The animation is running. */
    Running
};

/** @debugoperatorenum{AnimationState} */
MAGNUM_SCENEGRAPH_EXPORT Debug& operator<<(Debug& debug, AnimationState value);

/**
@brief Animable

Adds animation feature to object. Each Animable is part of some
@ref AnimableGroup, which takes care of running the animations.

@note
    The Animable feature affects only a single object. In many cases you have
    animations affecting multiple objects in a scene. See the @ref Animation
    namespace and the @ref Animation::Player class in particular for an
    alternative API.

@section SceneGraph-Animable-usage Usage

First thing is to add @ref Animable feature to some object and implement
@ref animationStep(). You can do it conveniently using multiple inheritance
(see @ref scenegraph-features for introduction). Override @ref animationStep()
to implement your animation, the function provides both absolute animation
time and time delta. Example:

@snippet MagnumSceneGraph.cpp Animable-usage-definition

Similarly to @ref Drawable feature, there is no way to just animate all the
objects in the scene. You need to create animable group and use it to control
given set of animations. You can also use @ref AnimableGroup::add() and
@ref AnimableGroup::remove() instead of passing the group in the constructor.
The animation is initially in stopped state and without repeat, see
@ref setState(), @ref setRepeated() and @ref setRepeatCount() for more
information.

@snippet MagnumSceneGraph.cpp Animable-usage

Animation step is performed by calling @ref AnimableGroup::step() in your draw
event implementation. The function expects absolute time from relative to some
fixed point in the past and time delta (i.e. duration of the frame). You can
use @ref Timeline for that, see its documentation for more information.

@snippet MagnumSceneGraph-gl.cpp Animable-usage-timeline

@section SceneGraph-Animable-multiple-groups Using multiple animable groups to improve performance

@ref AnimableGroup is optimized for case when no animation is running --- it
just puts itself to rest and waits until some animation changes its state to
@ref AnimationState::Running again. If you put animations which are not
permanently running into separate group, they will not be traversed every time
the @ref AnimableGroup::step() gets called, saving precious frame time.

@section SceneGraph-Animable-explicit-specializations Explicit template specializations

The following specializations are explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Magnum::Double "Double"
type) you have to use @ref Animable.hpp implementation file to avoid linker
errors. See also @ref compilation-speedup-hpp for more information.

-   @ref Animable2D, @ref AnimableGroup2D
-   @ref Animable3D, @ref AnimableGroup3D

@see @ref scenegraph, @ref BasicAnimable2D, @ref BasicAnimable3D,
    @ref Animable2D, @ref Animable3D, @ref AnimableGroup
*/
template<UnsignedInt dimensions, class T> class Animable: public AbstractGroupedFeature<dimensions, Animable<dimensions, T>, T> {
    friend AnimableGroup<dimensions, T>;

    public:
        /**
         * @brief Constructor
         * @param object    Object this animable belongs to
         * @param group     Group this animable belongs to
         *
         * Creates stopped non-repeating animation with infinite duration,
         * adds the feature to the object and also to group, if specified.
         * @see @ref setDuration(), @ref setState(), @ref setRepeated(),
         *      @ref AnimableGroup::add()
         */
        explicit Animable(AbstractObject<dimensions, T>& object, AnimableGroup<dimensions, T>* group = nullptr);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* This is here to avoid ambiguity with deleted copy constructor when
           passing `*this` from class subclassing both Animable and AbstractObject */
        template<class U, class = typename std::enable_if<std::is_base_of<AbstractObject<dimensions, T>, U>::value>::type> explicit Animable(U& object): Animable<dimensions, T>{static_cast<AbstractObject<dimensions, T>&>(object)} {}
        #endif

        ~Animable();

        /** @brief Animation duration */
        Float duration() const { return _duration; }

        /** @brief Animation state */
        AnimationState state() const { return _currentState; }

        /**
         * @brief Set animation state
         * @return Reference to self (for method chaining)
         *
         * Note that changing state from @ref AnimationState::Stopped to
         * @ref AnimationState::Paused is ignored and animation remains in
         * @ref AnimationState::Stopped state. See also @ref animationStep()
         * for more information.
         * @see @ref animationStarted(), @ref animationPaused(),
         *      @ref animationResumed(), @ref animationStopped()
         */
        Animable<dimensions, T>& setState(AnimationState state);

        /**
         * @brief Whether the animation is repeated
         *
         * @see @ref repeatCount()
         */
        bool isRepeated() const { return _repeated; }

        /**
         * @brief Enable/disable repeated animation
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp false @ce.
         * @see @ref setRepeatCount()
         */
        Animable<dimensions, T>& setRepeated(bool repeated) {
            _repeated = repeated;
            return *this;
        }

        /**
         * @brief Repeat count
         *
         * @see @ref isRepeated()
         */
        UnsignedShort repeatCount() const { return _repeatCount; }

        /**
         * @brief Set repeat count
         * @return Reference to self (for method chaining)
         *
         * Has effect only if repeated animation is enabled. @cpp 0 @ce means
         * infinitely repeated animation. Default is @cpp 0 @ce.
         * @see @ref setRepeated()
         */
        Animable<dimensions, T>& setRepeatCount(UnsignedShort count) {
            _repeatCount = count;
            return *this;
        }

        /**
         * @brief Group containing this animable
         *
         * If the animable doesn't belong to any group, returns @cpp nullptr @ce.
         */
        AnimableGroup<dimensions, T>* animables();
        const AnimableGroup<dimensions, T>* animables() const; /**< @overload */

    protected:
        /**
         * @brief Set animation duration
         * @return Reference to self (for method chaining)
         *
         * Sets duration of the animation cycle in seconds. Set to @cpp 0.0f @ce
         * for infinite non-repeating animation. Default is @cpp 0.0f @ce.
         */
        /* Protected so only animation implementer can change it */
        Animable<dimensions, T>& setDuration(Float duration) {
            _duration = duration;
            return *this;
        }

        /**
         * @brief Perform animation step
         * @param time      Time from start of the animation
         * @param delta     Time delta for current frame
         *
         * This function is periodically called from @ref AnimableGroup::step()
         * if the animation state is set to @ref AnimationState::Running. After
         * animation duration is exceeded and repeat is not enabled or repeat
         * count is exceeded, the animation state is set to @ref AnimationState::Stopped.
         *
         * If the animation is resumed from @ref AnimationState::Paused, this
         * function is called with @p time continuing from the point when it
         * was paused. If the animation is resumed from @ref AnimationState::Stopped,
         * @p time starts with zero.
         *
         * @see @ref state(), @ref duration(), @ref isRepeated(),
         *      @ref repeatCount()
         */
        virtual void animationStep(Float time, Float delta) = 0;

        /**
         * @brief Action on animation start
         *
         * Called from @ref AnimableGroup::step() when state is changed from
         * @ref AnimationState::Stopped to @ref AnimationState::Running and
         * before first @ref animationStep() is called.
         *
         * Default implementation does nothing.
         * @see @ref setState()
         */
        virtual void animationStarted() {}

        /**
         * @brief Action on animation pause
         *
         * Called from @ref AnimableGroup::step() when state changes from
         * @ref AnimationState::Running to @ref AnimationState::Paused and
         * after last @ref animationStep() is called.
         *
         * Default implementation does nothing.
         * @see @ref setState()
         */
        virtual void animationPaused() {}

        /**
         * @brief Action on animation resume
         *
         * Called from @ref AnimableGroup::step() when state changes from
         * @ref AnimationState::Paused to @ref AnimationState::Running and
         * before first @ref animationStep() is called.
         *
         * Default implementation does nothing.
         * @see @ref setState()
         */
        virtual void animationResumed() {}

        /**
         * @brief Action on animation stop
         *
         * Called from @ref AnimableGroup::step() when state changes from
         * either @ref AnimationState::Running or @ref AnimationState::Paused
         * to @ref AnimationState::Stopped and after last @ref animationStep()
         * is called.
         *
         * You may want to use this function to properly finish the animation
         * in case the framerate is not high enough to have @ref animationStep()
         * called enough times. Default implementation does nothing.
         * @see @ref setState()
         */
        virtual void animationStopped() {}

    private:
        Float _duration;
        Float _startTime, _pauseTime;
        AnimationState _previousState, _currentState;
        bool _repeated;
        UnsignedShort _repeatCount;
        UnsignedShort _repeats;
};

/**
@brief Animable for two-dimensional scenes

Convenience alternative to @cpp Animable<2, T> @ce. See @ref Animable for more
information.
@see @ref Animable2D, @ref BasicAnimable3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using BasicAnimable2D = Animable<2, T>;
#endif

/**
@brief Animable for two-dimensional float scenes

@see @ref Animable3D
*/
typedef BasicAnimable2D<Float> Animable2D;

/**
@brief Animable for three-dimensional scenes

Convenience alternative to @cpp Animable<3, T> @ce. See @ref Animable for more
information.
@see @ref Animable3D, @ref BasicAnimable2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using BasicAnimable3D = Animable<3, T>;
#endif

/**
@brief Animable for three-dimensional float scenes

@see @ref Animable2D
*/
typedef BasicAnimable3D<Float> Animable3D;

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT Animable<2, Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT Animable<3, Float>;
#endif

}}

#endif
