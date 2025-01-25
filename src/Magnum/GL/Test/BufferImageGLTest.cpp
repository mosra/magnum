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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once dataProperties() std::pair is gone */

#include "Magnum/PixelFormat.h"
#include "Magnum/GL/BufferImage.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/OpenGLTester.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct BufferImageGLTest: OpenGLTester {
    explicit BufferImageGLTest();

    void construct();
    void constructGeneric();
    void constructPlaceholder();
    void constructCompressed();
    void constructCompressedGeneric();
    void constructCompressedPlaceholder();
    void constructBuffer();
    void constructBufferGeneric();
    void constructBufferCompressed();
    void constructBufferCompressedGeneric();

    void constructInvalidSize();
    void constructCompressedInvalidSize();

    void constructMove();
    void constructMoveCompressed();

    void dataProperties();
    void dataPropertiesCompressed();

    void setData();
    void setDataGeneric();
    void setDataKeepStorage();
    void setDataCompressed();
    void setDataCompressedGeneric();
    void setDataCompressedKeepStorage();
    void setDataInvalidSize();
    void setDataCompressedInvalidSize();

    void release();
    void releaseCompressed();
};

BufferImageGLTest::BufferImageGLTest() {
    addTests({&BufferImageGLTest::construct,
              &BufferImageGLTest::constructGeneric,
              &BufferImageGLTest::constructPlaceholder,
              &BufferImageGLTest::constructCompressed,
              &BufferImageGLTest::constructCompressedGeneric,
              &BufferImageGLTest::constructCompressedPlaceholder,
              &BufferImageGLTest::constructBuffer,
              &BufferImageGLTest::constructBufferGeneric,
              &BufferImageGLTest::constructBufferCompressed,
              &BufferImageGLTest::constructBufferCompressedGeneric,

              &BufferImageGLTest::constructInvalidSize,
              &BufferImageGLTest::constructCompressedInvalidSize,

              &BufferImageGLTest::constructMove,
              &BufferImageGLTest::constructMoveCompressed,

              &BufferImageGLTest::dataProperties,
              &BufferImageGLTest::dataPropertiesCompressed,

              &BufferImageGLTest::setData,
              &BufferImageGLTest::setDataGeneric,
              &BufferImageGLTest::setDataKeepStorage,
              &BufferImageGLTest::setDataCompressed,
              &BufferImageGLTest::setDataCompressedGeneric,
              &BufferImageGLTest::setDataCompressedKeepStorage,
              &BufferImageGLTest::setDataInvalidSize,
              &BufferImageGLTest::setDataCompressedInvalidSize,

              &BufferImageGLTest::release,
              &BufferImageGLTest::releaseCompressed});
}

void BufferImageGLTest::construct() {
    const char data[] = { 'a', 'b', 'c' };
    BufferImage2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::Red, PixelType::UnsignedByte, {1, 3}, data, BufferUsage::StaticDraw};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), PixelFormat::Red);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(a.pixelSize(), 1);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.dataSize(), 3);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructGeneric() {
    const char data[] = { 'a', 'b', 'c' };
    BufferImage2D a{PixelStorage{}.setAlignment(1),
        Magnum::PixelFormat::R8Unorm, {1, 3}, data, BufferUsage::StaticDraw};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), PixelFormat::Red);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(a.pixelSize(), 1);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.dataSize(), 3);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructPlaceholder() {
    {
        BufferImage2D a{PixelFormat::Red, PixelType::UnsignedByte};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::Red);
        CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
        CORRADE_COMPARE(a.pixelSize(), 1);
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.dataSize(), 0);
        CORRADE_VERIFY(a.buffer().id());
    } {
        BufferImage2D a{
            PixelStorage{}
                /* Even with skip it shouldn't assert on data size */
                .setSkip({1, 0, 0})
                .setAlignment(1),
            PixelFormat::RGB, PixelType::UnsignedByte};

        CORRADE_COMPARE(a.storage().skip(), (Vector3i{1, 0, 0}));
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::RGB);
        CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
        CORRADE_COMPARE(a.pixelSize(), 3);
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.dataSize(), 0);
        CORRADE_VERIFY(a.buffer().id());
    }
}

