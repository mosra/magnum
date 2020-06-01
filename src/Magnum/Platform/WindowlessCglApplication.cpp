/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2013 <https://github.com/ArEnSc>
    Copyright © 2014 Travis Watkins <https://github.com/amaranth>

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

#include "WindowlessCglApplication.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"

namespace Magnum { namespace Platform {

WindowlessCglContext::WindowlessCglContext(const Configuration & configuration, GLContext*) {
    int formatCount;
    CGLPixelFormatAttribute attributes32[] = {
        kCGLPFAAccelerated,
        kCGLPFAOpenGLProfile,
        CGLPixelFormatAttribute(kCGLOGLPVersion_3_2_Core),
        CGLPixelFormatAttribute(0)
    };
    if(CGLChoosePixelFormat(attributes32, &_pixelFormat, &formatCount) != kCGLNoError) {
        Warning() << "Platform::WindowlessCglContext: cannot choose pixel format for GL 3.2, falling back to 3.0";

        CGLPixelFormatAttribute attributes30[] = {
            kCGLPFAAccelerated,
            kCGLPFAOpenGLProfile,
            CGLPixelFormatAttribute(kCGLOGLPVersion_GL3_Core),
            CGLPixelFormatAttribute(0)
        };
        if(CGLChoosePixelFormat(attributes30, &_pixelFormat, &formatCount) != kCGLNoError) {
            Warning() << "Platform::WindowlessCglContext: cannot choose pixel format for GL 3.0, falling back to 2.1";

            CGLPixelFormatAttribute attributes21[] = {
                kCGLPFAAccelerated,
                kCGLPFAOpenGLProfile,
                CGLPixelFormatAttribute(kCGLOGLPVersion_Legacy),
                CGLPixelFormatAttribute(0)
            };
            if(CGLChoosePixelFormat(attributes21, &_pixelFormat, &formatCount) != kCGLNoError) {
                Error() << "Platform::WindowlessCglContext: cannot choose pixel format";
                return;
            }
        }
    }

    if(CGLCreateContext(_pixelFormat, configuration.sharedContext(), &_context) != kCGLNoError)
        Error() << "Platform::WindowlessCglContext: cannot create context";
}

WindowlessCglContext::WindowlessCglContext(WindowlessCglContext&& other): _pixelFormat{other._pixelFormat}, _context{other._context} {
    other._pixelFormat = {};
    other._context = {};
}

WindowlessCglContext::~WindowlessCglContext() {
    if(_context) CGLDestroyContext(_context);
    if(_pixelFormat) CGLDestroyPixelFormat(_pixelFormat);
}

WindowlessCglContext& WindowlessCglContext::operator=(WindowlessCglContext&& other) {
    using std::swap;
    swap(other._pixelFormat, _pixelFormat);
    swap(other._context, _context);
    return *this;
}

bool WindowlessCglContext::makeCurrent() {
    if(CGLSetCurrentContext(_context) == kCGLNoError)
        return true;

    Error() << "Platform::WindowlessCglContext::makeCurrent(): cannot make context current";
    return false;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessCglApplication::WindowlessCglApplication(const Arguments& arguments): WindowlessCglApplication{arguments, Configuration{}} {}
#endif

WindowlessCglApplication::WindowlessCglApplication(const Arguments& arguments, const Configuration& configuration): WindowlessCglApplication{arguments, NoCreate} {
    createContext(configuration);
}

WindowlessCglApplication::WindowlessCglApplication(const Arguments& arguments, NoCreateT): _glContext{NoCreate}, _context{new GLContext{NoCreate, arguments.argc, arguments.argv}} {}

WindowlessCglApplication::~WindowlessCglApplication() = default;

void WindowlessCglApplication::createContext() { createContext({}); }

void WindowlessCglApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessCglApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == GL::Version::None, "Platform::WindowlessCglApplication::tryCreateContext(): context already created", false);

    WindowlessCglContext glContext{configuration, _context.get()};
    if(!glContext.isCreated() || !glContext.makeCurrent() || !_context->tryCreate())
        return false;

    _glContext = std::move(glContext);
    return true;
}

}}
