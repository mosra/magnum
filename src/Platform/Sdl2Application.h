#ifndef Magnum_Platform_Sdl2Application_h
#define Magnum_Platform_Sdl2Application_h
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
 * @brief Class Magnum::Platform::Sdl2Application
 */

#include "Math/Vector2.h"
#include "Magnum.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <Corrade/Containers/EnumSet.h>

#include "magnumCompatibility.h"

namespace Magnum {

class Context;

namespace Platform {

/** @nosubgrouping
@brief SDL2 application

Supports keyboard and mouse handling.

@section Sdl2Application-usage Usage

You need to implement at least drawEvent() and viewportEvent() to be able to
draw on the screen.  The subclass can be then used directly in `main()` - see
convenience macro MAGNUM_SDL2APPLICATION_MAIN().
@code
class MyApplication: public Magnum::Platform::Sdl2Application {
    // implement required methods...
};
MAGNUM_SDL2APPLICATION_MAIN(MyApplication)
@endcode
*/
class Sdl2Application {
    public:
        /**
         * @brief Constructor
         * @param argc      Count of arguments of `main()` function
         * @param argv      Arguments of `main()` function
         * @param title     Window title
         * @param size      Window size
         *
         * Creates centered non-resizable window with double-buffered
         * OpenGL 3.2 context with 24bit depth buffer.
         */
        Sdl2Application(int argc, char** argv, const std::string& title = "Magnum SDL2 application", const Math::Vector2<GLsizei>& size = Math::Vector2<GLsizei>(800, 600));