void BufferImageGLTest::constructCompressed() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0 };
    CompressedBufferImage2D a{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt1,
        {4, 4}, data, BufferUsage::StaticDraw};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.dataSize(), 8);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructCompressedGeneric() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0 };
    CompressedBufferImage2D a{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        Magnum::CompressedPixelFormat::Bc1RGBAUnorm,
        {4, 4}, data, BufferUsage::StaticDraw};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.dataSize(), 8);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructCompressedPlaceholder() {
    {
        CompressedBufferImage2D a;

        CORRADE_COMPARE(a.storage().rowLength(), 0);
        CORRADE_COMPARE(a.format(), CompressedPixelFormat{});
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.dataSize(), 0);
        CORRADE_VERIFY(a.buffer().id());
    } {
        CompressedBufferImage2D a{
            CompressedPixelStorage{}
                /* Even with skip it shouldn't assert on data size */
                .setSkip({1, 0, 0})
                .setRowLength(12)
        };

        CORRADE_COMPARE(a.storage().skip(), (Vector3i{1, 0, 0}));
        CORRADE_COMPARE(a.storage().rowLength(), 12);
        CORRADE_COMPARE(a.format(), CompressedPixelFormat{});
        CORRADE_COMPARE(a.size(), Vector2i{});
        CORRADE_COMPARE(a.dataSize(), 0);
        CORRADE_VERIFY(a.buffer().id());
    }
}

void BufferImageGLTest::constructBuffer() {
    const char data[] = { 'a', 'b', 'c' };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);
    const UnsignedInt id = buffer.id();

    BufferImage2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::Red, PixelType::UnsignedByte, {1, 3}, Utility::move(buffer), sizeof(data)};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_VERIFY(!buffer.id());
    CORRADE_COMPARE(a.buffer().id(), id);
    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), PixelFormat::Red);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(a.pixelSize(), 1);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.dataSize(), 3);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructBufferGeneric() {
    const char data[] = { 'a', 'b', 'c' };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);
    const UnsignedInt id = buffer.id();

    BufferImage2D a{PixelStorage{}.setAlignment(1),
        Magnum::PixelFormat::R8Unorm, {1, 3}, Utility::move(buffer), sizeof(data)};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_VERIFY(!buffer.id());
    CORRADE_COMPARE(a.buffer().id(), id);
    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), PixelFormat::Red);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(a.pixelSize(), 1);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.dataSize(), 3);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructBufferCompressed() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);
    const UnsignedInt id = buffer.id();

    CompressedBufferImage2D a{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt1,
        {4, 4}, Utility::move(buffer), sizeof(data)};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_VERIFY(!buffer.id());
    CORRADE_COMPARE(a.buffer().id(), id);
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.dataSize(), 8);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructBufferCompressedGeneric() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);
    const UnsignedInt id = buffer.id();

    CompressedBufferImage2D a{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        Magnum::CompressedPixelFormat::Bc1RGBAUnorm,
        {4, 4}, Utility::move(buffer), sizeof(data)};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_VERIFY(!buffer.id());
    CORRADE_COMPARE(a.buffer().id(), id);
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.dataSize(), 8);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    BufferImage2D{Magnum::PixelFormat::RGB8Unorm, {1, 3}, Containers::Array<char>{11}, BufferUsage::StaticDraw};
    CORRADE_COMPARE(out, "GL::BufferImage: data too small, got 11 but expected at least 12 bytes\n");
}

