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
#include "Framebuffer.h"
#include "Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class FramebufferGLTest: public AbstractOpenGLTester {
    public:
        explicit FramebufferGLTest();

        void construct();
        void constructCopy();
        void constructMove();

        void label();
};

FramebufferGLTest::FramebufferGLTest() {
    addTests({&FramebufferGLTest::construct,
              &FramebufferGLTest::constructCopy,
              &FramebufferGLTest::constructMove,

              &FramebufferGLTest::label});
}

void FramebufferGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    {
        const Framebuffer framebuffer({{32, 16}, {128, 256}});

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(framebuffer.id() > 0);
        CORRADE_COMPARE(framebuffer.viewport(), Range2Di({32, 16}, {128, 256}));
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void FramebufferGLTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Framebuffer, const Framebuffer&>{}));
    /* GCC 4.6 doesn't have std::is_assignable */
    #ifndef CORRADE_GCC46_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_assignable<Framebuffer, const Framebuffer&>{}));
    #endif
}

void FramebufferGLTest::constructMove() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Framebuffer a({{32, 16}, {128, 256}});
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    Framebuffer b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.viewport(), Range2Di({32, 16}, {128, 256}));

    Framebuffer c({{128, 256}, {32, 16}});
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.viewport(), Range2Di({32, 16}, {128, 256}));
}

void FramebufferGLTest::label() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current()->isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Framebuffer framebuffer({{}, Vector2i(32)});

    CORRADE_COMPARE(framebuffer.label(), "");
    MAGNUM_VERIFY_NO_ERROR();

    framebuffer.setLabel("MyFramebuffer");
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(framebuffer.label(), "MyFramebuffer");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::FramebufferGLTest)
