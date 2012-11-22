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

Sdl2Application::Sdl2Application(int, char**, const std::string& name, const Math::Vector2<GLsizei>& size): _redraw(true) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error() << "Cannot initialize SDL.";
        exit(1);
    }

    /* Enable double buffering and 24bt depth buffer */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        size.x(), size.y(), SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(!window) {
        Error() << "Cannot create window.";
        exit(2);
    }

    context = SDL_GL_CreateContext(window);

    /* This must be enabled, otherwise (on my NVidia) it crashes when creating
       VAO. WTF. */
    ExtensionWrangler::initialize(ExtensionWrangler::ExperimentalFeatures::Enable);

    /* Push resize event, so viewportEvent() is called at startup */
    SDL_Event* sizeEvent = new SDL_Event;
    sizeEvent->type = SDL_WINDOWEVENT;
    sizeEvent->window.event = SDL_WINDOWEVENT_RESIZED;
    sizeEvent->window.data1 = size.x();
    sizeEvent->window.data2 = size.y();
    SDL_PushEvent(sizeEvent);

    c = new Context;
}

Sdl2Application::~Sdl2Application() {
    delete c;

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int Sdl2Application::exec() {
    for(;;) {
        SDL_Event event;

        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_WINDOWEVENT:
                    switch(event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            viewportEvent({event.window.data1, event.window.data2});
                            _redraw = true;
                            break;
                        case SDL_WINDOWEVENT_EXPOSED:
                            _redraw = true;
                            break;
                    } break;

                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    KeyEvent e(static_cast<KeyEvent::Key>(event.key.keysym.sym), fixedModifiers(event.key.keysym.mod));
                    event.type == SDL_KEYDOWN ? keyPressEvent(e) : keyReleaseEvent(e);
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP: {
                    MouseEvent e(static_cast<MouseEvent::Button>(event.button.button), {event.button.x, event.button.y});
                    event.type == SDL_MOUSEBUTTONDOWN ? mousePressEvent(e) : mouseReleaseEvent(e);
                    break;
                }

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

                case SDL_QUIT: return 0;
            }
        }

        if(_redraw) {
            _redraw = false;
            drawEvent();
        } else Corrade::Utility::sleep(5);
    }

    return 0;
}

void Sdl2Application::setMouseLocked(bool enabled) {
    SDL_SetWindowGrab(window, enabled ? SDL_TRUE : SDL_FALSE);
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
}

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
