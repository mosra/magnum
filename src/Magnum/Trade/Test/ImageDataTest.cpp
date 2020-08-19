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
#include "Magnum/Trade/ImageData.h"

#include <sstream>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct ImageDataTest: TestSuite::Tester {
    explicit ImageDataTest();

    void constructGeneric();
    void constructImplementationSpecific();
    void constructCompressedGeneric();
    void constructCompressedImplementationSpecific();

    void constructGenericNotOwned();
    void constructImplementationSpecificNotOwned();
    void constructCompressedGenericNotOwned();
    void constructCompressedImplementationSpecificNotOwned();
    void constructGenericNotOwnedFlagOwned();
    void constructImplementationSpecificNotOwnedFlagOwned();
    void constructCompressedGenericNotOwnedFlagOwned();
    void constructCompressedImplementationSpecificNotOwnedFlagOwned();

    void constructInvalidSize();
    void constructCompressedInvalidSize();

    void constructCopy();

    void constructMoveGeneric();
    void constructMoveImplementationSpecific();
    void constructMoveCompressedGeneric();
    void constructMoveCompressedImplementationSpecific();

    void constructMoveAttachState();
    void constructMoveCompressedAttachState();

    template<class T> void toViewGeneric();
    template<class T> void toViewImplementationSpecific();
    template<class T> void toViewCompressedGeneric();
    template<class T> void toViewCompressedImplementationSpecific();

    void data();
    void dataRvalue();
    void mutableAccessNotAllowed();

    void dataProperties();

    void release();
    void releaseCompressed();

    void pixels1D();
    void pixels2D();
    void pixels3D();
    void pixelsCompressed();
};

template<class> struct MutabilityTraits;
template<> struct MutabilityTraits<const char> {
    typedef const ImageData2D ImageType;

    static const char* name() { return "ImageView"; }
};
template<> struct MutabilityTraits<char> {
    typedef ImageData2D ImageType;

    static const char* name() { return "MutableImageView"; }
};

struct {
    const char* name;
    DataFlags dataFlags;
} NotOwnedData[] {
    {"", {}},
    {"mutable", DataFlag::Mutable},
};

ImageDataTest::ImageDataTest() {
    addTests({&ImageDataTest::constructGeneric,
              &ImageDataTest::constructImplementationSpecific,
              &ImageDataTest::constructCompressedGeneric,
              &ImageDataTest::constructCompressedImplementationSpecific});

    addInstancedTests({&ImageDataTest::constructGenericNotOwned,
                       &ImageDataTest::constructImplementationSpecificNotOwned,
                       &ImageDataTest::constructCompressedGenericNotOwned,
                       &ImageDataTest::constructCompressedImplementationSpecificNotOwned},
        Containers::arraySize(NotOwnedData));

    addTests({&ImageDataTest::constructGenericNotOwnedFlagOwned,
              &ImageDataTest::constructImplementationSpecificNotOwnedFlagOwned,
              &ImageDataTest::constructCompressedGenericNotOwnedFlagOwned,
              &ImageDataTest::constructCompressedImplementationSpecificNotOwnedFlagOwned,

              &ImageDataTest::constructInvalidSize,
              &ImageDataTest::constructCompressedInvalidSize,

              &ImageDataTest::constructCopy,

              &ImageDataTest::constructMoveGeneric,
              &ImageDataTest::constructMoveImplementationSpecific,
              &ImageDataTest::constructMoveCompressedGeneric,
              &ImageDataTest::constructMoveCompressedImplementationSpecific,

              &ImageDataTest::constructMoveAttachState,
              &ImageDataTest::constructMoveCompressedAttachState,

              &ImageDataTest::toViewGeneric<const char>,
              &ImageDataTest::toViewGeneric<char>,
              &ImageDataTest::toViewImplementationSpecific<const char>,
              &ImageDataTest::toViewImplementationSpecific<char>,
              &ImageDataTest::toViewCompressedGeneric<const char>,
              &ImageDataTest::toViewCompressedGeneric<char>,
              &ImageDataTest::toViewCompressedImplementationSpecific<const char>,
              &ImageDataTest::toViewCompressedImplementationSpecific<char>,

              &ImageDataTest::data,
              &ImageDataTest::dataRvalue,
              &ImageDataTest::mutableAccessNotAllowed,

              &ImageDataTest::dataProperties,

              &ImageDataTest::release,
              &ImageDataTest::releaseCompressed,

              &ImageDataTest::pixels1D,
              &ImageDataTest::pixels2D,
              &ImageDataTest::pixels3D,
              &ImageDataTest::pixelsCompressed});
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

void ImageDataTest::constructGeneric() {
    {
        auto data = new char[4*4];
        int state;
        ImageData2D a{PixelFormat::RGBA8Unorm, {1, 3}, Containers::Array<char>{data, 4*4}, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RGBA8Unorm);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 4);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 4*4);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Color4ub>()[0][0]), data);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 4*4);
        CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Color4ub>()[0][0]), data);
        CORRADE_COMPARE(a.importerState(), &state);
    } {
        auto data = new char[3*2];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            PixelFormat::R16UI, {1, 3}, Containers::Array<char>{data, 3*2}, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::R16UI);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 2);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*2);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<UnsignedShort>()[0][0]), data);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 3*2);
        CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<UnsignedShort>()[0][0]), data);
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

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*12);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Color3>()[0][0]), data);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 3*12);
        CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Color3>()[0][0]), data);
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Format + extra */
    {
        auto data = new char[3*6];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Manual pixel size */
    {
        auto data = new char[3*6];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {1, 3}, Containers::Array<char>{data, 3*6}, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructCompressedGeneric() {
    {
        auto data = new char[8];
        int state;
        ImageData2D a{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4},
            Containers::Array<char>{data, 8}, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 8);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 8);
        CORRADE_COMPARE(a.importerState(), &state);
    } {
        auto data = new char[8];
        int state;
        ImageData2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            CompressedPixelFormat::Bc1RGBAUnorm, {4, 4},
            Containers::Array<char>{data, 8}, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), Vector2i(4, 4));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 8);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 8);
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

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.compressedFormat(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 8);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 8);
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Manual properties not implemented yet */
}

