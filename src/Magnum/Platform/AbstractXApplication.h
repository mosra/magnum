#ifndef Magnum_Platform_AbstractXApplication_h
#define Magnum_Platform_AbstractXApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2019, 2020 Konstantinos Chatzilygeroudis <costashatz@gmail.com>

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

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/String.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
/* undef Xlib nonsense to avoid conflicts */
#undef Always
#undef Bool
#undef Complex
#undef Convex
#undef None
#undef Status
#undef Success
#undef Button1
#undef Button2
#undef Button3
#undef Button4
#undef Button5

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Unfortunately Xlib *needs* the Bool type, so provide a typedef instead */
typedef int Bool;
#endif

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Platform/Platform.h"
#include "Magnum/Platform/GLContext.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* Some APIs used to take or return a std::string before */
#include <Corrade/Containers/StringStl.h>
#endif

namespace Magnum { namespace Platform {

namespace Implementation {
    template<class, class, class, class> class AbstractContextHandler;
}

/** @nosubgrouping
@brief Base for X11-based applications

Supports keyboard and mouse handling. See @ref platform for brief introduction.

@note Not meant to be used directly, see the @ref GlxApplication and
    @ref XEglApplication subclasses instead.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
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
        class GLConfiguration;
        class ViewportEvent;
        class InputEvent;
        class KeyEvent;
        class PointerEvent;
        class PointerMoveEvent;
        #ifdef MAGNUM_BUILD_DEPRECATED
        class MouseEvent;
        class MouseMoveEvent;
        #endif
        class ScrollEvent;

        /* The damn thing cannot handle forward enum declarations */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        enum class Modifier: unsigned int;
        enum class Key: KeySym;
        enum class PointerEventSource: UnsignedByte;
        enum class Pointer: UnsignedByte;
        #endif

        /**
         * @brief Set of keyboard modifiers
         * @m_since_latest
         *
         * @see @ref InputEvent::modifiers(), @ref platform-windowed-key-events
         */
        typedef Containers::EnumSet<Modifier> Modifiers;

        /**
         * @brief Set of pointer types
         * @m_since_latest
         *
         * @see @ref KeyEvent::pointers(), @ref PointerMoveEvent::pointers(),
         *      @ref ScrollEvent::pointers(),
         *      @ref platform-windowed-pointer-events
         */
        typedef Containers::EnumSet<Pointer> Pointers;

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
         * @return Value for returning from @cpp main() @ce
         *
         * Calls @ref mainLoopIteration() in a loop until @ref exit() is
         * called. See @ref MAGNUM_GLXAPPLICATION_MAIN() or
         * @ref MAGNUM_XEGLAPPLICATION_MAIN() for usage information.
         */
        int exec();

        /**
         * @brief Run one iteration of application main loop
         * @return @cpp false @ce if @ref exit() was called and the application
         *      should exit, @cpp true @ce otherwise
         * @m_since{2020,06}
         *
         * Called internally from @ref exec(). If you want to have better
         * control over how the main loop behaves, you can call this function
         * yourself from your own `main()` function instead of it being called
         * automatically from @ref exec() / @ref MAGNUM_GLXAPPLICATION_MAIN()
         * / @ref MAGNUM_XEGLAPPLICATION_MAIN().
         */
        bool mainLoopIteration();

        /**
         * @brief Exit application
         * @param exitCode  The exit code the application should return
         *
         * When called from application constructor, it will cause the
         * application to exit immediately after constructor ends, without any
         * events being processed. Calling this function is recommended over
         * @ref std::exit() or @ref Corrade::Utility::Fatal "Fatal", which exit
         * without calling destructors on local scope. Note that, however, you
         * need to explicitly @cpp return @ce after calling it, as it can't
         * exit the constructor on its own:
         *
         * @snippet Platform.cpp exit-from-constructor
         *
         * When called from the main loop, the application exits cleanly
         * before next main loop iteration is executed.
         */
        void exit(int exitCode = 0) {
            _flags |= Flag::Exit;
            _exitCode = exitCode;
        }

    protected:
        /* Nobody will need to have (and delete) AbstractXApplication*, thus
           this is faster than public pure virtual destructor */
        ~AbstractXApplication();

        /**
         * @brief Create a window with given configuration for OpenGL context
         * @param configuration     Application configuration
         * @param glConfiguration   OpenGL context configuration
         *
         * Must be called only if the context wasn't created by the constructor
         * itself, i.e. when passing @ref NoCreate to it. Error message is
         * printed and the program exits if the context cannot be created, see
         * @ref tryCreate() for an alternative.
         */
        void create(const Configuration& configuration, const GLConfiguration& glConfiguration);

