#ifndef Magnum_Platform_AbstractXApplication_h
#define Magnum_Platform_AbstractXApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

/** @file
 * @brief Class @ref Magnum::Platform::AbstractXApplication
 */

#include <memory>
#include <Corrade/Containers/EnumSet.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
/* undef Xlib nonsense to avoid conflicts */
#undef Always
#undef Complex
#undef None
#undef Status

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Platform/Platform.h"

namespace Magnum { namespace Platform {

namespace Implementation {
    template<class, class, class, class> class AbstractContextHandler;
}

/** @nosubgrouping
@brief Base for X11-based applications

Supports keyboard and mouse handling. See @ref platform for brief introduction.

@note Not meant to be used directly, see @ref GlxApplication and
    @ref XEglApplication subclasses.
*/
class AbstractXApplication {
    public:
        /** @brief Application arguments */
        struct Arguments {
            /** @brief Constructor */
            /*implicit*/ constexpr Arguments(int& argc, char** argv) noexcept: argc{argc}, argv{argv} {}

            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        class Configuration;
        class InputEvent;
        class KeyEvent;
        class MouseEvent;
        class MouseMoveEvent;

        /** @brief Copying is not allowed */
        AbstractXApplication(const AbstractXApplication&) = delete;

        /** @brief Moving is not allowed */
        AbstractXApplication(AbstractXApplication&&) = delete;

        /** @brief Copying is not allowed */
        AbstractXApplication& operator=(const AbstractXApplication&) = delete;

