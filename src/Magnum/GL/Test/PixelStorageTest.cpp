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
#include <Corrade/Utility/DebugStl.h> /** @todo drop once std::pair isn't used (i.e., the whole PixelStorage crap ceases to exist) */

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/GL/Implementation/imageProperties.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct PixelStorageTest: TestSuite::Tester {
    explicit PixelStorageTest();

    void occupiedCompressedImageDataSize();
};

PixelStorageTest::PixelStorageTest() {
    addTests({&PixelStorageTest::occupiedCompressedImageDataSize});
}

void PixelStorageTest::occupiedCompressedImageDataSize() {
    /* Assuming a custom 5x4x2 format with 16-byte blocks, with the whole image
       being 55x28x12 */
    const char data[(55/5)*(28/4)*(12/2)*16]{};

    /* The size is calculated from block properties and the *image* size, not
       the supplied row length / image height. This is what GL wants, it has no
       relation to anything useful. For comparison see the  PixelStorageTest::dataOffsetSizeCompressed() test in the core
       library. */
    {
        CompressedImageView3D image{
            CompressedPixelStorage{}
                .setRowLength(55)
                .setImageHeight(28)
                .setSkip({10, 8, 4}),
            42069, /* custom format */
            {5, 4, 2},
            16,
            {35, 20, 6},
            data};
        CORRADE_COMPARE(Implementation::occupiedCompressedImageDataSize(image),
            (35/5)*(20/4)*(6/2)*16);

    /* Same result if the size isn't whole blocks */
    } {
        CompressedImageView3D image{
            CompressedPixelStorage{}
                .setRowLength(55)
                .setImageHeight(28)
                .setSkip({10, 8, 4}),
            42069, /* custom format */
            {5, 4, 2},
            16,
            {31, 19, 5},
            data};
        CORRADE_COMPARE(Implementation::occupiedCompressedImageDataSize(image),
            (35/5)*(20/4)*(6/2)*16);
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::PixelStorageTest)
