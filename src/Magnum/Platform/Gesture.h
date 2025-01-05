#ifndef Magnum_Platform_Gesture_h
#define Magnum_Platform_Gesture_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Class @ref Magnum::Platform::TwoFingerGesture
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Functions.h" /* Math::sqrt() */
#include "Magnum/Math/Complex.h"

namespace Magnum { namespace Platform {

/**
@brief Two-finger gesture recognition
@m_since_latest

Tracks position of a primary finger and an arbitrary secondary finger based on
pointer events passed to @ref pressEvent(), @ref releaseEvent() and
@ref moveEvent(). Once two fingers are pressed, the instance is contextually
convertible to @cpp true @ce, and @ref position(), @ref direction(),
@ref relativeTranslation(), @ref relativeRotation() and @ref relativeScaling()
contain gesture properties. Example usage:

@snippet Platform.cpp TwoFingerGesture

The interface is designed primarily for @ref Sdl2Application "*Application"
subclasses and their @relativeref{Sdl2Application,PointerEvent} and
@relativeref{Sdl2Application,PointerMoveEvent} instances, but works also with
any other types that provide appropriate
@relativeref{Sdl2Application::PointerEvent,source()},
@relativeref{Sdl2Application::PointerEvent,isPrimary()},
@relativeref{Sdl2Application::PointerEvent,id()} and  @relativeref{Sdl2Application::PointerEvent,position()} members.

@experimental
*/
class TwoFingerGesture {
    public:
        /**
         * @brief Handle a press event
         *
         * Accepts a pointer event instance such as the one coming from
         * @ref Platform::Sdl2Application::pointerPressEvent(). If the event
         * comes from a primary finger, replaces the internal state with it,
         * waiting for the secondary finger press to happen. If the event comes
         * from a secondary finger, it's used only a primary finger is known,
         * there's no known secondary finger ID yet. or the ID matches the
         * known secondary finger ID. Events that don't come from a touch
         * source are ignored. Returns @cpp true @ce if the event was used,
         * @cpp false @ce if not.
         *
         * The function doesn't modify the event in any way. If needed, it's up
         * to the caller to call
         * @relativeref{Sdl2Application::PointerEvent,setAccepted()}.
         */
        template<class PointerEvent> bool pressEvent(const PointerEvent& event);

        /**
         * @brief Handle a release event
         *
         * Accepts a pointer event instance such as the one coming from
         * @ref Platform::Sdl2Application::pointerReleaseEvent(). If the
         * release comes from a primary finger whose ID is known, resets the
         * state for both the primary and secondary touch, waiting for a
         * primary finger press to happen again. Otherwise, if the release
         * comes from a secondary finger whose ID is known. resets just the
         * secondary finger state, waiting for a different secondary finger
         * press to happen. Events that don't come from a touch source are
         * ignored. Returns @cpp true @ce if the event was used, @cpp false @ce
         * if not.
         *
         * The function doesn't modify the event in any way. If needed, it's up
         * to the caller to call
         * @relativeref{Sdl2Application::PointerEvent,setAccepted()}.
         */
        template<class PointerEvent> bool releaseEvent(const PointerEvent& event);

        /**
         * @brief Handle a move event
         *
         * Accepts a pointer move event instance such as the one coming from
         * @ref Platform::Sdl2Application::pointerMoveEvent(). If the move
         * comes from a primary finger whose ID is known or from a secondary
         * finger whose ID is known, updates given finger state. Events that
         * don't come from a touch source are ignored. Returns @cpp true @ce if
         * the event was used, @cpp false @ce if not.
         *
         * The function doesn't modify the event in any way. If needed, it's up
         * to the caller to call
         * @relativeref{Sdl2Application::PointerEvent,setAccepted()}.
         */
        template<class PointerMoveEvent> bool moveEvent(const PointerMoveEvent& event);

        /**
         * @brief Count of known pressed fingers
         *
         * Is @cpp 0 @ce if @ref pressEvent() wasn't called yet or
         * @ref releaseEvent() happened for the primary finger, @cpp 1 @ce
         * if only the primary finger is pressed or a secondary finger was
         * released and @cpp 2 @ce if both the primary and a secondary finger
         * is currently pressed.
         * @see @ref isGesture()
         */
        UnsignedInt fingerCount() const {
            return (_primaryTouchId != NoTouchId) +
                   (_secondaryTouchId != NoTouchId);
        }

