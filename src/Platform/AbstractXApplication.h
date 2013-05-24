#ifndef Magnum_Platform_AbstractXApplication_h
#define Magnum_Platform_AbstractXApplication_h
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
 * @brief Class Magnum::Platform::AbstractXApplication
 */

#include <Containers/EnumSet.h>

#include "Magnum.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
/* undef Xlib nonsense to avoid conflicts */
#undef None
#undef Always

#include "Math/Vector2.h"
#include "AbstractContextHandler.h"

namespace Magnum {

class Context;

namespace Platform {

/** @nosubgrouping
@brief Base for X11-based applications

Supports keyboard and mouse handling. See @ref platform for brief introduction.
@note Not meant to be used directly, see subclasses.
*/
class AbstractXApplication {
    public:
        /** @brief Application arguments */
        struct Arguments {
            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        class Configuration;
        class InputEvent;
        class KeyEvent;
        class MouseEvent;
        class MouseMoveEvent;

        /**
         * @brief Default constructor
         * @param contextHandler OpenGL context handler
         * @param arguments     Application arguments
         *
         * Creates application with default configuration. See Configuration
         * for more information.
         */
        explicit AbstractXApplication(AbstractContextHandler<Display*, VisualID, Window>* contextHandler, const Arguments& arguments);

        /**
         * @brief Constructor
         * @param contextHandler OpenGL context handler
         * @param arguments     Application arguments
         * @param configuration Configuration
         *
         * The @p configuration is deleted afterwards. If `nullptr` is passed
         * as @p configuration, the context is not created and must be created
         * with createContext().
         */
        explicit AbstractXApplication(AbstractContextHandler<Display*, VisualID, Window>* contextHandler, const Arguments& arguments, Configuration* configuration);

        /**
         * @brief Destructor
         *
         * Deletes context and destroys the window.
         */
        virtual ~AbstractXApplication() = 0;

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`.
         */
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
        inline void swapBuffers() { contextHandler->swapBuffers(); }

        /** @copydoc GlutApplication::redraw() */
        inline void redraw() { flags |= Flag::Redraw; }

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

    private:
        enum class Flag: unsigned int {
            Redraw = 1 << 0,
            Exit = 1 << 1
        };

        typedef Containers::EnumSet<Flag, unsigned int> Flags;
        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        Display* display;
        Window window;
        Atom deleteWindow;

        AbstractContextHandler<Display*, VisualID, Window>* contextHandler;

        Context* c;

        /** @todo Get this from the created window */
        Vector2i viewportSize;

        Flags flags;
};

CORRADE_ENUMSET_OPERATORS(AbstractXApplication::Flags)

/**
@brief %Configuration

Double-buffered OpenGL context.
@see AbstractXApplication(), createContext()
@todo GLX_ARB_create_context_robustness/EGL_EXT_create_context_robustness
*/
class AbstractXApplication::Configuration {
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
         * Default is `"Magnum X Application"`.
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

    private:
        std::string _title;
        Vector2i _size;
};

/**
@brief Base for input events

@see KeyEvent, MouseEvent, MouseMoveEvent, keyPressEvent(), keyReleaseEvent(),
    mousePressEvent(), mouseReleaseEvent(), mouseMoveEvent()
*/
class AbstractXApplication::InputEvent {
    InputEvent(const InputEvent&) = delete;
    InputEvent(InputEvent&&) = delete;
    InputEvent& operator=(const InputEvent&) = delete;
    InputEvent& operator=(InputEvent&&) = delete;

    public:
    public:
        /**
         * @brief %Modifier
         *
         * @see Modifiers, modifiers()
         */
        enum class Modifier: unsigned int {
            Shift = ShiftMask,          /**< Shift */
            Ctrl = ControlMask,         /**< Ctrl */
            Alt = Mod1Mask,             /**< Alt */
            AltGr = Mod5Mask,           /**< AltGr */

            LeftButton = Button1Mask,   /**< Left mouse button */
            MiddleButton = Button2Mask, /**< Middle mouse button */
            RightButton = Button3Mask,  /**< Right mouse button */

            CapsLock = LockMask,        /**< Caps lock */
            NumLock = Mod2Mask          /**< Num lock */
        };

        /**
         * @brief Set of modifiers
         *
         * @see modifiers()
         */
        typedef Containers::EnumSet<Modifier, unsigned int> Modifiers;

        inline virtual ~InputEvent() {}

        /** @copydoc GlutApplication::InputEvent::setAccepted() */
        inline void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @copydoc GlutApplication::InputEvent::isAccepted() */
        inline bool isAccepted() { return _accepted; }

        /** @brief Modifiers */
        inline Modifiers modifiers() const { return _modifiers; }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        inline InputEvent(Modifiers modifiers): _modifiers(modifiers), _accepted(false) {}
    #endif

    private:
        Modifiers _modifiers;
        bool _accepted;
};

CORRADE_ENUMSET_OPERATORS(AbstractXApplication::InputEvent::Modifiers)

/**
@brief Key event

@see keyPressEvent(), keyReleaseEvent()
*/
class AbstractXApplication::KeyEvent: public AbstractXApplication::InputEvent {
    friend class AbstractXApplication;

    public:
        /**
         * @brief Key
         *
         * @see key()
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
        inline Key key() const { return _key; }

        /** @brief Position */
        inline Vector2i position() const { return _position; }

    private:
        inline KeyEvent(Key key, Modifiers modifiers, const Vector2i& position): InputEvent(modifiers), _key(key), _position(position) {}

        const Key _key;
        const Vector2i _position;
};

/**
@brief Mouse event

@see MouseMoveEvent, mousePressEvent(), mouseReleaseEvent()
*/
class AbstractXApplication::MouseEvent: public AbstractXApplication::InputEvent {
    friend class AbstractXApplication;

    public:
        /**
         * @brief Mouse button
         *
         * @see button()
         */
        enum class Button: unsigned int {
            Left = Button1,         /**< Left button */
            Middle = Button2,       /**< Middle button */
            Right = Button3,        /**< Right button */
            WheelUp = Button4,      /**< Wheel up */
            WheelDown = Button5     /**< Wheel down */
        };

        /** @brief Button */
        inline Button button() const { return _button; }

        /** @brief Position */
        inline Vector2i position() const { return _position; }

    private:
        inline MouseEvent(Button button, Modifiers modifiers, const Vector2i& position): InputEvent(modifiers), _button(button), _position(position) {}

        const Button _button;
        const Vector2i _position;
};

/**
@brief Mouse move event

@see MouseEvent, mouseMoveEvent()
*/
class AbstractXApplication::MouseMoveEvent: public AbstractXApplication::InputEvent {
    friend class AbstractXApplication;

    public:
        /** @brief Position */
        inline Vector2i position() const { return _position; }

    private:
        inline MouseMoveEvent(Modifiers modifiers, const Vector2i& position): InputEvent(modifiers), _position(position) {}

        const Vector2i _position;
};

/* Implementations for inline functions with unused parameters */
inline void AbstractXApplication::keyPressEvent(KeyEvent&) {}
inline void AbstractXApplication::keyReleaseEvent(KeyEvent&) {}
inline void AbstractXApplication::mousePressEvent(MouseEvent&) {}
inline void AbstractXApplication::mouseReleaseEvent(MouseEvent&) {}
inline void AbstractXApplication::mouseMoveEvent(MouseMoveEvent&) {}

}}

#endif
