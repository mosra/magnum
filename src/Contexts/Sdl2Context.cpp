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

#include "Sdl2Context.h"

namespace Magnum { namespace Contexts {

Sdl2Context::Sdl2Context(int argc, char** argv, const std::string& name, const Math::Vector2<GLsizei>& size): _redraw(true) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error() << "Cannot initialize SDL.";
        exit(1);
    }

    /* Request OpenGL 3.3 */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

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

    #ifndef MAGNUM_TARGET_GLES
    /* This must be enabled, otherwise (on my NVidia) it crashes when creating
       VAO. WTF. */
    glewExperimental = true;

    /* Init GLEW */
    GLenum err = glewInit();
    if(err != GLEW_OK) {
        Error() << "Sdl2Context: cannot initialize GLEW:" << glewGetErrorString(err);
        exit(1);
    }
    #endif

    /* Push resize event, so viewportEvent() is called at startup */
    SDL_Event* sizeEvent = new SDL_Event;
    sizeEvent->type = SDL_WINDOWEVENT;
    sizeEvent->window.event = SDL_WINDOWEVENT_RESIZED;
    sizeEvent->window.data1 = size.x();
    sizeEvent->window.data2 = size.y();
    SDL_PushEvent(sizeEvent);
}

Sdl2Context::~Sdl2Context() {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int Sdl2Context::exec() {
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
                    keyPressEvent(static_cast<Key>(event.key.keysym.sym), event.key.repeat);
                    break;
                case SDL_KEYUP:
                    keyReleaseEvent(static_cast<Key>(event.key.keysym.sym));
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    mouseEvent(static_cast<MouseButton>(event.button.button),
                               static_cast<MouseState>(event.button.state),
                               {event.button.x, event.button.y});
                    break;
                case SDL_MOUSEWHEEL:
                    mouseWheelEvent({event.wheel.x, event.wheel.y});
                    break;
                case SDL_MOUSEMOTION:
                    mouseMotionEvent({event.motion.x, event.motion.y},
                                     {event.motion.xrel, event.motion.yrel});
                    break;
                case SDL_QUIT:
                    return 0;
            }
        }

        if(_redraw) {
            drawEvent();
            _redraw = false;
        } else Corrade::Utility::sleep(5);
    }

    return 0;
}

}}