        /**
         * @brief Create a window with given configuration and OpenGL context
         *
         * Equivalent to calling @ref create(const Configuration&, const GLConfiguration&)
         * with default-constructed @ref GLConfiguration.
         */
        void create(const Configuration& configuration);

        /**
         * @brief Create a window with default configuration and OpenGL context
         *
         * Equivalent to calling @ref create(const Configuration&) with
         * default-constructed @ref Configuration.
         */
        void create();

        /**
         * @brief Try to create context with given configuration for OpenGL context
         *
         * Unlike @ref create(const Configuration&, const GLConfiguration&)
         * returns @cpp false @ce if the context cannot be created,
         * @cpp true @ce otherwise.
         */
        bool tryCreate(const Configuration& configuration, const GLConfiguration& glConfiguration);

        /**
         * @brief Try to create context with given configuration and OpenGL context
         *
         * Unlike @ref create(const Configuration&) returns @cpp false @ce if
         * the context cannot be created, @cpp true @ce otherwise.
         */
        bool tryCreate(const Configuration& configuration);

        /** @{ @name Screen handling */

    public:
        /**
         * @brief Window size
         *
         * Window size to which all input event coordinates can be related.
         * Same as @ref framebufferSize().
         */
        Vector2i windowSize() const { return _windowSize; }

        /**
         * @brief Framebuffer size
         *
         * Size of the default framebuffer. Same as @ref windowSize().
         */
        Vector2i framebufferSize() const { return _windowSize; }

    protected:
        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers();

        /** @copydoc Sdl2Application::redraw() */
        void redraw() { _flags |= Flag::Redraw; }

    private:
        /**
         * @brief Viewport event
         *
         * Called when window size changes. The default implementation does
         * nothing. If you want to respond to size changes, you should pass the
         * new size to @ref GL::DefaultFramebuffer::setViewport() (if using
         * OpenGL) and possibly elsewhere (to
         * @ref SceneGraph::Camera::setViewport(), other framebuffers...).
         *
         * Note that this function might not get called at all if the window
         * size doesn't change. You should configure the initial state of your
         * cameras, framebuffers etc. in application constructor rather than
         * relying on this function to be called. Size of the window can be
         * retrieved using @ref windowSize().
         * @see @ref platform-windowed-viewport-events
         */
        virtual void viewportEvent(ViewportEvent& event);

        /** @copydoc Sdl2Application::drawEvent() */
        virtual void drawEvent() = 0;

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Keyboard handling */

        /**
         * @brief Key press event
         *
         * Called when a key is pressed. Default implementation does nothing.
         * @see @ref platform-windowed-key-events
         */
        virtual void keyPressEvent(KeyEvent& event);

        /**
         * @brief Key release event
         *
         * Called when a key is released. Default implementation does nothing.
         * @see @ref platform-windowed-key-events
         */
        virtual void keyReleaseEvent(KeyEvent& event);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Pointer handling */

        /**
         * @brief Pointer press event
         * @m_since_latest
         *
         * Called when a mouse is pressed. Note that if at least one mouse
         * button is already pressed and another button gets pressed in
         * addition, @ref pointerMoveEvent() with the new combination is
         * called, not this function.
         *
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, default
         * implementation delegates to @ref mousePressEvent(). On builds with
         * deprecated functionality disabled, default implementation does
         * nothing.
         * @see @ref platform-windowed-pointer-events
         */
        virtual void pointerPressEvent(PointerEvent& event);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Mouse press event
         * @m_deprecated_since_latest Use @ref pointerPressEvent() instead,
         *      which is a better abstraction for covering both mouse and touch
         *      / pen input.
         *
         * Default implementation does nothing.
         */
        virtual CORRADE_DEPRECATED("use pointerPressEvent() instead") void mousePressEvent(MouseEvent& event);
        #endif

        /**
         * @brief Pointer release event
         * @m_since_latest
         *
         * Called when a mouse is released. Note that if multiple mouse buttons
         * are pressed and one of these is released, @ref pointerMoveEvent()
         * with the new combination is called, not this function.
         *
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, default
         * implementation delegates to @ref mouseReleaseEvent(). On builds with
         * deprecated functionality disabled, default implementation does
         * nothing.
         * @see @ref platform-windowed-pointer-events
         */
        virtual void pointerReleaseEvent(PointerEvent& event);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Mouse release event
         * @m_deprecated_since_latest Use @ref pointerReleaseEvent() instead,
         *      which is a better abstraction for covering both mouse and touch
         *      / pen input.
         *
         * Default implementation does nothing.
         */
        virtual CORRADE_DEPRECATED("use pointerReleaseEvent() instead") void mouseReleaseEvent(MouseEvent& event);
        #endif

