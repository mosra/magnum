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

#include "Context.h"
#include "ExtensionWrangler.h"

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

Sdl2Application::Sdl2Application(const Arguments&): context(nullptr), flags(Flag::Redraw) {
    initialize();
    createContext(new Configuration);
}

Sdl2Application::Sdl2Application(const Arguments&, Configuration* configuration): context(nullptr), flags(Flag::Redraw) {
    initialize();
    if(configuration) createContext(configuration);
}

void Sdl2Application::initialize() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error() << "Cannot initialize SDL.";
        std::exit(1);
    }
}

void Sdl2Application::createContext(Configuration* configuration) {
    if(!tryCreateContext(configuration)) {
        Error() << "Platform::Sdl2Application::createContext(): cannot create context:" << SDL_GetError();
        delete configuration;
        std::exit(1);

    } else delete configuration;
}

bool Sdl2Application::tryCreateContext(Configuration* configuration) {
    CORRADE_ASSERT(!context, "Platform::Sdl2Application::tryCreateContext(): context already created", false);

    /* Enable double buffering and 24bt depth buffer */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    /* Multisampling */
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, configuration->sampleCount() > 1 ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, configuration->sampleCount());

    /* Flags: if not hidden, set as shown */
    Uint32 flags(configuration->flags());
    if(!(configuration->flags() & Configuration::Flag::Hidden)) flags |= SDL_WINDOW_SHOWN;

    if(!(window = SDL_CreateWindow(configuration->title().c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            configuration->size().x(), configuration->size().y(),
            SDL_WINDOW_OPENGL|flags)))
        return false;

    if(!(context = SDL_GL_CreateContext(window))) {
        SDL_DestroyWindow(window);
        window = nullptr;
        return false;
    }

    /* This must be enabled, otherwise (on my NVidia) it crashes when creating
       VAO. WTF. */
    ExtensionWrangler::initialize(ExtensionWrangler::ExperimentalFeatures::Enable);

    /* Push resize event, so viewportEvent() is called at startup */
    SDL_Event* sizeEvent = new SDL_Event;
    sizeEvent->type = SDL_WINDOWEVENT;
    sizeEvent->window.event = SDL_WINDOWEVENT_RESIZED;
    sizeEvent->window.data1 = configuration->size().x();
    sizeEvent->window.data2 = configuration->size().y();
    SDL_PushEvent(sizeEvent);

    c = new Context;
    return true;
}

Sdl2Application::~Sdl2Application() {
    delete c;

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int Sdl2Application::exec() {
    while(!(flags & Flag::Exit)) mainLoop();
    return 0;
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
                MouseMoveEvent e({event.motion.x, event.motion.y}, {event.motion.xrel, event.motion.yrel});
                mouseMoveEvent(e);
                break;
            }

            case SDL_QUIT:
                flags |= Flag::Exit;
                return;
        }
    }

    if(flags & Flag::Redraw) {
        flags &= ~Flag::Redraw;
        drawEvent();
    } else SDL_WaitEvent(nullptr);
}

void Sdl2Application::setMouseLocked(bool enabled) {
    SDL_SetWindowGrab(window, enabled ? SDL_TRUE : SDL_FALSE);
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
}

Sdl2Application::Configuration::Configuration(): _title("Magnum SDL2 Application"), _size(800, 600), _flags(Flag::Resizable), _sampleCount(0) {}
Sdl2Application::Configuration::~Configuration() = default;

Sdl2Application::InputEvent::Modifiers Sdl2Application::MouseEvent::modifiers() {
    if(modifiersLoaded) return _modifiers;
    modifiersLoaded = true;
    return _modifiers = fixedModifiers(SDL_GetModState());
}

Sdl2Application::InputEvent::Modifiers Sdl2Application::MouseMoveEvent::modifiers() {
    if(modifiersLoaded) return _modifiers;
    modifiersLoaded = true;
    return _modifiers = fixedModifiers(SDL_GetModState());
}

}}
