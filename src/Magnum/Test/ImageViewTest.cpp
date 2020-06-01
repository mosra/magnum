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

/* Included as first to check that we *really* don't need the StridedArrayView
   header for definition of pixels(). We actually need, but just for the
   arrayCast() template, which is forward-declared. */
#include "Magnum/ImageView.h"

#include <sstream>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"

namespace Magnum { namespace Test { namespace {

struct ImageViewTest: TestSuite::Tester {
    explicit ImageViewTest();

    template<class T> void constructGeneric();
    template<class T> void constructGenericEmpty();
    template<class T> void constructImplementationSpecific();
    template<class T> void constructImplementationSpecificEmpty();
    template<class T> void constructCompressedGeneric();
    template<class T> void constructCompressedGenericEmpty();
    template<class T> void constructCompressedImplementationSpecific();
    template<class T> void constructCompressedImplementationSpecificEmpty();

    void construct3DFrom1D();
    void construct3DFrom2D();
    void constructCompressed3DFrom1D();
    void constructCompressed3DFrom2D();

    void constructFromMutable();
    void constructCompressedFromMutable();

    void constructNullptr();
    void constructInvalidSize();
    void constructCompressedInvalidSize();

    void dataProperties();
    void dataPropertiesCompressed();

    template<class T> void setData();
    template<class T> void setDataCompressed();

    void setDataInvalidSize();
    void setDataCompressedInvalidSize();

    template<class T> void pixels1D();
    template<class T> void pixels2D();
    template<class T> void pixels3D();
    void pixelsNullptr();
};

template<class> struct MutabilityTraits;
template<> struct MutabilityTraits<const char> {
    static const char* name() { return "ImageView"; }
};
template<> struct MutabilityTraits<char> {
    static const char* name() { return "MutableImageView"; }
};

ImageViewTest::ImageViewTest() {
    addTests({&ImageViewTest::constructGeneric<const char>,
              &ImageViewTest::constructGeneric<char>,
              &ImageViewTest::constructGenericEmpty<const char>,
              &ImageViewTest::constructGenericEmpty<char>,
              &ImageViewTest::constructImplementationSpecific<const char>,
              &ImageViewTest::constructImplementationSpecific<char>,
              &ImageViewTest::constructImplementationSpecificEmpty<const char>,
              &ImageViewTest::constructImplementationSpecificEmpty<char>,
              &ImageViewTest::constructCompressedGeneric<const char>,
              &ImageViewTest::constructCompressedGeneric<char>,
              &ImageViewTest::constructCompressedGenericEmpty<const char>,
              &ImageViewTest::constructCompressedGenericEmpty<char>,
              &ImageViewTest::constructCompressedImplementationSpecific<const char>,
              &ImageViewTest::constructCompressedImplementationSpecific<char>,
              &ImageViewTest::constructCompressedImplementationSpecificEmpty<const char>,
              &ImageViewTest::constructCompressedImplementationSpecificEmpty<char>,

              &ImageViewTest::construct3DFrom1D,
              &ImageViewTest::construct3DFrom2D,
              &ImageViewTest::constructCompressed3DFrom1D,
              &ImageViewTest::constructCompressed3DFrom2D,

              &ImageViewTest::constructFromMutable,
              &ImageViewTest::constructCompressedFromMutable,

              &ImageViewTest::constructNullptr,
              &ImageViewTest::constructInvalidSize,
              &ImageViewTest::constructCompressedInvalidSize,

              &ImageViewTest::dataProperties,
              &ImageViewTest::dataPropertiesCompressed,

              &ImageViewTest::setData<const char>,
              &ImageViewTest::setData<char>,
              &ImageViewTest::setDataCompressed<const char>,
              &ImageViewTest::setDataCompressed<char>,

              &ImageViewTest::setDataInvalidSize,
              &ImageViewTest::setDataCompressedInvalidSize,

              &ImageViewTest::pixels1D<const char>,
              &ImageViewTest::pixels1D<char>,
              &ImageViewTest::pixels2D<const char>,
              &ImageViewTest::pixels2D<char>,
              &ImageViewTest::pixels3D<const char>,
              &ImageViewTest::pixels3D<char>,
              &ImageViewTest::pixelsNullptr});
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
        #ifdef CORRADE_NO_ASSERT
        static_cast<void>(format);
        static_cast<void>(type);
        #endif
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
        #ifdef CORRADE_NO_ASSERT
        static_cast<void>(format);
        #endif
        return 12;
    }