        /**
         * @brief Pointer move event
         * @m_since_latest
         *
         * Called when any of the currently pressed pointers is moved or
         * changes its properties. Gets called also if the set of pressed mouse
         * buttons changes.
         *
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, default
         * implementation delegates to @ref mouseMoveEvent(), or if
         * @ref PointerMoveEvent::pointer() is not
         * @relativeref{Corrade,Containers::NullOpt}, to either
         * @ref mousePressEvent() or @ref mouseReleaseEvent(). On builds with
         * deprecated functionality disabled, default implementation does
         * nothing.
         * @see @ref platform-windowed-pointer-events
         */
        virtual void pointerMoveEvent(PointerMoveEvent& event);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Mouse move event
         * @m_deprecated_since_latest Use @ref pointerMoveEvent() instead,
         *      which is a better abstraction for covering both mouse and touch
         *      / pen input.
         *
         * Default implementation does nothing.
         */
        virtual CORRADE_DEPRECATED("use pointerMoveEvent() instead") void mouseMoveEvent(MouseMoveEvent& event);
        #endif

        /**
         * @brief Mouse scroll event
         * @m_since_latest
         *
         * Called when a scrolling device is used (mouse wheel or scrolling
         * area on a touchpad).
         *
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, default
         * implementation delegates to @ref mousePressEvent() and
         * @ref mouseReleaseEvent() with @ref MouseEvent::Button::WheelDown and
         * @ref MouseEvent::Button::WheelUp.
         * @see @ref platform-windowed-pointer-events
         */
        virtual void scrollEvent(ScrollEvent& event);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        explicit AbstractXApplication(Implementation::AbstractContextHandler<GLConfiguration, Display*, VisualID, Window>* contextHandler, const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration);

        explicit AbstractXApplication(Implementation::AbstractContextHandler<GLConfiguration, Display*, VisualID, Window>* contextHandler, const Arguments& arguments, NoCreateT);

    private:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /* Calls the base pointer*Event() in order to delegate to deprecated
           mouse events */
        template<class> friend class BasicScreenedApplication;
        template<class, bool> friend struct Implementation::ApplicationScrollEventMixin;
        #endif

        enum class Flag: unsigned int {
            Redraw = 1 << 0,
            Exit = 1 << 1
        };

        typedef Containers::EnumSet<Flag> Flags;
        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        Display* _display{};
        Window _window{};
        Atom _deleteWindow{};

        Containers::Pointer<Implementation::AbstractContextHandler<GLConfiguration, Display*, VisualID, Window>> _contextHandler;
        /* Has to be in an Optional because it gets explicitly destroyed before
           the GL context */
        Containers::Optional<Platform::GLContext> _context;
        int _exitCode = 0;

        /** @todo Get this from the created window */
        Vector2i _windowSize;

        Flags _flags;
};

/**
@brief Keyboard modifier
@m_since_latest

@see @ref Modifiers, @ref InputEvent::modifiers(),
    @ref platform-windowed-key-events
*/
enum class AbstractXApplication::Modifier: unsigned int {
    /**
     * Shift
     *
     * @see @ref KeyEvent::Key::LeftShift, @ref KeyEvent::Key::RightShift
     */
    Shift = ShiftMask,

    /**
     * Ctrl
     *
     * @see @ref KeyEvent::Key::LeftCtrl, @ref KeyEvent::Key::RightCtrl
     */
    Ctrl = ControlMask,

    /**
     * Alt
     *
     * @see @ref KeyEvent::Key::LeftAlt, @ref KeyEvent::Key::RightAlt
     */
    Alt = Mod1Mask,

    AltGr = Mod5Mask,           /**< AltGr */

    /**
     * Caps lock
     *
     * @see @ref KeyEvent::Key::CapsLock
     */
    CapsLock = LockMask,

    /**
     * Num lock
     *
     * @see @ref KeyEvent::Key::NumLock
     */
    NumLock = Mod2Mask
};

CORRADE_ENUMSET_OPERATORS(AbstractXApplication::Modifiers)

/**
@brief Key
@m_since_latest

@see @ref KeyEvent::key(), @ref platform-windowed-key-events
*/
enum class AbstractXApplication::Key: KeySym {
    /**
     * Left Shift
     * @m_since_latest
     *
     * @see @ref InputEvent::Modifier::Shift
     */
    LeftShift = XK_Shift_L,

    /**
     * Right Shift
     * @m_since_latest
     *
     * @see @ref InputEvent::Modifier::Shift
     */
    RightShift = XK_Shift_R,

    /**
     * Left Ctrl
     * @m_since_latest
     *
     * @see @ref InputEvent::Modifier::Ctrl
     */
    LeftCtrl = XK_Control_L,

