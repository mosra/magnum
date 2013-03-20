#ifndef Magnum_Platform_GlutApplication_h
#define Magnum_Platform_GlutApplication_h
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
 * @brief Class Magnum::Platform::GlutApplication
 */

#include <string>

#include "Math/Vector2.h"
#include "Magnum.h"
#include "OpenGL.h"

#include <GL/freeglut.h>

namespace Magnum {

class Context;

namespace Platform {

/** @nosubgrouping
@brief GLUT application

Creates double-buffered RGBA window with depth and stencil buffers. Supports
keyboard handling for limited subset of keys, mouse handling with support for
changing cursor and mouse tracking and warping.

@section GlutApplication-usage Usage

You need to implement at least drawEvent() and viewportEvent() to be able to
draw on the screen. The subclass can be then used directly in `main()` -- see
convenience macro MAGNUM_GLUTAPPLICATION_MAIN().
@code
class MyApplication: public Magnum::Platform::GlutApplication {
    // implement required methods...
};
MAGNUM_GLUTAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.
*/
class GlutApplication {
    public:
        class InputEvent;
        class KeyEvent;
        class MouseEvent;
        class MouseMoveEvent;

        /**
         * @brief Constructor
         * @param argc      Count of arguments of `main()` function
         * @param argv      Arguments of `main()` function
         * @param title     Window title
         * @param size      Window size
         */
        explicit GlutApplication(int& argc, char** argv, const std::string& title = "Magnum GLUT application", const Vector2i& size = Vector2i(800, 600));

