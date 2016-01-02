/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "Magnum/Buffer.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

struct AbstractObjectGLTest: AbstractOpenGLTester {
    explicit AbstractObjectGLTest();

    void labelNoOp();
};

AbstractObjectGLTest::AbstractObjectGLTest() {
    addTests({&AbstractObjectGLTest::labelNoOp});
}

void AbstractObjectGLTest::labelNoOp() {
    if(Context::current().isExtensionSupported<Extensions::GL::KHR::debug>())
        CORRADE_SKIP(Extensions::GL::KHR::debug::string() + std::string(" is supported."));

    Buffer buffer;
    buffer.setLabel("MyBuffer");
    CORRADE_COMPARE(buffer.label(), "");
    MAGNUM_VERIFY_NO_ERROR();
}

}}

MAGNUM_GL_TEST_MAIN(Magnum::Test::AbstractObjectGLTest)
