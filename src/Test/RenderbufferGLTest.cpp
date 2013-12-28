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

#include "Context.h"
#include "Extensions.h"
#include "Renderbuffer.h"
#include "Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class RenderbufferGLTest: public AbstractOpenGLTester {
    public:
        explicit RenderbufferGLTest();

        void construct();
        void constructCopy();
        void constructMove();

        void label();
};

RenderbufferGLTest::RenderbufferGLTest() {
    addTests({&RenderbufferGLTest::construct,
              &RenderbufferGLTest::constructCopy,
              &RenderbufferGLTest::constructMove,

              &RenderbufferGLTest::label});
}

void RenderbufferGLTest::construct() {
    {
        const Renderbuffer renderbuffer;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(renderbuffer.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void RenderbufferGLTest::constructCopy() {
     CORRADE_VERIFY(!(std::is_constructible<Renderbuffer, const Renderbuffer&>{}));
     CORRADE_VERIFY(!(std::is_assignable<Renderbuffer, const Renderbuffer&>{}));
}

void RenderbufferGLTest::constructMove() {
    Renderbuffer a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    Renderbuffer b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    Renderbuffer c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
}

void RenderbufferGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current()->isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Renderbuffer renderbuffer;
    CORRADE_COMPARE(renderbuffer.label(), "");

    renderbuffer.setLabel("MyRenderbuffer");
    CORRADE_COMPARE(renderbuffer.label(), "MyRenderbuffer");

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::RenderbufferGLTest)