    enum class CompressedPixelFormat { Bc1SRGBAlpha = 42 };
}

template<class T> void ImageViewTest::constructGeneric() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    {
        T data[4*4]{};
        ImageView<2, T> a{PixelFormat::RGBA8Unorm, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RGBA8Unorm);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 4);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 4*4);
    } {
        T data[3*2]{};
        ImageView<2, T> a{PixelStorage{}.setAlignment(1),
            PixelFormat::R16UI, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::R16UI);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 2);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 3*2);
    }
}

template<class T> void ImageViewTest::constructGenericEmpty() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    {
        ImageView<2, T> a{PixelFormat::RG32F, {2, 6}};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RG32F);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 8);
        CORRADE_COMPARE(a.size(), (Vector2i{2, 6}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        ImageView<2, T> a{PixelStorage{}.setAlignment(1),
            PixelFormat::RGB16F, {8, 3}};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::RGB16F);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{8, 3}));
        CORRADE_COMPARE(a.data(), nullptr);
    }
}

template<class T> void ImageViewTest::constructImplementationSpecific() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    /* Single format */
    {
        T data[3*12]{};
        ImageView<2, T> a{Vk::PixelFormat::R32G32B32F, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 3*12);
    } {
        T data[3*12]{};
        ImageView<2, T> a{PixelStorage{}.setAlignment(1),
            Vk::PixelFormat::R32G32B32F, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 3*12);
    }

    /* Format + extra */
    {
        T data[3*8]{};
        ImageView<2, T> a{GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 3*8);
    } {
        T data[3*6]{};
        ImageView<2, T> a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, data};

        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 3*6);
    }

    /* Manual pixel size */
    {
        T data[3*6]{};
        ImageView<2, T> a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {1, 3}, data};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), Vector2i(1, 3));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 3*6);
    }
}

template<class T> void ImageViewTest::constructImplementationSpecificEmpty() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    /* Single format */
    {
        ImageView<2, T> a{Vk::PixelFormat::R32G32B32F, {2, 16}};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{2, 16}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        ImageView<2, T> a{PixelStorage{}.setAlignment(1),
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
        ImageView<2, T> a{GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}};

        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        ImageView<2, T> a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {8, 2}};

        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{8, 2}));
        CORRADE_COMPARE(a.data(), nullptr);
    }

    /* Manual pixel size */
    {
        ImageView<2, T> a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {3, 3}};

        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{3, 3}));
        CORRADE_COMPARE(a.data(), nullptr);
    }
}

template<class T> void ImageViewTest::constructCompressedGeneric() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    {
        T data[8]{};
        CompressedImageView<2, T> a{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.format(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 8);
    } {
        T data[8]{};
        CompressedImageView<2, T> a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            CompressedPixelFormat::Bc1RGBAUnorm, {4, 4},
            data};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.format(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 8);
    }
}

template<class T> void ImageViewTest::constructCompressedGenericEmpty() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    {
        CompressedImageView<2, T> a{CompressedPixelFormat::Bc1RGBAUnorm, {8, 16}};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.format(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{8, 16}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        CompressedImageView<2, T> a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            CompressedPixelFormat::Bc1RGBAUnorm, {8, 16}};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.format(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{8, 16}));
        CORRADE_COMPARE(a.data(), nullptr);
    }
}

template<class T> void ImageViewTest::constructCompressedImplementationSpecific() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    /* Format with autodetection */
    {
        T data[8]{};
        CompressedImageView<2, T> a{GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4},
            data};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 8);
    } {
        T data[8]{};
        CompressedImageView<2, T> a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4}, data};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(a.data(), &data[0]);
        CORRADE_COMPARE(a.data().size(), 8);
    }

    /* Manual properties not implemented yet */
}

