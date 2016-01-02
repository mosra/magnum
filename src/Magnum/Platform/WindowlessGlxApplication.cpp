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

#include "WindowlessGlxApplication.h"

#include <cstring>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Version.h"
#include "Magnum/Platform/Context.h"

/* Saner way to define the None Xlib macro (anyway, FUCK YOU XLIB) */
namespace { enum { None = 0L }; }

namespace Magnum { namespace Platform {

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessGlxApplication::WindowlessGlxApplication(const Arguments& arguments): WindowlessGlxApplication{arguments, Configuration{}}  {}
#endif

WindowlessGlxApplication::WindowlessGlxApplication(const Arguments& arguments, const Configuration& configuration): WindowlessGlxApplication{arguments, nullptr} {
    createContext(configuration);
}

WindowlessGlxApplication::WindowlessGlxApplication(const Arguments& arguments, std::nullptr_t): _context{new Context{NoCreate, arguments.argc, arguments.argv}} {}

void WindowlessGlxApplication::createContext() { createContext({}); }

void WindowlessGlxApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessGlxApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == Version::None, "Platform::WindowlessGlxApplication::tryCreateContext(): context already created", false);
    _display = XOpenDisplay(nullptr);

    /* Check version */
    int major, minor;
    glXQueryVersion(_display, &major, &minor);
    if(major == 1 && minor < 4) {
        Error() << "Platform::WindowlessGlxApplication::tryCreateContext(): GLX version 1.4 or greater is required";
        return false;
    }

    /* Choose config */
    int configCount = 0;
    constexpr static const int fbAttributes[] = { None };
    GLXFBConfig* configs = glXChooseFBConfig(_display, DefaultScreen(_display), fbAttributes, &configCount);
    if(!configCount) {
        Error() << "Platform::WindowlessGlxApplication::tryCreateContext(): no supported framebuffer configuration found";
        return false;
    }

    /* Create pbuffer */
    constexpr static const int pbufferAttributes[] = {
        GLX_PBUFFER_WIDTH,  32,
        GLX_PBUFFER_HEIGHT, 32,
        None
    };
    _pbuffer = glXCreatePbuffer(_display, configs[0], pbufferAttributes);

    const GLint contextAttributes[] = {
        #ifdef MAGNUM_TARGET_GLES
        #ifdef MAGNUM_TARGET_GLES3
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        #elif defined(MAGNUM_TARGET_GLES2)
        GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
        #else
        #error unsupported OpenGL ES version
        #endif
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
        GLX_CONTEXT_FLAGS_ARB, GLint(configuration.flags()),
        #else
        /* Similarly to what's done in Sdl2Application, try to request core
           context first */
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 1,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB|GLint(configuration.flags()),
        #endif
        0
    };

    const PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));
    _glContext = glXCreateContextAttribsARB(_display, configs[0], nullptr, True, contextAttributes);

    #ifndef MAGNUM_TARGET_GLES
    /* Fall back to (forward compatible) GL 2.1, if version is not
       user-specified and either core context creation fails or we are on
       binary NVidia/AMD drivers on Linux/Windows. Instead of creating forward-
       compatible context with highest available version, they force the
       version to the one specified, which is completely useless behavior. */
    #ifndef CORRADE_TARGET_APPLE
    constexpr static const char nvidiaVendorString[] = "NVIDIA Corporation";
    constexpr static const char amdVendorString[] = "ATI Technologies Inc.";
    const char* vendorString;
    #endif
    if(!_glContext
        #ifndef CORRADE_TARGET_APPLE
        /* We need to make the context current first, sorry about the UGLY code
           and HOPEFULLY THERE WON'T BE MORE WORKAROUNDS */
        || (glXMakeContextCurrent(_display, _pbuffer, _pbuffer, _glContext) &&
        (vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
        (std::strncmp(vendorString, nvidiaVendorString, sizeof(nvidiaVendorString)) == 0 ||
         std::strncmp(vendorString, amdVendorString, sizeof(amdVendorString)) == 0) &&
        !_context->isDriverWorkaroundDisabled("amd-nv-no-forward-compatible-core-context")))
        #endif
    ) {
        /* Don't print any warning when doing the NV workaround, because the
           bug will be there probably forever */
        if(!_glContext) Warning()
            << "Platform::WindowlessGlxApplication::tryCreateContext(): cannot create core context, falling back to compatibility context";
        else {
            glXMakeCurrent(_display, None, nullptr);
            glXDestroyContext(_display, _glContext);
        }

        const GLint fallbackContextAttributes[] = {
            GLX_CONTEXT_FLAGS_ARB, GLint(configuration.flags()),
            0
        };

        _glContext = glXCreateContextAttribsARB(_display, configs[0], nullptr, True, fallbackContextAttributes);
    }
    #endif

    XFree(configs);

    if(!_glContext) {
        Error() << "Platform::WindowlessGlxApplication::tryCreateContext(): cannot create context";
        return false;
    }

    /* Set OpenGL context as current */
    if(!glXMakeContextCurrent(_display, _pbuffer, _pbuffer, _glContext)) {
        Error() << "Platform::WindowlessGlxApplication::tryCreateContext(): cannot make context current";
        return false;
    }

    /* Return true if the initialization succeeds */
    return _context->tryCreate();
}

WindowlessGlxApplication::~WindowlessGlxApplication() {
    _context.reset();

    glXMakeCurrent(_display, None, nullptr);
    glXDestroyPbuffer(_display, _pbuffer);
    glXDestroyContext(_display, _glContext);
}

}}
