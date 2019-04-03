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

#include <string>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"

#include "Implementation/Egl.h"

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
#include <Corrade/Containers/ArrayView.h>
#endif

namespace Magnum { namespace Platform {

WindowlessEglContext::WindowlessEglContext(const Configuration& configuration, GLContext* const magnumContext) {
    /* Initialize */
    _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(!eglInitialize(_display, nullptr, nullptr)) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot initialize EGL:" << Implementation::eglErrorString(eglGetError());
        return;
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
        return;
    }

    /* Choose EGL config */
    static const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        #ifndef MAGNUM_TARGET_GLES
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        #elif defined(MAGNUM_TARGET_GLES2) || defined(CORRADE_TARGET_EMSCRIPTEN)
        /* Emscripten doesn't know about EGL_OPENGL_ES3_BIT_KHR for WebGL 2 and
           the whole thing is controlled only by EGL_CONTEXT_CLIENT_VERSION and
           the `-s USE_WEBGL2=1` flag anyway, so it doesn't matter that we ask
           for ES2 on WebGL 2 as well. */
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        #elif defined(MAGNUM_TARGET_GLES3)
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
        #else
        #error unsupported OpenGL edition
        #endif
        EGL_NONE
    };
    EGLConfig config;
    EGLint configCount;
    if(!eglChooseConfig(_display, attribs, &config, 1, &configCount)) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot get EGL visual config:" << Implementation::eglErrorString(eglGetError());
        return;
    }

    if(!configCount) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): no matching EGL visual config available";
        return;
    }

    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_WEBGL)
    const /* Is modified below to work around a SwiftShader limitation */
    #endif
    EGLint attributes[] = {
        #ifdef MAGNUM_TARGET_GLES
        EGL_CONTEXT_CLIENT_VERSION,
            #if defined(MAGNUM_TARGET_GLES2) || (defined(CORRADE_TARGET_EMSCRIPTEN) && __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ < 13824)
            /* Emscripten before 1.38.24 doesn't know about version 3 for WebGL
               2 and the whole thing is controlled by -s USE_WEBGL2=1 flag
               anyway, so it doesn't matter that we ask for ES2 on WebGL 2 as
               well. https://github.com/emscripten-core/emscripten/pull/7858 */
            2,
            #elif defined(MAGNUM_TARGET_GLES3)
            3,
            #else
            #error unsupported OpenGL ES version
            #endif
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        /* Needs to be last because we're zeroing this out for SwiftShader (see
           below) */
        EGL_CONTEXT_FLAGS_KHR, EGLint(configuration.flags()),
        #endif
        EGL_NONE
    };

    #ifdef MAGNUM_TARGET_WEBGL
    static_cast<void>(configuration);
    #endif
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_WEBGL)
    static_cast<void>(magnumContext);
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    const std::string version = eglQueryString(_display, EGL_VERSION);

    /* SwiftShader 3.3.0.1 blows up on encountering EGL_CONTEXT_FLAGS_KHR with
       a zero value, so erase these. It also doesn't handle them as correct
       flags, but instead checks for the whole value, so a combination won't
       work either: https://github.com/google/swiftshader/blob/5fb5e817a20d3e60f29f7338493f922b5ac9d7c4/src/OpenGL/libEGL/libEGL.cpp#L794-L8104 */
    if(!configuration.flags() && version.find("SwiftShader") != std::string::npos && (!magnumContext || !magnumContext->isDriverWorkaroundDisabled("swiftshader-no-empty-egl-context-flags"))) {
        auto& contextFlags = attributes[Containers::arraySize(attributes) - 3];
        CORRADE_INTERNAL_ASSERT(contextFlags == EGL_CONTEXT_FLAGS_KHR);
        contextFlags = EGL_NONE;
    }
    #endif

    if(!(_context = eglCreateContext(_display, config, EGL_NO_CONTEXT, attributes))) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot create EGL context:" << Implementation::eglErrorString(eglGetError());
        return;
    }

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    /* SwiftShader 3.3.0.1 needs some pbuffer, otherwise it crashes somewhere
       deep inside when making the context current */
    if(version.find("SwiftShader") != std::string::npos && (!magnumContext || !magnumContext->isDriverWorkaroundDisabled("swiftshader-egl-context-needs-pbuffer"))) {
        EGLint surfaceAttributes[] = {
            EGL_WIDTH, 32,
            EGL_HEIGHT, 32,
            EGL_NONE
        };
        _surface = eglCreatePbufferSurface(_display, config, surfaceAttributes);
    }
    #endif
}

WindowlessEglContext::WindowlessEglContext(WindowlessEglContext&& other): _display{other._display}, _context{other._context} {
    other._display = {};
    other._context = {};
}

WindowlessEglContext::~WindowlessEglContext() {
    if(_context) eglDestroyContext(_display, _context);
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(_surface) eglDestroySurface(_display, _surface);
    #endif
    if(_display) eglTerminate(_display);
}

WindowlessEglContext& WindowlessEglContext::operator=(WindowlessEglContext && other) {
    using std::swap;
    swap(other._display, _display);
    swap(other._context, _context);
    return *this;
}

bool WindowlessEglContext::makeCurrent() {
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    /* _surface is EGL_NO_SURFACE everywhere except on SwiftShader. See above
       for details. */
    if(eglMakeCurrent(_display, _surface, _surface, _context))
        return true;
    #else
    if(eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, _context))
        return true;
    #endif

    Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot make context current:" << Implementation::eglErrorString(eglGetError());
    return false;
}

WindowlessEglContext::Configuration::Configuration()
    #ifndef MAGNUM_TARGET_GLES
    : _flags{Flag::ForwardCompatible}
    #elif !defined(MAGNUM_TARGET_WEBGL)
    : _flags{}
    #endif
    {}

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessEglApplication::WindowlessEglApplication(const Arguments& arguments): WindowlessEglApplication{arguments, Configuration{}} {}
#endif

WindowlessEglApplication::WindowlessEglApplication(const Arguments& arguments, const Configuration& configuration): WindowlessEglApplication{arguments, NoCreate} {
    createContext(configuration);
}

WindowlessEglApplication::WindowlessEglApplication(const Arguments& arguments, NoCreateT): _glContext{NoCreate}, _context{new GLContext{NoCreate, arguments.argc, arguments.argv}} {}

void WindowlessEglApplication::createContext() { createContext({}); }

void WindowlessEglApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessEglApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == GL::Version::None, "Platform::WindowlessEglApplication::tryCreateContext(): context already created", false);

    WindowlessEglContext glContext{configuration, _context.get()};
    if(!glContext.isCreated() || !glContext.makeCurrent() || !_context->tryCreate())
        return false;

    _glContext = std::move(glContext);
    return true;
}

WindowlessEglApplication::~WindowlessEglApplication() = default;

}}
