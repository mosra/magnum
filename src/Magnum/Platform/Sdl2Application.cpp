/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "Sdl2Application.h"

#include <cstring>
#ifndef CORRADE_TARGET_EMSCRIPTEN
#include <tuple>
#else
#include <emscripten/emscripten.h>
#endif

#include "Magnum/Platform/Context.h"
#include "Magnum/Version.h"
#include "Magnum/Platform/ScreenedApplication.hpp"

namespace Magnum { namespace Platform {

namespace {

/*
 * Fix up the modifiers -- we want >= operator to work properly on Shift,
 * Ctrl, Alt, but SDL generates different event for left / right keys, thus
 * (modifiers >= Shift) would pass only if both left and right were pressed,
 * which is usually not what the developers wants.
 */
Sdl2Application::InputEvent::Modifiers fixedModifiers(Uint16 mod) {
    Sdl2Application::InputEvent::Modifiers modifiers(static_cast<Sdl2Application::InputEvent::Modifier>(mod));
    if(modifiers & Sdl2Application::InputEvent::Modifier::Shift) modifiers |= Sdl2Application::InputEvent::Modifier::Shift;
    if(modifiers & Sdl2Application::InputEvent::Modifier::Ctrl) modifiers |= Sdl2Application::InputEvent::Modifier::Ctrl;
    if(modifiers & Sdl2Application::InputEvent::Modifier::Alt) modifiers |= Sdl2Application::InputEvent::Modifier::Alt;
    return modifiers;
}

}

#ifdef CORRADE_TARGET_EMSCRIPTEN
Sdl2Application* Sdl2Application::_instance = nullptr;
void Sdl2Application::staticMainLoop() {
    _instance->mainLoop();
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Sdl2Application::Sdl2Application(const Arguments& arguments): Sdl2Application{arguments, Configuration{}} {}
#endif

Sdl2Application::Sdl2Application(const Arguments& arguments, const Configuration& configuration): Sdl2Application{arguments, nullptr} {
    createContext(configuration);
}

Sdl2Application::Sdl2Application(const Arguments& arguments, std::nullptr_t): _glContext{nullptr},
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    _minimalLoopPeriod{0},
    #endif
    _context{new Context{NoCreate, arguments.argc, arguments.argv}}, _flags{Flag::Redraw}
{
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_ASSERT(!_instance, "Platform::Sdl2Application::Sdl2Application(): the instance is already created", );
    _instance = this;
    #endif

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error() << "Cannot initialize SDL.";
        std::exit(1);
    }
}

void Sdl2Application::createContext() { createContext({}); }

void Sdl2Application::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool Sdl2Application::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == Version::None, "Platform::Sdl2Application::tryCreateContext(): context already created", false);

