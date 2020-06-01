#ifndef Magnum_Platform_Implementation_EglContextHandler_h
#define Magnum_Platform_Implementation_EglContextHandler_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#ifndef SUPPORT_X11
#define SUPPORT_X11 // OpenGL ES on BeagleBoard needs this (?)
#endif
#include <EGL/egl.h>
/* undef Xlib nonsense to avoid conflicts */
#undef None
#undef Complex

#include "Magnum/Magnum.h"
#include "Magnum/Platform/AbstractXApplication.h"
#include "Magnum/Platform/Implementation/AbstractContextHandler.h"

namespace Magnum { namespace Platform { namespace Implementation {

#ifndef DOXYGEN_GENERATING_OUTPUT
/* EGL returns visual ID as int, but Xorg expects long unsigned int */
#ifdef __unix__
typedef VisualID VisualId;
#else
typedef EGLInt VisualId;
#endif
#endif

/**
@brief EGL context

Used in XEglApplication.
*/
class EglContextHandler: public AbstractContextHandler<AbstractXApplication::GLConfiguration, EGLNativeDisplayType, VisualId, EGLNativeWindowType> {
    public:
        explicit EglContextHandler() = default;
        ~EglContextHandler();

        VisualId getVisualId(EGLNativeDisplayType nativeDisplay) override;
        void createContext(const AbstractXApplication::GLConfiguration& glConfiguration, EGLNativeWindowType nativeWindow) override;

        void makeCurrent() override {
            eglMakeCurrent(display, surface, surface, context);
        }

        void swapBuffers() override {
            eglSwapBuffers(display, surface);
        }

    private:
        EGLDisplay display;
        EGLConfig config;
        EGLSurface surface;
        EGLContext context;
};

}}}

#endif
