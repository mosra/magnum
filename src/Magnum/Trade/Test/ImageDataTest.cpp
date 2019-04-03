/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct ImageDataTest: TestSuite::Tester {
    explicit ImageDataTest();

    void constructGeneric();
    void constructImplementationSpecific();
    void constructCompressedGeneric();
    void constructCompressedImplementationSpecific();

    void constructInvalidSize();
    void constructCompressedInvalidSize();

    void constructCopy();

    void constructMoveGeneric();
    void constructMoveImplementationSpecific();
    void constructMoveCompressedGeneric();
    void constructMoveCompressedImplementationSpecific();

    void constructMoveAttachState();
    void constructMoveCompressedAttachState();

    void toViewGeneric();
    void toViewImplementationSpecific();
    void toViewCompressedGeneric();
    void toViewCompressedImplementationSpecific();

    void access();
    void dataProperties();

    void release();
    void releaseCompressed();
};

ImageDataTest::ImageDataTest() {
    addTests({&ImageDataTest::constructGeneric,
              &ImageDataTest::constructImplementationSpecific,
              &ImageDataTest::constructCompressedGeneric,
              &ImageDataTest::constructCompressedImplementationSpecific,

              &ImageDataTest::constructInvalidSize,
              &ImageDataTest::constructCompressedInvalidSize,

              &ImageDataTest::constructCopy,

              &ImageDataTest::constructMoveGeneric,
              &ImageDataTest::constructMoveImplementationSpecific,
              &ImageDataTest::constructMoveCompressedGeneric,
              &ImageDataTest::constructMoveCompressedImplementationSpecific,

              &ImageDataTest::constructMoveAttachState,
              &ImageDataTest::constructMoveCompressedAttachState,

              &ImageDataTest::toViewGeneric,
              &ImageDataTest::toViewImplementationSpecific,
              &ImageDataTest::toViewCompressedGeneric,
              &ImageDataTest::toViewCompressedImplementationSpecific,

              &ImageDataTest::access,
              &ImageDataTest::dataProperties,

              &ImageDataTest::release,
              &ImageDataTest::releaseCompressed});
}

namespace GL {
    enum class PixelFormat { RGB = 666 };
    enum class PixelType { UnsignedShort = 1337 };
    /* Clang -Wmissing-prototypes warns otherwise, even though this is in an
       anonymous namespace */
    UnsignedInt pixelSize(PixelFormat, PixelType);
    UnsignedInt pixelSize(PixelFormat format, PixelType type) {
        CORRADE_INTERNAL_ASSERT(format == PixelFormat::RGB);
        CORRADE_INTERNAL_ASSERT(type == PixelType::UnsignedShort);
        return 6;
    }

    enum class CompressedPixelFormat { RGBS3tcDxt1 = 21 };
}

namespace Vk {
    enum class PixelFormat { R32G32B32F = 42 };
    /* Clang -Wmissing-prototypes warns otherwise, even though this is in an
       anonymous namespace */
    UnsignedInt pixelSize(PixelFormat);
    UnsignedInt pixelSize(PixelFormat format) {
        CORRADE_INTERNAL_ASSERT(format == PixelFormat::R32G32B32F);
        return 12;
    }

    enum class CompressedPixelFormat { Bc1SRGBAlpha = 42 };
}

void ImageDataTest::constructGeneric() {
    {
        auto data = new char[4*4];
        int state;
        ImageData2D a{PixelFormat::RGBA8Unorm, {1, 3}, Containers::Array<char>{data, 4*4}, &state};

        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RGBA8Unorm);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 4);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 4*4);
        CORRADE_COMPARE(a.importerState(), &state);
    } {
        auto data = new char[3*2];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            PixelFormat::R16UI, {1, 3}, Containers::Array<char>{data, 3*2}, &state};

        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::R16UI);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 2);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 3*2);
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructImplementationSpecific() {
    /* Single format */
    {
        auto data = new char[3*12];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            Vk::PixelFormat::R32G32B32F, {1, 3}, Containers::Array<char>{data, 3*12}, &state};

        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 3*12);
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Format + extra */
    {
        auto data = new char[3*6];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}, &state};

        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Manual pixel size */
    {
        auto data = new char[3*6];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {1, 3}, Containers::Array<char>{data, 3*6}, &state};

        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructCompressedGeneric() {
    {
        auto data = new char[8];
        int state;
        ImageData2D a{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4},
            Containers::Array<char>{data, 8}, &state};

        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 8);
        CORRADE_COMPARE(a.importerState(), &state);
    } {
        auto data = new char[8];
        int state;
        ImageData2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            CompressedPixelFormat::Bc1RGBAUnorm, {4, 4},
            Containers::Array<char>{data, 8}, &state};

        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), Vector2i(4, 4));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 8);
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructCompressedImplementationSpecific() {
    /* Format with autodetection */
    {
        auto data = new char[8];
        int state;
        ImageData2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4},
            Containers::Array<char>{data, 8}, &state};

        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.compressedFormat(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 8);
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Manual properties not implemented yet */
}

