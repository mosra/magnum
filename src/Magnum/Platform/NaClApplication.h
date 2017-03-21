#ifndef Magnum_Platform_NaClApplication_h
#define Magnum_Platform_NaClApplication_h
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
 * @brief Class @ref Magnum::Platform::NaClApplication, macro @ref MAGNUM_NACLAPPLICATION_MAIN()
 */

#include <memory>
#include <string>
#include <Corrade/Containers/EnumSet.h>

/* Needs to be first */
#include "Magnum/OpenGL.h"

#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/mouse_lock.h>
#include <ppapi/cpp/graphics_3d_client.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Platform/Platform.h"

namespace pp {
    class Graphics3D;
    class Fullscreen;
}

#ifndef CORRADE_TARGET_NACL
#error this header is available only on Google Chrome Native Client build
#endif

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief NaCl application

Application running in [Google Chrome Native Client](https://developers.google.com/native-client/).
Supports keyboard and mouse handling.

This application library is available only in
@ref CORRADE_TARGET_NACL "Google Chrome Native Client", see respective sections
in @ref building-corrade-cross-nacl "Corrade's" and @ref building-cross-nacl "Magnum's"
building documentation. It is built if `WITH_NACLAPPLICATION` is enabled in
CMake.

## Bootstrap application

Fully contained base application using @ref Sdl2Application for desktop build
and @ref NaClApplication for Native Client build along with full HTML markup
and CMake setup is available in `base-nacl` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-nacl.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-nacl.zip) file.
After extracting the downloaded archive, you can do the desktop build in the
same way as with @ref Sdl2Application. For the Native Client build you also
need to put the contents of toolchains repository from https://github.com/mosra/toolchains
in `toolchains/` subdirectory. Don't forget to adapt `NACL_PREFIX` variable in
`toolchains/generic/NaCl-newlib-x86-32.cmake` and
`toolchains/generic/NaCl-newlib-x86-64.cmake` to path where your SDK is
installed. Default is `/usr/nacl`. You may need to adapt also
`NACL_TOOLCHAIN_PATH` so CMake is able to find the compiler.

Then create build directories for x86-32 and x86-64 and run `cmake` and
build/install commands in them. The toolchains need access to the platform
file, so be sure to properly set **absolute** path to `toolchains/modules/`
directory containing `Platform/NaCl.cmake`. Set `CMAKE_INSTALL_PREFIX` to
location of your webserver to have the files installed in proper location (e.g.
`/srv/http/nacl`).

    mkdir build-nacl-x86-32 && cd build-nacl-x86-32
    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/NaCl-newlib-x86-32.cmake" \
        -DCMAKE_INSTALL_PREFIX=/srv/http/nacl
    cmake --build .
    cmake --build . --target install

    mkdir build-nacl-x86-64 && cd build-nacl-x86-64
    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/NaCl-newlib-x86-64.cmake" \
        -DCMAKE_INSTALL_PREFIX=/srv/http/nacl
    cmake --build .
    cmake --build . --target install

See @ref cmake for more information.

You can then open `MyApplication` through your webserver in Chrome (e.g.
`http://localhost/nacl/MyApplication.html`).

## General usage

For CMake you need to copy `FindOpenGLES2.cmake` from `modules/` directory in
Magnum source to `modules/` dir in your project (so it is able to find OpenGL
ES). Request `NaClApplication` component of `Magnum` package and link to
`Magnum::NaClApplication` target. If no other application is requested, you can
also use generic `Magnum::Application` alias to simplify porting. Again, see
@ref building and @ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass must be then registered to NaCl API using
@ref MAGNUM_NACLAPPLICATION_MAIN() macro. See @ref platform for more
information.
@code
class MyApplication: public Platform::NaClApplication {
    // implement required methods...
};
MAGNUM_NACLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.

### HTML markup and NMF file

You need to provide HTML markup for your application. Template one is below or
in the bootstrap application, you can modify it to your liking. The markup
references two files, `NaClApplication.js` and `WebApplication.css`, both are
in `Platform/` directory in the source tree and are also installed into
`share/magnum/` inside your NaCl toolchain. Change `&lt;application&gt;` to
name of your executable.
@code
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>Magnum NaCl Application</title>
    <meta charset="utf-8" />
    <link rel="stylesheet" href="WebApplication.css" />
  </head>
  <body>
    <h1>Magnum NaCl Application</h1>
    <div id="listener">
      <embed id="module" type="application/x-nacl" src="<application>.nmf"></embed>
      <div id="status">Initialization...</div>
      <div id="statusDescription"></div>
      <script src="NaClApplication.js"></script>
    </div>
  </body>
