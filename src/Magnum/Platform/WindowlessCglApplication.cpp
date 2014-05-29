/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "Magnum/Context.h"

namespace Magnum { namespace Platform {

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessCglApplication::WindowlessCglApplication(const Arguments& arguments): WindowlessCglApplication(arguments, Configuration{}) {}
#endif

WindowlessCglApplication::WindowlessCglApplication(const Arguments& arguments, const Configuration& configuration): WindowlessCglApplication(arguments, nullptr) {
    createContext(configuration);
}

WindowlessCglApplication::WindowlessCglApplication(const Arguments&, std::nullptr_t): c(nullptr) {}

void WindowlessCglApplication::createContext() { createContext({}); }

void WindowlessCglApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessCglApplication::tryCreateContext(const Configuration&) {
    CORRADE_ASSERT(!c, "Platform::WindowlessCglApplication::tryCreateContext(): context already created", false);

    /* Check version */
    int nPix = 0;
    GLint major, minor;
    CGLGetVersion(&major, &minor);
    CGLError cglError;

    if(major == 2 && minor < 1) {
        Error() << "Platform::WindowlessCglApplication::tryCreateContext(): OpenGL version 2.1 or greater is required";
        return false;
    }


    CGLPixelFormatAttribute pfAttributesGL_3_2[4] = {
        kCGLPFAAccelerated,
        kCGLPFAOpenGLProfile,
        (CGLPixelFormatAttribute) kCGLOGLPVersion_3_2_Core,
        (CGLPixelFormatAttribute) 0
    };

    cglError = CGLChoosePixelFormat(pfAttributesGL_3_2,&pixelFormat,&nPix);

    if(cglError == kCGLBadPixelFormat) {
        Error() << "Platform::WindowlessCglApplication::tryCreateContext(): OpenGL version 3.2 has failed trying GL version 3.0";

        CGLPixelFormatAttribute pfAttributesGL3[4] = {
            kCGLPFAAccelerated,
            kCGLPFAOpenGLProfile,
            (CGLPixelFormatAttribute) kCGLOGLPVersion_GL3_Core,
            (CGLPixelFormatAttribute) 0
        };

        cglError = CGLChoosePixelFormat(pfAttributesGL3,&pixelFormat,&nPix);

        if(cglError == kCGLBadPixelFormat) {
            Error() << "Platform::WindowlessCglApplication::tryCreateContext(): OpenGL version 3.0 has failed trying GL version Legacy";

            CGLPixelFormatAttribute pfAttributesLegacy[4] = {
                kCGLPFAAccelerated,
                kCGLPFAOpenGLProfile,
                (CGLPixelFormatAttribute) kCGLOGLPVersion_Legacy,
                (CGLPixelFormatAttribute) 0
            };

            cglError = CGLChoosePixelFormat(pfAttributesLegacy,&pixelFormat,&nPix);

            if(cglError == kCGLBadPixelFormat)
            {
                Error() << "Platform::WindowlessCglApplication::tryCreateContext(): Context could not be created";
                return false;
            }
        }
    }

    cglError = CGLCreateContext(pixelFormat, NULL, &context);
    if(cglError == kCGLBadContext) {
        Error() << "Platform::WindowlessCglApplication::tryCreateContext(): cannot create context";
        return false;
    }

    cglError = CGLSetCurrentContext(context);
    c = new Context;
    return true;
}

WindowlessCglApplication::~WindowlessCglApplication() {
    delete c;

    CGLDestroyContext(context);
    CGLDestroyPixelFormat(pixelFormat);
}

}}