template<class T> void ImageViewTest::constructCompressedImplementationSpecificEmpty() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    /* Format with autodetection */
    {
        CompressedImageView<2, T> a{GL::CompressedPixelFormat::RGBS3tcDxt1, {8, 16}};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{8, 16}));
        CORRADE_COMPARE(a.data(), nullptr);
    } {
        CompressedImageView<2, T> a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 8}};

        CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{4, 8}));
        CORRADE_COMPARE(a.data(), nullptr);
    }

    /* Manual properties not implemented yet */
}

void ImageViewTest::construct3DFrom1D() {
    /* Copy of "Manual pixel size" in constructImplementationSpecific(), as
       that exposes most fields */
    const char data[3*6]{};
    ImageView1D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, 3, data};
    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
    CORRADE_COMPARE(a.pixelSize(), 6);
    CORRADE_COMPARE(a.size(), 3);
    CORRADE_COMPARE(a.data(), &data[0]);
    CORRADE_COMPARE(a.data().size(), 3*6);

    ImageView3D b = a; /* implicit conversion allowed */
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(b.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
    CORRADE_COMPARE(b.pixelSize(), 6);
    CORRADE_COMPARE(b.size(), (Vector3i{3, 1, 1}));
    CORRADE_COMPARE(b.data(), &data[0]);
    CORRADE_COMPARE(b.data().size(), 3*6);

    /* Conversion the other way is not allowed (will be later, but explicitly
       via a slice<1>() like with StridedArrayView); conversion from const to
       mutable is not possible either */
    CORRADE_VERIFY((std::is_convertible<ImageView1D, ImageView3D>::value));
    CORRADE_VERIFY(!(std::is_convertible<ImageView3D, ImageView1D>::value));
    CORRADE_VERIFY(!(std::is_convertible<ImageView1D, MutableImageView3D>::value));
}

void ImageViewTest::construct3DFrom2D() {
    /* Copy of "Manual pixel size" in constructImplementationSpecific(), as
       that exposes most fields */
    char data[3*6]{};
    MutableImageView2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {1, 3}, data};
    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
    CORRADE_COMPARE(a.pixelSize(), 6);
    CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(a.data(), &data[0]);
    CORRADE_COMPARE(a.data().size(), 3*6);

    MutableImageView3D b = a;
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(b.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
    CORRADE_COMPARE(b.pixelSize(), 6);
    CORRADE_COMPARE(b.size(), (Vector3i{1, 3, 1}));
    CORRADE_COMPARE(b.data(), &data[0]);
    CORRADE_COMPARE(b.data().size(), 3*6);

    /* Conversion the other way is not allowed (will be later, but explicitly
       via a slice<1>() like with StridedArrayView) */
    CORRADE_VERIFY((std::is_convertible<MutableImageView1D, MutableImageView3D>::value));
    CORRADE_VERIFY(!(std::is_convertible<MutableImageView3D, MutableImageView1D>::value));
}

void ImageViewTest::constructCompressed3DFrom1D() {
    /* Copied from constructCompressedImplementationSpecific(), as that exposes
       most fields */
    /** @todo S3TC doesn't have 1D compression so this might blow up once we
        check for block sizes */
    const char data[8]{};
    CompressedImageView1D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        GL::CompressedPixelFormat::RGBS3tcDxt1, 4, data};
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(a.size(), 4);
    CORRADE_COMPARE(a.data(), &data[0]);
    CORRADE_COMPARE(a.data().size(), 8);

    CompressedImageView3D b = a;
    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(b.size(), (Vector3i{4, 1, 1}));
    CORRADE_COMPARE(b.data(), &data[0]);
    CORRADE_COMPARE(b.data().size(), 8);

    /* Conversion the other way is not allowed (will be later, but explicitly
       via a slice<1>() like with StridedArrayView); conversion from const to
       mutable is not possible either */
    CORRADE_VERIFY((std::is_convertible<CompressedImageView1D, CompressedImageView3D>::value));
    CORRADE_VERIFY(!(std::is_convertible<CompressedImageView3D, CompressedImageView1D>::value));
    CORRADE_VERIFY(!(std::is_convertible<CompressedImageView1D, MutableCompressedImageView3D>::value));
}

