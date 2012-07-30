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

#define None 0L // redef Xlib nonsense

using namespace std;

namespace Magnum { namespace Contexts {

EglContext::EglContext(int&, char**, const string& title, const Math::Vector2<GLsizei>& size): viewportSize(size) {
    /* Get default X display and root window, init EGL */
    xDisplay = XOpenDisplay(0);
    display = eglGetDisplay(xDisplay);
    eglInitialize(display, 0, 0);
    #ifndef MAGNUM_TARGET_GLES
    eglBindAPI(EGL_OPENGL_API);
    #else
    eglBindAPI(EGL_OPENGL_ES_API);
    #endif
    int screenNumber = DefaultScreen(xDisplay);
    Window root = RootWindow(xDisplay, screenNumber);

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
    EGLConfig config;
    EGLint configCount;
    if(!eglChooseConfig(display, attribs, &config, 1, &configCount)) {
        Error() << "Cannot get EGL visual config";
        exit(1);
    }

    /* Get X visual */
    EGLint visualId;
    if(!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &visualId)) {
        Error() << "Cannot get native visual ID";
        exit(1);
    }
    XVisualInfo *visInfo, visTemplate;
    int visualCount;
    visTemplate.visualid = visualId;
    visInfo = XGetVisualInfo(xDisplay, VisualIDMask, &visTemplate, &visualCount);
    if(!visInfo) {
        Error() << "Cannot get X visual";
        exit(1);
    }

    /* Create X Window */
    XSetWindowAttributes attr;
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap( xDisplay, root, visInfo->visual, AllocNone);
    attr.event_mask = StructureNotifyMask|ExposureMask|KeyPressMask;
    unsigned long mask = CWBackPixel|CWBorderPixel|CWColormap|CWEventMask;
    xWindow = XCreateWindow(xDisplay, root, 20, 20, size.x(), size.y(), 0, visInfo->depth, InputOutput, visInfo->visual, mask, &attr);
    XSetStandardProperties(xDisplay, xWindow, title.c_str(), 0, None, 0, 0, 0);
    XFree(visInfo);

    /* Create context and window surface */
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
    surface = eglCreateWindowSurface(display, config, xWindow, NULL);
    if(!surface) {
        Error() << "Cannot create window surface";
        exit(1);
    }

    /* Show window and set OpenGL context as current */
    XMapWindow(xDisplay, xWindow);
    eglMakeCurrent(display, surface, surface, context);

    /** @bug Fixme: GLEW initialization fails (thinks that the context is not created) */
    #ifndef MAGNUM_TARGET_GLES
    /* Init GLEW */
    GLenum err = glewInit();
    if(err != GLEW_OK) {
        Error() << "EglContext: cannot initialize GLEW:" << glewGetErrorString(err);
        exit(1);
    }
    #endif
}

EglContext::~EglContext() {
    /* Shut down EGL */
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);

    /* Shut down X */
    XDestroyWindow(xDisplay, xWindow);
    XCloseDisplay(xDisplay);
}

int EglContext::exec() {
    /* Call viewportEvent for the first time */
    viewportEvent(viewportSize);

    while(true) {
        /** @todo Handle at least window closing and resizing */
        eglMakeCurrent(display, surface, surface, context);
        drawEvent();
    }

    return 0;
}

}}
