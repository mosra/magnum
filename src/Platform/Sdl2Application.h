#ifndef Magnum_Platform_Sdl2Application_h
#define Magnum_Platform_Sdl2Application_h
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

/** @file
 * @brief Class Magnum::Platform::Sdl2Application
 */

#include "Math/Vector2.h"
#include "Magnum.h"

#ifdef _WIN32 /* Windows version of SDL2 redefines main(), we don't want that */
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <Corrade/Containers/EnumSet.h>

namespace Magnum {

class Context;

namespace Platform {

/** @nosubgrouping
@brief SDL2 application

Application using [Simple DirectMedia Layer](www.libsdl.org/). Supports
keyboard and mouse handling. See @ref platform for brief introduction.

@section Sdl2Application-usage Usage

You need to implement at least drawEvent() and viewportEvent() to be able to
draw on the screen. The subclass can be then used directly in `main()` -- see
convenience macro MAGNUM_SDL2APPLICATION_MAIN().
@code
class MyApplication: public Magnum::Platform::Sdl2Application {
    // implement required methods...
};
MAGNUM_SDL2APPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.
*/
class Sdl2Application {
    public:
        /** @brief Application arguments */
        typedef std::pair<int&, char**> Arguments;

        class Configuration;
        class InputEvent;
        class KeyEvent;
        class MouseEvent;
        class MouseMoveEvent;

        /** @copydoc GlutApplication::GlutApplication(const Arguments&) */
        explicit Sdl2Application(const Arguments& arguments);

        /** @copydoc GlutApplication::GlutApplication(const Arguments&, Configuration*) */
        explicit Sdl2Application(const Arguments& arguments, Configuration* configuration);

        virtual ~Sdl2Application();

        /** @copydoc GlutApplication::exec() */
        int exec();

        /** @brief Exit application main loop */
        inline void exit() { flags |= Flag::Exit; }

    protected:
        /** @copydoc GlutApplication::createContext() */
        void createContext(Configuration* configuration);

        /** @{ @name Drawing functions */

        /** @copydoc GlutApplication::viewportEvent() */
        virtual void viewportEvent(const Vector2i& size) = 0;

        /** @copydoc GlutApplication::drawEvent() */
        virtual void drawEvent() = 0;

        /** @copydoc GlutApplication::swapBuffers() */
        inline void swapBuffers() { SDL_GL_SwapWindow(window); }

        /** @copydoc GlutApplication::redraw() */
        inline void redraw() { flags |= Flag::Redraw; }

        /*@}*/

        /** @{ @name Keyboard handling */

        /** @copydoc GlutApplication::keyPressEvent() */
        virtual void keyPressEvent(KeyEvent& event);

        /**
         * @brief Key release event
         *
         * Called when an key is released. Default implementation does nothing.
         */
        virtual void keyReleaseEvent(KeyEvent& event);

        /*@}*/

        /** @{ @name Mouse handling */

    public:
        /** @brief Whether mouse is locked */
        inline bool isMouseLocked() const {
            return SDL_GetRelativeMouseMode();
        }

        /**
         * @brief Enable or disable mouse locking
         *
         * When mouse is locked, the cursor is hidden and only
         * MouseMoveEvent::relativePosition() is changing, absolute position
         * stays the same.
         */
        void setMouseLocked(bool enabled);

    protected:
        /** @copydoc GlutApplication::mousePressEvent() */
        virtual void mousePressEvent(MouseEvent& event);

        /** @copydoc GlutApplication::mouseReleaseEvent() */
        virtual void mouseReleaseEvent(MouseEvent& event);

        /**
         * @brief Mouse move event
         *
         * Called when mouse is moved. Default implementation does nothing.
         */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

        /*@}*/

    private:
        enum class Flag: UnsignedByte {
            Redraw = 1 << 0,
            Exit = 1 << 1
        };

        typedef Corrade::Containers::EnumSet<Flag, UnsignedByte> Flags;
        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        SDL_Window* window;
        SDL_GLContext context;

        Context* c;

        Flags flags;
};

CORRADE_ENUMSET_OPERATORS(Sdl2Application::Flags)

/**
@brief %Configuration

Centered non-resizable window with double-buffered OpenGL context and 24bit
depth buffer.
@see Sdl2Application(), createContext()
*/
class Sdl2Application::Configuration {
    Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration& operator=(Configuration&&) = delete;

    public:
        explicit Configuration();
        ~Configuration();

        /** @brief Window title */
        inline std::string title() const { return _title; }

        /**
         * @brief Set window title
         * @return Pointer to self (for method chaining)
         *
         * Default is `"Magnum SDL2 Application"`.
         */
        inline Configuration* setTitle(std::string title) {
            _title = std::move(title);
            return this;
        }

        /** @brief Window size */
        inline Vector2i size() const { return _size; }

        /**
         * @brief Set window size
         * @return Pointer to self (for method chaining)
         *
         * Default is `{800, 600}`.
         */
        inline Configuration* setSize(const Vector2i& size) {
            _size = size;
            return this;
        }

        /** @brief Sample count */
        inline Int sampleCount() const { return _sampleCount; }

