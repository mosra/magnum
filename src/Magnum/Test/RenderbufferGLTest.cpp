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

#include "Magnum/Math/Vector2.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/RenderbufferFormat.h"
#include "Magnum/OpenGLTester.h"

namespace Magnum { namespace Test {

struct RenderbufferGLTest: OpenGLTester {
    explicit RenderbufferGLTest();

    void construct();
    void constructCopy();
    void constructMove();
    void wrap();

    void label();

    void setStorage();
    void setStorageMultisample();
};

RenderbufferGLTest::RenderbufferGLTest() {
    addTests({&RenderbufferGLTest::construct,
              &RenderbufferGLTest::constructCopy,
              &RenderbufferGLTest::constructMove,
              &RenderbufferGLTest::wrap,

              &RenderbufferGLTest::label,

              &RenderbufferGLTest::setStorage,
              &RenderbufferGLTest::setStorageMultisample});
}

void RenderbufferGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

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
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

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

void RenderbufferGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    GLuint id;
    glGenRenderbuffers(1, &id);

    /* Releasing won't delete anything */
    {
        auto renderbuffer = Renderbuffer::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(renderbuffer.release(), id);
    }

    /* ...so we can wrap it again */
    Renderbuffer::wrap(id);
    glDeleteRenderbuffers(1, &id);
}

void RenderbufferGLTest::label() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Renderbuffer renderbuffer;

    CORRADE_COMPARE(renderbuffer.label(), "");
    MAGNUM_VERIFY_NO_ERROR();

    renderbuffer.setLabel("MyRenderbuffer");
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(renderbuffer.label(), "MyRenderbuffer");
}

void RenderbufferGLTest::setStorage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Renderbuffer renderbuffer;

    #ifndef MAGNUM_TARGET_GLES2
    renderbuffer.setStorage(RenderbufferFormat::RGBA8, {128, 128});
    #else
    renderbuffer.setStorage(RenderbufferFormat::RGBA4, {128, 128});
    #endif

    MAGNUM_VERIFY_NO_ERROR();
}

void RenderbufferGLTest::setStorageMultisample() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::GL::ANGLE::framebuffer_multisample>() &&
       !Context::current().isExtensionSupported<Extensions::GL::NV::framebuffer_multisample>())
        CORRADE_SKIP("Required extension is not available.");
    #endif

    Renderbuffer renderbuffer;

    #ifndef MAGNUM_TARGET_GLES2
    renderbuffer.setStorageMultisample(Renderbuffer::maxSamples(), RenderbufferFormat::RGBA8, {128, 128});
    #else
    renderbuffer.setStorageMultisample(Renderbuffer::maxSamples(), RenderbufferFormat::RGBA4, {128, 128});
    #endif

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::RenderbufferGLTest)
