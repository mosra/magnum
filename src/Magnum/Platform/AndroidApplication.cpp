/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024
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

AndroidApplication::Pointers motionEventPointers(AInputEvent* event, const AndroidApplication::Pointers pressedButtons) {
    switch(AMotionEvent_getToolType(event, 0)) {
        case AMOTION_EVENT_TOOL_TYPE_MOUSE:
            /** @todo MouseButton4 / MouseButton5, once they're added &
                tested */
            return (AndroidApplication::Pointer::MouseLeft|
                    AndroidApplication::Pointer::MouseMiddle|
                    AndroidApplication::Pointer::MouseRight) & pressedButtons;
        case AMOTION_EVENT_TOOL_TYPE_FINGER:
            return AndroidApplication::Pointer::Finger;
        case AMOTION_EVENT_TOOL_TYPE_STYLUS:
            /** @todo use pressedButtonsPointers once there's additional pen
                button enum values */
            return AndroidApplication::Pointer::Pen;
        case AMOTION_EVENT_TOOL_TYPE_ERASER:
            return AndroidApplication::Pointer::Eraser;
        case AMOTION_EVENT_TOOL_TYPE_UNKNOWN:
        default:
            return AndroidApplication::Pointer::Unknown;
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
            case AMOTION_EVENT_ACTION_UP: {
                const Vector2 position{AMotionEvent_getX(event, 0),
                                       AMotionEvent_getY(event, 0)};

                /* Query the currently pressed buttons. If this is not a mouse
                   event, it'll give back garbage, but that's fine as we won't
                   use it in that case. Then, based on whether it's a press or
                   a release, use the previously recorded pointers to figure
                   out what was actually pressed. */
                const Pointers pressedButtons = motionEventButtons(event);
                const Pointers pointers = motionEventPointers(event,
                    action == AMOTION_EVENT_ACTION_DOWN ?
                        pressedButtons & ~app._previousPressedButtons :
                        ~pressedButtons & app._previousPressedButtons);

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
                if(pointers && !(UnsignedByte(pointers) & (UnsignedByte(pointers) - 1)))
                    pointer = Pointer(UnsignedByte(pointers));
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
                PointerEvent e{event, pointer};
                action == AMOTION_EVENT_ACTION_DOWN ?
                    app.pointerPressEvent(e) : app.pointerReleaseEvent(e);

                /* Remember the currently pressed pointers for the next time */
                app._previousPressedButtons = pressedButtons;
                /* A touch screen doesn't have hover events, so remember the
                   position here as well. See below for why this has to be
                   remembered at all. */
                app._previousPointerPosition = position;

                return e.isAccepted();
            }

            case AMOTION_EVENT_ACTION_MOVE: {
                const Pointers pressedButtons = motionEventButtons(event);
                const Pointers pointers = motionEventPointers(event, pressedButtons);
                const Vector2 position{AMotionEvent_getX(event, 0),
                                       AMotionEvent_getY(event, 0)};
                const Vector2 relativePosition =
                    Math::isNan(app._previousPointerPosition).all() ?
                        Vector2{} : position - app._previousPointerPosition;

                /* The thing fires move events right after press events, with
                   the exact same position, for (emulated?) events at least. I
                   suppose that's some sort of unasked-for misfeature for
                   "improving" UX or fixing broken apps. Not interested, filter
                   those out if the relative position is zero and the set of
                   pressed buttons is the same. Hopefully not accepting those
                   doesn't lead to some strange behavior. */
                bool accepted = false;
                if(relativePosition != Vector2{} || pressedButtons != app._previousPressedButtons) {
                    PointerMoveEvent e{event, {}, pointers, relativePosition};
                    app.pointerMoveEvent(e);
                    accepted = e.isAccepted();
                }

                /* Remember the currently pressed buttons for the next time.
                   Ideally should only be needed for AMOTION_EVENT_ACTION_DOWN
                   and AMOTION_EVENT_ACTION_UP, but if some events get lost, we
                   have a chance to resynchronize here. */
                app._previousPressedButtons = pressedButtons;

                /* Remember also the current position. There's
                   AMotionEvent_getHistoricalX()/Y(), but those are coalesced
                   events between the previous and currently fired move events,
                   i.e. not the full delta. Documented here:
                    https://developer.android.com/reference/android/view/MotionEvent#batching
                   There's also AMOTION_EVENT_AXIS_RELATIVE_X/_Y, but based on
                    https://developer.android.com/reference/android/view/MotionEvent#AXIS_X
                   the coordinate system is different for each event type, and
                   the last thing I want to do is adding special handling for
                   things the damn platform API should be doing for me. */
                app._previousPointerPosition = position;

                return accepted;
            }

            /* Like AMOTION_EVENT_ACTION_MOVE, but without anything pressed */
            case AMOTION_EVENT_ACTION_HOVER_MOVE: {
                const Vector2 position{AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
                const Vector2 relativePosition =
                    Math::isNan(app._previousPointerPosition).all() ?
                        Vector2{} : position - app._previousPointerPosition;

                /* Similarly as with AMOTION_EVENT_ACTION_MOVE, the damn thing
                   fires hover events with zero position delta when scrolling
                   the mouse wheel. Useless, filter those away. */
                bool accepted = false;
                if(relativePosition != Vector2{}) {
                    PointerMoveEvent e{event, {}, {}, relativePosition};
                    app.pointerMoveEvent(e);
                    accepted = e.isAccepted();
                }

                /* Reset the currently pressed buttons, since there should be
                   none if we're just hovering */
                app._previousPressedButtons = {};
                /* Remember the current position. See above for why
                   AMotionEvent_getHistoricalX()/Y() is useless. */
                app._previousPointerPosition = position;

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

    for(;;) {
        /* Read all pending events. Block and wait for them only if the app
           doesn't want to redraw immediately WHY THIS GODDAMN THING DOESNT
           HAVE SOMETHING LIKE WAIT FOR EVENT SO I NEED TO TANGLE THIS TANGLED
           MESS OF HELL */
        int ident, events;
        android_poll_source* source;
        while((ident = ALooper_pollAll(
            data.instance && (data.instance->_flags & Flag::Redraw) ? 0 : -1,
            nullptr, &events, reinterpret_cast<void**>(&source))) >= 0)
        {
            /* Process this event OH SIR MAY MY POOR EXISTENCE CALL THIS
               FUNCTION FOR YOU IF YOU DON'T MIND? */
            if(source) source->process(state, source);

            /* Exit WHY THIS HAS TO BE HANDLED HERE WHILE EVERY OTHER THING
               IS HANDLED THROUGH CALLBACK GODDAMMIT */
            if(state->destroyRequested != 0) return;
        }

        /* Redraw the app if it wants to be redrawn. Frame limiting is done by
           Android itself */
        if(data.instance && (data.instance->_flags & Flag::Redraw))
            data.instance->drawEvent();
    }

    state->userData = nullptr;
}

void AndroidApplication::pointerPressEvent(PointerEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
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
