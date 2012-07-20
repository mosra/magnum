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

Supports keyboard and mouse handling.

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
         * @brief Destructor
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

        /** @{ @name Keyboard handling */
    public:
        /**
         * @brief Key
         * @see keyPressEvent(), keyReleaseEvent()
         */
        enum class Key: SDL_Keycode {
            Up = SDLK_UP,                   /**< Up arrow */
            Down = SDLK_DOWN,               /**< Down arrow */
            Left = SDLK_LEFT,               /**< Left arrow */
            Right = SDLK_RIGHT,             /**< Right arrow */
            Plus = SDLK_PLUS,               /**< Plus */
            Minus = SDLK_MINUS              /**< Minus */
        };

    protected:
        /**
         * @brief Key press event
         * @param key       Key pressed
         * @param repeat    Non-zero if this is a key repeat
         */
        virtual void keyPressEvent(Key key, Uint8 repeat);

        /**
         * @brief Key release event
         * @param key       Key release
         */
        virtual void keyReleaseEvent(Key key);

        /*@}*/

        /** @{ @name Mouse handling */

    public:
        /**
         * @brief Mouse button
         * @see mouseEvent()
         */
        enum class MouseButton: Uint8 {
            Left = SDL_BUTTON_LEFT,         /**< Left button */
            Middle = SDL_BUTTON_MIDDLE,     /**< Middle button */
            Right = SDL_BUTTON_RIGHT        /**< Right button */
        };

        /**
         * @brief Mouse state
         * @see mouseEvent()
         */
        enum class MouseState: Uint8 {
            Pressed = SDL_PRESSED,          /**< Button pressed */
            Released = SDL_RELEASED         /**< Button released */
        };

    protected:
        /**
         * @brief Mouse event
         * @param button    Mouse button
         * @param state     Mouse state
         * @param position  Mouse position relative to the window
         *
         * Called when mouse button is pressed or released. Default
         * implementation does nothing.
         */
        virtual void mouseEvent(MouseButton button, MouseState state, const Math::Vector2<int>& position);

        /**
         * @brief Mouse wheel event
         * @param direction Wheel rotation direction. Negative Y is up and
         *      positive X is right.
         *
         * Called when mouse wheel is rotated. Default implementation does
         * nothing.
         */
        virtual void mouseWheelEvent(const Math::Vector2<int>& direction);

        /**
         * @brief Mouse motion event
         * @param position  Mouse position relative to the window
         * @param delta     Mouse position relative to last motion event
         *
         * Called when mouse is moved. Default implementation does nothing.
         */
        virtual void mouseMotionEvent(const Math::Vector2<int>& position, const Math::Vector2<int>& delta);

        /*@}*/

    private:
        SDL_Window* window;
        SDL_GLContext context;

        bool _redraw;
};

/* Implementations for inline functions with unused parameters */
inline void Sdl2Context::keyPressEvent(Sdl2Context::Key, Uint8) {}
inline void Sdl2Context::keyReleaseEvent(Sdl2Context::Key) {}
inline void Sdl2Context::mouseEvent(Sdl2Context::MouseButton, Sdl2Context::MouseState, const Math::Vector2<int>&) {}
inline void Sdl2Context::mouseWheelEvent(const Math::Vector2<int>&) {}
inline void Sdl2Context::mouseMotionEvent(const Math::Vector2<int>&, const Math::Vector2<int>&) {}

}}

#endif
