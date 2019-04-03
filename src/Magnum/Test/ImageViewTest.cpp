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

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"

namespace Magnum { namespace Test { namespace {

struct ImageViewTest: TestSuite::Tester {
    explicit ImageViewTest();

    void constructGeneric();
    void constructGenericEmpty();
    void constructGenericEmptyNullptr();
    void constructImplementationSpecific();
    void constructImplementationSpecificEmpty();
    void constructImplementationSpecificEmptyNullptr();
    void constructCompressedGeneric();
    void constructCompressedGenericEmpty();
    void constructCompressedImplementationSpecific();
    void constructCompressedImplementationSpecificEmpty();

    void constructInvalidSize();
    void constructCompressedInvalidSize();

    void dataProperties();
    void dataPropertiesCompressed();

    void setData();
    void setDataCompressed();

    void setDataInvalidSize();
    void setDataCompressedInvalidSize();
};

ImageViewTest::ImageViewTest() {
    addTests({&ImageViewTest::constructGeneric,
              &ImageViewTest::constructGenericEmpty,
              &ImageViewTest::constructGenericEmptyNullptr,
              &ImageViewTest::constructImplementationSpecific,
              &ImageViewTest::constructImplementationSpecificEmpty,
              &ImageViewTest::constructImplementationSpecificEmptyNullptr,
              &ImageViewTest::constructCompressedGeneric,
              &ImageViewTest::constructCompressedGenericEmpty,
              &ImageViewTest::constructCompressedImplementationSpecific,
              &ImageViewTest::constructCompressedImplementationSpecificEmpty,

              &ImageViewTest::constructInvalidSize,
              &ImageViewTest::constructCompressedInvalidSize,

              &ImageViewTest::dataProperties,
              &ImageViewTest::dataPropertiesCompressed,

              &ImageViewTest::setData,
              &ImageViewTest::setDataCompressed,

              &ImageViewTest::setDataInvalidSize,
              &ImageViewTest::setDataCompressedInvalidSize});
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

void ImageViewTest::constructGeneric() {
    {
        const char data[4*4]{};
        ImageView2D a{PixelFormat::RGBA8Unorm, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RGBA8Unorm);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 4);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 4*4);
    } {
        const char data[3*2]{};
        ImageView2D a{PixelStorage{}.setAlignment(1),
            PixelFormat::R16UI, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::R16UI);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 2);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 3*2);
    }
}

void ImageViewTest::constructGenericEmpty() {
    {
        ImageView2D a{PixelFormat::RG32F, {2, 6}};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RG32F);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 8);
        CORRADE_COMPARE(a.size(), (Vector2i{2, 6}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        ImageView2D a{PixelStorage{}.setAlignment(1),
            PixelFormat::RGB16F, {8, 3}};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::RGB16F);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{8, 3}));
        CORRADE_COMPARE(a.data(), nullptr);
    }
}

void ImageViewTest::constructGenericEmptyNullptr() {
    /* This should be deprecated/removed, as it doesn't provide anything over
       the above and can lead to silent errors */

    {
        ImageView2D a{PixelFormat::RG32F, {2, 6}, nullptr};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RG32F);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 8);
        CORRADE_COMPARE(a.size(), (Vector2i{2, 6}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        ImageView2D a{PixelStorage{}.setAlignment(1),
            PixelFormat::RGB16F, {8, 3}, nullptr};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::RGB16F);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{8, 3}));
        CORRADE_COMPARE(a.data(), nullptr);
    }
}

void ImageViewTest::constructImplementationSpecific() {
    /* Single format */
    {
        const char data[3*12]{};
        ImageView2D a{Vk::PixelFormat::R32G32B32F, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 3*12);
    } {
        const char data[3*12]{};
        ImageView2D a{PixelStorage{}.setAlignment(1),
            Vk::PixelFormat::R32G32B32F, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 3*12);
    }

    /* Format + extra */
    {
        const char data[3*8]{};
        ImageView2D a{GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 3*8);
    } {
        const char data[3*6]{};
        ImageView2D a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, data};

        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
    }

    /* Manual pixel size */
    {
        const char data[3*6]{};
        ImageView2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
    }
}

void ImageViewTest::constructImplementationSpecificEmpty() {
    /* Single format */
    {
        ImageView2D a{Vk::PixelFormat::R32G32B32F, {2, 16}};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{2, 16}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        ImageView2D a{PixelStorage{}.setAlignment(1),
            Vk::PixelFormat::R32G32B32F, {1, 2}};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 2}));
        CORRADE_COMPARE(a.data(), nullptr);
    }

    /* Format + extra */
    {
        ImageView2D a{GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        ImageView2D a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {8, 2}};

        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{8, 2}));
        CORRADE_COMPARE(a.data(), nullptr);
    }

    /* Manual pixel size */
    {
        ImageView2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {3, 3}};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{3, 3}));
        CORRADE_COMPARE(a.data(), nullptr);
    }
}

void ImageViewTest::constructImplementationSpecificEmptyNullptr() {
    /* This should be deprecated/removed, as it doesn't provide anything over
       the above and can lead to silent errors */

    /* Single format */
    {
        ImageView2D a{Vk::PixelFormat::R32G32B32F, {2, 16}, nullptr};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{2, 16}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        ImageView2D a{PixelStorage{}.setAlignment(1),
            Vk::PixelFormat::R32G32B32F, {1, 2}, nullptr};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 2}));
        CORRADE_COMPARE(a.data(), nullptr);
    }

    /* Format + extra */
    {
        ImageView2D a{GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, nullptr};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        ImageView2D a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {8, 2}, nullptr};

        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{8, 2}));
        CORRADE_COMPARE(a.data(), nullptr);
    }

    /* Manual pixel size */
    {
        ImageView2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {3, 3}, nullptr};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{3, 3}));
        CORRADE_COMPARE(a.data(), nullptr);
    }
}

