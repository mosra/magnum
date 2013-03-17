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

#include "GlxContextHandler.h"

#include <GL/glxext.h>
#include <Utility/Debug.h>

#include "Context.h"

#define None 0L // redef Xlib nonsense

namespace Magnum { namespace Platform {

VisualID GlxContextHandler::getVisualId(Display* nativeDisplay) {
    display = nativeDisplay;

    /* Check version */
    int major, minor;
    glXQueryVersion(nativeDisplay, &major, &minor);
    if(major == 1 && minor < 4) {
        Error() << "GlxContextHandler: GLX version 1.4 or greater is required.";
        std::exit(1);
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
        Error() << "GlxContextHandler: no supported framebuffer configuration found.";
        std::exit(1);
    }

    /* Get visual ID */
    XVisualInfo* info = glXGetVisualFromFBConfig(display, configs[0]);
    VisualID visualId = info->visualid;
    XFree(info);

    return visualId;
}

void GlxContextHandler::createContext(Window nativeWindow) {
    window = nativeWindow;

    GLint attributes[] = {
        #ifdef MAGNUM_TARGET_GLES
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
        Error() << "GlxContextHandler: cannot create context.";
        std::exit(1);
    }
}

GlxContextHandler::~GlxContextHandler() {
    glXMakeCurrent(display, None, nullptr);
    glXDestroyContext(display, context);
}

}}
