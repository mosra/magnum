/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Utility/DebugStl.h>

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct AbstractObjectGLTest: OpenGLTester {
    explicit AbstractObjectGLTest();

    void labelNoOp();
};

AbstractObjectGLTest::AbstractObjectGLTest() {
    addTests({&AbstractObjectGLTest::labelNoOp});
}

void AbstractObjectGLTest::labelNoOp() {
    if(Context::current().isExtensionSupported<GL::Extensions::KHR::debug>())
        CORRADE_SKIP(GL::Extensions::KHR::debug::string() + std::string(" is supported."));
    if(Context::current().isExtensionSupported<GL::Extensions::EXT::debug_label>())
        CORRADE_SKIP(GL::Extensions::EXT::debug_label::string() + std::string(" is supported."));

    Buffer buffer;
    buffer.setLabel("MyBuffer");
    CORRADE_COMPARE(buffer.label(), "");
    MAGNUM_VERIFY_NO_GL_ERROR();
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::AbstractObjectGLTest)
