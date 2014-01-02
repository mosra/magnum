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
 * @brief Class @ref Magnum::Platform::Sdl2Application
 */

#include <Containers/EnumSet.h>

#include "Platform/Platform.h"
#include "Math/Vector2.h"
#include "Magnum.h"

#ifdef CORRADE_TARGET_WINDOWS /* Windows version of SDL2 redefines main(), we don't want that */
#define SDL_MAIN_HANDLED
#endif
#include <SDL.h>
#include <SDL_scancode.h>
#include <Containers/EnumSet.h>
#include <Corrade.h>

namespace Magnum {

class Context;

namespace Platform {

/** @nosubgrouping
@brief SDL2 application

Application using [Simple DirectMedia Layer](http://www.libsdl.org/) toolkit.
Supports keyboard and mouse handling.

This application library is in theory available for all platforms for which
SDL2 is ported (thus also @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten", but not
@ref CORRADE_TARGET_NACL "NaCl"). It depends on **SDL2** library (Emscripten
has it built in) and is built if `WITH_SDL2APPLICATION` is enabled in CMake. To
use it, you need to copy `FindSDL2.cmake` from `modules/` directory in %Magnum
source to `modules/` dir in your project (so CMake is able to find SDL2),
request `%Sdl2Application` component in CMake, add
`${MAGNUM_SDL2APPLICATION_INCLUDE_DIRS}` to include path and link to
`${MAGNUM_SDL2APPLICATION_LIBRARIES}`. If no other application is requested,
you can also use generic `${MAGNUM_APPLICATION_INCLUDE_DIRS}` and
`${MAGNUM_APPLICATION_LIBRARIES}` aliases to simplify porting. See
@ref building and @ref cmake for more information.

@section Sdl2Application-usage Usage

You need to implement at least @ref drawEvent() to be able to draw on the
screen. The subclass can be then used directly in `main()` -- see convenience
macro @ref MAGNUM_SDL2APPLICATION_MAIN(). See @ref platform for more
information.
@code
class MyApplication: public Platform::Sdl2Application {
    // implement required methods...
};
MAGNUM_SDL2APPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.

@section Sdl2Application-html Usage with Emscripten

If you are targetting @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten", you need to
provide HTML markup for your application. Template one is below, you can modify
it to your liking. The markup references two files, `EmscriptenApplication.js`
and `WebApplication.css`, both are in `Platform/` directory in the source tree
and are also installed into `share/magnum/` inside your Emscripten toolchain.
Change `&lt;application&gt;` to name of your executable.
@code
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>Magnum Emscripten Application</title>
    <meta charset="utf-8" />
    <link rel="stylesheet" href="WebApplication.css" />
  </head>
  <body>
    <h1>Magnum Emscripten Application</h1>
    <div id="listener">
      <canvas id="module"></canvas>
      <div id="status">Initialization...</div>
      <div id="statusDescription" />
      <script src="EmscriptenApplication.js"></script>
      <script async="async" src="<application>.js"></script>
    </div>
  </body>
</html>
@endcode

You can modify all the files to your liking, but the HTML file must contain at
least the `&lt;canvas&gt;` enclosed in listener `&lt;div&gt;`. The JavaScript
file contains event listeners which print loading status on the page. The
status displayed in the remaining two `&lt;div&gt;`s, if they are available.
The CSS file contains rudimentary style to avoid eye bleeding.

The application redirects @ref Corrade::Utility::Debug "Debug",
@ref Corrade::Utility::Warning "Warning" and @ref Corrade::Utility::Error "Error"
output to JavaScript console.
*/
class Sdl2Application {
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
         * @param arguments     Application arguments
         * @param configuration %Configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref Configuration for more information. The program exits if
         * the context cannot be created, see below for an alternative.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit Sdl2Application(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit Sdl2Application(const Arguments& arguments, const Configuration& configuration);
        explicit Sdl2Application(const Arguments& arguments);
        #endif

        /**
         * @brief Constructor
         * @param arguments     Application arguments
         *
         * Unlike above, the context is not created and must be created later
         * with @ref createContext() or @ref tryCreateContext().
         */
        explicit Sdl2Application(const Arguments& arguments, std::nullptr_t);

        /** @brief Copying is not allowed */
        Sdl2Application(const Sdl2Application&) = delete;

        /** @brief Moving is not allowed */
        Sdl2Application(Sdl2Application&&) = delete;

        /** @brief Copying is not allowed */
        Sdl2Application& operator=(const Sdl2Application&) = delete;

        /** @brief Moving is not allowed */
        Sdl2Application& operator=(Sdl2Application&&) = delete;

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`.
         */
        int exec();

        /** @brief Exit application main loop */
        void exit();

    protected:
        /* Nobody will need to have (and delete) Sdl2Application*, thus this is
           faster than public pure virtual destructor */
        ~Sdl2Application();

        /**
         * @brief Create context with given configuration
         *
         * Must be called if and only if the context wasn't created by the
         * constructor itself. The program exits if the context cannot be
         * created, see @ref tryCreateContext() for an alternative.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void createContext(const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        void createContext(const Configuration& configuration);
        void createContext();
        #endif

        /**
         * @brief Try to create context with given configuration
         *
         * Unlike @ref createContext() returns `false` if the context cannot be
         * created, `true` otherwise.
         */
        bool tryCreateContext(const Configuration& configuration);

        /** @{ @name Screen handling */

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers();

        /**
         * @brief Redraw immediately
         *
         * Marks the window for redrawing, resulting in call to @ref drawEvent()
         * in the next iteration.
         */
        void redraw() { flags |= Flag::Redraw; }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Viewport event
         *
         * Called when window size changes. The default implementation does
         * nothing, if you want to respond to size changes, you should pass the
         * new size to @ref DefaultFramebuffer::setViewport() and possibly
         * elsewhere (to @ref SceneGraph::AbstractCamera::setViewport() "SceneGraph::Camera*D::setViewport()",
         * other framebuffers...).
         *
         * Note that this function might not get called at all if the window
         * size doesn't change. You should configure the initial state of your
         * cameras, framebuffers etc. in application constructor rather than
         * relying on this function to be called. Viewport of default
         * framebuffer can be retrieved via @ref DefaultFramebuffer::viewport().
         */
        virtual void viewportEvent(const Vector2i& size);

        /**
         * @brief Draw event
         *
         * Called when the screen is redrawn. You should clean the framebuffer
         * using @ref DefaultFramebuffer::clear() and then add your own drawing
         * functions. After drawing is finished, call @ref swapBuffers(). If
         * you want to draw immediately again, call also @ref redraw().
         */
        virtual void drawEvent() = 0;

        /*@}*/

        /** @{ @name Keyboard handling */

        /**
         * @brief Key press event
         *
         * Called when an key is pressed. Default implementation does nothing.
         */
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
        bool isMouseLocked() const { return SDL_GetRelativeMouseMode(); }

        /**
         * @brief Enable or disable mouse locking
         *
         * When mouse is locked, the cursor is hidden and only
         * @ref MouseMoveEvent::relativePosition() is changing, absolute
         * position stays the same.
         */
        void setMouseLocked(bool enabled);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Mouse press event
         *
         * Called when mouse button is pressed. Default implementation does
         * nothing.
         */
        virtual void mousePressEvent(MouseEvent& event);

        /**
         * @brief Mouse release event
         *
         * Called when mouse button is released. Default implementation does
         * nothing.
         */
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
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            Exit = 1 << 1
            #endif
        };

        typedef Containers::EnumSet<Flag, UnsignedByte> Flags;
        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        #ifdef CORRADE_TARGET_EMSCRIPTEN
        static Sdl2Application* instance;
        static void staticMainLoop();
        #endif

        void initialize();
        void mainLoop();

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        SDL_Window* window;
        SDL_GLContext context;
        #else
        SDL_Surface* context;
        #endif

        Context* c;

        Flags flags;
};

CORRADE_ENUMSET_OPERATORS(Sdl2Application::Flags)

/**
@brief %Configuration

Centered non-resizable window with double-buffered OpenGL context and 24bit
depth buffer.
@see @ref Sdl2Application(), @ref createContext(), @ref tryCreateContext()
*/
class Sdl2Application::Configuration {
    public:
        /**
         * @brief Window flag
         *
         * @see @ref Flags, @ref setFlags()
         */
        enum class Flag: Uint32 {
            Resizable = SDL_WINDOW_RESIZABLE,       /**< Resizable window */
            Fullscreen = SDL_WINDOW_FULLSCREEN,     /**< Fullscreen window */
            Hidden = SDL_WINDOW_HIDDEN,             /**< Hidden window */
            Maximized = SDL_WINDOW_MAXIMIZED,       /**< Maximized window */
            Minimized = SDL_WINDOW_MINIMIZED,       /**< Minimized window */
            MouseLocked = SDL_WINDOW_INPUT_GRABBED  /**< Window with mouse locked */
        };

        /**
         * @brief Window flags
         *
         * @see @ref setFlags()
         */
        typedef Containers::EnumSet<Flag, Uint32, SDL_WINDOW_RESIZABLE|
            SDL_WINDOW_FULLSCREEN|SDL_WINDOW_HIDDEN|SDL_WINDOW_MAXIMIZED|
            SDL_WINDOW_MINIMIZED|SDL_WINDOW_INPUT_GRABBED> Flags;

        /*implicit*/ Configuration();
        ~Configuration();

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Window title
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        std::string title() const { return _title; }
        #endif

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * Default is `"Magnum SDL2 Application"`.
         * @note In @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" this function
         *      does nothing and is included only for compatibility. You need
         *      to set the title separately in application's HTML markup.
         */
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        Configuration& setTitle(std::string title) {
            _title = std::move(title);
            return *this;
        }
        #else
        template<class T> Configuration& setTitle(const T&) { return *this; }
        #endif

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

        /** @brief Window flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set window flags
         * @return Reference to self (for method chaining)
         *
         * Default is @ref Flag::Resizable.
         */
        Configuration& setFlags(const Flags flags) {
            _flags = flags;
            return *this;
        }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Context version
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        Version version() const { return _version; }
        #endif

        /**
         * @brief Set context version
         *
         * If requesting version greater or equal to OpenGL 3.1, core profile
         * is used. The created context will then have any version which is
         * backwards-compatible with requested one. Default is
         * @ref Version::None, i.e. any provided version is used.
         * @note In @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" this function
         *      does nothing (@ref Version::GLES200 is always used).
         */
        Configuration& setVersion(Version version) {
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            _version = version;
            #else
            static_cast<void>(version);
            #endif
            return *this;
        }

        /** @brief Sample count */
        Int sampleCount() const { return _sampleCount; }

        /**
         * @brief Set sample count
         * @return Reference to self (for method chaining)
         *
         * Default is `0`, thus no multisampling. See also
         * @ref Renderer::Feature::Multisampling.
         */
        Configuration& setSampleCount(Int count) {
            _sampleCount = count;
            return *this;
        }

    private:
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        std::string _title;
        #endif
        Vector2i _size;
        Flags _flags;
        Int _sampleCount;
        Version _version;
};

CORRADE_ENUMSET_OPERATORS(Sdl2Application::Configuration::Flags)

/**
@brief Base for input events

@see @ref KeyEvent, @ref MouseEvent, @ref MouseMoveEvent, @ref keyPressEvent(),
    @ref keyReleaseEvent(), @ref mousePressEvent(), @ref mouseReleaseEvent(),
    @ref mouseMoveEvent()
*/
class Sdl2Application::InputEvent {
    public:
        /**
         * @brief %Modifier
         *
         * @see @ref Modifiers, @ref KeyEvent::modifiers(),
         *      @ref MouseEvent::modifiers(), @ref MouseMoveEvent::modifiers()
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
         * @see @ref KeyEvent::modifiers(), @ref MouseEvent::modifiers(),
         *      @ref MouseMoveEvent::modifiers()
         */
        typedef Containers::EnumSet<Modifier, Uint16> Modifiers;

        /** @brief Copying is not allowed */
        InputEvent(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent(InputEvent&&) = delete;

        /** @brief Copying is not allowed */
        InputEvent& operator=(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent& operator=(InputEvent&&) = delete;

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Whether the event is accepted */
        constexpr bool isAccepted() const { return _accepted; }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        constexpr explicit InputEvent(): _accepted(false) {}

        ~InputEvent() = default;
    #endif

    private:
        bool _accepted;
};

/**
@brief Key event

@see @ref keyPressEvent(), @ref keyReleaseEvent()
*/
class Sdl2Application::KeyEvent: public Sdl2Application::InputEvent {
    friend class Sdl2Application;

    public:
        /**
         * @brief Key
         *
         * @see @ref key()
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
        constexpr Key key() const { return _key; }

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

    private:
        constexpr KeyEvent(Key key, Modifiers modifiers): _key(key), _modifiers(modifiers) {}

        const Key _key;
        const Modifiers _modifiers;
};

/**
@brief Mouse event

@see @ref MouseMoveEvent, @ref mousePressEvent(), @ref mouseReleaseEvent()
*/
class Sdl2Application::MouseEvent: public Sdl2Application::InputEvent {
    friend class Sdl2Application;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: Uint8 {
            Left = SDL_BUTTON_LEFT,         /**< Left button */
            Middle = SDL_BUTTON_MIDDLE,     /**< Middle button */
            Right = SDL_BUTTON_RIGHT,       /**< Right button */
            WheelUp = SDL_BUTTON_X1,        /**< Wheel up */
            WheelDown = SDL_BUTTON_X2       /**< Wheel down */
        };

        /** @brief Button */
        constexpr Button button() const { return _button; }

        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

        /**
         * @brief Modifiers
         *
         * Lazily populated on first request.
         */
        Modifiers modifiers();

    private:
        constexpr MouseEvent(Button button, const Vector2i& position): _button(button), _position(position), modifiersLoaded(false) {}

        const Button _button;
        const Vector2i _position;
        bool modifiersLoaded;
        Modifiers _modifiers;
};

/**
@brief Mouse move event

@see @ref MouseEvent, @ref mouseMoveEvent()
*/
class Sdl2Application::MouseMoveEvent: public Sdl2Application::InputEvent {
    friend class Sdl2Application;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref Buttons, @ref buttons()
         */
        enum class Button: Uint32 {
            Left = SDL_BUTTON_LMASK,        /**< Left button */
            Middle = SDL_BUTTON_MMASK,      /**< Middle button */
            Right = SDL_BUTTON_RMASK,       /**< Right button */
            WheelUp = SDL_BUTTON_X1MASK,    /**< Wheel up */
            WheelDown = SDL_BUTTON_X2MASK   /**< Wheel down */
        };

        /**
         * @brief Set of mouse buttons
         *
         * @see @ref buttons()
         */
        typedef Containers::EnumSet<Button, Uint32> Buttons;

        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

        /**
         * @brief Relative position
         *
         * Position relative to previous event.
         */
        constexpr Vector2i relativePosition() const { return _relativePosition; }

        /** @brief Mouse buttons */
        constexpr Buttons buttons() const { return _buttons; }

        /**
         * @brief Modifiers
         *
         * Lazily populated on first request.
         */
        Modifiers modifiers();

    private:
        constexpr MouseMoveEvent(const Vector2i& position, const Vector2i& relativePosition, Buttons buttons): _position(position), _relativePosition(relativePosition), modifiersLoaded(false), _buttons(buttons) {}

        const Vector2i _position, _relativePosition;
        bool modifiersLoaded;
        Buttons _buttons;
        Modifiers _modifiers;
};

/** @hideinitializer
@brief Entry point for SDL2-based applications
@param className Class name

Can be used with @ref Magnum::Platform::Sdl2Application "Platform::Sdl2Application"
subclasses as equivalent to the following code to achieve better portability,
see @ref portability-applications for more information.
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
typedef BasicScreen<Sdl2Application> Screen;
typedef BasicScreenedApplication<Sdl2Application> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_SDL2APPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

CORRADE_ENUMSET_OPERATORS(Sdl2Application::InputEvent::Modifiers)
CORRADE_ENUMSET_OPERATORS(Sdl2Application::MouseMoveEvent::Buttons)

}}

#endif