void ImageDataTest::constructGenericNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    {
        char data[4*4];
        int state;
        ImageData2D a{PixelFormat::RGBA8Unorm, {1, 3}, instanceData.dataFlags, data, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RGBA8Unorm);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 4);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 4*4);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Color4ub>()[0][0]), data);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 4*4);
            CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Color4ub>()[0][0]), data);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    } {
        char data[3*2];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            PixelFormat::R16UI, {1, 3}, instanceData.dataFlags, data, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::R16UI);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 2);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*2);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<UnsignedShort>()[0][0]), data);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 3*2);
            CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<UnsignedShort>()[0][0]), data);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructImplementationSpecificNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    /* Single format */
    {
        char data[3*12];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            Vk::PixelFormat::R32G32B32F, {1, 3}, instanceData.dataFlags, data, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*12);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Color3>()[0][0]), data);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 3*12);
            CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Color3>()[0][0]), data);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Format + extra */
    {
        char data[3*6];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, instanceData.dataFlags, data, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 3*6);
            CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Manual pixel size */
    {
        char data[3*6];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {1, 3}, instanceData.dataFlags, data, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 3*6);
            CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructCompressedGenericNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    {
        char data[8];
        int state;
        ImageData2D a{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4},
            instanceData.dataFlags, data, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), Vector3i{0});
        CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 8);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 8);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    } {
        char data[8];
        int state;
        ImageData2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            CompressedPixelFormat::Bc1RGBAUnorm, {4, 4},
            instanceData.dataFlags, data, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.size(), Vector2i(4, 4));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 8);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 8);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructCompressedImplementationSpecificNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    /* Format with autodetection */
    {
        char data[8];
        int state;
        ImageData2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
            GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4},
            instanceData.dataFlags, data, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), Vector3i{4});
        CORRADE_COMPARE(a.compressedFormat(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 8);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 8);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Manual properties not implemented yet */
}

void ImageDataTest::constructGenericNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char data[4*4];

    std::ostringstream out;
    Error redirectError{&out};
    ImageData2D{PixelFormat::RGBA8Unorm, {1, 3}, DataFlag::Owned, data};
    ImageData2D{PixelStorage{}.setAlignment(1), PixelFormat::R16UI, {1, 3}, DataFlag::Owned, data};
    CORRADE_COMPARE(out.str(),
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n"
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void ImageDataTest::constructImplementationSpecificNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char data[3*12];

    std::ostringstream out;
    Error redirectError{&out};
    ImageData2D{PixelStorage{}.setAlignment(1), Vk::PixelFormat::R32G32B32F, {1, 3}, DataFlag::Owned, data};
    ImageData2D{PixelStorage{}.setAlignment(1), GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, DataFlag::Owned, data};
    CORRADE_COMPARE(out.str(),
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n"
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void ImageDataTest::constructCompressedGenericNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char data[8];

    std::ostringstream out;
    Error redirectError{&out};
    ImageData2D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, DataFlag::Owned, data};
    ImageData2D{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, DataFlag::Owned, data};
    CORRADE_COMPARE(out.str(),
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n"
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void ImageDataTest::constructCompressedImplementationSpecificNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char data[8];

    std::ostringstream out;
    Error redirectError{&out};
    ImageData2D a{CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4}, DataFlag::Owned, data};
    CORRADE_COMPARE(out.str(),
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void ImageDataTest::constructInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* Doesn't consider alignment */
    ImageData2D{PixelFormat::RGB8Unorm, {1, 3}, Containers::Array<char>{3*3}};
    CORRADE_COMPARE(out.str(), "Trade::ImageData: data too small, got 9 but expected at least 12 bytes\n");
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
    CORRADE_VERIFY(!(std::is_constructible<ImageData2D, const ImageData2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<ImageData2D, const ImageData2D&>{}));
}

void ImageDataTest::constructMoveGeneric() {
    auto data = new char[3*16];
    int state;
    ImageData2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGBA32F, {1, 3}, Containers::Array<char>{data, 3*16}, &state};
    ImageData2D b(std::move(a));

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
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

    CORRADE_COMPARE(c.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!c.isCompressed());
    CORRADE_COMPARE(c.storage().alignment(), 1);
    CORRADE_COMPARE(c.format(), PixelFormat::RGBA32F);
    CORRADE_COMPARE(c.formatExtra(), 0);
    CORRADE_COMPARE(c.pixelSize(), 16);
    CORRADE_COMPARE(c.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(c.data(), data);
    CORRADE_COMPARE(c.data().size(), 3*16);
    CORRADE_COMPARE(c.importerState(), &state);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<ImageData2D>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<ImageData2D>::value);
}

