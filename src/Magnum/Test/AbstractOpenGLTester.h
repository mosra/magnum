#ifndef Magnum_Test_AbstractOpenGLTester_h
#define Magnum_Test_AbstractOpenGLTester_h
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

#include "Magnum/OpenGLTester.h"
#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_DEPRECATED_FILE("use Magnum/OpenGLTester.h and Magnum::OpenGLTester library instead")

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/DebugOutput.h"

namespace Magnum { namespace Test {

class AbstractOpenGLTester: public TestSuite::Tester {
    public:
        /* Deprecating the class doesn't make GCC print the warnings :( */
        explicit CORRADE_DEPRECATED("use OpenGLTester instead") AbstractOpenGLTester();

    private:
        struct WindowlessApplication: Platform::WindowlessApplication {
            explicit WindowlessApplication(const Arguments& arguments): Platform::WindowlessApplication{arguments, NoCreate} {}
            int exec() override final { return 0; }

            using Platform::WindowlessApplication::tryCreateContext;
            using Platform::WindowlessApplication::createContext;

        } _windowlessApplication;
};

AbstractOpenGLTester::AbstractOpenGLTester(): TestSuite::Tester{TestSuite::Tester::TesterConfiguration{}.setSkippedArgumentPrefixes({"magnum"})}, _windowlessApplication{{arguments().first, arguments().second}} {
    /* Try to create debug context, fallback to normal one if not possible. No
       such thing on macOS or iOS. */
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

#define MAGNUM_GL_TEST_MAIN(Class) CORRADE_TEST_MAIN(Class)
#else
#error use Magnum/OpenGLTester.h and Magnum::OpenGLTester library instead
#endif

}}

#endif
