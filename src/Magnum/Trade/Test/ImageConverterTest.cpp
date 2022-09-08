/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/StringStl.h> /** @todo remove once Debug is stream-free */
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/StringToFile.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once Debug is stream-free */
#include <Corrade/Utility/Path.h>

#include "Magnum/Trade/Implementation/converterUtilities.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct ImageConverterTest: TestSuite::Tester {
    explicit ImageConverterTest();

    void infoImplementation();
    void infoImplementationError();
};

ImageConverterTest::ImageConverterTest() {
    addTests({&ImageConverterTest::infoImplementation,
              &ImageConverterTest::infoImplementationError});
}

void ImageConverterTest::infoImplementation() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        /* Three 1D images, one with two levels and named, one compressed,
           one just to not have two of everything */
        UnsignedInt doImage1DCount() const override { return 3; }
        UnsignedInt doImage1DLevelCount(UnsignedInt id) override {
            return id == 1 ? 2 : 1;
        }
        Containers::String doImage1DName(UnsignedInt id) override {
            return id == 2 ? "Third 1D image just so there aren't two" : "";
        }
        Containers::Optional<Trade::ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level) override {
            if(id == 0 && level == 0)
                return Trade::ImageData1D{CompressedPixelFormat::Astc10x10RGBAF, 1024, Containers::Array<char>{NoInit, 4096}};
            if(id == 1 && level == 0)
                return Trade::ImageData1D{PixelFormat::RGBA8Snorm, 16, Containers::Array<char>{NoInit, 64}};
            if(id == 1 && level == 1)
                return Trade::ImageData1D{PixelFormat::RGBA8Snorm, 8, Containers::Array<char>{NoInit, 32}};
            if(id == 2 && level == 0)
                return Trade::ImageData1D{PixelFormat::Depth16Unorm, 4, Containers::Array<char>{NoInit, 8}};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        /* Two 2D images, one with three levels and named, the other compressed
           and array */
        UnsignedInt doImage2DCount() const override { return 2; }
        UnsignedInt doImage2DLevelCount(UnsignedInt id) override {
            return id == 0 ? 3 : 1;
        }
        Containers::String doImage2DName(UnsignedInt id) override {
            return id == 0 ? "A very nice mipmapped 2D image" : "";
        }
        Containers::Optional<Trade::ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level) override {
            if(id == 0 && level == 0)
                return Trade::ImageData2D{PixelFormat::RG16F, {256, 128}, Containers::Array<char>{NoInit, 131072}};
            if(id == 0 && level == 1)
                return Trade::ImageData2D{PixelFormat::RG16F, {128, 64}, Containers::Array<char>{NoInit, 32768}};
            if(id == 0 && level == 2)
                return Trade::ImageData2D{PixelFormat::RG16F, {64, 32}, Containers::Array<char>{NoInit, 8192}};
            if(id == 1)
                return Trade::ImageData2D{CompressedPixelFormat::PvrtcRGB2bppUnorm, {4, 8}, Containers::Array<char>{NoInit, 32}, ImageFlag2D::Array};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        /* One 2D cube map array image, one 3D mipmapped & named and two 2D
           array; with one externally owned */
        UnsignedInt doImage3DCount() const override { return 4; }
        UnsignedInt doImage3DLevelCount(UnsignedInt id) override {
            return id == 1 ? 2 : 1;
        }
        Containers::String doImage3DName(UnsignedInt id) override {
            return id == 1 ? "Volume kills!" : "";
        }
        Containers::Optional<Trade::ImageData3D> doImage3D(UnsignedInt id, UnsignedInt level) override {
            if(id == 0 && level == 0)
                return Trade::ImageData3D{PixelFormat::R8Unorm, {16, 16, 12}, Containers::Array<char>{NoInit, 3072}, ImageFlag3D::CubeMap|ImageFlag3D::Array};
            if(id == 1 && level == 0)
                return Trade::ImageData3D{PixelFormat::R8Unorm, {16, 16, 16}, Containers::Array<char>{NoInit, 4096}};
            if(id == 1 && level == 1)
                return Trade::ImageData3D{PixelFormat::R8Unorm, {8, 8, 6}, Containers::Array<char>{NoInit, 2048}};
            if(id == 2 && level == 0)
                return Trade::ImageData3D{CompressedPixelFormat::Bc1RGBSrgb, {4, 1, 1}, Containers::Array<char>{NoInit, 16}, ImageFlag3D::Array};
            if(id == 3 && level == 0)
                return Trade::ImageData3D{PixelFormat::R32F, {1, 4, 1}, Trade::DataFlag::ExternallyOwned|Trade::DataFlag::Mutable, data, ImageFlag3D::Array};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        char data[16];
    } importer;

    bool error = false;
    std::chrono::high_resolution_clock::duration time;
    Containers::Array<Implementation::ImageInfo> infos = Implementation::imageInfo(importer, error, time);
    CORRADE_VERIFY(!error);
    CORRADE_COMPARE(infos.size(), 13);

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printImageInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, infos, nullptr, nullptr, nullptr);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    Implementation::printImageInfo(Debug::Flag::DisableColors, infos, nullptr, nullptr, nullptr);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(TRADE_TEST_DIR, "ImageConverterTestFiles/info.txt"),
        TestSuite::Compare::StringToFile);
}

void ImageConverterTest::infoImplementationError() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 2; }
        Containers::Optional<Trade::ImageData1D> doImage1D(UnsignedInt id, UnsignedInt) override {
            Error{} << "1D image" << id << "error!";
            return {};
        }

        UnsignedInt doImage2DCount() const override { return 2; }
        Containers::Optional<Trade::ImageData2D> doImage2D(UnsignedInt id, UnsignedInt) override {
            Error{} << "2D image" << id << "error!";
            return {};
        }

        UnsignedInt doImage3DCount() const override { return 2; }
        Containers::Optional<Trade::ImageData3D> doImage3D(UnsignedInt id, UnsignedInt) override {
            Error{} << "3D image" << id << "error!";
            return {};
        }
    } importer;

    bool error = false;
    std::chrono::high_resolution_clock::duration time;
    std::ostringstream out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    Containers::Array<Implementation::ImageInfo> infos = Implementation::imageInfo(importer, error, time);
    /* It should return a failure and no output */
    CORRADE_VERIFY(error);
    CORRADE_VERIFY(infos.isEmpty());
    /* But it should not exit after first error */
    CORRADE_COMPARE(out.str(),
        "1D image 0 error!\n"
        "1D image 1 error!\n"
        "2D image 0 error!\n"
        "2D image 1 error!\n"
        "3D image 0 error!\n"
        "3D image 1 error!\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ImageConverterTest)
