/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include "WindowlessWindowsEglApplication.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Version.h"
#include "Magnum/Platform/Context.h"

#include "Implementation/Egl.h"

namespace Magnum { namespace Platform {

#ifndef DOXYGEN_GENERATING_OUTPUT
int WindowlessWindowsEglApplication::create(LRESULT(CALLBACK windowProcedure)(HWND, UINT, WPARAM, LPARAM)) {
    const WNDCLASS wc{
        0,
        windowProcedure,
        0,
        0,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        HBRUSH(COLOR_BACKGROUND),
        nullptr,
        L"Magnum Windowless Application"
    };
    if(!RegisterClass(&wc)) return 1;

    CreateWindowW(wc.lpszClassName, L"Magnum Windowless Application",
        WS_OVERLAPPEDWINDOW, 0, 0, 32, 32, 0, 0, wc.hInstance, 0);

    /* Hammer the return code out of the messaging thingy */
    MSG msg;
    do {} while(GetMessageW(&msg, nullptr, 0, 0) != 0);
    return msg.wParam;
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessWindowsEglApplication::WindowlessWindowsEglApplication(const Arguments& arguments): WindowlessWindowsEglApplication{arguments, Configuration{}} {}
#endif

WindowlessWindowsEglApplication::WindowlessWindowsEglApplication(const Arguments& arguments, const Configuration& configuration): WindowlessWindowsEglApplication{arguments, nullptr} {
    createContext(configuration);
}

WindowlessWindowsEglApplication::WindowlessWindowsEglApplication(const Arguments& arguments, std::nullptr_t): _window(arguments.window), _context{new Context{NoCreate, arguments.argc, arguments.argv}} {}

void WindowlessWindowsEglApplication::createContext() { createContext({}); }

void WindowlessWindowsEglApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessWindowsEglApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == Version::None, "Platform::WindowlessWindowsEglApplication::tryCreateContext(): context already created", false);

    /* Initialize */
    _display = eglGetDisplay(GetDC(_window));
    if(!eglInitialize(_display, nullptr, nullptr)) {
        Error() << "Platform::WindowlessWindowsEglApplication::tryCreateContext(): cannot initialize EGL:" << Implementation::eglErrorString(eglGetError());
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
        Error() << "Platform::WindowlessWindowsEglApplication::tryCreateContext(): cannot bind EGL API:" << Implementation::eglErrorString(eglGetError());
        return false;
    }

    /* Choose EGL config */
    static const EGLint attribs[] = {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_DEPTH_SIZE, 1,
        #ifndef MAGNUM_TARGET_GLES
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        #elif defined(MAGNUM_TARGET_GLES3)
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        #elif defined(MAGNUM_TARGET_GLES2)
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        #else
        #error unsupported OpenGL edition
        #endif
        EGL_NONE
    };
    EGLint configCount;
    if(!eglChooseConfig(_display, attribs, &_config, 1, &configCount)) {
        Error() << "Platform::WindowlessWindowsEglApplication::tryCreateContext(): cannot get EGL visual config:" << Implementation::eglErrorString(eglGetError());
        return false;
    }

    if(!configCount) {
        Error() << "Platform::WindowlessWindowsEglApplication::tryCreateContext(): no matching EGL visual config available";
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
        Error() << "Platform::WindowlessWindowsEglApplication::tryCreateContext(): cannot create EGL context:" << Implementation::eglErrorString(eglGetError());
        return false;
    }
    if(!(_surface = eglCreateWindowSurface(_display, _config, _window, nullptr))) {
        Error() << "Platform::WindowlessWindowsEglApplication::tryCreateContext(): cannot create window surface:" << Implementation::eglErrorString(eglGetError());
        return false;
    }

    if(!eglMakeCurrent(_display, _surface, _surface, _glContext)) {
        Error() << "Platform::WindowlessWindowsEglApplication::tryCreateContext(): cannot make context current:" << Implementation::eglErrorString(eglGetError());
        return false;
    }

    /* Return true if the initialization succeeds */
    return _context->tryCreate();
}

WindowlessWindowsEglApplication::~WindowlessWindowsEglApplication() {
    _context.reset();

    eglDestroyContext(_display, _glContext);
    eglDestroySurface(_display, _surface);
    eglTerminate(_display);
}

}}
