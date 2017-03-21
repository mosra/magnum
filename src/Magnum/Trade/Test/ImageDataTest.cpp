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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace Trade { namespace Test {

class ImageDataTest: public TestSuite::Tester {
    public:
        explicit ImageDataTest();

        void construct();
        void constructCompressed();
        void constructCopy();
        void constructMove();
        void constructMoveCompressed();

        void toView();
        void toViewCompressed();
        void release();
        void releaseCompressed();
};

ImageDataTest::ImageDataTest() {
    addTests({&ImageDataTest::construct,
              &ImageDataTest::constructCompressed,
              &ImageDataTest::constructCopy,
              &ImageDataTest::constructMove,
              &ImageDataTest::constructMoveCompressed,

              &ImageDataTest::toView,
              &ImageDataTest::toViewCompressed,
              &ImageDataTest::release,
              &ImageDataTest::releaseCompressed});
}

void ImageDataTest::construct() {
    auto data = new char[3*3];
    Trade::ImageData2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, Containers::Array<char>{data, 3*3}};

    CORRADE_VERIFY(!a.isCompressed());
    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), PixelFormat::RGB);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), data);
}

void ImageDataTest::constructCompressed() {
    auto data = new char[8];
    Trade::ImageData2D a{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};

    CORRADE_VERIFY(a.isCompressed());
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.data(), data);
    CORRADE_COMPARE(a.data().size(), 8);
}

void ImageDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Trade::ImageData2D, const Trade::ImageData2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Trade::ImageData2D, const Trade::ImageData2D&>{}));
}

void ImageDataTest::constructMove() {
    auto data = new char[3*3];
    Trade::ImageData2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, Containers::Array<char>{data, 3*3}};
    Trade::ImageData2D b(std::move(a));

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_VERIFY(!b.isCompressed());
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), PixelFormat::RGB);
    CORRADE_COMPARE(b.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);

    auto data2 = new char[2*2*6*4];
    Trade::ImageData2D c{PixelFormat::RGBA, PixelType::UnsignedShort, {2, 6}, Containers::Array<char>{data2, 2*2*6*4}};
    c = std::move(b);

    CORRADE_COMPARE(b.data(), data2);
    CORRADE_COMPARE(b.size(), Vector2i(2, 6));

    CORRADE_VERIFY(!c.isCompressed());
    CORRADE_COMPARE(c.storage().alignment(), 1);
    CORRADE_COMPARE(c.format(), PixelFormat::RGB);
    CORRADE_COMPARE(c.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(c.size(), Vector2i(1, 3));
    CORRADE_COMPARE(c.data(), data);
}

void ImageDataTest::constructMoveCompressed() {
    auto data = new char[8];
    Trade::ImageData2D a{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    Trade::ImageData2D b{std::move(a)};

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_VERIFY(b.isCompressed());
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(b.compressedStorage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(b.compressedFormat(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(b.size(), Vector2i(4, 4));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);

    auto data2 = new char[16];
    Trade::ImageData2D c{CompressedPixelFormat::RGBAS3tcDxt3, {8, 4}, Containers::Array<char>{data2, 16}};
    c = std::move(b);

    CORRADE_COMPARE_AS(b.data(), data2, char*);
    CORRADE_COMPARE(b.data().size(), 16);
    CORRADE_COMPARE(b.size(), Vector2i(8, 4));

    CORRADE_VERIFY(c.isCompressed());
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(c.compressedStorage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(c.compressedFormat(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(c.size(), Vector2i(4, 4));
    CORRADE_COMPARE(c.data(), data);
    CORRADE_COMPARE(c.data().size(), 8);
}

void ImageDataTest::toView() {
    auto data = new char[3*3];
    const Trade::ImageData2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, Containers::Array<char>{data, 3*3}};
    ImageView2D b = a;

    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), PixelFormat::RGB);
    CORRADE_COMPARE(b.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);
}

void ImageDataTest::toViewCompressed() {
    auto data = new char[8];
    const Trade::ImageData2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    CompressedImageView2D b = a;

    CORRADE_COMPARE(b.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(b.size(), Vector2i(4, 4));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);
}

void ImageDataTest::release() {
    char data[] = {'b', 'e', 'e', 'r'};
    Trade::ImageData2D a{PixelFormat::RGBA, PixelType::UnsignedByte, {1, 1}, Containers::Array<char>{data, 4}};
    const char* const pointer = a.release().release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());
}

void ImageDataTest::releaseCompressed() {
    char data[8];
    Trade::ImageData2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    const char* const pointer = a.release().release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ImageDataTest)
