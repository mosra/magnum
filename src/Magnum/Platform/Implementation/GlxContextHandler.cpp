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

#include "GlxContextHandler.h"

#include <cstdlib>
#include <tuple>
#include <GL/glxext.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Version.h"

namespace Magnum { namespace Platform { namespace Implementation {

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

void GlxContextHandler::createContext(const AbstractXApplication::GLConfiguration& glConfiguration, Window nativeWindow) {
    window = nativeWindow;

    GLint attributes[] = {
        /* Leave some space for optional attributes below */
        0, 0,
        0, 0,
        0, 0,

        0
    };

    /* Set context version, if requested */
    if(glConfiguration.version() != GL::Version::None) {
        Int major, minor;
        std::tie(major, minor) = version(glConfiguration.version());

        attributes[0] = GLX_CONTEXT_MAJOR_VERSION_ARB;
        attributes[1] = major;
        attributes[2] = GLX_CONTEXT_MINOR_VERSION_ARB;
        attributes[3] = minor;

        #ifndef MAGNUM_TARGET_GLES
        if(glConfiguration.version() >= GL::Version::GL310) {
            attributes[4] = GLX_CONTEXT_PROFILE_MASK_ARB;
            attributes[5] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
        }
        #else
        attributes[4] = GLX_CONTEXT_PROFILE_MASK_ARB;
        attributes[5] = GLX_CONTEXT_ES2_PROFILE_BIT_EXT;
        #endif
    }

    /* We need this to run ES (default is desktop GL) */
    #ifdef MAGNUM_TARGET_GLES
    else {
        attributes[0] = GLX_CONTEXT_MAJOR_VERSION_ARB;
        #ifdef MAGNUM_TARGET_GLES3
        attributes[1] = 3;
        #elif defined(MAGNUM_TARGET_GLES2)
        attributes[1] = 2;
        #else
        #error unsupported OpenGL ES version
        #endif
        attributes[2] = GLX_CONTEXT_MINOR_VERSION_ARB;
        attributes[3] = 0;
        attributes[4] = GLX_CONTEXT_PROFILE_MASK_ARB;
        attributes[5] = GLX_CONTEXT_ES2_PROFILE_BIT_EXT;
    }
    #endif

    /** @todo Use some extension wrangler for this, not GLEW, as it apparently needs context to create context, yo dawg wtf. */
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
    context = glXCreateContextAttribsARB(display, configs[0], nullptr, True, attributes);
    XFree(configs);
    if(!context) {
        Error() << "GlxContextHandler: cannot create context.";
        std::exit(1);
    }
}

GlxContextHandler::~GlxContextHandler() {
    glXMakeCurrent(display, 0, nullptr);
    glXDestroyContext(display, context);
}

}}}
