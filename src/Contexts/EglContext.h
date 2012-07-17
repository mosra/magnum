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

    protected:
        /** @copydoc GlutContext::viewportEvent() */
        virtual void viewportEvent(const Math::Vector2<GLsizei>& size) = 0;

        /** @copydoc GlutContext::drawEvent() */
        virtual void drawEvent() = 0;

        /** @copydoc GlutContext::swapBuffers() */
        inline void swapBuffers() { eglSwapBuffers(display, surface); }

    private:
        Display* xDisplay;
        Window xWindow;

        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;

        /** @todo Get this from the created window */
        Math::Vector2<GLsizei> viewportSize;
};

}}

#endif