    /* Enable double buffering and 24bt depth buffer */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    /* Multisampling */
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, configuration.sampleCount() > 1 ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, configuration.sampleCount());

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Context flags */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(configuration.flags()));
    #endif

    /* Flags: if not hidden, set as shown */
    Uint32 windowFlags(configuration.windowFlags());
    if(!(configuration.windowFlags() & Configuration::WindowFlag::Hidden))
        windowFlags |= SDL_WINDOW_SHOWN;

    /** @todo Remove when Emscripten has proper SDL2 support */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Set context version, if user-specified */
    if(configuration.version() != Version::None) {
        Int major, minor;
        std::tie(major, minor) = version(configuration.version());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);

        #ifndef MAGNUM_TARGET_GLES
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, configuration.version() >= Version::GL310 ?
            SDL_GL_CONTEXT_PROFILE_CORE : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        #endif

    /* Request usable version otherwise */
    } else {
        #ifndef MAGNUM_TARGET_GLES
        /* First try to create core context. This is needed mainly on OS X and
           Mesa, as support for recent OpenGL versions isn't implemented in
           compatibility contexts (which are the default). At least GL 3.2 is
           needed on OSX, at least GL 3.1 is needed on Mesa. Bite the bullet
           and try 3.1 also elsewhere. */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        #ifdef CORRADE_TARGET_APPLE
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        #endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        #else
        /* For ES the major context version is compile-time constant */
        #ifdef MAGNUM_TARGET_GLES3
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        #elif defined(MAGNUM_TARGET_GLES2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        #else
        #error unsupported OpenGL ES version
        #endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        #endif
    }

    /* Create window */
    if(!(_window = SDL_CreateWindow(configuration.title().data(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        configuration.size().x(), configuration.size().y(),
        SDL_WINDOW_OPENGL|windowFlags)))
    {
        Error() << "Platform::Sdl2Application::tryCreateContext(): cannot create window:" << SDL_GetError();
        return false;
    }

    /* Create context */
    _glContext = SDL_GL_CreateContext(_window);

    #ifndef MAGNUM_TARGET_GLES
    /* Fall back to (forward compatible) GL 2.1, if version is not
       user-specified and either core context creation fails or we are on
       binary NVidia/AMD drivers on Linux/Windows. Instead of creating forward-
       compatible context with highest available version, they force the
       version to the one specified, which is completely useless behavior. */
    #ifndef CORRADE_TARGET_APPLE
    constexpr static const char nvidiaVendorString[] = "NVIDIA Corporation";
    constexpr static const char amdVendorString[] = "ATI Technologies Inc.";
    const char* vendorString;
    #endif
    if(configuration.version() == Version::None && (!_glContext
        #ifndef CORRADE_TARGET_APPLE
        /* Sorry about the UGLY code, HOPEFULLY THERE WON'T BE MORE WORKAROUNDS */
        || (vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
        (std::strncmp(vendorString, nvidiaVendorString, sizeof(nvidiaVendorString)) == 0 ||
         std::strncmp(vendorString, amdVendorString, sizeof(amdVendorString)) == 0))
        #endif
    )) {
        /* Don't print any warning when doing the NV workaround, because the
           bug will be there probably forever */
        if(!_glContext) Warning()
            << "Platform::Sdl2Application::tryCreateContext(): cannot create core context:"
            << SDL_GetError() << "(falling back to compatibility context)";
        else SDL_GL_DeleteContext(_glContext);

        SDL_DestroyWindow(_window);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);

        if(!(_window = SDL_CreateWindow(configuration.title().data(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            configuration.size().x(), configuration.size().y(),
            SDL_WINDOW_OPENGL|windowFlags)))
        {
            Error() << "Platform::Sdl2Application::tryCreateContext(): cannot create window:" << SDL_GetError();
            return false;
        }

        /* Create compatibility context */
        _glContext = SDL_GL_CreateContext(_window);
    }
    #endif

    /* Cannot create context (or fallback compatibility context on desktop) */
    if(!_glContext) {
        Error() << "Platform::Sdl2Application::tryCreateContext(): cannot create context:" << SDL_GetError();
        SDL_DestroyWindow(_window);
        _window = nullptr;
        return false;
    }

    #else
    /* Emscripten-specific initialization */
    _glContext = SDL_SetVideoMode(configuration.size().x(), configuration.size().y(), 24, SDL_OPENGL|SDL_HWSURFACE|SDL_DOUBLEBUF);
    #endif

    /* Return true if the initialization succeeds */
    return _context->tryCreate();
}

void Sdl2Application::swapBuffers() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_GL_SwapWindow(_window);
    #else
    SDL_Flip(_glContext);
    #endif
}

Int Sdl2Application::swapInterval() const {
    return SDL_GL_GetSwapInterval();
}

bool Sdl2Application::setSwapInterval(const Int interval) {
    if(SDL_GL_SetSwapInterval(interval) == -1) {
        Error() << "Platform::Sdl2Application::setSwapInterval(): cannot set swap interval:" << SDL_GetError();
        _flags &= ~Flag::VSyncEnabled;
        return false;
    }

    if(SDL_GL_GetSwapInterval() != interval) {
        Error() << "Platform::Sdl2Application::setSwapInterval(): swap interval setting ignored by the driver";
        _flags &= ~Flag::VSyncEnabled;
        return false;
    }

    _flags |= Flag::VSyncEnabled;
    return true;
}

Sdl2Application::~Sdl2Application() {
    _context.reset();

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_GL_DeleteContext(_glContext);
    SDL_DestroyWindow(_window);
    #else
    SDL_FreeSurface(_glContext);
    CORRADE_INTERNAL_ASSERT(_instance == this);
    _instance = nullptr;
    #endif
    SDL_Quit();
}

int Sdl2Application::exec() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    while(!(_flags & Flag::Exit)) mainLoop();
    #else
    emscripten_set_main_loop(staticMainLoop, 0, true);
    #endif
    return 0;
}

void Sdl2Application::exit() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    _flags |= Flag::Exit;
    #else
    emscripten_cancel_main_loop();
    #endif
}

