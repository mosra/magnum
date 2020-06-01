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

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/PixelStorage.h"

namespace Magnum { namespace Test { namespace {

struct PixelStorageTest: TestSuite::Tester {
    explicit PixelStorageTest();

    void compare();
    void compareCompressed();

    void dataProperties();
    void dataPropertiesAlignment();
    void dataPropertiesRowLength();
    void dataPropertiesImageHeight();

    void dataSize1D();
    void dataSize2D();
    void dataSize3D();

    void dataPropertiesCompressed();
    void dataPropertiesCompressedRowLength();
    void dataPropertiesCompressedImageHeight();

    void dataOffsetSizeCompressed();
};

typedef Math::Vector3<std::size_t> Vector3st;

PixelStorageTest::PixelStorageTest() {
    addTests({&PixelStorageTest::compare,
              &PixelStorageTest::compareCompressed,

              &PixelStorageTest::dataProperties,
              &PixelStorageTest::dataPropertiesAlignment,
              &PixelStorageTest::dataPropertiesRowLength,
              &PixelStorageTest::dataPropertiesImageHeight,

              &PixelStorageTest::dataSize1D,
              &PixelStorageTest::dataSize2D,
              &PixelStorageTest::dataSize3D,

              &PixelStorageTest::dataPropertiesCompressed,
              &PixelStorageTest::dataPropertiesCompressedRowLength,
              &PixelStorageTest::dataPropertiesCompressedImageHeight,

              &PixelStorageTest::dataOffsetSizeCompressed});
}

void PixelStorageTest::compare() {
    PixelStorage a;
    a.setRowLength(1)
     .setImageHeight(15)
     .setSkip({1, 3, 4})
     .setAlignment(3);

    CORRADE_VERIFY(a == a);
    CORRADE_VERIFY(a != PixelStorage{});
    CORRADE_VERIFY(PixelStorage{} == PixelStorage{});
    CORRADE_VERIFY(PixelStorage{}.setRowLength(15) != PixelStorage{}.setRowLength(17));
    CORRADE_VERIFY(PixelStorage{}.setImageHeight(32) != PixelStorage{}.setImageHeight(31));
    CORRADE_VERIFY(PixelStorage{}.setSkip({1, 5, 7}) != PixelStorage{}.setSkip({7, 1, 5}));
    CORRADE_VERIFY(PixelStorage{}.setAlignment(3) != PixelStorage{}.setAlignment(5));
}

void PixelStorageTest::compareCompressed() {
    CompressedPixelStorage a;
    a.setSkip({16, 2, 1})
     .setCompressedBlockSize({4, 8, 2})
     .setCompressedBlockDataSize(16);

    CORRADE_VERIFY(a == a);
    CORRADE_VERIFY(a != CompressedPixelStorage{});
    CORRADE_VERIFY(CompressedPixelStorage{} == CompressedPixelStorage{});
    CORRADE_VERIFY(CompressedPixelStorage{}.setSkip({16, 4, 17}) != CompressedPixelStorage{}.setSkip({4, 35, 12}));
    CORRADE_VERIFY(CompressedPixelStorage{}.setCompressedBlockSize({2, 7, 19}) != CompressedPixelStorage{}.setCompressedBlockSize({2, 7, 16}));
    CORRADE_VERIFY(CompressedPixelStorage{}.setCompressedBlockDataSize(32) != CompressedPixelStorage{}.setCompressedBlockDataSize(30));
}

void PixelStorageTest::dataProperties() {
    PixelStorage storage;
    storage.setAlignment(1);

    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::RGBA8Unorm), Vector3i{0}),
        (std::pair<Vector3st, Vector3st>{{}, {0, 0, 0}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::RGBA8Unorm), Vector3i{1}),
        (std::pair<Vector3st, Vector3st>{{}, {4, 1, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {8, 2, 1}),
        (std::pair<Vector3st, Vector3st>{{}, {8, 2, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {2, 4, 1}),
        (std::pair<Vector3st, Vector3st>{{}, {2, 4, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {2, 4, 6}),
        (std::pair<Vector3st, Vector3st>{{}, {2, 4, 6}}));
}

void PixelStorageTest::dataPropertiesAlignment() {
    PixelStorage storage;
    storage.setAlignment(8)
        .setSkip({3, 2, 1});

    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::RGBA8Unorm), Vector3i{0}),
        (std::pair<Vector3st, Vector3st>{{3*4, 0, 0}, {0, 0, 0}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::RGBA8Unorm), Vector3i{1}),
        (std::pair<Vector3st, Vector3st>{{12, 16, 8}, {8, 1, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {8, 2, 1}),
        (std::pair<Vector3st, Vector3st>{{3, 16, 16}, {8, 2, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {2, 4, 1}),
        (std::pair<Vector3st, Vector3st>{{3, 16, 32}, {8, 4, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {2, 4, 6}),
        (std::pair<Vector3st, Vector3st>{{3, 16, 32}, {8, 4, 6}}));
}

void PixelStorageTest::dataPropertiesRowLength() {
    PixelStorage storage;
    storage.setAlignment(4)
        .setRowLength(15)
        .setSkip({3, 7, 0});

    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::RGBA8Unorm), Vector3i{0}),
        (std::pair<Vector3st, Vector3st>{{3*4, 7*15*4, 0}, {0, 0, 0}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::RGBA8Unorm), Vector3i{1}),
        (std::pair<Vector3st, Vector3st>{{3*4, 7*15*4, 0}, {60, 1, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {4, 2, 1}),
        (std::pair<Vector3st, Vector3st>{{3, 7*16, 0}, {16, 2, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {2, 4, 1}),
        (std::pair<Vector3st, Vector3st>{{3, 7*16, 0}, {16, 4, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {2, 4, 6}),
        (std::pair<Vector3st, Vector3st>{{3, 7*16, 0}, {16, 4, 6}}));
}

void PixelStorageTest::dataPropertiesImageHeight() {
    PixelStorage storage;
    storage.setAlignment(1)
        .setImageHeight(128)
        .setSkip({3, 7, 2});

    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::RGBA8Unorm), Vector3i{0}),
        (std::pair<Vector3st, Vector3st>{{3*4, 0, 0}, {0, 0, 0}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::RGBA8Unorm), Vector3i{1}),
        (std::pair<Vector3st, Vector3st>{{3*4, 7*1*4, 2*128*1*4}, {4, 128, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {4, 2, 1}),
        (std::pair<Vector3st, Vector3st>{{3, 7*1*4, 2*128*4}, {4, 128, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {2, 4, 1}),
        (std::pair<Vector3st, Vector3st>{{3, 7*1*2, 2*128*2}, {2, 128, 1}}));
    CORRADE_COMPARE(storage.dataProperties(pixelSize(PixelFormat::R8Unorm), {2, 4, 6}),
        (std::pair<Vector3st, Vector3st>{{3, 7*1*2, 2*128*2}, {2, 128, 6}}));
}

void PixelStorageTest::dataSize1D() {
    const Image1D image{
        PixelStorage{}.setAlignment(2).setSkip({2, 0, 0}),
        PixelFormat::RGB8Unorm};

    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Math::Vector<1, Int>{3}),
        16);
}

void PixelStorageTest::dataSize2D() {
    const Image2D image{
        PixelStorage{}.setAlignment(2)
            .setRowLength(7)
            .setSkip({2, 3, 0}),
        PixelFormat::RGB8Unorm};

    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Vector2i{5, 9}),
        (3 + 9)*22);

    /* This shouldn't overflow the 128x128 rectangle */
    const Image2D image2{
        PixelStorage{}.setSkip({64, 0, 0})
            .setRowLength(128),
        PixelFormat::RGBA8Unorm};

    CORRADE_COMPARE(Implementation::imageDataSizeFor(image2, Vector2i{64, 128}), 65536);

    /* This shouldn't overflow the 128x128 rectangle */
    const Image2D image3{
        PixelStorage{}.setSkip({64, 64, 0})
            .setRowLength(128),
        PixelFormat::RGBA8Unorm};

    CORRADE_COMPARE(Implementation::imageDataSizeFor(image3, Vector2i{64, 64}), 65536);
}

void PixelStorageTest::dataSize3D() {
    const Image3D image{
        PixelStorage{}.setAlignment(2)
            .setRowLength(7)
            .setImageHeight(10)
            .setSkip({2, 3, 1}),
        PixelFormat::RGB8Unorm};

    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Vector3i{5, 9, 3}),
        (1 + 3)*10*22);

    /* This shouldn't overflow the 128x128x128 cube */
    const Image3D image2{
        PixelStorage{}.setSkip({64, 64, 0})
            .setRowLength(128)
            .setImageHeight(128),
        PixelFormat::RGBA8Unorm};

    CORRADE_COMPARE(Implementation::imageDataSizeFor(image2, Vector3i{64, 64, 128}), 8388608);

    /* This shouldn't overflow the 128x128x128 cube */
    const Image3D image3{
        PixelStorage{}.setSkip({64, 64, 64})
            .setRowLength(128)
            .setImageHeight(128),
        PixelFormat::RGBA8Unorm};

    CORRADE_COMPARE(Implementation::imageDataSizeFor(image3, Vector3i{64, 64, 64}), 8388608);
}

void PixelStorageTest::dataPropertiesCompressed() {
    CompressedPixelStorage storage;
    storage.setCompressedBlockSize({3, 4, 5})
        .setCompressedBlockDataSize(16);

    CORRADE_COMPARE(storage.dataProperties({2, 8, 11}),
        (std::pair<Vector3st, Vector3st>{{}, {1, 2, 3}}));
}

void PixelStorageTest::dataPropertiesCompressedRowLength() {
    CompressedPixelStorage storage;
    storage.setCompressedBlockSize({3, 4, 5})
        .setCompressedBlockDataSize(9)
        .setRowLength(12)
        .setSkip({5, 8, 0});

    CORRADE_COMPARE(storage.dataProperties({2, 8, 11}),
        (std::pair<Vector3st, Vector3st>{{2*9, 8*9, 0}, {4, 2, 3}}));
}

void PixelStorageTest::dataPropertiesCompressedImageHeight() {
    CompressedPixelStorage storage;
    storage.setCompressedBlockSize({3, 4, 5})
        .setCompressedBlockDataSize(16)
        .setImageHeight(12)
        .setSkip({5, 8, 11});

    CORRADE_COMPARE(storage.dataProperties({2, 8, 11}),
        (std::pair<Vector3st, Vector3st>{{2*16, 2*16, 9*16}, {1, 3, 3}}));
}

void PixelStorageTest::dataOffsetSizeCompressed() {
    /* The same parameters as in PixelStorageGLTest 3D case */
    const CompressedImage3D image{CompressedPixelStorage{}
        .setCompressedBlockSize({4, 4, 1})
        .setCompressedBlockDataSize(16)
        .setRowLength(8)
        .setImageHeight(8)
        .setSkip({4, 4, 4})};
    CORRADE_COMPARE(Implementation::compressedImageDataOffsetSizeFor(image, Vector3i{4, 4, 1}),
        (std::pair<std::size_t, std::size_t>{16*4*4 + 16*2 + 16, 16}));
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::PixelStorageTest)
