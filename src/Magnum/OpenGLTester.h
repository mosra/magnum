#ifndef Magnum_OpenGLTester_h
#define Magnum_OpenGLTester_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

/** @file
 * @brief Class @ref Magnum::OpenGLTester
 */

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Renderer.h"

#ifdef MAGNUM_TARGET_HEADLESS
#include "Magnum/Platform/WindowlessEglApplication.h"
#elif defined(CORRADE_TARGET_IOS)
#include "Magnum/Platform/WindowlessIosApplication.h"
#elif defined(CORRADE_TARGET_APPLE)
#include "Magnum/Platform/WindowlessCglApplication.h"
#elif defined(CORRADE_TARGET_UNIX)
#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessEglApplication.h"
#else
#include "Magnum/Platform/WindowlessGlxApplication.h"
#endif
#elif defined(CORRADE_TARGET_WINDOWS)
#if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessWglApplication.h"
#else
#include "Magnum/Platform/WindowlessWindowsEglApplication.h"
#endif
#else
#error cannot run OpenGL tests on this platform
#endif

namespace Magnum {

/**
@brief Base class for OpenGL tests and benchmarks

Extends @ref Corrade::TestSuite::Tester with features for OpenGL testing and
benchmarking. Be sure to read its documentation first to have an overview of
the base features.

This class is available only on platforms with corresponding
`Platform::Windowless*Application` implementation. That currently means all
platforms except @ref CORRADE_TARGET_ANDROID "Android",
@ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" and @ref CORRADE_TARGET_NACL "NaCl".
It is built into a separate static library and only if `WITH_OPENGLTESTER` is
enabled when building Magnum. To use it, you need to request `OpenGLTester`
component of `Magnum` package in CMake, derive your test class from this class
instead of @ref Corrade::TestSuite::Tester and either link to
`Magnum::OpenGLTester` target or add it to `LIBRARIES` section of the
@ref corrade-cmake-add-test "corrade_add_test()" macro. See @ref building and
@ref cmake for more information.

## OpenGL context creation

Upon construction the class creates an OpenGL context, meaning you don't have
to worry about OpenGL context being available during the tests. If the context
creation fails, the test executable exits with non-zero return code. The tester
uses a single OpenGL context for all test cases, meaning you can share
precalculated state among test cases, but on the other hand potential OpenGL
misuses will propagate to following test cases. See
@ref TestSuite-Tester-command-line "command-line option overview" for a way
to run single isolated test cases.

## Debug context and error checking

On platforms that support it, the OpenGL context is created with synchronous
debug output, meaning that every OpenGL error is directly reported to standard
output. While it is possible, the tester class doesn't abort the test cases
upon encountering a GL error -- this should be done explicitly with
@ref MAGNUM_VERIFY_NO_ERROR() instead, as the debug output is not available on
all platforms and not all GL errors are fatal.
*/
class OpenGLTester: public TestSuite::Tester {
    public:
        /**
         * @brief Constructor
         *
         * Creates an OpenGL context.
         */
        explicit OpenGLTester();

        ~OpenGLTester();

    private:
        struct WindowlessApplication: Platform::WindowlessApplication {
            explicit WindowlessApplication(const Arguments& arguments): Platform::WindowlessApplication{arguments, NoCreate} {}
            int exec() override final { return 0; }

            using Platform::WindowlessApplication::tryCreateContext;
            using Platform::WindowlessApplication::createContext;

        } _windowlessApplication;
};

/** @hideinitializer
@relatesalso Magnum::OpenGLTester
@brief Verify that no OpenGL error occurred

Equivalent to
@code
CORRADE_COMPARE(Magnum::Renderer::error(), Magnum::Renderer::Error::NoError)
@endcode
*/
#define MAGNUM_VERIFY_NO_ERROR() CORRADE_COMPARE(Magnum::Renderer::error(), Magnum::Renderer::Error::NoError)

}

#endif
