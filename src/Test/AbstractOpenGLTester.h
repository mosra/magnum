#ifndef Magnum_Test_AbstractOpenGLTester_h
#define Magnum_Test_AbstractOpenGLTester_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <TestSuite/Tester.h>

#include "Renderer.h"

#if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Platform/WindowlessGlxApplication.h"
#else
#error Cannot run OpenGL tests on this platform
#endif

namespace Magnum { namespace Test {

class AbstractOpenGLTester: public TestSuite::Tester, public Platform::WindowlessApplication {
    public:
        explicit AbstractOpenGLTester(): Platform::WindowlessApplication({zero, nullptr}) {}

        using TestSuite::Tester::exec;
        int exec() override { return TestSuite::Tester::exec(); }

    private:
        static int zero;
};

int AbstractOpenGLTester::zero = 0;

#define MAGNUM_VERIFY_NO_ERROR() CORRADE_COMPARE(Renderer::error(), Renderer::Error::NoError)

}}

#endif