    /**
     * Right Ctrl
     * @m_since_latest
     *
     * @see @ref InputEvent::Modifier::Ctrl
     */
    RightCtrl = XK_Control_R,

    /**
     * Left Alt
     * @m_since_latest
     *
     * @see @ref InputEvent::Modifier::Alt
     */
    LeftAlt = XK_Alt_L,

    /**
     * Right Alt
     * @m_since_latest
     *
     * @see @ref InputEvent::Modifier::Alt
     */
    RightAlt = XK_Alt_R,

    /**
     * Left Super key (Windows/⌘)
     * @m_since_latest
     */
    LeftSuper = XK_Super_L,

    /**
     * Right Super key (Windows/⌘)
     * @m_since_latest
     */
    RightSuper = XK_Super_R,

    /* AltGr missing */

    Enter = XK_Return,          /**< Enter */
    Esc = XK_Escape,            /**< Escape */

    Up = XK_Up,                 /**< Up arrow */
    Down = XK_Down,             /**< Down arrow */
    Left = XK_Left,             /**< Left arrow */
    Right = XK_Right,           /**< Right arrow */
    Home = XK_Home,             /**< Home */
    End = XK_End,               /**< End */
    PageUp = XK_Page_Up,        /**< Page up */
    PageDown = XK_Page_Down,    /**< Page down */

    /**
     * Backspace
     * @m_since_latest
     */
    Backspace = XK_BackSpace,

    /**
     * Insert
     * @m_since_latest
     */
    Insert = XK_Insert,

    /**
     * Delete
     * @m_since_latest
     */
    Delete = XK_Delete,

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

    Space = XK_space,           /**< Space */

    /**
     * Tab
     * @m_since_latest
     */
    Tab = XK_Tab,

    /**
     * Quote (<tt>'</tt>)
     * @m_since_latest
     */
    Quote = XK_apostrophe,

    Comma = XK_comma,           /**< Comma */
    Period = XK_period,         /**< Period */
    Minus = XK_minus,           /**< Minus */
    Plus = XK_plus,             /**< Plus */
    Slash = XK_slash,           /**< Slash */
    Percent = XK_percent,       /**< Percent */

    /**
     * Semicolon (`;`)
     * @m_since_latest
     */
    Semicolon = XK_semicolon,

    Equal = XK_equal,           /**< Equal */

    /**
     * Left bracket (`[`)
     * @m_since_latest
     */
    LeftBracket = XK_bracketleft,

    /**
     * Right bracket (`]`)
     * @m_since_latest
     */
    RightBracket = XK_bracketright,

    /**
     * Backslash (`\`)
     * @m_since_latest
     */
    Backslash = XK_backslash,

    /**
     * Backquote (<tt>`</tt>)
     * @m_since_latest
     */
    Backquote = XK_grave,

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
    Z = XK_z,                   /**< Small letter Z */

    /**
     * Caps lock
     *
     * @see @ref InputEvent::Modifier::CapsLock
     * @m_since_latest
     */
    CapsLock = XK_Caps_Lock,

    /**
     * Scroll lock
     * @m_since_latest
     */
    ScrollLock = XK_Scroll_Lock,

    /**
     * Num lock
     *
     * @see @ref InputEvent::Modifier::NumLock
     * @m_since_latest
     */
    NumLock = XK_Num_Lock,

    /**
     * Print screen
     * @m_since_latest
     */
    PrintScreen = XK_Print,

    /* Pause, Menu missing */

    /**
     * Numpad zero
     * @m_since_latest
     */
    NumZero = XK_KP_0,

    /**
     * Numpad one
     * @m_since_latest
     */
    NumOne = XK_KP_1,

    /**
     * Numpad two
     * @m_since_latest
     */
    NumTwo = XK_KP_2,

    /**
     * Numpad three
     * @m_since_latest
     */
    NumThree = XK_KP_3,

    /**
     * Numpad four
     * @m_since_latest
     */
    NumFour = XK_KP_4,

    /**
     * Numpad five
     * @m_since_latest
     */
    NumFive = XK_KP_5,

    /**
     * Numpad six
     * @m_since_latest
     */
    NumSix = XK_KP_6,

    /**
     * Numpad seven
     * @m_since_latest
     */
    NumSeven = XK_KP_7,

    /**
     * Numpad eight
     * @m_since_latest
     */
    NumEight = XK_KP_8,

    /**
     * Numpad nine
     * @m_since_latest
     */
    NumNine = XK_KP_9,

    /**
     * Numpad decimal
     * @m_since_latest
     */
    NumDecimal = XK_KP_Decimal,

    /**
     * Numpad divide
     * @m_since_latest
     */
    NumDivide = XK_KP_Divide,

    /**
     * Numpad multiply
     * @m_since_latest
     */
    NumMultiply = XK_KP_Multiply,