void ImageViewTest::constructCompressedGeneric() {
    {
        const char data[8]{};
        CompressedImageView2D a{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.format(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 8);
    } {
        const char data[8]{};
        CompressedImageView2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            CompressedPixelFormat::Bc1RGBAUnorm, {4, 4},
            data};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.format(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 8);
    }
}

void ImageViewTest::constructCompressedGenericEmpty() {
    {
        CompressedImageView2D a{CompressedPixelFormat::Bc1RGBAUnorm, {8, 16}};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.format(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{8, 16}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        CompressedImageView2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            CompressedPixelFormat::Bc1RGBAUnorm, {8, 16}};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.format(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{8, 16}));
        CORRADE_COMPARE(a.data(), nullptr);
    }
}

void ImageViewTest::constructCompressedImplementationSpecific() {
    /* Format with autodetection */
    {
        const char data[8]{};
        CompressedImageView2D a{GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4},
            data};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 8);
    } {
        const char data[8]{};
        CompressedImageView2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4}, data};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(a.data(), data);
        CORRADE_COMPARE(a.data().size(), 8);
    }

    /* Manual properties not implemented yet */
}

void ImageViewTest::constructCompressedImplementationSpecificEmpty() {
    /* Format with autodetection */
    {
        CompressedImageView2D a{GL::CompressedPixelFormat::RGBS3tcDxt1, {8, 16}};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{8, 16}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        CompressedImageView2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 8}};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{4, 8}));
        CORRADE_COMPARE(a.data(), nullptr);
    }

    /* Manual properties not implemented yet */
}

void ImageViewTest::constructInvalidSize() {
    std::ostringstream out;
    Error redirectError{&out};

    /* Doesn't consider alignment */
    const char data[3*3]{};
    ImageView2D{PixelFormat::RGB8Unorm, {1, 3}, data};
    CORRADE_COMPARE(out.str(), "ImageView::ImageView(): data too small, got 9 but expected at least 12 bytes\n");
}

void ImageViewTest::constructCompressedInvalidSize() {
    CORRADE_EXPECT_FAIL("Size checking for compressed image data is not implemented yet.");

    const char data[2]{};

    /* Too small for given format */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CompressedImageView2D{CompressedPixelFormat::Bc2RGBAUnorm, {4, 4}, data};
        CORRADE_COMPARE(out.str(), "CompressedImageView::CompressedImageView(): data too small, got 2 but expected at least 4 bytes\n");

    /* Size should be rounded up even if the image size is not full block */
    } {
        std::ostringstream out;
        Error redirectError{&out};
        CompressedImageView2D{CompressedPixelFormat::Bc2RGBAUnorm, {2, 2}, data};
        CORRADE_COMPARE(out.str(), "CompressedImageView::CompressedImageView(): data too small, got 2 but expected at least 4 bytes\n");
    }
}

void ImageViewTest::dataProperties() {
    const char data[224]{};
    ImageView3D image{
        PixelStorage{}
            .setAlignment(8)
            .setSkip({3, 2, 1}),
        PixelFormat::R8Unorm, {2, 4, 6}, data};
    CORRADE_COMPARE(image.dataProperties(),
        (std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>>{{3, 16, 32}, {8, 4, 6}}));
}

void ImageViewTest::dataPropertiesCompressed() {
    /* Yes, I know, this is totally bogus and doesn't match the BC1 format */
    const char data[1]{};
    CompressedImageView3D image{
        CompressedPixelStorage{}
            .setCompressedBlockSize({3, 4, 5})
            .setCompressedBlockDataSize(16)
            .setImageHeight(12)
            .setSkip({5, 8, 11}),
        CompressedPixelFormat::Bc1RGBAUnorm, {2, 8, 11},
        data};
    CORRADE_COMPARE(image.dataProperties(),
        (std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>>{{2*16, 2*16, 9*16}, {1, 3, 3}}));
}

void ImageViewTest::setData() {
    const char data[3*3]{};
    ImageView2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGB8Snorm, {1, 3}, data};
    const char data2[3*3]{};
    a.setData(data2);

    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), PixelFormat::RGB8Snorm);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), data2);
}

void ImageViewTest::setDataCompressed() {
    const char data[8]{};
    CompressedImageView2D a{
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data};
    const char data2[16]{};
    a.setData(data2);

    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::Bc1RGBAUnorm);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.data(), data2);
}

void ImageViewTest::setDataInvalidSize() {
    std::ostringstream out;
    Error redirectError{&out};

    ImageView2D image{PixelFormat::RGB8Unorm, {1, 3}};
    const char data[3*3]{};

    /* Doesn't consider alignment */
    image.setData(data);
    CORRADE_COMPARE(out.str(), "ImageView::setData(): data too small, got 9 but expected at least 12 bytes\n");
}

void ImageViewTest::setDataCompressedInvalidSize() {
    CORRADE_EXPECT_FAIL("Size checking for compressed image data is not implemented yet.");

    const char data[2]{};

    /* Too small for given format */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CompressedImageView2D{CompressedPixelFormat::Bc2RGBAUnorm, {4, 4}, data};
        CORRADE_COMPARE(out.str(), "CompressedImageView::setData(): data too small, got 2 but expected at least 4 bytes\n");

    /* Size should be rounded up even if the image size is not that big */
    } {
        std::ostringstream out;
        Error redirectError{&out};
        CompressedImageView2D{CompressedPixelFormat::Bc2RGBAUnorm, {2, 2}, data};
        CORRADE_COMPARE(out.str(), "CompressedImageView::setData(): data too small, got 2 but expected at least 4 bytes\n");
    }
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::ImageViewTest)
