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

#include "GlxContext.h"

#include <GL/glxext.h>
#include <Utility/Debug.h>

namespace Magnum { namespace Contexts {

VisualID GlxContext::getVisualId(Display* nativeDisplay) {
    display = nativeDisplay;

    /* Check version */
    int major, minor;
    glXQueryVersion(nativeDisplay, &major, &minor);
    if(major == 1 && minor < 4) {
        Error() << "GlxContext: GLX version 1.4 or greater is required.";
        exit(1);
    }

    /* Choose config */
    int configCount = 0;
    static const int attributes[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER, True,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        0
    };
    configs = glXChooseFBConfig(nativeDisplay, DefaultScreen(nativeDisplay), attributes, &configCount);
    if(!configCount) {
        Error() << "GlxContext: no supported framebuffer configuration found.";
        exit(1);
    }

    /* Get visual ID */
    XVisualInfo* info = glXGetVisualFromFBConfig(display, configs[0]);
    VisualID visualId = info->visualid;
    XFree(info);

    return visualId;
}

void GlxContext::createContext(Window nativeWindow) {
    window = nativeWindow;

    GLint attributes[] = {
        #ifndef MAGNUM_TARGET_GLES
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        #else
        GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
        #endif
        0
    };

    /** @todo Use some extension wrangler for this, not GLEW, as it apparently needs context to create context, yo dawg wtf. */
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
    context = glXCreateContextAttribsARB(display, configs[0], 0, True, attributes);
    XFree(configs);
    if(!context) {
        Error() << "GlxContext: cannot create context.";
        exit(1);
    }
}

GlxContext::~GlxContext() {
    glXMakeCurrent(display, None, nullptr);
    glXDestroyContext(display, context);
}

}}
