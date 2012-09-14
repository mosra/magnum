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

#include "EglContext.h"

#include "Context.h"

namespace Magnum { namespace Contexts {

EglContext::~EglContext() {
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);
}

VisualId EglContext::getVisualId(EGLNativeDisplayType nativeDisplay) {
    /* Initialize */
    display = eglGetDisplay(nativeDisplay);
    eglInitialize(display, 0, 0);
    #ifndef MAGNUM_TARGET_GLES
    eglBindAPI(EGL_OPENGL_API);
    #else
    eglBindAPI(EGL_OPENGL_ES_API);
    #endif

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
        Error() << "Cannot get EGL visual config";
        exit(1);
    }

    /* Get visual ID */
    EGLint visualId;
    if(!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &visualId)) {
        Error() << "Cannot get native visual ID";
        exit(1);
    }

    return visualId;
}

void EglContext::createContext(EGLNativeWindowType window) {
    static const EGLint contextAttributes[] = {
        #ifdef MAGNUM_TARGET_GLES
        EGL_CONTEXT_CLIENT_VERSION, 2,
        #endif
        EGL_NONE
    };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);
    if(!context) {
        Error() << "Cannot create EGL context";
        exit(1);
    }
    surface = eglCreateWindowSurface(display, config, window, NULL);
    if(!surface) {
        Error() << "Cannot create window surface";
        exit(1);
    }

    /** @bug Fixme: On desktop OpenGL and Mesa EGL implementation OpenGL version is 1.0, which is wrong */
}

}}
