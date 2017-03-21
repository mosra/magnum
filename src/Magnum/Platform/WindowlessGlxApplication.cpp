/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

WindowlessGlxContext::WindowlessGlxContext(const WindowlessGlxContext::Configuration& configuration, Context* const magnumContext) {
    _display = XOpenDisplay(nullptr);

    /* Check version */
    int major, minor;
    glXQueryVersion(_display, &major, &minor);
    if(major == 1 && minor < 4) {
        Error() << "Platform::WindowlessGlxContext: GLX version 1.4 or greater is required";
        return;
    }

    /* Choose config */
    int configCount = 0;
    constexpr static const int fbAttributes[] = { None };
    GLXFBConfig* configs = glXChooseFBConfig(_display, DefaultScreen(_display), fbAttributes, &configCount);
    if(!configCount) {
        Error() << "Platform::WindowlessGlxContext: no supported framebuffer configuration found";
        return;
    }

    /* Create pbuffer */
    constexpr static const int pbufferAttributes[] = {
        GLX_PBUFFER_WIDTH,  32,
        GLX_PBUFFER_HEIGHT, 32,
        None
    };
    _pbuffer = glXCreatePbuffer(_display, configs[0], pbufferAttributes);

    /* Get pointer to proper context creation function */
    const PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));

    /* Optimistically choose core context first */
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
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 1,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB|GLint(configuration.flags()),
        #endif
        0
    };
    _context = glXCreateContextAttribsARB(_display, configs[0], nullptr, True, contextAttributes);

    #ifndef MAGNUM_TARGET_GLES
    /* Fall back to (forward compatible) GL 2.1 if core context creation fails */
    if(!_context) {
        Warning() << "Platform::WindowlessGlxContext: cannot create core context, falling back to compatibility context";

        const GLint fallbackContextAttributes[] = {
            GLX_CONTEXT_FLAGS_ARB, GLint(configuration.flags()),
            0
        };
        _context = glXCreateContextAttribsARB(_display, configs[0], nullptr, True, fallbackContextAttributes);

    /* Fall back to (forward compatible) GL 2.1 if we are on binary NVidia/AMD
       drivers on Linux. Instead of creating forward-compatible context with
       highest available version, they force the version to the one specified,
       which is completely useless behavior. */
    } else {
        /* We need to make the context current to read out vendor string, so
           save the previous values so we can safely revert back without
           messing up the state */
        GLXDrawable currentDrawable = glXGetCurrentDrawable();
        GLXDrawable currentReadDrawable = glXGetCurrentReadDrawable();
        GLXContext currentContext = glXGetCurrentContext();
        if(!glXMakeContextCurrent(_display, _pbuffer, _pbuffer, _context)) {
            Error() << "Platform::WindowlessGlxContext: cannot make context current";
            return;
        }

        /* The workaround check is the last so it doesn't appear in workaround
           list on unrelated drivers */
        constexpr static const char nvidiaVendorString[] = "NVIDIA Corporation";
        constexpr static const char amdVendorString[] = "ATI Technologies Inc.";
        const char* const vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        if((std::strncmp(vendorString, nvidiaVendorString, sizeof(nvidiaVendorString)) == 0 ||
            std::strncmp(vendorString, amdVendorString, sizeof(amdVendorString)) == 0) &&
            (!magnumContext || !magnumContext->isDriverWorkaroundDisabled("no-forward-compatible-core-context")))
        {
            /* Destroy the core context and create a compatibility one */
            glXDestroyContext(_display, _context);
            const GLint fallbackContextAttributes[] = {
                GLX_CONTEXT_FLAGS_ARB, GLint(configuration.flags()),
                0
            };
            _context = glXCreateContextAttribsARB(_display, configs[0], nullptr, True, fallbackContextAttributes);
        }

        /* Revert back the old context */
        if(!glXMakeContextCurrent(_display, currentDrawable, currentReadDrawable, currentContext)) {
            Error() << "Platform::WindowlessGlxContext: cannot make the previous context current";
            return;
        }
    }
    #else
    static_cast<void>(magnumContext);
    #endif

    XFree(configs);

    if(!_context)
        Error() << "Platform::WindowlessGlxContext: cannot create context";
}

WindowlessGlxContext::WindowlessGlxContext(WindowlessGlxContext&& other): _display{other._display}, _pbuffer{other._pbuffer}, _context{other._context} {
    other._display = {};
    other._context = {};
    other._pbuffer = {};
}

WindowlessGlxContext::~WindowlessGlxContext() {
    if(_context) glXDestroyContext(_display, _context);
    if(_pbuffer) glXDestroyPbuffer(_display, _pbuffer);
    if(_display) XCloseDisplay(_display);
}

WindowlessGlxContext& WindowlessGlxContext::operator=(WindowlessGlxContext&& other) {
    using std::swap;
    swap(other._display, _display);
    swap(other._pbuffer, _pbuffer);
    swap(other._context, _context);
    return *this;
}

bool WindowlessGlxContext::makeCurrent() {
    if(glXMakeContextCurrent(_display, _pbuffer, _pbuffer, _context))
        return true;

    Error() << "Platform::WindowlessGlxContext::makeCurrent(): cannot make context current";
    return false;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessGlxApplication::WindowlessGlxApplication(const Arguments& arguments): WindowlessGlxApplication{arguments, Configuration{}}  {}
#endif

WindowlessGlxApplication::WindowlessGlxApplication(const Arguments& arguments, const Configuration& configuration): WindowlessGlxApplication{arguments, NoCreate} {
    createContext(configuration);
}

WindowlessGlxApplication::WindowlessGlxApplication(const Arguments& arguments, NoCreateT): _glContext{NoCreate}, _context{new Context{NoCreate, arguments.argc, arguments.argv}} {}

void WindowlessGlxApplication::createContext() { createContext({}); }

void WindowlessGlxApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessGlxApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == Version::None, "Platform::WindowlessGlxApplication::tryCreateContext(): context already created", false);

    WindowlessGlxContext glContext{configuration, _context.get()};
    if(!glContext.isCreated() || !glContext.makeCurrent() || !_context->tryCreate())
        return false;

    _glContext = std::move(glContext);
    return true;
}

WindowlessGlxApplication::~WindowlessGlxApplication() = default;

}}
