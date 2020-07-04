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

#include "AndroidApplication.h"

#include <Corrade/Utility/AndroidLogStreamBuffer.h>
#include <Corrade/Utility/Debug.h>
#include <android_native_app_glue.h>

#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"
#include "Magnum/Platform/ScreenedApplication.hpp"

#include "Implementation/Egl.h"

/* This function would be stripped by the linker otherwise. Search for the name
   below for the complete rant. */
extern "C" void ANativeActivity_onCreate(struct ANativeActivity*, void*, size_t);

namespace Magnum { namespace Platform {

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

AndroidApplication::AndroidApplication(const Arguments& arguments, NoCreateT): _state{arguments}, _context{new GLContext{NoCreate, 0, nullptr}} {
    /* Redirect debug output to Android log */
    _logOutput.reset(new LogOutput);
}

AndroidApplication::~AndroidApplication() {
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
        #elif defined(MAGNUM_TARGET_GLES3)
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
    return _context->tryCreate();
}

Vector2i AndroidApplication::framebufferSize() const {
    return {ANativeWindow_getWidth(_state->window),
            ANativeWindow_getHeight(_state->window)};
}

void AndroidApplication::swapBuffers() {
    eglSwapBuffers(_display, _surface);
}

void AndroidApplication::viewportEvent(ViewportEvent& event) {
    static_cast<void>(event);
}

void AndroidApplication::mousePressEvent(MouseEvent&) {}
void AndroidApplication::mouseReleaseEvent(MouseEvent&) {}
void AndroidApplication::mouseMoveEvent(MouseMoveEvent&) {}

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

std::int32_t AndroidApplication::inputEvent(android_app* state, AInputEvent* event) {
    CORRADE_INTERNAL_ASSERT(static_cast<Data*>(state->userData)->instance);
    AndroidApplication& app = *static_cast<Data*>(state->userData)->instance;
    if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        const std::int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
        switch(action) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_UP: {
                /* On a touch screen move events aren't reported when the
                   finger is moving above (of course), so remember the position
                   always */
                app._previousMouseMovePosition = {Int(AMotionEvent_getX(event, 0)), Int(AMotionEvent_getY(event, 0))};
                MouseEvent e(event);
                action == AMOTION_EVENT_ACTION_DOWN ? app.mousePressEvent(e) : app.mouseReleaseEvent(e);
                return e.isAccepted() ? 1 : 0;
            }

            case AMOTION_EVENT_ACTION_MOVE: {
                Vector2i position{Int(AMotionEvent_getX(event, 0)), Int(AMotionEvent_getY(event, 0))};
                MouseMoveEvent e{event,
                    app._previousMouseMovePosition == Vector2i{-1} ? Vector2i{} :
                    position - app._previousMouseMovePosition};
                app._previousMouseMovePosition = position;
                app.mouseMoveEvent(e);
                return e.isAccepted() ? 1 : 0;
            }
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

template class BasicScreen<AndroidApplication>;
template class BasicScreenedApplication<AndroidApplication>;

}}