        /** @brief Moving is not allowed */
        AbstractXApplication& operator=(AbstractXApplication&&) = delete;

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`
         *
         * See @ref MAGNUM_GLXAPPLICATION_MAIN() or
         * @ref MAGNUM_XEGLAPPLICATION_MAIN() for usage information.
         */
        int exec();

        /** @brief Exit application main loop */
        void exit() { _flags |= Flag::Exit; }

    protected:
        /* Nobody will need to have (and delete) AbstractXApplication*, thus
           this is faster than public pure virtual destructor */
        ~AbstractXApplication();

        /** @copydoc Sdl2Application::createContext() */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void createContext(const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        void createContext(const Configuration& configuration);
        void createContext();
        #endif

        /** @copydoc Sdl2Application::tryCreateContext() */
        bool tryCreateContext(const Configuration& configuration);

        /** @{ @name Screen handling */

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers();

        /** @copydoc Sdl2Application::redraw() */
        void redraw() { _flags |= Flag::Redraw; }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @copydoc Sdl2Application::viewportEvent() */
        virtual void viewportEvent(const Vector2i& size);

        /** @copydoc Sdl2Application::drawEvent() */
        virtual void drawEvent() = 0;

        /*@}*/

        /** @{ @name Keyboard handling */

        /** @copydoc Sdl2Application::keyPressEvent() */
        virtual void keyPressEvent(KeyEvent& event);

        /** @copydoc Sdl2Application::keyReleaseEvent() */
        virtual void keyReleaseEvent(KeyEvent& event);

        /*@}*/

        /** @{ @name Mouse handling */

        /** @copydoc Sdl2Application::mousePressEvent() */
        virtual void mousePressEvent(MouseEvent& event);

        /** @copydoc Sdl2Application::mouseReleaseEvent() */
        virtual void mouseReleaseEvent(MouseEvent& event);

        /** @copydoc Sdl2Application::mouseMoveEvent() */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

        /*@}*/

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        explicit AbstractXApplication(Implementation::AbstractContextHandler<Configuration, Display*, VisualID, Window>* contextHandler, const Arguments& arguments, const Configuration& configuration);

        explicit AbstractXApplication(Implementation::AbstractContextHandler<Configuration, Display*, VisualID, Window>* contextHandler, const Arguments& arguments, NoCreateT);

    private:
        enum class Flag: unsigned int {
            Redraw = 1 << 0,
            Exit = 1 << 1
        };

        typedef Containers::EnumSet<Flag> Flags;
        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        Display* _display;
        Window _window;
        Atom _deleteWindow;

        std::unique_ptr<Implementation::AbstractContextHandler<Configuration, Display*, VisualID, Window>> _contextHandler;
        std::unique_ptr<Platform::Context> _context;

        /** @todo Get this from the created window */
        Vector2i _viewportSize;

        Flags _flags;
};

CORRADE_ENUMSET_OPERATORS(AbstractXApplication::Flags)

/**
@brief Configuration

Double-buffered OpenGL context.
@see @ref GlxApplication::GlxApplication(), @ref XEglApplication::XEglApplication(),
    @ref createContext(), @ref tryCreateContext()
@todo GLX_ARB_create_context_robustness/EGL_EXT_create_context_robustness
*/
class AbstractXApplication::Configuration {
    public:
        /*implicit*/ Configuration();
        ~Configuration();

        /** @brief Window title */
        std::string title() const { return _title; }

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * Default is `"Magnum X Application"`.
         */
        Configuration& setTitle(std::string title) {
            _title = std::move(title);
            return *this;
        }

        /** @copydoc Sdl2Application::Configuration::size() */
        Vector2i size() const { return _size; }

        /**
         * @brief Set window size
         * @return Reference to self (for method chaining)
         *
         * Default is `{800, 600}`.
         */
        Configuration& setSize(const Vector2i& size) {
            _size = size;
            return *this;
        }

        /** @copydoc Sdl2Application::Configuration::version() */
        Version version() const { return _version; }

        /** @copydoc Sdl2Application::Configuration::setVersion() */
        Configuration& setVersion(Version version) {
            _version = version;
            return *this;
        }

    private:
        std::string _title;
        Vector2i _size;
        Version _version;
};

/**
@brief Base for input events

@see @ref KeyEvent, @ref MouseEvent, @ref MouseMoveEvent, @ref keyPressEvent(),
    @ref keyReleaseEvent(), @ref mousePressEvent(), @ref mouseReleaseEvent(),
    @ref mouseMoveEvent()
*/
class AbstractXApplication::InputEvent {
    public:
        /**
         * @brief Modifier
         *
         * @see @ref Modifiers, @ref modifiers()
         */
        enum class Modifier: unsigned int {
            Shift = ShiftMask,          /**< Shift */
            Ctrl = ControlMask,         /**< Ctrl */
            Alt = Mod1Mask,             /**< Alt */
            AltGr = Mod5Mask,           /**< AltGr */
            CapsLock = LockMask,        /**< Caps lock */
            NumLock = Mod2Mask          /**< Num lock */
        };

        /**
         * @brief Set of modifiers
         *
         * @see @ref modifiers()
         */
        typedef Containers::EnumSet<Modifier> Modifiers;

        /**
         * @brief Mouse button
         *
         * @see @ref Buttons, @ref buttons()
         */
        enum class Button: unsigned int {
            Left = Button1Mask,     /**< Left button */
            Middle = Button2Mask,   /**< Middle button */
            Right = Button3Mask     /**< Right button */
        };

        /**
         * @brief Set of mouse buttons
         *
         * @see @ref buttons()
         */
        typedef Containers::EnumSet<Button> Buttons;

        /** @brief Copying is not allowed */
        InputEvent(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent(InputEvent&&) = delete;

        /** @brief Copying is not allowed */
        InputEvent& operator=(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent& operator=(InputEvent&&) = delete;

        /** @copydoc Sdl2Application::InputEvent::setAccepted() */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @copydoc Sdl2Application::InputEvent::isAccepted() */
        constexpr bool isAccepted() const { return _accepted; }

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

        /** @brief Mouse buttons */
        constexpr Buttons buttons() const { return Button(static_cast<unsigned int>(_modifiers)); }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        constexpr InputEvent(Modifiers modifiers): _modifiers(modifiers), _accepted(false) {}

        ~InputEvent() = default;
    #endif

    private:
        Modifiers _modifiers;
        bool _accepted;
};

CORRADE_ENUMSET_OPERATORS(AbstractXApplication::InputEvent::Modifiers)
CORRADE_ENUMSET_OPERATORS(AbstractXApplication::InputEvent::Buttons)

/**
@brief Key event

@see @ref keyPressEvent(), @ref keyReleaseEvent()
*/
class AbstractXApplication::KeyEvent: public AbstractXApplication::InputEvent {
    friend AbstractXApplication;

    public:
        /**
         * @brief Key
         *
         * @see @ref key()
         */
        enum class Key: KeySym {
            Enter = XK_Return,          /**< Enter */
            Esc = XK_Escape,            /**< Escape */

            Up = XK_Up,                 /**< Up arrow */
            Down = XK_Down,             /**< Down arrow */
            Left = XK_Left,             /**< Left arrow */
            Right = XK_Right,           /**< Right arrow */
            F1 = XK_F1,                 /**< F1 */
            F2 = XK_F2,                 /**< F2 */
            F3 = XK_F3,                 /**< F3 */
            F4 = XK_F4,                 /**< F4 */
            F5 = XK_F5,                 /**< F5 */
            F6 = XK_F6,                 /**< F6 */
            F7 = XK_F7,                 /**< F7 */
            F8 = XK_F8,                 /**< F8 */
            F9 = XK_F9,                 /**< F9 */
            F10 = XK_F10,               /**< F10 */
            F11 = XK_F11,               /**< F11 */
            F12 = XK_F12,               /**< F12 */
            Home = XK_Home,             /**< Home */
            End = XK_End,               /**< End */
            PageUp = XK_Page_Up,        /**< Page up */
            PageDown = XK_Page_Down,    /**< Page down */

            Space = XK_space,           /**< Space */
            Comma = XK_comma,           /**< Comma */
            Period = XK_period,         /**< Period */
            Minus = XK_minus,           /**< Minus */
            Plus = XK_plus,             /**< Plus */
            Slash = XK_slash,           /**< Slash */
            Percent = XK_percent,       /**< Percent */
            Equal = XK_equal,           /**< Equal */

            Zero = XK_0,                /**< Zero */
            One = XK_1,                 /**< One */
            Two = XK_2,                 /**< Two */
            Three = XK_3,               /**< Three */
            Four = XK_4,                /**< Four */
            Five = XK_5,                /**< Five */
            Six = XK_6,                 /**< Six */
            Seven = XK_7,               /**< Seven */
            Eight = XK_8,               /**< Eight */
            Nine = XK_9,                /**< Nine */

            A = XK_a,                   /**< Small letter A */
            B = XK_b,                   /**< Small letter B */
            C = XK_c,                   /**< Small letter C */
            D = XK_d,                   /**< Small letter D */
            E = XK_e,                   /**< Small letter E */
            F = XK_f,                   /**< Small letter F */
            G = XK_g,                   /**< Small letter G */
            H = XK_h,                   /**< Small letter H */
            I = XK_i,                   /**< Small letter I */
            J = XK_j,                   /**< Small letter J */
            K = XK_k,                   /**< Small letter K */
            L = XK_l,                   /**< Small letter L */
            M = XK_m,                   /**< Small letter M */
            N = XK_n,                   /**< Small letter N */
            O = XK_o,                   /**< Small letter O */
            P = XK_p,                   /**< Small letter P */
            Q = XK_q,                   /**< Small letter Q */
            R = XK_r,                   /**< Small letter R */
            S = XK_s,                   /**< Small letter S */
            T = XK_t,                   /**< Small letter T */
            U = XK_u,                   /**< Small letter U */
            V = XK_v,                   /**< Small letter V */
            W = XK_w,                   /**< Small letter W */
            X = XK_x,                   /**< Small letter X */
            Y = XK_y,                   /**< Small letter Y */
            Z = XK_z                    /**< Small letter Z */
        };

        /** @brief Key */
        constexpr Key key() const { return _key; }

        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

    private:
        constexpr KeyEvent(Key key, Modifiers modifiers, const Vector2i& position): InputEvent(modifiers), _key(key), _position(position) {}

        const Key _key;
        const Vector2i _position;
};

/**
@brief Mouse event

@see @ref MouseMoveEvent, @ref mousePressEvent(), @ref mouseReleaseEvent()
*/
class AbstractXApplication::MouseEvent: public AbstractXApplication::InputEvent {
    friend AbstractXApplication;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: unsigned int {
            Left = Button1,         /**< Left button */
            Middle = Button2,       /**< Middle button */
            Right = Button3,        /**< Right button */
            WheelUp = Button4,      /**< Wheel up */
            WheelDown = Button5     /**< Wheel down */
        };

        /** @brief Button */
        constexpr Button button() const { return _button; }

        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

    private:
        constexpr MouseEvent(Button button, Modifiers modifiers, const Vector2i& position): InputEvent(modifiers), _button(button), _position(position) {}

        const Button _button;
        const Vector2i _position;
};

/**
@brief Mouse move event

@see @ref MouseEvent, @ref mouseMoveEvent()
*/
class AbstractXApplication::MouseMoveEvent: public AbstractXApplication::InputEvent {
    friend AbstractXApplication;

    public:
        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

    private:
        constexpr MouseMoveEvent(Modifiers modifiers, const Vector2i& position): InputEvent(modifiers), _position(position) {}

        const Vector2i _position;
};

}}

#endif