void BufferImageGLTest::constructCompressedInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    CORRADE_EXPECT_FAIL("Size checking for compressed image data is not implemented yet.");

    /* Too small for given format */
    {
        Containers::String out;
        Error redirectError{&out};
        CompressedBufferImage2D{Magnum::CompressedPixelFormat::Bc2RGBAUnorm, {4, 4}, Containers::Array<char>{15}, BufferUsage::StaticDraw};
        /* Here it's assuming the buffer is already filled, of given size */
        CompressedBufferImage2D{Magnum::CompressedPixelFormat::Bc2RGBAUnorm, {4, 4}, GL::Buffer{}, 15};
        CORRADE_COMPARE_AS(out,
            "GL::CompressedBufferImage: data too small, got 15 but expected at least 16 bytes\n"
            "GL::CompressedBufferImage: data too small, got 15 but expected at least 16 bytes\n",
            TestSuite::Compare::String);

    /* Size should be rounded up even if the image size is not full block */
    } {
        Containers::String out;
        Error redirectError{&out};
        CompressedBufferImage2D{Magnum::CompressedPixelFormat::Bc2RGBAUnorm, {2, 2}, Containers::Array<char>{15}, BufferUsage::StaticDraw};
        /* Here it's assuming the buffer is already filled, of given size */
        CompressedBufferImage2D{Magnum::CompressedPixelFormat::Bc2RGBAUnorm, {2, 2}, GL::Buffer{}, 15};
        CORRADE_COMPARE_AS(out,
            "GL::CompressedBufferImage: data too small, got 15 but expected at least 16 bytes\n"
            "GL::CompressedBufferImage: data too small, got 15 but expected at least 16 bytes\n",
            TestSuite::Compare::String);
    }
}

void BufferImageGLTest::constructMove() {
    BufferImage2D a{PixelFormat::RGB, PixelType::UnsignedByte, {4, 1}, "abcabcabcab", BufferUsage::StaticDraw};
    const Int id = a.buffer().id();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    BufferImage2D b(Utility::move(a));

    CORRADE_COMPARE(a.buffer().id(), 0);
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_COMPARE(b.storage().alignment(), 4);
    CORRADE_COMPARE(b.format(), PixelFormat::RGB);
    CORRADE_COMPARE(b.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(b.pixelSize(), 3);
    CORRADE_COMPARE(b.size(), Vector2i(4, 1));
    CORRADE_COMPARE(b.dataSize(), 12);
    CORRADE_COMPARE(b.buffer().id(), id);

    const unsigned short data2[2*4] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    BufferImage2D c{PixelStorage{}.setAlignment(1),
        PixelFormat::RGBA, PixelType::UnsignedShort, {1, 2}, data2, BufferUsage::StaticDraw};
    const Int cId = c.buffer().id();
    c = Utility::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.buffer().id(), cId);
    CORRADE_COMPARE(b.size(), Vector2i(1, 2));

    CORRADE_COMPARE(c.storage().alignment(), 4);
    CORRADE_COMPARE(c.format(), PixelFormat::RGB);
    CORRADE_COMPARE(c.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(c.pixelSize(), 3);
    CORRADE_COMPARE(c.size(), Vector2i(4, 1));
    CORRADE_COMPARE(c.dataSize(), 12);
    CORRADE_COMPARE(c.buffer().id(), id);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<BufferImage2D>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<BufferImage2D>::value);
}

void BufferImageGLTest::constructMoveCompressed() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0 };
    CompressedBufferImage2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, data, BufferUsage::StaticDraw};
    const Int id = a.buffer().id();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    CompressedBufferImage2D b{Utility::move(a)};

    CORRADE_COMPARE(a.buffer().id(), 0);
    CORRADE_COMPARE(a.size(), Vector2i());
    CORRADE_COMPARE(a.dataSize(), 0);

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{0});
    #endif
    CORRADE_COMPARE(b.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(b.size(), Vector2i(4, 4));
    CORRADE_COMPARE(b.dataSize(), 8);
    CORRADE_COMPARE(b.buffer().id(), id);

    const unsigned char data2[] = { 'a', 0, 0, 0, 'b', 0, 0, 0, 'c', 0, 0, 0, 'd', 0, 0, 0 };
    CompressedBufferImage2D c{
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt1, {8, 4}, data2, BufferUsage::StaticDraw};
    const Int cId = c.buffer().id();
    c = Utility::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.buffer().id(), cId);
    CORRADE_COMPARE(b.size(), Vector2i(8, 4));
    CORRADE_COMPARE(b.dataSize(), 16);

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(c.storage().compressedBlockSize(), Vector3i{0});
    #endif
    CORRADE_COMPARE(c.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(c.size(), Vector2i(4, 4));
    CORRADE_COMPARE(c.dataSize(), 8);
    CORRADE_COMPARE(c.buffer().id(), id);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<CompressedBufferImage2D>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<CompressedBufferImage2D>::value);
}

