/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Text/GlyphCacheGL.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct GlyphCacheGL_Test: TestSuite::Tester {
    explicit GlyphCacheGL_Test();

    void constructNoCreate();
    #ifndef MAGNUM_TARGET_GLES2
    void constructNoCreateArray();
    #endif
};

GlyphCacheGL_Test::GlyphCacheGL_Test() {
    addTests({&GlyphCacheGL_Test::constructNoCreate,
              #ifndef MAGNUM_TARGET_GLES2
              &GlyphCacheGL_Test::constructNoCreateArray
              #endif
              });
}

void GlyphCacheGL_Test::constructNoCreate() {
    GlyphCacheGL cache{NoCreate};

    /* Shouldn't crash or try to acces GL */
    CORRADE_VERIFY(true);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, GlyphCacheGL>::value);
}

#ifndef MAGNUM_TARGET_GLES2
void GlyphCacheGL_Test::constructNoCreateArray() {
    GlyphCacheArrayGL cache{NoCreate};

    /* Shouldn't crash or try to acces GL */
    CORRADE_VERIFY(true);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, GlyphCacheArrayGL>::value);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::GlyphCacheGL_Test)
