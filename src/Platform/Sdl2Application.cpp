/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#ifdef CORRADE_TARGET_EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

#include "Context.h"
#include "Platform/ScreenedApplication.hpp"

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
Sdl2Application* Sdl2Application::instance = nullptr;
void Sdl2Application::staticMainLoop() {
    instance->mainLoop();
}
#endif

/** @todo Delegating constructor when support for GCC 4.6 is dropped */

Sdl2Application::Sdl2Application(const Arguments&, const Configuration& configuration): context(nullptr), flags(Flag::Redraw) {
    initialize();
    createContext(configuration);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Sdl2Application::Sdl2Application(const Arguments&): context(nullptr), flags(Flag::Redraw) {
    initialize();
    createContext({});
}
#endif

Sdl2Application::Sdl2Application(const Arguments&, std::nullptr_t): context(nullptr), flags(Flag::Redraw) {
    initialize();
}

void Sdl2Application::initialize() {
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_ASSERT(!instance, "Platform::Sdl2Application::Sdl2Application(): the instance is already created", );
    instance = this;
    #endif

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error() << "Cannot initialize SDL.";
        std::exit(1);
    }
}

void Sdl2Application::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool Sdl2Application::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(!context, "Platform::Sdl2Application::tryCreateContext(): context already created", false);

    /* Enable double buffering and 24bt depth buffer */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    /* Multisampling */
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, configuration.sampleCount() > 1 ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, configuration.sampleCount());

    /* Flags: if not hidden, set as shown */
    Uint32 flags(configuration.flags());
    if(!(configuration.flags() & Configuration::Flag::Hidden)) flags |= SDL_WINDOW_SHOWN;

    /** @todo Remove when Emscripten has proper SDL2 support */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    if(!(window = SDL_CreateWindow(configuration.title().data(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            configuration.size().x(), configuration.size().y(),
            SDL_WINDOW_OPENGL|flags))) {
        Error() << "Platform::Sdl2Application::tryCreateContext(): cannot create window:" << SDL_GetError();
        return false;
    }

    if(!(context = SDL_GL_CreateContext(window))) {
        Error() << "Platform::Sdl2Application::tryCreateContext(): cannot create context:" << SDL_GetError();
        SDL_DestroyWindow(window);
        window = nullptr;
        return false;
    }
    #else
    context = SDL_SetVideoMode(configuration.size().x(), configuration.size().y(), 24, SDL_OPENGL|SDL_HWSURFACE|SDL_DOUBLEBUF);
    #endif

    c = new Context;
    return true;
}

void Sdl2Application::swapBuffers() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_GL_SwapWindow(window);
    #else
    SDL_Flip(context);
    #endif
}

Sdl2Application::~Sdl2Application() {
    delete c;

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    #else
    SDL_FreeSurface(context);
    CORRADE_INTERNAL_ASSERT(instance == this);
    instance = nullptr;
    #endif
    SDL_Quit();
}

int Sdl2Application::exec() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    while(!(flags & Flag::Exit)) mainLoop();
    #else
    emscripten_set_main_loop(staticMainLoop, 0, true);
    #endif
    return 0;
}

void Sdl2Application::exit() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    flags |= Flag::Exit;
    #else
    emscripten_cancel_main_loop();
    #endif
}

void Sdl2Application::mainLoop() {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_WINDOWEVENT:
                switch(event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        viewportEvent({event.window.data1, event.window.data2});
                        flags |= Flag::Redraw;
                        break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        flags |= Flag::Redraw;
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
                    MouseEvent e(event.wheel.y < 0 ? MouseEvent::Button::WheelUp : MouseEvent::Button::WheelDown, {event.wheel.x, event.wheel.y});
                    mousePressEvent(e);
                } break;

            case SDL_MOUSEMOTION: {
                MouseMoveEvent e({event.motion.x, event.motion.y}, {event.motion.xrel, event.motion.yrel}, static_cast<MouseMoveEvent::Button>(event.motion.state));
                mouseMoveEvent(e);
                break;
            }

            case SDL_QUIT:
                #ifndef CORRADE_TARGET_EMSCRIPTEN
                flags |= Flag::Exit;
                #else
                emscripten_cancel_main_loop();
                #endif
                return;
        }
    }

    if(flags & Flag::Redraw) {
        flags &= ~Flag::Redraw;
        drawEvent();
        return;
    }

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_WaitEvent(nullptr);
    #endif
}

void Sdl2Application::setMouseLocked(bool enabled) {
    /** @todo Implement this in Emscripten */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_SetWindowGrab(window, enabled ? SDL_TRUE : SDL_FALSE);
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
    #else
    CORRADE_ASSERT(false, "Sdl2Application::setMouseLocked(): not implemented", );
    static_cast<void>(enabled);
    #endif
}

void Sdl2Application::keyPressEvent(KeyEvent&) {}
void Sdl2Application::keyReleaseEvent(KeyEvent&) {}
void Sdl2Application::mousePressEvent(MouseEvent&) {}
void Sdl2Application::mouseReleaseEvent(MouseEvent&) {}
void Sdl2Application::mouseMoveEvent(MouseMoveEvent&) {}

Sdl2Application::Configuration::Configuration(): _title("Magnum SDL2 Application"), _size(800, 600), _flags(Flag::Resizable), _sampleCount(0) {}
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