    /**
     * Numpad subtract
     * @m_since_latest
     */
    NumSubtract = XK_KP_Subtract,

    /**
     * Numpad add
     * @m_since_latest
     */
    NumAdd = XK_KP_Add,

    /**
     * Numpad enter
     * @m_since_latest
     */
    NumEnter = XK_KP_Enter,

    /**
     * Numpad equal
     * @m_since_latest
     */
    NumEqual = XK_KP_Equal
};

/**
@brief Pointer event source
@m_since_latest

@see @ref PointerEvent::source(), @ref PointerMoveEvent::source(),
    @ref platform-windowed-pointer-events
*/
enum class AbstractXApplication::PointerEventSource: UnsignedByte {
    /**
     * The event is coming from a mouse
     * @see @ref Pointer::MouseLeft, @ref Pointer::MouseMiddle,
     *      @ref Pointer::MouseRight
     */
    Mouse,
};

/**
@brief Pointer type
@m_since_latest

@see @ref Pointers, @ref KeyEvent::pointers(), @ref PointerEvent::pointer(),
    @ref PointerMoveEvent::pointer(), @ref PointerMoveEvent::pointers(),
    @ref ScrollEvent::pointers(), @ref platform-windowed-pointer-events
*/
enum class AbstractXApplication::Pointer: UnsignedByte {
    /**
     * Left mouse button. Corresponds to `Button1` / `Button1Mask`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseLeft = 1 << 0,

    /**
     * Middle mouse button. Corresponds to `Button2` / `Button2Mask`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseMiddle = 1 << 1,

    /**
     * Right mouse button. Corresponds to `Button3` / `Button3Mask`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseRight = 1 << 2
};

CORRADE_ENUMSET_OPERATORS(AbstractXApplication::Pointers)

/**
@brief OpenGL context configuration

Double-buffered OpenGL context.
@see @ref GlxApplication::GlxApplication(), @ref XEglApplication::XEglApplication(),
    @ref Configuration, @ref create(), @ref tryCreate()
@todo GLX_ARB_create_context_robustness/EGL_EXT_create_context_robustness
*/
class AbstractXApplication::GLConfiguration: public GL::Context::Configuration {
    public:
        /**
         * @brief Context flag
         *
         * Includes also everything from @ref GL::Context::Configuration::Flag
         * except for @relativeref{GL::Context::Configuration,Flag::Windowless},
         * which is not meant to be enabled for windowed apps.
         * @see @ref Flags, @ref setFlags(), @ref GL::Context::Flag
         */
        enum class Flag: UnsignedLong {
            /**
             * @copydoc GL::Context::Configuration::Flag::QuietLog
             * @m_since_latest
             */
            QuietLog = UnsignedLong(GL::Context::Configuration::Flag::QuietLog),

            /**
             * @copydoc GL::Context::Configuration::Flag::VerboseLog
             * @m_since_latest
             */
            VerboseLog = UnsignedLong(GL::Context::Configuration::Flag::VerboseLog),

            /**
             * @copydoc GL::Context::Configuration::Flag::GpuValidation
             * @m_since_latest
             */
            GpuValidation = UnsignedLong(GL::Context::Configuration::Flag::GpuValidation),

            /**
             * @copydoc GL::Context::Configuration::Flag::GpuValidationNoError
             * @m_since_latest
             */
            GpuValidationNoError = UnsignedLong(GL::Context::Configuration::Flag::GpuValidationNoError)
        };

        /**
         * @brief Context flags
         *
         * @see @ref setFlags(), @ref GL::Context::Flags
         */
        typedef Containers::EnumSet<Flag> Flags;

        explicit GLConfiguration();
        ~GLConfiguration();

        /** @brief Context flags */
        Flags flags() const {
            return Flag(UnsignedLong(GL::Context::Configuration::flags()));
        }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is no flag. To avoid clearing default flags by accident,
         * prefer to use @ref addFlags() and @ref clearFlags() instead.
         * @see @ref GL::Context::flags()
         */
        GLConfiguration& setFlags(Flags flags) {
            GL::Context::Configuration::setFlags(GL::Context::Configuration::Flag(UnsignedLong(flags)));
            return *this;
        }

        /**
         * @brief Add context flags
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref setFlags(), ORs the flags with existing instead of
         * replacing them. Useful for preserving the defaults.
         * @see @ref clearFlags()
         */
        GLConfiguration& addFlags(Flags flags) {
            GL::Context::Configuration::addFlags(GL::Context::Configuration::Flag(UnsignedLong(flags)));
            return *this;
        }

