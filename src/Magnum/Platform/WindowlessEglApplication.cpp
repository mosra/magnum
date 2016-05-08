/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "WindowlessEglApplication.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Version.h"
#include "Magnum/Platform/Context.h"

#include "Implementation/Egl.h"

namespace Magnum { namespace Platform {

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessEglApplication::WindowlessEglApplication(const Arguments& arguments): WindowlessEglApplication{arguments, Configuration{}} {}
#endif

WindowlessEglApplication::WindowlessEglApplication(const Arguments& arguments, const Configuration& configuration): WindowlessEglApplication{arguments, nullptr} {
    createContext(configuration);
}

WindowlessEglApplication::WindowlessEglApplication(const Arguments& arguments, std::nullptr_t): _context{new Context{NoCreate, arguments.argc, arguments.argv}} {}

void WindowlessEglApplication::createContext() { createContext({}); }

void WindowlessEglApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessEglApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == Version::None, "Platform::WindowlessEglApplication::tryCreateContext(): context already created", false);

    /* Initialize */
    _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(!eglInitialize(_display, nullptr, nullptr)) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot initialize EGL:" << Implementation::eglErrorString(eglGetError());
        return false;
    }

    const EGLenum api =
        #ifndef MAGNUM_TARGET_GLES
        EGL_OPENGL_API
        #else
        EGL_OPENGL_ES_API
        #endif
        ;
    if(!eglBindAPI(api)) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot bind EGL API:" << Implementation::eglErrorString(eglGetError());
        return false;
    }

    /* Choose EGL config */
    static const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        #ifndef MAGNUM_TARGET_GLES
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        #elif defined(MAGNUM_TARGET_GLES3)
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
        #elif defined(MAGNUM_TARGET_GLES2)
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        #else
        #error unsupported OpenGL edition
        #endif
        EGL_NONE
    };
    EGLint configCount;
    if(!eglChooseConfig(_display, attribs, &_config, 1, &configCount)) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot get EGL visual config:" << Implementation::eglErrorString(eglGetError());
        return false;
    }

    if(!configCount) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): no matching EGL visual config available";
        return false;
    }

    const EGLint attributes[] = {
        #ifdef MAGNUM_TARGET_GLES
        EGL_CONTEXT_CLIENT_VERSION,
            #ifdef MAGNUM_TARGET_GLES3
            3,
            #elif defined(MAGNUM_TARGET_GLES2)
            2,
            #else
            #error unsupported OpenGL ES version
            #endif
        #endif
        EGL_CONTEXT_FLAGS_KHR, EGLint(configuration.flags()),
        EGL_NONE
    };

    if(!(_glContext = eglCreateContext(_display, _config, EGL_NO_CONTEXT, attributes))) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot create EGL context:" << Implementation::eglErrorString(eglGetError());
        return false;
    }
    if(!eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, _glContext)) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot make context current:" << Implementation::eglErrorString(eglGetError());
        return false;
    }

    /* Return true if the initialization succeeds */
    return _context->tryCreate();
}

WindowlessEglApplication::~WindowlessEglApplication() {
    _context.reset();

    eglDestroyContext(_display, _glContext);
    eglDestroySurface(_display, EGL_NO_SURFACE);
    eglTerminate(_display);
}

}}
