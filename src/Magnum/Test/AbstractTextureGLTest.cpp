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

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Texture.h"
#include "Magnum/OpenGLTester.h"

namespace Magnum { namespace Test {

struct AbstractTextureGLTest: OpenGLTester {
    explicit AbstractTextureGLTest();

    void construct();
    void constructCopy();
    void constructMove();

    void label();
};

AbstractTextureGLTest::AbstractTextureGLTest() {
    addTests({&AbstractTextureGLTest::construct,
              &AbstractTextureGLTest::constructCopy,
              &AbstractTextureGLTest::constructMove,

              &AbstractTextureGLTest::label});
}

void AbstractTextureGLTest::construct() {
    {
        const Texture2D texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void AbstractTextureGLTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Texture2D, const Texture2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Texture2D, const Texture2D&>{}));
}

void AbstractTextureGLTest::constructMove() {
    Texture2D a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    Texture2D b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    Texture2D c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
}

void AbstractTextureGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Texture2D texture;

    CORRADE_COMPARE(texture.label(), "");
    MAGNUM_VERIFY_NO_ERROR();

    texture.setLabel("MyTexture");
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.label(), "MyTexture");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::AbstractTextureGLTest)
