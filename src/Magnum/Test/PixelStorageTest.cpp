/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/PixelStorage.h"

namespace Magnum { namespace Test {

struct PixelStorageTest: TestSuite::Tester {
    explicit PixelStorageTest();

    void pixelSize();

    void dataProperties();
    void dataPropertiesAlignment();
    void dataPropertiesRowLength();
    #ifndef MAGNUM_TARGET_GLES2
    void dataPropertiesImageHeight();
    #endif

    void dataSize();

    #ifndef MAGNUM_TARGET_GLES
    void dataPropertiesCompressed();
    void dataPropertiesCompressedRowLength();
    void dataPropertiesCompressedImageHeight();

    void dataOffsetSizeCompressed();
    #endif
};

typedef Math::Vector3<std::size_t> Vector3st;

PixelStorageTest::PixelStorageTest() {
    addTests({&PixelStorageTest::pixelSize,

              &PixelStorageTest::dataProperties,
              &PixelStorageTest::dataPropertiesAlignment,
              &PixelStorageTest::dataPropertiesRowLength,
              #ifndef MAGNUM_TARGET_GLES2
              &PixelStorageTest::dataPropertiesImageHeight,
              #endif

              &PixelStorageTest::dataSize,

              #ifndef MAGNUM_TARGET_GLES
              &PixelStorageTest::dataPropertiesCompressed,
              &PixelStorageTest::dataPropertiesCompressedRowLength,
              &PixelStorageTest::dataPropertiesCompressedImageHeight,

              &PixelStorageTest::dataOffsetSizeCompressed
              #endif
              });
}

void PixelStorageTest::pixelSize() {
    CORRADE_COMPARE(PixelStorage::pixelSize(PixelFormat::RGBA, PixelType::UnsignedInt), 4*4);
    CORRADE_COMPARE(PixelStorage::pixelSize(PixelFormat::DepthComponent, PixelType::UnsignedShort), 2);
    CORRADE_COMPARE(PixelStorage::pixelSize(PixelFormat::StencilIndex, PixelType::UnsignedByte), 1);
    CORRADE_COMPARE(PixelStorage::pixelSize(PixelFormat::DepthStencil, PixelType::UnsignedInt248), 4);
}

void PixelStorageTest::dataProperties() {
    PixelStorage storage;
    storage.setAlignment(1);

    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{0}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{0, {0, 0, 0}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{0, {4, 1, 1}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {8, 2, 1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{0, {8, 2, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{0, {2, 4, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 6}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{0, {2, 4, 6}, 1}));
}

void PixelStorageTest::dataPropertiesAlignment() {
    PixelStorage storage;
    storage.setAlignment(8)
        .setSkip({3, 2, 1});

    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{0}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3*4, {0, 0, 0}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{8 + 16 + 3*4, {8, 1, 1}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {8, 2, 1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{16 + 16 + 3, {8, 2, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{32 + 16 + 3, {8, 4, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 6}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{32 + 16 + 3, {8, 4, 6}, 1}));
}

void PixelStorageTest::dataPropertiesRowLength() {
    PixelStorage storage;
    storage.setAlignment(4)
        .setRowLength(15)
        .setSkip({3, 7, 0});

    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{0}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3*4 + 7*15*4, {0, 0, 0}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3*4 + 7*15*4, {60, 1, 1}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {4, 2, 1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3 + 7*16, {16, 2, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3 + 7*16, {16, 4, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 6}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3 + 7*16, {16, 4, 6}, 1}));
}

#ifndef MAGNUM_TARGET_GLES2
void PixelStorageTest::dataPropertiesImageHeight() {
    PixelStorage storage;
    storage.setAlignment(1)
        .setImageHeight(128)
        .setSkip({3, 7, 2});

    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{0}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3*4, {0, 0, 0}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3*4 + 7*1*4 + 2*128*1*4, {4, 128, 1}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {4, 2, 1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3 + 7*1*4 + 2*128*4, {4, 128, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 1}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3 + 7*1*2 + 2*128*2, {2, 128, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 6}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{3 + 7*1*2 + 2*128*2, {2, 128, 6}, 1}));
}
#endif

void PixelStorageTest::dataSize() {
    /* The same parameters as in PixelStorageGLTest 3D case */
    const Image2D image{PixelStorage{}.setAlignment(2)
        .setRowLength(3)
        #ifndef MAGNUM_TARGET_GLES2
        .setImageHeight(5)
        #endif
        .setSkip({2, 3, 1}),
        PixelFormat::RGB, PixelType::UnsignedByte};

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Vector2i{2, 3}),
        5*10 + 3*10 + 6 + 3*10);
    #else
    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Vector2i{2, 3}),
        3*10 + 3*10 + 6 + 3*10);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void PixelStorageTest::dataPropertiesCompressed() {
    CompressedPixelStorage storage;
    storage.setCompressedBlockSize({3, 4, 5})
        .setCompressedBlockDataSize(16);

    CORRADE_COMPARE(storage.dataProperties({2, 8, 11}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{0, {1, 2, 3}, 16}));
}

void PixelStorageTest::dataPropertiesCompressedRowLength() {
    CompressedPixelStorage storage;
    storage.setCompressedBlockSize({3, 4, 5})
        .setCompressedBlockDataSize(9)
        .setRowLength(12)
        .setSkip({5, 8, 0});

    CORRADE_COMPARE(storage.dataProperties({2, 8, 11}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{(2 + 8)*9, {4, 2, 3}, 9}));
}

void PixelStorageTest::dataPropertiesCompressedImageHeight() {
    CompressedPixelStorage storage;
    storage.setCompressedBlockSize({3, 4, 5})
        .setCompressedBlockDataSize(16)
        .setImageHeight(12)
        .setSkip({5, 8, 11});

    CORRADE_COMPARE(storage.dataProperties({2, 8, 11}),
        (std::tuple<std::size_t, Vector3st, std::size_t>{(2 + 2 + 9)*16, {1, 3, 3}, 16}));
}

void PixelStorageTest::dataOffsetSizeCompressed() {
    /* Tf the storage doesn't contain any info about block sizes (the default),
       using the provided value */
    CORRADE_COMPARE(Implementation::compressedImageDataOffsetSizeFor(CompressedImage3D{},
        Vector2i{37, 35}, 1579), (std::pair<std::size_t, std::size_t>{0, 1579}));

    /* The same parameters as in PixelStorageGLTest 3D case */
    const CompressedImage3D image{CompressedPixelStorage{}
        .setCompressedBlockSize({4, 4, 1})
        .setCompressedBlockDataSize(16)
        .setRowLength(8)
        .setImageHeight(8)
        .setSkip({4, 4, 4})};
    CORRADE_COMPARE(Implementation::compressedImageDataOffsetSizeFor(image, Vector3i{4, 4, 1}, 1579),
        (std::pair<std::size_t, std::size_t>{16*4*4 + 16*2 + 16, 16}));
}
#endif

}}

CORRADE_TEST_MAIN(Magnum::Test::PixelStorageTest)
