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

#include "AbstractShaderProgram.h"
#include "Context.h"
#include "Extensions.h"
#include "Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class AbstractShaderProgramGLTest: public AbstractOpenGLTester {
    public:
        explicit AbstractShaderProgramGLTest();

        void construct();
        void constructCopy();
        void constructMove();

        void label();
};

AbstractShaderProgramGLTest::AbstractShaderProgramGLTest() {
    addTests({&AbstractShaderProgramGLTest::construct,
              &AbstractShaderProgramGLTest::constructCopy,
              &AbstractShaderProgramGLTest::constructMove,

              &AbstractShaderProgramGLTest::label});
}

namespace {

class MyShader: public AbstractShaderProgram {
    public:
        explicit MyShader() {}
};

}

void AbstractShaderProgramGLTest::construct() {
    {
        const MyShader shader;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(shader.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void AbstractShaderProgramGLTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<MyShader, const MyShader&>{}));
    /* GCC 4.6 doesn't have std::is_assignable */
    #ifndef CORRADE_GCC46_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_assignable<MyShader, const MyShader&>{}));
    #endif
}

void AbstractShaderProgramGLTest::constructMove() {
    MyShader a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    MyShader b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    MyShader c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
}

void AbstractShaderProgramGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current()->isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    MyShader shader;
    CORRADE_COMPARE(shader.label(), "");

    shader.setLabel("MyShader");
    CORRADE_COMPARE(shader.label(), "MyShader");

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::AbstractShaderProgramGLTest)
