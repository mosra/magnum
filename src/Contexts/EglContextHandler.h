#ifndef Magnum_Contexts_EglContextHandler_h
#define Magnum_Contexts_EglContextHandler_h
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
 * @brief Class Magnum::Contexts::EglContextHandler
 */

#include "Magnum.h"

#ifndef SUPPORT_X11
#define SUPPORT_X11 // OpenGL ES on BeagleBoard needs this (?)
#endif
#include <EGL/egl.h>

/* undef Xlib nonsense to avoid conflicts */
#undef None

#include "AbstractContextHandler.h"

#include "magnumCompatibility.h"

namespace Magnum { namespace Contexts {

#ifndef DOXYGEN_GENERATING_OUTPUT
/* EGL returns visual ID as int, but Xorg expects long unsigned int */
#ifdef __unix__
typedef VisualID VisualId;
#else
typedef EGLInt VisualId;
#endif
#endif

/**
@brief EGL interface

Used in XEglWindowContext.
*/
class EglContextHandler: public AbstractContextHandler<EGLNativeDisplayType, VisualId, EGLNativeWindowType> {
    public:
        ~EglContextHandler();

        VisualId getVisualId(EGLNativeDisplayType nativeDisplay) override;
        void createContext(EGLNativeWindowType nativeWindow) override;

        inline void makeCurrent() override {
            eglMakeCurrent(display, surface, surface, context);
        }

        inline void swapBuffers() override {
            eglSwapBuffers(display, surface);
        }

    private:
        EGLDisplay display;
        EGLConfig config;
        EGLSurface surface;
        EGLContext context;
};

}}

#endif