        /**
         * @brief Destructor
         *
         * Deletes context and destroys the window.
         */
        virtual ~Sdl2Application();

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`.
         */
        int exec();

        /** @{ @name Drawing functions */

    protected:
        /** @copydoc GlutApplication::viewportEvent() */
        virtual void viewportEvent(const Math::Vector2<GLsizei>& size) = 0;

        /** @copydoc GlutApplication::drawEvent() */
        virtual void drawEvent() = 0;

        /** @copydoc GlutApplication::swapBuffers() */
        inline void swapBuffers() { SDL_GL_SwapWindow(window); }

        /** @copydoc GlutApplication::redraw() */
        inline void redraw() { _redraw = true; }

        /*@}*/

        /** @{ @name Keyboard handling */
    public:
        /**
         * @brief %Modifier
         *
         * @see Modifiers, keyPressEvent(), keyReleaseEvent()
         */
        enum class Modifier: Uint16 {
            Shift = KMOD_SHIFT,         /**< Shift */
            Ctrl = KMOD_CTRL,           /**< Ctrl */
            Alt = KMOD_ALT,             /**< Alt */
            AltGr = KMOD_MODE,          /**< AltGr */

            CapsLock = KMOD_CAPS,       /**< Caps lock */
            NumLock = KMOD_NUM          /**< Num lock */
        };

        /**
         * @brief Set of modifiers
         *
         * @see keyPressEvent(), keyReleaseEvent()
         */
        typedef Corrade::Containers::EnumSet<Modifier, Uint16> Modifiers;

        /**
         * @brief Key
         *
         * @see keyPressEvent(), keyReleaseEvent()
         */
        enum class Key: SDL_Keycode {
            Enter = SDLK_RETURN,        /**< Enter */
            Esc = SDLK_ESCAPE,          /**< Escape */

            Up = SDLK_UP,               /**< Up arrow */
            Down = SDLK_DOWN,           /**< Down arrow */
            Left = SDLK_LEFT,           /**< Left arrow */
            Right = SDLK_RIGHT,         /**< Right arrow */
            F1 = SDLK_F1,               /**< F1 */
            F2 = SDLK_F2,               /**< F2 */
            F3 = SDLK_F3,               /**< F3 */
            F4 = SDLK_F4,               /**< F4 */
            F5 = SDLK_F5,               /**< F5 */
            F6 = SDLK_F6,               /**< F6 */
            F7 = SDLK_F7,               /**< F7 */
            F8 = SDLK_F8,               /**< F8 */
            F9 = SDLK_F9,               /**< F9 */
            F10 = SDLK_F10,             /**< F10 */
            F11 = SDLK_F11,             /**< F11 */
            F12 = SDLK_F12,             /**< F12 */
            Home = SDLK_HOME,           /**< Home */
            End = SDLK_END,             /**< End */
            PageUp = SDLK_PAGEUP,       /**< Page up */
            PageDown = SDLK_PAGEDOWN,   /**< Page down */

            Space = SDLK_SPACE,         /**< Space */
            Comma = SDLK_COMMA,         /**< Comma */
            Period = SDLK_PERIOD,       /**< Period */
            Minus = SDLK_MINUS,         /**< Minus */
            Plus = SDLK_PLUS,           /**< Plus */
            Slash = SDLK_SLASH,         /**< Slash */
            Percent = SDLK_PERCENT,     /**< Percent */
            Equal = SDLK_EQUALS,        /**< Equal */

            Zero = SDLK_0,              /**< Zero */
            One = SDLK_1,               /**< One */
            Two = SDLK_2,               /**< Two */
            Three = SDLK_3,             /**< Three */
            Four = SDLK_4,              /**< Four */
            Five = SDLK_5,              /**< Five */
            Six = SDLK_6,               /**< Six */
            Seven = SDLK_7,             /**< Seven */
            Eight = SDLK_8,             /**< Eight */
            Nine = SDLK_9,              /**< Nine */

            A = SDLK_a,                 /**< Letter A */
            B = SDLK_b,                 /**< Letter B */
            C = SDLK_c,                 /**< Letter C */
            D = SDLK_d,                 /**< Letter D */
            E = SDLK_e,                 /**< Letter E */
            F = SDLK_f,                 /**< Letter F */
            G = SDLK_g,                 /**< Letter G */
            H = SDLK_h,                 /**< Letter H */
            I = SDLK_i,                 /**< Letter I */
            J = SDLK_j,                 /**< Letter J */
            K = SDLK_k,                 /**< Letter K */
            L = SDLK_l,                 /**< Letter L */
            M = SDLK_m,                 /**< Letter M */
            N = SDLK_n,                 /**< Letter N */
            O = SDLK_o,                 /**< Letter O */
            P = SDLK_p,                 /**< Letter P */
            Q = SDLK_q,                 /**< Letter Q */
            R = SDLK_r,                 /**< Letter R */
            S = SDLK_s,                 /**< Letter S */
            T = SDLK_t,                 /**< Letter T */
            U = SDLK_u,                 /**< Letter U */
            V = SDLK_v,                 /**< Letter V */
            W = SDLK_w,                 /**< Letter W */
            X = SDLK_x,                 /**< Letter X */
            Y = SDLK_y,                 /**< Letter Y */
            Z = SDLK_z                  /**< Letter Z */
        };

    protected:
        /**
         * @brief Key press event
         * @param key       Key pressed
         * @param modifiers Active modifiers
         * @param position  Cursor position (not yet implemented)
         */
        virtual void keyPressEvent(Key key, Modifiers modifiers, const Math::Vector2<int>& position);

        /**
         * @brief Key release event
         * @param key       Key released
         * @param modifiers Active modifiers
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
         * @param modifiers Active modifiers (not implemented)
         * @param position  Cursor position
         *
         * Called when mouse button is pressed. Default implementation does
         * nothing.
         */
        virtual void mousePressEvent(MouseButton button, Modifiers modifiers, const Math::Vector2<int>& position);

        /**
         * @brief Mouse release event
         * @param button    Button released
         * @param modifiers Active modifiers (not implemented)
         * @param position  Cursor position
         *
         * Called when mouse button is released. Default implementation does
         * nothing.
         */
        virtual void mouseReleaseEvent(MouseButton button, Modifiers modifiers, const Math::Vector2<int>& position);

        /**
         * @brief Mouse motion event
         * @param modifiers Active modifiers (not implemented)
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

/** @hideinitializer
@param className Class name

Can be used as equivalent to the following code to achieve better portability,
see @ref portability-applications for more information.
@code
int main(int argc, char** argv) {
    className app(argc, argv);
    return app.exec();
}
@endcode
When no other application header is included this macro is also aliased to
`MAGNUM_APPLICATION_MAIN()`.
*/
#define MAGNUM_SDL2APPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app(argc, argv);                                          \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_SDL2APPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

CORRADE_ENUMSET_OPERATORS(Sdl2Application::Modifiers)

/* Implementations for inline functions with unused parameters */
inline void Sdl2Application::keyPressEvent(Key, Modifiers, const Math::Vector2<int>&) {}
inline void Sdl2Application::keyReleaseEvent(Key, Modifiers, const Math::Vector2<int>&) {}
inline void Sdl2Application::mousePressEvent(MouseButton, Modifiers, const Math::Vector2<int>&) {}
inline void Sdl2Application::mouseReleaseEvent(MouseButton, Modifiers, const Math::Vector2<int>&) {}
inline void Sdl2Application::mouseMotionEvent(Modifiers, const Math::Vector2<int>&) {}

}}

#endif