        /**
         * @brief Whether the internal state represents a two-finger gesture
         *
         * Returns @cpp true @ce if both the primary and a secondary finger
         * are pressed, @cpp false @ce otherwise. Same as @ref operator bool().
         */
        bool isGesture() const {
            return fingerCount() == 2;
        }

        /**
         * @brief Whether the internal state represents a two-finger gesture
         *
         * Returns @cpp true @ce if both the primary and a secondary finger
         * are pressed, @cpp false @ce otherwise. Same as @ref operator bool().
         */
        explicit operator bool() const {
            return fingerCount() == 2;
        }

        /**
         * @brief Centroid between the two known pressed finger positions
         *
         * If only one or no fingers are pressed --- i.e., @ref isGesture() is
         * @cpp false @ce --- returns a NaN vector.
         * @see @ref direction(), @ref relativeTranslation(),
         *      @ref Math::isNan()
         */
        Vector2 position() const {
            return (_primaryTouchPosition + _secondaryTouchPosition)*0.5f;
        }

        /**
         * @brief Direction from the center to the primary finger position
         *
         * Negate the return value to get direction from the center to the
         * secondary finger. If only one or no fingers are pressed --- i.e.,
         * @ref isGesture() is @cpp false @ce ---  returns a NaN vector.
         * @see @ref position(), @ref relativeRotation(),
         *      @ref relativeScaling(), @ref Math::isNan()
         */
        Vector2 direction() const {
            return (_primaryTouchPosition - _secondaryTouchPosition)*0.5f;
        }

        /**
         * @brief Translation of the centroid relative to the previous finger positions
         *
         * If there was no movement since the press, returns a zero vector. If
         * only one or no fingers are pressed --- i.e., @ref isGesture() is
         * @cpp false @ce --- returns a NaN vector.
         * @see @ref position(), @ref relativeRotation(),
         *      @ref relativeScaling(), @ref Math::isNan()
         */
        Vector2 relativeTranslation() const {
            return (_primaryTouchPosition - _primaryPreviousTouchPosition +
                    _secondaryTouchPosition - _secondaryPreviousTouchPosition)*0.5f;
        }

        /**
         * @brief Rotation relative to the previous finger positions
         *
         * Note that given the event coordinates are in a Y down coordinate,
         * positive rotation angle is clockwise. If there was no movement since
         * the press, returns an identity rotation. If only one or no fingers
         * are pressed --- i.e., @ref isGesture() is @cpp false @ce --- returns
         * a complex NaN.
         *
         * The function returns a @relativeref{Math,Complex} instead of an
         * angle as the angle would likely be converted back to a rotation
         * representation anyway. Use @ref Complex::toMatrix(),
         * @ref Complex::transformVector() or @ref Complex::angle() if a
         * different representation is needed.
         * @see @ref direction(), @ref relativeTranslation(),
         *      @ref relativeScaling(), @ref Math::isNan()
         */
        Complex relativeRotation() const {
            /*           prev * rot = cur
               prev^-1 * prev * rot = prev^-1 * cur
                                rot = prev^-1 * cur */
            return Complex{(_primaryPreviousTouchPosition - _secondaryPreviousTouchPosition).normalized()}.inverted()*Complex{(_primaryTouchPosition - _secondaryTouchPosition).normalized()};
        }

        /**
         * @brief Scaling relative to the previous finger positions
         *
         * The returned value is always positive. Values less than
         * @cpp 1.0f @ce are when the points are getting closer, values larger
         * than @cpp 1.0f @ce are when the points are getting further apart. If
         * there was no movement since the press, returns @cpp 1.0f @ce. If
         * only one or no fingers are pressed --- i.e., @ref isGesture() is
         * @cpp false @ce --- returns a NaN.
         * @see @ref direction(), @ref relativeTranslation(),
         *      @ref relativeRotation(), @ref Math::isNan()
         */
        Float relativeScaling() const {
            return Math::sqrt(
                (_secondaryTouchPosition - _primaryTouchPosition).dot()/
                (_secondaryPreviousTouchPosition - _primaryPreviousTouchPosition).dot());
        }

    private:
        struct Touch {
            Long id;
            Vector2 position;
        };

        /* ~Long{} is -1, which may collide with actual pointer IDs (SDL uses
           it to denote a mouse, for example) */
        constexpr static Long NoTouchId = 1ll << 63;

        Long _primaryTouchId = NoTouchId;
        Vector2 _primaryTouchPosition{Constants::nan()},
            _primaryPreviousTouchPosition{Constants::nan()};