void BufferImageGLTest::dataProperties() {
    const char data[224]{};
    BufferImage3D image{
        PixelStorage{}
            .setAlignment(8)
            .setSkip({3, 2, 1}),
        Magnum::PixelFormat::R8Unorm, {2, 4, 6}, data, BufferUsage::StaticDraw};
    CORRADE_COMPARE(image.dataProperties(),
        (std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>>{{3, 16, 32}, {8, 4, 6}}));
}

void BufferImageGLTest::dataPropertiesCompressed() {
    /* Yes, I know, this is totally bogus and doesn't match the BC1 format */
    const char data[1]{};
    CompressedBufferImage3D image{
        CompressedPixelStorage{}
            .setCompressedBlockSize({3, 4, 5})
            .setCompressedBlockDataSize(16)
            .setImageHeight(12)
            .setSkip({5, 8, 11}),
        Magnum::CompressedPixelFormat::Bc1RGBAUnorm, {2, 8, 11}, data, BufferUsage::StaticDraw};
    CORRADE_COMPARE(image.dataProperties(),
        (std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>>{{2*16, 2*16, 9*16}, {1, 3, 3}}));
}

void BufferImageGLTest::setData() {
    const char data[4] = { 'a', 'b', 'c', 'd' };
    BufferImage2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::Red, PixelType::UnsignedByte, {4, 1}, data, BufferUsage::StaticDraw};

    const UnsignedShort data2[2*4] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    a.setData(PixelFormat::RGBA, PixelType::UnsignedShort, {1, 2}, data2, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(a.storage().alignment(), 4);
    CORRADE_COMPARE(a.format(), PixelFormat::RGBA);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedShort);
    CORRADE_COMPARE(a.size(), Vector2i(1, 2));
    CORRADE_COMPARE(a.pixelSize(), 8);
    CORRADE_COMPARE(a.dataSize(), 16);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(imageData),
        Containers::arrayView(data2),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::setDataGeneric() {
    const char data[4] = { 'a', 'b', 'c', 'd' };
    BufferImage2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::Red, PixelType::UnsignedByte, {4, 1}, data, BufferUsage::StaticDraw};

    const UnsignedShort data2[2*4] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    a.setData(Magnum::PixelFormat::RGBA16Unorm, {1, 2}, data2, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(a.storage().alignment(), 4);
    CORRADE_COMPARE(a.format(), PixelFormat::RGBA);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedShort);
    CORRADE_COMPARE(a.size(), Vector2i(1, 2));
    CORRADE_COMPARE(a.pixelSize(), 8);
    CORRADE_COMPARE(a.dataSize(), 16);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(imageData),
        Containers::arrayView(data2),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::setDataKeepStorage() {
    const char data[]{
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l'
    };
    BufferImage2D a{
        PixelFormat::Red, PixelType::UnsignedByte, {4, 1},
        data, BufferUsage::StaticDraw};

    a.setData(PixelStorage{}.setAlignment(1),
        PixelFormat::RGB, PixelType::UnsignedShort, {2, 1},
        nullptr, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), PixelFormat::RGB);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedShort);
    CORRADE_COMPARE(a.size(), (Vector2i{2, 1}));
    CORRADE_COMPARE(a.pixelSize(), 6);
    CORRADE_COMPARE(a.dataSize(), 12);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData,
        Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::setDataCompressed() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0 };
    CompressedBufferImage2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, data, BufferUsage::StaticDraw};

    const char data2[] = { 'a', 0, 0, 0, 'b', 0, 0, 0, 'c', 0, 0, 0, 'd', 0, 0, 0 };
    a.setData(
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        CompressedPixelFormat::RGBAS3tcDxt3, {8, 4}, data2, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt3);
    CORRADE_COMPARE(a.size(), Vector2i(8, 4));
    CORRADE_COMPARE(a.dataSize(), 16);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data2),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::setDataCompressedGeneric() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0 };
    CompressedBufferImage2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, data, BufferUsage::StaticDraw};

    const char data2[] = { 'a', 0, 0, 0, 'b', 0, 0, 0, 'c', 0, 0, 0, 'd', 0, 0, 0 };
    a.setData(
        #ifndef MAGNUM_TARGET_GLES
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        #endif
        Magnum::CompressedPixelFormat::Bc2RGBAUnorm, {8, 4}, data2, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt3);
    CORRADE_COMPARE(a.size(), Vector2i(8, 4));
    CORRADE_COMPARE(a.dataSize(), 16);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data2),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::setDataCompressedKeepStorage() {
    const char data[]{
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'
    };
    CompressedBufferImage2D a{
        CompressedPixelFormat::RGBAS3tcDxt1, {8, 3},
        data, BufferUsage::StaticDraw};

    a.setData(CompressedPixelStorage{}.setRowLength(3),
        CompressedPixelFormat::SRGB8Alpha8Astc4x4, {2, 4},
        nullptr, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(a.storage().rowLength(), 3);
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::SRGB8Alpha8Astc4x4);
    CORRADE_COMPARE(a.size(), (Vector2i{2, 4}));
    CORRADE_COMPARE(a.dataSize(), 16);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData,
        Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::setDataInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    BufferImage2D image{PixelFormat::Red, PixelType::UnsignedByte, {}, {nullptr, 7}, BufferUsage::StaticDraw};

    Containers::String out;
    Error redirectError{&out};
    image.setData(PixelFormat::RGB, PixelType::UnsignedByte, {1, 3},  Containers::Array<char>{11}, BufferUsage::StaticDraw);
    /* Keeping current storage */
    image.setData(PixelFormat::RGBA, PixelType::UnsignedByte, {2, 1},  nullptr, BufferUsage::StaticDraw);
    CORRADE_COMPARE_AS(out,
        "GL::BufferImage::setData(): data too small, got 11 but expected at least 12 bytes\n"
        "GL::BufferImage::setData(): current storage too small, got 7 but expected at least 8 bytes\n",
        TestSuite::Compare::String);
}