void ImageDataTest::constructInvalidSize() {
    std::ostringstream out;
    Error redirectError{&out};

    /* Doesn't consider alignment */
    ImageData2D{PixelFormat::RGB8Unorm, {1, 3}, Containers::Array<char>{3*3}};
    CORRADE_COMPARE(out.str(), "Trade::ImageData::ImageData(): data too small, got 9 but expected at least 12 bytes\n");
}

void ImageDataTest::constructCompressedInvalidSize() {
    CORRADE_EXPECT_FAIL("Size checking for compressed image data is not implemented yet.");

    /* Too small for given format */
    {
        std::ostringstream out;
        Error redirectError{&out};
        ImageData2D{CompressedPixelFormat::Bc2RGBAUnorm, {4, 4}, Containers::Array<char>{2}};
        CORRADE_COMPARE(out.str(), "Trade::ImageData::ImageData(): data too small, got 2 but expected at least 4 bytes\n");

    /* Size should be rounded up even if the image size is not full block */
    } {
        std::ostringstream out;
        Error redirectError{&out};
        ImageData2D{CompressedPixelFormat::Bc2RGBAUnorm, {2, 2}, Containers::Array<char>{2}};
        CORRADE_COMPARE(out.str(), "Trade::ImageData::ImageData(): data too small, got 2 but expected at least 4 bytes\n");
    }
}

void ImageDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Trade::ImageData2D, const Trade::ImageData2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Trade::ImageData2D, const Trade::ImageData2D&>{}));
}

void ImageDataTest::constructMoveGeneric() {
    auto data = new char[3*16];
    int state;
    ImageData2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGBA32F, {1, 3}, Containers::Array<char>{data, 3*16}, &state};
    ImageData2D b(std::move(a));

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_VERIFY(!b.isCompressed());
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), PixelFormat::RGBA32F);
    CORRADE_COMPARE(b.formatExtra(), 0);
    CORRADE_COMPARE(b.pixelSize(), 16);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 3*16);
    CORRADE_COMPARE(b.importerState(), &state);

    auto data2 = new char[24];
    ImageData2D c{PixelFormat::R8I, {2, 6}, Containers::Array<char>{data2, 24}};
    c = std::move(b);

    CORRADE_COMPARE(b.data(), data2);
    CORRADE_COMPARE(b.size(), (Vector2i{2, 6}));

    CORRADE_VERIFY(!c.isCompressed());
    CORRADE_COMPARE(c.storage().alignment(), 1);
    CORRADE_COMPARE(c.format(), PixelFormat::RGBA32F);
    CORRADE_COMPARE(c.formatExtra(), 0);
    CORRADE_COMPARE(c.pixelSize(), 16);
    CORRADE_COMPARE(c.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(c.data(), data);
    CORRADE_COMPARE(c.data().size(), 3*16);
    CORRADE_COMPARE(c.importerState(), &state);
}

