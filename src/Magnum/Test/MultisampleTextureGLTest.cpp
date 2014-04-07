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

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/MultisampleTexture.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class MultisampleTextureGLTest: public AbstractOpenGLTester {
    public:
        explicit MultisampleTextureGLTest();

        void construct2D();
        void construct2DArray();

        void bind2D();
        void bind2DArray();

        void storage2D();
        void storage2DArray();

        void invalidateImage2D();
        void invalidateImage2DArray();

        void invalidateSubImage2D();
        void invalidateSubImage2DArray();
};

MultisampleTextureGLTest::MultisampleTextureGLTest() {
    addTests({&MultisampleTextureGLTest::construct2D,
              &MultisampleTextureGLTest::construct2DArray,

              &MultisampleTextureGLTest::bind2D,
              &MultisampleTextureGLTest::bind2DArray,

              &MultisampleTextureGLTest::storage2D,
              &MultisampleTextureGLTest::storage2DArray,

              &MultisampleTextureGLTest::invalidateImage2D,
              &MultisampleTextureGLTest::invalidateImage2DArray,

              &MultisampleTextureGLTest::invalidateSubImage2D,
              &MultisampleTextureGLTest::invalidateSubImage2DArray});
}

void MultisampleTextureGLTest::construct2D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    {
        MultisampleTexture2D texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::construct2DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    {
        MultisampleTexture2DArray texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::bind2D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    MultisampleTexture2D texture;

    if(Context::current()->isExtensionSupported<Extensions::GL::ARB::multi_bind>()) {
        CORRADE_EXPECT_FAIL("With ARB_multi_bind the texture must be associated with given target at least once before binding it.");
        CORRADE_VERIFY(false);
        CORRADE_SKIP("...but we don't have any function to do that yet.");
    }

    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::bind2DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    MultisampleTexture2DArray texture;

    if(Context::current()->isExtensionSupported<Extensions::GL::ARB::multi_bind>()) {
        CORRADE_EXPECT_FAIL("With ARB_multi_bind the texture must be associated with given target at least once before binding it.");
        CORRADE_VERIFY(false);
        CORRADE_SKIP("...but we don't have any function to do that yet.");
    }

    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::storage2D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void MultisampleTextureGLTest::storage2DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void MultisampleTextureGLTest::invalidateImage2D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Multisample storage is not implemented yet.");
}

void MultisampleTextureGLTest::invalidateImage2DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Multisample storage is not implemented yet.");
}

void MultisampleTextureGLTest::invalidateSubImage2D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Multisample storage is not implemented yet.");
}

void MultisampleTextureGLTest::invalidateSubImage2DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Multisample storage is not implemented yet.");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::MultisampleTextureGLTest)