</html>
@endcode

You can modify all the files to your liking, but the HTML file must contain at
least the `&lt;embed&gt;` enclosed in listener `&lt;div&gt;`. The JavaScript
file contains event listeners which print loading status on the page. The
status displayed in the remaining two `&lt;div&gt;`s, if they are available.
The CSS file contains rudimentary style to avoid eye bleeding.

The `&lt;embed&gt;` file references NMF file which you need to provide too. If
you target @ref CORRADE_TARGET_NACL_NEWLIB "newlib", the file is pretty simple,
for example (change `&lt;application&gt;` to name of your executable):
@code
{
    "program": {
        "x86-32": {"url": "<application>-x86-32.nexe"},
        "x86-64": {"url": "<application>-x86-64.nexe"}
    }
}
@endcode

If you target @ref CORRADE_TARGET_NACL_GLIBC "glibc", you need to specify also
all additional dependencies. See [Native Client](https://developers.google.com/native-client/)
documentation for more information.

## Redirecting output to Chrome's JavaScript console

The application by default redirects @ref Corrade::Utility::Debug "Debug",
@ref Corrade::Utility::Warning "Warning" and @ref Corrade::Utility::Error "Error"
output to JavaScript console. See also @ref Corrade::Utility::NaClConsoleStreamBuffer
for more information.
*/
class NaClApplication: public pp::Instance, public pp::Graphics3DClient, public pp::MouseLock {
    public:
        /** @brief Application arguments */
        typedef PP_Instance Arguments;

        class Configuration;
        class InputEvent;
        class KeyEvent;
        class MouseEvent;
        class MouseMoveEvent;
        class MouseScrollEvent;

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, const Configuration&) */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit NaClApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit NaClApplication(const Arguments& arguments, const Configuration& configuration);
        explicit NaClApplication(const Arguments& arguments);
        #endif

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, NoCreateT) */
        explicit NaClApplication(const Arguments& arguments, NoCreateT);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief NaClApplication(const Arguments&, NoCreateT)
         * @deprecated Use @ref NaClApplication(const Arguments&, NoCreateT) instead.
         */
        CORRADE_DEPRECATED("use NaClApplication(const Arguments&, NoCreateT) instead") explicit NaClApplication(const Arguments& arguments, std::nullptr_t): NaClApplication{arguments, NoCreate} {}
        #endif

        /** @brief Copying is not allowed */
        NaClApplication(const NaClApplication&) = delete;

        /** @brief Moving is not allowed */
        NaClApplication(NaClApplication&&) = delete;

        /** @brief Copying is not allowed */
        NaClApplication& operator=(const NaClApplication&) = delete;

        /** @brief Moving is not allowed */
        NaClApplication& operator=(NaClApplication&&) = delete;

    protected:
        /* Nobody will need to have (and delete) NaClApplication*, thus this is
           faster than public pure virtual destructor */
        ~NaClApplication();

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

    public:
        /** @brief Whether the application runs fullscreen */
        bool isFullscreen();

        /**
         * @brief Set fullscreen
         * @return `False` if switch to opposite mode is in progress or if the
         *      switch is not possible, `true` otherwise.
         *
         * The switch is done asynchronously, during the switch no event
         * processing is done.
         */
        bool setFullscreen(bool enabled);

    protected:
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

        /**
         * @brief Key press event
         *
         * Called when an key is pressed. Default implementation does nothing.
         * If you accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void keyPressEvent(KeyEvent& event);

        /**
         * @brief Key release event
         *
         * Called when an key is released. Default implementation does nothing.
         * If you accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void keyReleaseEvent(KeyEvent& event);

        /*@}*/

        /** @{ @name Mouse handling */

    public:
        /** @brief Whether mouse is locked */
        bool isMouseLocked() const { return _flags & Flag::MouseLocked; }

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
         * nothing. If you accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void mousePressEvent(MouseEvent& event);

        /**
         * @brief Mouse release event
         *
         * Called when mouse button is released. Default implementation does
         * nothing. If you accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void mouseReleaseEvent(MouseEvent& event);

        /**
         * @brief Mouse move event
         *
         * Called when mouse is moved. Default implementation does nothing. If
         * you accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

        /**
         * @brief Mouse scroll event
         *
         * Called when a scrolling device is used (mouse wheel or scrolling
         * area on a touchpad). Default implementation does nothing. If you
         * accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void mouseScrollEvent(MouseScrollEvent& event);

        /*@}*/

    private:
        struct ConsoleDebugOutput;

        enum class Flag: UnsignedByte {
            SwapInProgress = 1 << 0,
            Redraw = 1 << 1,
            FullscreenSwitchInProgress = 1 << 2,
            WillBeFullscreen = 1 << 3,
            MouseLocked = 1 << 4
        };
        typedef Containers::EnumSet<Flag> Flags;

        void Graphics3DContextLost() override {
            CORRADE_ASSERT(false, "NaClApplication: context unexpectedly lost", );
        }

        void MouseLockLost() override {
            _flags &= ~Flag::MouseLocked;
        }

        void DidChangeView(const pp::View& view) override;

        bool HandleInputEvent(const pp::InputEvent& event) override;

        static void swapCallback(void* applicationInstance, std::int32_t);
        static void mouseLockCallback(void* applicationInstance, std::int32_t);

        std::unique_ptr<pp::Graphics3D> _graphics;
        std::unique_ptr<pp::Fullscreen> _fullscreen;
        Vector2i _viewportSize;
        Flags _flags;

        std::unique_ptr<ConsoleDebugOutput> _debugOutput;
        std::unique_ptr<Platform::Context> _context;

        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)
};

