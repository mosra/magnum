/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2021 nodoteve <nodoteve@yandex.com>
    Copyright © 2025 hsdk123 <daegon.dhsk@outlook.com>

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

#include "AndroidApplication.h"

#include <Corrade/Utility/AndroidLogStreamBuffer.h>
#include <Corrade/Utility/Debug.h>
#include <android_native_app_glue.h>

#include "Magnum/GL/Version.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Platform/ScreenedApplication.hpp"

#include "Implementation/Egl.h"

/* This function would be stripped by the linker otherwise. Search for the name
   below for the complete rant. */
extern "C" void ANativeActivity_onCreate(struct ANativeActivity*, void*, size_t);

namespace Magnum { namespace Platform {

enum class AndroidApplication::Flag: UnsignedByte {
    Redraw = 1 << 0
};

struct AndroidApplication::LogOutput {
    LogOutput();

    Utility::AndroidLogStreamBuffer debugBuffer, warningBuffer, errorBuffer;
    std::ostream debugStream, warningStream, errorStream;
    Debug redirectDebug;
    Warning redirectWarning;
    Error redirectError;
};

AndroidApplication::LogOutput::LogOutput():
    debugBuffer(Utility::AndroidLogStreamBuffer::LogPriority::Info, "magnum"),
    warningBuffer(Utility::AndroidLogStreamBuffer::LogPriority::Warning, "magnum"),
    errorBuffer(Utility::AndroidLogStreamBuffer::LogPriority::Error, "magnum"),
    debugStream(&debugBuffer), warningStream(&warningBuffer), errorStream(&errorBuffer),
    redirectDebug{&debugStream}, redirectWarning{&warningStream}, redirectError{&errorStream}
{}

AndroidApplication::AndroidApplication(const Arguments& arguments): AndroidApplication{arguments, Configuration{}, GLConfiguration{}} {}

AndroidApplication::AndroidApplication(const Arguments& arguments, const Configuration& configuration): AndroidApplication{arguments, configuration, GLConfiguration{}} {}

AndroidApplication::AndroidApplication(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration): AndroidApplication{arguments, NoCreate} {
    create(configuration, glConfiguration);
}

AndroidApplication::AndroidApplication(const Arguments& arguments, NoCreateT): _state{arguments}, _context{InPlaceInit, NoCreate} {
    /* Redirect debug output to Android log */
    _logOutput.reset(new LogOutput);
}

AndroidApplication::~AndroidApplication() {
    /* Destroy Magnum context first to avoid it potentially accessing the
       now-destroyed GL context after */
    _context = Containers::NullOpt;

    eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(_display, _glContext);
    eglDestroySurface(_display, _surface);
    eglTerminate(_display);
}

ANativeActivity* AndroidApplication::nativeActivity() {
    return _state->activity;
}

void AndroidApplication::create() {
    create(Configuration{}, GLConfiguration{});
}

void AndroidApplication::create(const Configuration& configuration) {
    create(configuration, GLConfiguration{});
}

void AndroidApplication::create(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    if(!tryCreate(configuration, glConfiguration)) std::exit(32);
}

bool AndroidApplication::tryCreate(const Configuration& configuration) {
    return tryCreate(configuration, GLConfiguration{});
}

bool AndroidApplication::tryCreate(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    CORRADE_ASSERT(_context->version() == GL::Version::None, "Platform::AndroidApplication::tryCreate(): context already created", false);

    /* Initialize EGL */
    _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(!eglInitialize(_display, nullptr, nullptr)) {
        Error() << "Platform::AndroidApplication::tryCreate(): cannot initialize EGL:"
                << Implementation::eglErrorString(eglGetError());
        return false;
    }

    /* Choose config */
    const EGLint configAttributes[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, glConfiguration.colorBufferSize().r(),
        EGL_GREEN_SIZE, glConfiguration.colorBufferSize().g(),
        EGL_BLUE_SIZE, glConfiguration.colorBufferSize().b(),
        EGL_ALPHA_SIZE, glConfiguration.colorBufferSize().a(),
        EGL_DEPTH_SIZE, glConfiguration.depthBufferSize(),
        EGL_STENCIL_SIZE, glConfiguration.stencilBufferSize(),
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    EGLint configCount;
    EGLConfig config;
    if(!eglChooseConfig(_display, configAttributes, &config, 1, &configCount)) {
        Error() << "Platform::AndroidApplication::tryCreate(): cannot choose EGL config:"
                << Implementation::eglErrorString(eglGetError());
        return false;
    }

    /* Resize native window and match it to the selected format */
    EGLint format;
    CORRADE_INTERNAL_ASSERT_OUTPUT(eglGetConfigAttrib(_display, config, EGL_NATIVE_VISUAL_ID, &format));
    ANativeWindow_setBuffersGeometry(_state->window,
        configuration.size().isZero() ? 0 : configuration.size().x(),
        configuration.size().isZero() ? 0 : configuration.size().y(), format);

    /* Create surface and context */
    if(!(_surface = eglCreateWindowSurface(_display, config, _state->window, nullptr))) {
        Error() << "Platform::AndroidApplication::tryCreate(): cannot create EGL window surface:"
                << Implementation::eglErrorString(eglGetError());
        return false;
    }
    const EGLint contextAttributes[] = {
        #ifdef MAGNUM_TARGET_GLES2
        EGL_CONTEXT_CLIENT_VERSION, 2,
        #elif defined(MAGNUM_TARGET_GLES)
        EGL_CONTEXT_CLIENT_VERSION, 3,
        #else
        #error Android with desktop OpenGL? Wow, that is a new thing.
        #endif
        EGL_NONE
    };
    if(!(_glContext = eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttributes))) {
        Error() << "Platform::AndroidApplication::tryCreate(): cannot create EGL context:"
                << Implementation::eglErrorString(eglGetError());
        return false;
    }

    /* Make the context current */
    CORRADE_INTERNAL_ASSERT_OUTPUT(eglMakeCurrent(_display, _surface, _surface, _glContext));

    /* Return true if the initialization succeeds */
    return _context->tryCreate(glConfiguration);
}

Vector2i AndroidApplication::framebufferSize() const {
    return {ANativeWindow_getWidth(_state->window),
            ANativeWindow_getHeight(_state->window)};
}

void AndroidApplication::swapBuffers() {
    eglSwapBuffers(_display, _surface);
}

void AndroidApplication::redraw() {
    _flags |= Flag::Redraw;
}

void AndroidApplication::viewportEvent(ViewportEvent&) {}

namespace {
    struct Data {
        Data(Containers::Pointer<AndroidApplication>(*instancer)(const AndroidApplication::Arguments&), void(*nativeActivity)(ANativeActivity*,void*,size_t)): instancer(instancer), nativeActivity{nativeActivity} {}

