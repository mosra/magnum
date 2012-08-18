#ifndef Magnum_Contexts_EglInterface_h
#define Magnum_Contexts_EglInterface_h
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
 * @brief Class Magnum::Contexts::EglInterface
 */

#include "Magnum.h"

#ifndef SUPPORT_X11
#define SUPPORT_X11 // OpenGL ES on BeagleBoard needs this (?)
#endif
#include <EGL/egl.h>

#include "AbstractGlInterface.h"

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

Used in XEglContext.
*/
class EglInterface: public AbstractGlInterface<EGLNativeDisplayType, VisualId, EGLNativeWindowType> {
    public:
        ~EglInterface();

        VisualId getVisualId(EGLNativeDisplayType nativeDisplay);
        void createContext(EGLNativeWindowType nativeWindow);

        inline void makeCurrent() {
            eglMakeCurrent(display, surface, surface, context);
        }

        inline void swapBuffers() {
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