/**
@brief Configuration

Double-buffered RGBA canvas with depth and stencil buffers.
@see @ref NaClApplication(), @ref createContext(), @ref tryCreateContext()
*/
class NaClApplication::Configuration {
    public:
        constexpr /*implicit*/ Configuration(): _size(640, 480), _sampleCount(0) {}

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * @note This function does nothing and is included only for
         *      compatibility with other toolkits. You need to set the title
         *      separately in application's HTML markup.
         */
        template<class T> Configuration& setTitle(const T&) { return *this; }

        /** @brief Window size */
        Vector2i size() const { return _size; }

        /**
         * @brief Set window size
         * @return Reference to self (for method chaining)
         *
         * Default is `{640, 480}`.
         */
        Configuration& setSize(const Vector2i& size) {
            _size = size;
            return *this;
        }

        /**
         * @brief Set context version
         *
         * @note This function does nothing and is included only for
         *      compatibility with other toolkits. @ref Version::GLES200 is
         *      always used.
         */
        Configuration& setVersion(Version) { return *this; }

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
        Vector2i _size;
        Int _sampleCount;
};

/**
@brief Base for input events

If you accept the event, call @ref setAccepted(), otherwise the event will be
propagated to the browser.
@see @ref KeyEvent, @ref MouseEvent, @ref MouseMoveEvent, @ref keyPressEvent(),
    @ref keyReleaseEvent(), @ref mousePressEvent(), @ref mouseReleaseEvent(),
    @ref mouseMoveEvent()
*/
class NaClApplication::InputEvent {
    public:
        /**
         * @brief Modifier
         *
         * @see @ref Modifiers, @ref modifiers()
         * @todo AltGr + PP_INPUTEVENT_MODIFIER_ISKEYPAD, PP_INPUTEVENT_MODIFIER_ISAUTOREPEAT
         */
        enum class Modifier: std::uint32_t {
            Shift = PP_INPUTEVENT_MODIFIER_SHIFTKEY,    /**< Shift */
            Ctrl = PP_INPUTEVENT_MODIFIER_CONTROLKEY,   /**< Ctrl */
            Alt = PP_INPUTEVENT_MODIFIER_ALTKEY,        /**< Alt */
            Meta = PP_INPUTEVENT_MODIFIER_METAKEY,      /**< Meta */
            CapsLock = PP_INPUTEVENT_MODIFIER_CAPSLOCKKEY,  /**< Caps lock */
            NumLock = PP_INPUTEVENT_MODIFIER_NUMLOCKKEY     /**< Num lock */
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
        enum class Button: std::uint32_t {
            Left = PP_INPUTEVENT_MODIFIER_LEFTBUTTONDOWN,       /**< Left button */
            Middle = PP_INPUTEVENT_MODIFIER_MIDDLEBUTTONDOWN,   /**< Middle button */
            Right = PP_INPUTEVENT_MODIFIER_RIGHTBUTTONDOWN      /**< Right button */
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

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

