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

#include <TestSuite/Tester.h>

#include "Image.h"
#include "ImageFormat.h"

namespace Magnum { namespace Test {

class ImageTest: public TestSuite::Tester {
    public:
        explicit ImageTest();

        void moveConstructor();
        void moveAssignment();
};

ImageTest::ImageTest() {
    addTests({&ImageTest::moveConstructor,
              &ImageTest::moveAssignment});
}

void ImageTest::moveConstructor() {
    unsigned char* data = new unsigned char[3];
    Image2D a({1, 3}, ImageFormat::Red, ImageType::UnsignedByte, data);

    Image2D b(std::move(a));
    CORRADE_VERIFY(!a.data());
    CORRADE_COMPARE(b.format(), ImageFormat::Red);
    CORRADE_COMPARE(b.type(), ImageType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_VERIFY(b.data() == data);
}

void ImageTest::moveAssignment() {
    unsigned char* data = new unsigned char[3];
    Image2D a({1, 3}, ImageFormat::Red, ImageType::UnsignedByte, data);

    Image2D b(ImageFormat::Red, ImageType::UnsignedByte);
    b = std::move(a);
    CORRADE_VERIFY(!a.data());
    CORRADE_COMPARE(b.format(), ImageFormat::Red);
    CORRADE_COMPARE(b.type(), ImageType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_VERIFY(b.data() == data);
}

}}

CORRADE_TEST_MAIN(Magnum::Test::ImageTest)