        /**
         * @brief Clear context flags
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref setFlags(), ANDs the inverse of @p flags with existing
         * instead of replacing them. Useful for removing default flags.
         * @see @ref addFlags()
         */
        GLConfiguration& clearFlags(Flags flags) {
            GL::Context::Configuration::clearFlags(GL::Context::Configuration::Flag(UnsignedLong(flags)));
            return *this;
        }

        /** @copydoc Sdl2Application::GLConfiguration::version() */
        GL::Version version() const { return _version; }

        /** @copydoc Sdl2Application::GLConfiguration::setVersion() */
        GLConfiguration& setVersion(GL::Version version) {
            _version = version;
            return *this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        MAGNUM_GL_CONTEXT_CONFIGURATION_SUBCLASS_IMPLEMENTATION(GLConfiguration)
        #endif

    private:
        GL::Version _version;
};

/**
@brief Configuration

@see @ref GlxApplication::GlxApplication(), @ref XEglApplication::XEglApplication(),
    @ref GLConfiguration, @ref create(), @ref tryCreate()
*/
class AbstractXApplication::Configuration {
    public:
        /*implicit*/ Configuration();
        ~Configuration();

        /**
         * @brief Window title
         *
         * The returned view is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * is valid until the next call to @ref setTitle().
         */
        Containers::StringView title() const { return _title; }

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp "Magnum X Application" @ce.
         */
        Configuration& setTitle(Containers::StringView title) {
            _title = Containers::String::nullTerminatedGlobalView(title);
            return *this;
        }

        /** @copydoc Sdl2Application::Configuration::size() */
        Vector2i size() const { return _size; }

        /**
         * @brief Set window size
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp {800, 600} @ce.
         */
        Configuration& setSize(const Vector2i& size) {
            _size = size;
            return *this;
        }

    private:
        Containers::String _title;
        Vector2i _size;
};

/**
@brief Viewport event

@see @ref viewportEvent(), @ref platform-windowed-viewport-events
*/
class AbstractXApplication::ViewportEvent {
    public:
        /** @brief Copying is not allowed */
        ViewportEvent(const ViewportEvent&) = delete;

        /** @brief Moving is not allowed */
        ViewportEvent(ViewportEvent&&) = delete;

        /** @brief Copying is not allowed */
        ViewportEvent& operator=(const ViewportEvent&) = delete;

        /** @brief Moving is not allowed */
        ViewportEvent& operator=(ViewportEvent&&) = delete;

        /**
         * @brief Window size
         *
         * Same as @ref framebufferSize().
         * @see @ref AbstractXApplication::windowSize()
         */
        Vector2i windowSize() const { return _size; }

        /**
         * @brief Framebuffer size
         *
         * Same as @ref windowSize().
         * @see @ref AbstractXApplication::framebufferSize()
         */
        Vector2i framebufferSize() const { return _size; }

    private:
        friend AbstractXApplication;

        explicit ViewportEvent(const Vector2i& size): _size{size} {}

        const Vector2i _size;
};

/**
@brief Base for input events

@see @ref KeyEvent, @ref PointerEvent, @ref PointerMoveEvent, @ref ScrollEvent,
    @ref keyPressEvent(), @ref keyReleaseEvent(), @ref pointerPressEvent(),
    @ref pointerReleaseEvent(), @ref pointerMoveEvent(), @ref scrollEvent()
*/
class AbstractXApplication::InputEvent {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief AbstractXApplication::Modifier
         * @m_deprecated_since_latest Use @ref AbstractXApplication::Modifier
         *      instead.
         */
        typedef CORRADE_DEPRECATED("use AbstractXApplication::Modifier instead") AbstractXApplication::Modifier Modifier;

        /**
         * @brief @copybrief AbstractXApplication::Modifiers
         * @m_deprecated_since_latest Use @ref AbstractXApplication::Modifiers
         *      instead.
         */
        typedef CORRADE_DEPRECATED("use AbstractXApplication::Modifiers instead") AbstractXApplication::Modifiers Modifiers;

        /**
         * @brief Mouse button
         * @m_deprecated_since_latest Use @ref Pointer instead.
         */
        enum class CORRADE_DEPRECATED_ENUM("use Pointer instead") Button: unsigned int {
            Left = Button1Mask,     /**< Left button */
            Middle = Button2Mask,   /**< Middle button */
            Right = Button3Mask     /**< Right button */
        };

