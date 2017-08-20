#ifndef Magnum_Platform_GlutApplication_h
#define Magnum_Platform_GlutApplication_h
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
 * @brief Class @ref Magnum::Platform::GlutApplication, macro @ref MAGNUM_GLUTAPPLICATION_MAIN()
 */

#include <memory>
#include <string>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Platform/Platform.h"

/* We must include our own GL headers first to avoid conflicts */
#include "Magnum/OpenGL.h"
#include <GL/freeglut.h>

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief GLUT application

Application using GLUT toolkit. Supports keyboard and mouse handling with
support for changing cursor and mouse tracking and warping.

This application library is available only on desktop OpenGL (Linux, Windows,
macOS). It depends on **GLUT** library and is built if `WITH_GLUTAPPLICATION`
is enabled in CMake.

## Bootstrap application

Fully contained base application using @ref GlutApplication along with
CMake setup is available in `base-glut` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-glut.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-glut.zip) file.
After extracting the downloaded archive you can build and run the application
with these four commands:

    mkdir build && cd build
    cmake ..
    cmake --build .
    ./src/MyApplication # or ./src/Debug/MyApplication

See @ref cmake for more information.

## General usage

In CMake you need to request `GlutApplication` component of `Magnum` package
and link to `Magnum::GlutApplication` target. If no other application is
requested, you can also use generic `Magnum::Application` alias to simplify
porting. Again, see @ref building and @ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass can be then used directly in `main()` -- see
convenience macro @ref MAGNUM_GLUTAPPLICATION_MAIN(). See @ref platform for
more information.
@code
class MyApplication: public Platform::GlutApplication {
    // implement required methods...
};
MAGNUM_GLUTAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.
*/
class GlutApplication {
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

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, const Configuration&) */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit GlutApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit GlutApplication(const Arguments& arguments, const Configuration& configuration);
        explicit GlutApplication(const Arguments& arguments);
        #endif

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, NoCreateT) */
        explicit GlutApplication(const Arguments& arguments, NoCreateT);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief GlutApplication(const Arguments&, NoCreateT)
         * @deprecated Use @ref GlutApplication(const Arguments&, NoCreateT) instead.
         */
        CORRADE_DEPRECATED("use GlutApplication(const Arguments&, NoCreateT) instead") explicit GlutApplication(const Arguments& arguments, std::nullptr_t): GlutApplication{arguments, NoCreate} {}
        #endif

        /** @brief Copying is not allowed */
        GlutApplication(const GlutApplication&) = delete;

        /** @brief Moving is not allowed */
        GlutApplication(GlutApplication&&) = delete;

        /** @brief Copying is not allowed */
        GlutApplication& operator=(const GlutApplication&) = delete;

        /** @brief Moving is not allowed */
        GlutApplication& operator=(GlutApplication&&) = delete;

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`
         *
         * See @ref MAGNUM_GLUTAPPLICATION_MAIN() for usage information.
         */
        int exec() {
            glutMainLoop();
            return 0;
        }

    protected:
        /* Nobody will need to have (and delete) GlutApplication*, thus this is
           faster than public pure virtual destructor */
        ~GlutApplication();

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
        void swapBuffers() { glutSwapBuffers(); }

        /** @copydoc Sdl2Application::redraw() */
        void redraw() { glutPostRedisplay(); }

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
        /**
         * @brief Mouse cursor
         *
         * @see @ref setMouseCursor()
         */
        enum class MouseCursor: int {
            Default = GLUT_CURSOR_INHERIT,  /**< Default cursor provided by parent window */
            None = GLUT_CURSOR_NONE         /**< No cursor */
        };

        /**
         * @brief Enable or disable mouse tracking
         *
         * When mouse tracking is enabled, @ref mouseMoveEvent() is called even
         * when no button is pressed. Mouse tracking is disabled by default.
         */
        void setMouseTracking(bool enabled) {
            glutPassiveMotionFunc(enabled ? staticMouseMoveEvent : nullptr);
        }

        /** @brief Set mouse cursor */
        void setMouseCursor(MouseCursor cursor) {
            glutSetCursor(static_cast<int>(cursor));
        }

        /** @brief Warp mouse cursor to given coordinates */
        void warpMouseCursor(const Vector2i& position) {
            glutWarpPointer(position.x(), position.y());
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
         * @see @ref setMouseTracking()
         */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

        /*@}*/

    private:
        static void staticViewportEvent(int x, int y) {
            _instance->viewportEvent({x, y});
        }

        static void staticKeyPressEvent(unsigned char key, int x, int y);
        static void staticKeyReleaseEvent(unsigned char key, int x, int y);

        static void staticSpecialKeyPressEvent(int key, int x, int y);
        static void staticSpecialKeyReleaseEvent(int key, int x, int y);

        static void staticMouseEvent(int button, int state, int x, int y);

        static void staticMouseMoveEvent(int x, int y);

        static void staticDrawEvent() {
            _instance->drawEvent();
        }

        static GlutApplication* _instance;

        std::unique_ptr<Platform::Context> _context;
};

/**
@brief Configuration

Double-buffered RGBA window with depth and stencil buffers.
@see @ref GlutApplication(), @ref createContext(), @ref tryCreateContext()
*/
class GlutApplication::Configuration {
    public:
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref setFlags()
         */
        enum class Flag: int {
            Debug = GLUT_DEBUG  /**< Create debug context */
        };

        /**
         * @brief Context flags
         *
         * @see @ref setFlags()
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typedef Containers::EnumSet<Flag, GLUT_DEBUG> Flags;
        #else
        typedef Containers::EnumSet<Flag> Flags;
        #endif

        /*implicit*/ Configuration();
        ~Configuration();

        /** @brief Window title */
        std::string title() const { return _title; }

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * Default is `"Magnum GLUT Application"`.
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
         * ignored, GLUT either enables it or disables. See also
         * @ref Renderer::Feature::Multisampling.
         */
        Configuration& setSampleCount(Int count) {
            _sampleCount = count;
            return *this;
        }

    private:
        std::string _title;
        Vector2i _size;
        Int _sampleCount;
        Version _version;
        Flags _flags;
};

CORRADE_ENUMSET_OPERATORS(GlutApplication::Configuration::Flags)

/**
@brief Base for input events

@see @ref KeyEvent, @ref MouseEvent, @ref MouseMoveEvent, @ref keyPressEvent(),
    @ref mousePressEvent(), @ref mouseReleaseEvent(), @ref mouseMoveEvent()
*/
class GlutApplication::InputEvent {
    public:
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

/**
@brief Key event

@see @ref keyPressEvent()
*/
class GlutApplication::KeyEvent: public GlutApplication::InputEvent {
    friend GlutApplication;

    public:
        /**
         * @brief Key
         *
         * @see @ref key()
         */
        enum class Key: UnsignedInt {
            Esc = '\x1b',               /**< Escape */

            Up = GLUT_KEY_UP << 16,     /**< Up arrow */
            Down = GLUT_KEY_DOWN << 16, /**< Down arrow */
            Left = GLUT_KEY_LEFT << 16, /**< Left arrow */
            Right = GLUT_KEY_RIGHT << 16, /**< Right arrow */
            F1 = GLUT_KEY_F1 << 16,     /**< F1 */
            F2 = GLUT_KEY_F2 << 16,     /**< F2 */
            F3 = GLUT_KEY_F3 << 16,     /**< F3 */
            F4 = GLUT_KEY_F4 << 16,     /**< F4 */
            F5 = GLUT_KEY_F5 << 16,     /**< F5 */
            F6 = GLUT_KEY_F6 << 16,     /**< F6 */
            F7 = GLUT_KEY_F7 << 16,     /**< F7 */
            F8 = GLUT_KEY_F8 << 16,     /**< F8 */
            F9 = GLUT_KEY_F9 << 16,     /**< F9 */
            F10 = GLUT_KEY_F10 << 16,   /**< F10 */
            F11 = GLUT_KEY_F11 << 16,   /**< F11 */
            F12 = GLUT_KEY_F12 << 16,   /**< F12 */
            Home = GLUT_KEY_HOME << 16, /**< Home */
            End = GLUT_KEY_END << 16,   /**< End */
            PageUp = GLUT_KEY_PAGE_UP << 16, /**< Page up */
            PageDown = GLUT_KEY_PAGE_DOWN << 16, /**< Page down */

            Space = ' ',                /**< Space */
            Comma = ',',                /**< Comma */
            Period = '.',               /**< Period */
            Minus = '-',                /**< Minus */
            Plus = '+',                 /**< Plus */
            Slash = '/',                /**< Slash */
            Percent = '%',              /**< Percent */
            Equal = '=',                /**< Equal */

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

            A = 'a',                    /**< Letter A */
            B = 'b',                    /**< Letter B */
            C = 'c',                    /**< Letter C */
            D = 'd',                    /**< Letter D */
            E = 'e',                    /**< Letter E */
            F = 'f',                    /**< Letter F */
            G = 'g',                    /**< Letter G */
            H = 'h',                    /**< Letter H */
            I = 'i',                    /**< Letter I */
            J = 'j',                    /**< Letter J */
            K = 'k',                    /**< Letter K */
            L = 'l',                    /**< Letter L */
            M = 'm',                    /**< Letter M */
            N = 'n',                    /**< Letter N */
            O = 'o',                    /**< Letter O */
            P = 'p',                    /**< Letter P */
            Q = 'q',                    /**< Letter Q */
            R = 'r',                    /**< Letter R */
            S = 's',                    /**< Letter S */
            T = 't',                    /**< Letter T */
            U = 'u',                    /**< Letter U */
            V = 'v',                    /**< Letter V */
            W = 'w',                    /**< Letter W */
            X = 'x',                    /**< Letter X */
            Y = 'y',                    /**< Letter Y */
            Z = 'z'                     /**< Letter Z */
        };

        /** @brief Key */
        constexpr Key key() const { return _key; }

        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

    private:
        constexpr KeyEvent(Key key, const Vector2i& position): _key(key), _position(position) {}

        const Key _key;
        const Vector2i _position;
};

/**
@brief Mouse event

@see @ref MouseMoveEvent, @ref mousePressEvent(), @ref mouseReleaseEvent()
*/
class GlutApplication::MouseEvent: public GlutApplication::InputEvent {
    friend GlutApplication;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: int {
            Left = GLUT_LEFT_BUTTON,        /**< Left button */
            Middle = GLUT_MIDDLE_BUTTON,    /**< Middle button */
            Right = GLUT_RIGHT_BUTTON,      /**< Right button */
            WheelUp = 3,                    /**< Wheel up */
            WheelDown = 4                   /**< Wheel down */
        };

        /** @brief Button */
        constexpr Button button() const { return _button; }

        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

    private:
        constexpr MouseEvent(Button button, const Vector2i& position): _button(button), _position(position) {}

        const Button _button;
        const Vector2i _position;
};

/**
@brief Mouse move event

@see @ref MouseEvent, @ref mouseMoveEvent()
*/
class GlutApplication::MouseMoveEvent: public GlutApplication::InputEvent {
    friend GlutApplication;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref Buttons, @ref buttons()
         */
        enum class Button: UnsignedByte {
            /**
             * Any button. Note that GLUT doesn't differentiate between mouse
             * buttons when firing the event.
             */
            Left = 1
        };

        /**
         * @brief Set of mouse buttons
         *
         * @see @ref buttons()
         */
        typedef Containers::EnumSet<Button> Buttons;

        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

        /** @brief Mouse buttons */
        constexpr Buttons buttons() const { return _buttons; }

    private:
        constexpr MouseMoveEvent(const Vector2i& position, Buttons buttons): _position(position), _buttons(buttons) {}

        const Vector2i _position;
        const Buttons _buttons;
};

/** @hideinitializer
@brief Entry point for GLUT-based applications
@param className Class name

See @ref Magnum::Platform::GlutApplication "Platform::GlutApplication" for
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
#define MAGNUM_GLUTAPPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef GlutApplication Application;
typedef BasicScreen<GlutApplication> Screen;
typedef BasicScreenedApplication<GlutApplication> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_GLUTAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

CORRADE_ENUMSET_OPERATORS(GlutApplication::MouseMoveEvent::Buttons)

}}

#endif