void BufferImageGLTest::setDataCompressedInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Fits almost two blocks, but only almost */
    CompressedBufferImage2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, "helloheyhellhe", BufferUsage::StaticDraw};
    CORRADE_COMPARE(a.dataSize(), 15);

    CORRADE_EXPECT_FAIL("Size checking for compressed image data is not implemented yet.");

    /* Too small for given format */
    {
        Containers::String out;
        Error redirectError{&out};
        a.setData(CompressedPixelFormat::RGBAS3tcDxt3, {8, 4}, "helloheyhelloheyhelloheyhellhe", BufferUsage::StaticDraw);
        /* Keeping current storage */
        a.setData(CompressedPixelFormat::RGBAS3tcDxt1, {8, 4}, nullptr, BufferUsage::StaticDraw);
        CORRADE_COMPARE_AS(out,
            "GL::CompressedBufferImage::setData(): data too small, got 31 but expected at least 32 bytes\n"
            "GL::CompressedBufferImage::setData(): current storage too small, got 15 but expected at least 16 bytes\n",
            TestSuite::Compare::String);

    /* Size should be rounded up even if the image size is not that big */
    } {
        Containers::String out;
        Error redirectError{&out};
        a.setData(CompressedPixelFormat::RGBAS3tcDxt3, {5, 2}, "helloheyhelloheyhelloheyhellhe", BufferUsage::StaticDraw);
        /* Keeping current storage */
        a.setData(CompressedPixelFormat::RGBAS3tcDxt1, {5, 2}, nullptr, BufferUsage::StaticDraw);
        CORRADE_COMPARE_AS(out,
            "GL::CompressedBufferImage::setData(): data too small, got 31 but expected at least 32 bytes\n"
            "GL::CompressedBufferImage::setData(): current storage too small, got 15 but expected at least 16 bytes\n",
            TestSuite::Compare::String);
    }
}

void BufferImageGLTest::release() {
    BufferImage2D a{PixelFormat::RGBA, PixelType::UnsignedByte};
    const UnsignedInt id = a.buffer().id();

    CORRADE_VERIFY(a.buffer().id());
    Buffer b = a.release();
    CORRADE_VERIFY(!a.buffer().id());
    CORRADE_COMPARE(b.id(), id);
}

void BufferImageGLTest::releaseCompressed() {
    CompressedBufferImage2D a;
    const UnsignedInt id = a.buffer().id();

    CORRADE_VERIFY(a.buffer().id());
    Buffer b = a.release();
    CORRADE_VERIFY(!a.buffer().id());
    CORRADE_COMPARE(b.id(), id);
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::BufferImageGLTest)