void ImageViewTest::constructCompressed3DFrom2D() {
    /* Copied from constructCompressedImplementationSpecific(), as that exposes
       most fields */
    char data[8*2]{};
    MutableCompressedImageView2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 8}, data};
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(a.size(), (Vector2i{4, 8}));
    CORRADE_COMPARE(a.data(), &data[0]);
    CORRADE_COMPARE(a.data().size(), 8*2);

    MutableCompressedImageView3D b = a;
    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(b.size(), (Vector3i{4, 8, 1}));
    CORRADE_COMPARE(b.data(), &data[0]);
    CORRADE_COMPARE(b.data().size(), 8*2);

    /* Conversion the other way is not allowed (will be later, but explicitly
       via a slice<1>() like with StridedArrayView) */
    CORRADE_VERIFY((std::is_convertible<MutableCompressedImageView1D, MutableCompressedImageView3D>::value));
    CORRADE_VERIFY(!(std::is_convertible<MutableCompressedImageView3D, MutableCompressedImageView1D>::value));
}

void ImageViewTest::constructFromMutable() {
    /* Copy of "Manual pixel size" in constructImplementationSpecific(), as
       that exposes most fields */
    char data[3*6]{};
    MutableImageView2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {1, 3}, data};
    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
    CORRADE_COMPARE(a.pixelSize(), 6);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), &data[0]);
    CORRADE_COMPARE(a.data().size(), 3*6);

    ImageView2D b = a;
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(b.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
    CORRADE_COMPARE(b.pixelSize(), 6);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), &data[0]);
    CORRADE_COMPARE(b.data().size(), 3*6);
}

void ImageViewTest::constructCompressedFromMutable() {
    /* Copied from constructCompressedImplementationSpecific(), as that exposes
       most fields */
    char data[8]{};
    MutableCompressedImageView2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4}, data};
    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(a.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(a.data(), &data[0]);
    CORRADE_COMPARE(a.data().size(), 8);

    CompressedImageView2D b = a;
    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(b.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(b.data(), &data[0]);
    CORRADE_COMPARE(b.data().size(), 8);
}

void ImageViewTest::constructNullptr() {
    #ifdef CORRADE_BUILD_DEPRECATED
    CORRADE_SKIP("This is still allowed on a deprecated build, can't test.");
    #endif

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    ImageView2D{PixelFormat::RGB8Unorm, {1, 3},  nullptr};
    CORRADE_COMPARE(out.str(), "ImageView: data too small, got 0 but expected at least 12 bytes\n");
}

void ImageViewTest::constructInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* Doesn't consider alignment */
    const char data[3*3]{};
    ImageView2D{PixelFormat::RGB8Unorm, {1, 3}, data};
    CORRADE_COMPARE(out.str(), "ImageView: data too small, got 9 but expected at least 12 bytes\n");
}

void ImageViewTest::constructCompressedInvalidSize() {
    CORRADE_EXPECT_FAIL("Size checking for compressed image data is not implemented yet.");

    const char data[2]{};

    /* Too small for given format */
    {
        std::ostringstream out;
        Error redirectError{&out};
        CompressedImageView2D{CompressedPixelFormat::Bc2RGBAUnorm, {4, 4}, data};
        CORRADE_COMPARE(out.str(), "CompressedImageView: data too small, got 2 but expected at least 4 bytes\n");

    /* Size should be rounded up even if the image size is not full block */
    } {
        std::ostringstream out;
        Error redirectError{&out};
        CompressedImageView2D{CompressedPixelFormat::Bc2RGBAUnorm, {2, 2}, data};
        CORRADE_COMPARE(out.str(), "CompressedImageView: data too small, got 2 but expected at least 4 bytes\n");
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

template<class T> void ImageViewTest::setData() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    T data[3*3]{};
    ImageView<2, T> a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGB8Snorm, {1, 3}, data};
    T data2[3*3]{};
    a.setData(data2);

    CORRADE_COMPARE(a.storage().alignment(), 1);
    CORRADE_COMPARE(a.format(), PixelFormat::RGB8Snorm);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));
    CORRADE_COMPARE(a.data(), &data2[0]);
}

