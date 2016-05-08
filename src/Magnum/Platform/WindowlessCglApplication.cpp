/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
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

#include "Magnum/Version.h"
#include "Magnum/Platform/Context.h"

namespace Magnum { namespace Platform {

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessCglApplication::WindowlessCglApplication(const Arguments& arguments): WindowlessCglApplication{arguments, Configuration{}} {}
#endif

WindowlessCglApplication::WindowlessCglApplication(const Arguments& arguments, const Configuration& configuration): WindowlessCglApplication{arguments, nullptr} {
    createContext(configuration);
}

WindowlessCglApplication::WindowlessCglApplication(const Arguments& arguments, std::nullptr_t): _context{new Context{NoCreate, arguments.argc, arguments.argv}} {}

void WindowlessCglApplication::createContext() { createContext({}); }

void WindowlessCglApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessCglApplication::tryCreateContext(const Configuration&) {
    CORRADE_ASSERT(_context->version() == Version::None, "Platform::WindowlessCglApplication::tryCreateContext(): context already created", false);

    int formatCount;
    CGLPixelFormatAttribute attributes32[] = {
        kCGLPFAAccelerated,
        kCGLPFAOpenGLProfile,
        CGLPixelFormatAttribute(kCGLOGLPVersion_3_2_Core),
        CGLPixelFormatAttribute(0)
    };
    if(CGLChoosePixelFormat(attributes32, &_pixelFormat, &formatCount) != kCGLNoError) {
        Error() << "Platform::WindowlessCglApplication::tryCreateContext(): cannot choose pixel format for GL 3.2, falling back to 3.0";

        CGLPixelFormatAttribute attributes30[] = {
            kCGLPFAAccelerated,
            kCGLPFAOpenGLProfile,
            CGLPixelFormatAttribute(kCGLOGLPVersion_GL3_Core),
            CGLPixelFormatAttribute(0)
        };
        if(CGLChoosePixelFormat(attributes30, &_pixelFormat, &formatCount) != kCGLNoError) {
            Error() << "Platform::WindowlessCglApplication::tryCreateContext(): cannot choose pixel format for GL 3.0, falling back to 2.1";

            CGLPixelFormatAttribute attributes21[] = {
                kCGLPFAAccelerated,
                kCGLPFAOpenGLProfile,
                CGLPixelFormatAttribute(kCGLOGLPVersion_Legacy),
                CGLPixelFormatAttribute(0)
            };
            if(CGLChoosePixelFormat(attributes21, &_pixelFormat, &formatCount) != kCGLNoError) {
                Error() << "Platform::WindowlessCglApplication::tryCreateContext(): cannot choose pixel format";
                return false;
            }
        }
    }

    if(CGLCreateContext(_pixelFormat, nullptr, &_glContext) != kCGLNoError) {
        Error() << "Platform::WindowlessCglApplication::tryCreateContext(): cannot create context";
        return false;
    }

    if(CGLSetCurrentContext(_glContext) != kCGLNoError) {
        Error() << "Platform::WindowlessCglApplication::tryCreateContext(): cannot make context current";
        return false;
    }

    /* Return true if the initialization succeeds */
    return _context->tryCreate();
}

WindowlessCglApplication::~WindowlessCglApplication() {
    _context.reset();

    CGLDestroyContext(_glContext);
    CGLDestroyPixelFormat(_pixelFormat);
}

}}