void ImageDataTest::constructMoveImplementationSpecific() {
    auto data = new char[3*6];
    int state;
    ImageData2D a{PixelStorage{}.setAlignment(1),
        GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}, &state};
    ImageData2D b(std::move(a));

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_VERIFY(!b.isCompressed());
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(b.formatExtra(), 1337);
    CORRADE_COMPARE(b.pixelSize(), 6);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 3*6);
    CORRADE_COMPARE(b.importerState(), &state);

    auto data2 = new char[12*4*2];
    ImageData2D c{PixelStorage{},
        1, 2, 8, {2, 6}, Containers::Array<char>{data2, 12*4*2}};
    c = std::move(b);

    CORRADE_COMPARE(b.data(), data2);
    CORRADE_COMPARE(b.size(), Vector2i(2, 6));

    CORRADE_VERIFY(!c.isCompressed());
    CORRADE_COMPARE(c.storage().alignment(), 1);
    CORRADE_COMPARE(c.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(c.formatExtra(), 1337);
    CORRADE_COMPARE(c.pixelSize(), 6);
    CORRADE_COMPARE(c.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(c.data(), data);
    CORRADE_COMPARE(c.data().size(), 3*6);
    CORRADE_COMPARE(c.importerState(), &state);
}

void ImageDataTest::constructMoveCompressedGeneric() {
    auto data = new char[8];
    int state;
    ImageData2D a{
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        CompressedPixelFormat::Bc3RGBAUnorm, {4, 4}, Containers::Array<char>{data, 8}, &state};
    ImageData2D b{std::move(a)};

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_VERIFY(b.isCompressed());
    CORRADE_COMPARE(b.compressedStorage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.compressedFormat(), CompressedPixelFormat::Bc3RGBAUnorm);
    CORRADE_COMPARE(b.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);
    CORRADE_COMPARE(b.importerState(), &state);

    auto data2 = new char[16];
    ImageData2D c{CompressedPixelFormat::Bc1RGBAUnorm, {8, 4}, Containers::Array<char>{data2, 16}};
    c = std::move(b);

    CORRADE_COMPARE(b.data(), data2);
    CORRADE_COMPARE(b.size(), (Vector2i{8, 4}));

    CORRADE_VERIFY(c.isCompressed());
    CORRADE_COMPARE(c.compressedStorage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(c.compressedFormat(), CompressedPixelFormat::Bc3RGBAUnorm);
    CORRADE_COMPARE(c.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(c.data(), data);
    CORRADE_COMPARE(c.data().size(), 8);
    CORRADE_COMPARE(c.importerState(), &state);
}

void ImageDataTest::constructMoveCompressedImplementationSpecific() {
    auto data = new char[8];
    int state;
    ImageData2D a{
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}, &state};
    ImageData2D b{std::move(a)};

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_VERIFY(b.isCompressed());
    CORRADE_COMPARE(b.compressedStorage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.compressedFormat(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(b.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);
    CORRADE_COMPARE(b.importerState(), &state);

    auto data2 = new char[16];
    ImageData2D c{CompressedPixelFormat::Bc2RGBAUnorm, {8, 4}, Containers::Array<char>{data2, 16}};
    c = std::move(b);

    CORRADE_COMPARE(b.data(), data2);
    CORRADE_COMPARE(b.size(), (Vector2i{8, 4}));

    CORRADE_VERIFY(c.isCompressed());
    CORRADE_COMPARE(c.compressedStorage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(c.compressedFormat(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(c.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(c.data(), data);
    CORRADE_COMPARE(c.data().size(), 8);
    CORRADE_COMPARE(c.importerState(), &state);
}

void ImageDataTest::constructMoveAttachState() {
    auto data = new char[3*6];
    int stateOld, stateNew;
    ImageData2D a{PixelStorage{}.setAlignment(1),
        GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}, &stateOld};
    ImageData2D b{std::move(a), &stateNew};

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_VERIFY(!b.isCompressed());
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(b.formatExtra(), 1337);
    CORRADE_COMPARE(b.pixelSize(), 6);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 3*6);
    CORRADE_COMPARE(b.importerState(), &stateNew);
}

void ImageDataTest::constructMoveCompressedAttachState() {
    auto data = new char[8];
    int stateOld, stateNew;
    ImageData2D a{
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}, &stateOld};
    ImageData2D b{std::move(a), &stateNew};

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_VERIFY(b.isCompressed());
    CORRADE_COMPARE(b.compressedStorage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.compressedFormat(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(b.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);
    CORRADE_COMPARE(b.importerState(), &stateNew);
}

void ImageDataTest::toViewGeneric() {
    auto data = new char[3*4];
    const ImageData2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RG16I, {1, 3}, Containers::Array<char>{data, 3*4}};
    ImageView2D b = a;

    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), PixelFormat::RG16I);
    CORRADE_COMPARE(b.formatExtra(), 0);
    CORRADE_COMPARE(b.pixelSize(), 4);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), data);
}

void ImageDataTest::toViewImplementationSpecific() {
    auto data = new char[3*6];
    const ImageData2D a{PixelStorage{}.setAlignment(1),
        GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}};
    ImageView2D b = a;

    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(b.formatExtra(), 1337);
    CORRADE_COMPARE(b.pixelSize(), 6);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), data);
}

void ImageDataTest::toViewCompressedGeneric() {
    auto data = new char[8];
    const ImageData2D a{
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        CompressedPixelFormat::Bc1RGBUnorm, {4, 4}, Containers::Array<char>{data, 8}};
    CompressedImageView2D b = a;

    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.format(), CompressedPixelFormat::Bc1RGBUnorm);
    CORRADE_COMPARE(b.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);
}

void ImageDataTest::toViewCompressedImplementationSpecific() {
    auto data = new char[8];
    const ImageData2D a{
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    CompressedImageView2D b = a;

    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(b.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);
}

void ImageDataTest::access() {
    auto data = new char[4*4];
    ImageData2D a{PixelFormat::RGBA8Unorm, {1, 3}, Containers::Array<char>{data, 4*4}};
    const ImageData2D& ca = a;
    CORRADE_COMPARE(a.data(), data);
    CORRADE_COMPARE(ca.data(), data);
}

void ImageDataTest::dataProperties() {
    ImageData3D image{
        PixelStorage{}
            .setAlignment(8)
            .setSkip({3, 2, 1}),
        PixelFormat::R8Unorm, {2, 4, 6},
        Containers::Array<char>{224}};
    CORRADE_COMPARE(image.dataProperties(),
        (std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>>{{3, 16, 32}, {8, 4, 6}}));
}

void ImageDataTest::release() {
    char data[] = {'b', 'e', 'e', 'r'};
    Trade::ImageData2D a{PixelFormat::RGBA8Unorm, {1, 1}, Containers::Array<char>{data, 4}};
    const char* const pointer = a.release().release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());
}

void ImageDataTest::releaseCompressed() {
    char data[8];
    Trade::ImageData2D a{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, Containers::Array<char>{data, 8}};
    const char* const pointer = a.release().release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ImageDataTest)