template<class T> void ImageViewTest::setDataCompressed() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    T data[8]{};
    CompressedImageView<2, T> a{
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, data};
    T data2[16]{};
    a.setData(data2);

    CORRADE_COMPARE(a.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(a.format(), CompressedPixelFormat::Bc1RGBAUnorm);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.data(), &data2[0]);
}

void ImageViewTest::setDataInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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

template<class T> void ImageViewTest::pixels1D() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    ImageView<1, T> image{
        PixelStorage{}
            .setAlignment(1) /** @todo alignment 4 expects 17 bytes. what */
            .setSkip({3, 0, 0}),
        PixelFormat::RGB8Unorm, 2,
        {nullptr, 15}};

    /* Full test is in ImageTest, this is just a sanity check */

    auto pixels = image.template pixels<Color3ub>();
    CORRADE_COMPARE(decltype(pixels)::Dimensions, 1);
    CORRADE_COMPARE(std::is_const<typename decltype(pixels)::Type>::value, std::is_const<T>::value);
    CORRADE_COMPARE(pixels.size(), 2);
    CORRADE_COMPARE(pixels.stride(), 3);
    CORRADE_COMPARE(pixels.data(), image.data() + 3*3);
}

template<class T> void ImageViewTest::pixels2D() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    ImageView<2, T> image{
        PixelStorage{}
            .setAlignment(4)
            .setSkip({3, 2, 0})
            .setRowLength(6),
        PixelFormat::RGB8Unorm, {2, 4},
        {nullptr, 120}};

    /* Full test is in ImageTest, this is just a sanity check */

    auto pixels = image.template pixels<Color3ub>();
    CORRADE_COMPARE(decltype(pixels)::Dimensions, 2);
    CORRADE_COMPARE(std::is_const<typename decltype(pixels)::Type>::value, std::is_const<T>::value);
    CORRADE_COMPARE(pixels.size(), (Containers::StridedArrayView2D<Color3ub>::Size{4, 2}));
    CORRADE_COMPARE(pixels.stride(), (Containers::StridedArrayView2D<Color3ub>::Stride{20, 3}));
    CORRADE_COMPARE(pixels.data(), image.data() + 2*20 + 3*3);
}

template<class T> void ImageViewTest::pixels3D() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    ImageView<3, T> image{
        PixelStorage{}
            .setAlignment(4)
            .setSkip({3, 2, 1})
            .setRowLength(6)
            .setImageHeight(7),
        PixelFormat::RGB8Unorm, {2, 4, 3},
        {nullptr, 560}};

    /* Full test is in ImageTest, this is just a sanity check */

    auto pixels = image.template pixels<Color3ub>();
    CORRADE_COMPARE(decltype(pixels)::Dimensions, 3);
    CORRADE_COMPARE(std::is_const<typename decltype(pixels)::Type>::value, std::is_const<T>::value);
    CORRADE_COMPARE(pixels.size(), (Containers::StridedArrayView3D<Color3ub>::Size{3, 4, 2}));
    CORRADE_COMPARE(pixels.stride(), (Containers::StridedArrayView3D<Color3ub>::Stride{140, 20, 3}));
    CORRADE_COMPARE(pixels.data(), image.data() + 140 + 2*20 + 3*3);
}

void ImageViewTest::pixelsNullptr() {
    ImageView3D image{PixelFormat::RGB8Unorm, {2, 4, 3}};

    CORRADE_COMPARE(image.data(), nullptr);
    CORRADE_COMPARE(image.data().size(), 0);

    CORRADE_COMPARE(image.pixels().data(), nullptr);
    CORRADE_COMPARE(image.pixels().size(),
        Containers::StridedArrayView4D<const char>::Size{});

    CORRADE_COMPARE(image.pixels<Color3ub>().data(), nullptr);
    CORRADE_COMPARE(image.pixels<Color3ub>().size(),
        Containers::StridedArrayView3D<Color3ub>::Size{});
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::ImageViewTest)