        /**
         * @brief Set sample count
         * @return Pointer to self (for method chaining)
         *
         * Default is `0`, thus no multisampling. See also
         * @ref Renderer::Feature "Renderer::Feature::Multisampling".
         */
        inline Configuration* setSampleCount(Int count) {
            _sampleCount = count;
            return this;
        }

    private:
        std::string _title;
        Vector2i _size;
        Int _sampleCount;
};

/**
@brief Base for input events

@see KeyEvent, MouseEvent, MouseMoveEvent, keyPressEvent(), keyReleaseEvent(),
    mousePressEvent(), mouseReleaseEvent(), mouseMoveEvent()
*/
class Sdl2Application::InputEvent {
    InputEvent(const InputEvent&) = delete;
    InputEvent(InputEvent&&) = delete;
    InputEvent& operator=(const InputEvent&) = delete;
    InputEvent& operator=(InputEvent&&) = delete;

    public:
        /**
         * @brief %Modifier
         *
         * @see Modifiers, KeyEvent::modifiers(), MouseEvent::modifiers(),
         *      MouseMoveEvent::modifiers()
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
         * @see KeyEvent::modifiers(), MouseEvent::modifiers(),
         *      MouseMoveEvent::modifiers()
         */
        typedef Corrade::Containers::EnumSet<Modifier, Uint16> Modifiers;

        inline virtual ~InputEvent() {}

        /** @copydoc GlutApplication::InputEvent::setAccepted() */
        inline void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @copydoc GlutApplication::InputEvent::isAccepted() */
        inline bool isAccepted() { return _accepted; }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        inline InputEvent(): _accepted(false) {}
    #endif

    private:
        bool _accepted;
};

/**
@brief Key event

@see keyPressEvent(), keyReleaseEvent()
*/
class Sdl2Application::KeyEvent: public Sdl2Application::InputEvent {
    friend class Sdl2Application;

    public:
        /**
         * @brief Key
         *
         * @see key()
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

        /** @brief Key */
        inline Key key() const { return _key; }

        /** @brief Modifiers */
        inline Modifiers modifiers() const { return _modifiers; }

    private:
        inline KeyEvent(Key key, Modifiers modifiers): _key(key), _modifiers(modifiers) {}

        const Key _key;
        const Modifiers _modifiers;
};

/**
@brief Mouse event

@see MouseMoveEvent, mousePressEvent(), mouseReleaseEvent()
*/
class Sdl2Application::MouseEvent: public Sdl2Application::InputEvent {
    friend class Sdl2Application;

    public:
        /**
         * @brief Mouse button
         *
         * @see button()
         */
        enum class Button: Uint8 {
            Left = SDL_BUTTON_LEFT,         /**< Left button */
            Middle = SDL_BUTTON_MIDDLE,     /**< Middle button */
            Right = SDL_BUTTON_RIGHT,       /**< Right button */
            WheelUp = 4,                    /**< Wheel up */
            WheelDown = 5                   /**< Wheel down */
        };

        /** @brief Button */
        inline Button button() const { return _button; }

        /** @brief Position */
        inline Vector2i position() const { return _position; }

        /**
         * @brief Modifiers
         *
         * Lazily populated on first request.
         */
        Modifiers modifiers();

    private:
        inline MouseEvent(Button button, const Vector2i& position): _button(button), _position(position), modifiersLoaded(false) {}

        const Button _button;
        const Vector2i _position;
        bool modifiersLoaded;
        Modifiers _modifiers;
};

/**
@brief Mouse move event

@see MouseEvent, mouseMoveEvent()
*/
class Sdl2Application::MouseMoveEvent: public Sdl2Application::InputEvent {
    friend class Sdl2Application;

    public:
        /** @brief Position */
        inline Vector2i position() const { return _position; }

        /**
         * @brief Relative position
         *
         * Position relative to previous event.
         */
        inline Vector2i relativePosition() const { return _relativePosition; }

        /**
         * @brief Modifiers
         *
         * Lazily populated on first request.
         */
        Modifiers modifiers();

    private:
        inline MouseMoveEvent(const Vector2i& position, const Vector2i& relativePosition): _position(position), _relativePosition(relativePosition), modifiersLoaded(false) {}

        const Vector2i _position, _relativePosition;
        bool modifiersLoaded;
        Modifiers _modifiers;
};

/** @hideinitializer
@brief Entry point for SDL2-based applications
@param className Class name

Can be used with Sdl2Application subclasses as equivalent to the following
code to achieve better portability, see @ref portability-applications for more
information.
@code
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode
When no other application header is included this macro is also aliased to
`MAGNUM_APPLICATION_MAIN()`.
*/
#define MAGNUM_SDL2APPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef Sdl2Application Application;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_SDL2APPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

CORRADE_ENUMSET_OPERATORS(Sdl2Application::InputEvent::Modifiers)

/* Implementations for inline functions with unused parameters */
inline void Sdl2Application::keyPressEvent(KeyEvent&) {}
inline void Sdl2Application::keyReleaseEvent(KeyEvent&) {}
inline void Sdl2Application::mousePressEvent(MouseEvent&) {}
inline void Sdl2Application::mouseReleaseEvent(MouseEvent&) {}
inline void Sdl2Application::mouseMoveEvent(MouseMoveEvent&) {}

}}

#endif