void Sdl2Application::mainLoop() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    const UnsignedInt timeBefore = _minimalLoopPeriod ? SDL_GetTicks() : 0;
    #endif

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_WINDOWEVENT:
                switch(event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        viewportEvent({event.window.data1, event.window.data2});
                        _flags |= Flag::Redraw;
                        break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        _flags |= Flag::Redraw;
                        break;
                } break;

            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                KeyEvent e(static_cast<KeyEvent::Key>(event.key.keysym.sym), fixedModifiers(event.key.keysym.mod));
                event.type == SDL_KEYDOWN ? keyPressEvent(e) : keyReleaseEvent(e);
            } break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                MouseEvent e(static_cast<MouseEvent::Button>(event.button.button), {event.button.x, event.button.y});
                event.type == SDL_MOUSEBUTTONDOWN ? mousePressEvent(e) : mouseReleaseEvent(e);
            } break;

            case SDL_MOUSEWHEEL:
                if(event.wheel.y != 0) {
                    MouseEvent e(event.wheel.y > 0 ? MouseEvent::Button::WheelUp : MouseEvent::Button::WheelDown, {event.wheel.x, event.wheel.y});
                    mousePressEvent(e);
                } break;

            case SDL_MOUSEMOTION: {
                MouseMoveEvent e({event.motion.x, event.motion.y}, {event.motion.xrel, event.motion.yrel}, static_cast<MouseMoveEvent::Button>(event.motion.state));
                mouseMoveEvent(e);
                break;
            }

            case SDL_QUIT:
                #ifndef CORRADE_TARGET_EMSCRIPTEN
                _flags |= Flag::Exit;
                #else
                emscripten_cancel_main_loop();
                #endif
                return;
        }
    }

    /* Tick event */
    if(!(_flags & Flag::NoTickEvent)) tickEvent();

    /* Draw event */
    if(_flags & Flag::Redraw) {
        _flags &= ~Flag::Redraw;
        drawEvent();

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /* If VSync is not enabled, delay to prevent CPU hogging (if set) */
        if(!(_flags & Flag::VSyncEnabled) && _minimalLoopPeriod) {
            const UnsignedInt loopTime = SDL_GetTicks() - timeBefore;
            if(loopTime < _minimalLoopPeriod)
                SDL_Delay(_minimalLoopPeriod - loopTime);
        }
        #endif

        return;
    }

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* If not drawing anything, delay to prevent CPU hogging (if set) */
    if(_minimalLoopPeriod) {
        const UnsignedInt loopTime = SDL_GetTicks() - timeBefore;
        if(loopTime < _minimalLoopPeriod)
            SDL_Delay(_minimalLoopPeriod - loopTime);
    }

    /* Then, if the tick event doesn't need to be called periodically, wait
       indefinitely for next input event */
    if(_flags & Flag::NoTickEvent) SDL_WaitEvent(nullptr);
    #endif
}

void Sdl2Application::setMouseLocked(bool enabled) {
    /** @todo Implement this in Emscripten */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_SetWindowGrab(_window, enabled ? SDL_TRUE : SDL_FALSE);
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
    #else
    CORRADE_ASSERT(false, "Sdl2Application::setMouseLocked(): not implemented", );
    static_cast<void>(enabled);
    #endif
}

void Sdl2Application::tickEvent() {
    /* If this got called, the tick event is not implemented by user and thus
       we don't need to call it ever again */
    _flags |= Flag::NoTickEvent;
}

void Sdl2Application::viewportEvent(const Vector2i&) {}
void Sdl2Application::keyPressEvent(KeyEvent&) {}
void Sdl2Application::keyReleaseEvent(KeyEvent&) {}
void Sdl2Application::mousePressEvent(MouseEvent&) {}
void Sdl2Application::mouseReleaseEvent(MouseEvent&) {}
void Sdl2Application::mouseMoveEvent(MouseMoveEvent&) {}

Sdl2Application::Configuration::Configuration():
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    _title("Magnum SDL2 Application"),
    #endif
    _size(800, 600), _windowFlags{}, _sampleCount(0)
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    , _version(Version::None)
    #endif
    {}

Sdl2Application::Configuration::~Configuration() = default;

Sdl2Application::InputEvent::Modifiers Sdl2Application::MouseEvent::modifiers() {
    if(modifiersLoaded) return _modifiers;
    modifiersLoaded = true;
    return _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
}

Sdl2Application::InputEvent::Modifiers Sdl2Application::MouseMoveEvent::modifiers() {
    if(modifiersLoaded) return _modifiers;
    modifiersLoaded = true;
    return _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
}

template class BasicScreen<Sdl2Application>;
template class BasicScreenedApplication<Sdl2Application>;

}}
