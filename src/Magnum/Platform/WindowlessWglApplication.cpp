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

#include "WindowlessWglApplication.h"

#include <cstring>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Define stuff that we need because I can't be bothered with creating a new
   header just for a few defines */
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
#endif

namespace Magnum { namespace Platform {

WindowlessWglContext::WindowlessWglContext(const Configuration& configuration, GLContext* const magnumContext) {
    /* Register the window class (if not yet done) */
    WNDCLASSW wc;
    if(!GetClassInfoW(GetModuleHandleW(nullptr), L"Magnum Windowless Application", &wc)) {
        wc = WNDCLASSW{
            0,
            DefWindowProcW,
            0,
            0,
            GetModuleHandleW(nullptr),
            nullptr,
            nullptr,
            HBRUSH(COLOR_BACKGROUND),
            nullptr,
            L"Magnum Windowless Application"
        };

        if(!RegisterClassW(&wc)) {
            Error() << "Platform::WindowlessWglContext: cannot create window class:" << GetLastError();
            return;
        }
    }

    /* Create the window */
    _window = CreateWindowW(wc.lpszClassName, L"Magnum Windowless Application",
        WS_OVERLAPPEDWINDOW, 0, 0, 32, 32, 0, 0, wc.hInstance, 0);

    /* Get device context from the newly created window and save the previous
       one. In case the previous one is null, wglMakeCurrent(null, ...) would
       fail and thus we need to pass at least something there. As a commenter
       on https://github.com/glfw/glfw/issues/245#issuecomment-43475120 said:
       the Windows API is horrible. HORRIBLE. */
    HDC currentDeviceContext = wglGetCurrentDC();
    _deviceContext = GetDC(_window);
    if(!currentDeviceContext) currentDeviceContext = _deviceContext;

    /* Use first provided pixel format  */
    constexpr static const PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        /* Double-buffered with OpenGL support */
        PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,          /* RGBA */
        32,                     /* 32 bit */
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                     /* 24-bit depth buffer */
        8,                      /* 8-bit stencil buffer */
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    const int pixelFormat = ChoosePixelFormat(_deviceContext, &pfd);
    SetPixelFormat(_deviceContext, pixelFormat, &pfd);

    /* Create temporary context so we are able to get the pointer to
       wglCreateContextAttribsARB(). To avoid messing up the app state we need
       to save the old active context and then restore it later. */
    const HGLRC currentContext = wglGetCurrentContext();
    const HGLRC temporaryContext = wglCreateContext(_deviceContext);
    if(!wglMakeCurrent(_deviceContext, temporaryContext)) {
        Error() << "Platform::WindowlessWglContext: cannot make temporary context current:" << GetLastError();
        wglDeleteContext(temporaryContext);
        return;
    }

    /* Get pointer to proper context creation function */
    typedef HGLRC(WINAPI*PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
    const PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>( wglGetProcAddress(reinterpret_cast<LPCSTR>("wglCreateContextAttribsARB")));

    /* Request debug context if --magnum-gpu-validation is enabled */
    Configuration::Flags flags = configuration.flags();
    if(magnumContext && magnumContext->internalFlags() & GL::Context::InternalFlag::GpuValidation)
        flags |= Configuration::Flag::Debug;

    /* Optimistically choose core context first */
    const GLint contextAttributes[] = {
        #ifdef MAGNUM_TARGET_GLES
        #ifdef MAGNUM_TARGET_GLES3
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        #elif defined(MAGNUM_TARGET_GLES2)
        WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
        #else
        #error unsupported OpenGL ES version
        #endif
        WGL_CONTEXT_MINOR_VERSION_ARB, 0,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_ES2_PROFILE_BIT_EXT,
        WGL_CONTEXT_FLAGS_ARB, GLint(flags),
        #else
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, GLint(flags),
        #endif
        0
    };
    _context = wglCreateContextAttribsARB(_deviceContext, configuration.sharedContext(), contextAttributes);

    #ifndef MAGNUM_TARGET_GLES
    /* Fall back to (forward compatible) GL 2.1 if core context creation fails */
    if(!_context) {
        Warning() << "Platform::WindowlessWglContext: cannot create core context, falling back to compatibility context:" << GetLastError();

        const int fallbackContextAttributes[] = {
            /** @todo or keep the fwcompat? */
            WGL_CONTEXT_FLAGS_ARB, GLint(flags & ~Configuration::Flag::ForwardCompatible),
            0
        };
        _context = wglCreateContextAttribsARB(_deviceContext, configuration.sharedContext(), fallbackContextAttributes);

    /* Fall back to (forward compatible) GL 2.1 if we are on binary
       NVidia/AMD/Intel drivers on Windows. Instead of creating forward-compatible
       context with highest available version, they force the version to the
       one specified, which is completely useless behavior. */
    } else {
        /* We need to make the context current to read out vendor string */
        if(!wglMakeCurrent(_deviceContext, _context)) {
            Error() << "Platform::WindowlessWglContext: cannot make context current:" << GetLastError();

            /* Everything failed, at least try to delete the dangling contexts
               and revert to the previous context to regain some sanity */
            wglMakeCurrent(_deviceContext, currentContext);
            wglDeleteContext(temporaryContext);
            return;
        }

        /* The workaround check is the last so it doesn't appear in workaround
           list on unrelated drivers */
        constexpr static const char nvidiaVendorString[] = "NVIDIA Corporation";
        constexpr static const char intelVendorString[] = "Intel";
        constexpr static const char amdVendorString[] = "ATI Technologies Inc.";
        const char* const vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        /* If context creation fails *really bad*, glGetString() may actually
           return nullptr. Check for that to avoid crashes deep inside
           strncmp() */
        if(vendorString && (std::strncmp(vendorString, nvidiaVendorString, sizeof(nvidiaVendorString)) == 0 ||
            std::strncmp(vendorString, intelVendorString, sizeof(intelVendorString)) == 0 ||
            std::strncmp(vendorString, amdVendorString, sizeof(amdVendorString)) == 0) &&
            (!magnumContext || !magnumContext->isDriverWorkaroundDisabled("no-forward-compatible-core-context")))
        {
            /* Destroy the core context and create a compatibility one */
            wglDeleteContext(_context);
            const int fallbackContextAttributes[] = {
                /** @todo or keep the fwcompat? */
                WGL_CONTEXT_FLAGS_ARB, GLint(flags & ~Configuration::Flag::ForwardCompatible),
                0
            };
            _context = wglCreateContextAttribsARB(_deviceContext, configuration.sharedContext(), fallbackContextAttributes);
        }
    }
    #endif

    /* Make the previous context active and delete the temporary context */
    if(!wglMakeCurrent(currentDeviceContext, currentContext)) {
        Error() << "Platform::WindowlessWglContext: cannot make the previous context current:" << GetLastError();

        /* Everything is fucked up, but try to delete the temporary context
           anyway */
        wglDeleteContext(temporaryContext);
        return;
    }
    wglDeleteContext(temporaryContext);

    if(!_context)
        Error() << "Platform::WindowlessWglContext: cannot create context:" << GetLastError();
}

WindowlessWglContext::WindowlessWglContext(WindowlessWglContext&& other): _window{other._window}, _deviceContext{other._deviceContext}, _context{other._context} {
    other._window = {};
    other._deviceContext = {};
    other._context = {};
}

WindowlessWglContext::~WindowlessWglContext() {
    if(_context) wglDeleteContext(_context);
    if(_window) DestroyWindow(_window);
}

WindowlessWglContext& WindowlessWglContext::operator=(WindowlessWglContext&& other) {
    using std::swap;
    swap(other._window, _window);
    swap(other._deviceContext, _deviceContext);
    swap(other._context, _context);
    return *this;
}

bool WindowlessWglContext::makeCurrent() {
    if(wglMakeCurrent(_deviceContext, _context))
        return true;

    Error() << "Platform::WindowlessWglContext::makeCurrent(): cannot make context current:" << GetLastError();
    return false;
}

WindowlessWglContext::Configuration::Configuration():
    #ifndef MAGNUM_TARGET_GLES
    _flags{Flag::ForwardCompatible}
    #else
    _flags{}
    #endif
    {}

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessWglApplication::WindowlessWglApplication(const Arguments& arguments): WindowlessWglApplication{arguments, Configuration{}} {}
#endif

WindowlessWglApplication::WindowlessWglApplication(const Arguments& arguments, const Configuration& configuration): WindowlessWglApplication{arguments, NoCreate} {
    createContext(configuration);
}

WindowlessWglApplication::WindowlessWglApplication(const Arguments& arguments, NoCreateT): _glContext{NoCreate}, _context{new GLContext{NoCreate, arguments.argc, arguments.argv}} {}

void WindowlessWglApplication::createContext() { createContext({}); }

void WindowlessWglApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessWglApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == GL::Version::None, "Platform::WindowlessWglApplication::tryCreateContext(): context already created", false);

    WindowlessWglContext glContext{configuration, _context.get()};
    if(!glContext.isCreated() || !glContext.makeCurrent() || !_context->tryCreate())
        return false;

    _glContext = std::move(glContext);
    return true;
}

WindowlessWglApplication::~WindowlessWglApplication() = default;

}}
