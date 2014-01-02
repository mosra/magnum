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

#include "EglContextHandler.h"

#include <tuple>
#include <EGL/eglext.h>
#include <Utility/Debug.h>

#include "Context.h"
#include "Version.h"

namespace Magnum { namespace Platform { namespace Implementation {

EglContextHandler::~EglContextHandler() {
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);
}

VisualId EglContextHandler::getVisualId(EGLNativeDisplayType nativeDisplay) {
    /* Initialize */
    display = eglGetDisplay(nativeDisplay);
    if(!eglInitialize(display, nullptr, nullptr)) {
        Error() << "Cannot initialize EGL:" << errorString(eglGetError());
        std::exit(1);
    }

    #ifndef MAGNUM_TARGET_GLES
    EGLenum api = EGL_OPENGL_API;
    #else
    EGLenum api = EGL_OPENGL_ES_API;
    #endif
    if(!eglBindAPI(api)) {
        Error() << "Cannot bind EGL API:" << errorString(eglGetError());
        std::exit(1);
    }

    /* Choose EGL config */
    static const EGLint attribs[] = {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_DEPTH_SIZE, 1,
        #ifndef MAGNUM_TARGET_GLES
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        #else
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        #endif
        EGL_NONE
    };
    EGLint configCount;
    if(!eglChooseConfig(display, attribs, &config, 1, &configCount)) {
        Error() << "Cannot get EGL visual config:" << errorString(eglGetError());
        std::exit(1);
    }

    if(!configCount) {
        Error() << "No matching EGL visual config available";
        std::exit(1);
    }

    /* Get visual ID */
    EGLint visualId;
    if(!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &visualId)) {
        Error() << "Cannot get native visual ID:" << errorString(eglGetError());
        std::exit(1);
    }

    return visualId;
}

void EglContextHandler::createContext(const AbstractXApplication::Configuration& configuration, EGLNativeWindowType window) {
    EGLint attributes[] = {
        /* Leave some space for optional attributes below */
        EGL_NONE, EGL_NONE,
        EGL_NONE, EGL_NONE,
        EGL_NONE, EGL_NONE,

        EGL_NONE
    };

    /* Set context version, if requested. On desktop needs
       EGL_KHR_create_context. */
    /** @todo Test for presence of EGL_KHR_create_context extension */
    if(configuration.version() != Version::None) {
        Int major, minor;
        std::tie(major, minor) = version(configuration.version());

        attributes[0] = EGL_CONTEXT_MAJOR_VERSION_KHR;
        attributes[1] = major;
        attributes[2] = EGL_CONTEXT_MINOR_VERSION_KHR;
        attributes[3] = minor;

        #ifndef MAGNUM_TARGET_GLES
        if(configuration.version() >= Version::GL310) {
            attributes[4] = EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR;
            attributes[5] = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR;
        }
        #endif
    }

    /* We need this to run ES (default is desktop GL) */
    #ifdef MAGNUM_TARGET_GLES
    else {
        attributes[0] = EGL_CONTEXT_CLIENT_VERSION;
        attributes[1] = 2;
    }
    #endif

    if(!eglCreateContext(display, config, EGL_NO_CONTEXT, attributes)) {
        Error() << "Cannot create EGL context:" << errorString(eglGetError());
        std::exit(1);
    }
    if(!(surface = eglCreateWindowSurface(display, config, window, nullptr))) {
        Error() << "Cannot create window surface:" << errorString(eglGetError());
        std::exit(1);
    }

    /** @bug Fixme: On desktop OpenGL and Mesa EGL implementation OpenGL version is 1.0, which is wrong */
}

const char* EglContextHandler::errorString(EGLint error) {
    switch(error) {
        #define _error(name) case name: return #name;
        _error(EGL_SUCCESS)
        _error(EGL_NOT_INITIALIZED)
        _error(EGL_BAD_ACCESS)
        _error(EGL_BAD_ALLOC)
        _error(EGL_BAD_ATTRIBUTE)
        _error(EGL_BAD_CONTEXT)
        _error(EGL_BAD_CONFIG)
        _error(EGL_BAD_CURRENT_SURFACE)
        _error(EGL_BAD_DISPLAY)
        _error(EGL_BAD_SURFACE)
        _error(EGL_BAD_MATCH)
        _error(EGL_BAD_PARAMETER)
        _error(EGL_BAD_NATIVE_PIXMAP)
        _error(EGL_BAD_NATIVE_WINDOW)
        _error(EGL_CONTEXT_LOST)
        #undef _error
    }

    return {};
}

}}}