        /** @brief Mouse buttons */
        constexpr Buttons buttons() const { return Button(static_cast<std::uint32_t>(_modifiers)); }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it is
         * propagated elsewhere (e.g. to the browser). By default is each
         * event ignored.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Whether the event is accepted */
        constexpr bool isAccepted() const { return _accepted; }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        constexpr InputEvent(Modifiers modifiers): _accepted(false), _modifiers(modifiers) {}

        ~InputEvent() = default;
    #endif

    private:
        bool _accepted;
        const Modifiers _modifiers;
};

/**
@brief Key event

See also @ref InputEvent for more information.
@see @ref keyPressEvent(), @ref keyReleaseEvent()
*/
class NaClApplication::KeyEvent: public NaClApplication::InputEvent {
    friend NaClApplication;

    public:
        /**
         * @brief Key
         *
         * @see @ref key()
         * @todo Slash, percent, equal to be compatible with *XApplication
         */
        enum class Key: std::uint32_t {
            Enter = 0x0D,               /**< Enter */
            Esc = 0x1B,                 /**< Escape */

            Up = 0x26,                  /**< Up arrow */
            Down = 0x28,                /**< Down arrow */
            Left = 0x25,                /**< Left arrow */
            Right = 0x27,               /**< Right arrow */
            F1 = 0x70,                  /**< F1 */
            F2 = 0x71,                  /**< F2 */
            F3 = 0x72,                  /**< F3 */
            F4 = 0x73,                  /**< F4 */
            F5 = 0x74,                  /**< F5 */
            F6 = 0x75,                  /**< F6 */
            F7 = 0x76,                  /**< F7 */
            F8 = 0x77,                  /**< F8 */
            F9 = 0x78,                  /**< F9 */
            F10 = 0x79,                 /**< F10 */
            F11 = 0x7A,                 /**< F11 */
            F12 = 0x7B,                 /**< F12 */
            Home = 0x24,                /**< Home */
            End = 0x23,                 /**< End */
            PageUp = 0x21,              /**< Page up */
            PageDown = 0x22,            /**< Page down */

            Space = 0x20,               /**< Space */
            Comma = 0xBC,               /**< Comma */
            Period = 0xBE,              /**< Period */
            Minus = 0xBD,               /**< Minus */
            Plus = 0xBB,                /**< Plus */

            Zero = '0',                 /**< Zero */
            One = '1',                  /**< One */
            Two = '2',                  /**< Two */
            Three = '3',                /**< Three */
            Four = '4',                 /**< Four */
            Five = '5',                 /**< Five */
            Six = '6',                  /**< Six */
            Seven = '7',                /**< Seven */
            Eight = '8',                /**< Eight */
            Nine = '9',                 /**< Nine */

            A = 'A',                    /**< Letter A */
            B = 'B',                    /**< Letter B */
            C = 'C',                    /**< Letter C */
            D = 'D',                    /**< Letter D */
            E = 'E',                    /**< Letter E */
            F = 'F',                    /**< Letter F */
            G = 'G',                    /**< Letter G */
            H = 'H',                    /**< Letter H */
            I = 'I',                    /**< Letter I */
            J = 'J',                    /**< Letter J */
            K = 'K',                    /**< Letter K */
            L = 'L',                    /**< Letter L */
            M = 'M',                    /**< Letter M */
            N = 'N',                    /**< Letter N */
            O = 'O',                    /**< Letter O */
            P = 'P',                    /**< Letter P */
            Q = 'Q',                    /**< Letter Q */
            R = 'R',                    /**< Letter R */
            S = 'S',                    /**< Letter S */
            T = 'T',                    /**< Letter T */
            U = 'U',                    /**< Letter U */
            V = 'V',                    /**< Letter V */
            W = 'W',                    /**< Letter W */
            X = 'X',                    /**< Letter X */
            Y = 'Y',                    /**< Letter Y */
            Z = 'Z'                     /**< Letter Z */
        };

        /** @brief Key */
        constexpr Key key() const { return _key; }

    private:
        constexpr KeyEvent(Key key, Modifiers modifiers): InputEvent(modifiers), _key(key) {}

        const Key _key;
};

/**
@brief Mouse event

See also @ref InputEvent for more information.
@see @ref MouseMoveEvent, @ref MouseScrollEvent, @ref mousePressEvent(),
    @ref mouseReleaseEvent()
*/
class NaClApplication::MouseEvent: public NaClApplication::InputEvent {
    friend NaClApplication;

