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

#include "Magnum/GL/TextureArray.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct TextureArrayTest: TestSuite::Tester {
    explicit TextureArrayTest();

    #ifndef MAGNUM_TARGET_GLES
    void construct1DNoCreate();
    #endif
    void construct2DNoCreate();

    #ifndef MAGNUM_TARGET_GLES
    void constructCopy1D();
    #endif
    void constructCopy2D();
};

TextureArrayTest::TextureArrayTest() {
    addTests({
        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayTest::construct1DNoCreate,
        #endif
        &TextureArrayTest::construct2DNoCreate,

        #ifndef MAGNUM_TARGET_GLES
        &TextureArrayTest::constructCopy1D,
        #endif
        &TextureArrayTest::constructCopy2D});
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayTest::construct1DNoCreate() {
    {
        Texture1DArray texture{NoCreate};
        CORRADE_COMPARE(texture.id(), 0);
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Texture1DArray>::value));
}
#endif

void TextureArrayTest::construct2DNoCreate() {
    {
        Texture2DArray texture{NoCreate};
        CORRADE_COMPARE(texture.id(), 0);
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Texture2DArray>::value));
}

#ifndef MAGNUM_TARGET_GLES
void TextureArrayTest::constructCopy1D() {
    CORRADE_VERIFY(!(std::is_constructible<Texture1DArray, const Texture1DArray&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Texture1DArray, const Texture1DArray&>{}));
}
#endif

void TextureArrayTest::constructCopy2D() {
    CORRADE_VERIFY(!(std::is_constructible<Texture2DArray, const Texture2DArray&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Texture2DArray, const Texture2DArray&>{}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::TextureArrayTest)