        /**
         * @brief Set of mouse buttons
         * @m_deprecated_since_latest Use @ref Pointers instead.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH
        typedef CORRADE_DEPRECATED("use Pointers instead") Containers::EnumSet<Button> Buttons;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

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
        bool isAccepted() const { return _accepted; }

        /** @brief Keyboard modifiers */
        AbstractXApplication::Modifiers modifiers() const {
            return AbstractXApplication::Modifiers(_modifiers & (ShiftMask|ControlMask|Mod1Mask|Mod5Mask|LockMask|Mod2Mask));
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Mouse buttons
         * @m_deprecated_since_latest Use @ref KeyEvent::pointers() or
         *      @ref PointerMoveEvent::pointers() instead.
         */
        CORRADE_DEPRECATED("use pointers() instead") Buttons buttons() const {
            return Buttons(_modifiers & (Button1Mask|Button2Mask|Button3Mask));
        }
        #endif

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        explicit InputEvent(unsigned int modifiers): _modifiers(modifiers), _accepted(false) {}

        ~InputEvent() = default;
    #endif

    private:
        friend AbstractXApplication;

        unsigned int _modifiers;
        bool _accepted;
};

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_ENUMSET_OPERATORS(AbstractXApplication::InputEvent::Buttons)
CORRADE_IGNORE_DEPRECATED_POP
#endif

/**
@brief Key event

@see @ref keyPressEvent(), @ref keyReleaseEvent(),
    @ref platform-windowed-key-events
*/
class AbstractXApplication::KeyEvent: public AbstractXApplication::InputEvent {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief AbstractXApplication::Key
         * @m_deprecated_since_latest Use @ref AbstractXApplication::Key
         *      instead.
         */
        typedef CORRADE_DEPRECATED("use AbstractXApplication::Key instead") AbstractXApplication::Key Key;
        #endif

        /** @brief Key */
        AbstractXApplication::Key key() const { return _key; }

        /** @brief Position */
        Vector2i position() const { return _position; }

        /**
         * @brief Pointer types pressed in this event
         * @m_since_latest
         *
         * Returns an empty set if no pointers are pressed in addition to the
         * key.
         */
        Pointers pointers() const;

    private:
        friend AbstractXApplication;

        explicit KeyEvent(AbstractXApplication::Key key, unsigned int modifiers, const Vector2i& position): InputEvent(modifiers), _key(key), _position(position) {}

        const AbstractXApplication::Key _key;
        const Vector2i _position;
};

/**
@brief Pointer event
@m_since_latest

@see @ref PointerMoveEvent, @ref pointerPressEvent(),
    @ref pointerReleaseEvent(), @ref platform-windowed-pointer-events
*/
class AbstractXApplication::PointerEvent: public InputEvent {
    public:
        /** @brief Copying is not allowed */
        PointerEvent(const PointerEvent&) = delete;

        /** @brief Moving is not allowed */
        PointerEvent(PointerEvent&&) = delete;

        /** @brief Copying is not allowed */
        PointerEvent& operator=(const PointerEvent&) = delete;

        /** @brief Moving is not allowed */
        PointerEvent& operator=(PointerEvent&&) = delete;

        /**
         * @brief Pointer event source
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @ref PointerEventSource::Mouse.
         */
        PointerEventSource source() const { return PointerEventSource::Mouse; }

        /** @brief Pointer type that was pressed or released */
        Pointer pointer() const { return _pointer; }

        /**
         * @brief Whether the pointer is primary
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @cpp true @ce.
         */
        bool isPrimary() const { return true; }

        /**
         * @brief Pointer ID
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @cpp 0 @ce.
         */
        Long id() const { return 0; }

        /**
         * @brief Position
         *
         * For mouse input the position is always reported in whole pixels.
         */
        Vector2 position() const { return _position; }

    private:
        friend AbstractXApplication;

        explicit PointerEvent(Pointer pointer, const Vector2& position, unsigned int modifiers): InputEvent{modifiers}, _pointer(pointer), _position{position} {}

        const Pointer _pointer;
        const Vector2 _position;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Mouse event
@m_deprecated_since_latest Use @ref PointerEvent, @ref pointerPressEvent() and
    @ref pointerReleaseEvent() instead, which is a better abstraction for
    covering both mouse and touch / pen input.

@see @ref MouseMoveEvent, @ref ScrollEvent, @ref mousePressEvent(),
    @ref mouseReleaseEvent()
*/
class CORRADE_DEPRECATED("use PointerEvent, pointerPressEvent() and pointerReleaseEvent() instead") AbstractXApplication::MouseEvent: public AbstractXApplication::InputEvent {
    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: unsigned int {
            Left      = 1 /*Button1*/,  /**< Left button */
            Middle    = 2 /*Button2*/,  /**< Middle button */
            Right     = 3 /*Button3*/,  /**< Right button */

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Wheel up
             * @m_deprecated_since_latest Implement @ref scrollEvent() instead.
             */
            WheelUp CORRADE_DEPRECATED_ENUM("implement scrollEvent() instead") = 4,

            /**
             * Wheel down
             * @m_deprecated_since_latest Implement @ref scrollEvent() instead.
             */
            WheelDown CORRADE_DEPRECATED_ENUM("implement scrollEvent() instead") = 5
            #endif
        };

