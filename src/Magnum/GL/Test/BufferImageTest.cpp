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

#include "Magnum/GL/BufferImage.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct BufferImageTest: TestSuite::Tester {
    explicit BufferImageTest();

    void constructNoCreate();
    void constructNoCreateCompressed();

    void constructCopy();
    void constructCopyCompressed();
};

BufferImageTest::BufferImageTest() {
    addTests({&BufferImageTest::constructNoCreate,
              &BufferImageTest::constructNoCreateCompressed,

              &BufferImageTest::constructCopy,
              &BufferImageTest::constructCopyCompressed});
}

void BufferImageTest::constructNoCreate() {
    {
        BufferImage2D image{NoCreate};
        CORRADE_COMPARE(image.buffer().id(), 0);
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, BufferImage2D>::value));
}

void BufferImageTest::constructNoCreateCompressed() {
    {
        CompressedBufferImage2D image{NoCreate};
        CORRADE_COMPARE(image.buffer().id(), 0);
    }

    CORRADE_VERIFY(true);
}

void BufferImageTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<BufferImage2D, const BufferImage2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<BufferImage2D, const BufferImage2D&>{}));
}

void BufferImageTest::constructCopyCompressed() {
    CORRADE_VERIFY(!(std::is_constructible<CompressedBufferImage2D, const CompressedBufferImage2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<CompressedBufferImage2D, const CompressedBufferImage2D&>{}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::BufferImageTest)
