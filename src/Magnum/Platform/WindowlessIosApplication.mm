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

#include "WindowlessIosApplication.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"

#import "EAGL.h"

#if __has_feature(objc_arc)
#error no, we want to use manual memory management
#endif

namespace Magnum { namespace Platform {

WindowlessIosContext::WindowlessIosContext(const Configuration&, GLContext*) {
    if(!(_context = [[EAGLContext alloc]
        #ifdef MAGNUM_TARGET_GLES2
        initWithAPI:kEAGLRenderingAPIOpenGLES2
        #else
        initWithAPI:kEAGLRenderingAPIOpenGLES3
        #endif
        ]))
    {
        Error() << "Platform::WindowlessIosContext(): cannot create EAGL context";
    }
}

WindowlessIosContext::WindowlessIosContext(WindowlessIosContext&& other): _context{other._context} {
    other._context = {};
}

WindowlessIosContext::~WindowlessIosContext() {
    if(_context) [_context dealloc];
}

WindowlessIosContext& WindowlessIosContext::operator=(WindowlessIosContext&& other) {
    using std::swap;
    swap(other._context, _context);
    return *this;
}

bool WindowlessIosContext::makeCurrent() {
    if([EAGLContext setCurrentContext:_context])
        return true;

    Error() << "Platform::WindowlessIosContext::makeCurrent(): cannot make context current";
    return false;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessIosApplication::WindowlessIosApplication(const Arguments& arguments): WindowlessIosApplication{arguments, Configuration{}} {}
#endif

WindowlessIosApplication::WindowlessIosApplication(const Arguments& arguments, const Configuration& configuration): WindowlessIosApplication{arguments, NoCreate} {
    createContext(configuration);
}

WindowlessIosApplication::WindowlessIosApplication(const Arguments& arguments, NoCreateT): _glContext{NoCreate}, _context{new GLContext{NoCreate, arguments.argc, arguments.argv}} {}

void WindowlessIosApplication::createContext() { createContext({}); }

void WindowlessIosApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessIosApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == GL::Version::None, "Platform::WindowlessIosApplication::tryCreateContext(): context already created", false);

    WindowlessIosContext glContext{configuration, _context.get()};
    if(!glContext.isCreated() || !glContext.makeCurrent() || !_context->tryCreate())
        return false;

    _glContext = std::move(glContext);
    return true;
}

WindowlessIosApplication::~WindowlessIosApplication() = default;

}}
