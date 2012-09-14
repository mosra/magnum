#ifndef Magnum_Contexts_Sdl2WindowContext_h
#define Magnum_Contexts_Sdl2WindowContext_h
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
 * @brief Class Magnum::Contexts::Sdl2WindowContext
 */

#include "Math/Vector2.h"
#include "Magnum.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <Corrade/Containers/EnumSet.h>

#include "AbstractWindowContext.h"

namespace Magnum {

class Context;

namespace Contexts {

/** @nosubgrouping
@brief SDL2 context

Supports keyboard and mouse handling.

You need to implement at least drawEvent() and viewportEvent() to be able to
draw on the screen.
*/
class Sdl2WindowContext: public AbstractWindowContext {
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
        Sdl2WindowContext(int argc, char** argv, const std::string& title = "Magnum SDL2 window context", const Math::Vector2<GLsizei>& size = Math::Vector2<GLsizei>(800, 600));

        /**
         * @brief Destructor
         *
         * Deletes context and destroys the window.
         */
        ~Sdl2WindowContext();

        int exec();

        /** @{ @name Drawing functions */

    protected:
        /** @copydoc GlutWindowContext::viewportEvent() */
        virtual void viewportEvent(const Math::Vector2<GLsizei>& size) = 0;

        /** @copydoc GlutWindowContext::drawEvent() */
        virtual void drawEvent() = 0;

        /** @copydoc GlutWindowContext::swapBuffers() */
        inline void swapBuffers() { SDL_GL_SwapWindow(window); }

        /** @copydoc GlutWindowContext::redraw() */
        inline void redraw() { _redraw = true; }

        /*@}*/

        /** @{ @name Keyboard handling */
    public:
        /**
         * @brief %Modifier
         *
         * @see Modifiers, keyPressEvent(), keyReleaseEvent(),
         *      mousePressEvent(), mouseReleaseEvent(), mouseMotionEvent()
         */
        enum class Modifier: unsigned int {};

        /**
         * @brief Set of modifiers
         *
         * @see keyPressEvent(), keyReleaseEvent()
         */
        typedef Corrade::Containers::EnumSet<Modifier, unsigned int> Modifiers;

        /**
         * @brief Key
         *
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
         * @param modifiers Active modifiers (not yet implemented)
         * @param position  Cursor position (not yet implemented)
         */
        virtual void keyPressEvent(Key key, Modifiers modifiers, const Math::Vector2<int>& position);

        /**
         * @brief Key release event
         * @param key       Key released
         * @param modifiers Active modifiers (not yet implemented)
         * @param position  Cursor position (not yet implemented)
         */
        virtual void keyReleaseEvent(Key key, Modifiers modifiers, const Math::Vector2<int>& position);

        /*@}*/

        /** @{ @name Mouse handling */

    public:
        /**
         * @brief Mouse button
         *
         * @see mouseEvent()
         */
        enum class MouseButton: Uint8 {
            Left = SDL_BUTTON_LEFT,         /**< Left button */
            Middle = SDL_BUTTON_MIDDLE,     /**< Middle button */
            Right = SDL_BUTTON_RIGHT,       /**< Right button */
            WheelUp = 4,                    /**< Wheel up */
            WheelDown = 5                   /**< Wheel down */
        };

        /**
         * @brief Mouse state
         *
         * @see mouseEvent()
         */
        enum class MouseState: Uint8 {
            Pressed = SDL_PRESSED,          /**< Button pressed */
            Released = SDL_RELEASED         /**< Button released */
        };

    protected:
        /**
         * @brief Mouse press event
         * @param button    Button pressed
         * @param modifiers Active modifiers (not yet implemented)
         * @param position  Cursor position
         *
         * Called when mouse button is pressed. Default implementation does
         * nothing.
         */
        virtual void mousePressEvent(MouseButton button, Modifiers modifiers, const Math::Vector2<int>& position);

        /**
         * @brief Mouse release event
         * @param button    Button released
         * @param modifiers Active modifiers (not yet implemented)
         * @param position  Cursor position
         *
         * Called when mouse button is released. Default implementation does
         * nothing.
         */
        virtual void mouseReleaseEvent(MouseButton button, Modifiers modifiers, const Math::Vector2<int>& position);

        /**
         * @brief Mouse motion event
         * @param modifiers Active modifiers (not yet implemented)
         * @param position  Mouse position relative to the window
         *
         * Called when mouse is moved. Default implementation does nothing.
         */
        virtual void mouseMotionEvent(Modifiers modifiers, const Math::Vector2<int>& position);

        /*@}*/

    private:
        SDL_Window* window;
        SDL_GLContext context;

        Context* c;

        bool _redraw;
};

CORRADE_ENUMSET_OPERATORS(Sdl2WindowContext::Modifiers)

/* Implementations for inline functions with unused parameters */
inline void Sdl2WindowContext::keyPressEvent(Key, Modifiers, const Math::Vector2<int>&) {}
inline void Sdl2WindowContext::keyReleaseEvent(Key, Modifiers, const Math::Vector2<int>&) {}
inline void Sdl2WindowContext::mousePressEvent(MouseButton, Modifiers, const Math::Vector2<int>&) {}
inline void Sdl2WindowContext::mouseReleaseEvent(MouseButton, Modifiers, const Math::Vector2<int>&) {}
inline void Sdl2WindowContext::mouseMotionEvent(Modifiers, const Math::Vector2<int>&) {}

}}

#endif