void ImageDataTest::constructMoveImplementationSpecific() {
    auto data = new char[3*6];
    int state;
    ImageData2D a{PixelStorage{}.setAlignment(1),
        GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}, &state};
    ImageData2D b(std::move(a));

    CORRADE_COMPARE(a.data(), nullptr);
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
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

    CORRADE_COMPARE(c.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
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

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
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

    CORRADE_COMPARE(c.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
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

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
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

    CORRADE_COMPARE(c.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
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

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
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

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(b.isCompressed());
    CORRADE_COMPARE(b.compressedStorage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.compressedFormat(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(b.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);
    CORRADE_COMPARE(b.importerState(), &stateNew);
}

template<class T> void ImageDataTest::toViewGeneric() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    auto data = new char[3*4];
    typename MutabilityTraits<T>::ImageType a{PixelStorage{}.setAlignment(1),
        PixelFormat::RG16I, {1, 3}, Containers::Array<char>{data, 3*4}};
    ImageView<2, T> b = a;

    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), PixelFormat::RG16I);
    CORRADE_COMPARE(b.formatExtra(), 0);
    CORRADE_COMPARE(b.pixelSize(), 4);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), data);
}

template<class T> void ImageDataTest::toViewImplementationSpecific() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    auto data = new char[3*6];
    typename MutabilityTraits<T>::ImageType a{PixelStorage{}.setAlignment(1),
        GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}};
    ImageView<2, T> b = a;

    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(b.formatExtra(), 1337);
    CORRADE_COMPARE(b.pixelSize(), 6);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), data);
}

template<class T> void ImageDataTest::toViewCompressedGeneric() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    auto data = new char[8];
    typename MutabilityTraits<T>::ImageType a{
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        CompressedPixelFormat::Bc1RGBUnorm, {4, 4}, Containers::Array<char>{data, 8}};
    CompressedImageView<2, T> b = a;

    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.format(), CompressedPixelFormat::Bc1RGBUnorm);
    CORRADE_COMPARE(b.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);
}