        virtual ~GlutApplication();

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`.
         */
        inline int exec() {
            glutMainLoop();
            return 0;
        }

        /** @{ @name Drawing functions */

    protected:
        /**
         * @brief Viewport event
         *
         * Called when window size changes. You should pass the new size to
         * DefaultFramebuffer::setViewport() and possibly elsewhere (cameras,
         * other framebuffers...).
         */
        virtual void viewportEvent(const Vector2i& size) = 0;

        /**
         * @brief Draw event
         *
         * Called when the screen is redrawn. You should clean the framebuffer
         * using Framebuffer::clear() and then add your own drawing functions,
         * such as calling SceneGraph::AbstractCamera::draw(). After drawing
         * is finished, call swapBuffers(). If you want to draw immediately
         * again, call also redraw().
         */
        virtual void drawEvent() = 0;

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        inline void swapBuffers() {
            glutSwapBuffers();
        }

        /**
         * @brief Redraw immediately
         *
         * Marks the window for redrawing, resulting in call to drawEvent()
         * in the next iteration.
         */
        virtual inline void redraw() {
            glutPostRedisplay();
        }

        /*@}*/

        /** @{ @name Keyboard handling */

        /**
         * @brief Key press event
         *
         * Called when an key is pressed. Default implementation does nothing.
         */
        virtual void keyPressEvent(KeyEvent& event);

        /*@}*/

        /** @{ @name Mouse handling */

    public:
        /**
         * @brief Mouse cursor
         *
         * @see setMouseCursor()
         */
        enum class MouseCursor: int {
            Default = GLUT_CURSOR_INHERIT,  /**< Default cursor provided by parent window */
            None = GLUT_CURSOR_NONE         /**< No cursor */
        };

        /**
         * @brief Enable or disable mouse tracking
         *
         * When mouse tracking is enabled, mouseMoveEvent() is called even
         * when no button is pressed. Mouse tracking is disabled by default.
         */
        inline void setMouseTracking(bool enabled) {
            glutPassiveMotionFunc(enabled ? staticMouseMoveEvent : nullptr);
        }

        /** @brief Set mouse cursor */
        inline void setMouseCursor(MouseCursor cursor) {
            glutSetCursor(static_cast<int>(cursor));
        }

        /** @brief Warp mouse cursor to given coordinates */
        inline void warpMouseCursor(const Vector2i& position) {
            glutWarpPointer(position.x(), position.y());
        }

    protected:
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
         * Called when any mouse button is pressed and mouse is moved. Default
         * implementation does nothing.
         * @see setMouseTracking()
         */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

        /*@}*/

    private:
        inline static void staticViewportEvent(int x, int y) {
            instance->viewportEvent({x, y});
        }

        static void staticKeyEvent(int key, int x, int y);

        static void staticMouseEvent(int button, int state, int x, int y);

        static void staticMouseMoveEvent(int x, int y);

        inline static void staticDrawEvent() {
            instance->drawEvent();
        }

        static GlutApplication* instance;

        Context* c;
};

/**
@brief Base for input events

@see KeyEvent, MouseEvent, MouseMoveEvent, keyPressEvent(), mousePressEvent(),
    mouseReleaseEvent(), mouseMoveEvent()
*/
class GlutApplication::InputEvent {
    InputEvent(const InputEvent&) = delete;
    InputEvent(InputEvent&&) = delete;
    InputEvent& operator=(const InputEvent&) = delete;
    InputEvent& operator=(InputEvent&&) = delete;

    public:
        inline virtual ~InputEvent() {}

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere. By default is each event ignored.
         */
        inline void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Whether the event is accepted */
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

@see keyPressEvent()
*/
class GlutApplication::KeyEvent: public GlutApplication::InputEvent {
    friend class GlutApplication;

    public:
        /**
         * @brief Key
         *
         * @see key()
         */
        enum class Key: int {
            Up = GLUT_KEY_UP,               /**< Up arrow */
            Down = GLUT_KEY_DOWN,           /**< Down arrow */
            Left = GLUT_KEY_LEFT,           /**< Left arrow */
            Right = GLUT_KEY_RIGHT,         /**< Right arrow */
            F1 = GLUT_KEY_F1,               /**< F1 */
            F2 = GLUT_KEY_F2,               /**< F2 */
            F3 = GLUT_KEY_F3,               /**< F3 */
            F4 = GLUT_KEY_F4,               /**< F4 */
            F5 = GLUT_KEY_F5,               /**< F5 */
            F6 = GLUT_KEY_F6,               /**< F6 */
            F7 = GLUT_KEY_F7,               /**< F7 */
            F8 = GLUT_KEY_F8,               /**< F8 */
            F9 = GLUT_KEY_F9,               /**< F9 */
            F10 = GLUT_KEY_F10,             /**< F10 */
            F11 = GLUT_KEY_F11,             /**< F11 */
            F12 = GLUT_KEY_F12,             /**< F12 */
            Home = GLUT_KEY_HOME,           /**< Home */
            End = GLUT_KEY_END,             /**< End */
            PageUp = GLUT_KEY_PAGE_UP,      /**< Page up */
            PageDown = GLUT_KEY_PAGE_DOWN   /**< Page down */
        };

        /** @brief Key */
        inline Key key() const { return _key; }

        /** @brief Position */
        inline Vector2i position() const { return _position; }

    private:
        inline KeyEvent(Key key, const Vector2i& position): _key(key), _position(position) {}

        const Key _key;
        const Vector2i _position;
};

/**
@brief Mouse event

@see MouseMoveEvent, mousePressEvent(), mouseReleaseEvent()
*/
class GlutApplication::MouseEvent: public GlutApplication::InputEvent {
    friend class GlutApplication;

    public:
        /**
         * @brief Mouse button
         *
         * @see button()
         */
        enum class Button: int {
            Left = GLUT_LEFT_BUTTON,        /**< Left button */
            Middle = GLUT_MIDDLE_BUTTON,    /**< Middle button */
            Right = GLUT_RIGHT_BUTTON,      /**< Right button */
            WheelUp = 3,                    /**< Wheel up */
            WheelDown = 4                   /**< Wheel down */
        };

        /** @brief Button */
        inline Button button() const { return _button; }

        /** @brief Position */
        inline Vector2i position() const { return _position; }

    private:
        inline MouseEvent(Button button, const Vector2i& position): _button(button), _position(position) {}

        const Button _button;
        const Vector2i _position;
};

/**
@brief Mouse move event

@see MouseEvent, mouseMoveEvent()
*/
class GlutApplication::MouseMoveEvent: public GlutApplication::InputEvent {
    friend class GlutApplication;

    public:
        /** @brief Position */
        inline Vector2i position() const { return _position; }

    private:
        inline MouseMoveEvent(const Vector2i& position): _position(position) {}

        const Vector2i _position;
};

/** @hideinitializer
@brief Entry point for GLUT-based applications
@param className Class name

Can be with GlutApplication subclasses used as equivalent to the following
code to achieve better portability, see @ref portability-applications for more
information.
@code
int main(int argc, char** argv) {
    className app(argc, argv);
    return app.exec();
}
@endcode
When no other application header is included this macro is also aliased to
`MAGNUM_APPLICATION_MAIN()`.
*/
#define MAGNUM_GLUTAPPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app(argc, argv);                                          \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef GlutApplication Application;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_GLUTAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

/* Implementations for inline functions with unused parameters */
inline void GlutApplication::keyPressEvent(KeyEvent&) {}
inline void GlutApplication::mousePressEvent(MouseEvent&) {}
inline void GlutApplication::mouseReleaseEvent(MouseEvent&) {}
inline void GlutApplication::mouseMoveEvent(MouseMoveEvent&) {}

}}

#endif