        /** @brief Button */
        Button button() const { return _button; }

        /** @brief Position */
        Vector2i position() const { return _position; }

    private:
        friend AbstractXApplication;

        explicit MouseEvent(Button button, unsigned int modifiers, const Vector2i& position): InputEvent(modifiers), _button(button), _position(position) {}

        const Button _button;
        const Vector2i _position;
};
#endif

/**
@brief Pointer move event
@m_since_latest

@see @ref PointerEvent, @ref pointerMoveEvent(),
    @ref platform-windowed-pointer-events
*/
class AbstractXApplication::PointerMoveEvent: public InputEvent {
    public:
        /** @brief Copying is not allowed */
        PointerMoveEvent(const PointerMoveEvent&) = delete;

        /** @brief Moving is not allowed */
        PointerMoveEvent(PointerMoveEvent&&) = delete;

        /** @brief Copying is not allowed */
        PointerMoveEvent& operator=(const PointerMoveEvent&) = delete;

        /** @brief Moving is not allowed */
        PointerMoveEvent& operator=(PointerMoveEvent&&) = delete;

        /**
         * @brief Pointer event source
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @ref PointerEventSource::Mouse.
         */
        PointerEventSource source() const { return PointerEventSource::Mouse; }

        /**
         * @brief Pointer type that was added or removed from the set of pressed pointers
         *
         * Use @ref pointers() to query the set of pointers pressed in this
         * event. This field is is non-empty only in case a mouse button was
         * pressed in addition to an already pressed button, or if one mouse
         * button from multiple pressed buttons was released. If non-empty and
         * @ref pointers() don't contain given @ref Pointer value, the button
         * was released, if they contain given value, the button was pressed.
         * @see @ref source()
         */
        Containers::Optional<Pointer> pointer() const { return _pointer; }

        /**
         * @brief Pointer types pressed in this event
         *
         * Returns an empty set if no pointers are pressed, which happens for
         * example when a mouse is just moved around.
         * @see @ref pointer()
         */
        Pointers pointers() const { return _pointers; }

        /**
         * @brief Whether the pointer is primary
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @cpp true @ce.
         */
        bool isPrimary() const { return true; }

        /**
         * @brief Pointer ID
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @cpp 0 @ce.
         */
        Long id() const { return 0; }

        /**
         * @brief Position
         *
         * For mouse input the position is always reported in whole pixels.
         */
        Vector2 position() const { return _position; }

    private:
        friend AbstractXApplication;

        explicit PointerMoveEvent(Containers::Optional<Pointer> pointer, Pointers pointers, const Vector2& position, unsigned int modifiers): InputEvent{modifiers}, _pointer{pointer}, _pointers{pointers}, _position{position} {}

        const Containers::Optional<Pointer> _pointer;
        Pointers _pointers;
        const Vector2 _position;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Mouse move event
@m_deprecated_since_latest Use @ref PointerMoveEvent and
    @ref pointerMoveEvent() instead, which is a better abstraction for covering
    both mouse and touch / pen input.

@see @ref MouseEvent, @ref ScrollEvent, @ref mouseMoveEvent()
*/
class CORRADE_DEPRECATED("use PointerMoveEvent and pointerMoveEvent() instead") AbstractXApplication::MouseMoveEvent: public AbstractXApplication::InputEvent {
    public:
        /** @brief Position */
        Vector2i position() const { return _position; }

    private:
        friend AbstractXApplication;

        explicit MouseMoveEvent(unsigned int modifiers, const Vector2i& position): InputEvent(modifiers), _position(position) {}

        const Vector2i _position;
};
#endif

/**
@brief Scroll event
@m_since_latest

@see @ref PointerEvent, @ref PointerMoveEvent, @ref scrollEvent(),
    @ref platform-windowed-pointer-events
*/
class AbstractXApplication::ScrollEvent: public InputEvent {
    public:
        /**
         * @brief Scroll offset
         *
         * Is always either @cpp -1.0f @ce or @cpp +1.0f @ce.
         */
        Vector2 offset() const { return _offset; }

        /**
         * @brief Position
         *
         * For mouse input the position is always reported in whole pixels.
         */
        Vector2 position() const { return _position; }

        /**
         * @brief Pointer types pressed in this event
         *
         * Returns an empty set if no pointers are pressed in addition to the
         * mouse wheel.
         */
        Pointers pointers() const;

    private:
        friend AbstractXApplication;

        explicit ScrollEvent(const Vector2& offset, const Vector2& position, unsigned int modifiers): InputEvent{modifiers}, _offset{offset}, _position{position} {}

        const Vector2 _offset;
        const Vector2 _position;
};

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
