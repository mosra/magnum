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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Framebuffer.h"

namespace Magnum { namespace Test {

struct FramebufferTest: TestSuite::Tester {
    explicit FramebufferTest();

    void constructNoCreate();

    void debugStatus();
};

FramebufferTest::FramebufferTest() {
    addTests({&FramebufferTest::constructNoCreate,

              &FramebufferTest::debugStatus});
}

void FramebufferTest::constructNoCreate() {
    {
        Framebuffer framebuffer{NoCreate};
        CORRADE_COMPARE(framebuffer.id(), 0);
    }

    CORRADE_VERIFY(true);
}

void FramebufferTest::debugStatus() {
    std::ostringstream out;

    Debug(&out) << Framebuffer::Status::IncompleteMissingAttachment << Framebuffer::Status(0xdead);
    CORRADE_COMPARE(out.str(), "Framebuffer::Status::IncompleteMissingAttachment Framebuffer::Status(0xdead)\n");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::FramebufferTest)
