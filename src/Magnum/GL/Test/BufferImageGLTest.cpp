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

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/GL/BufferImage.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/OpenGLTester.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct BufferImageGLTest: OpenGLTester {
    explicit BufferImageGLTest();

    void construct();
    void constructGeneric();
    void constructCompressed();
    void constructCompressedGeneric();
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
    void setDataCompressed();
    void setDataCompressedGeneric();

    void release();
    void releaseCompressed();
};

BufferImageGLTest::BufferImageGLTest() {
    addTests({&BufferImageGLTest::construct,
              &BufferImageGLTest::constructGeneric,
              &BufferImageGLTest::constructCompressed,
              &BufferImageGLTest::constructCompressedGeneric,
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
              &BufferImageGLTest::setDataCompressed,
              &BufferImageGLTest::setDataCompressedGeneric,

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
    CORRADE_COMPARE(a.format(), GL::PixelFormat::Red);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
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
    CORRADE_COMPARE(a.format(), GL::PixelFormat::Red);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.dataSize(), 3);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
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
    CORRADE_COMPARE(a.format(), GL::CompressedPixelFormat::RGBAS3tcDxt1);
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
    CORRADE_COMPARE(a.format(), GL::CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.dataSize(), 8);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructBuffer() {
    const char data[] = { 'a', 'b', 'c' };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);
    const UnsignedInt id = buffer.id();

    BufferImage2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::Red, PixelType::UnsignedByte, {1, 3}, std::move(buffer), sizeof(data)};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_VERIFY(!buffer.id());
    CORRADE_COMPARE(a.buffer().id(), id);
    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), GL::PixelFormat::Red);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
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
        Magnum::PixelFormat::R8Unorm, {1, 3}, std::move(buffer), sizeof(data)};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_VERIFY(!buffer.id());
    CORRADE_COMPARE(a.buffer().id(), id);
    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), GL::PixelFormat::Red);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
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
        {4, 4}, std::move(buffer), sizeof(data)};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_VERIFY(!buffer.id());
    CORRADE_COMPARE(a.buffer().id(), id);
    CORRADE_COMPARE(a.format(), GL::CompressedPixelFormat::RGBAS3tcDxt1);
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
        {4, 4}, std::move(buffer), sizeof(data)};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    #endif
    CORRADE_VERIFY(!buffer.id());
    CORRADE_COMPARE(a.buffer().id(), id);
    CORRADE_COMPARE(a.format(), GL::CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.dataSize(), 8);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data),
        TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* Doesn't consider alignment */
    BufferImage2D{Magnum::PixelFormat::RGB8Unorm, {1, 3}, Containers::Array<char>{3*3}, BufferUsage::StaticDraw};
    CORRADE_COMPARE(out.str(), "GL::BufferImage::BufferImage(): data too small, got 9 but expected at least 12 bytes\n");
}

void BufferImageGLTest::constructCompressedInvalidSize() {
    CORRADE_EXPECT_FAIL("Size checking for compressed image data is not implemented yet.");

    /* Too small for given format */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CompressedBufferImage2D{Magnum::CompressedPixelFormat::Bc2RGBAUnorm, {4, 4}, Containers::Array<char>{2}, BufferUsage::StaticDraw};
        CORRADE_COMPARE(out.str(), "GL::CompressedBufferImage::CompressedBufferImage(): data too small, got 2 but expected at least 4 bytes\n");

    /* Size should be rounded up even if the image size is not full block */
    } {
        std::ostringstream out;
        Error redirectError{&out};
        CompressedBufferImage2D{Magnum::CompressedPixelFormat::Bc2RGBAUnorm, {2, 2}, Containers::Array<char>{2}, BufferUsage::StaticDraw};
        CORRADE_COMPARE(out.str(), "GL::CompressedBufferImage::CompressedBufferImage(): data too small, got 2 but expected at least 4 bytes\n");
    }
}

void BufferImageGLTest::constructMove() {
    const char data[4] = { 'a', 'b', 'c', 'd' };
    BufferImage2D a{PixelFormat::Red, PixelType::UnsignedByte, {4, 1}, data, BufferUsage::StaticDraw};
    const Int id = a.buffer().id();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    BufferImage2D b(std::move(a));

    CORRADE_COMPARE(a.buffer().id(), 0);
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_COMPARE(b.storage().alignment(), 4);
    CORRADE_COMPARE(b.format(), GL::PixelFormat::Red);
    CORRADE_COMPARE(b.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(4, 1));
    CORRADE_COMPARE(b.dataSize(), 4);
    CORRADE_COMPARE(b.buffer().id(), id);

    const unsigned short data2[2*4] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    BufferImage2D c{PixelStorage{}.setAlignment(1),
        PixelFormat::RGBA, PixelType::UnsignedShort, {1, 2}, data2, BufferUsage::StaticDraw};
    const Int cId = c.buffer().id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.buffer().id(), cId);
    CORRADE_COMPARE(b.size(), Vector2i(1, 2));

    CORRADE_COMPARE(c.storage().alignment(), 4);
    CORRADE_COMPARE(c.format(), GL::PixelFormat::Red);
    CORRADE_COMPARE(c.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(c.size(), Vector2i(4, 1));
    CORRADE_COMPARE(c.dataSize(), 4);
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

    CompressedBufferImage2D b{std::move(a)};

    CORRADE_COMPARE(a.buffer().id(), 0);
    CORRADE_COMPARE(a.size(), Vector2i());
    CORRADE_COMPARE(a.dataSize(), 0);

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{0});
    #endif
    CORRADE_COMPARE(b.format(), GL::CompressedPixelFormat::RGBAS3tcDxt1);
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
    c = std::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.buffer().id(), cId);
    CORRADE_COMPARE(b.size(), Vector2i(8, 4));
    CORRADE_COMPARE(b.dataSize(), 16);

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(c.storage().compressedBlockSize(), Vector3i{0});
    #endif
    CORRADE_COMPARE(c.format(), GL::CompressedPixelFormat::RGBAS3tcDxt1);
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
    CORRADE_COMPARE(a.format(), GL::PixelFormat::RGBA);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedShort);
    CORRADE_COMPARE(a.size(), Vector2i(1, 2));
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
    CORRADE_COMPARE(a.format(), GL::PixelFormat::RGBA);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedShort);
    CORRADE_COMPARE(a.size(), Vector2i(1, 2));
    CORRADE_COMPARE(a.dataSize(), 16);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(imageData),
        Containers::arrayView(data2),
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
    CORRADE_COMPARE(a.format(), GL::CompressedPixelFormat::RGBAS3tcDxt3);
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
    CORRADE_COMPARE(a.format(), GL::CompressedPixelFormat::RGBAS3tcDxt3);
    CORRADE_COMPARE(a.size(), Vector2i(8, 4));
    CORRADE_COMPARE(a.dataSize(), 16);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::arrayView(data2),
        TestSuite::Compare::Container);
    #endif
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
