#ifndef Magnum_Platform_GlfwApplication_h
#define Magnum_Platform_GlfwApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Class @ref Magnum::Platform::GlfwApplication, macro @ref MAGNUM_GLFWAPPLICATION_MAIN()
 */

#include <memory>
#include <string>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Platform/Platform.h"

/* We must include our own GL headers first to avoid conflicts */
#include "Magnum/OpenGL.h"

#include <glfw3.h>

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief GLFW application

Application using GLFW toolkit. Supports keyboard and mouse handling with
support for changing cursor and mouse tracking and warping.

This application library is available on all platforms where GLFW is ported. It
depends on **GLFW** library and is built if `WITH_GLFWAPPLICATION` is enabled
in CMake.

## Bootstrap application

Fully contained base application using @ref GlfwApplication along with CMake
setup is available in `base-glfw` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-glfw.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-glfw.zip) file.
After extracting the downloaded archive you can build and run the application
with these four commands:

    mkdir build && cd build
    cmake ..
    cmake --build .
    ./src/MyApplication # or ./src/Debug/MyApplication

See @ref cmake for more information.

## General usage

In CMake you need to request `GlfwApplication` component of `Magnum` package
and link to `Magnum::GlfwApplication` target. If no other application is
requested, you can also use generic `Magnum::Application` alias to simplify
porting. Again, see @ref building and @ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass can be then used directly in `main()` -- see
convenience macro @ref MAGNUM_GLFWAPPLICATION_MAIN(). See @ref platform for
more information.
@code
class MyApplication: public Platform::GlfwApplication {
    // implement required methods...
};
MAGNUM_GLFWAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.
*/
class GlfwApplication {
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
        class MouseScrollEvent;
        class TextInputEvent;

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, const Configuration&) */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit GlfwApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit GlfwApplication(const Arguments& arguments, const Configuration& configuration);
        explicit GlfwApplication(const Arguments& arguments);
        #endif

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, NoCreateT) */
        explicit GlfwApplication(const Arguments& arguments, NoCreateT);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief GlfwApplication(const Arguments&, NoCreateT)
         * @deprecated Use @ref GlfwApplication(const Arguments&, NoCreateT) instead.
         */
        CORRADE_DEPRECATED("use GlfwApplication(const Arguments&, NoCreateT) instead") explicit GlfwApplication(const Arguments& arguments, std::nullptr_t): GlfwApplication{arguments, NoCreate} {}
        #endif

        /** @brief Copying is not allowed */
        GlfwApplication(const GlfwApplication&) = delete;

        /** @brief Moving is not allowed */
        GlfwApplication(GlfwApplication&&) = delete;

        /** @brief Copying is not allowed */
        GlfwApplication& operator=(const GlfwApplication&) = delete;

        /** @brief Moving is not allowed */
        GlfwApplication& operator=(GlfwApplication&&) = delete;

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`
         *
         * See @ref MAGNUM_GLFWAPPLICATION_MAIN() for usage information.
         */
        int exec();

        /** @brief Exit application main loop */
        void exit() {
            glfwSetWindowShouldClose(_window, true);
        }

        /**
         * @brief Underlying window handle
         *
         * Use in case you need to call GLFW functionality directly.
         */
        GLFWwindow* window() { return _window; }

    protected:
        /* Nobody will need to have (and delete) GlfwApplication*, thus this is
           faster than public pure virtual destructor */
        ~GlfwApplication();

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

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Window size
         *
         * Window size to which all input event coordinates can be related.
         * Note that especially on HiDPI systems the reported window size might
         * not be the same as framebuffer size.
         */
        Vector2i windowSize();
        #endif

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers() { glfwSwapBuffers(_window); }

        /**
         * @brief Set swap interval
         *
         * Set `0` for no VSync, `1` for enabled VSync. Some platforms support
         * `-1` for late swap tearing. Default is driver-dependent.
         */
        void setSwapInterval(Int interval);

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

    public:
        /** @brief Warp mouse cursor to given coordinates */
        void warpCursor(const Vector2i& position) {
            glfwSetCursorPos(_window, Double(position.x()), Double(position.y()));
        }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @copydoc Sdl2Application::mousePressEvent() */
        virtual void mousePressEvent(MouseEvent& event);

        /** @copydoc Sdl2Application::mouseReleaseEvent() */
        virtual void mouseReleaseEvent(MouseEvent& event);

        /**
         * @brief Mouse move event
         *
         * Called when any mouse button is pressed and mouse is moved. Default
         * implementation does nothing.
         */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

        /** @copydoc Sdl2Application::mouseScrollEvent() */
        virtual void mouseScrollEvent(MouseScrollEvent& event);

        /*@}*/

        /** @{ @name Text input handling */
    public:
        /**
         * @brief Whether text input is active
         *
         * If text input is active, text input events go to
         * @ref textInputEvent().
         * @see @ref startTextInput(), @ref stopTextInput()
         */
        bool isTextInputActive() const { return !!(_flags & Flag::TextInputActive); }

        /**
         * @brief Start text input
         *
         * Starts text input that will go to @ref textInputEvent().
         * @see @ref stopTextInput(), @ref isTextInputActive()
         */
        void startTextInput() { _flags |= Flag::TextInputActive; }

        /**
         * @brief Stop text input
         *
         * Stops text input that went to @ref textInputEvent().
         * @see @ref startTextInput(), @ref isTextInputActive(),
         *      @ref textInputEvent()
         */
        void stopTextInput() { _flags &= ~Flag::TextInputActive; }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Text input event
         *
         * Called when text input is active and the text is being input.
         * @see @ref isTextInputActive()
         */
        virtual void textInputEvent(TextInputEvent& event);

        /*@}*/

    private:
        enum class Flag: UnsignedByte {
            Redraw = 1 << 0,
            TextInputActive = 1 << 1
        };

        typedef Containers::EnumSet<Flag> Flags;
        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        static void staticViewportEvent(GLFWwindow*, int w, int h) {
            _instance->viewportEvent({w, h});
        }

        static void staticKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mod);

        static void staticMouseEvent(GLFWwindow* window, int button, int action, int mods);

        static void staticMouseMoveEvent(GLFWwindow* window, double x, double y);

        static void staticMouseScrollEvent(GLFWwindow* window, double xoffset, double yoffset);

        static void staticErrorCallback(int error, const char* description);

        static void staticTextInputEvent(GLFWwindow* window, unsigned int codepoint);

        static GlfwApplication* _instance;

        GLFWwindow* _window;
        std::unique_ptr<Platform::Context> _context;
        Flags _flags;
};

CORRADE_ENUMSET_OPERATORS(GlfwApplication::Flags)

/**
@brief Configuration

Double-buffered RGBA window with depth and stencil buffers.
@see @ref GlfwApplication(), @ref createContext(), @ref tryCreateContext()
*/
class GlfwApplication::Configuration {
    public:
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref setFlags(), @ref Context::Flag
         */
        enum class Flag: Int {
            #if defined(DOXYGEN_GENERATING_OUTPUT) || defined(GLFW_CONTEXT_NO_ERROR)
            /**
             * Specifies whether errors should be generated by the context.
             * If enabled, situations that would have generated errors instead
             * cause undefined behavior.
             *
             * @note Supported since GLFW 3.2.
             */
            NoError = GLFW_CONTEXT_NO_ERROR,
            #endif

            Debug = GLFW_OPENGL_DEBUG_CONTEXT,  /**< Debug context */
            Stereo = GLFW_STEREO,               /**< Stereo rendering */
        };

        /**
         * @brief Context flags
         *
         * @see @ref setFlags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Window flag
         *
         * @see @ref WindowFlags, @ref setWindowFlags()
         */
        enum class WindowFlag: UnsignedShort {
            Fullscreen = 1 << 0,   /**< Fullscreen window */
            Resizable = 1 << 1,    /**< Resizable window */

            #ifdef MAGNUM_BUILD_DEPRECATED
            /** @copydoc WindowFlag::Resizable
             * @deprecated Use @ref WindowFlag::Resizable instead.
             */
            Resizeable CORRADE_DEPRECATED_ENUM("use WindowFlag::Resizable instead") = UnsignedShort(WindowFlag::Resizable),
            #endif

            Hidden = 1 << 2,       /**< Hidden window */

            #if defined(DOXYGEN_GENERATING_OUTPUT) || defined(GLFW_MAXIMIZED)
            /**
             * Maximized window
             *
             * @note Supported since GLFW 3.2.
             */
            Maximized = 1 << 3,
            #endif

            Minimized = 1 << 4,    /**< Minimized window */
            Floating = 1 << 5,     /**< Window floating above others, top-most */

            /**
             * Automatically iconify (minimize) if fullscreen window loses
             * input focus
             */
            AutoIconify = 1 << 6,

            Focused = 1 << 7       /**< Window has input focus */
        };

        /**
         * @brief Window flags
         *
         * @see @ref setWindowFlags()
         */
        typedef Containers::EnumSet<WindowFlag> WindowFlags;

        /** @brief Cursor mode */
        enum class CursorMode: Int {
            /** Visible unconstrained cursor */
            Normal = GLFW_CURSOR_NORMAL,

            /** Hidden cursor */
            Hidden = GLFW_CURSOR_HIDDEN,

            /** Cursor hidden and locked window */
            Disabled = GLFW_CURSOR_DISABLED
        };

        /*implicit*/ Configuration();
        ~Configuration();

        /** @brief Window title */
        std::string title() const { return _title; }

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * Default is `"Magnum GLFW Application"`.
         */
        Configuration& setTitle(std::string title) {
            _title = std::move(title);
            return *this;
        }

        /** @brief Window size */
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

        /** @brief Context flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is no flag.
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        /** @brief Window flags */
        WindowFlags windowFlags() const {
            return _windowFlags;
        }

        /**
         * @brief Set window flags
         * @return  Reference to self (for method chaining)
         *
         * Default is @ref WindowFlag::Focused.
         */
        Configuration& setWindowFlags(WindowFlags windowFlags) {
            _windowFlags = windowFlags;
            return *this;
        }

        /** @brief Cursor mode */
        CursorMode cursorMode() const {
            return _cursorMode;
        }

        /**
         * @brief Set cursor mode
         * @return  Reference to self (for method chaining)
         *
         * Default is @ref CursorMode::Normal.
         */
        Configuration& setCursorMode(CursorMode cursorMode) {
            _cursorMode = cursorMode;
            return *this;
        }

        /** @brief Context version */
        Version version() const { return _version; }

        /**
         * @brief Set context version
         *
         * If requesting version greater or equal to OpenGL 3.1, core profile
         * is used. The created context will then have any version which is
         * backwards-compatible with requested one. Default is
         * @ref Version::None, i.e. any provided version is used.
         */
        Configuration& setVersion(Version version) {
            _version = version;
            return *this;
        }

        /** @brief Sample count */
        Int sampleCount() const { return _sampleCount; }

        /**
         * @brief Set sample count
         * @return Reference to self (for method chaining)
         *
         * Default is `0`, thus no multisampling. The actual sample count is
         * ignored, GLFW either enables it or disables. See also
         * @ref Renderer::Feature::Multisampling.
         */
        Configuration& setSampleCount(Int count) {
            _sampleCount = count;
            return *this;
        }

        /** @brief sRGB-capable default framebuffer */
        bool isSRGBCapable() const {
            return _srgbCapable;
        }

        /**
         * @brief Set sRGB-capable default framebuffer
         * @return Reference to self (for method chaining)
         */
        Configuration& setSRGBCapable(bool enabled) {
            _srgbCapable = enabled;
            return *this;
        }

    private:
        std::string _title;
        Vector2i _size;
        Int _sampleCount;
        Version _version;
        Flags _flags;
        WindowFlags _windowFlags;
        CursorMode _cursorMode;
        bool _srgbCapable;
};

CORRADE_ENUMSET_OPERATORS(GlfwApplication::Configuration::Flags)
CORRADE_ENUMSET_OPERATORS(GlfwApplication::Configuration::WindowFlags)

/**
@brief Base for input events

@see @ref KeyEvent, @ref MouseEvent, @ref MouseMoveEvent, @ref keyPressEvent(),
    @ref mousePressEvent(), @ref mouseReleaseEvent(), @ref mouseMoveEvent()
*/
class GlfwApplication::InputEvent {
    public:
        /**
         * @brief Modifier
         *
         * @see @ref Modifiers, @ref KeyEvent::modifiers(),
         *      @ref MouseEvent::modifiers()
         */
        enum class Modifier: Int {
            /**
             * Shift
             *
             * @see @ref KeyEvent::Key::LeftShift, @ref KeyEvent::Key::RightShift
             */
            Shift = GLFW_MOD_SHIFT,

            /**
             * Ctrl
             *
             * @see @ref KeyEvent::Key::LeftCtrl, @ref KeyEvent::Key::RightCtrl
             */
            Ctrl = GLFW_MOD_CONTROL,

            /**
             * Alt
             *
             * @see @ref KeyEvent::Key::LeftAlt, @ref KeyEvent::Key::RightAlt
             */
            Alt = GLFW_MOD_ALT,

            /**
             * Super key (Windows/⌘)
             *
             * @see @ref KeyEvent::Key::LeftSuper, @ref KeyEvent::Key::RightSuper
             */
            Super = GLFW_MOD_SUPER
        };

        /**
         * @brief Set of modifiers
         *
         * @see @ref KeyEvent::modifiers(), @ref MouseEvent::modifiers(),
         *      @ref MouseMoveEvent::modifiers()
         */
        typedef Containers::EnumSet<Modifier> Modifiers;

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

    protected:
        constexpr InputEvent(): _accepted(false) {}

        ~InputEvent() = default;

    private:
        bool _accepted;
};

CORRADE_ENUMSET_OPERATORS(GlfwApplication::InputEvent::Modifiers)

/**
@brief Key event

@see @ref keyPressEvent(), @ref keyReleaseEvent()
*/
class GlfwApplication::KeyEvent: public GlfwApplication::InputEvent {
    friend GlfwApplication;

    public:
        /**
         * @brief Key
         *
         * @see @ref key()
         */
        enum class Key: Int {
            Unknown = GLFW_KEY_UNKNOWN,         /**< Unknown key */

            /**
             * Left Shift
             *
             * @see @ref InputEvent::Modifier::Shift
             */
            LeftShift = GLFW_KEY_LEFT_SHIFT,

            /**
             * Right Shift
             *
             * @see @ref InputEvent::Modifier::Shift
             */
            RightShift = GLFW_KEY_RIGHT_SHIFT,

            /**
             * Left Ctrl
             *
             * @see @ref InputEvent::Modifier::Ctrl
             */
            LeftCtrl = GLFW_KEY_LEFT_CONTROL,

            /**
             * Right Ctrl
             *
             * @see @ref InputEvent::Modifier::Ctrl
             */
            RightCtrl = GLFW_KEY_RIGHT_CONTROL,

            /**
             * Left Alt
             *
             * @see @ref InputEvent::Modifier::Alt
             */
            LeftAlt = GLFW_KEY_LEFT_ALT,

            /**
             * Right Alt
             *
             * @see @ref InputEvent::Modifier::Alt
             */
            RightAlt = GLFW_KEY_RIGHT_ALT,

            /**
             * Left Super key (Windows/⌘)
             *
             * @see @ref InputEvent::Modifier::Super
             */
            LeftSuper = GLFW_KEY_LEFT_SUPER,

            /**
             * Right Super key (Windows/⌘)
             *
             * @see @ref InputEvent::Modifier::Super
             */
            RightSuper = GLFW_KEY_RIGHT_SUPER,

            Enter = GLFW_KEY_ENTER,             /**< Enter */
            Esc = GLFW_KEY_ESCAPE,              /**< Escape */

            Up = GLFW_KEY_UP,                   /**< Up arrow */
            Down = GLFW_KEY_DOWN,               /**< Down arrow */
            Left = GLFW_KEY_LEFT,               /**< Left arrow */
            Right = GLFW_KEY_RIGHT,             /**< Right arrow */
            Home = GLFW_KEY_HOME,               /**< Home */
            End = GLFW_KEY_END,                 /**< End */
            PageUp = GLFW_KEY_PAGE_UP,          /**< Page up */
            PageDown = GLFW_KEY_PAGE_DOWN,      /**< Page down */
            Backspace = GLFW_KEY_BACKSPACE,     /**< Backspace */
            Insert = GLFW_KEY_INSERT,           /**< Insert */
            Delete = GLFW_KEY_DELETE,           /**< Delete */

            F1 = GLFW_KEY_F1,                   /**< F1 */
            F2 = GLFW_KEY_F2,                   /**< F2 */
            F3 = GLFW_KEY_F3,                   /**< F3 */
            F4 = GLFW_KEY_F4,                   /**< F4 */
            F5 = GLFW_KEY_F5,                   /**< F5 */
            F6 = GLFW_KEY_F6,                   /**< F6 */
            F7 = GLFW_KEY_F7,                   /**< F7 */
            F8 = GLFW_KEY_F8,                   /**< F8 */
            F9 = GLFW_KEY_F9,                   /**< F9 */
            F10 = GLFW_KEY_F10,                 /**< F10 */
            F11 = GLFW_KEY_F11,                 /**< F11 */
            F12 = GLFW_KEY_F12,                 /**< F12 */

            Space = GLFW_KEY_SPACE,             /**< Space */
            Tab = GLFW_KEY_TAB,                 /**< Tab */
            Comma = GLFW_KEY_COMMA,             /**< Comma */
            Period = GLFW_KEY_PERIOD,           /**< Period */
            Minus = GLFW_KEY_MINUS,             /**< Minus */
            /* Note: This may only be represented as SHIFT + = */
            Plus = '+',                         /**< Plus */
            Slash = GLFW_KEY_SLASH,             /**< Slash */
            /* Note: This may only be represented as SHIFT + 5 */
            Percent = '%',                      /**< Percent */
            Smicolon = GLFW_KEY_SEMICOLON,      /**< Semicolon */
            Equal = GLFW_KEY_EQUAL,             /**< Equal */

            Zero = GLFW_KEY_0,                  /**< Zero */
            One = GLFW_KEY_1,                   /**< One */
            Two = GLFW_KEY_2,                   /**< Two */
            Three = GLFW_KEY_3,                 /**< Three */
            Four = GLFW_KEY_4,                  /**< Four */
            Five = GLFW_KEY_5,                  /**< Five */
            Six = GLFW_KEY_6,                   /**< Six */
            Seven = GLFW_KEY_7,                 /**< Seven */
            Eight = GLFW_KEY_8,                 /**< Eight */
            Nine = GLFW_KEY_9,                  /**< Nine */

            A = GLFW_KEY_A,                     /**< Letter A */
            B = GLFW_KEY_B,                     /**< Letter B */
            C = GLFW_KEY_C,                     /**< Letter C */
            D = GLFW_KEY_D,                     /**< Letter D */
            E = GLFW_KEY_E,                     /**< Letter E */
            F = GLFW_KEY_F,                     /**< Letter F */
            G = GLFW_KEY_G,                     /**< Letter G */
            H = GLFW_KEY_H,                     /**< Letter H */
            I = GLFW_KEY_I,                     /**< Letter I */
            J = GLFW_KEY_J,                     /**< Letter J */
            K = GLFW_KEY_K,                     /**< Letter K */
            L = GLFW_KEY_L,                     /**< Letter L */
            M = GLFW_KEY_M,                     /**< Letter M */
            N = GLFW_KEY_N,                     /**< Letter N */
            O = GLFW_KEY_O,                     /**< Letter O */
            P = GLFW_KEY_P,                     /**< Letter P */
            Q = GLFW_KEY_Q,                     /**< Letter Q */
            R = GLFW_KEY_R,                     /**< Letter R */
            S = GLFW_KEY_S,                     /**< Letter S */
            T = GLFW_KEY_T,                     /**< Letter T */
            U = GLFW_KEY_U,                     /**< Letter U */
            V = GLFW_KEY_V,                     /**< Letter V */
            W = GLFW_KEY_W,                     /**< Letter W */
            X = GLFW_KEY_X,                     /**< Letter X */
            Y = GLFW_KEY_Y,                     /**< Letter Y */
            Z = GLFW_KEY_Z,                     /**< Letter Z */

            /* Function keys */
            CapsLock = GLFW_KEY_CAPS_LOCK,      /**< Caps lock */
            ScrollLock = GLFW_KEY_SCROLL_LOCK,  /**< Scroll lock */
            NumLock = GLFW_KEY_NUM_LOCK,        /**< Num lock */
            PrintScreen = GLFW_KEY_PRINT_SCREEN,/**< Print screen */
            Pause = GLFW_KEY_PAUSE,             /**< Pause */
            NumZero = GLFW_KEY_KP_0,            /**< Numpad zero */
            NumOne = GLFW_KEY_KP_1,             /**< Numpad one */
            NumTwo = GLFW_KEY_KP_2,             /**< Numpad two */
            NumThree = GLFW_KEY_KP_3,           /**< Numpad three */
            NumFour = GLFW_KEY_KP_4,            /**< Numpad four */
            NumFive = GLFW_KEY_KP_5,            /**< Numpad five */
            NumSix = GLFW_KEY_KP_6,             /**< Numpad six */
            NumSeven = GLFW_KEY_KP_7,           /**< Numpad seven */
            NumEight = GLFW_KEY_KP_8,           /**< Numpad eight */
            NumNine = GLFW_KEY_KP_9,            /**< Numpad nine */
            NumDecimal = GLFW_KEY_KP_DECIMAL,   /**< Numpad decimal */
            NumDivide = GLFW_KEY_KP_DIVIDE,     /**< Numpad divide */
            NumMultiply = GLFW_KEY_KP_MULTIPLY, /**< Numpad multiply */
            NumSubtract = GLFW_KEY_KP_SUBTRACT, /**< Numpad subtract */
            NumAdd = GLFW_KEY_KP_ADD,           /**< Numpad add */
            NumEnter = GLFW_KEY_KP_ENTER,       /**< Numpad enter */
            NumEqual = GLFW_KEY_KP_EQUAL,       /**< Numpad equal */
            Menu = GLFW_KEY_MENU                /**< Menu */
        };

        #if defined(DOXYGEN_GENERATING_OUTPUT) || GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 302
        /**
         * @brief Name for given key
         *
         * Human-readable localized UTF-8 name for given @p key, intended for
         * displaying to the user in e.g. key binding configuration. If there
         * is no name for given key, empty string is returned.
         * @see @ref keyName(Key)
         * @note Supported since GLFW 3.2.
         */
        static std::string keyName(Key key);
        #endif

        /** @copydoc Sdl2Application::KeyEvent::key() */
        constexpr Key key() const { return _key; }

        #if defined(DOXYGEN_GENERATING_OUTPUT) || GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 302
        /**
         * @brief Key name
         *
         * Human-readable localized UTF-8 name for the key returned by
         * @ref key(), intended for displaying to the user in e.g.
         * key binding configuration. If there is no name for that key, empty
         * string is returned.
         * @see @ref keyName(Key)
         * @note Supported since GLFW 3.2.
         */
        std::string keyName() const;
        #endif

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

        /** @copydoc Sdl2Application::KeyEvent::isRepeated() */
        constexpr bool isRepeated() const { return _repeated; }

    private:
        static Modifiers getCurrentGlfwModifiers(GLFWwindow* window);

        constexpr KeyEvent(Key key, Modifiers modifiers, bool repeated): _key{key}, _modifiers{modifiers}, _repeated{repeated} {}

        const Key _key;
        const Modifiers _modifiers;
        const bool _repeated;
};

/**
@brief Mouse event

@see @ref MouseMoveEvent, @ref MouseScrollEvent, @ref mousePressEvent(),
    @ref mouseReleaseEvent()
*/
class GlfwApplication::MouseEvent: public GlfwApplication::InputEvent {
    friend GlfwApplication;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: int {
            Left = GLFW_MOUSE_BUTTON_LEFT,        /**< Left button */
            Middle = GLFW_MOUSE_BUTTON_MIDDLE,    /**< Middle button */
            Right = GLFW_MOUSE_BUTTON_RIGHT,      /**< Right button */
            Button1 = GLFW_MOUSE_BUTTON_1,        /**< Mouse button 1 */
            Button2 = GLFW_MOUSE_BUTTON_2,        /**< Mouse button 2 */
            Button3 = GLFW_MOUSE_BUTTON_3,        /**< Mouse button 3 */
            Button4 = GLFW_MOUSE_BUTTON_4,        /**< Mouse button 4 */
            Button5 = GLFW_MOUSE_BUTTON_5,        /**< Mouse button 5 */
            Button6 = GLFW_MOUSE_BUTTON_6,        /**< Mouse button 6 */
            Button7 = GLFW_MOUSE_BUTTON_7,        /**< Mouse button 7 */
            Button8 = GLFW_MOUSE_BUTTON_8,        /**< Mouse button 8 */

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Wheel up
             * @deprecated Use @ref MouseScrollEvent and @ref mouseScrollEvent() instead.
             */
            WheelUp CORRADE_DEPRECATED_ENUM("use mouseScrollEvent() and MouseScrollEvent instead") = GLFW_MOUSE_BUTTON_LAST + 1,

            /**
             * Wheel down
             * @deprecated Use @ref MouseScrollEvent and @ref mouseScrollEvent() instead.
             */
            WheelDown CORRADE_DEPRECATED_ENUM("use mouseScrollEvent() and MouseScrollEvent instead") = GLFW_MOUSE_BUTTON_LAST + 2
            #endif
        };

        /** @brief Button */
        constexpr Button button() const { return _button; }

        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

    private:
        constexpr MouseEvent(Button button, const Vector2i& position, Modifiers modifiers): _button(button), _position{position}, _modifiers(modifiers) {}

        const Button _button;
        const Vector2i _position;
        const Modifiers _modifiers;
};

/**
@brief Mouse move event

@see @ref MouseEvent, @ref MouseScrollEvent, @ref mouseMoveEvent()
*/
class GlfwApplication::MouseMoveEvent: public GlfwApplication::InputEvent {
    friend GlfwApplication;

    public:
        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

    private:
        constexpr MouseMoveEvent(const Vector2i& position, Modifiers modifiers): _position(position), _modifiers(modifiers) {}

        const Vector2i _position;
        const Modifiers _modifiers;
};

/**
@brief Mouse scroll event

@see @ref MouseEvent, @ref MouseMoveEvent, @ref mouseScrollEvent()
*/
class GlfwApplication::MouseScrollEvent: public GlfwApplication::InputEvent {
    friend GlfwApplication;

    public:
        /** @brief Scroll offset */
        constexpr Vector2 offset() const { return _offset; }

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

    private:
        constexpr MouseScrollEvent(const Vector2& offset, Modifiers modifiers): _offset(offset), _modifiers(modifiers) {}

        const Vector2 _offset;
        const Modifiers _modifiers;
};

/**
@brief Text input event

@see @ref textInputEvent()
*/
class GlfwApplication::TextInputEvent {
    friend GlfwApplication;

    public:
        /** @brief Copying is not allowed */
        TextInputEvent(const TextInputEvent&) = delete;

        /** @brief Moving is not allowed */
        TextInputEvent(TextInputEvent&&) = delete;

        /** @brief Copying is not allowed */
        TextInputEvent& operator=(const TextInputEvent&) = delete;

        /** @brief Moving is not allowed */
        TextInputEvent& operator=(TextInputEvent&&) = delete;

        /** @brief Whether the event is accepted */
        constexpr bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Input text in UTF-8 */
        constexpr Containers::ArrayView<const char> text() const { return _text; }

    private:
        constexpr TextInputEvent(Containers::ArrayView<const char> text): _text{text}, _accepted{false} {}

        Containers::ArrayView<const char> _text;
        bool _accepted;
};

/** @hideinitializer
@brief Entry point for GLFW-based applications
@param className Class name

See @ref Magnum::Platform::GlfwApplication "Platform::GlfwApplication" for
usage information. This macro abstracts out platform-specific entry point code
and is equivalent to the following, see @ref portability-applications for more
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
#define MAGNUM_GLFWAPPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef GlfwApplication Application;
typedef BasicScreen<GlfwApplication> Screen;
typedef BasicScreenedApplication<GlfwApplication> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_GLFWAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

}}

#endif