        Long _secondaryTouchId = NoTouchId;
        Vector2 _secondaryTouchPosition{Constants::nan()},
            _secondaryPreviousTouchPosition{Constants::nan()};
};

namespace Implementation {

/* Not all Application::PointerEventSource enums have a Touch member (e.g.
   GlfwApplication doesn't), this makes it return false for such types, instead
   of failing to compile */
template<class PointerEventSource> constexpr bool isTouchPointerEventSource(PointerEventSource p, decltype(PointerEventSource::Touch)* = nullptr) {
    return p == PointerEventSource::Touch;
}
constexpr bool isTouchPointerEventSource(...) { return false; }

}

template<class PointerEvent> bool TwoFingerGesture::pressEvent(const PointerEvent& event) {
    /* Filter away non-touch sources. Other than that just assume it's a finger
       or something equivalent, capable of multi-touch -- i.e., don't even
       check pointers(). */
    if(!Implementation::isTouchPointerEventSource(event.source()))
        return false;

    /* If this is the primary finger, unconditionally replace the primary touch
       with it, and reset everything else */
    if(event.isPrimary()) {
        _primaryTouchId = event.id();
        _primaryTouchPosition = event.position();
        _primaryPreviousTouchPosition = event.position();
        _secondaryTouchId = NoTouchId;
        _secondaryTouchPosition = Vector2{Constants::nan()};
        _secondaryPreviousTouchPosition = Vector2{Constants::nan()};
        return true;
    }

    /* If this is a secondary finger and a primary finger is already known,
       remember it either if it has a matching ID or there's no recorded second
       touch yet */
    if(_primaryTouchId != NoTouchId && (_secondaryTouchId == NoTouchId || event.id() == _secondaryTouchId)) {
        _secondaryTouchId = event.id();
        _secondaryTouchPosition = event.position();
        _secondaryPreviousTouchPosition = event.position();
        return true;
    }

    /* If this is a secondary finger and a primary finger is not known yet, or
       if this is a secondary finger with a different ID, do nothing */
    return false;
}

template<class PointerEvent> bool TwoFingerGesture::releaseEvent(const PointerEvent& event) {
    /* Filter away non-touch sources. Other than that just assume it's a finger
       or something equivalent, capable of multi-touch -- i.e., don't even
       check pointers(). */
    if(!Implementation::isTouchPointerEventSource(event.source()))
        return false;

    /* If the primary finger is lifted, reset everything and wait for the next
       time a primary finger is pressed again */
    if(event.isPrimary() && event.id() == _primaryTouchId) {
        _primaryTouchId = NoTouchId;
        _primaryTouchPosition = Vector2{Constants::nan()};
        _primaryPreviousTouchPosition = Vector2{Constants::nan()};
        _secondaryTouchId = NoTouchId;
        _secondaryTouchPosition = Vector2{Constants::nan()};
        _secondaryPreviousTouchPosition = Vector2{Constants::nan()};
        return true;
    }

    /* If this is a secondary finger, reset just that one, and wait for another
       secondary finger press to take up its place */
    if(!event.isPrimary() && event.id() == _secondaryTouchId) {
        _secondaryTouchId = NoTouchId;
        _secondaryTouchPosition = Vector2{Constants::nan()};
        _secondaryPreviousTouchPosition = Vector2{Constants::nan()};
        return true;
    }

    /* If the IDs don't match or their primary/secondary state doesn't match,
       do nothing */
    return false;
}

template<class PointerMoveEvent> bool TwoFingerGesture::moveEvent(const PointerMoveEvent& event) {
    /* Filter away non-touch sources. Other than that just assume it's a finger
       or something equivalent, capable of multi-touch -- i.e., don't even
       check pointers(). */
    if(!Implementation::isTouchPointerEventSource(event.source()))
        return false;

    /* If the event matches any of the recorded IDs, update the corresponding
       values */
    if(event.isPrimary() && event.id() == _primaryTouchId) {
        _primaryPreviousTouchPosition = _primaryTouchPosition;
        _primaryTouchPosition = event.position();
        return true;
    }
    if(!event.isPrimary() && event.id() == _secondaryTouchId) {
        _secondaryPreviousTouchPosition = _secondaryTouchPosition;
        _secondaryTouchPosition = event.position();
        return true;
    }

    /* If the IDs don't match or their primary/secondary state doesn't match,
       do nothing */
    return false;
}

}}

#endif
