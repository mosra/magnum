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

#include "WindowlessGlxApplication.h"

#include <Utility/Debug.h>

#include "Context.h"

#define None 0L // redef Xlib nonsense

namespace Magnum { namespace Platform {

WindowlessGlxApplication::WindowlessGlxApplication(int&, char**) {
    display = XOpenDisplay(nullptr);

    /* Check version */
    int major, minor;
    glXQueryVersion(display, &major, &minor);
    if(major == 1 && minor < 4) {
        Error() << "WindowlessGlxApplication: GLX version 1.4 or greater is required.";
        std::exit(1);
    }

    /* Choose config */
    int configCount = 0;
    static const int fbAttributes[] = { None };
    GLXFBConfig* configs = glXChooseFBConfig(display, DefaultScreen(display), fbAttributes, &configCount);
    if(!configCount) {
        Error() << "WindowlessGlxApplication: no supported framebuffer configuration found.";
        std::exit(1);
    }

    GLint contextAttributes[] = {
        #ifdef MAGNUM_TARGET_GLES
        GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
        #endif
        0
    };

    /** @todo Use some extension wrangler for this, not GLEW, as it apparently needs context to create context, yo dawg wtf. */
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
    context = glXCreateContextAttribsARB(display, configs[0], 0, True, contextAttributes);
    if(!context) {
        Error() << "WindowlessGlxApplication: cannot create context.";
        std::exit(1);
    }

    /* Create pbuffer */
    int pbufferAttributes[] = {
        GLX_PBUFFER_WIDTH,  32,
        GLX_PBUFFER_HEIGHT, 32,
        None
    };
    pbuffer = glXCreatePbuffer(display, configs[0], pbufferAttributes);

    XFree(configs);

    /* Set OpenGL context as current */
    if(!glXMakeContextCurrent(display, pbuffer, pbuffer, context)) {
        Error() << "WindowlessGlxApplication: cannot make context current";
        std::exit(1);
    }

    /* Initialize extension wrangler */
    ExtensionWrangler::initialize(ExtensionWrangler::ExperimentalFeatures::Enable);

    c = new Context;
}

WindowlessGlxApplication::~WindowlessGlxApplication() {
    glXMakeCurrent(display, None, nullptr);
    glXDestroyContext(display, context);
}

}}