        Containers::Pointer<AndroidApplication>(*instancer)(const AndroidApplication::Arguments&);
        Containers::Pointer<AndroidApplication> instance;

        void(*nativeActivity)(ANativeActivity*,void*,size_t);
    };
}

void AndroidApplication::commandEvent(android_app* state, int32_t cmd) {
    Data& data = *static_cast<Data*>(state->userData);

    switch(cmd) {
        case APP_CMD_SAVE_STATE:
            /** @todo Make use of this */
            break;

        case APP_CMD_INIT_WINDOW:
            /* Create the application */
            if(!data.instance) {
                data.instance = data.instancer(state);
                data.instance->drawEvent();
            }
            break;

        case APP_CMD_TERM_WINDOW:
            /* Destroy the application */
            data.instance.reset();
            break;

        case APP_CMD_GAINED_FOCUS:
        case APP_CMD_LOST_FOCUS:
            /** @todo Make use of these */
            break;

        case APP_CMD_CONFIG_CHANGED: {
            /* This says "the current device configuration has changed", which
               is about as vague as it can get. It seems to be that this gets
               emitted when screen orientation changes, for example. Fire the
               viewport event in this case. */
            ViewportEvent e{{ANativeWindow_getWidth(data.instance->_state->window),
                             ANativeWindow_getHeight(data.instance->_state->window)}};
            data.instance->viewportEvent(e);
        } break;
    }
}

namespace {

AndroidApplication::Pointers motionEventButtons(AInputEvent* event) {
    const std::int32_t buttons = AMotionEvent_getButtonState(event);
    AndroidApplication::Pointers pointers;
    if(buttons & AMOTION_EVENT_BUTTON_PRIMARY)
        pointers |= AndroidApplication::Pointer::MouseLeft;
    if(buttons & AMOTION_EVENT_BUTTON_TERTIARY)
        pointers |= AndroidApplication::Pointer::MouseMiddle;
    if(buttons & AMOTION_EVENT_BUTTON_SECONDARY)
        pointers |= AndroidApplication::Pointer::MouseRight;
    /** @todo AMOTION_EVENT_BUTTON_BACK, AMOTION_EVENT_BUTTON_FORWARD once it's
        possible to verify they match MouseButton4 / MouseButton5 in
        GlfwApplication and Sdl2Application */
    return pointers;
}

Containers::Pair<AndroidApplication::PointerEventSource, AndroidApplication::Pointers> motionEventPointers(AInputEvent* event, std::size_t i, const AndroidApplication::Pointers pressedButtons) {
    switch(AMotionEvent_getToolType(event, i)) {
        case AMOTION_EVENT_TOOL_TYPE_MOUSE:
            /** @todo MouseButton4 / MouseButton5, once they're added &
                tested */
            return {AndroidApplication::PointerEventSource::Mouse,
                    (AndroidApplication::Pointer::MouseLeft|
                     AndroidApplication::Pointer::MouseMiddle|
                     AndroidApplication::Pointer::MouseRight) & pressedButtons};
        case AMOTION_EVENT_TOOL_TYPE_FINGER:
            return {AndroidApplication::PointerEventSource::Touch,
                    AndroidApplication::Pointer::Finger};
        case AMOTION_EVENT_TOOL_TYPE_STYLUS:
            /** @todo use pressedButtonsPointers once there's additional pen
                button enum values */
            return {AndroidApplication::PointerEventSource::Pen,
                    AndroidApplication::Pointer::Pen};
        case AMOTION_EVENT_TOOL_TYPE_ERASER:
            return {AndroidApplication::PointerEventSource::Touch,
                    AndroidApplication::Pointer::Eraser};
        case AMOTION_EVENT_TOOL_TYPE_UNKNOWN:
        default:
            return {AndroidApplication::PointerEventSource::Unknown,
                    AndroidApplication::Pointer::Unknown};
    }
}

template<class T> Vector2 updatePreviousTouch(T(&previousTouches)[32], const std::int32_t id, const Containers::Optional<Vector2>& position) {
    std::size_t firstFree = ~std::size_t{};
    for(std::size_t i = 0; i != Containers::arraySize(previousTouches); ++i) {
        /* Previous position found */
        if(previousTouches[i].id == id) {
            /* Update with the current position, return delta to previous */
            if(position) {
                const Vector2 relative = *position - previousTouches[i].position;
                previousTouches[i].position = *position;
                return relative;
            /* Clear previous position */
            } else {
                previousTouches[i].id = ~Int{};
                return {};
            }
        /* Unused slot, remember in case there won't be any previous position
           found */
        } else if(previousTouches[i].id == ~Int{} && firstFree == ~std::size_t{}) {
            firstFree = i;
        }
    }

    /* If we're not resetting the position and there's a place where to put the
       new one, save. Otherwise don't do anything -- the touch that didn't fit
       will always report as having no relative position. */
    if(position && firstFree != ~std::size_t{}) {
        previousTouches[firstFree].id = id;
        previousTouches[firstFree].position = *position;
    }

    return {};
}

/* Unlike e.g. SDL, which guarantees that pointer IDs are unique among all
   pointer types, here they of course don't care. So use the reported ID only
   for touches and artificial constants for the rest. */
std::int32_t pointerIdForSource(AndroidApplication::PointerEventSource source, std::int32_t id) {
    switch(source) {
        case AndroidApplication::PointerEventSource::Touch:
            return id;
        case AndroidApplication::PointerEventSource::Mouse:
            return -1;
        case AndroidApplication::PointerEventSource::Pen:
            return -2;
        case AndroidApplication::PointerEventSource::Unknown:
            return -3;
    }
}

}

std::int32_t AndroidApplication::inputEvent(android_app* state, AInputEvent* event) {
    CORRADE_INTERNAL_ASSERT(static_cast<Data*>(state->userData)->instance);
    AndroidApplication& app = *static_cast<Data*>(state->userData)->instance;
    if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        const std::int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
        switch(action) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP: {
                /* Figure out which pointer actually changed in given event,
                   because OF COURSE the API is so horrible that this is
                   non-trivial. For AMOTION_EVENT_ACTION_DOWN we assume it's
                   the first ever pointer being pressed, and thus the count
                   being 1, thus the pointer that changed is the first and
                   only. */
                std::int32_t pointerChanged;
                if(action == AMOTION_EVENT_ACTION_DOWN) {
                    CORRADE_INTERNAL_ASSERT(AMotionEvent_getPointerCount(event) == 1);
                    pointerChanged = 0;
                /* For AMOTION_EVENT_ACTION_UP it's ... apparently the last
                   remaining pointer going up. Not the primary one (see below
                   for the `primary` bit decision tree). The docs make it look
                   like the event also contains any other pointers, but it's
                   probably just mentioning the AMotionEvent_getHistoricalX()
                   etc. fields? Er? Why is it not mentioning that for for
                   AMOTION_EVENT_ACTION_POINTER_UP then?
                    https://developer.android.com/reference/android/view/MotionEvent#ACTION_UP */
                } else if(action == AMOTION_EVENT_ACTION_UP) {
                    CORRADE_INTERNAL_ASSERT(AMotionEvent_getPointerCount(event) == 1);
                    pointerChanged = 0;
                /* The AMOTION_EVENT_ACTION_POINTER_DOWN/_UP actually mean a
                   secondary pointer was pressed or released. Who would have
                   thought. In that case, the actual changed pointer is given
                   to us with this fucking atrocity of a bitmask. Well,
                   alright, what can I do, but why such a bitmask couldn't be
                   done above as well, huh??? */
                } else if(action == AMOTION_EVENT_ACTION_POINTER_DOWN ||
                          action == AMOTION_EVENT_ACTION_POINTER_UP) {
                    pointerChanged = (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

                const bool press =
                    action == AMOTION_EVENT_ACTION_DOWN ||
                    action == AMOTION_EVENT_ACTION_POINTER_DOWN;
                const Vector2 position{AMotionEvent_getX(event, pointerChanged),
                                       AMotionEvent_getY(event, pointerChanged)};

                /* Query the currently pressed buttons. If this is not a mouse
                   event, it'll give back garbage, but that's fine as we won't
                   use it in that case. Then, based on whether it's a press or
                   a release, use the previously recorded pointers to figure
                   out what was actually pressed. */
                const Pointers pressedButtons = motionEventButtons(event);
                const Containers::Pair<PointerEventSource, Pointers> sourcePointers =
                    motionEventPointers(event, pointerChanged, press ?
                        pressedButtons & ~app._previousPressedButtons :
                        ~pressedButtons & app._previousPressedButtons);
                const std::int32_t pointerId = pointerIdForSource(sourcePointers.first(), AMotionEvent_getPointerId(event, pointerChanged));

                /* Decide whether this is a primary pointer. It's tempting to
                   use the distinction between _DOWN and _POINTER_DOWN to
                   distinguish a primary pointer from a secondary one, but
                   that'd be giving too much credit to this damn API. The
                   problem is that, if multiple fingers is pressed, _POINTER_UP
                   is fired if any of the secondary fingers are lifted, but
                   also if the primary finger is lifted. Which in turn means
                   the primary finger would be treated as secondary for the
                   up event, which is wrong. Second, then none of the remaining
                   fingers then have any reasonable way to get promoted to a
                   primary one, so they all stay secondary. BUT THEN, if the
                   last one of the secondary fingers gets lifted, _UP is fired
                   for it, so it suddenly becomes primary. Which a total trash
                   fire of a broken behavior. A mention worth a laugh is the
                   official Android developer blog, where in 2010 they
                   suggested the same thing --- in particular, when the primary
                   pointer is lifted, *an arbitrary one* from the rest is
                   chosen as primary.
                    https://android-developers.googleblog.com/2010/06/making-sense-of-multitouch.html
                   Eh. Maybe it makes sense for just two touches, but
                   definitely not for multiple. So let's just ignore all that
                   and do it by hand like in Sdl2Application and
                   EmscriptenApplication, to have consistent behavior across
                   all.

                   Mouse and pen is always a primary pointer. */
                bool primary;
                if(sourcePointers.first() == PointerEventSource::Mouse ||
                   sourcePointers.first() == PointerEventSource::Pen) {
                    primary = true;

                /* For touch update primary finger info */
                } else if(sourcePointers.first() == PointerEventSource::Touch) {
                    /* If there's no primary finger yet and this is the first
                       finger pressed (i.e., what AMOTION_EVENT_ACTION_DOWN
                       implies), it becomes the primary finger. If the primary
                       finger is lifted, no other finger becomes primary until
                       all others are lifted as well. Again, this is the same
                       as in Sdl2Application and EmscriptenApplication. */
                    if(app._primaryFingerId == ~Int{} && action == AMOTION_EVENT_ACTION_DOWN) {
                        CORRADE_INTERNAL_ASSERT(AMotionEvent_getPointerCount(event) == 1);
                        primary = true;
                        app._primaryFingerId = pointerId;
                    /* Otherwise, if this is the primary finger, mark it as
                       such */
                    } else if(app._primaryFingerId == pointerId) {
                        primary = true;
                        /* ... but if it's a release, it's no longer primary */
                        if(!press)
                            app._primaryFingerId = ~Int{};
                    /* Otherwise this is not the primary finger */
                    } else primary = false;

                /* Unknown pointer is probably not a primary one */
                } else primary = false;

                /* The expectation is that the difference betweeen the
                   previously recorded set of pointers and current one will be
                   exactly one bit for a pointer type that got either pressed
                   or released. If it's not, it means we lost some events, and
                   until API 33+ and AMotionEvent_getActionButton() on
                   AMOTION_EVENT_BUTTON_PRESS / AMOTION_EVENT_BUTTON_RELEASE,
                   there's no way to reliably know what concrete mouse / pen
                   button caused the event. */
                Pointer pointer;
                /* http://www.graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2 */
                if(sourcePointers.second() && !(UnsignedByte(sourcePointers.second()) & (UnsignedByte(sourcePointers.second()) - 1)))
                    pointer = Pointer(UnsignedByte(sourcePointers.second()));
                else
                    pointer = Pointer::Unknown;

                /** @todo Once there's an ability to actually *know* what
                    button was pressed or released (API 33+), implement
                    translation to move events like in GlfwApplication,
                    Sdl2Application and EmscriptenApplication. With my emulator
                    testing, where a mouse was interpreted as a stylus (?!),
                    multiple buttons being pressed didn't even trigger a press
                    or release event, so this scenario is seemingly impossible
                    to happen. */

                /* Assuming there's never more than 256 pointers in a single
                   event. Even that feels like a lot. */
                PointerEvent e{event, UnsignedByte(pointerChanged), sourcePointers.first(), pointer, primary, pointerId};
                press ? app.pointerPressEvent(e) : app.pointerReleaseEvent(e);

                /* Remember the currently pressed pointers for the next time */
                app._previousPressedButtons = pressedButtons;

                /* If this is a touch press, remember its position for next
                   events. If this is a touch release, free the slot used by
                   this identifier for next events. Mouse and pen supports
                   hover and thus is updated only in AMOTION_EVENT_ACTION_MOVE.
                   See below for why this has to be remembered at all. */
                if(sourcePointers.first() == PointerEventSource::Touch) {
                    if(press)
                        updatePreviousTouch(app._previousTouches, pointerId, position);
                    else
                        updatePreviousTouch(app._previousTouches, pointerId, {});
                }

                return e.isAccepted();
            }

            case AMOTION_EVENT_ACTION_MOVE: {
                const Pointers pressedButtons = motionEventButtons(event);

                /* Unlike AMOTION_EVENT_ACTION_DOWN / AMOTION_EVENT_ACTION_UP,
                   the move event can contain multiple moving pointers so
                   there's no mask telling which pointer moved. Go through all
                   and emit a move event only for those that changed. */
                bool accepted = false;
                const std::size_t pointerCount = AMotionEvent_getPointerCount(event);
                for(std::size_t i = 0; i != pointerCount; ++i) {
                    const Containers::Pair<PointerEventSource, Pointers> sourcePointers = motionEventPointers(event, i, pressedButtons);
                    const std::int32_t pointerId = pointerIdForSource(sourcePointers.first(), AMotionEvent_getPointerId(event, i));
                    const Vector2 position{AMotionEvent_getX(event, i),
                                           AMotionEvent_getY(event, i)};

                    /* Query position relative to the previous one for the same
                       pointer type and identifier, update it with current.
                       Ideally I would get it somewhere from the platform APIs.
                       There's AMotionEvent_getHistoricalX()/Y(), but those are
                       coalesced events between the previous and currently
                       fired events, i.e. not the full delta. Documented here:
                        https://developer.android.com/reference/android/view/MotionEvent#batching
                       There's also AMOTION_EVENT_AXIS_RELATIVE_X/_Y, but
                       according to
                        https://developer.android.com/reference/android/view/MotionEvent#AXIS_X
                       the coordinate system is different for each event type,
                       and the last thing I want to do is adding special
                       handling for things the damn platform API should be
                       doing for me. */
                    Vector2 relativePosition{NoInit};
                    if(sourcePointers.first() == PointerEventSource::Mouse ||
                       sourcePointers.first() == PointerEventSource::Pen) {
                        relativePosition = Math::isNan(app._previousHoverPointerPosition).all() ?
                            Vector2{} : position - app._previousHoverPointerPosition;
                        app._previousHoverPointerPosition = position;
                    } else if(sourcePointers.first() == PointerEventSource::Touch) {
                        relativePosition = updatePreviousTouch(app._previousTouches, pointerId, position);
                    } else {
                        /* No relative position for Unknown */
                        relativePosition = {};
                    }

                    /* Decide whether this is a primary pointer. Mouse and pen
                       is always a primary pointer. */
                    bool primary;
                    if(sourcePointers.first() == PointerEventSource::Mouse ||
                       sourcePointers.first() == PointerEventSource::Pen) {
                        primary = true;
                    /* For touch, it's a primary finger only if it was
                       registered as such during the last press. If the primary
                       finger was lifted, no other finger will step into its
                       place until all others are lifted as well. */
                    } else if(sourcePointers.first() == PointerEventSource::Touch) {
                        primary = app._primaryFingerId == pointerId;
                    /* Unknown pointer is probably not a primary one */
                    } else primary = false;

                    /* The thing fires move events right after press events,
                       with the exact same position, for (emulated?) events at
                       least. I suppose that's some sort of unasked-for
                       misfeature for "improving" UX or fixing broken apps. Not
                       interested, filter those out if the relative position is
                       zero and the set of pressed buttons is the same.
                       Hopefully not accepting those doesn't lead to some
                       strange behavior. */
                    if(relativePosition != Vector2{} || pressedButtons != app._previousPressedButtons) {
                        /* Assuming there's never more than 256 pointers in a
                           single event. Even that feels like a lot. */
                        PointerMoveEvent e{event, UnsignedByte(i), sourcePointers.first(), {}, sourcePointers.second(), primary, pointerId, relativePosition};
                        app.pointerMoveEvent(e);
                        accepted = accepted || e.isAccepted();
                    }
                }

                /* Remember the currently pressed buttons for the next time.
                   Ideally should only be needed for AMOTION_EVENT_ACTION_DOWN
                   and AMOTION_EVENT_ACTION_UP, but if some events get lost, we
                   have a chance to resynchronize here. */
                app._previousPressedButtons = pressedButtons;

                return accepted;
            }

            /* Like AMOTION_EVENT_ACTION_MOVE, but without anything pressed */
            case AMOTION_EVENT_ACTION_HOVER_MOVE: {
                /* Assuming there's just one pointer reported for a hover, and
                   it's either a mouse or a pen. Or something unknown. */
                CORRADE_INTERNAL_ASSERT(AMotionEvent_getPointerCount(event) == 1);
                PointerEventSource source;
                switch(AMotionEvent_getToolType(event, 0)) {
                    case AMOTION_EVENT_TOOL_TYPE_MOUSE:
                        source = AndroidApplication::PointerEventSource::Mouse;
                        break;
                    case AMOTION_EVENT_TOOL_TYPE_FINGER:
                        CORRADE_INTERNAL_ASSERT_UNREACHABLE();
                    case AMOTION_EVENT_TOOL_TYPE_STYLUS:
                    case AMOTION_EVENT_TOOL_TYPE_ERASER:
                        source = AndroidApplication::PointerEventSource::Pen;
                        break;
                    case AMOTION_EVENT_TOOL_TYPE_UNKNOWN:
                    default:
                        source = AndroidApplication::PointerEventSource::Unknown;
                        break;
                }

                const std::int32_t pointerId = pointerIdForSource(source, AMotionEvent_getPointerId(event, 0));
                const Vector2 position{AMotionEvent_getX(event, 0),
                                       AMotionEvent_getY(event, 0)};
                const Vector2 relativePosition =
                    Math::isNan(app._previousHoverPointerPosition).all() ?
                        Vector2{} : position - app._previousHoverPointerPosition;

                /* Similarly as with AMOTION_EVENT_ACTION_MOVE, the damn thing
                   fires hover events with zero position delta when scrolling
                   the mouse wheel. Useless, filter those away. */
                bool accepted = false;
                if(relativePosition != Vector2{}) {
                    PointerMoveEvent e{event, 0, source, {}, {}, true, pointerId, relativePosition};
                    app.pointerMoveEvent(e);
                    accepted = e.isAccepted();
                }

                /* Reset the currently pressed buttons, since there should be
                   none if we're just hovering */
                app._previousPressedButtons = {};
                /* Remember the current position. See above for why
                   AMotionEvent_getHistoricalX()/Y() is useless. */
                app._previousHoverPointerPosition = position;

                return accepted;
            }

            /** @todo there's AMOTION_EVENT_ACTION_HOVER_ENTER and
                AMOTION_EVENT_ACTION_HOVER_EXIT, implement once other apps get
                something similar */
            /** @todo AMOTION_EVENT_ACTION_SCROLL */
        }

    /** @todo Implement also other input events */
    }

    return 0;
}

AndroidApplication::GLConfiguration::GLConfiguration():
    _colorBufferSize{8, 8, 8, 8}, _depthBufferSize{24}, _stencilBufferSize{0} {}

void AndroidApplication::exec(android_app* state, Containers::Pointer<AndroidApplication>(*instancer)(const Arguments&)) {
    state->onAppCmd = commandEvent;
    state->onInputEvent = inputEvent;

    /* Long time ago there was a call to app_dummy() that prevented stripping
       the ANativeActivity_onCreate() symbol. It was awful enough on its own,
       but they decided that it's no longer needed and the PROPER AND BETTER
       SOLUTION is to pollute all downstream build scripts with
       `-u ANativeActivity_onCreate` passed to linker. That's just fucking
       awful. I can't use app_dummy() as it's deprecated, so I'm simply
       retrieving the function pointer to the ANativeActivity_onCreate function
       and saving it somewhere to convince the linker it's really needed. I
       WANT TO SCREAM. https://github.com/android-ndk/ndk/issues/381 */
    /** @todo Make use of saved state */
    Data data{instancer, ANativeActivity_onCreate};
    state->userData = &data;

    /* Poll for event until destroy is requested */
    while(!state->destroyRequested) {
        android_poll_source* source = nullptr;
        const int result = ALooper_pollOnce(
            data.instance && (data.instance->_flags & Flag::Redraw) ?
                0 : -1, /* negative value: wait indefinitely until an event appears */
            nullptr, nullptr, reinterpret_cast<void**>(&source));

        /** @todo shouldn't this be acted upon somehow? the change in
            https://github.com/android/ndk-samples/pull/1008 prints some error
            message, the docs at https://developer.android.com/ndk/reference/group/looper
            don't say anything useful as usual */
        if(result == ALOOPER_POLL_ERROR)
            return;

        /* Process this event OH SIR MAY MY POOR EXISTENCE CALL THIS FUNCTION
           FOR YOU IF YOU DON'T MIND? */
        if(source)
            source->process(state, source);

        /* Redraw the app if it wants to be redrawn. Frame limiting is done by
           Android itself */
        if(data.instance && (data.instance->_flags & Flag::Redraw))
            data.instance->drawEvent();
    }

    state->userData = nullptr;
}

void AndroidApplication::pointerPressEvent(PointerEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    if(!event.isPrimary())
        return;

    CORRADE_IGNORE_DEPRECATED_PUSH
    MouseEvent mouseEvent{event._event};
    mousePressEvent(mouseEvent);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void AndroidApplication::mousePressEvent(MouseEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void AndroidApplication::pointerReleaseEvent(PointerEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    if(!event.isPrimary())
        return;

    CORRADE_IGNORE_DEPRECATED_PUSH
    MouseEvent mouseEvent{event._event};
    mouseReleaseEvent(mouseEvent);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void AndroidApplication::mouseReleaseEvent(MouseEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void AndroidApplication::pointerMoveEvent(PointerMoveEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    if(!event.isPrimary())
        return;

    const Vector2i roundedPosition{Math::round(event.position())};

    /* If the event is due to some button being additionally pressed or one
       button from a larger set being released, delegate to a press/release
       event instead */
    /** @todo This codepath is never used as move events with pointer() being
        set aren't emitted at all above. Keeping it here as that may change
        with API 33+. */
    CORRADE_IGNORE_DEPRECATED_PUSH
    if(event.pointer()) {
        /* Android reports either a move or a press/release, so there shouldn't
           be any move in this case */
        CORRADE_INTERNAL_ASSERT(event.relativePosition() == Vector2{});
        MouseEvent mouseEvent{event._event};
        event.pointers() >= *event.pointer() ?
            mousePressEvent(mouseEvent) : mouseReleaseEvent(mouseEvent);
    } else {
        /* Can't do just Math::round(event.relativePosition()) because if the
           previous position was 4.6 and the new 5.3, they both round to 5 but
           the relativePosition is 0.6 and rounds to 1. Conversely, if it'd be
           5.3 and 5.6, the positions round to 5 and 6 but relative position
           stays 0. */
        const Vector2i previousRoundedPosition{Math::round(event.position() - event.relativePosition())};
        /* Call the event only if the integer values actually changed */
        if(roundedPosition != previousRoundedPosition) {
            MouseMoveEvent mouseEvent{event._event, roundedPosition - previousRoundedPosition};
            mouseMoveEvent(mouseEvent);
        }
    }
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void AndroidApplication::mouseMoveEvent(MouseMoveEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

template class BasicScreen<AndroidApplication>;
template class BasicScreenedApplication<AndroidApplication>;

}}
