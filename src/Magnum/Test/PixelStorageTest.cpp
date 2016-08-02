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
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void dataPropertiesRowLength();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void dataPropertiesImageHeight();
    #endif

    void dataSize1D();
    void dataSize2D();
    void dataSize3D();

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
              #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
              &PixelStorageTest::dataPropertiesRowLength,
              #endif
              #ifndef MAGNUM_TARGET_GLES2
              &PixelStorageTest::dataPropertiesImageHeight,
              #endif

              &PixelStorageTest::dataSize1D,
              &PixelStorageTest::dataSize2D,
              &PixelStorageTest::dataSize3D,

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
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE(PixelStorage::pixelSize(PixelFormat::StencilIndex, PixelType::UnsignedByte), 1);
    #endif
    CORRADE_COMPARE(PixelStorage::pixelSize(PixelFormat::DepthStencil, PixelType::UnsignedInt248), 4);
}

void PixelStorageTest::dataProperties() {
    PixelStorage storage;
    storage.setAlignment(1);

    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{0}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{}, {0, 0, 0}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{}, {4, 1, 1}, 4}));
    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {8, 2, 1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{}, {8, 2, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{}, {2, 4, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 6}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{}, {2, 4, 6}, 1}));
    #endif
}

void PixelStorageTest::dataPropertiesAlignment() {
    PixelStorage storage;
    storage.setAlignment(8)
        .setSkip({3, 2, 1});

    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{0}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3*4, 0, 0}, {0, 0, 0}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{12, 16, 8}, {8, 1, 1}, 4}));
    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {8, 2, 1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3, 16, 16}, {8, 2, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3, 16, 32}, {8, 4, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 6}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3, 16, 32}, {8, 4, 6}, 1}));
    #endif
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void PixelStorageTest::dataPropertiesRowLength() {
    PixelStorage storage;
    storage.setAlignment(4)
        .setRowLength(15)
        .setSkip({3, 7, 0});

    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{0}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3*4, 7*15*4, 0}, {0, 0, 0}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3*4, 7*15*4, 0}, {60, 1, 1}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {4, 2, 1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3, 7*16, 0}, {16, 2, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3, 7*16, 0}, {16, 4, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 6}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3, 7*16, 0}, {16, 4, 6}, 1}));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void PixelStorageTest::dataPropertiesImageHeight() {
    PixelStorage storage;
    storage.setAlignment(1)
        .setImageHeight(128)
        .setSkip({3, 7, 2});

    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{0}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3*4, 0, 0}, {0, 0, 0}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3*4, 7*1*4, 2*128*1*4}, {4, 128, 1}, 4}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {4, 2, 1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3, 7*1*4, 2*128*4}, {4, 128, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 1}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3, 7*1*2, 2*128*2}, {2, 128, 1}, 1}));
    CORRADE_COMPARE(storage.dataProperties(PixelFormat::Red, PixelType::UnsignedByte, {2, 4, 6}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{3, 7*1*2, 2*128*2}, {2, 128, 6}, 1}));
}
#endif

void PixelStorageTest::dataSize1D() {
    const Image1D image{PixelStorage{}.setAlignment(2)
        .setSkip({2, 0, 0}),
        PixelFormat::RGB, PixelType::UnsignedByte};

    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Math::Vector<1, Int>{3}),
        16);
}

void PixelStorageTest::dataSize2D() {
    /* The same parameters as in PixelStorageGLTest 3D case */
    const Image2D image{PixelStorage{}.setAlignment(2)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        .setRowLength(7)
        #endif
        .setSkip({2, 3, 0}),
        PixelFormat::RGB, PixelType::UnsignedByte};

    #if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Vector2i{5, 9}),
        (3 + 9)*15);
    #else
    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Vector2i{5, 9}),
        (3 + 9)*22);
    #endif
}

void PixelStorageTest::dataSize3D() {
    /* The same parameters as in PixelStorageGLTest 3D case */
    const Image3D image{PixelStorage{}.setAlignment(2)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        .setRowLength(7)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        .setImageHeight(10)
        #endif
        .setSkip({2, 3, 1}),
        PixelFormat::RGB, PixelType::UnsignedByte};

    #if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Vector3i{5, 9, 3}),
        (1 + 3)*9*15);
    #elif defined(MAGNUM_TARGET_GLES2)
    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Vector3i{5, 9, 3}),
        (1 + 3)*9*22);
    #else
    CORRADE_COMPARE(Implementation::imageDataSizeFor(image, Vector3i{5, 9, 3}),
        (1 + 3)*10*22);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void PixelStorageTest::dataPropertiesCompressed() {
    CompressedPixelStorage storage;
    storage.setCompressedBlockSize({3, 4, 5})
        .setCompressedBlockDataSize(16);

    CORRADE_COMPARE(storage.dataProperties({2, 8, 11}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{}, {1, 2, 3}, 16}));
}

void PixelStorageTest::dataPropertiesCompressedRowLength() {
    CompressedPixelStorage storage;
    storage.setCompressedBlockSize({3, 4, 5})
        .setCompressedBlockDataSize(9)
        .setRowLength(12)
        .setSkip({5, 8, 0});

    CORRADE_COMPARE(storage.dataProperties({2, 8, 11}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{2*9, 8*9, 0}, {4, 2, 3}, 9}));
}

void PixelStorageTest::dataPropertiesCompressedImageHeight() {
    CompressedPixelStorage storage;
    storage.setCompressedBlockSize({3, 4, 5})
        .setCompressedBlockDataSize(16)
        .setImageHeight(12)
        .setSkip({5, 8, 11});

    CORRADE_COMPARE(storage.dataProperties({2, 8, 11}),
        (std::tuple<Vector3st, Vector3st, std::size_t>{{2*16, 2*16, 9*16}, {1, 3, 3}, 16}));
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
