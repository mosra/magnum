#ifndef Magnum_Contexts_EglContext_h
#define Magnum_Contexts_EglContext_h
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
 * @brief Class Magnum::Contexts::EglContext
 */

#include "Magnum.h"

#include <X11/Xlib.h>
#ifdef None // undef Xlib nonsense to avoid conflicts
#undef None
#endif

#ifndef SUPPORT_X11
#define SUPPORT_X11 // OpenGL ES on BeagleBoard needs this (?)
#endif
#include <EGL/egl.h>

#include "AbstractContext.h"

namespace Magnum { namespace Contexts {

/**
@brief X/EGL context

Currently only barebone implementation with no event handling.
*/
class EglContext: public AbstractContext {
    public:
        /**
         * @brief Constructor
         * @param argc      Count of arguments of `main()` function
         * @param argv      Arguments of `main()` function
         * @param title     Window title
         * @param size      Window size
         *
         * Creates window with double-buffered OpenGL ES 2 context.
         */
        EglContext(int& argc, char** argv, const std::string& title = "Magnum X/EGL context", const Math::Vector2<GLsizei>& size = Math::Vector2<GLsizei>(800, 600));

        /**
         * @brief Destructor
         *
         * Deletes context and destroys the window.
         */
        ~EglContext();

        int exec();

        /** @{ @name Drawing functions */

    protected:
        /** @copydoc GlutContext::viewportEvent() */
        virtual void viewportEvent(const Math::Vector2<GLsizei>& size) = 0;

        /** @copydoc GlutContext::drawEvent() */
        virtual void drawEvent() = 0;

        /** @copydoc GlutContext::swapBuffers() */
        inline void swapBuffers() { eglSwapBuffers(display, surface); }

        /*@}*/

        /** @{ @name Keyboard handling */

    public:
        /** @brief Key */
        enum class Key: KeySym {
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
            PageDown = XK_Page_Down     /**< Page down */
        };

    protected:
        /**
         * @brief Key press event
         * @param key       Key pressed
         * @param position  Cursor position
         *
         * Called when an key is pressed. Default implementation does nothing.
         */
        virtual void keyPressEvent(Key key, const Math::Vector2<int>& position);

        /**
         * @brief Key press event
         * @param key       Key released
         * @param position  Cursor position
         *
         * Called when an key is released. Default implementation does nothing.
         */
        virtual void keyReleaseEvent(Key key, const Math::Vector2<int>& position);

        /*@}*/

        /** @{ @name Mouse handling */
    public:
        /** @brief Mouse button */
        enum class MouseButton: unsigned int {
            Left = Button1,         /**< Left button */
            Middle = Button2,       /**< Middle button */
            Right = Button3,        /**< Right button */
            WheelUp = Button4,      /**< Wheel up */
            WheelDown = Button5     /**< Wheel down */
        };

    protected:
        /**
         * @brief Mouse press event
         *
         * Called when mouse button is pressed. Default implementation does
         * nothing.
         */
        virtual void mousePressEvent(MouseButton button, const Math::Vector2<int>& position);

        /**
         * @brief Mouse release event
         *
         * Called when mouse button is released. Default implementation does
         * nothing.
         */
        virtual void mouseReleaseEvent(MouseButton button, const Math::Vector2<int>& position);

    private:
        Display* xDisplay;
        Window xWindow;
        Atom deleteWindow;

        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;

        /** @todo Get this from the created window */
        Math::Vector2<GLsizei> viewportSize;
};

inline void EglContext::keyPressEvent(EglContext::Key, const Math::Vector2<int>&) {}
inline void EglContext::keyReleaseEvent(EglContext::Key, const Math::Vector2<int>&) {}
inline void EglContext::mousePressEvent(EglContext::MouseButton, const Math::Vector2<int>&) {}
inline void EglContext::mouseReleaseEvent(EglContext::MouseButton, const Math::Vector2<int>&) {}


}}

#endif
