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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/GL/Texture.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct TextureTest: TestSuite::Tester {
    explicit TextureTest();

    #ifndef MAGNUM_TARGET_GLES
    void construct1DNoCreate();
    #endif
    void construct2DNoCreate();
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void construct3DNoCreate();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void constructCopy1D();
    #endif
    void constructCopy2D();
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void constructCopy3D();
    #endif
};

TextureTest::TextureTest() {
    addTests({
        #ifndef MAGNUM_TARGET_GLES
        &TextureTest::construct1DNoCreate,
        #endif
        &TextureTest::construct2DNoCreate,
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        &TextureTest::construct3DNoCreate,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureTest::constructCopy1D,
        #endif
        &TextureTest::constructCopy2D,
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        &TextureTest::constructCopy3D
        #endif
        });
}

#ifndef MAGNUM_TARGET_GLES
void TextureTest::construct1DNoCreate() {
    {
        Texture1D texture{NoCreate};
        CORRADE_COMPARE(texture.id(), 0);
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Texture1D>::value));
}
#endif

void TextureTest::construct2DNoCreate() {
    {
        Texture2D texture{NoCreate};
        CORRADE_COMPARE(texture.id(), 0);
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Texture2D>::value));
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void TextureTest::construct3DNoCreate() {
    {
        Texture3D texture{NoCreate};
        CORRADE_COMPARE(texture.id(), 0);
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Texture3D>::value));
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureTest::constructCopy1D() {
    CORRADE_VERIFY(!(std::is_constructible<Texture1D, const Texture1D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Texture1D, const Texture1D&>{}));
}
#endif

void TextureTest::constructCopy2D() {
    CORRADE_VERIFY(!(std::is_constructible<Texture2D, const Texture2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Texture2D, const Texture2D&>{}));
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void TextureTest::constructCopy3D() {
    CORRADE_VERIFY(!(std::is_constructible<Texture3D, const Texture3D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Texture3D, const Texture3D&>{}));
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::TextureTest)