    public:
        /**
         * @brief Button
         *
         * @see @ref button()
         */
        enum class Button: unsigned int {
            Left = PP_INPUTEVENT_MOUSEBUTTON_LEFT,      /**< Left button */
            Middle = PP_INPUTEVENT_MOUSEBUTTON_MIDDLE,  /**< Middle button */
            Right = PP_INPUTEVENT_MOUSEBUTTON_RIGHT,    /**< Right button */

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Wheel up
             * @deprecated Use @ref MouseScrollEvent and @ref mouseScrollEvent() instead.
             */
            WheelUp CORRADE_DEPRECATED_ENUM("use mouseScrollEvent() and MouseScrollEvent instead") = 0xFFFF01,

            /**
             * Wheel down
             * @deprecated Use @ref MouseScrollEvent and @ref mouseScrollEvent() instead.
             */
            WheelDown CORRADE_DEPRECATED_ENUM("use mouseScrollEvent() and MouseScrollEvent instead") = 0xFFFF02
            #endif
        };

        /** @brief Button */
        constexpr Button button() const { return _button; }

        /**
         * @brief Position
         *
         * @attention Note that due to the way the @ref Button::WheelUp and
         *      @ref Button::WheelDown events are handled by Native Client, the
         *      position information is not available, i.e. it is set to zero.
         */
        constexpr Vector2i position() const { return _position; }

    private:
        constexpr MouseEvent(Button button, const Vector2i& position, Modifiers modifiers): InputEvent(modifiers), _button(button), _position(position) {}

        const Button _button;
        const Vector2i _position;
};

/**
@brief Mouse move event

See also @ref InputEvent for more information.
@see @ref MouseEvent, @ref MouseScrollEvent, @ref mouseMoveEvent()
*/
class NaClApplication::MouseMoveEvent: public NaClApplication::InputEvent {
    friend NaClApplication;

    public:
        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

        /**
         * @brief Relative position
         *
         * Position relative to previous event.
         */
        constexpr Vector2i relativePosition() const { return _relativePosition; }

    private:
        constexpr MouseMoveEvent(const Vector2i& position, const Vector2i& relativePosition, Modifiers modifiers): InputEvent(modifiers), _position(position), _relativePosition(relativePosition) {}

        const Vector2i _position, _relativePosition;
};

/**
@brief Mouse scroll event

See also @ref InputEvent for more information.
@see @ref MouseEvent, @ref MouseMoveEvent, @ref mouseScrollEvent()
*/
class NaClApplication::MouseScrollEvent: public NaClApplication::InputEvent {
    friend NaClApplication;

    public:
        /** @brief Scroll offset */
        constexpr Vector2 offset() const { return _offset; }

    private:
        constexpr MouseScrollEvent(const Vector2& offset, Modifiers modifiers): InputEvent{modifiers}, _offset{offset} {}

        const Vector2 _offset;
};

CORRADE_ENUMSET_OPERATORS(NaClApplication::Flags)

namespace Implementation {
    template<class Application> class NaClModule: public pp::Module {
        public:
            ~NaClModule() { glTerminatePPAPI(); }

            bool Init() override {
                return glInitializePPAPI(get_browser_interface());
            }

            pp::Instance* CreateInstance(PP_Instance instance) {
                return new Application(instance);
            }
    };
}

/** @hideinitializer
@brief Entry point for NaCl application
@param className Class name

See @ref Magnum::Platform::NaClApplication "Platform::NaClApplication" for
usage information. This macro abstracts out platform-specific entry point code
(the classic `main()` function cannot be used in NaCl). See
@ref portability-applications for more information. When no other application
header is included this macro is also aliased to `MAGNUM_APPLICATION_MAIN()`.
*/
/* look at that insane placement of __attribute__. WTF. */
#define MAGNUM_NACLAPPLICATION_MAIN(className)                              \
    namespace pp {                                                          \
        Module __attribute__ ((visibility ("default"))) * CreateModule();   \
        Module __attribute__ ((visibility ("default"))) * CreateModule() {  \
            return new Magnum::Platform::Implementation::NaClModule<className>(); \
        }                                                                   \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef NaClApplication Application;
typedef BasicScreen<NaClApplication> Screen;
typedef BasicScreenedApplication<NaClApplication> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_NACLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

CORRADE_ENUMSET_OPERATORS(NaClApplication::InputEvent::Modifiers)
CORRADE_ENUMSET_OPERATORS(NaClApplication::InputEvent::Buttons)

}}

#endif