template<class T> void ImageDataTest::toViewCompressedImplementationSpecific() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    auto data = new char[8];
    typename MutabilityTraits<T>::ImageType a{
        CompressedPixelStorage{}.setCompressedBlockSize(Vector3i{4}),
        GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4}, Containers::Array<char>{data, 8}};
    CompressedImageView<2, T> b = a;

    CORRADE_COMPARE(b.storage().compressedBlockSize(), Vector3i{4});
    CORRADE_COMPARE(b.format(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(b.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(b.data(), data);
    CORRADE_COMPARE(b.data().size(), 8);
}

void ImageDataTest::data() {
    auto data = new char[4*4];
    ImageData2D a{PixelFormat::RGBA8Unorm, {1, 3}, Containers::Array<char>{data, 4*4}};
    const ImageData2D& ca = a;
    CORRADE_COMPARE(a.data(), data);
    CORRADE_COMPARE(ca.data(), data);
}

void ImageDataTest::dataRvalue() {
    auto data = new char[4*4];
    Containers::Array<char> released = ImageData2D{PixelFormat::RGBA8Unorm,
        {1, 3}, Containers::Array<char>{data, 4*4}}.data();
    CORRADE_COMPARE(released.data(), data);
}

void ImageDataTest::mutableAccessNotAllowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const char data[4*4]{};
    ImageData2D a{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, data};

    std::ostringstream out;
    Error redirectError{&out};
    a.mutableData();
    a.mutablePixels();
    /* Can't do just MutableImageView2D(a) because the compiler then treats it
       as a function. Can't do MutableImageView2D{a} because that doesn't work
       on old Clang. So it's this mess, then. Sigh. */
    auto b = MutableImageView2D(a);
    static_cast<void>(b);
    auto c = MutableCompressedImageView2D(a);
    static_cast<void>(c);
    /* a.mutablePixels<T>() calls non-templated mutablePixels(), so assume
       there it will blow up correctly as well (can't test because it asserts
       inside arrayCast() due to zero stride) */
    CORRADE_COMPARE(out.str(),
        "Trade::ImageData::mutableData(): the image is not mutable\n"
        "Trade::ImageData::mutablePixels(): the image is not mutable\n"
        "Trade::ImageData: the image is not mutable\n"
        "Trade::ImageData: the image is not mutable\n");
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

void ImageDataTest::pixels1D() {
    ImageData1D image{
        PixelStorage{}
            .setAlignment(1) /** @todo alignment 4 expects 17 bytes. what */
            .setSkip({3, 0, 0}),
        PixelFormat::RGB8Unorm, 2,
        Containers::Array<char>{15}};
    const ImageData1D& cimage = image;

    /* Full test is in ImageTest, this is just a sanity check */

    {
        Containers::StridedArrayView1D<Color3ub> pixels = image.mutablePixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), 2);
        CORRADE_COMPARE(pixels.stride(), 3);
        CORRADE_COMPARE(pixels.data(), image.data() + 3*3);
    } {
        Containers::StridedArrayView1D<const Color3ub> pixels = cimage.pixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), 2);
        CORRADE_COMPARE(pixels.stride(), 3);
        CORRADE_COMPARE(pixels.data(), cimage.data() + 3*3);
    }
}

void ImageDataTest::pixels2D() {
    ImageData2D image{
        PixelStorage{}
            .setAlignment(4)
            .setSkip({3, 2, 0})
            .setRowLength(6),
        PixelFormat::RGB8Unorm, {2, 4},
        Containers::Array<char>{120}};
    const ImageData2D& cimage = image;

    /* Full test is in ImageTest, this is just a sanity check */

    {
        Containers::StridedArrayView2D<Color3ub> pixels = image.mutablePixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), (Containers::StridedArrayView2D<Color3ub>::Size{4, 2}));
        CORRADE_COMPARE(pixels.stride(), (Containers::StridedArrayView2D<Color3ub>::Stride{20, 3}));
        CORRADE_COMPARE(pixels.data(), image.data() + 2*20 + 3*3);
    } {
        Containers::StridedArrayView2D<const Color3ub> pixels = cimage.pixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), (Containers::StridedArrayView2D<const Color3ub>::Size{4, 2}));
        CORRADE_COMPARE(pixels.stride(), (Containers::StridedArrayView2D<const Color3ub>::Stride{20, 3}));
        CORRADE_COMPARE(pixels.data(), cimage.data() + 2*20 + 3*3);
    }
}

void ImageDataTest::pixels3D() {
    ImageData3D image{
        PixelStorage{}
            .setAlignment(4)
            .setSkip({3, 2, 1})
            .setRowLength(6)
            .setImageHeight(7),
        PixelFormat::RGB8Unorm, {2, 4, 3},
        Containers::Array<char>{560}};
    const ImageData3D& cimage = image;

    /* Full test is in ImageTest, this is just a sanity check */

    {
        Containers::StridedArrayView3D<Color3ub> pixels = image.mutablePixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), (Containers::StridedArrayView3D<Color3ub>::Size{3, 4, 2}));
        CORRADE_COMPARE(pixels.stride(), (Containers::StridedArrayView3D<Color3ub>::Stride{140, 20, 3}));
        CORRADE_COMPARE(pixels.data(), image.data() + 140 + 2*20 + 3*3);
    } {
        Containers::StridedArrayView3D<const Color3ub> pixels = cimage.pixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), (Containers::StridedArrayView3D<const Color3ub>::Size{3, 4, 2}));
        CORRADE_COMPARE(pixels.stride(), (Containers::StridedArrayView3D<const Color3ub>::Stride{140, 20, 3}));
        CORRADE_COMPARE(pixels.data(), cimage.data() + 140 + 2*20 + 3*3);
    }
}

void ImageDataTest::pixelsCompressed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::ImageData2D a{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, Containers::Array<char>{8}};

    std::ostringstream out;
    Error redirectError{&out};

    a.pixels();
    /* a.pixels<T>() calls non-templated pixels(), so assume there it will
       blow up correctly as well (can't test because it asserts inside
       arrayCast() due to zero stride) */
    CORRADE_COMPARE(out.str(), "Trade::ImageData::pixels(): the image is compressed\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ImageDataTest)
