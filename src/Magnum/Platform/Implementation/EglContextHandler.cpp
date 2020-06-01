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

#include "EglContextHandler.h"

#include <tuple>
#include <EGL/eglext.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Version.h"

#include "Magnum/Platform/Implementation/Egl.h"

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
        Error() << "Cannot initialize EGL:" << Implementation::eglErrorString(eglGetError());
        std::exit(1);
    }

    #ifndef MAGNUM_TARGET_GLES
    EGLenum api = EGL_OPENGL_API;
    #else
    EGLenum api = EGL_OPENGL_ES_API;
    #endif
    if(!eglBindAPI(api)) {
        Error() << "Cannot bind EGL API:" << Implementation::eglErrorString(eglGetError());
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
    if(!eglChooseConfig(display, attribs, &config, 1, &configCount)) {
        Error() << "Cannot get EGL visual config:" << Implementation::eglErrorString(eglGetError());
        std::exit(1);
    }

    if(!configCount) {
        Error() << "No matching EGL visual config available";
        std::exit(1);
    }

    /* Get visual ID */
    EGLint visualId;
    if(!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &visualId)) {
        Error() << "Cannot get native visual ID:" << Implementation::eglErrorString(eglGetError());
        std::exit(1);
    }

    return visualId;
}

void EglContextHandler::createContext(const AbstractXApplication::GLConfiguration& glConfiguration, EGLNativeWindowType window) {
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
    if(glConfiguration.version() != GL::Version::None) {
        Int major, minor;
        std::tie(major, minor) = version(glConfiguration.version());

        attributes[0] = EGL_CONTEXT_MAJOR_VERSION_KHR;
        attributes[1] = major;
        attributes[2] = EGL_CONTEXT_MINOR_VERSION_KHR;
        attributes[3] = minor;

        #ifndef MAGNUM_TARGET_GLES
        if(glConfiguration.version() >= GL::Version::GL310) {
            attributes[4] = EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR;
            attributes[5] = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR;
        }
        #endif
    }

    /* We need this to run ES (default is desktop GL) */
    #ifdef MAGNUM_TARGET_GLES
    else {
        attributes[0] = EGL_CONTEXT_CLIENT_VERSION;
        #ifdef MAGNUM_TARGET_GLES3
        attributes[1] = 3;
        #elif defined(MAGNUM_TARGET_GLES2)
        attributes[1] = 2;
        #else
        #error unsupported OpenGL ES version
        #endif
    }
    #endif

    if(!(context = eglCreateContext(display, config, EGL_NO_CONTEXT, attributes))) {
        Error() << "Cannot create EGL context:" << Implementation::eglErrorString(eglGetError());
        std::exit(1);
    }
    if(!(surface = eglCreateWindowSurface(display, config, window, nullptr))) {
        Error() << "Cannot create window surface:" << Implementation::eglErrorString(eglGetError());
        std::exit(1);
    }

    /** @bug Fixme: On desktop OpenGL and Mesa EGL implementation OpenGL version is 1.0, which is wrong */
}

}}}
