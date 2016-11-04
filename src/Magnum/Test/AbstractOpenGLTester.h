#ifndef Magnum_Test_AbstractOpenGLTester_h
#define Magnum_Test_AbstractOpenGLTester_h
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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/DebugOutput.h"
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

namespace Magnum { namespace Test {

class AbstractOpenGLTester: public TestSuite::Tester {
    public:
        explicit AbstractOpenGLTester();

        /* Private use only */
        static std::optional<Platform::WindowlessApplication::Arguments> _windowlessApplicationArguments;

    private:
        struct WindowlessApplication: Platform::WindowlessApplication {
            explicit WindowlessApplication(const Arguments& arguments): Platform::WindowlessApplication{arguments, NoCreate} {}
            int exec() override final { return 0; }

            using Platform::WindowlessApplication::tryCreateContext;
            using Platform::WindowlessApplication::createContext;

        } _windowlessApplication;
};

AbstractOpenGLTester::AbstractOpenGLTester(): TestSuite::Tester{TestSuite::Tester::TesterConfiguration{}.setSkippedArgumentPrefixes({"magnum"})}, _windowlessApplication{*_windowlessApplicationArguments} {
    /* Try to create debug context, fallback to normal one if not possible. No
       such thing on OSX or iOS. */
    #ifndef CORRADE_TARGET_APPLE
    if(!_windowlessApplication.tryCreateContext(Platform::WindowlessApplication::Configuration{}.setFlags(Platform::WindowlessApplication::Configuration::Flag::Debug)))
        _windowlessApplication.createContext();
    #else
    _windowlessApplication.createContext();
    #endif

    if(Context::current().isExtensionSupported<Extensions::GL::KHR::debug>()) {
        Renderer::enable(Renderer::Feature::DebugOutput);
        Renderer::enable(Renderer::Feature::DebugOutputSynchronous);
        DebugOutput::setDefaultCallback();

        /* Disable "Buffer detailed info" message on NV (too spammy) */
        DebugOutput::setEnabled(DebugOutput::Source::Api, DebugOutput::Type::Other, {131185}, false);
    }
}

std::optional<Platform::WindowlessApplication::Arguments> AbstractOpenGLTester::_windowlessApplicationArguments;

#define MAGNUM_VERIFY_NO_ERROR() CORRADE_COMPARE(Magnum::Renderer::error(), Magnum::Renderer::Error::NoError)

#ifdef CORRADE_TESTSUITE_TARGET_XCTEST
#define MAGNUM_GL_TEST_MAIN(Class)                                          \
    int CORRADE_VISIBILITY_EXPORT corradeTestMain(int argc, char** argv) {  \
        Magnum::Test::AbstractOpenGLTester::_windowlessApplicationArguments.emplace(argc, argv); \
        Class t;                                                            \
        t.registerTest(__FILE__, #Class);                                   \
        return t.exec(argc, argv);                                          \
    }
#else
#define MAGNUM_GL_TEST_MAIN(Class)                                          \
    int main(int argc, char** argv) {                                       \
        Magnum::Test::AbstractOpenGLTester::_windowlessApplicationArguments.emplace(argc, argv); \
        Class t;                                                            \
        t.registerTest(__FILE__, #Class);                                   \
        return t.exec(argc, argv);                                          \
    }
#endif

}}

#endif
