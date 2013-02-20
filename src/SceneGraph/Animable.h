#ifndef Magnum_SceneGraph_Animable_h
#define Magnum_SceneGraph_Animable_h
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
 * @brief Class Magnum::SceneGraph::Animable, enum Magnum::SceneGraph::AnimationState
 */

#include "AbstractGroupedFeature.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Animation state

@see Animable::setState()
*/
enum class AnimationState: std::uint8_t {
    /**
     * The animation is stopped. The animation will be started from the
     * beginning when state is changed to @ref AnimationState "AnimationState::Running".
     */
    Stopped,

    /**
     * The animation is stopped. The animation will continue from paused
     * position when state is changed to @ref AnimationState "AnimationState::Running".
     */
    Paused,

    /** The animation is running. */
    Running
};

/** @debugoperator{Magnum::SceneGraph::Animable} */
Debug MAGNUM_SCENEGRAPH_EXPORT operator<<(Debug debug, AnimationState value);

/**
@brief %Animable

Adds animation feature to object. Each %Animable is part of some AnimableGroup,
which takes care of running the animations.

@section Animable-usage Usage

First thing is add Animable feature to some object and implement
animationStep(). You can do it conveniently using multiple inheritance (see
@ref scenegraph-features for introduction). Override animationStep() to
implement your animation, the function provides both absolute animation
time and time delta. Example:
@code
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D<>> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D<>> Scene3D;

class AnimableObject: public Object3D, SceneGraph::Animable3D<> {
    public:
        AnimableObject(Object* parent = nullptr, SceneGraph::DrawableGroup3D<>* group = nullptr): Object3D(parent), SceneGraph::Animable3D<>(this, group) {
            setDuration(10.0f);
            // ...
        }

        void animationStep(GLfloat time, GLfloat delta) override {
            rotateX(15.0_degf*delta); // rotate at 15 degrees per second
        }
}
@endcode

Then add the object to your scene and some animation group. You can also use
AnimableGroup::add() and AnimableGroup::remove() instead of passing the group
in the constructor. The animation is initially in stopped state and without
repeat, see setState(), setRepeated() and setRepeatCount() for more information.
@code
Scene3D scene;
SceneGraph::AnimableGroup3D<> animables;

(new AnimableObject(&scene, &animables))
    ->setState(SceneGraph::AnimationState::Running);
// ...
@endcode

Animation step is performed by calling AnimableGroup::step() in your draw event
implementation. The function expects absolute time from relative to some fixed
point in the past and time delta (i.e. duration of the frame). You can use
Timeline for that, see its documentation for more information.
@code
Timeline timeline;
timeline.start();

void MyApplication::drawEvent() {
    animables.step(timeline.lastFrameTime(), timeline.lastFrameDuration());

    // ...

    timeline.nextFrame();
}
@endcode

@section Animable-performance Using animable groups to improve performance

AnimableGroup is optimized for case when no animation is running - it just
puts itself to rest and waits until some animation changes its state to
@ref AnimationState "AnimationState::Running" again. If you put animations
which are not pernamently running to separate group, they will not be always
traversed when calling AnimableGroup::step(), saving precious frame time.

@section Animable-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into %SceneGraph library.
For other specializations (e.g. using `double` type) you have to use
Animable.hpp implementation file to avoid linker errors. See also
@ref compilation-speedup-hpp for more information.

 - @ref Animable "Animable<2, GLfloat>", @ref AnimableGroup "AnimableGroup<2, GLfloat>"
 - @ref Animable "Animable<3, GLfloat>", @ref AnimableGroup "AnimableGroup<3, GLfloat>"

@see @ref scenegraph, Animable2D, Animable3D, AnimableGroup2D, AnimableGroup3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::uint8_t dimensions, class T>
#else
template<std::uint8_t dimensions, class T = GLfloat>
#endif
class MAGNUM_SCENEGRAPH_EXPORT Animable: public AbstractGroupedFeature<dimensions, Animable<dimensions, T>, T> {
    friend class AnimableGroup<dimensions, T>;

    public:
        /**
         * @brief Constructor
         * @param object    %Object this animable belongs to
         * @param group     Group this animable belongs to
         *
         * Creates stopped non-repeating animation with infinite duration,
         * adds the feature to the object and also to group, if specified.
         * @see setDuration(), setState(), setRepeated(), AnimableGroup::add()
         */
        explicit Animable(AbstractObject<dimensions, T>* object, AnimableGroup<dimensions, T>* group = nullptr);

        ~Animable();

        /** @brief Animation duration */
        inline GLfloat duration() const { return _duration; }

        /** @brief Animation state */
        inline AnimationState state() const { return currentState; }

        /**
         * @brief Set animation state
         * @return Pointer to self (for method chaining)
         *
         * Note that changing state from @ref AnimationState "AnimationState::Stopped"
         * to @ref AnimationState "AnimationState::Paused" is ignored and
         * animation remains in @ref AnimationState "AnimationState::Stopped"
         * state. See also animationStep() for more information.
         * @see animationStarted(), animationPaused(), animationResumed(),
         *      animationStopped()
         */
        Animable<dimensions, T>* setState(AnimationState state);

        /**
         * @brief Whether the animation is repeated
         *
         * @see repeatCount()
         */
        inline bool isRepeated() const { return _repeated; }

        /**
         * @brief Enable/disable repeated animation
         * @return Pointer to self (for method chaining)
         *
         * Default is `false`.
         * @see setRepeatCount()
         */
        inline Animable<dimensions, T>* setRepeated(bool repeated) {
            _repeated = repeated;
            return this;
        }

        /**
         * @brief Repeat count
         *
         * @see isRepeated()
         */
        inline std::uint16_t repeatCount() const { return _repeatCount; }

        /**
         * @brief Set repeat count
         * @return Pointer to self (for method chaining)
         *
         * Has effect only if repeated animation is enabled. `0` means
         * infinitely repeated animation. Default is `0`.
         * @see setRepeated()
         */
        inline Animable<dimensions, T>* setRepeatCount(std::uint16_t count) {
            _repeatCount = count;
            return this;
        }

        /**
         * @brief %Animable group containing this animable
         *
         * If the animable doesn't belong to any group, returns `nullptr`.
         */
        AnimableGroup<dimensions, T>* group();
        const AnimableGroup<dimensions, T>* group() const; /**< @overload */

    protected:
        /**
         * @brief Set animation duration
         * @return Pointer to self (for method chaining)
         *
         * Sets duration of the animation cycle in seconds. Set to `0.0f` for
         * infinite non-repeating animation. Default is `0.0f`.
         */
        /* Protected so only animation implementer can change it */
        inline Animable<dimensions, T>* setDuration(GLfloat duration) {
            _duration = duration;
            return this;
        }

        /**
         * @brief Perform animation step
         * @param time      Time from start of the animation
         * @param delta     Time delta for current frame
         *
         * This function is periodically called from AnimableGroup::step() if
         * the animation state is set to @ref AnimationState "AnimationState::Running".
         * After animation duration is exceeded and repeat is not enabled or
         * repeat count is exceeded, the animation state is set to
         * @ref AnimationState "AnimationState::Stopped".
         *
         * If the animation is resumed from @ref AnimationState "AnimationState::Paused",
         * this function is called with @p time continuing from the point
         * when it was paused. If the animation is resumed from
         * @ref AnimationState "AnimationState::Stopped", @p time starts with
         * zero.
         *
         * @see state(), duration(), isRepeated(), repeatCount()
         */
        virtual void animationStep(GLfloat time, GLfloat delta) = 0;

        /**
         * @brief Action on animation start
         *
         * Called from AnimableGroup::step() when state is changed from
         * @ref AnimationState "AnimationState::Stopped" to
         * @ref AnimationState "AnimationState::Running" and before first
         * animationStep() is called.
         *
         * Default implementation does nothing.
         *
         * @see setState()
         */
        inline virtual void animationStarted() {}

        /**
         * @brief Action on animation pause
         *
         * Called from AnimableGroup::step() when state changes from
         * @ref AnimationState "AnimationState::Running" to
         * @ref AnimationState "AnimationState::Paused" and after last
         * animationStep() is called.
         *
         * Default implementation does nothing.
         *
         * @see setState()
         */
        inline virtual void animationPaused() {}

        /**
         * @brief Action on animation resume
         *
         * Called from AnimableGroup::step() when state changes from
         * @ref AnimationState "AnimationState::Paused" to
         * @ref AnimationState "AnimationState::Running" and before first
         * animationStep() is called.
         *
         * Default implementation does nothing.
         *
         * @see setState()
         */
        inline virtual void animationResumed() {}

        /**
         * @brief Action on animation stop
         *
         * Called from AnimableGroup::step() when state changes from either
         * @ref AnimationState "AnimationState::Running" or
         * @ref AnimationState "AnimationState::Paused" to
         * @ref AnimationState "AnimationState::Stopped" and after last
         * animationStep() is called.
         *
         * You may want to use this function to properly finish the animation
         * in case the framerate is not high enough to have animationStep()
         * called enough times. Default implementation does nothing.
         *
         * @see setState()
         */
        inline virtual void animationStopped() {}

    private:
        GLfloat _duration;
        GLfloat startTime, pauseTime;
        AnimationState previousState;
        AnimationState currentState;
        bool _repeated;
        std::uint16_t _repeatCount;
        std::uint16_t repeats;
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Two-dimensional drawable

Convenience alternative to <tt>%Animable<2, T></tt>. See Animable for more
information.
@note Not available on GCC < 4.7. Use <tt>%Animable<2, T></tt> instead.
@see Animable3D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T = GLfloat>
#else
template<class T>
#endif
using Animable2D = Animable<2, T>;

/**
@brief Three-dimensional animable

Convenience alternative to <tt>%Animable<3, T></tt>. See Animable for more
information.
@note Not available on GCC < 4.7. Use <tt>%Animable<3, T></tt> instead.
@see Animable2D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T = GLfloat>
#else
template<class T>
#endif
using Animable3D = Animable<3, T>;
#endif

}}

#endif
