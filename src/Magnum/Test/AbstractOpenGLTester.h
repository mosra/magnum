#ifndef Magnum_Test_AbstractOpenGLTester_h
#define Magnum_Test_AbstractOpenGLTester_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
#include <Corrade/compatibility.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/DebugMessage.h"
#include "Magnum/Renderer.h"

#if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessGlxApplication.h"
#else
#error Cannot run OpenGL tests on this platform
#endif

namespace Magnum { namespace Test {

class AbstractOpenGLTester: public TestSuite::Tester, public Platform::WindowlessApplication {
    public:
        explicit AbstractOpenGLTester();

        using TestSuite::Tester::exec;
        int exec() override final { return TestSuite::Tester::exec(); }

    private:
        static int zero;
};

AbstractOpenGLTester::AbstractOpenGLTester(): Platform::WindowlessApplication({zero, nullptr}) {
    if(Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>()) {
        Renderer::enable(Renderer::Feature::DebugOutput);
        Renderer::enable(Renderer::Feature::DebugOutputSynchronous);
        DebugMessage::setDefaultCallback();
    }
}

int AbstractOpenGLTester::zero = 0;

#define MAGNUM_VERIFY_NO_ERROR() CORRADE_COMPARE(Magnum::Renderer::error(), Magnum::Renderer::Error::NoError)

}}

#endif
