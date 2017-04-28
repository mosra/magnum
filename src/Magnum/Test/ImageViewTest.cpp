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

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"

namespace Magnum { namespace Test {

struct ImageViewTest: TestSuite::Tester {
    explicit ImageViewTest();

    void construct();
    void constructNullptr();
    void constructCompressed();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void constructDeprecatedArrayView();
    void constructDeprecatedArray();
    #endif

    void setData();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void setDataDeprecatedArrayView();
    void setDataDeprecatedArray();
    #endif
    void setDataCompressed();
};

ImageViewTest::ImageViewTest() {
    addTests({&ImageViewTest::construct,
              &ImageViewTest::constructNullptr,
              &ImageViewTest::constructCompressed,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &ImageViewTest::constructDeprecatedArrayView,
              &ImageViewTest::constructDeprecatedArray,
              #endif

              &ImageViewTest::setData,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &ImageViewTest::setDataDeprecatedArrayView,
              &ImageViewTest::setDataDeprecatedArray,
              #endif
              &ImageViewTest::setDataCompressed});
}

void ImageViewTest::construct() {
    const char data[3*3]{};
    ImageView2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, data};

    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), PixelFormat::RGB);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), data);
}

void ImageViewTest::constructNullptr() {
    /* Just verify that it won't assert when passing nullptr array -- useful
       e.g. for old-style texture allocation using setImage() */
    ImageView2D a{PixelFormat::RGBA, PixelType::UnsignedByte, {256, 128}, nullptr};
    CORRADE_COMPARE(a.size(), (Vector2i{256, 128}));
}

void ImageViewTest::constructCompressed() {
    const char data[8]{};
    CompressedImageView2D a{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, data};

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.data(), data);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void ImageViewTest::constructDeprecatedArrayView() {
    char data[12]{};
    Containers::ArrayView<char> view{data};
    ImageView2D a{PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, view};
    CORRADE_COMPARE(a.data(), data);

    Containers::ArrayView<const char> cview{data};
    ImageView2D b{PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, cview};
    CORRADE_COMPARE(b.data(), data);
}

void ImageViewTest::constructDeprecatedArray() {
    Containers::Array<char> data{12};
    ImageView2D a{PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, data};

    CORRADE_COMPARE(a.data(), data);
}
#endif

void ImageViewTest::setData() {
    const char data[3*3]{};
    ImageView2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, data};
    const char data2[3*3]{};
    a.setData(data2);

    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), PixelFormat::RGB);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), data2);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void ImageViewTest::setDataDeprecatedArrayView() {
    ImageView2D a{PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, nullptr};

    const char data[12]{};
    Containers::ArrayView<const char> view{data};
    a.setData(view);
    CORRADE_COMPARE(a.data(), data);
}

void ImageViewTest::setDataDeprecatedArray() {
    ImageView2D a{PixelFormat::RGB, PixelType::UnsignedByte, {1, 3}, nullptr};

    Containers::Array<char> data{12};
    a.setData(data);
    CORRADE_COMPARE(a.data(), data);
}
#endif

void ImageViewTest::setDataCompressed() {
    const char data[8]{};
    CompressedImageView2D a{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, data};
    const char data2[16]{};
    a.setData(data2);

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.data(), data2);
}

}}

CORRADE_TEST_MAIN(Magnum::Test::ImageViewTest)
