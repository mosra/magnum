#ifndef Magnum_Contexts_Sdl2Context_h
#define Magnum_Contexts_Sdl2Context_h
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
 * @brief Class Magnum::Contexts::Sdl2Context
 */

#include "Magnum.h"
#include <SDL.h>

#include "AbstractContext.h"
#include <SDL2/SDL_scancode.h>

namespace Magnum { namespace Contexts {

/** @nosubgrouping
@brief SDL2 context

Currently doesn't have any mouse/keyboard handling.

You need to implement at least drawEvent() and viewportEvent() to be able to
draw on the screen.
*/
class Sdl2Context: public AbstractContext {
    public:
        /**
         * @brief Constructor
         * @param argc      Count of arguments of `main()` function
         * @param argv      Arguments of `main()` function
         * @param title     Window title
         * @param size      Window size
         *
         * Creates centered non-resizable window with double-buffered
         * OpenGL 3.3 context with 24bit depth buffer.
         */
        Sdl2Context(int argc, char** argv, const std::string& title = "Magnum SDL2 context", const Math::Vector2<GLsizei>& size = Math::Vector2<GLsizei>(800, 600));

        /**
         * @brief Destructors
         *
         * Deletes context and destroys the window.
         */
        ~Sdl2Context();

        int exec();

        /** @{ @name Drawing functions */

    protected:
        /** @copydoc GlutContext::viewportEvent() */
        virtual void viewportEvent(const Math::Vector2<GLsizei>& size) = 0;

        /** @copydoc GlutContext::drawEvent() */
        virtual void drawEvent() = 0;

        /** @copydoc GlutContext::swapBuffers() */
        inline void swapBuffers() { SDL_GL_SwapWindow(window); }

        /** @copydoc GlutContext::redraw() */
        inline void redraw() { _redraw = true; }

        /*@}*/

    private:
        SDL_Window* window;
        SDL_GLContext context;

        bool _redraw;
};

}}

#endif
